/* $Id: vdnu.c,v 5.70 2013/10/04 07:25:03 sasa Exp $ */
/*26.06.2013	15.02.2000	Белых А.И.	vdnu.c
Работа с шапкой документа на услуги
Если вернули 0 - сделали запись
	     1 - нет
*/
#include	"buhg.h"


int zapdusl(VVOD*,short,int,int,const char*,const char*,const char*,const char*,const char *k00);

extern class iceb_tu_str bndsu1; /*0% НДС реализация на територии Украины.*/
extern class iceb_tu_str bndsu2; /*0% НДС экспорт.*/
extern class iceb_tu_str bndsu3; /*Освобождение от НДС статья 5.*/

int vdnu(short *dend,short *mesd,short *godd,
int *podrd,
class iceb_tu_str *nomdok,
int tipz) 
{
static class iceb_tu_str datazap("");
int un_nom_zap=0;
int lnds=0;//*Метка ндс документа*/
SQL_str         row;
char		strsql[2048];
int             i,i1;
short           K;
int             N;
short god_nd=*godd; /*Год порядка нумерации документа*/
class iceb_tu_str naimkontr("");
class iceb_tu_str naimpodr("");
class iceb_tu_str naimoper("");
class iceb_tu_str nomdokz("");
class iceb_tu_str datz("");
class iceb_tu_str podrz("");
short		d,m,g;
int		kom1;
class iceb_tu_str kod("");
int		poz,komv;
class iceb_tu_str nomnn("");
class iceb_tu_str kontrz("");
short		dpnn=0,mpnn=0,gpnn=0;
short dz=0,mz=0,gz=0;
SQLCURSOR curr;
short dt,mt,gt;
class iceb_tu_str kodv("");
class iceb_tu_str naimv("");
float pnds=0.;
class iceb_tu_str k00("00");

poltekdat(&dt,&mt,&gt);

if(*dend == 0)
 pnds=iceb_t_pnds(dt,mt,gt);
else
 pnds=iceb_t_pnds(*dend,*mesd,*godd);

VVOD VV(0);
VVOD MENU(3);
VVOD VVOD1(2);

VV.VVOD_SPISOK_add_data(10);//0
VV.VVOD_SPISOK_add_data(11);//1
VV.VVOD_SPISOK_add_data(5); //2
VV.VVOD_SPISOK_add_data(20);//3
VV.VVOD_SPISOK_add_data(20);//4
VV.VVOD_SPISOK_add_data(80);//5
VV.VVOD_SPISOK_add_data(40);//6
VV.VVOD_SPISOK_add_data(40);//7
VV.VVOD_SPISOK_add_data(11);//8
VV.VVOD_SPISOK_add_data(40);//9
VV.VVOD_SPISOK_add_data(80);//10
VV.VVOD_SPISOK_add_data(20);//11
VV.VVOD_SPISOK_add_data(11);//12
VV.VVOD_SPISOK_add_data(11);//13
VV.VVOD_SPISOK_add_data(40);//14
VV.VVOD_SPISOK_add_data(11);//15
VV.VVOD_SPISOK_add_data(80);//16

if(*dend != 0)
 {
  sprintf(strsql,"select * from Usldokum where datd='%04d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d",
  *godd,*mesd,*dend,*podrd,nomdok->ravno(),tipz);
  if(readkey(strsql,&row,&curr) != 1)
   {
    sprintf(strsql,"%s %d.%d.%d %d %s %d !",gettext("Не найдена шапка документа"),*dend,*mesd,*godd,*podrd,nomdok->ravno(),tipz);
    iceb_t_soob(strsql);
    return(1);
    
   }

  VV.data_plus(0,row[3]);
  sprintf(strsql,"%d.%d.%d",*dend,*mesd,*godd);
  VV.data_plus(1,strsql);
  VV.data_plus(2,*podrd);
  VV.data_plus(3,row[4]);
  VV.data_plus(4,row[6]);
  VV.data_plus(5,row[10]);
  VV.data_plus(6,row[8]);
  VV.data_plus(7,row[5]);
  VV.data_plus(8,iceb_tu_datzap(row[9]));
  lnds=atoi(row[7]);
  if(lnds == 0)
   {
    sprintf(strsql,"%.f%% %s",pnds,gettext("НДС"));
    VV.data_plus(9,strsql);
   }
  if(lnds == 1)
     VV.data_plus(9,gettext("0% НДС реализация на територии Украины"));
  if(lnds == 2)
     VV.data_plus(9,gettext("0% НДС экспорт"));
  if(lnds == 3)
     VV.data_plus(9,gettext("Освобождение от НДС"));

  VV.data_plus(10,row[23]);

  VV.data_plus(11,row[16]);

  VV.data_plus(12,iceb_tu_datzap(row[17]));

  VV.data_plus(13,iceb_tu_datzap(row[24]));
  VV.data_plus(14,row[18]);

  VV.data_plus(15,iceb_tu_datzap(row[19]));
  VV.data_plus(16,row[20]);
  k00.new_plus(row[28]);
  
 }
else
 {
  if(datazap.getdlinna() <= 1)
   {
    sprintf(strsql,"%d.%d.%d",dt,mt,gt);    
    datazap.new_plus(strsql);
   }
  VV.data_plus(1,datazap.ravno());
 }
 
/*
printw("vdnu-%d %d\n",rr,lnds);
refresh();
*/
N=0;
nomdokz.new_plus(VV.VVOD_SPISOK_return_data(3));
datz.new_plus("");

kontrz.new_plus(VV.VVOD_SPISOK_return_data(0));

if(*dend != 0)
 {
  datz.new_plus(VV.VVOD_SPISOK_return_data(1));
  rsdat(&dz,&mz,&gz,datz.ravno(),1);
 }

podrz.new_plus(VV.VVOD_SPISOK_return_data(2));
nomnn.new_plus(VV.VVOD_SPISOK_return_data(7));

if(kontrz.getdlinna() > 1)
 {
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontrz.ravno());
  if(readkey(strsql,&row,&curr) == 1)
    naimkontr.new_plus(row[0]);
 }
if(podrz.getdlinna() > 1)
 {
  sprintf(strsql,"select naik from Uslpodr where kod=%s",podrz.ravno());
  if(readkey(strsql,&row,&curr) > 0)
    naimpodr.new_plus(row[0]);
 }

if(VV.VVOD_SPISOK_return_data(4)[0] != '\0')
 {
  if(tipz == 1)
   sprintf(strsql,"select naik,vido from Usloper1 where kod='%s'",
   VV.VVOD_SPISOK_return_data(4));
  if(tipz == 2)
   sprintf(strsql,"select naik,vido from Usloper2 where kod='%s'",
   VV.VVOD_SPISOK_return_data(4));

  if(readkey(strsql,&row,&curr) > 0)
   {
    naimoper.new_plus(row[0]);
   }
 }


VV.VVOD_SPISOK_add_MD(gettext("Код контрагента............."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата........................"));//1
VV.VVOD_SPISOK_add_MD(gettext("Подразделение..............."));//2
VV.VVOD_SPISOK_add_MD(gettext("Номер документа............."));//3
VV.VVOD_SPISOK_add_MD(gettext("Код операции................"));//4
VV.VVOD_SPISOK_add_MD(gettext("Вид договора................"));//5
VV.VVOD_SPISOK_add_MD(gettext("Форма оплаты................"));//6
VV.VVOD_SPISOK_add_MD(gettext("Номер налоговой накладной..."));//7
VV.VVOD_SPISOK_add_MD(gettext("Дата оплаты................."));//8
VV.VVOD_SPISOK_add_MD(gettext("НДС........................."));//9
VV.VVOD_SPISOK_add_MD(gettext("Основание..................."));//10
VV.VVOD_SPISOK_add_MD(gettext("Номер документа поставщика.."));//11
VV.VVOD_SPISOK_add_MD(gettext("Дата выписки нал. накладной."));//12
if(tipz == 1)
 VV.VVOD_SPISOK_add_MD(gettext("Дата пол-ния нал. накладной."));//13
if(tipz == 2)
 VV.VVOD_SPISOK_add_MD(gettext("Дата видачи нал. накладной.."));//13
VV.VVOD_SPISOK_add_MD(gettext("Доверенность................"));//14
VV.VVOD_SPISOK_add_MD(gettext("Дата выдачи доверенности...."));//15
VV.VVOD_SPISOK_add_MD(gettext("Через кого.................."));//16

if(*dend == 0)
 {

  /*Если в списке подразделений 1 подразделение то сразу
    записываем код*/
  sprintf(strsql,"select kod,naik from Uslpodr");
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
   {
    VV.data_plus(2,row[0]);
    naimpodr.new_plus(row[1]); 
   }
  /*Если в списке операций 1-на операция то сразу
    записываем код*/
  if(tipz == 1)
    sprintf(strsql,"select kod,naik from Usloper1");
  if(tipz == 2)
    sprintf(strsql,"select kod,naik from Usloper2");

  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
   {
    VV.data_plus(4,row[0]);
    naimoper.new_plus(row[1]); 
   }

  lnds=0;
nazad1:;
  if(proverka(bndsu1.ravno(),VV.VVOD_SPISOK_return_data(4),0,1) == 0 || bndsu1.ravno()[0] == '*')
   lnds=1;
  if(proverka(bndsu2.ravno(),VV.VVOD_SPISOK_return_data(4),0,1) == 0 || bndsu2.ravno()[0] == '*')
   lnds=2;
  if(proverka(bndsu3.ravno(),VV.VVOD_SPISOK_return_data(4),0,1) == 0 || bndsu3.ravno()[0] == '*')
       lnds=3;

  if(lnds == 0)
   {
    sprintf(strsql,"%.f%% %s",pnds,gettext("НДС"));
    VV.data_plus(9,strsql);
   }   
  if(lnds == 1)
    VV.data_plus(9,gettext("0% НДС реализация на територии Украины"));
  if(lnds == 2)
    VV.data_plus(9,gettext("0% НДС экспорт"));
  if(lnds == 3)
    VV.data_plus(9,gettext("Освобождение от НДС"));
  
 }

nazad:;

clear();

attron(A_BLINK);
if(tipz == 1)
  printw(gettext("ПРИХОД"));
if(tipz == 2)
  printw(gettext("РАСХОД"));
attroff(A_BLINK);
printw(" %s",k00.ravno());


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("реквизиты"),
"F4",gettext("номер докум."),
"F5",gettext("конт.00"),
"F10",gettext("выход"),NULL);


VV.vramka(4,0,0);

attron(VV.VVOD_return_cs(iceb_CFS)); 

VV.vprintw(N,stdscr);

for(;;)
 {
  if(naimkontr.ravno()[0] != '\0')
   {
    move(1,0);
    printw("%s:%.*s",gettext("Контрагент"),
    iceb_tu_kolbait(40,naimkontr.ravno()),
    naimkontr.ravno());
   }
  if(naimpodr.getdlinna() > 1)
   {
    move(2,0);
    printw("%s:%.40s",gettext("Подразделение"),naimpodr.ravno());
   }
  if(naimoper.getdlinna() > 1)
   {
    move(3,0);
    printw("%s:%.40s",gettext("Операция"),naimoper.ravno());
   }

  refresh();

  K=getch();

  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);
  if(K == KEY_RESIZE)
    goto nazad;
  if(K == TAB && N == 0)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));

    VVOD1.VVOD_delete();
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите наименование контрагента для поиска"));
    naimv.new_plus("");
    if((kom1=vvod1(&naimv,40,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
      goto nazad;
    if(kom1 == ESC)
      goto nazad;

    
    clear();
    if(dirkontr(&kodv,&naimv,1) == 0)
     {
      naimkontr.new_plus(naimv.ravno());
      VV.data_plus(0,kodv.ravno());
     }
    goto nazad;
   }

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));
    VV.vprintw_clear(N,stdscr);

    if(N == 4) /*Код операции*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFV));
      if(tipz == 1)
        strcpy(strsql,"Usloper1");
      if(tipz == 2)
        strcpy(strsql,"Usloper2");
      N++;
      class iceb_tu_str naik("");
      if(tipz == 1)
      if((i1=vvodkod(strsql,&kod,&naik,-1,-1,0)) == 0)
       {
        goto nazad;
       }
      if(tipz == 2)
       if(diruslop2(2,&kod,&naik) != 0)
        goto nazad;
      VV.data_plus(4,kod.ravno());
      naimoper.new_plus(naik.ravno());
      goto nazad1;

     }

    if(N == 6) /*Форма оплаты*/
     {
      class iceb_tu_str naik("");
      if((i1=vvodkod("Foroplat",&kod,&naik,-1,-1,0)) == 0)
       {
        VV.data_plus(6,"");
        N++;
        goto nazad;
       }
      sprintf(strsql,"%s|%s",kod.ravno(),naik.ravno());
      VV.data_plus(6,strsql);
      if(i1 > 1)
       {
        N++;
        goto nazad1;
       }      
     }
     
    if(N == 9) /*НДС*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFV));
      MENU.VVOD_delete();

      sprintf(strsql,"%.f%% %s",pnds,gettext("НДС"));

      MENU.VVOD_SPISOK_add_MD(strsql);
      MENU.VVOD_SPISOK_add_MD(gettext("0% НДС реализация на територии Украины"));
      MENU.VVOD_SPISOK_add_MD(gettext("0% НДС экспорт"));
      MENU.VVOD_SPISOK_add_MD(gettext("Освобождение от НДС"));

      lnds=0;
      while(menu3w(stdscr,&MENU,&lnds,-1,-1,0) != 0);

      if(lnds == 0)
       {
        sprintf(strsql,"%.f%% %s",pnds,gettext("НДС"));
        VV.data_plus(9,strsql);
       }
      if(lnds == 1)
        VV.data_plus(9,gettext("0% НДС реализация на територии Украины"));
      if(lnds == 2)
        VV.data_plus(9,gettext("0% НДС экспорт"));
      if(lnds == 3)
        VV.data_plus(9,gettext("Освобождение от НДС"));
      goto nazad;
     }

    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 0)  /*Код организации*/
     {
      if(VV.VVOD_SPISOK_return_data(N)[0] != '\0')
       {
        sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
        VV.VVOD_SPISOK_return_data(N));
        if(sql_readkey(&bd,strsql,&row,&curr) != 1)
         {
          attroff(VV.VVOD_return_cs(iceb_CFM));

          sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),VV.VVOD_SPISOK_return_data(N));
          iceb_t_soob(strsql);

          naimkontr.new_plus("");
          VV.data_plus(N,"");
           
          goto nazad;
         }
        naimkontr.new_plus(row[0]);
        class iceb_tu_str uslprod("");

        if(VV.data_ravno(10)[0] == '\0')
         if(iceb_t_getuslp(VV.data_ravno(0),&uslprod,1) == 0)
          VV.data_plus(10,uslprod.ravno());           

        if(VV.data_ravno(5)[0] == '\0')
         if(iceb_t_getuslp(VV.data_ravno(0),&uslprod) == 0)
          VV.data_plus(5,uslprod.ravno());           
       
        /*Автоматическая установка номера накладной*/
        if(*dend == 0 && VV.VVOD_SPISOK_return_data(3)[0] == '\0' && VV.VVOD_SPISOK_return_data(2)[0] != '\0' && \
        VV.VVOD_SPISOK_return_data(1)[0] != '\0') 
         {
           rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(1),0);
           char nomer_dok[64];
           iceb_t_nomnak(g,VV.VVOD_SPISOK_return_data(2),nomer_dok,tipz,1,0);
           VV.data_plus(3,nomer_dok);
  	 }
        N++;
        goto nazad; 

       }
     }       

    if(N == 1)  /*Дата*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      if(rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(N),0) != 0)
       {
        iceb_t_soob(gettext("Не верно введена дата !"));
	goto nazad;
       }

      if(iceb_t_pbpds(m,g) != 0)
       {       
        if(*dend != 0)
         {
          sprintf(strsql,"%d.%d.%d",*dend,*mesd,*godd);
          VV.data_plus(N,strsql);
         }
        else
         VV.data_plus(N,"");
        goto nazad;
       }       

      if(iceb_t_pbpds(*mesd,*godd) != 0)
       {       
        if(*dend != 0)
         {
          sprintf(strsql,"%d.%d.%d",*dend,*mesd,*godd);
          VV.data_plus(N,strsql);
         }
        else
         VV.data_plus(N,"");
        goto nazad;
       }       
      pnds=iceb_t_pnds(d,m,g);
      
       /*Автоматическая установка номера накладной*/
      if(VV.data_ravno(2)[0] != '\0')
       if(VV.VVOD_SPISOK_return_data(3)[0] == '\0' || god_nd != g) /*Если изменился год значит нужно установить новый номер документа*/
       {
        god_nd=g;
        rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(N),0);
        char nomer_dok[64];
        iceb_t_nomnak(g,VV.VVOD_SPISOK_return_data(2),nomer_dok,tipz,1,0);
        VV.data_plus(3,nomer_dok);

       }
      N++;
      goto nazad;
     }

    if(N == 2) /*Подразделение*/
     {
      if(VV.VVOD_SPISOK_return_data(N)[0] == '\0')
        goto nazad;
      attroff(VV.VVOD_return_cs(iceb_CFM));

      sprintf(strsql,"select naik from Uslpodr where kod=%s",VV.VVOD_SPISOK_return_data(N));
      if((i=sql_readkey(&bd,strsql,&row,&curr)) != 1)
       {
        sprintf(strsql,"%s %s !",gettext("Не найден код подразделения"),VV.VVOD_SPISOK_return_data(N));
        iceb_t_soob(strsql);

        naimpodr.new_plus("");
        VV.data_plus(N,"");
        goto nazad;
       }
      naimpodr.new_plus(row[0]);

      /*Автоматическая установка номера накладной*/
      if(VV.VVOD_SPISOK_return_data(3)[0] == '\0' && VV.VVOD_SPISOK_return_data(1)[0] != '\0') 
       {
        rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(1),0);
        char nomer_dok[64];
        iceb_t_nomnak(g,VV.VVOD_SPISOK_return_data(2),nomer_dok,tipz,1,0);
        VV.data_plus(3,nomer_dok);
       }

      N++;
      goto nazad;

     }

    if(N == 3)  /*Ном. накладной*/
     {
      /*Проверяем есть ли такой номер документа*/
      if(SRAV(nomdokz.ravno(),VV.VVOD_SPISOK_return_data(N),0) != 0 && VV.VVOD_SPISOK_return_data(2)[0] != '\0')      
       {
        g=gt;
        if(VV.VVOD_SPISOK_return_data(1)[0] != '\0')
          rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(1),0);
        sprintf(strsql,"select nomd from Usldokum where \
datd >= '%04d-01-01' and datd <= '%04d-12-31' and podr=%s and nomd='%s'",
        g,g,VV.VVOD_SPISOK_return_data(2),VV.VVOD_SPISOK_return_data(N));
        if(sql_readkey(&bd,strsql,&row,&curr) >= 1)
         {
          attroff(VV.VVOD_return_cs(iceb_CFM));

          sprintf(strsql,gettext("С номером %s документ уже есть !"),VV.VVOD_SPISOK_return_data(N));
          iceb_t_soob(strsql);
    	  VV.data_plus(N,"");
  	  goto nazad;
         }
       }

     }

    if(N == 7) /*Номер налоговой накладной */
     {                  
      if(tipz == 1)
       {                  
        if(iceb_t_pnnnp(VV.data_ravno(13),VV.data_ravno(12),VV.data_ravno(0),VV.data_ravno(7),VV.data_ravno(1),VV.data_ravno(3),2) != 0)
         goto nazad;
       }

      if(tipz == 2)
       {                  
        /*Проверяем есть ли такой номер документа*/
        if(SRAV(nomnn.ravno(),VV.VVOD_SPISOK_return_data(N),0) != 0 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
         {
          g=gt;
          m=mt;
          if(VV.VVOD_SPISOK_return_data(1)[0] != '\0')
            rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(1),0);
          if(provnomnnak(m,g,VV.VVOD_SPISOK_return_data(N)) != 0)
           {
            VV.data_plus(N,nomnn.ravno());
            goto nazad;
           }
         }
       }

     }

    if( N == 8 && VV.VVOD_SPISOK_return_data(N)[0] != '\0') /*Дата оплаты*/
     {
      if(rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(N),1) != 0)
       {
        attroff(VV.VVOD_return_cs(iceb_CFM));

        iceb_t_soob(gettext("Не верно введена дата !"));

        VV.data_plus(N,"");
        goto nazad;
       }
     }     

    if( N == 13) /*Дата получения-выдачи налоговой накладной*/
     {
      if(tipz == 1)
       {                  
        if(iceb_t_pnnnp(VV.data_ravno(13),VV.data_ravno(12),VV.data_ravno(0),VV.data_ravno(7),VV.data_ravno(1),VV.data_ravno(3),2) != 0)
         goto nazad;
       }
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
    attron(VV.VVOD_return_cs(iceb_CB));
    return(1);
   }
  if( K == FK1)
   {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     attron(VV.VVOD_return_cs(iceb_CFM));
    GDITE();
    iceb_t_pdoc("usl2_1.txt");
    clear();
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

    if(VV.VVOD_SPISOK_return_data(7)[0] != '\0' && VV.VVOD_SPISOK_return_data(13)[0] == '\0')
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      if(tipz == 1)  
        iceb_t_soob(gettext("Не введена дата получения налоговой накладной !"));
      if(tipz == 2)  
        iceb_t_soob(gettext("Не введена дата выдачи налоговой накладной !"));
      goto nazad;

     }

    if(VV.VVOD_SPISOK_return_data(12)[0] != '\0')
    if(rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(12),0) != 0)
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      iceb_t_soob(gettext("Неправильно введена дата виписки !"));
      goto nazad;
     }

    if(VV.VVOD_SPISOK_return_data(13)[0] != '\0')
    if(rsdat(&dpnn,&mpnn,&gpnn,VV.VVOD_SPISOK_return_data(13),0) != 0)
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      if(tipz == 1)
         iceb_t_soob(gettext("Не верно введена дата получения налоговой накладной !"));
      if(tipz == 2)
         iceb_t_soob(gettext("Не верно введена дата выдачи налоговой накладной !"));
      goto nazad;
     }

    short dp,mp,gp;
    dz=mz=gz=0;
    if(datz.getdlinna() > 1)
     {
      rsdat(&dz,&mz,&gz,datz.ravno(),0);
      if(iceb_t_pbpds(mz,gz) != 0)
       goto nazad;
     }
    rsdat(&dp,&mp,&gp,VV.VVOD_SPISOK_return_data(1),0);
    if(iceb_t_pbpds(mp,gp) != 0)
     goto nazad;

    if(mz != 0)
     if(mz != mp || gz != gp)
       if(iceb_t_pvkdd(6,dz,mz,gz,mp,gp,podrz.ravno_atoi(),nomdokz.ravno(),tipz) != 0)
         goto nazad;
         
    break;
   }
  if(K == FK3) /*Посмотреть реквизиты*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS)); 

    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка форм оплат"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка договоров"));//4
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 5:
      case -1:
       goto nazad;

      case 0:


      
       naimv.new_plus("");
       clear();
       if(dirkontr(&kodv,&naimv,1) == 0)
        {
         naimkontr.new_plus(naimv.ravno());
         VV.data_plus(0,kodv.ravno());
        }
       goto nazad;

      case 1:

        clear();
       naimv.new_plus("");
        poz=komv=0;
        if(dirmat1("Foroplat",&kodv,&naimv,LINES-5,0,3,1,&poz,&komv,"","\0") == 0)
         {
          sprintf(strsql,"%s|%s",kodv.ravno(),naimv.ravno());
          VV.data_plus(6,strsql);
         }
        goto nazad;

      case 2:

        naimv.new_plus("");
                    
        poz=komv=0;
        clear();
        if(tipz == 1)
        if(dirmat1("Usloper1",&kodv,&naimv,LINES-5,0,3,1,&poz,&komv,"","\0") == 0)
         {
          
          naimoper.new_plus(naimv.ravno());  
          VV.data_plus(4,kodv.ravno());
          lnds=0;
          if(proverka(bndsu1.ravno(),VV.VVOD_SPISOK_return_data(4),0,1) == 0 || bndsu1.ravno()[0] == '*')
               lnds=1;
          if(proverka(bndsu2.ravno(),VV.VVOD_SPISOK_return_data(4),0,1) == 0 || bndsu2.ravno()[0] == '*')
               lnds=2;
          if(proverka(bndsu3.ravno(),VV.VVOD_SPISOK_return_data(4),0,1) == 0 || bndsu3.ravno()[0] == '*')
               lnds=3;

          if(lnds == 0)
           {
            sprintf(strsql,"%.f%% %s",pnds,gettext("НДС"));
            VV.data_plus(9,strsql);
           }
          if(lnds == 1)
            VV.data_plus(9,gettext("0% НДС реализация на територии Украины"));
          if(lnds == 2)
            VV.data_plus(9,gettext("0% НДС экспорт"));
          if(lnds == 3)
            VV.data_plus(9,gettext("Освобождение от НДС"));
          N++;
         }

        if(tipz == 2)
        if(diruslop2(1,&kodv,&naimv) == 0)
         {
          
          naimoper.new_plus(naimv.ravno());  
          VV.data_plus(4,kodv.ravno());
          lnds=0;
          if(proverka(bndsu1.ravno(),VV.VVOD_SPISOK_return_data(4),0,1) == 0 || bndsu1.ravno()[0] == '*')
               lnds=1;
          if(proverka(bndsu2.ravno(),VV.VVOD_SPISOK_return_data(4),0,1) == 0 || bndsu2.ravno()[0] == '*')
               lnds=2;
          if(proverka(bndsu3.ravno(),VV.VVOD_SPISOK_return_data(4),0,1) == 0 || bndsu3.ravno()[0] == '*')
               lnds=3;
          if(lnds == 0)
           {
            sprintf(strsql,"%.f%% %s",pnds,gettext("НДС"));
            VV.data_plus(9,strsql);
           }
          if(lnds == 1)
            VV.data_plus(9,gettext("0% НДС реализация на територии Украины"));
          if(lnds == 2)
            VV.data_plus(9,gettext("0% НДС экспорт"));
          if(lnds == 3)
            VV.data_plus(9,gettext("Освобождение от НДС"));
          N++;
         }
          
        goto nazad;

      case 3:

       naimv.new_plus("");
       poz=komv=0;
       clear();
       if(dirmat1("Uslpodr",&kodv,&naimv,LINES-5,0,2,1,&poz,&komv,"","\0") == 0)
        {
         naimpodr.new_plus(naimv.ravno());       
         VV.data_plus(2,kodv.ravno());
         rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(1),0);
         class iceb_tu_str nomer_dok("");
         iceb_t_nomnak(g,kodv.ravno(),&nomer_dok,tipz,1,0);   /*Автоматическая установка номера документа*/
         VV.data_plus(3,nomer_dok.ravno());

         if(tipz == 2)   
          {
           sprintf(strsql,"%s N%s %s %s %s",gettext("Счёт"),VV.VVOD_SPISOK_return_data(3),gettext("от"),VV.VVOD_SPISOK_return_data(1),gettext("г.")); 
           VV.data_plus(5,strsql);
          }
        }
       goto nazad;

      case 4:
       if((un_nom_zap=iceb_t_dirdog(1,VV.data_ravno(0))) >= 0)
        {
         class iceb_tu_str uslprod("");
         if(iceb_t_getuslp(un_nom_zap,&uslprod) == 0)
          VV.data_plus(5,uslprod.ravno());
         if(iceb_t_getuslp(un_nom_zap,&uslprod,1) == 0)
          VV.data_plus(10,uslprod.ravno());
        }       
       goto nazad;

      }

    } 
  if(K == FK4) /*Определение номера документа*/
   {
    if(VV.VVOD_SPISOK_return_data(2)[0] != '\0' && VV.VVOD_SPISOK_return_data(1)[0] != '\0')
     {
      rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(1),0);

      class iceb_tu_str nomer_dok("");
      iceb_t_nomnak(g,VV.VVOD_SPISOK_return_data(2),&nomer_dok,tipz,1,0);
      VV.data_plus(3,nomer_dok.ravno());
      goto nazad;
     }
   }
  if(K == SFK4 && tipz == 2) //Определить номер налоговой накладной
   {
    attroff(VV.VVOD_return_cs(iceb_CFS)); 

    g=gt;
    m=mt;
    if(VV.VVOD_SPISOK_return_data(1)[0] != '\0') 
      rsdat(&d,&m,&g,VV.VVOD_SPISOK_return_data(1),0);
    sprintf(strsql,"%d",nomnalnak(m,g));

    VV.VVOD_SPISOK_zapis_data(7,strsql);    
//    printw("\nНомер налоговой накладной=%s\n",strsql);
//    OSTANOV();
    goto nazad;
   }  
  if(K == FK5) /*Ввод кода контрагента реквизиты которого будут братся в распечатки*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS)); 
    iceb_t_vk00(&k00);
    goto nazad;
   }
 }


if(zapdusl(&VV,*dend,tipz,lnds,datz.ravno(),nomdokz.ravno(),podrz.ravno(),kontrz.ravno(),k00.ravno()) != 0)
 goto nazad;

rsdat(dend,mesd,godd,VV.data_ravno(1),1);
*podrd=VV.data_ravno_atoi(2);
nomdok->new_plus(VV.data_ravno(3));
datazap.new_plus(VV.data_ravno(1));

return(0);
}
/*******************/
/*Запись информации*/
/*******************/
int zapdusl(VVOD *VV,short dend, //0-ввод != 0-корректировка
int tipz, //1-приход 2-расход
int lnds, //Метка ндс документа
const char *datz,
const char *nomdokz,
const char *podrz,
const char *kontrz,  //Код контрагента первоначальный
const char *k00) /*код контрагента реквизиты которого берутся в распечатку*/
{
char		strsql[2048];
short		d,m,g;
short		dz,mz,gz;
short		dop,mop,gop;
class iceb_tu_str forop("");
time_t		vrem;
short		prov=0;
SQL_str		row;
SQLCURSOR cur;
short		dpnn=0,mpnn=0,gpnn=0;
short ddov,mdov,gdov;

GDITE();
rsdat(&ddov,&mdov,&gdov,VV->VVOD_SPISOK_return_data(15),1);

dop=mop=gop=0;
rsdat(&d,&m,&g,VV->VVOD_SPISOK_return_data(1),1);
rsdat(&dop,&mop,&gop,VV->VVOD_SPISOK_return_data(8),1);
if(polen(VV->VVOD_SPISOK_return_data(6),&forop,1,'|') != 0)
  forop.new_plus(VV->VVOD_SPISOK_return_data(6));

rsdat(&dpnn,&mpnn,&gpnn,VV->VVOD_SPISOK_return_data(13),1);
short denv=0,mesv=0,godv=0;
rsdat(&denv,&mesv,&godv,VV->data_ravno(12),1);

int metka_opl=0;

//Узнаём нужно ли для этой операции делать проводки
if(tipz == 1)
 sprintf(strsql,"select prov,vido from Usloper1 where kod='%s'",VV->VVOD_SPISOK_return_data(4));
if(tipz == 2)
 sprintf(strsql,"select prov,vido from Usloper2 where kod='%s'",VV->VVOD_SPISOK_return_data(4));

if(readkey(strsql,&row,&cur) == 1)
 if(row[0][0] == '1')
  prov=1;

if(atoi(row[1]) != 0)
 metka_opl=1;

time(&vrem);
float nds=iceb_t_pnds(d,m,g);

/*Ввод новой записи*/
if(dend == 0)
 {
  sprintf(strsql,"insert into Usldokum \
values (%d,'%04d-%d-%d',%s,'%s','%s','%s','%s',%d,'%s','%04d-%d-%d','%s',\
%.2f,%d,%d,%d,%d,'%s','%04d-%d-%d','%s','%04d-%d-%d','%s',%d,%ld,'%s',\
'%04d-%d-%d',0.,%.2f,%d,'%s')",
  tipz,
  g,m,d,
  VV->VVOD_SPISOK_return_data(2),
  VV->VVOD_SPISOK_return_data(0),
  VV->VVOD_SPISOK_return_data(3),
  VV->VVOD_SPISOK_return_data(7),
  VV->VVOD_SPISOK_return_data(4),
  lnds,
  forop.ravno(),
  gop,mop,dop,
  VV->data_ravno_filtr(5),
  0.,
  0,
  prov,
  0,
  0,
  VV->VVOD_SPISOK_return_data(11),
  godv,mesv,denv,
  VV->VVOD_SPISOK_return_data(14),
  gdov,mdov,ddov,
  VV->data_ravno_filtr(16),
  iceb_t_getuid(),
  vrem,
  VV->data_ravno_filtr(10),
  gpnn,mpnn,dpnn,
  nds,
  metka_opl,
  k00);
  
  if(sql_zap(&bd,strsql) != 0)
   {
    msql_error(&bd,gettext("Ошибка записи !"),strsql);
    return(1);
   }
 }

/*Корректировка записи*/
if(dend != 0)
 {
  rsdat(&dz,&mz,&gz,datz,1);  

  /*узнаём хозяина записи*/
  sprintf(strsql,"select ktoi from Usldokum where tp=%d and datd='%04d-%d-%d' and podr=%s and nomd='%s'",
  tipz,gz,mz,dz,podrz,nomdokz);

  if(readkey(strsql,&row,&cur) == 1)
   if(usl_pvku(atoi(row[0])) != 0)
    return(1);
    
  sprintf(strsql,"update Usldokum set \
datd='%04d-%d-%d',\
podr=%s,\
kontr='%s',\
nomd='%s',\
nomnn='%s',\
kodop='%s',\
nds=%d,\
forop='%s',\
datop='%04d-%d-%d',\
uslpr='%s',\
ktoi=%d,\
vrem=%ld,\
osnov='%s',\
nomdp='%s',\
pro=%d,\
datpnn='%04d-%d-%d',\
dover='%s',\
datdov='%04d-%02d-%02d',\
datdp='%04d-%02d-%02d',\
sherez='%s',\
pn=%.2f,\
k00='%s' \
where tp=%d and datd='%04d-%d-%d' and podr=%s and nomd='%s'",
  g,m,d,
  VV->VVOD_SPISOK_return_data(2),
  VV->VVOD_SPISOK_return_data(0),
  VV->VVOD_SPISOK_return_data(3),
  VV->VVOD_SPISOK_return_data(7),
  VV->VVOD_SPISOK_return_data(4),
  lnds,
  forop.ravno(),
  gop,mop,dop,
  VV->data_ravno_filtr(5),
  iceb_t_getuid(),
  vrem,
  VV->data_ravno_filtr(10),
  VV->VVOD_SPISOK_return_data(11),
  prov,
  gpnn,mpnn,dpnn,
  VV->VVOD_SPISOK_return_data(14),
  gdov,mdov,ddov,
  godv,mesv,denv,
  VV->data_ravno_filtr(16),
  nds,
  k00,
  tipz,gz,mz,dz,podrz,nomdokz);

  if(sql_zap(&bd,strsql) != 0)
   {
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
    return(1);
   }

  sprintf(strsql,"update Usldokum1 set \
datd='%04d-%d-%d',\
podr=%s,\
nomd='%s'\
where tp=%d and datd='%04d-%d-%d' and podr=%s and nomd='%s'",
  g,m,d,VV->VVOD_SPISOK_return_data(2),VV->VVOD_SPISOK_return_data(3), 
  tipz,gz,mz,dz,podrz,nomdokz);
  if(sql_zap(&bd,strsql) != 0)
   {
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
   }

  /*Исправляем подтверждающие записи в которых дата подтверждения
   совпадает с датой документа*/

  sprintf(strsql,"update Usldokum2 set \
datd='%04d-%d-%d',\
datp='%04d-%d-%d',\
podr=%s,\
nomd='%s'\
where tp=%d and datd='%04d-%d-%d' and datp='%d-%d-%d' and podr=%s and \
nomd='%s'",
  g,m,d,g,m,d,VV->VVOD_SPISOK_return_data(2),VV->VVOD_SPISOK_return_data(3), 
  tipz,gz,mz,dz,gz,mz,dz,podrz,nomdokz);
  if(sql_zap(&bd,strsql) != 0)
   {
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
   }

  /*Исправляем все остальные подтверждающие записи*/
  sprintf(strsql,"update Usldokum2 set \
datd='%04d-%d-%d',\
podr=%s,\
nomd='%s'\
where tp=%d and datd='%04d-%d-%d' and podr=%s and nomd='%s'",
  g,m,d,VV->VVOD_SPISOK_return_data(2),VV->VVOD_SPISOK_return_data(3), 
  tipz,gz,mz,dz,podrz,nomdokz);
  if(sql_zap(&bd,strsql) != 0)
   {
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
   }

  /*Исправляем проводки в которых дата проводки совпадает с
  датой документа*/
/*
  printw(gettext("Проверяем проводки...\n"));
  refresh();
*/
  sprintf(strsql,"update Prov \
set \
oper='%s',\
nomd='%s',\
datp='%04d-%02d-%02d',\
datd='%04d-%02d-%02d',\
pod=%s,\
ktoi=%d,\
vrem=%ld \
where datd='%04d-%d-%d' and datp='%04d-%d-%d' and nomd='%s' and \
pod=%s and kto='%s'",
  VV->VVOD_SPISOK_return_data(4),VV->VVOD_SPISOK_return_data(3),g,m,d,g,m,d,VV->VVOD_SPISOK_return_data(2),iceb_t_getuid(),vrem,
  gz,mz,dz,gz,mz,dz,nomdokz,podrz,ICEB_MP_USLUGI);
  if(sql_zap(&bd,strsql) != 0)
   {
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
   }
  /*Исправляем остальные проводки*/

  sprintf(strsql,"update Prov \
set \
oper='%s',\
nomd='%s',\
datd='%04d-%02d-%02d',\
pod=%s,\
ktoi=%d,\
vrem=%ld \
where datd='%04d-%02d-%02d' and nomd='%s' and \
pod=%s and kto='%s'",
VV->VVOD_SPISOK_return_data(4),VV->VVOD_SPISOK_return_data(3),g,m,d,VV->VVOD_SPISOK_return_data(2),iceb_t_getuid(),vrem,
gz,mz,dz,nomdokz,podrz,ICEB_MP_USLUGI);
  if(sql_zap(&bd,strsql) != 0)
   {
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
   }

  if(SRAV(kontrz,VV->VVOD_SPISOK_return_data(0),0) != 0)
   {
    /*Исправляем код контрагента в проводках*/
    sprintf(strsql,"update Prov \
set \
kodkon='%s' \
where datd='%04d-%d-%d' and nomd='%s' and \
pod=%s and kto='%s' and kodkon='%s'",
    VV->VVOD_SPISOK_return_data(0),
    gz,mz,dz,nomdokz,podrz,ICEB_MP_USLUGI,kontrz);
    if(sql_zap(&bd,strsql) != 0)
     {
      msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
     }

   }
 
 }

return(0);
}
