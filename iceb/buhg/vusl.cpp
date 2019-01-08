/* $Id: vusl.c,v 5.41 2013/09/26 09:43:44 sasa Exp $ */
/*28.09.2014	17.03.2000	Белых А.И.	vusl.c
Ввод услуг
*/
#include        <ctype.h>
#include        "buhg.h"

extern double	okrcn;  /*Округление цены*/
extern double	okrg1;  /*Округление1 */

void vusl(short dd,short md,short gd, //Дата документа
int tipz,
int podr,
const char *nomdok,
int metka, //0-материал 1-услуга
float pnds)
{
class iceb_tu_str bros("");
SQL_str         row;
char		strsql[2048];
class iceb_tu_str naimus("");
class iceb_tu_str naimm("");
class iceb_tu_str poisk("");
int		kodus,kgru;
int		i;
double		kolih,cena,cenabn;
class iceb_tu_str ei("");
class iceb_tu_str shetu("");
int      	N,K;
double		bb,bb1,ndscen=0.;
double          sumasnds=0.;

struct OPSHET	shetv;
SQLCURSOR curr;

helstr(LINES-1,0,"Enter",gettext("материалы"),
"F10",gettext("выход"),"  ",
gettext(" + перед числом - поиск по числу в наименовании"),NULL);

VVOD VVOD1(2);
if(metka == 1)
 VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код или наименование услуги"));
if(metka == 0)
 VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код или наименование материалла"));

if((i=vvod1(&poisk,32,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
 {
   return;
 }
if(i == ESC)
  return;

GDITE();

kgru=0;
kodus=poisk.ravno_atoi();

if(poisk.ravno()[0] == '\0' || isdigit(poisk.ravno()[0]) == 0)
 {
  clear();

  if(polen(poisk.ravno(),&bros,2,'+') != 0)
    bros.new_plus(poisk.ravno());
  kodus=0;
  if(isdigit(bros.ravno()[0]) != 0)
     kodus=bros.ravno_atoi();
  naimus.new_plus(poisk.ravno());
  if(kodus != 0)
    naimus.new_plus(kodus);

  if(metka == 1)
   if((i=dirusl(&kgru,&kodus,&naimus,1,"\0")) != 0)
     return;
  if(metka == 0)
   if((i=dirmatr1(&kgru,&kodus,&naimus,0,"\0",0,0,1)) != 0)
     return;
 }


ndscen=0.;
if(metka == 1)
 {
  sprintf(strsql,"select cena,ei,naius,nds,shetu from Uslugi \
  where kodus=%d",kodus);

  if(readkey(strsql,&row,&curr) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код услуги"),kodus);
    iceb_t_soob(strsql);
    return;
   }
  shetu.new_plus(row[4]);
 }

if(metka == 0)
 {
  sprintf(strsql,"select cenapr,ei,naimat,nds from Material \
  where kodm=%d",kodus);

  if(readkey(strsql,&row,&curr) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код материалла"),kodus);
    iceb_t_soob(strsql);
    return;
   }
 }

cenabn=cena=atof(row[0]);
ei.new_plus(row[1]);
naimus.new_plus(row[2]);
ndscen=atof(row[3]);

if(cena != 0.  && ndscen == 0.)
 {
  cenabn=cena-cena*pnds/(100.+pnds);
  cenabn=okrug(cenabn,okrcn);
 }

bb=0.;

class VVOD VV(0);

memset(strsql,'\0',sizeof(strsql));
if(metka == 0)
 sprintf(strsql,"%s:%d %s",gettext("Материал"),kodus,naimus.ravno());
if(metka == 1)
 sprintf(strsql,"%s:%d %s",gettext("Услуга"),kodus,naimus.ravno());

VV.VVOD_SPISOK_add_ZAG(strsql);

VV.VVOD_SPISOK_add_MD(gettext("Количество.........."));//0
VV.VVOD_SPISOK_add_MD(gettext("Цена с НДС.........."));//1
VV.VVOD_SPISOK_add_MD(gettext("Цена без НДС........"));//2
VV.VVOD_SPISOK_add_MD(gettext("Единица измерения..."));//3
VV.VVOD_SPISOK_add_MD(gettext("Счёт учёта.........."));//4
VV.VVOD_SPISOK_add_MD(gettext("Сумма без НДС......."));//5
VV.VVOD_SPISOK_add_MD(gettext("Сумма с НДС........."));//6
if(tipz == 1)
 VV.VVOD_SPISOK_add_MD(gettext("Счёт получения......"));//7
if(tipz == 2)
 VV.VVOD_SPISOK_add_MD(gettext("Счёт списания......."));//7

VV.VVOD_SPISOK_add_data(20);//0
VV.VVOD_SPISOK_add_data(20);//1
VV.VVOD_SPISOK_add_data(20);//2
VV.VVOD_SPISOK_add_data(10);//3
VV.VVOD_SPISOK_add_data(20);//4
VV.VVOD_SPISOK_add_data(20);//5
VV.VVOD_SPISOK_add_data(20);//6
VV.VVOD_SPISOK_add_data(20);//7

if(cenabn != 0.)
 {
  VV.data_plus(2,cenabn);
  bb=cenabn+cenabn*pnds/100.;
  bb=okrug(bb,okrg1);
 }
if(bb != 0.)
  VV.data_plus(1,bb);

VV.data_plus(3,ei.ravno());
VV.data_plus(4,shetu.ravno());


N=0;

naz:;


clear(); /*Очистить экран и закрасить фоновым цветом*/


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("реквизиты"),
"F10",gettext("выход"),NULL);


VV.vramka(-1,-1,0);

attron(VV.VVOD_return_cs(iceb_CFS)); 

VV.vprintw(N,stdscr);

for(;;)
 {
  refresh();
  K=getch();

  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));
    VV.vprintw(N,stdscr);

    if(N == 3) //Единица измерения
     {
      naimm.new_plus("");
      if(iceb_tl_ei(2,&ei,&naimm) != 0)
       ei.new_plus("");
      VV.data_plus(N,ei.ravno());
      goto naz;
     }

    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 0) /*Количество*/
     {
      bb1=ATOFM(VV.VVOD_SPISOK_return_data(2))*ATOFM(VV.VVOD_SPISOK_return_data(0));
      bb1=okrug(bb1,okrg1);
      sprintf(strsql,"%.10g",bb1);
      VV.VVOD_SPISOK_zapis_data(5,strsql);
      if(VV.VVOD_SPISOK_return_data(2)[0] != '\0')
       {
        double suma_b_nds=atof(VV.VVOD_SPISOK_return_data(2))*atof(VV.VVOD_SPISOK_return_data(0));
        suma_b_nds=okrug(suma_b_nds,okrg1);
        sprintf(strsql,"%.10g",suma_b_nds);
        VV.VVOD_SPISOK_zapis_data(5,strsql);
       }      
      if(VV.VVOD_SPISOK_return_data(1)[0] != '\0')
       {
        sumasnds=atof(VV.VVOD_SPISOK_return_data(1))*atof(VV.VVOD_SPISOK_return_data(0));
        sumasnds=okrug(sumasnds,okrg1);
        sprintf(strsql,"%.10g",sumasnds);
        VV.VVOD_SPISOK_zapis_data(6,strsql);
       }      
      N++;
      goto naz;
     }

    if(N == 1) /*Ввод цены с ндс*/
     {
        attroff(VV.VVOD_return_cs(iceb_CFM));
        attron(VV.VVOD_return_cs(iceb_CB));

      /*Ввод цены с процентом наценки*/
      if(polen(VV.VVOD_SPISOK_return_data(N),&bb1,2,'/') == 0)
       {
//	bb1=ATOFM(bros);
	cenabn=ATOFM(VV.VVOD_SPISOK_return_data(N));
	cenabn=cenabn+cenabn*bb1/100.;
       }
      else
	cenabn=ATOFM(VV.VVOD_SPISOK_return_data(N));
      if(cenabn != 0.)
       {      
        sumasnds=cenabn*ATOFM(VV.VVOD_SPISOK_return_data(0));
        sumasnds=okrug(sumasnds,okrg1);
        sprintf(strsql,"%.10g",sumasnds);
        VV.VVOD_SPISOK_zapis_data(6,strsql);
       }
       
      sprintf(strsql,"%.10g",cenabn);
      VV.VVOD_SPISOK_zapis_data(1,strsql);

      cenabn=cenabn-cenabn*pnds/(100.+pnds);
      cenabn=okrug(cenabn,okrcn);
      VV.data_plus(2,cenabn);
      bb1=cenabn*ATOFM(VV.VVOD_SPISOK_return_data(0));
      bb1=okrug(bb1,okrg1);
      VV.data_plus(5,bb1);
      N+=2;
      goto naz;
     }
    if(N == 2) /*Ввод цены без НДС*/
     {
        attroff(VV.VVOD_return_cs(iceb_CFM));
        attron(VV.VVOD_return_cs(iceb_CB));
      /*Ввод цены с процентом наценки*/
      if(polen(VV.VVOD_SPISOK_return_data(N),&bb1,2,'/') == 0)
       {
//	bb1=ATOFM(bros);
	cenabn=ATOFM(VV.VVOD_SPISOK_return_data(N));
	cenabn=cenabn+cenabn*bb1/100.;
       }
      else
	cenabn=ATOFM(VV.VVOD_SPISOK_return_data(N));

      cenabn=okrug(cenabn,okrcn);
      VV.data_plus(2,cenabn);

      cenabn=cenabn+cenabn*pnds/100.;
      cenabn=okrug(cenabn,okrg1);

      VV.data_plus(1,cenabn);

      if(cenabn != 0.)
       {      
        sumasnds=cenabn*ATOFM(VV.VVOD_SPISOK_return_data(0));
        sumasnds=okrug(sumasnds,okrg1);
        sprintf(strsql,"%.10g",sumasnds);
        VV.VVOD_SPISOK_zapis_data(6,strsql);
       }

      bb1=ATOFM(VV.VVOD_SPISOK_return_data(2))*ATOFM(VV.VVOD_SPISOK_return_data(0));
      bb1=okrug(bb1,okrg1);
      if(bb1 != 0)
        VV.data_plus(5,bb1);

      goto naz;
            
     }

    if(N == 4 && VV.VVOD_SPISOK_return_data(N)[0] != '\0') /*Ввод Счета*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      attron(VV.VVOD_return_cs(iceb_CB));
      if(prsh1(VV.VVOD_SPISOK_return_data(4),&shetv) != 0)
        goto naz;
     }

    if(N == 5) /*Сумма без ндс*/
     {
      if(VV.VVOD_SPISOK_return_data(0)[0] != '\0')
       {
        //Пересчитываем цену
        cenabn=ATOFM(VV.VVOD_SPISOK_return_data(N))/ATOFM(VV.VVOD_SPISOK_return_data(0));
        cenabn=okrug(cenabn,okrcn);

        sprintf(strsql,"%.10g",cenabn);
        VV.VVOD_SPISOK_zapis_data(2,strsql);

        cenabn=cenabn+cenabn*pnds/100.;
        cenabn=okrug(cenabn,okrg1);

        sprintf(strsql,"%.10g",cenabn);
        VV.VVOD_SPISOK_zapis_data(1,strsql);
       }
      else
       {
        //Пересчитываем количество
        if(VV.VVOD_SPISOK_return_data(2)[0] != '\0')
         {
          cenabn=ATOFM(VV.VVOD_SPISOK_return_data(N))/ATOFM(VV.VVOD_SPISOK_return_data(2));
          sprintf(strsql,"%.10g",cenabn);
          VV.VVOD_SPISOK_zapis_data(0,strsql);

         }
       }     

      goto naz;
     }

    if(N == 6) /*Сумма с ндс*/
     {
      sumasnds=ATOFM(VV.VVOD_SPISOK_return_data(N));
      double suma_b_nds=sumasnds-sumasnds*pnds/(100.+pnds);
      suma_b_nds=okrug(suma_b_nds,okrg1);

      sprintf(strsql,"%.10g",suma_b_nds);
      VV.VVOD_SPISOK_zapis_data(5,strsql);
      
      if(VV.VVOD_SPISOK_return_data(0)[0] != '\0')
       {
        //Пересчитываем цену
        cenabn=suma_b_nds/ATOFM(VV.VVOD_SPISOK_return_data(0));
        cenabn=okrug(cenabn,okrcn);

        sprintf(strsql,"%.10g",cenabn);
        VV.VVOD_SPISOK_zapis_data(2,strsql);

        cenabn=cenabn+cenabn*pnds/100.;
        cenabn=okrug(cenabn,okrg1);

        sprintf(strsql,"%.10g",cenabn);
        VV.VVOD_SPISOK_zapis_data(1,strsql);
      
       }
      else
       {
        //Пересчитываем количество
        if(VV.VVOD_SPISOK_return_data(1)[0] != '\0')
         {
          cenabn=ATOFM(VV.VVOD_SPISOK_return_data(N))/ATOFM(VV.VVOD_SPISOK_return_data(1));
          sprintf(strsql,"%.10g",cenabn);
          VV.VVOD_SPISOK_zapis_data(0,strsql);
         }
       }    
      goto naz;
     }

    VV.vprintw(N,stdscr);
    N++;
   }
  if(K==SDN) /*Стрелка вниз*/
   {
    N++;
   }
  if(K==SUP) /*Стрелка вверх*/
   {
    N--;
   }
  if(N > VV.KLST-1)
   {
    N=0;
   }
  if(N < 0)
   {
    N=VV.KLST-1;
   }
  attron(VV.VVOD_return_cs(iceb_CFS)); 
  VV.vprintw(N,stdscr);

   if(K == ESC || K == FK10)
    {
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
     N=-1;
     break;
    }
   if(K == FK1)
    {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     GDITE();
     iceb_t_pdoc("usl2_1_1_2.txt");
     clear();
     goto naz;
    }
   if(K == FK2 || K == PLU) /*Записываем*/
    {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     attron(VV.VVOD_return_cs(iceb_CFM));
     
     GDITE();
     
     if(VV.VVOD_SPISOK_return_data(0)[0] == '\0' || VV.VVOD_SPISOK_return_data(2)[0] == '\0'
      || VV.VVOD_SPISOK_return_data(3)[0] == '\0' || VV.VVOD_SPISOK_return_data(4)[0] == '\0')
      {
       VVOD SOOB(1);
       SOOB.VVOD_SPISOK_add_MD(gettext("Не введены все обязательные реквизиты !"));
       soobshw(&SOOB,stdscr,-1,-1,0,1);
       goto naz;
      }

     if(prsh1(VV.VVOD_SPISOK_return_data(4),&shetv) != 0)
       goto naz;

    /*Проверяем единицу измерения*/
    sprintf(strsql,"select kod from Edizmer where kod='%s'",
    VV.VVOD_SPISOK_return_data(3));
    if(sql_readkey(&bd,strsql,&row,&curr) != 1)
     {
      VVOD SOOB(1);
      sprintf(strsql,gettext("Не нашли единицу измерения %s !"),VV.VVOD_SPISOK_return_data(3));
      SOOB.VVOD_SPISOK_add_MD(strsql);
      soobshw(&SOOB,stdscr,-1,-1,0,1);
      goto naz;
     }

    cenabn=ATOFM(VV.VVOD_SPISOK_return_data(2));
    cenabn=okrug(cenabn,okrcn);
    if(cenabn == 0.)
     {
      VVOD SOOB(1);
      SOOB.VVOD_SPISOK_add_MD(gettext("Не введена цена без НДС !"));
      soobshw(&SOOB,stdscr,-1,-1,0,1);
      goto naz;
     }

    if(VV.VVOD_SPISOK_return_data(7)[0] != '\0')
     {
      if(prsh1(VV.VVOD_SPISOK_return_data(7),&shetv) != 0)
       goto naz;
     } 
    kolih=ATOFM(VV.VVOD_SPISOK_return_data(0));
    ei.new_plus(VV.VVOD_SPISOK_return_data(3));
    shetu.new_plus(VV.VVOD_SPISOK_return_data(4));
    if(zapuvdok(tipz,dd,md,gd,nomdok,metka,kodus,kolih,cenabn,ei.ravno(),shetu.ravno(),podr,VV.VVOD_SPISOK_return_data(7),"") != 0)
     goto naz;
    break;
   }
  if(K == FK3) /*Смотрим реквизиты*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS)); 
    class iceb_tu_str kod("");
    VVOD MENU(3);
    MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите  нужное и нажмите Enter"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка единиц измерения"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    
    int kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 2 :
      case -1:
       goto naz;


      case 0 :
        if(vibrek("Plansh",&kod) == 0)
         {
          VV.data_plus(4,kod.ravno());
         }
        goto naz;

      case 1 :
        if(vibrek("Edizmer",&kod) == 0)
         {
          VV.data_plus(3,kod.ravno());
         }
        goto naz;
      }
     goto naz;
   }
 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();


}
