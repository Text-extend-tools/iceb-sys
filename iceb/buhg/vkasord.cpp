/* $Id: vkasord.c,v 5.52 2013/09/26 09:43:43 sasa Exp $ */
/*25.03.2013	20.09.2000	Белых А.И.	vkasord.c
Ввод и корректировка кассовых ордеров на приход и расход
Возвращаем 0 если записали
           1-нет
*/
#include	"buhg.h"

void vkasord_kom(short d,short m,short g,int tipz,const char *kassa,const char *nomd,const char *kodop,class iceb_tu_str *osnov);
int vkasord_ved(short dd,short md,short gd,const char *nomvs,const char *nomvn,const char*,const char*);

extern int	regim;

int vkasord(int tipz, //1-приход 2-расход
int mkor, //0-ввод новой записи 1-корректировка
/*********Возвращяемые значения**************/
class iceb_tu_str *kassa,class iceb_tu_str *nomd,short *dd,short *md,short *gd)
{
SQL_str		row;
SQLCURSOR curr;
char		strsql[1024];
short           K;
int             N;
short           PR;
class iceb_tu_str kod("");
class iceb_tu_str naim("");
class iceb_tu_str naimop("");
class iceb_tu_str naimkas("");
class iceb_tu_str naimshet("");
class iceb_tu_str nomer_ved("");

int		kom1;
int		poz,komv;
class iceb_tu_str tabl("");
short		d,m,g;
short		dz,mz,gz;
class iceb_tu_str nomdz("");
class iceb_tu_str kassaz("");
struct OPSHET	shetv;
//int 		i1=0;
class iceb_tu_str nomer_bl("");
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
VVOD MENU(3);

K=N=PR=0;
VVOD VV(0);

sozvkasord(&VV,tipz);

if( tipz == 1)
 {
  VV.VVOD_SPISOK_add_MD(gettext("Касса............")); //0
  VV.VVOD_SPISOK_add_MD(gettext("Дата.............")); //1
  VV.VVOD_SPISOK_add_MD(gettext("Номер документа..")); //2
  VV.VVOD_SPISOK_add_MD(gettext("Код операции.....")); //3
  VV.VVOD_SPISOK_add_MD(gettext("Счёт.............")); //4
  VV.VVOD_SPISOK_add_MD(gettext("Основание........")); //5
  VV.VVOD_SPISOK_add_MD(gettext("Дополнение.......")); //6
  VV.VVOD_SPISOK_add_MD(gettext("Фамилия..........")); //7
  VV.VVOD_SPISOK_add_MD(gettext("Номер бланка.....")); //8

 }

if(tipz == 2)
 {
  VV.VVOD_SPISOK_add_MD(gettext("Касса............")); //0
  VV.VVOD_SPISOK_add_MD(gettext("Дата.............")); //1
  VV.VVOD_SPISOK_add_MD(gettext("Номер документа..")); //2
  VV.VVOD_SPISOK_add_MD(gettext("Код операции.....")); //3
  VV.VVOD_SPISOK_add_MD(gettext("Счёт.............")); //4
  VV.VVOD_SPISOK_add_MD(gettext("Основание........")); //5
  VV.VVOD_SPISOK_add_MD(gettext("Дополнение.......")); //6
  VV.VVOD_SPISOK_add_MD(gettext("Фамилия..........")); //7
  VV.VVOD_SPISOK_add_MD(gettext("Номер бланка.....")); //8
  VV.VVOD_SPISOK_add_MD(gettext("Документ.........")); //9
  VV.VVOD_SPISOK_add_MD(gettext("Номер ведомости..")); //10

 }


if(mkor == 0) //Ввод новой записи
 {
  if(regim != 0)
   {
    sprintf(strsql,"%d",regim);
    VV.VVOD_SPISOK_zapis_data(0,strsql);
   }

  if(regim == 0)
   {
    /*Если в списке касс только одна касса, то код ее записать сразу*/
    sprintf(strsql,"select kod,naik from Kas");
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
     {
      VV.VVOD_SPISOK_zapis_data(0,row[0]);
     }
   }

  sprintf(strsql,"%d.%d.%d",dt,mt,gt);
  VV.VVOD_SPISOK_zapis_data(1,strsql);
  if(VV.VVOD_SPISOK_return_data(0)[0] != '\0')
    N++;
  /*Если в списке операций только одна, то код ее записать сразу*/
  if(tipz == 1)
    sprintf(strsql,"select kod,naik,shetkor from Kasop1");
  if(tipz == 2)
    sprintf(strsql,"select kod,naik,shetkor from Kasop2");
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
   {
    VV.VVOD_SPISOK_zapis_data(3,row[0]);
    VV.VVOD_SPISOK_zapis_data(4,row[2]);
   }

 } 
else /*Корректировка существующей шапки*/
 {
  sprintf(strsql,"select * from Kasord where nomd='%s' and kassa=%d and tp=%d and god=%d",
  nomd->ravno(),kassa->ravno_atoi(),tipz,*gd);
  int kst=0;
  if((kst=readkey(strsql,&row,&curr)) != 1)
   {
    printw("\nНе найдена шапка кассового ордера !!! kst=%d\n",kst);
    printw("%s\n",strsql);
    OSTANOV();
    return(1);
   }

  VV.data_plus(0,row[0]);
  sprintf(strsql,"%d.%d.%d",*dd,*md,*gd);
  VV.data_plus(1,strsql);
  VV.data_plus(2,nomd->ravno());
  VV.data_plus(3,row[5]);
  VV.data_plus(4,row[4]);
  VV.data_plus(5,row[8]);
  VV.data_plus(6,row[9]);
  VV.data_plus(7,row[10]);
  VV.data_plus(8,row[15]);
  nomer_bl.new_plus(row[15]);

  if(tipz == 2)
   {
    VV.data_plus(9,row[11]);
    VV.data_plus(10,row[16]);
    nomer_ved.new_plus(row[16]);
   }
 }

kassaz.new_plus(VV.data_ravno(0));

rsdat(&dz,&mz,&gz,VV.VVOD_SPISOK_return_data(1),1);
nomdz.new_plus(VV.VVOD_SPISOK_return_data(2));

/*Читаем наименование кассы*/

naimkas.new_plus("");
if(VV.VVOD_SPISOK_return_data(0)[0] != '\0')
 {
  sprintf(strsql,"select naik from Kas where kod='%s'",
  VV.VVOD_SPISOK_return_data(0));
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найдена касса"),VV.VVOD_SPISOK_return_data(0));
    iceb_t_soob(strsql);
    goto nazad;
   }
  naimkas.new_plus(row[0]);
 }

naimop.new_plus("");
if(VV.VVOD_SPISOK_return_data(3)[0] != '\0')
 {
  if(tipz == 1)
    sprintf(strsql,"select naik from Kasop1 where kod='%s'",
    VV.VVOD_SPISOK_return_data(3));
  if(tipz == 2)
    sprintf(strsql,"select naik from Kasop2 where kod='%s'",
    VV.VVOD_SPISOK_return_data(3));
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    sprintf(strsql,gettext("Не найден код операции %s !"),VV.VVOD_SPISOK_return_data(3));
    iceb_t_soob(strsql);
   }
  else
    naimop.new_plus(row[0]);
 }

if(VV.VVOD_SPISOK_return_data(4)[0] != '\0')
 {
  sprintf(strsql,"select nais from Plansh where ns='%s'",VV.VVOD_SPISOK_return_data(4));
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    sprintf(strsql,gettext("Не найден счёт %s в плане счетов !"),VV.VVOD_SPISOK_return_data(4));
    iceb_t_soob(strsql);
   }
  else
    naimshet.new_plus(row[0]);
 }



nazad:;
clear();

attron(A_BLINK);
if(tipz == 1)
  printw(gettext("ПРИХОД."));
if(tipz == 2)
  printw(gettext("РАСХОД."));
attroff(A_BLINK);


if(tipz == 1)
  helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("реквизиты"),
"F4",gettext("номер докум."),
"F5",gettext("копировать"),
"F6",gettext("копировать"),
"F10",gettext("выход"),NULL);
if(tipz == 2)
  helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("реквизиты"),
"F4",gettext("номер докум."),
"F5",gettext("копировать"),
"F6",gettext("копировать"),
"F7",gettext("ном.вед."),
"F10",gettext("выход"),NULL);


VV.vramka(4,0,0);
int PY=4;
PR=0;

for(;;)
 {

    
  attron(VV.VVOD_return_cs(iceb_CFS)); 
  VV.vprintw(N,stdscr);
  if(naimkas.ravno()[0] != 0)
   {
    move(PY-3,0);
    printw("%s:%.*s",gettext("Касса"),iceb_tu_kolbait(70,naimkas.ravno()),naimkas.ravno());
   }
  if(naimshet.ravno()[0] != 0)
   {
    move(PY-2,0);
    printw("%s:%.*s",gettext("Счёт"),iceb_tu_kolbait(70,naimshet.ravno()),naimshet.ravno());
   }
  if(naimop.ravno()[0] != 0)
   {
    move(PY-1,0);
    printw("%s:%.*s",gettext("Операция"),iceb_tu_kolbait(70,naimop.ravno()),naimop.ravno());
   }

  K=getch();

  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == KEY_RESIZE)
   goto nazad;
  refresh();

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));
    VV.vprintw_clear(N,stdscr);

    if(N == 3) /*Код операции*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFV));
/********************
      if(tipz == 1)
        strcpy(strsql,"Kasop1");
      if(tipz == 2)
        strcpy(strsql,"Kasop2");
************************/
      class iceb_tu_str naik("");
      if(dirkasop(tipz,2,&kod,&naik) != 0)
       {
        VV.data_plus(3,"");
        goto nazad;
       }
/*************
      if((i1=vvodkod(strsql,&kod,&naik,-1,-1,0)) == 0)
       {
        goto nazad;
       }
***************/
      VV.VVOD_SPISOK_zapis_data(3,kod.ravno());
      naimop.new_plus(naik.ravno());

      if(tipz == 1)
        sprintf(strsql,"select naik,shetkor from Kasop1 where kod='%s'",kod.ravno());
      if(tipz == 2)
        sprintf(strsql,"select naik,shetkor from Kasop2 where kod='%s'",kod.ravno());
      if(sql_readkey(&bd,strsql,&row,&curr) != 1)
       {
        sprintf(strsql,gettext("Не найден код операции %s !"),kod.ravno());
        iceb_t_soob(strsql);
        goto nazad;
       }

      VV.data_plus(5,row[0]);
      VV.data_plus(4,row[1]);
      sprintf(strsql,"select nais from Plansh where ns='%s'",VV.VVOD_SPISOK_return_data(4));
      if(sql_readkey(&bd,strsql,&row,&curr) != 1)
       {
        goto nazad;
       }
      naimshet.new_plus(row[0]);
      N++;
      goto nazad;

     }

    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 0 && VV.VVOD_SPISOK_return_data(N)[0] != '\0') //Касса
     {
      naimkas.new_plus("");
      sprintf(strsql,"select naik from Kas where kod='%s'",
      VV.VVOD_SPISOK_return_data(N));
      if(sql_readkey(&bd,strsql,&row,&curr) != 1)
       {
        sprintf(strsql,"%s %s !",gettext("Не найдена касса"),VV.VVOD_SPISOK_return_data(N));
        iceb_t_soob(strsql);

        VV.data_plus(N,"");
        goto nazad;
       }
      naimkas.new_plus(row[0]);      
     }

    if(N == 1 && VV.VVOD_SPISOK_return_data(N)[0] != '\0') //Дата документа
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      if(rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(N),0) != 0)
       {
        iceb_t_soob(gettext("Не верно введена дата !"));
	goto nazad;
       }
      if(pbkas(m,g) != 0)
       goto nazad;

      /*Автоматическая установка номера накладной*/

      if(mkor == 0 && VV.VVOD_SPISOK_return_data(2)[0] == '\0' && VV.VVOD_SPISOK_return_data(1)[0] != '\0' && \
      VV.VVOD_SPISOK_return_data(0)[0] != '\0')
       {
        class iceb_tu_str nomer_dok("");
        iceb_t_nomnak(g,VV.VVOD_SPISOK_return_data(0),&nomer_dok,(int)tipz,(int)2,1);
        VV.data_plus(2,nomer_dok.ravno());
        N++;
        goto nazad;
       }        
     }    

    if(N == 2 && VV.VVOD_SPISOK_return_data(N)[0] != '\0') //Номер документа
     {
      //Проверяем номер документа
      if(VV.VVOD_SPISOK_return_data(0)[0] != '\0')
      if(SRAV(nomdz.ravno(),VV.VVOD_SPISOK_return_data(2),0) != 0)
       {
        sprintf(strsql,"select nomd from Kasord where nomd='%s' and \
kassa=%s and tp=%d and god=%d",VV.VVOD_SPISOK_return_data(2),VV.VVOD_SPISOK_return_data(0),tipz,g);
        if(sql_readkey(&bd,strsql,&row,&curr) == 1)
         {
          sprintf(strsql,gettext("С номером %s документ уже есть !"),VV.VVOD_SPISOK_return_data(2));
          iceb_t_soob(strsql);
          goto nazad;
         }
       }
     }

    if(N == 4 && VV.VVOD_SPISOK_return_data(N)[0] != '\0') //Счёт
     {
      naimshet.new_plus("");
      if(prsh1(VV.VVOD_SPISOK_return_data(N),&shetv) != 0)
	goto nazad;

      sprintf(strsql,"select nais from Plansh where ns='%s'",VV.VVOD_SPISOK_return_data(N));
      if(sql_readkey(&bd,strsql,&row,&curr) != 1)
       {
        sprintf(strsql,gettext("Не найден счёт %s в плане счетов !"),VV.VVOD_SPISOK_return_data(N));
        iceb_t_soob(strsql);
        VV.data_plus(N,"");
        goto nazad;
       }
      naimshet.new_plus(row[0]);
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

  if(K == ESC || K == FK10)
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CB));
    return(1);
   }

  if(K == FK1) /*Помощь*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CB));
    GDITE();
    if(tipz == 1)
     sprintf(strsql,"%s","kas2_2.txt");
    if(tipz == 2)
     sprintf(strsql,"%s","kas2_1.txt");
    iceb_t_pdoc(strsql);
    goto nazad;
   }

  if(K == PLU || K == FK2) /*Продолжать*/
   {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     attron(VV.VVOD_return_cs(iceb_CFM));
     if(VV.VVOD_SPISOK_return_data(0)[0] == '\0' || VV.VVOD_SPISOK_return_data(1)[0] == '\0' || VV.VVOD_SPISOK_return_data(2)[0] == '\0' ||
	VV.VVOD_SPISOK_return_data(3)[0] == '\0' || VV.VVOD_SPISOK_return_data(4)[0] == '\0')
      {
       attroff(VV.VVOD_return_cs(iceb_CFM));
       iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
       goto nazad;
      }
     if(prsh1(VV.VVOD_SPISOK_return_data(4),&shetv) != 0)
      goto nazad;
     break;
    }
  if(K == FK3) /*Посмотреть реквизиты*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS)); 
    MENU.VVOD_delete();

    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка касс"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 3:
      case -1:
       goto nazad;


      case 0:

        clear();
        kod.new_plus("");
        naim.new_plus("");
        poz=komv=0;
        if(tipz == 1)
         tabl.new_plus("Kasop1");        
        if(tipz == 2)
         tabl.new_plus("Kasop2");        
        if(dirmat1(tabl.ravno(),&kod,&naim,LINES-5,0,3,1,&poz,&komv,"","\0") == 0)
         {
          VV.VVOD_SPISOK_zapis_data(3,kod.ravno());
          naimop.new_plus(naim.ravno());
          if(VV.VVOD_SPISOK_return_data(4)[0] == '\0')
           {
            if(tipz == 1)
              sprintf(strsql,"select naik,shetkor from Kasop1 where kod='%s'",kod.ravno());
            if(tipz == 2)
              sprintf(strsql,"select naik,shetkor from Kasop2 where kod='%s'",
              kod.ravno());
            if(sql_readkey(&bd,strsql,&row,&curr) != 1)
             {
              sprintf(strsql,gettext("Не найден код операции %s !"),kod.ravno());
              iceb_t_soob(strsql);
              goto nazad;
             }

            VV.data_plus(4,row[1]);
            VV.data_plus(5,row[0]);
            sprintf(strsql,"select nais from Plansh where ns='%s'",VV.VVOD_SPISOK_return_data(4));
            if(sql_readkey(&bd,strsql,&row,&curr) != 1)
             {
              goto nazad;
             }
            naimshet.new_plus(row[0]);
           }
         }
        goto nazad;

      case 1:

        clear();
        naim.new_plus("");
        if(dirkas(1,&kod,&naim) == 0)
         {
          VV.VVOD_SPISOK_zapis_data(0,kod.ravno());
          naimkas.new_plus(naim.ravno());
         }
        goto nazad;

      case 2:

        clear();

        kod.new_plus("");
        if(dirshet(&kod,&naim,1) == 0)
         {
          VV.VVOD_SPISOK_zapis_data(4,kod.ravno());
          naimshet.new_plus(naim.ravno());
         }
        goto nazad;

     }
   }
  if(K == FK4 && VV.data_ravno(1)[0] != '\0') /*Получить свободный номер документа*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS)); 
    class iceb_tu_str nomer_dok("");
    iceb_t_nomnak(VV.data_ravno_god(1),VV.VVOD_SPISOK_return_data(0),&nomer_dok,tipz,2,1);
    VV.data_plus(2,nomer_dok.ravno());
    goto nazad;
   }
  if(K == FK5) //Скопировать реквизит основание в реквизит фамилия
   {
    VV.VVOD_SPISOK_zapis_data(7,VV.VVOD_SPISOK_return_data(5));
    goto nazad;
   }

  if(K == FK6) //Скопировать комментарии проводок в основание
   {

    rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(1),1);
    class iceb_tu_str osnovanie;
    vkasord_kom(d,m,g,tipz,VV.data_ravno(0),VV.data_ravno(2),VV.data_ravno(3),&osnovanie);
    VV.data_plus(5,osnovanie.ravno());

    goto nazad;

   }

  if(K == FK7) /*Получить номер ведомости*/
   {
    if(tipz == 1)
     goto nazad;    
    VV.data_plus(10,nomved_pol_nom());
    goto nazad;

   }
 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
GDITE();
/*********************************************/
/* Записываем шапку документа                */
/*********************************************/
/*
printw("\nVV.VVOD_SPISOK_return_data(0)=%s\n",VV.VVOD_SPISOK_return_data(0));
OSTANOV();
*/

/*Проверяем номер бланка*/
if(VV.data_ravno(8)[0] != '\0')
 if(SRAV(nomer_bl.ravno(),VV.data_ravno(8),0) != 0)
  {
   sprintf(strsql,"select nomd from Kasord where nb='%s'",VV.data_ravno(8));
   if(readkey(strsql,&row,&curr) > 0)
    {
     sprintf(strsql,gettext("Номер бланка %s уже есть у документа %s !"),VV.data_ravno(8),row[0]);
     
     iceb_t_soob(strsql);
     goto nazad;
    } 
  }

rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(1),1);


if(dz != 0)
 if(m != mz || g != gz)
  if(iceb_t_pvkdd(7,dz,mz,gz,m,g,kassa->ravno_atoi(),nomd->ravno(),tipz) != 0)
   return(1);
   
class iceb_tu_str dokum("");
if(tipz == 2)
 dokum.new_plus(VV.VVOD_SPISOK_return_data(9));


class iceb_tu_str osnov("");
osnov.new_plus(VV.VVOD_SPISOK_return_data(5));

class iceb_tu_str dopol("");
dopol.new_plus(VV.VVOD_SPISOK_return_data(6));


int kodv=0;

if(tipz == 2)
 {
  
  if(vkasord_ved(d,m,g,nomer_ved.ravno(),VV.data_ravno(10),VV.data_ravno(2),VV.data_ravno_filtr(5)) != 0)
   goto nazad;

  nomer_ved.new_plus(VV.data_ravno_filtr(10));
 }
 
if((kodv=zaphkor(mkor,
VV.data_ravno(0),
tipz,d,m,g,VV.data_ravno(2),
VV.data_ravno(4),VV.data_ravno(3),
osnov.ravno_filtr(),dopol.ravno_filtr(),VV.data_ravno(7),
dokum.ravno_filtr(),kassaz.ravno(),nomdz.ravno_filtr(),dz,mz,gz,VV.data_ravno(8),nomer_ved.ravno_filtr())) != 0)
 {
  if(kodv == 1)
   {
    sprintf(strsql,gettext("С номером %s документ уже есть !"),VV.VVOD_SPISOK_return_data(2));
    iceb_t_soob(strsql);
   }
  goto nazad;
 }

kassa->new_plus(VV.data_ravno(0));
rsdat(dd,md,gd,VV.VVOD_SPISOK_return_data(1),1);
nomd->new_plus(VV.data_ravno(2));

return(0);

}
/****************************/

void	sozvkasord(VVOD *VV,int tipz)
{

if( tipz == 1)
 {
  VV->VVOD_SPISOK_add_data(10); //0
  VV->VVOD_SPISOK_add_data(11); //1
  VV->VVOD_SPISOK_add_data(20); //2
  VV->VVOD_SPISOK_add_data(20); //3
  VV->VVOD_SPISOK_add_data(20); //4
  VV->VVOD_SPISOK_add_data(250); //5
  VV->VVOD_SPISOK_add_data(80); //6
  VV->VVOD_SPISOK_add_data(60); //7
  VV->VVOD_SPISOK_add_data(20); //8
 }

if(tipz == 2)
 {
  VV->VVOD_SPISOK_add_data(10); //0
  VV->VVOD_SPISOK_add_data(11); //1
  VV->VVOD_SPISOK_add_data(20); //2
  VV->VVOD_SPISOK_add_data(20); //3
  VV->VVOD_SPISOK_add_data(20); //4
  VV->VVOD_SPISOK_add_data(250); //5
  VV->VVOD_SPISOK_add_data(80); //6
  VV->VVOD_SPISOK_add_data(60); //7
  VV->VVOD_SPISOK_add_data(20); //8
  VV->VVOD_SPISOK_add_data(255); //9
  VV->VVOD_SPISOK_add_data(20); //10
 }

}
/************************************************/
/*Копирование комментариев из проводок в основание*/
/*************************************************/
void vkasord_kom(short d,short m,short g,
int tipz,
const char *kassa,const char *nomd,const char *kodop,class iceb_tu_str *osnov)
{
int kolstr=0;
char strsql[1024];
class iceb_tu_str metkasys=(ICEB_MP_KASA);


osnov->new_plus("");

sprintf(strsql,"select deb,komen from Prov where kto='%s' and pod=%s and nomd='%s' and oper='%s' and \
datd='%04d-%02d-%02d' and tz=%d and deb <> 0. order by datp asc",
metkasys.ravno(),kassa,nomd,kodop,g,m,d,tipz);

SQLCURSOR cur;  
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной проводки!"));
  return;
 }
SQL_str row;
while(cur.read_cursor(&row) != 0)
 {
  if(row[0][0] != '\0')
   {
    osnov->plus(" ");
    osnov->plus(row[1]);
    osnov->plus(":");
    osnov->plus(row[0]);
   }
 }
}

/*******************************/
/*Ведомость*/
/***************************/
int vkasord_ved(short dd,short md,short gd,const char *nomvs,const char *nomvn,const char *nomord,const char *koment)
{
char strsql[512];
if(SRAV(nomvs,nomvn,0) != 0)
 {
  if(nomvn[0] != '\0')
   {
    sprintf(strsql,"select nomv from Kasnomved where god=%d and nomv='%s'",gd,nomvn);
    if(readkey(strsql) > 0)
     {
        sprintf(strsql,gettext("Номер ведомости %s уже есть!"),nomvn);
        iceb_t_soob(strsql);
        return(1);
     }
   }


 }
if(nomvs[0] != '\0')
 {
   /*Удаляем старую запись*/
   sprintf(strsql,"delete from Kasnomved where god=%d and nomv='%s'",gd,nomvs);
   sql_zapis(strsql,0,0);
 }
if(nomvn[0] != '\0')
 {
  sprintf(strsql,"insert into Kasnomved values('%04d-%02d-%02d',%d,'%s',0.,'%s',%d,%ld,'%s')",
  gd,md,dd,gd,nomvn,koment,iceb_t_getuid(),time(NULL),nomord);

  return(sql_zapis(strsql,0,0));
 }
return(0);
}


