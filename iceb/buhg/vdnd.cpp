/* $Id: vdnd.c,v 5.85 2014/07/31 07:09:17 sasa Exp $ */
/*26.01.2018    06.05.1997      Белых А.И.      vdnd.c
Ввод накладных 
Если вернули 0 - записали шапку накладной
             1- нет
*/
#include	"buhg.h"

int zapdok(VVOD*,int,const char*,int,int,const char*,const char*,const char*,int,double,int,int,const char*,const char *k00);

short           mdd;  /*0-обычный документ 1-двойной*/
extern class iceb_tu_str bnds1; /*0% НДС реализация на територии Украины.*/
extern class iceb_tu_str bnds2; /*0% НДС экспорт.*/
extern class iceb_tu_str bnds3; /*0% Освобождение от НДС статья 5.*/
extern class iceb_tu_str nds_mp; /*НДС на мед. препараты*/

short vdnd(VVOD *VV,class iceb_tu_str *dat0, //запомненная дата
int tipz, //1-приход 2-расход
class iceb_tu_str *kprr, //Код прихода-расхода
int rr,    //0-ввод 1-корректировка
int lnds,  //Метка ндс документа
double sumkor,  //Сумма корректировки
int loginrash, //логин распечатвшего чек
int metkapr) //0-xmu 1-xpro
{
short    mvnp=0; /*0-внешняя 1 -внутреняя 2-изменение стоимости*/
SQL_str         row;
char		strsql[2048];
int		sklz=0,skl1=0;
int             i,i1;
short           K;
int             N;
short           d,m,g;
short god_nd=0; /*Год нумерации документов*/
class iceb_tu_str skll("");
class iceb_tu_str naimpr(""); /*Наименование прихода-расхода*/
char		bros[1024];
class iceb_tu_str bros1("");
class iceb_tu_str bros2("");
class iceb_tu_str naimo("");
class iceb_tu_str naiskl("");
class iceb_tu_str naiskl1("");
class iceb_tu_str kpos("");
class iceb_tu_str nom_pd(""); /*Номер парного документа*/
class iceb_tu_str nomnn(""); /*Номер налоговой накладной*/
class iceb_tu_str datazap(""); /*Запомненная дата*/
class iceb_tu_str nomdokz(""); /*Запомненный номер документа*/
int		poz,komv;
int		kom1;
class iceb_tu_str copiq("");
class iceb_tu_str kontrz("");
class iceb_tu_str kod("");
class iceb_tu_str naik("");
short		dpnn=0,mpnn=0,gpnn=0; //Дата получения налоговой накладной
class iceb_tu_str kodkontr("");
class iceb_tu_str naimkontr("");
short dt,mt,gt;
float pnds=0.;
int un_nom_zap=0;
class iceb_tu_str k00("00");
class SQLCURSOR       cur;


poltekdat(&dt,&mt,&gt);
if(dat0->ravno()[0] == '\0')
 pnds=iceb_t_pnds(dt,mt,gt);
else
 pnds=iceb_t_pnds(dat0->ravno());
 
if(rr == 1) /*корректировка постепенно нужно передалать*/
 {
  sprintf(strsql,"select * from Dokummat where datd='%s' and sklad=%d and nomd='%s'",VV->ravno_sqldat(1),VV->data_ravno_atoi(2),VV->data_ravno_filtr(3));
  if(readkey(strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s %d %s",gettext("Не найден документ"),VV->data_ravno(1),VV->data_ravno_atoi(2),VV->data_ravno(3));
    iceb_t_soob(strsql);
    return(1); 
   }  
  k00.new_plus(row[15]);
 }





VVOD MENU(3);
VVOD VVOD1(2);
VVOD DANET(1);

d=dt; m=mt; g=gt;
N=0;
sklz=(int)ATOFM(VV->VVOD_SPISOK_return_data(2));
kontrz.new_plus(VV->VVOD_SPISOK_return_data(0));

rsdat(&d,&m,&god_nd,VV->data_ravno(1),1);
datazap.new_plus(VV->VVOD_SPISOK_return_data(1));

nomdokz.new_plus(VV->VVOD_SPISOK_return_data(3));


nom_pd.new_plus(VV->VVOD_SPISOK_return_data(7));

nomnn.new_plus(VV->VVOD_SPISOK_return_data(11));

/*Читаем наименование контрагента*/
if(VV->VVOD_SPISOK_return_data(0)[0] != '\0')
 {
  memset(bros,'\0',sizeof(bros));
  if(VV->VVOD_SPISOK_return_data(0)[0] == '0' && VV->VVOD_SPISOK_return_data(0)[1] == '0' && VV->VVOD_SPISOK_return_data(0)[2] == '-')
      strcpy(bros,"00");
  else
      strncpy(bros,VV->VVOD_SPISOK_return_data(0),sizeof(bros)-1);

  sprintf(strsql,"select naikon from Kontragent \
where kodkon='%s'",bros);

  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    beep();
    sprintf(strsql,"%s-%s %s !",__FUNCTION__,gettext("Не найден код контрагента"),bros);
    iceb_t_soob(strsql);
   }
  else
   naimo.new_plus(row[0]);

 }
/*Читаем наименование склада*/
if(VV->VVOD_SPISOK_return_data(2)[0] != '\0')
 {
  sprintf(strsql,"select naik from Sklad where kod=%s",VV->VVOD_SPISOK_return_data(2));

  if(readkey(strsql,&row,&cur) != 1)
   {
    beep();
    sprintf(strsql,"%s-%s %s !",__FUNCTION__,gettext("Не найден склад"),VV->VVOD_SPISOK_return_data(2));
    iceb_t_soob(strsql);
   }
  else
   naiskl.new_plus(row[0]);
 }

/*Читаем наименование операции*/
if(tipz == 1)
   bros1.new_plus("Prihod");
if(tipz == 2)
   bros1.new_plus("Rashod");

sprintf(strsql,"select naik,vido from %s where kod='%s'",bros1.ravno(),kprr->ravno());
if((i=sql_readkey(&bd,strsql,&row,&cur)) != 1)
 {
  printw("\ni=%d strsql=%s\n",i,strsql);
  if(i < 0)
    printw("%s\n",sql_error(&bd));
  beep();
  printw("%s-",__FUNCTION__);
  printw(gettext("Не найден код операции %s !"),kprr);
  printw("\n");
  OSTANOV();
  return(1);
 }
mvnp=atoi(row[1]);
naimpr.new_plus(row[0]);


if(rr == 0)
 {
  lnds=0;
  if(proverka(bnds1.ravno(),kprr->ravno(),0,1) == 0 || bnds1.ravno()[0] == '*')
    lnds=1;
  if(proverka(bnds2.ravno(),kprr->ravno(),0,1) == 0 || bnds2.ravno()[0] == '*')
    lnds=2;
  if(proverka(bnds3.ravno(),kprr->ravno(),0,1) == 0 || bnds3.ravno()[0] == '*')
    lnds=3;
  if(proverka(nds_mp.ravno(),kprr->ravno(),0,1) == 0 || nds_mp.ravno()[0] == '*')
    lnds=4;

  if(tipz == 1)
   {
    if(lnds == 0)
     {
      sprintf(strsql,"%.f%% %s",pnds,gettext("НДС"));
      VV->VVOD_SPISOK_zapis_data(11,strsql);
     }
    if(lnds == 1)
      VV->VVOD_SPISOK_zapis_data(11,gettext("0% НДС реализация на територии Украины"));
    if(lnds == 2)
      VV->VVOD_SPISOK_zapis_data(11,gettext("0% НДС экспорт"));
    if(lnds == 3)
      VV->VVOD_SPISOK_zapis_data(11,gettext("Освобождение от НДС"));
    if(lnds == 4)
      VV->VVOD_SPISOK_zapis_data(11,gettext("7% НДС на мед.препараты"));
   }
  if(tipz == 2)
   {
    if(lnds == 0)
     {
      sprintf(strsql,"%.f%% %s",pnds,gettext("НДС"));
      VV->VVOD_SPISOK_zapis_data(14,strsql);
     }
    if(lnds == 1)
      VV->VVOD_SPISOK_zapis_data(14,gettext("0% НДС реализация на територии Украины"));
    if(lnds == 2)
      VV->VVOD_SPISOK_zapis_data(14,gettext("0% НДС экспорт"));
    if(lnds == 3)
      VV->VVOD_SPISOK_zapis_data(14,gettext("Освобождение от НДС"));
    if(lnds == 4)
      VV->VVOD_SPISOK_zapis_data(14,gettext("7% НДС на мед.препараты"));
    
   }
 }

if(tipz == 1)
 {
  VV->VVOD_SPISOK_add_MD(gettext("Код контрагента поставщика.."));//0
  VV->VVOD_SPISOK_add_MD(gettext("Дата........................"));//1
  VV->VVOD_SPISOK_add_MD(gettext("Склад получатель............"));//2
  VV->VVOD_SPISOK_add_MD(gettext("Номер документа............."));//3
  VV->VVOD_SPISOK_add_MD(gettext("Доверенность................"));//4
  VV->VVOD_SPISOK_add_MD(gettext("Дата видачи доверенности...."));//5
  VV->VVOD_SPISOK_add_MD(gettext("Через кого.................."));//6
  VV->VVOD_SPISOK_add_MD(gettext("Накладная поставщика........"));//7
  VV->VVOD_SPISOK_add_MD(gettext("Номер налоговой накладной..."));//8
  VV->VVOD_SPISOK_add_MD(gettext("Дата выписки нал.накладной.."));//9
  VV->VVOD_SPISOK_add_MD(gettext("Дата получения нал.нак-ной.."));//10
  VV->VVOD_SPISOK_add_MD(gettext("НДС........................."));//11
  VV->VVOD_SPISOK_add_MD(gettext("Дата оплаты................."));//12
  VV->VVOD_SPISOK_add_MD(gettext("Форма оплаты................"));//13
  VV->VVOD_SPISOK_add_MD(gettext("Коментарий.................."));//14

 }

if(tipz == 2)
 {
  VV->VVOD_SPISOK_add_MD(gettext("Код контрагента получателя.."));//0
  VV->VVOD_SPISOK_add_MD(gettext("Дата........................"));//1
  VV->VVOD_SPISOK_add_MD(gettext("Склад поставщик............."));//2
  VV->VVOD_SPISOK_add_MD(gettext("Номер документа............."));//3
  VV->VVOD_SPISOK_add_MD(gettext("Доверенность................"));//4
  VV->VVOD_SPISOK_add_MD(gettext("Дата видачи доверенности...."));//5
  VV->VVOD_SPISOK_add_MD(gettext("Через кого.................."));//6
  VV->VVOD_SPISOK_add_MD(gettext("Номер парного документа....."));//7
  VV->VVOD_SPISOK_add_MD(gettext("Основание..................."));//8
  VV->VVOD_SPISOK_add_MD(gettext("Вид договора................"));//9
  VV->VVOD_SPISOK_add_MD(gettext("Форма оплаты................"));//10
  VV->VVOD_SPISOK_add_MD(gettext("Номер налоговой накладной..."));//11
  VV->VVOD_SPISOK_add_MD(gettext("Дата оплаты................."));//12
  VV->VVOD_SPISOK_add_MD(gettext("Номер машины................"));//13
  VV->VVOD_SPISOK_add_MD(gettext("НДС........................."));//14
  VV->VVOD_SPISOK_add_MD(gettext("Дата выдачи нал. накладной.."));//15
 }

nazad:;


clear();

move(0,0);
printw(gettext("Операция: %s %s"),kprr->ravno(),naimpr.ravno());

attron(A_BLINK); /*Включение мерцания*/
if(tipz == 1)
  printw(" %s",gettext("Приход."));
if(tipz == 2)
  printw(" %s",gettext("Расход."));
if(mvnp == 0)
  printw(" %s",gettext("Внеш."));
if(mvnp == 1)
  printw(" %s",gettext("Внут."));
if(mvnp == 2)
  printw(" %s",gettext("Изм."));
if(mdd == 1)
  printw(" %s",gettext("(двойной документ)"));
printw(" %s",k00.ravno());

attroff(A_BLINK); /*Выключение мерцания*/

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("реквизиты"),
"F4",gettext("номер докум."),
"F5",gettext("конт.00"),
"F10",gettext("выход"),NULL);

VV->vramka(4,0,0);


for(;;)
 {
  attron(VV->VVOD_return_cs(iceb_CFS)); 

  if(N > VV->KLST-1)
   {
    N=0;
   }
  if(N < 0)
   {
    N=VV->KLST-1;
   }

  VV->vprintw(N,stdscr);
  
  if(naimo.getdlinna() > 1)
   {
    move(1,0);
    printw("%s%-*s",gettext("Контрагент:"),iceb_tu_kolbait(40,naimo.ravno()),naimo.ravno());
   }
  if(naiskl.getdlinna() > 1)
   {
    move(2,0);
    printw("%s%-*s",gettext("Склад     :"),iceb_tu_kolbait(40,naiskl.ravno()),naiskl.ravno());
   }
  if(naiskl1.getdlinna() > 1)
   {
    move(3,0);
    printw("%-*s",iceb_tu_kolbait(40,naiskl1.ravno()),naiskl1.ravno());
   }
  refresh();

  K=getch();

  attroff(VV->VVOD_return_cs(iceb_CFS));
  attron(VV->VVOD_return_cs(iceb_CFM));

  VV->vprintw(N,stdscr);

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV->PY,VV->PX,VV->MDLS,VV->KLST,stdscr);

  if(K == KEY_RESIZE)
   {
//    getch();
    goto nazad;
   }    

  if(K == TAB && N == 0)
   {
    attroff(VV->VVOD_return_cs(iceb_CFM));

    naimkontr.new_plus("");

    helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
    
    VVOD1.VVOD_delete();
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите наименование контрагента для поиска"));
    if((kom1=vvod1(&naimkontr,40,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
      goto nazad;
    if(kom1 == ESC)
      goto nazad;

    clear();
    if(dirkontr(&kodkontr,&naimkontr,1) == 0)
     {
      skl1=0;
      kpos.new_plus(kodkontr.ravno());
      naimo.new_plus(naimkontr.ravno());
      VV->VVOD_SPISOK_zapis_data(0,kodkontr.ravno());
      goto nazad;
     }
    goto nazad;
   }

  if(K == ENTER)
   {
    attroff(VV->VVOD_return_cs(iceb_CFM));
    attron(VV->VVOD_return_cs(iceb_CFV));

    VV->vprintw_clear(N,stdscr);

    if(N == 0 && VV->VVOD_SPISOK_return_data(0)[0] == '0' && VV->VVOD_SPISOK_return_data(0)[1] == '0' &&
    VV->VVOD_SPISOK_return_data(0)[2] == '-' && VV->VVOD_SPISOK_return_data(1)[0] != '\0' && VV->VVOD_SPISOK_return_data(2)[0] != '\0' && \
    VV->VVOD_SPISOK_return_data(3)[0] != '\0')
     {
      //Если в накладной есть записи, то код изменить нельзя
      rsdat(&d,&m,&g,VV->VVOD_SPISOK_return_data(1),0);
      sprintf(strsql,"select sklad from Dokummat1 where datd='%d-%d-%d' \
and nomd='%s' and sklad=%s",g,m,d,VV->VVOD_SPISOK_return_data(3),VV->VVOD_SPISOK_return_data(2));
      if(sql_readkey(&bd,strsql,&row,&cur) > 0)
       {
        attroff(VV->VVOD_return_cs(iceb_CFV));
        VVOD SOOB(1);
        SOOB.VVOD_SPISOK_add_MD(gettext("Код контрагента в этом случае изменить нельзя !"));
        SOOB.VVOD_SPISOK_add_MD(gettext("Нужно удалить документ и ввести с нужным кодом контрагента !"));
        soobshw(&SOOB,stdscr,-1,-1,0,1);
        goto nazad;

       }
     }    
    
    if(N == 2) /*Склад*/
     if(sklz != 0)
      {
       attroff(VV->VVOD_return_cs(iceb_CFV));
       VVOD SOOB(1);
       SOOB.VVOD_SPISOK_add_MD(gettext("Склад изменить нельзя !"));
       SOOB.VVOD_SPISOK_add_MD(gettext("Нужно удалить документ и ввести с нужным складом !"));
       soobshw(&SOOB,stdscr,-1,-1,0,1);
       goto nazad;
      }

    if(tipz == 1 && N == 11) /*НДС*/
     {
      attroff(VV->VVOD_return_cs(iceb_CFV));
      MENU.VVOD_delete();
      sprintf(strsql,"%.f%% %s",pnds,gettext("НДС"));

      MENU.VVOD_SPISOK_add_MD(strsql);
      MENU.VVOD_SPISOK_add_MD(gettext("0% НДС реализация на територии Украины"));
      MENU.VVOD_SPISOK_add_MD(gettext("0% НДС экспорт."));
      MENU.VVOD_SPISOK_add_MD(gettext("Освобождение от НДС"));
      MENU.VVOD_SPISOK_add_MD(gettext("7% НДС на мед.препараты"));
            
      while(menu3w(stdscr,&MENU,&lnds,-1,-1,0) != 0);
      
      if(lnds == 0)
       {
        sprintf(strsql,"%.f%% %s",pnds,gettext("НДС"));
        VV->VVOD_SPISOK_zapis_data(11,strsql);
       }
      if(lnds == 1)
        VV->VVOD_SPISOK_zapis_data(11,gettext("0% НДС реализация на територии Украины"));
      if(lnds == 2)
        VV->VVOD_SPISOK_zapis_data(11,gettext("0% НДС экспорт"));
      if(lnds == 3)
        VV->VVOD_SPISOK_zapis_data(11,gettext("Освобождение от НДС"));
      if(lnds == 4)
        VV->VVOD_SPISOK_zapis_data(11,gettext("7% НДС на мед.препараты"));
      goto nazad;
     }

    if((tipz == 2 && N == 10) || (tipz == 1 && N == 13)) /*Форма оплаты*/
     {
      if((i1=vvodkod("Foroplat",&kod,&naik,-1,-1,0)) == 0)
       {
        if(tipz == 1)  
         VV->data_plus(13,"");
        if(tipz == 2)  
         VV->data_plus(10,"");
        goto nazad;
       }
      sprintf(strsql,"%s|%s",kod.ravno(),naik.ravno());
      if(tipz == 1)
        VV->VVOD_SPISOK_zapis_data(13,strsql);
      if(tipz == 2)
        VV->VVOD_SPISOK_zapis_data(10,strsql);
      if(i1 > 1)
       {
//        if(tipz == 2)
         N++;
        goto nazad;
       }
     }

    if(tipz == 2 && N == 14) /*НДС*/
     {
      attroff(VV->VVOD_return_cs(iceb_CFV));
      MENU.VVOD_delete();
      sprintf(strsql,"%.f%% %s",pnds,gettext("НДС"));
      MENU.VVOD_SPISOK_add_MD(strsql);
      MENU.VVOD_SPISOK_add_MD(gettext("0% НДС реализация на територии Украины"));
      MENU.VVOD_SPISOK_add_MD(gettext("0% НДС экспорт"));
      MENU.VVOD_SPISOK_add_MD(gettext("Освобождение от НДС статья"));
      MENU.VVOD_SPISOK_add_MD(gettext("7% НДС на мед.препараты"));
      
      while(menu3w(stdscr,&MENU,&lnds,-1,-1,0) != 0);
      
      if(lnds == 0)
       {
        sprintf(strsql,"%.f%% %s",pnds,gettext("НДС"));
        VV->VVOD_SPISOK_zapis_data(14,strsql);
       }
      if(lnds == 1)
        VV->VVOD_SPISOK_zapis_data(14,gettext("0% НДС реализация на територии Украины"));
      if(lnds == 2)
        VV->VVOD_SPISOK_zapis_data(14,gettext("0% НДС экспорт"));
      if(lnds == 3)
        VV->VVOD_SPISOK_zapis_data(14,gettext("Освобождение от НДС"));
      if(lnds == 4)
        VV->VVOD_SPISOK_zapis_data(14,gettext("7% НДС на мед.препараты"));
      goto nazad;
     }

    copiq.new_plus(VV->VVOD_SPISOK_return_data(N));

    VV->vgetstr(N,stdscr);
    
    attroff(VV->VVOD_return_cs(iceb_CFV));
    attron(VV->VVOD_return_cs(iceb_CFM));

    if(N == 0)  /*Код организации*/
     {
      attroff(VV->VVOD_return_cs(iceb_CFM));
      kpos.new_plus(VV->VVOD_SPISOK_return_data(N));
      skl1=0;
      if(kpos.ravno()[0] == '0' && kpos.ravno()[1] == '0' && kpos.ravno()[2] == '-')
       {
	/*внутреннее перемещение с двойным документом или нет*/
	if(mvnp != 0)
	  mdd=1;

        if(tipz == 1)
         {
          iceb_t_soob(gettext("Двойной документ можно сделать только оформляя расходный документ !"));

	  kpos.new_plus("");
	  naimo.new_plus("");
	  VV->VVOD_SPISOK_zapis_data(N,"");
	  goto nazad;
         }

	if(mdd == 0)
	 {
          iceb_t_soob(gettext("Двойной документ можно сделать только для внутренней операции !"));
	  kpos.new_plus("");
	  naimo.new_plus("");
	  VV->VVOD_SPISOK_zapis_data(N,"");
	  goto nazad;
	 }

	if(polen(kpos.ravno(),bros,sizeof(bros),2,'-') == 0)
	 {
	  skl1=(int)ATOFM(bros);
          sprintf(strsql,"select * from Sklad where kod=%d",skl1);
          if(sql_readkey(&bd,strsql,&row,&cur) != 1)
           {
            sprintf(strsql,gettext("Не найден склад %d в списке складов !"),skl1);
            iceb_t_soob(strsql);
	    skl1=0;
	    naiskl1.new_plus("");
            VV->VVOD_SPISOK_zapis_data(N,"");
	    goto nazad;
           }
	  naiskl1.new_plus(row[1]);
	  if(mdd == 1 && skl1 != 0 && rr == 0 && dat0->ravno()[0] != '\0')
           {
            rsdat(&d,&m,&g,dat0->ravno(),0);
            class iceb_tu_str nomer_dok("");
	    iceb_t_nomnak(g,skl1,&nomer_dok,1,0,0);
            VV->data_plus(7,nomer_dok.ravno());
           }
	 /*
	  printw("\nmdd=%d rr=%d skl1=%d nomdokp=%s\n",mdd,rr,skl,nomdokp);
	  OSTANOV();
	   */
	  N++;
	  goto nazad;
	 }
       }

      if(kpos.ravno()[0] == '0' && kpos.ravno()[1] == '0' && kpos.ravno()[2] == '-')
	bros2.new_plus("00");
      else
	bros2.new_plus(kpos.ravno());

      if(bros2.getdlinna() > 1)
       {
        sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",bros2.ravno());
        if(sql_readkey(&bd,strsql,&row,&cur) != 1)
         {
          sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kpos.ravno());
          iceb_t_soob(strsql);

          kpos.new_plus("");
          naimo.new_plus("");
          VV->VVOD_SPISOK_zapis_data(N,"");

          goto nazad;
         }

        naimo.new_plus(row[0]);
        
        if(tipz == 2)
         {

          VV->VVOD_SPISOK_zapis_data(8,"");
          class iceb_tu_str uslprod("");

          if(VV->data_ravno(9)[0] == '\0')
           if(iceb_t_getuslp(bros2.ravno(),&uslprod) == 0)
            VV->data_plus(9,uslprod.ravno());           

          if(VV->data_ravno(8)[0] == '\0')
           if(iceb_t_getuslp(bros2.ravno(),&uslprod,1) == 0)
            VV->data_plus(8,uslprod.ravno());           


          N++;
          goto nazad;  
         } 
       }
     }

    if(N == 1)  /*Дата*/
     {
      attroff(VV->VVOD_return_cs(iceb_CFM));
      if(loginrash != 0)
       {
        iceb_t_soob(gettext("К документу распечатан чек ! Дату изменить нельзя !"));
	goto nazad;
       }
      dat0->new_plus(VV->VVOD_SPISOK_return_data(N));
      if(rsdat(&d,&m,&g,dat0->ravno(),0) != 0)
       {
        iceb_t_soob(gettext("Не верно введена дата !"));
	goto nazad;
       }
  
      if(iceb_t_pbpds(m,g) != 0)
       {
        VV->data_plus(N,datazap.ravno());
        goto nazad;
       }
      if(iceb_t_pbpds(datazap.ravno()) != 0)
       {
        VV->data_plus(N,datazap.ravno());
        goto nazad;
       }

      pnds=iceb_t_pnds(d,m,g);

       /*Автоматическая установка номера накладной*/
      if(VV->data_ravno(2)[0] != '\0')
      if(VV->VVOD_SPISOK_return_data(3)[0] == '\0' ||  god_nd != g) /*Если меняется год то меняется и порядок нумерации документов*/
       {
        god_nd=g;
        memset(strsql,'\0',sizeof(strsql));
        rsdat(&d,&m,&g,dat0->ravno(),0);
        iceb_t_nomnak(g,VV->VVOD_SPISOK_return_data(2),strsql,tipz,0,0);
        VV->VVOD_SPISOK_zapis_data(3,strsql);
       }

      if(mdd == 1 && skl1 != 0 && rr == 0 && tipz == 2)
       {
        bros2.new_plus(skl1);
        rsdat(&d,&m,&g,dat0->ravno(),0);
        char nomer_dok[64];
        memset(nomer_dok,'\0',sizeof(nomer_dok));
	iceb_t_nomnak(g,bros2.ravno(),nomer_dok,1,0,0);
        VV->data_plus(7,nomer_dok);
       }

      
      N++;
      goto nazad;
     }

    if(N == 2) /*Склад*/
     {
      if(VV->VVOD_SPISOK_return_data(N)[0] == '\0')
        goto nazad;
      attroff(VV->VVOD_return_cs(iceb_CFM));

      sprintf(strsql,"select naik from Sklad where kod=%s",VV->VVOD_SPISOK_return_data(N));
      if((i=sql_readkey(&bd,strsql,&row,&cur)) != 1)
       {
        sprintf(strsql,gettext("Нет склада %d в списке складов !"),atoi(VV->VVOD_SPISOK_return_data(N)));
        iceb_t_soob(strsql);

        naiskl.new_plus("");
        VV->VVOD_SPISOK_zapis_data(N,"");
        goto nazad;
       }

      if(atoi(VV->VVOD_SPISOK_return_data(2)) == skl1)
       {
        if(mvnp == 1)
         {

          DANET.VVOD_delete();
          DANET.VVOD_SPISOK_add_MD(gettext("Код входного и выходного склада равны !"));
          DANET.VVOD_SPISOK_add_MD(gettext("Это правильно ? Вы уверены ?"));
          if(danet(&DANET,2,stdscr) == 2)
           {
            naiskl.new_plus("");
            VV->VVOD_SPISOK_zapis_data(N,"");
            goto nazad;
           }
         }
       }
      if(mvnp == 2 && skl1 != 0)
      if(skl1 != atoi(VV->VVOD_SPISOK_return_data(2)))
       {
        VVOD SOOB(1);
        SOOB.VVOD_SPISOK_add_MD(gettext("Для операции изменения стоимости код входного и выходного склада"));
        sprintf(strsql,"%s",gettext("должны быть одинаковы"));
        SOOB.VVOD_SPISOK_add_MD(strsql);
        soobshw(&SOOB,stdscr,-1,-1,0,1);
        VV->data_plus(2,"");
        goto nazad;
       }

      naiskl.new_plus(row[0]);

      /*Автоматическая установка номера накладной*/
      rsdat(&d,&m,&g,dat0->ravno(),0);
      
      iceb_t_nomnak(g,VV->VVOD_SPISOK_return_data(N),strsql,tipz,0,0);
      VV->data_plus(3,strsql);

      N++;
      goto nazad;

     }

    if(N == 3)  /*Ном. накладной*/
     {
      /*Проверяем есть ли такой номер документа*/
      if(SRAV(copiq.ravno(),VV->VVOD_SPISOK_return_data(N),0) != 0)
       {
        g=gt;
        if(VV->VVOD_SPISOK_return_data(1)[0] != '\0') 
           rsdat(&d,&m,&g,VV->VVOD_SPISOK_return_data(1), 0);

        //Уникальный номер документа для прихода и расхода
        sprintf(strsql,"select nomd from Dokummat where \
datd >= '%d-01-01' and datd <= '%d-12-31' and sklad=%s and nomd='%s'",
        g,g,VV->VVOD_SPISOK_return_data(2),VV->VVOD_SPISOK_return_data(N));

//        printw("%s\n",strsql);
        
        if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
         {
          attroff(VV->VVOD_return_cs(iceb_CFM));

          sprintf(strsql,gettext("С номером %s документ уже есть !"),VV->VVOD_SPISOK_return_data(N));
          iceb_t_soob(strsql);
  	  goto nazad;
         }
       }
     }

    if( N == 7) /*Накладная поставщика*/
     {
      if(tipz == 2 && mdd == 1)
       {
	/*Проверяем есть ли такой номер документа*/
        sprintf(strsql,"select nomd from Dokummat where tip=%d and \
datd >= '%04d-01-01' and datd <= '%d-12-31' and sklad=%d and nomd='%s'",
        tipz,gt,gt,skl1,VV->VVOD_SPISOK_return_data(N));
        if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
         {
          attroff(VV->VVOD_return_cs(iceb_CFM));

          sprintf(strsql,gettext("С номером %s документ уже есть !"),VV->VVOD_SPISOK_return_data(N));
          iceb_t_soob(strsql);

          VV->VVOD_SPISOK_zapis_data(N,"");
	  goto nazad;
	 }
       }
     }

    if(tipz == 2 && N == 11) /*Номер налоговой накладной */
     {                  
      /*Проверяем есть ли такой номер документа*/
      if(SRAV(nomnn.ravno(),VV->VVOD_SPISOK_return_data(N),0) != 0 && VV->VVOD_SPISOK_return_data(N)[0] != '\0')
       {
        g=gt;
        m=mt;
        if(dat0->ravno()[0] != '\0') 
           rsdat(&d,&m,&g,dat0->ravno(),0);
        if(VV->data_ravno(0)[0] != '\0')
           rsdat(&d,&m,&g,VV->data_ravno(1),1);
        if(provnomnnak(m,g,VV->VVOD_SPISOK_return_data(N)) != 0)
         {
          VV->VVOD_SPISOK_zapis_data(N,nomnn.ravno());
          goto nazad;
         }        
       }

      nomnn.new_plus(VV->VVOD_SPISOK_return_data(N));
     }

    if(tipz == 1)
     {                  
      if(N == 8) /*Номер налоговой накладной */
       {    
        if(iceb_t_pnnnp(VV->data_ravno(10),VV->data_ravno(9),VV->data_ravno(0),VV->data_ravno(8),VV->data_ravno(1),VV->data_ravno(3),1) != 0)
         goto nazad;
       }

      if(N == 10) /*Дата получения налоговой накладной */
       {    
        if(iceb_t_pnnnp(VV->data_ravno(10),VV->data_ravno(9),VV->data_ravno(0),VV->data_ravno(8),VV->data_ravno(1),VV->data_ravno(3),1) != 0)
         goto nazad;

        if(VV->data_ravno(9)[0] == '\0') /*дата выписки налоговой накладной*/
         VV->data_plus(9,VV->data_ravno(10));
       }
     }


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

  if(K == ESC || K == FK10)
   {
    attroff(VV->VVOD_return_cs(iceb_CFS));
    attron(VV->VVOD_return_cs(iceb_CB));
    return(1);
   }

  if(K == FK1) /*Помощь*/
   {
    attroff(VV->VVOD_return_cs(iceb_CFS));
    attron(VV->VVOD_return_cs(iceb_CB));
    GDITE();
    iceb_t_pdoc("matu2_1_1.txt");
    goto nazad;
   }

  if(K == PLU || K == FK2) /*Продолжать*/
   {
    attroff(VV->VVOD_return_cs(iceb_CFS));
    attron(VV->VVOD_return_cs(iceb_CFM));
    if(VV->VVOD_SPISOK_return_data(0)[0] == '\0' || VV->VVOD_SPISOK_return_data(1)[0] == '\0' || VV->VVOD_SPISOK_return_data(2)[0] == '\0' ||
     VV->VVOD_SPISOK_return_data(3)[0] == '\0')
     {
      attroff(VV->VVOD_return_cs(iceb_CFM));
      iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
      goto nazad;
     }

    if(mdd == 1 && VV->VVOD_SPISOK_return_data(7)[0] == '\0')
     {
      attroff(VV->VVOD_return_cs(iceb_CFM));
      iceb_t_soob(gettext("Не введён номер встречного документа !"));
      goto nazad;
     }
    if(rsdat(&d,&m,&g,VV->VVOD_SPISOK_return_data(1),0) != 0)
     {
      attroff(VV->VVOD_return_cs(iceb_CFM));
      iceb_t_soob(gettext("Не верно введена дата !"));
      goto nazad;
     }

    if(tipz == 1)
     {
      if(VV->VVOD_SPISOK_return_data(8)[0] != '\0' && VV->VVOD_SPISOK_return_data(10)[0] == '\0')
       {
        attroff(VV->VVOD_return_cs(iceb_CFM));
        iceb_t_soob(gettext("Не введена дата получения налоговой накладной !"));
        goto nazad;

       }
      if(VV->VVOD_SPISOK_return_data(10)[0] != '\0')
      if(rsdat(&dpnn,&mpnn,&gpnn,VV->VVOD_SPISOK_return_data(10),0) != 0)
       {
        attroff(VV->VVOD_return_cs(iceb_CFM));
        iceb_t_soob(gettext("Не верно введена дата получения налоговой накладной !"));
        goto nazad;
       }
     }

    if(tipz == 2)
     {
      if(VV->VVOD_SPISOK_return_data(11)[0] != '\0' && VV->VVOD_SPISOK_return_data(15)[0] == '\0')
       {
        attroff(VV->VVOD_return_cs(iceb_CFM));
        iceb_t_soob(gettext("Не введена дата выдачи налоговой накладной !"));
        goto nazad;

       }
      if(VV->data_ravno(15)[0] != '\0')
      if(rsdat(&dpnn,&mpnn,&gpnn,VV->VVOD_SPISOK_return_data(15),0) != 0)
       {
        attroff(VV->VVOD_return_cs(iceb_CFM));
        iceb_t_soob(gettext("Не верно введена дата выдачи налоговой накладной !"));
        goto nazad;
       }
      }
     rsdat(&d,&m,&g,VV->VVOD_SPISOK_return_data(1),1);
     if(iceb_t_pbpds(m,g) != 0)
      goto nazad;

     if(mvnp == 2 && skl1 != 0)
     if(skl1 != atoi(VV->VVOD_SPISOK_return_data(2)))
      {
       VVOD SOOB(1);
       SOOB.VVOD_SPISOK_add_MD(gettext("Для операции изменения стоимости код входного и выходного склада"));
       SOOB.VVOD_SPISOK_add_MD(gettext("должны быть одинаковы"));
       soobshw(&SOOB,stdscr,-1,-1,0,1);
       goto nazad;
      }

     if(rr == 1) //Корректировка, проверяем блокировку документа
      {
       rsdat(&d,&m,&g,datazap.ravno(),1);
       if(iceb_t_pbpds(m,g) != 0)
        goto nazad;

       sprintf(strsql,"select sodz from Dokummat2 where god=%d and \
sklad=%d and nomd='%s' and nomerz=12",g,atoi(VV->VVOD_SPISOK_return_data(2)),nomdokz.ravno());
       if(sql_readkey(&bd,strsql,&row,&cur) > 0)
        {
         sprintf(strsql,"%s %s",gettext("Документ заблокирован"),iceb_t_kszap(row[0]));
         iceb_t_soob(strsql);
         goto nazad;

        }
      }      
     break;
   }


  if(K == FK3) /*Посмотреть реквизиты*/
   {
    attroff(VV->VVOD_return_cs(iceb_CFS)); 

    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов")); //0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка форм оплат"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка складов"));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка договоров"));//4
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//5

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    class iceb_tu_str kod_v("");
    class iceb_tu_str naim_v("");
    switch (kom1)
     {
      case 5:
      case -1:
       goto nazad;

      case 0:

       bros1.new_plus("");
       bros2.new_plus("");
      
       clear();
       naimkontr.new_plus("");
       if(dirkontr(&kodkontr,&naimkontr,1) == 0)
        {
         skl1=0;
         kpos.new_plus(kodkontr.ravno());
         naimo.new_plus(naimkontr.ravno());
         VV->VVOD_SPISOK_zapis_data(0,kodkontr.ravno());
         goto nazad;
        }
       goto nazad;

      case 1:

        clear();
        skll.new_plus("");
        naik.new_plus("");
        i=mvnp;
        poz=komv=0;
        if(dirmat1("Foroplat",&skll,&naik,LINES-5,0,2,1,&poz,&komv,"","\0") == 0)
         {
          sprintf(strsql,"%s|%s",skll.ravno(),naik.ravno());
          if(tipz == 1)
           VV->VVOD_SPISOK_zapis_data(13,strsql);
          if(tipz == 2)
           VV->VVOD_SPISOK_zapis_data(10,strsql);
         }
        mvnp=i;
        goto nazad;

      case 2:

        bros1.new_plus("");
        kod.new_plus("");
        naik.new_plus("");
        i=mvnp;
        if(tipz == 1)
          bros1.new_plus("Prihod");
        if(tipz == 2)
          bros1.new_plus("Rashod");
          
        poz=komv=0;
        clear();
        if(dirmat1(bros1.ravno(),&kod,&naik,LINES-5,0,3,1,&poz,&komv,"","\0") == 0)
         {
          kprr->new_plus(kod.ravno());
          naimpr.new_plus(naik.ravno());  
         }
        else
          mvnp=i;
          
        goto nazad;

      case 3:

       if(dirsklad(2,&kod_v,&naim_v) == 0)       
        {
         if(sklz != 0)
          {
           attroff(VV->VVOD_return_cs(iceb_CFV));
           VVOD SOOB(1);
           SOOB.VVOD_SPISOK_add_MD(gettext("Склад изменить нельзя !"));
           SOOB.VVOD_SPISOK_add_MD(gettext("Нужно удалить документ и ввести с нужным складом !"));
           soobshw(&SOOB,stdscr,-1,-1,0,1);
           goto nazad;
          }
      
         naiskl.new_plus(naim_v.ravno());       
         
         VV->VVOD_SPISOK_zapis_data(2,kod_v.ravno());
         
         rsdat(&d,&m,&g,dat0->ravno(),0);
         class iceb_tu_str nomer_dok("");         
         iceb_t_nomnak(g,kod_v.ravno(),&nomer_dok,tipz,0,0); /*Автоматическая установка номера накладной*/
         VV->data_plus(3,nomer_dok.ravno());

        }
       mvnp=i;
       goto nazad;

      case 4:
       if(tipz == 1)
        {
         iceb_t_soob(gettext("Это нужно только для расходных документов!"));
         goto nazad;
        }
       if((un_nom_zap=iceb_t_dirdog(1,VV->data_ravno(0))) >= 0)
        {
         class iceb_tu_str uslprod("");
         if(iceb_t_getuslp(un_nom_zap,&uslprod) == 0)
          VV->data_plus(9,uslprod.ravno());
         if(iceb_t_getuslp(un_nom_zap,&uslprod,1) == 0)
          VV->data_plus(8,uslprod.ravno());
        }       
       mvnp=i;
       goto nazad;
      }

    }

  if(K == FK4) /*Определить номер документа*/
   {
    attroff(VV->VVOD_return_cs(iceb_CFS)); 

    if(VV->VVOD_SPISOK_return_data(2)[0] == '\0')
     {
      iceb_t_soob(gettext("Не введён код склада !"));
      goto nazad;
     }

    /*Автоматическая установка номера накладной*/

    rsdat(&d,&m,&g,VV->VVOD_SPISOK_return_data(1),0);
    char nomer_dok[64];
    iceb_t_nomnak(g,VV->VVOD_SPISOK_return_data(2),nomer_dok,tipz,0,0);
    VV->data_plus(3,nomer_dok);

    if(mdd == 1 && skl1 != 0 && rr == 0 && tipz == 2)
     {
      bros2.new_plus(skl1);
      rsdat(&d,&m,&g,VV->VVOD_SPISOK_return_data(1),0);
      class iceb_tu_str nomer_dok("");
      iceb_t_nomnak(g,bros2.ravno(),&nomer_dok,1,0,0);
      VV->data_plus(7,nomer_dok.ravno());
     }

    goto nazad;
   }
  
  if(K == SFK4 && tipz == 2) //Определить номер налоговой накладной
   {
    attroff(VV->VVOD_return_cs(iceb_CFS)); 

    g=gt;
    m=mt;
    if(dat0->ravno()[0] != '\0') 
       rsdat(&d,&m,&g,dat0->ravno(),0);
    if(VV->data_ravno(1)[0] != '\0')
     rsdat(&d,&m,&g,VV->data_ravno(1),1);   
    sprintf(strsql,"%d",nomnalnak(m,g));

    VV->VVOD_SPISOK_zapis_data(11,strsql);    
    goto nazad;
   }  

  if(K == FK5) /*ввод кода контрагента реквизиты которого будут взяты в распечатки*/
   {
    attroff(VV->VVOD_return_cs(iceb_CFS)); 
    iceb_t_vk00(&k00);
    goto nazad;
   }
 }
attroff(VV->VVOD_return_cs(iceb_CFS));
attron(VV->VVOD_return_cs(iceb_CB));

if(zapdok(VV,tipz,kprr->ravno(),skl1,lnds,datazap.ravno(),nomdokz.ravno(),kontrz.ravno(),rr,sumkor,loginrash,metkapr,nom_pd.ravno(),k00.ravno()) != 0)
 goto nazad;

return(0);
}
/***********************************/
//Создание списков нужного размера
/**********************************/
void	sozvdnd(int tipz,VVOD *VV)
{
if(tipz == 1)
 {
  VV->VVOD_SPISOK_add_data(10); //0
  VV->VVOD_SPISOK_add_data(11); //1
  VV->VVOD_SPISOK_add_data(10); //2
  VV->VVOD_SPISOK_add_data(20); //3
  VV->VVOD_SPISOK_add_data(80-13); //4
  VV->VVOD_SPISOK_add_data(11); //5
  VV->VVOD_SPISOK_add_data(80); //6
  VV->VVOD_SPISOK_add_data(20); //7

  VV->VVOD_SPISOK_add_data(20); //8
  VV->VVOD_SPISOK_add_data(11); //9
  VV->VVOD_SPISOK_add_data(11); //10
  VV->VVOD_SPISOK_add_data(64); //11
  VV->VVOD_SPISOK_add_data(11); //12
  VV->VVOD_SPISOK_add_data(80); //13
  VV->VVOD_SPISOK_add_data(80); //14


 }
if(tipz == 2)
 {
  VV->VVOD_SPISOK_add_data(10); //0
  VV->VVOD_SPISOK_add_data(11); //1
  VV->VVOD_SPISOK_add_data(10);  //2
  VV->VVOD_SPISOK_add_data(20); //3
  VV->VVOD_SPISOK_add_data(80-13); //4
  VV->VVOD_SPISOK_add_data(11); //5
  VV->VVOD_SPISOK_add_data(80); //6
  VV->VVOD_SPISOK_add_data(20); //7
  VV->VVOD_SPISOK_add_data(80); //8
  VV->VVOD_SPISOK_add_data(80); //9
  VV->VVOD_SPISOK_add_data(64); //10
  VV->VVOD_SPISOK_add_data(40); //11
  VV->VVOD_SPISOK_add_data(11); //12
  VV->VVOD_SPISOK_add_data(40); //13
  VV->VVOD_SPISOK_add_data(64); //14
  VV->VVOD_SPISOK_add_data(11); //15

 }
}
