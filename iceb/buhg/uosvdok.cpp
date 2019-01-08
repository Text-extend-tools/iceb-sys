/* $Id: uosvdok.c,v 5.59 2013/05/17 14:56:14 sasa Exp $ */
/*18.10.2012   07.07.1996      Белых А.И.      uosvdok.c
Ввод документов в учете основных средств
Если вернули 0- шапка записана
	     1- не записана
*/
#include        <errno.h>
#include        "buhg.h"
#include        "uos.h"

short           provnd(short,short,const char*);
int		provdok(short,short,short,const char*,short);
void  uosinp(short,short,short,short,short,const char*);

extern class UOS uos;
extern short           mdd;  /*0-обычный документ 1-двойной*/
extern char	*bnds1,*bnds2,*bnds3;
 
int uosvdok(VVOD *VV,int mr)//*0-ввод нового документа 1-корректировка*/
{
short           mvnp=0; /*0-внешняя 1 -внутреняя 2-изменение стоимости*/
int             N,K;
short           d,m,g;
short           dz,mz,gz; //запомненая дата документа взятого на корректировку
int		kom1;
class iceb_tu_str kod("");
class iceb_tu_str nai("");
SQL_str         row1;
char		strsql[2048];
time_t		vrem;
class iceb_tu_str nomdokz("");
class iceb_tu_str nomdokz1("");
class iceb_tu_str kontrz("");		
char		bros[512];
int		uospodz,uoskodolz;
short 		prov=0;
int		i1;
short god_nd=0; /*Год нумерации документов*/
class iceb_tu_str kod_v("");
class iceb_tu_str naim_v("");
short dt,mt,gt;
SQLCURSOR curr;
VVOD MENU(3);
float pnds=0.;
poltekdat(&dt,&mt,&gt);
if(mr == 0)
 pnds=iceb_t_pnds();
else
 pnds=iceb_t_pnds(VV->data_ravno(0));
 
/*
printw("\nuosvdok-%d\n",mr);
refresh();
*/
N=0;

rsdat(&d,&m,&g,VV->VVOD_SPISOK_return_data(0),1);
god_nd=g;
dz=d; mz=m; gz=g;
nomdokz.new_plus(uos.nomdok.ravno());
nomdokz1.new_plus(uos.nomdokp.ravno());
kontrz.new_plus(uos.kontr.ravno());
uospodz=uos.pod;
uoskodolz=uos.kodol;

uos.nds=0;
if(uos.tipz == 1)
  sprintf(strsql,"select vido from Uospri where kod='%s'",uos.kprr.ravno());
if(uos.tipz == 2)
  sprintf(strsql,"select vido from Uosras where kod='%s'",uos.kprr.ravno());
if(readkey(strsql,&row1,&curr) == 1)
 mvnp=atoi(row1[0]);  
if(mr == 0 && mdd == 0) 
 {
  if(bnds1 != NULL)
   if(bnds1[0] != '\0')
    if(pole1(bnds1,uos.kprr.ravno(),',',0,&i1) == 0 || SRAV(bnds1,uos.kprr.ravno(),0) == 0)
       uos.nds=1;
  if(bnds2 != NULL)
   if(bnds2[0] != '\0')
    if(pole1(bnds2,uos.kprr.ravno(),',',0,&i1) == 0 || SRAV(bnds2,uos.kprr.ravno(),0) == 0)
       uos.nds=2;
  if(bnds3 != NULL)
   if(bnds3[0] != '\0')
    if(pole1(bnds3,uos.kprr.ravno(),',',0,&i1) == 0 || SRAV(bnds3,uos.kprr.ravno(),0) == 0)
       uos.nds=3;

  if(uos.nds == 0)
   {
    sprintf(strsql,"%.f%% %s",pnds,gettext("НДС"));
    VV->data_plus(13,strsql);
   }
  if(uos.nds == 1)
    VV->data_plus(13,gettext("0% НДС реализация на територии Украины"));
  if(uos.nds == 2)
    VV->data_plus(13,gettext("0% НДС экспорт"));
  if(uos.nds == 3)
    VV->data_plus(13,gettext("Освобождение от НДС"));
 }
if(uos.tipz == 1)
 {
  if(mdd == 1)
   {
  VV->VVOD_SPISOK_add_MD(gettext("Дата..........................."));//0
  VV->VVOD_SPISOK_add_MD(gettext("Номер документа получателя....."));//1
  VV->VVOD_SPISOK_add_MD(gettext("Подразделение получатель......."));//2
  VV->VVOD_SPISOK_add_MD(gettext("Мат.-ответственный получатель.."));//3
  VV->VVOD_SPISOK_add_MD(gettext("Номер документа поставщика....."));//4
  VV->VVOD_SPISOK_add_MD(gettext("Подразделение поставщик........"));//5
  VV->VVOD_SPISOK_add_MD(gettext("Мат.-ответственный поставщик..."));//6
  VV->VVOD_SPISOK_add_MD(gettext("Основание......................"));//7
   }
  else
   {
  VV->VVOD_SPISOK_add_MD(gettext("Дата............................"));//0
  VV->VVOD_SPISOK_add_MD(gettext("Номер документа получателя......"));//1
  VV->VVOD_SPISOK_add_MD(gettext("Подразделение получатель........"));//2
  VV->VVOD_SPISOK_add_MD(gettext("Мат.-ответственный получатель..."));//3
  VV->VVOD_SPISOK_add_MD(gettext("Код контрагента поставщика......"));//4
  VV->VVOD_SPISOK_add_MD(gettext("Номер документа поставщика......"));//5
  VV->VVOD_SPISOK_add_MD(gettext("Основание......................."));//6

  VV->VVOD_SPISOK_add_MD(gettext("Доверенность...................."));//7
  VV->VVOD_SPISOK_add_MD(gettext("Дата выдачи доверенности........"));//8
  VV->VVOD_SPISOK_add_MD(gettext("Через кого......................"));//9
  VV->VVOD_SPISOK_add_MD(gettext("Номер налоговой накладной......."));//10
  VV->VVOD_SPISOK_add_MD(gettext("Дата получения нал.накладной...."));//11
  VV->VVOD_SPISOK_add_MD(gettext("Дата оплаты....................."));//12
  VV->VVOD_SPISOK_add_MD(gettext("НДС............................."));//13
  VV->VVOD_SPISOK_add_MD(gettext("Форма оплаты...................."));//14
  VV->VVOD_SPISOK_add_MD(gettext("Дата виписки нал. накл-ной......"));//15
  VV->VVOD_SPISOK_add_MD(gettext("Вид договора...................."));//16
   }
 }

if(uos.tipz == 2)
 {
  if(mdd == 1)
   {
  VV->VVOD_SPISOK_add_MD(gettext("Дата..........................."));//0
  VV->VVOD_SPISOK_add_MD(gettext("Номер документа поставщика....."));//1
  VV->VVOD_SPISOK_add_MD(gettext("Подразделение поставщик........"));//2
  VV->VVOD_SPISOK_add_MD(gettext("Мат.-ответственный поставшик..."));//3
  VV->VVOD_SPISOK_add_MD(gettext("Номер документа получателя....."));//4
  VV->VVOD_SPISOK_add_MD(gettext("Подразделение получатель......."));//5
  VV->VVOD_SPISOK_add_MD(gettext("Мат.-ответственный получатель.."));//6
  VV->VVOD_SPISOK_add_MD(gettext("Основание......................"));//7

   }
  else
   {
  VV->VVOD_SPISOK_add_MD(gettext("Дата.........................."));//0
  VV->VVOD_SPISOK_add_MD(gettext("Номер документа поставщика...."));//1
  VV->VVOD_SPISOK_add_MD(gettext("Подразделение поставщик......."));//2
  VV->VVOD_SPISOK_add_MD(gettext("Мат.-ответственный поставщик.."));//3
  VV->VVOD_SPISOK_add_MD(gettext("Код контрагента получателя...."));//4
  VV->VVOD_SPISOK_add_MD(gettext("Основание....................."));//5

  VV->VVOD_SPISOK_add_MD(gettext("Доверенность.................."));//6
  VV->VVOD_SPISOK_add_MD(gettext("Дата выдачи доверенности......"));//7
  VV->VVOD_SPISOK_add_MD(gettext("Через кого...................."));//8
  VV->VVOD_SPISOK_add_MD(gettext("Форма оплаты.................."));//9
  VV->VVOD_SPISOK_add_MD(gettext("Номер налоговой накладной....."));//10
  VV->VVOD_SPISOK_add_MD(gettext("Дата выдачи нал.накладной....."));//11
  VV->VVOD_SPISOK_add_MD(gettext("Дата оплаты..................."));//12
  VV->VVOD_SPISOK_add_MD(gettext("НДС..........................."));//13
  VV->VVOD_SPISOK_add_MD(gettext("Вид договора.................."));//14
   }
 }

nazad:;

clear();
if(uos.kontr.ravno()[0] != '\0')
 {
  polen(uos.kontr.ravno(),bros,sizeof(bros),1,'-');
  if(SRAV(bros,"00",0) != 0)
   strcpy(bros,uos.kontr.ravno());
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",bros);
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    attroff(VV->VVOD_return_cs(iceb_CFV));

    sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),bros);
    iceb_t_soob(strsql);

    uos.kontr.new_plus("");
    uos.naimo.new_plus("");
   }
  else
    uos.naimo.new_plus(row1[0]);
 }

move(0,0);
printw("%s: %s %.30s ",gettext("Операция"),uos.kprr.ravno(),uos.naimpr.ravno());

attron(A_BLINK); /*Включение мерцания*/
if(uos.tipz == 1)
  printw(gettext("Приход"));
if(uos.tipz == 2)
  printw(gettext("Расход"));
if(mdd == 1)
  printw(" %s",gettext("(двойной документ)"));
if(mvnp == 2)
  printw(" %s",gettext("(изменение стоимости)"));
attroff(A_BLINK); /*Выключение мерцания*/


int PYz=4;
int PXz=0; /*Координаты вывода меню*/
if(mdd == 1)
 PYz=6;
   
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("реквизиты"),
"F10",gettext("выход"),NULL);

VV->vramka(PYz,PXz,0);

attron(VV->VVOD_return_cs(iceb_CFS)); 


for(;;)
 {
  move(1,0);

  if(uos.naimo.ravno()[0] != '\0')
   {
    printw("%s: %s\n",gettext("Контрагент"),uos.naimo.ravno());
   }
  if(uos.naipod.ravno()[0] != '\0')
   {
    if(mdd == 0)
      printw("%s: %s\n",gettext("Подразделение получатель"),uos.naipod.ravno());
    else
      printw("%s: %s\n",gettext("Подразделение поставщик"),uos.naipod.ravno());
   }
  if(uos.naipod1.ravno()[0] != '\0')
   {
    printw("%s:%s\n",gettext("Подразделение получатель"),uos.naipod1.ravno());
   }
  if(uos.naikol.ravno()[0] != '\0')
   {
    if(mdd == 0)
      printw("%s: %s\n",gettext("Мат.-ответственный получатель"),uos.naikol.ravno());
    else
      printw("%s: %s\n",gettext("Мат.-ответственный поставщик"),uos.naikol.ravno());
   }
  if(uos.naikol1.ravno()[0] != '\0')
   {
    printw("%s: %s\n",gettext("Мат.-ответственный получатель"),uos.naikol1.ravno());
   }
  if(N > VV->KLST-1)
   {
    N=0;
   }
  if(N < 0)
   {
    N=VV->KLST-1;
   }

  VV->vprintw(N,stdscr);
  
  K=getch();

  attroff(VV->VVOD_return_cs(iceb_CFS));
  attron(VV->VVOD_return_cs(iceb_CFM));

  VV->vprintw(N,stdscr);

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV->PY,VV->PX,VV->MDLS,VV->KLST,stdscr);
  if(K == KEY_RESIZE)
    goto nazad;
    
  if(K == TAB && N == 4 && mdd != 1)
   {
    attroff(VV->VVOD_return_cs(iceb_CFM));

    helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
    
    VVOD VVOD1(2);    
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите наименование контрагента для поиска"));
    naim_v.new_plus("");
    if((kom1=vvod1(&naim_v,112,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
      goto nazad;
    if(kom1 == ESC)
      goto nazad;


   
    clear();
    if(dirkontr(&kod_v,&naim_v,1) == 0)
     {
      uos.naimo.new_plus(naim_v.ravno());
      VV->VVOD_SPISOK_zapis_data(4,kod_v.ravno());
      uos.kontr.new_plus(kod_v.ravno());
     }
    goto nazad;
   }

  if(K == ENTER)
   {
    attroff(VV->VVOD_return_cs(iceb_CFM));
    attron(VV->VVOD_return_cs(iceb_CFV));
    VV->vprintw_clear(N,stdscr);

    if(N == 2) //Подразделение
     {
      attroff(VV->VVOD_return_cs(iceb_CFV));
      if(mr == 1 && provdok(dz,mz,gz,uos.nomdok.ravno(),uos.tipz) != 0)
       {
        attroff(VV->VVOD_return_cs(iceb_CFV));
        VVOD SOOB(1);
        SOOB.VVOD_SPISOK_add_MD(gettext("Корректировка не возможна !"));
        SOOB.VVOD_SPISOK_add_MD(gettext("В документ уже введены записи !"));
        soobshw(&SOOB,stdscr,-1,-1,0,1);

        VV->data_plus(N,uos.pod);
        goto nazad;
       }
      kod_v.new_plus("");
      naim_v.new_plus("");
      if(diruospod(2,&kod_v,&naim_v,1) == 0)
       {
        uos.naipod.new_plus(naim_v.ravno());
        uos.pod=kod_v.ravno_atoi();
        VV->data_plus(2,kod_v.ravno());
        N++;
       }
      goto nazad;

     }

    if(N == 3) //Материально-ответственный
     {
      attroff(VV->VVOD_return_cs(iceb_CFV));
      if(mr == 1 && provdok(dz,mz,gz,uos.nomdok.ravno(),uos.tipz) != 0)
       {
        attroff(VV->VVOD_return_cs(iceb_CFV));
        VVOD SOOB(1);
        SOOB.VVOD_SPISOK_add_MD(gettext("Корректировка не возможна !"));
        SOOB.VVOD_SPISOK_add_MD(gettext("В документ уже введены записи !"));
        soobshw(&SOOB,stdscr,-1,-1,0,1);
        VV->data_plus(N,uos.kodol);
        goto nazad;
       }
      kod_v.new_plus("");
      naim_v.new_plus("");
      if(dirmatot(2,&kod_v,&naim_v,1) == 0)
       {
        uos.naikol.new_plus(naim_v.ravno());
        uos.kodol=kod_v.ravno_atoi();
        VV->data_plus(3,kod_v.ravno());
        N++;
       }
      goto nazad;
      
     }

    if(N == 5 && mdd == 1) //Встречное подразделение
     {
      attroff(VV->VVOD_return_cs(iceb_CFV));
      if(mr == 1 && provdok(dz,mz,gz,uos.nomdok.ravno(),uos.tipz) != 0)
       {
        attroff(VV->VVOD_return_cs(iceb_CFV));
        VVOD SOOB(1);
        SOOB.VVOD_SPISOK_add_MD(gettext("Корректировка не возможна !"));
        SOOB.VVOD_SPISOK_add_MD(gettext("В документ уже введены записи !"));
        soobshw(&SOOB,stdscr,-1,-1,0,1);

        VV->data_plus(N,uos.pod1);
        goto nazad;
       }
      kod_v.new_plus("");
      naim_v.new_plus("");
      if(diruospod(2,&kod_v,&naim_v,1) == 0)
       {
        uos.naipod1.new_plus(naim_v.ravno());
        uos.pod1=kod_v.ravno_atoi();
        VV->data_plus(5,kod_v.ravno());
        N++;
       }
      goto nazad;
     }

    if(N == 6 && mdd == 1) //Встречный Материально-ответственный
     {
      attroff(VV->VVOD_return_cs(iceb_CFV));
      if(mr == 1 && provdok(dz,mz,gz,uos.nomdok.ravno(),uos.tipz) != 0)
       {
        attroff(VV->VVOD_return_cs(iceb_CFV));
        VVOD SOOB(1);
        SOOB.VVOD_SPISOK_add_MD(gettext("Корректировка не возможна !"));
        SOOB.VVOD_SPISOK_add_MD(gettext("В документ уже введены записи !"));
        soobshw(&SOOB,stdscr,-1,-1,0,1);

        VV->data_plus(N,uos.kodol1);
        goto nazad;
       }
      kod_v.new_plus("");
      naim_v.new_plus("");
      if((i1=dirmatot(2,&kod_v,&naim_v,1)) == 0)
       {
        uos.naikol1.new_plus(naim_v.ravno());
        uos.kodol1=kod_v.ravno_atoi();
        VV->data_plus(6,kod_v.ravno());
        N++;
       }

      goto nazad;
     }

    if((N == 9 && uos.tipz == 2)||(N == 14 && uos.tipz == 1) ) //Форма оплаты
     {
      attroff(VV->VVOD_return_cs(iceb_CFV));
      if((i1=vvodkod("Foroplat",&kod,&nai,-1,-1,0)) == 0)
       {
        VV->data_plus(0,"");
        goto nazad;
       }
      uos.forop.new_plus(kod.ravno());
      sprintf(strsql,"%s|%s",kod.ravno(),nai.ravno());
      VV->data_plus(N,strsql);
      if(i1 > 1)
       {
        N++;
        goto nazad;
       }

     }
    if(N == 13) //НДС
     {
      attroff(VV->VVOD_return_cs(iceb_CFV));
      MENU.VVOD_delete();
      sprintf(strsql,"%.f%% %s",pnds,gettext("НДС"));
      MENU.VVOD_SPISOK_add_MD(strsql);
      MENU.VVOD_SPISOK_add_MD(gettext("0% НДС реализация на територии Украины"));
      MENU.VVOD_SPISOK_add_MD(gettext("0% НДС экспорт"));
      MENU.VVOD_SPISOK_add_MD(gettext("Освобождение от НДС"));
      uos.nds=0;
      int kom=0;
      while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
      uos.nds=kom;
 
      if(uos.nds == 0)
       {
        sprintf(strsql,"%.f%% %s",pnds,gettext("НДС"));
        VV->data_plus(13,strsql);
       }
      if(uos.nds == 1)
        VV->data_plus(13,gettext("0% НДС реализация на територии Украины"));
      if(uos.nds == 2)
        VV->data_plus(13,gettext("0% НДС экспорт"));
      if(uos.nds == 3)
        VV->data_plus(13,gettext("Освобождение от НДС"));
      goto nazad;
     }

    VV->vgetstr(N,stdscr);
    
    if(N == 0)  /*Дата*/
     {
      attroff(VV->VVOD_return_cs(iceb_CFV));

      if(mr == 1) //корректировка шапки документа
       {
        if(iceb_t_pbpds(mz,gz) != 0)
         {
          VV->data_plus(N,"");
          goto nazad;
         }
       }


      uos.datpr.new_plus(VV->VVOD_SPISOK_return_data(N));
      if(rsdat(&d,&m,&g,VV->VVOD_SPISOK_return_data(N),1) != 0)
       {
        iceb_t_soob(gettext("Не верно введена дата !"));
	goto nazad;
       }

      if(iceb_t_pbpds(m,g) != 0)
       {
          VV->data_plus(N,"");
	goto nazad;
       }
      pnds=iceb_t_pnds(d,m,g);
      if(uos.nomdok.getdlinna() <= 1 || god_nd != uos.datpr.ravno_god())
       {
        uosnd(g,uos.tipz,&uos.nomdok);
        god_nd=uos.datpr.ravno_god();
       }
      VV->VVOD_SPISOK_zapis_data(1,uos.nomdok.ravno());
      if(mdd == 1)
       {
        if(uos.nomdokp.ravno()[0] == '\0')
          uosnd(g,uos.tipzv,&uos.nomdokp);
        VV->VVOD_SPISOK_zapis_data(4,uos.nomdokp.ravno());
       }

      N++;
      goto nazad;
     }

    if(N == 1)  /*Ном. документа*/
     {
      uos.nomdok.new_plus(VV->VVOD_SPISOK_return_data(N));
      if(provnd(g,uos.tipz,uos.nomdok.ravno()) != 0)
       {
        attroff(VV->VVOD_return_cs(iceb_CFM));
        iceb_t_soob(gettext("Документ с таким номером уже есть !"));

	uos.nomdok.new_plus("");
	VV->data_plus(N,"");
	goto nazad;

       }
     }

    if(N == 4) 
     {
      if(mdd != 1) /*Код контрагента*/
       {
        uos.kontr.new_plus(VV->VVOD_SPISOK_return_data(N));
        uos.pod1=0;

        uos.naimo.new_plus("");
        if(VV->VVOD_SPISOK_return_data(N)[0] != '\0')
         {
          sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
          uos.kontr.ravno_filtr());
          if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
           {
            attroff(VV->VVOD_return_cs(iceb_CFV));

            sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),uos.kontr.ravno());
            iceb_t_soob(strsql);

            uos.kontr.new_plus("");
            uos.naimo.new_plus("");
            VV->data_plus(N,"");
            goto nazad;
           }
          uos.naimo.new_plus(row1[0]);
         }     
       }
      if(mdd == 1) /*Номер встречного документа*/
       {
        uos.nomdokp.new_plus(VV->VVOD_SPISOK_return_data(N));
	if(provnd(g,uos.tipzv,uos.nomdokp.ravno()) != 0)
	 {
          attroff(VV->VVOD_return_cs(iceb_CFM));

          iceb_t_soob(gettext("Документ с таким номером уже есть !"));

	  uos.nomdokp.new_plus("");
          VV->data_plus(N,"");
	  goto nazad;
	}
       }
     }
    if(N == 5) 
     {
      if(uos.tipz == 1) /*Номер встречного документа*/
       {
        uos.nomdokp.new_plus(VV->VVOD_SPISOK_return_data(N));
        if(provnd(g,uos.tipzv,uos.nomdokp.ravno()) != 0)
         {
          attroff(VV->VVOD_return_cs(iceb_CFM));

          iceb_t_soob(gettext("Документ с таким номером уже есть !"));

          uos.nomdokp.new_plus("");
          VV->data_plus(N,"");
          goto nazad;
         }
       }
      if(uos.tipz == 2) //Основание
        uos.osn.new_plus(VV->VVOD_SPISOK_return_data(N));
     }
    if(N == 6)
     {
      if(uos.tipz == 1) /*Основание*/
       uos.osn.new_plus(VV->VVOD_SPISOK_return_data(N));
      if(uos.tipz == 2) /*Доверенность*/
       uos.dover.new_plus(VV->VVOD_SPISOK_return_data(N));
     }

    if(N == 7)
     {
      if(mdd == 1) /*Основание*/
         uos.osn.new_plus(VV->VVOD_SPISOK_return_data(N));
      else
       {
        if(uos.tipz == 1) //Доверенность
         {
          uos.dover.new_plus(VV->VVOD_SPISOK_return_data(N));
         }
        if(uos.tipz == 2) /*Дата выдачи доверенности*/
         {
          if( VV->VVOD_SPISOK_return_data(N)[0] != '\0')
           {
            if(rsdat(&uos.dvd,&uos.mvd,&uos.gvd,VV->VVOD_SPISOK_return_data(N),1) != 0)
             {
              attroff(VV->VVOD_return_cs(iceb_CFV));
              iceb_t_soob(gettext("Не верно введена дата выдачи доверенности !"));
              goto nazad;
             }
           }
          else
           uos.dvd=uos.mvd=uos.gvd=0;
         }
       }
     }   

    if(N == 8)
     {
      
      if(uos.tipz == 1) /*Дата выдачи доверенности*/
       {
        if(VV->VVOD_SPISOK_return_data(N)[0] != '\0')
         {
          if(rsdat(&uos.dvd,&uos.mvd,&uos.gvd,VV->VVOD_SPISOK_return_data(N),1) != 0)
           {
            attroff(VV->VVOD_return_cs(iceb_CFV));
            iceb_t_soob(gettext("Не верно введена дата выдачи доверенности !"));
            goto nazad;
           }
         }
        else
         uos.dvd=uos.mvd=uos.gvd=0;
       }
            
      if(uos.tipz == 2) //Через кого
        uos.sherez.new_plus(VV->VVOD_SPISOK_return_data(N));
     }

    if(N == 9) //Через кого
     {
      uos.sherez.new_plus(VV->VVOD_SPISOK_return_data(N));
     }

    if(N == 10) //Номер налоговой накладной
     {
      
      if(uos.tipz == 2)
       {
        /*Проверяем есть ли такой номер документа*/
        if(SRAV(uos.nomnn.ravno(),VV->VVOD_SPISOK_return_data(N),0) != 0 && VV->VVOD_SPISOK_return_data(N)[0] != '\0')
         {
          g=gt;
          m=mt;
          if(VV->VVOD_SPISOK_return_data(0)[0] != '\0') 
             rsdat(&d,&m,&g,VV->VVOD_SPISOK_return_data(0),0);
          if(provnomnnak(m,g,VV->VVOD_SPISOK_return_data(N)) != 0)
           {
            VV->VVOD_SPISOK_zapis_data(N,uos.nomnn.ravno());
            goto nazad;
           }        
         }

       }
     if(uos.tipz == 1)
      {
       if(iceb_t_pnnnp(VV->data_ravno(11),VV->data_ravno(15),VV->data_ravno(4),VV->data_ravno(10),VV->data_ravno(0),VV->data_ravno(1),3) != 0)
         goto nazad;
      }

      uos.nomnn.new_plus(VV->VVOD_SPISOK_return_data(N));
     }

    if(N == 11 ) /*Дата получения-выдачи налоговой накладной*/
     {
      if(VV->VVOD_SPISOK_return_data(N)[0] != '\0')
       {
        if(rsdat(&uos.dnn,&uos.mnn,&uos.gnn,VV->VVOD_SPISOK_return_data(N),1) != 0)
         {
          attroff(VV->VVOD_return_cs(iceb_CFV));
          iceb_t_soob(gettext("Не введена дата выдачи налоговой накладной !"));
          goto nazad;
         }
       }
      else
       uos.dnn=uos.mnn=uos.gnn=0;

     if(uos.tipz == 1)
      {
       if(iceb_t_pnnnp(VV->data_ravno(11),VV->data_ravno(15),VV->data_ravno(4),VV->data_ravno(10),VV->data_ravno(0),VV->data_ravno(1),3) != 0)
         goto nazad;
      }
     }

    if(N == 12 )
     {
      if(VV->VVOD_SPISOK_return_data(N)[0] != '\0')
       {
        if(rsdat(&uos.dop,&uos.mop,&uos.gop,VV->VVOD_SPISOK_return_data(N),1) != 0)
         {
          attroff(VV->VVOD_return_cs(iceb_CFV));
          iceb_t_soob(gettext("Не верно введена дата оплаты !"));
          goto nazad;
         }
       }
      else
       uos.dop=uos.mop=uos.gop=0;
     }


    if(uos.tipz == 1) /*Приход*/
     {
      if(N == 16 )/*вид договора*/
        uos.vid_dog.new_plus(VV->VVOD_SPISOK_return_data(N));
     }

    if(uos.tipz == 2) /*Расхода*/
     {
      if(N == 14 ) /*вид договора*/
        uos.vid_dog.new_plus(VV->VVOD_SPISOK_return_data(N));
     }     
    attroff(VV->VVOD_return_cs(iceb_CFV));
    attron(VV->VVOD_return_cs(iceb_CFM));

    VV->vprintw(N,stdscr);
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
  attron(VV->VVOD_return_cs(iceb_CFS)); 

  switch (K)
   {
    case FK10: /*Выход*/
    case ESC : /*Выход*/
     attroff(VV->VVOD_return_cs(iceb_CFS)); 
  
     return(1);

    case FK1:   /*Помощь*/
     attroff(VV->VVOD_return_cs(iceb_CFS)); 
     GDITE();
     if(uos.tipz == 1)
       sprintf(strsql,"%s","uos2_1_1.txt");
     if(uos.tipz == 2)
       sprintf(strsql,"%s","uos2_1_2.txt");
     iceb_t_pdoc(strsql);
     clear();
     goto nazad;

    case FK2: /*Записать*/
    case PLU: /*Записать*/

     attroff(VV->VVOD_return_cs(iceb_CFS));
     if(mdd == 1)
      {
       if(VV->VVOD_SPISOK_return_data(0)[0] == '\0' || VV->VVOD_SPISOK_return_data(1)[0] == '\0' || VV->VVOD_SPISOK_return_data(2)[0] == '\0' ||\
       VV->VVOD_SPISOK_return_data(3)[0] == '\0' || VV->VVOD_SPISOK_return_data(4)[0] == '\0' || VV->VVOD_SPISOK_return_data(5)[0] == '\0' ||\
       VV->VVOD_SPISOK_return_data(6)[0] == '\0')
        {
         attroff(VV->VVOD_return_cs(iceb_CFM));
         iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
         goto nazad;
        }
      }
     else
      {
       if(VV->VVOD_SPISOK_return_data(0)[0] == '\0' || VV->VVOD_SPISOK_return_data(1)[0] == '\0' || VV->VVOD_SPISOK_return_data(2)[0] == '\0' ||\
       VV->VVOD_SPISOK_return_data(3)[0] == '\0' || VV->VVOD_SPISOK_return_data(4)[0] == '\0')
        {
         attroff(VV->VVOD_return_cs(iceb_CFM));
         iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
         goto nazad;
        }
      }

     if(rsdat(&d,&m,&g,uos.datpr.ravno(),1) != 0)
      {
       attroff(VV->VVOD_return_cs(iceb_CFM));
       iceb_t_soob(gettext("Не верно введена дата !"));
       goto nazad;
      }
     
     uos.dvnn=uos.mvnn=uos.gvnn=0;
     if(mdd == 0 && uos.tipz == 1)
      {
       if(VV->data_ravno(15)[0] != '\0')
         if(rsdat(&uos.dvnn,&uos.mvnn,&uos.gvnn,VV->data_ravno(15),1) != 0)
          {
           attroff(VV->VVOD_return_cs(iceb_CFM));
           iceb_t_soob(gettext("Не верно введена дата виписки!"));
           goto nazad;
          }
        
      }
     
     if(mdd == 0 && uos.tipz == 2)
      {
       if(VV->VVOD_SPISOK_return_data(7)[0] != '\0')
         if(rsdat(&uos.dvd,&uos.mvd,&uos.gvd,VV->VVOD_SPISOK_return_data(7),1) != 0)
          {
           attroff(VV->VVOD_return_cs(iceb_CFV));
           iceb_t_soob(gettext("Не верно введена дата выдачи доверенности !"));
           goto nazad;
          }

       if(VV->VVOD_SPISOK_return_data(10)[0] != '\0' && VV->VVOD_SPISOK_return_data(11)[0] == '\0')
        {
         attroff(VV->VVOD_return_cs(iceb_CFM));
         iceb_t_soob(gettext("Не введена дата выдачи налоговой накладной !"));
         goto nazad;

        }
       if(VV->VVOD_SPISOK_return_data(11)[0] != '\0')
       if(rsdat(&uos.dnn,&uos.mnn,&uos.gnn,VV->VVOD_SPISOK_return_data(11),0) != 0)
        {
         attroff(VV->VVOD_return_cs(iceb_CFM));
         iceb_t_soob(gettext("Не верно введена дата выдачи налоговой накладной !"));
         goto nazad;
        }

       if(VV->VVOD_SPISOK_return_data(12)[0] != '\0')
         if(rsdat(&uos.dop,&uos.mop,&uos.gop,VV->VVOD_SPISOK_return_data(12),1) != 0)
          {
           iceb_t_soob(gettext("Не верно введена дата оплаты !"));
           goto nazad;
          }

      }
     if(mdd == 1 && uos.pod == uos.pod1 && uos.kodol == uos.kodol1)
      {
       attroff(VV->VVOD_return_cs(iceb_CFM));
       iceb_t_soob(gettext("Коди подразделений и ответственных совпали !"));
       goto nazad;
      }
     goto dal;


    case FK3: /*Посмотреть реквизиты*/
      attroff(VV->VVOD_return_cs(iceb_CFS));
      MENU.VVOD_delete();
      MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));
      MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
      MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка мат.ответственных"));
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


          attroff(VV->VVOD_return_cs(iceb_CFS));
          clear(); /*Очистить экран и закрасить фоновым цветом*/
          naim_v.new_plus("");
          if(dirkontr(&kod_v,&naim_v,1) == 0)
           {
            if(mdd != 1)
             {
              uos.naimo.new_plus(naim_v.ravno());
              VV->VVOD_SPISOK_zapis_data(4,kod_v.ravno());
              uos.kontr.new_plus(kod_v.ravno());
             }
           }
          goto nazad;


        case 1:

          clear();
//          komv=0;
          if(diruospod(1,&kod_v,&naim_v,1) == 0 )
           {
            if(mr == 1 && provdok(dz,mz,gz,uos.nomdok.ravno(),uos.tipz) != 0)
             {
              attroff(VV->VVOD_return_cs(iceb_CFV));
              VVOD SOOB(1);
              SOOB.VVOD_SPISOK_add_MD(gettext("Корректировка не возможна !"));
              SOOB.VVOD_SPISOK_add_MD(gettext("В документ уже введены записи !"));
              soobshw(&SOOB,stdscr,-1,-1,0,1);
              VV->data_plus(N,uos.kodol);
              goto nazad;
             }
            if(mdd != 1)
             {
              uos.naipod.new_plus(naim_v.ravno());
              VV->VVOD_SPISOK_zapis_data(2,kod_v.ravno());
              uos.pod=kod_v.ravno_atoi();
             }
           }
          goto nazad;
  
          case 2:

          if(dirmatot(1,&kod_v,&naim_v,1) == 0 )
           {
            if(mr == 1 && provdok(dz,mz,gz,uos.nomdok.ravno(),uos.tipz) != 0)
             {
              attroff(VV->VVOD_return_cs(iceb_CFV));
              VVOD SOOB(1);
              SOOB.VVOD_SPISOK_add_MD(gettext("Корректировка не возможна !"));
              SOOB.VVOD_SPISOK_add_MD(gettext("В документ уже введены записи !"));
              soobshw(&SOOB,stdscr,-1,-1,0,1);
              VV->data_plus(N,uos.kodol);
              goto nazad;
             }
            if(mdd != 1)
             {
              uos.naikol.new_plus(naim_v.ravno());
              VV->VVOD_SPISOK_zapis_data(3,kod_v.ravno());
              uos.kodol=kod_v.ravno_atoi();
             }
           }

          goto nazad;
       
     }

    case SFK4: /*Получить номер налоговой накладной для расходного документа*/
     attroff(VV->VVOD_return_cs(iceb_CFS));
     if(uos.tipz == 1 || mdd == 1)
       goto nazad;

     if(VV->data_ravno(0)[0] == '\0')
      {
       iceb_t_soob(gettext("Не введена дата документа!"));
       goto nazad;
      
      }      
     if(rsdat(&d,&m,&g,VV->VVOD_SPISOK_return_data(0),1) != 0)
      {
       iceb_t_soob(gettext("Не верно введена дата !"));
       goto nazad;
      }
     sprintf(strsql,"%d",nomnalnak(m,g));
     VV->data_plus(10,strsql);
     goto nazad;
    default:
     break;

  }
 
 }

dal:;
attron(VV->VVOD_return_cs(iceb_CB));

GDITE();
int metka_opl=0;
//Узнаём нужно ли для этой операции делать проводки
if(uos.tipz == 1)
 sprintf(strsql,"select prov,vido from Uospri where kod='%s'",uos.kprr.ravno());
if(uos.tipz == 2)
 sprintf(strsql,"select prov,vido from Uosras where kod='%s'",uos.kprr.ravno());

prov=1;
if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
 if(row1[0][0] == '1')
  prov=0;
if(atoi(row1[1]) != 0)
 metka_opl=1;
if(rsdat(&d,&m,&g,VV->VVOD_SPISOK_return_data(0),1) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto nazad;
 }

if(iceb_t_pbpds(m,g) != 0)
 {
  VV->data_plus(0,"");
  goto nazad;
 }


if(mr == 0) /* Запись нового документа*/
 {

  /*Проверяем номер документа*/
  if(provnd(g,uos.tipz,uos.nomdok.ravno()) != 0)
   {
    iceb_t_soob(gettext("Документ с таким номером уже есть !"));
    goto nazad;
   }
  if(mdd == 1)
  if(provnd(g,uos.tipzv,uos.nomdokp.ravno()) != 0)
   {
    iceb_t_soob(gettext("Номер встречного документа уже есть !"));
    goto nazad;
   }

  float nds=iceb_t_pnds(d,m,g);

  time(&vrem);

  /*Записываем шапку документа*/

  sprintf(strsql,"insert into Uosdok \
values ('%04d-%d-%d',%d,'%s','%s','%s',%d,%d,%d,'%s',%d,%d,%d,'%s',%d,%ld,\
'%s','%04d-%d-%d','%s','%04d-%d-%d','%s','%s','%04d-%d-%d',%d,%.2f,'%04d-%02d-%02d',%d,'%s')",
  g,m,d,uos.tipz,uos.kprr.ravno(),uos.kontr.ravno_filtr(),uos.nomdok.ravno_filtr(),uos.pod,uos.kodol,
  uos.pod1,uos.nomdokp.ravno_filtr(),uos.kodol1,prov,0,uos.osn.ravno_filtr(),
  iceb_t_getuid(),vrem,uos.dover.ravno_filtr(),uos.gvd,uos.mvd,uos.dvd,uos.forop.ravno_filtr(),
  uos.gnn,uos.mnn,uos.dnn,uos.nomnn.ravno_filtr(),uos.sherez.ravno_filtr(),uos.gop,uos.mop,uos.dop,
  uos.nds,nds,uos.gvnn,uos.mvnn,uos.dvnn,metka_opl,uos.vid_dog.ravno_filtr());

  if(sql_zapis(strsql,1,0) != 0)
    goto nazad;

  if(mdd == 1)
   {
    sprintf(strsql,"insert into Uosdok \
    values ('%04d-%d-%d',%d,'%s','%s','%s',%d,%d,%d,'%s',%d,%d,%d,'%s',\
%d,%ld,'%s','%04d-%d-%d','%s','%04d-%d-%d','%s','%s','%04d-%d-%d',%d,%.2f,'%04d-%02d-%02d',%d,'%s')",
    g,m,d,uos.tipzv,uos.kprr.ravno(),uos.kontr.ravno_filtr(),uos.nomdokp.ravno_filtr(),uos.pod1,uos.kodol1,
    uos.pod,uos.nomdok.ravno_filtr(),uos.kodol,0,0,uos.osn.ravno_filtr(),
    iceb_t_getuid(),vrem,uos.dover.ravno_filtr(),uos.gvd,uos.mvd,uos.dvd,uos.forop.ravno_filtr(),
    0,0,0,"","",0,0,0,0,nds,uos.gvnn,uos.mvnn,uos.dvnn,metka_opl,uos.vid_dog.ravno_filtr());

    if(sql_zapis(strsql,1,0) != 0)
      goto nazad;
   }
 }

if(mr == 1) /*Корректировка  шапки документа*/
 {
  if(m != mz || g != gz) /*Проверяем блокировку проводок по обоим датам*/
    if(iceb_t_pvkdd(3,dz,mz,gz,m,g,uos.pod,nomdokz.ravno(),uos.tipz) != 0)
      goto nazad;


  /*Проверяем номер документа Если в существующем документе изменить год-то надо проверять*/
  if(SRAV(uos.nomdok.ravno(),nomdokz.ravno(),0) != 0 || gz != g)
  if(provnd(g,uos.tipz,uos.nomdok.ravno()) != 0)
   {
//    printw("\n%d %d %d %s %s\n",g,gz,uos.tipz,uos.nomdok,nomdokz);
    iceb_t_soob(gettext("Документ с таким номером уже есть !"));
    goto nazad;
   }

  if(mdd == 1)
  if(SRAV(uos.nomdokp.ravno(),nomdokz1.ravno(),0) != 0)
  if(provnd(g,uos.tipzv,uos.nomdokp.ravno()) != 0)
   {
    iceb_t_soob(gettext("Номер встречного документа уже есть !"));
    goto nazad;
   }

  time(&vrem);

  sprintf(strsql,"update Uosdok \
set \
datd='%04d-%d-%d',\
tipz=%d,\
kodop='%s',\
kontr='%s',\
nomd='%s',\
podr=%d,\
kodol=%d,\
podrv=%d,\
nomdv='%s',\
kodolv=%d,\
osnov='%s',\
ktoz=%d,\
vrem=%ld,\
prov=%d,\
dover='%s',\
datvd='%04d-%d-%d',\
forop='%s',\
datpnn='%04d-%d-%d',\
nomnn='%s',\
sherez='%s',\
datop='%04d-%d-%d',\
nds=%d,\
dvnn='%04d-%d-%d',\
vd='%s' \
where datd='%d-%d-%d' and nomd='%s'",
  g,m,d,uos.tipz,uos.kprr.ravno(),uos.kontr.ravno_filtr(),uos.nomdok.ravno_filtr(),uos.pod,uos.kodol,
  uos.pod1,uos.nomdokp.ravno_filtr(),uos.kodol1,uos.osn.ravno_filtr(),iceb_t_getuid(),vrem,prov,
  uos.dover.ravno_filtr(),uos.gvd,uos.mvd,uos.dvd,uos.forop.ravno_filtr(),
  uos.gnn,uos.mnn,uos.dnn,uos.nomnn.ravno_filtr(),uos.sherez.ravno_filtr(),uos.gop,uos.mop,uos.dop,
  uos.nds,uos.gvnn,uos.mvnn,uos.dvnn,uos.vid_dog.ravno_filtr(),
  gz,mz,dz,nomdokz.ravno_filtr());
//  printw("\n%s\n",strsql);
//  OSTANOV();

  if(sql_zapis(strsql,1,0) != 0)
   goto nazad;

  if(mdd == 1)
   {
    sprintf(strsql,"update Uosdok \
set \
datd='%04d-%d-%d',\
tipz=%d,\
kodop='%s',\
kontr='%s',\
nomd='%s',\
podr=%d,\
kodol=%d,\
podrv=%d,\
nomdv='%s',\
kodolv=%d,\
osnov='%s',\
ktoz=%d,\
vrem=%ld,\
datpnn='%04d-%d-%d',\
nomnn='%s',\
sherez='%s',\
datop='%04d-%d-%d',\
nds=%d \
where datd='%04d-%d-%d' and nomd='%s'",
    g,m,d,uos.tipzv,uos.kprr.ravno(),uos.kontr.ravno_filtr(),uos.nomdokp.ravno_filtr(),uos.pod1,uos.kodol1,
    uos.pod,uos.nomdok.ravno_filtr(),uos.kodol,uos.osn.ravno_filtr(),iceb_t_getuid(),vrem,0,0,0,"","",0,0,0,0,
    gz,mz,dz,nomdokz1.ravno_filtr());

    if(sql_zapis(strsql,1,0) != 0)
      goto nazad;

   }
  /*Исправляем записи в документе*/
  if(SRAV1(g,m,d,gz,mz,dz) != 0 || SRAV(uos.nomdok.ravno(),nomdokz.ravno(),0) != 0 ||
  uos.pod != uospodz || uos.kodol != uoskodolz)
   {
    sprintf(strsql,"update Uosdok1 \
set \
datd='%04d-%d-%d',\
nomd='%s',\
podr=%d,\
kodol=%d \
where datd='%04d-%d-%d' and nomd='%s'",
    g,m,d,uos.nomdok.ravno_filtr(),uos.pod,uos.kodol,
    gz,mz,dz,nomdokz.ravno_filtr());

    if(sql_zapis(strsql,1,0) != 0)
      goto nazad;

   }

  if(mdd == 1)
  if(SRAV1(g,m,d,gz,mz,dz) != 0 || SRAV(uos.nomdokp.ravno(),nomdokz1.ravno(),0) != 0)
   {
    sprintf(strsql,"update Uosdok1 \
set \
datd='%04d-%d-%d',\
nomd='%s' \
where datd='%04d-%d-%d' and nomd='%s'",
    g,m,d,uos.nomdokp.ravno_filtr(),
    gz,mz,dz,nomdokz1.ravno_filtr());

    if(sql_zapis(strsql,1,0) != 0)
      goto nazad;
   }

//  Исправляем дату в промежуточных записях
  if(SRAV1(g,m,d,gz,mz,dz) != 0 )
   {
    uosinp(d,m,g,mz,gz,uos.nomdok.ravno());
   }  

  if(SRAV1(g,m,d,gz,mz,dz) != 0 || SRAV(uos.nomdok.ravno(),nomdokz.ravno(),0) != 0)
   {
    /*Исправляем проводки в которых дата проводки совпадает с
    датой документа*/

    sprintf(strsql,"update Prov \
set \
oper='%s',\
nomd='%s',\
datp='%04d-%d-%d',\
datd='%04d-%d-%d',\
ktoi=%d,\
vrem=%ld \
where datd='%04d-%d-%d' and datp='%04d-%d-%d' and nomd='%s' and pod=%d and \
kto='%s' and tz=%d",
    uos.kprr.ravno(),uos.nomdok.ravno_filtr(),g,m,d,g,m,d,iceb_t_getuid(),vrem,
    gz,mz,dz,gz,mz,dz,nomdokz.ravno_filtr(),uospodz,ICEB_MP_UOS,
    uos.tipz);

    sql_zapis(strsql,1,0);
    /*Исправляем остальные проводки*/

    sprintf(strsql,"update Prov \
set \
oper='%s',\
nomd='%s',\
datd='%04d-%d-%d',\
ktoi=%d,\
vrem=%ld \
where datd='%04d-%d-%d' and nomd='%s' and pod=%d and kto='%s' and tz=%d",
   uos.kprr.ravno(),uos.nomdok.ravno_filtr(),g,m,d,iceb_t_getuid(),vrem,
   gz,mz,dz,nomdokz.ravno_filtr(),uospodz,ICEB_MP_UOS,
   uos.tipz);

    sql_zapis(strsql,1,0);
   }

  if(SRAV(kontrz.ravno(),uos.kontr.ravno(),0) != 0)
   {
    /*Исправляем код контрагента в проводках*/
    sprintf(strsql,"update Prov \
set \
kodkon='%s' \
where datd='%04d-%d-%d' and nomd='%s' and pod=%d and kto='%s' and kodkon='%s' and tz=%d",
    uos.kontr.ravno_filtr(),
    gz,mz,dz,nomdokz.ravno_filtr(),uos.pod,ICEB_MP_UOS,kontrz.ravno_filtr(),uos.tipz);

    sql_zapis(strsql,1,0);
   }

 }
return(0);
}


/***************************/
/*Проверка номера документа*/
/***************************/
/*Если вернули 0-документа нет
	       1-есть
 */

short           provnd(short g,short tz,const char *ndk)
{
char		strsql[512];

sprintf(strsql,"select nomd from Uosdok where datd >= '%04d-01-01' and \
datd <= '%d-12-31' and nomd='%s'",g,g,ndk);
if(sql_readkey(&bd,strsql) >= 1)
 return(1);

return(0);
 
}

/*********************************************/
/*Автоматическое определение номера документа*/
/*********************************************/
void uosnd(short g,short tipz,class iceb_tu_str *nomdok)
{
int             mpn;  /*Максимальный порядковый номер*/
int		i;
long		kolstr;
SQL_str         row;
char		strsql[512];

mpn=0;

sprintf(strsql,"select nomd from Uosdok where datd >= '%04d-01-01' and \
datd <= '%04d-12-31' and tipz=%d",g,g,tipz);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {

  i=0;

  if(tipz == 1)
   if(POLE(row[0],strsql,2,'+') == 0)
    i=atoi(strsql);

  if(tipz == 2)
   if(POLE(row[0],strsql,2,'-') == 0)
    i=atoi(strsql);

  if(i > mpn)
    mpn=i;

 }

mpn++;
memset(strsql,'\0',sizeof(strsql));
if(tipz == 1)
  sprintf(strsql,"+%d",mpn);
if(tipz == 2)
  sprintf(strsql,"-%d",mpn);
nomdok->new_plus(strsql);
}

/*********************************************/
/*Проверка документа на наличие в нем записей*/
/*********************************************/
/*Возвращаем количество записей в документе*/

int		provdok(short dd,short md,short gd,const char *ndk,short tz)
{
int		kolstr;
char		strsql[512];

sprintf(strsql,"select innom from Uosdok1 where datd='%04d-%02d-%02d' and \
nomd='%s' and tipz=%d",gd,md,dd,ndk,tz);
/*
printw("%s\n",strsql);
refresh();  
*/
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(0);
 }


return(kolstr);

}
/************************************/
void	sozuosvdok(VVOD *VV)
{

if(uos.tipz == 1)
 {
  if(mdd == 1)
   {
    VV->VVOD_SPISOK_add_data(11);//0
    VV->VVOD_SPISOK_add_data(20);//1
    VV->VVOD_SPISOK_add_data(10);//2
    VV->VVOD_SPISOK_add_data(10);//3
    VV->VVOD_SPISOK_add_data(20);//4
    VV->VVOD_SPISOK_add_data(10);//5
    VV->VVOD_SPISOK_add_data(10);//6
    VV->VVOD_SPISOK_add_data(40);//7
   }
  else
   {
    VV->VVOD_SPISOK_add_data(11);//0
    VV->VVOD_SPISOK_add_data(20);//1
    VV->VVOD_SPISOK_add_data(10);//2
    VV->VVOD_SPISOK_add_data(10);//3
    VV->VVOD_SPISOK_add_data(10);//4
    VV->VVOD_SPISOK_add_data(20);//5
    VV->VVOD_SPISOK_add_data(40);//6
    VV->VVOD_SPISOK_add_data(40);//7
    VV->VVOD_SPISOK_add_data(11);//8
    VV->VVOD_SPISOK_add_data(40);//9
    VV->VVOD_SPISOK_add_data(16);//10
    VV->VVOD_SPISOK_add_data(11);//11
    VV->VVOD_SPISOK_add_data(11);//12
    VV->VVOD_SPISOK_add_data(50);//13
    VV->VVOD_SPISOK_add_data(50);//14
    VV->VVOD_SPISOK_add_data(11);//15
    VV->VVOD_SPISOK_add_data(128);//16

   }
 }

if(uos.tipz == 2)
 {
  if(mdd == 1)
   {
    VV->VVOD_SPISOK_add_data(11);//0
    VV->VVOD_SPISOK_add_data(20);//1
    VV->VVOD_SPISOK_add_data(10);//2
    VV->VVOD_SPISOK_add_data(10);//3
    VV->VVOD_SPISOK_add_data(20);//4
    VV->VVOD_SPISOK_add_data(10);//5
    VV->VVOD_SPISOK_add_data(10);//6
    VV->VVOD_SPISOK_add_data(40);//7
   }
  else
   {
    VV->VVOD_SPISOK_add_data(11);//0
    VV->VVOD_SPISOK_add_data(20);//1
    VV->VVOD_SPISOK_add_data(10);//2
    VV->VVOD_SPISOK_add_data(10);//3
    VV->VVOD_SPISOK_add_data(10);//4
    VV->VVOD_SPISOK_add_data(40);//5
    VV->VVOD_SPISOK_add_data(30);//6
    VV->VVOD_SPISOK_add_data(11);//7
    VV->VVOD_SPISOK_add_data(40);//8
    VV->VVOD_SPISOK_add_data(40);//9
    VV->VVOD_SPISOK_add_data(16);//10
    VV->VVOD_SPISOK_add_data(11);//11
    VV->VVOD_SPISOK_add_data(11);//12
    VV->VVOD_SPISOK_add_data(40);//13
    VV->VVOD_SPISOK_add_data(128);//14
   }                             
 }


}
/********************************************************************/ 
/*Подпрограмма исправления промежуточных записей для всех инвентарных 
номеров в документе*/
/**********************************************************************/

void  uosinp(short dd,short md,short gd, //Дата документа
short mz,short gz, //Предыдущая дата документа
const char *nomdok)
{
char strsql[512];
int	kolstr=0;
SQL_str	row;

sprintf(strsql,"select innom from Uosdok1 \
where datd='%04d-%02d-%02d' and nomd='%s'",
gd,md,dd,nomdok);


SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 return;

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"update Uosinp \
set \
mes=%d,\
god=%d \
where god=%d and mes=%d and innom=%s",
    md,gd,gz,mz,row[0]);
  sql_zapis(strsql,1,0);

 }

}
