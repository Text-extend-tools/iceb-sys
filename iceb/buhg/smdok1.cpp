/* $Id: smdok1.c,v 5.30 2013/08/13 05:49:55 sasa Exp $ */
/*01.12.2016    15.04.1997      Белых А.И.      smdok1.c
Просмотр накладных для материального учета
*/
#include	"buhg.h"

void shsmn(short mpn,short metpoi,const char *nomdokv,short dn,short mn,short gn,short dk,short mk,short gk,const char *kps,
const char *kprrz,int skl11,int tipzz,const char *dover,const char *nomnalnak);

int sravdok(SQL_str row,short dn,short mn,short gn,short dk,short mk,short gk,const char *kps,const char *kprrz,int skl11,int tipzz,const char *nomdok,int mpn,const char *dover,short metpoi,const char *nomnalnak);

extern short    ddd,mmm,ggg;

void            smdok1(short dnp,short mnp,short gnp, //Дата начала поиска
		KASSA kasr)	// кассовый регистратор
{
long		kolstr;
long		pozz; /*Позиция первой строчки меню*/
long            pozz1; /*Количество строк прочитанных для заполнения меню*/
SQL_str         row,row1;
char		strsql[1024];
struct  tm      *bf;
short           voz;  /*Возврат*/
short           dn,mn,gn,dk,mk,gk;
short		d,m,g;
int             K;
short           kls;
short           x=0,y=2;
int           i,kom,kom1;
int		prc;
class iceb_t_mstr spis_menu(0);
short           metpoi;
static int      skl11;
static class iceb_tu_str kps("");
static class iceb_tu_str dat1(""),dat2("");
static class iceb_tu_str kprrz("");
short		mpn; /*0-все 1-не подтвержденный 2-без проводок*/
short		kl;
static class iceb_tu_str dover("");
static class iceb_tu_str nomdokv("");
class iceb_tu_str nomdokd("");
class iceb_tu_str bros("");
int		tipzz=0,tipzd=0;
class iceb_tu_str naimo("");
class iceb_tu_str naiskl(""),naimpr("");
time_t  	tmm;
short		mspz; /*метка соответствия первой записи*/
int		skld=0;
class iceb_tu_str kontr("");
class iceb_tu_str oper("");
class iceb_tu_str skladch("");
class iceb_tu_str ktozap("");
static class iceb_tu_str nomnalnak("");
SQLCURSOR curr;

kls=LINES-1-4;
tipzz=mpn=kom=voz=0;

metpoi=0;
dn=dk=skl11=0;
 
dn=dnp;
mn=mnp;
gn=gnp;

dk=mk=gk=0;

clear();
dnp=0;
pozz=0;
VVOD SOOB(1);
VVOD VV(0);
VVOD MENU(3);

naz1:;

clear();
GDITE();

if(dn != 0 || dk != 0 || skl11 != 0 || kprrz.ravno()[0] != '\0')
 metpoi=1;

sprintf(strsql,"select * from Dokummat where datd >= '%d-01-01' \
order by datd asc",gn);

if(dn != 0)
  sprintf(strsql,"select * from Dokummat where datd >= '%d-%02d-%02d' \
order by datd asc",gn,mn,dn);

if(dn != 0 && dk != 0)
  sprintf(strsql,"select * from Dokummat \
where datd >= '%d-%02d-%02d' and datd <= '%d-%02d-%02d' \
order by datd asc",gn,mn,dn,gk,mk,dk);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }  

if(kolstr == 0)
 {

lll:;

  shsmn(mpn,metpoi,nomdokv.ravno(),dn,mn,gn,dk,mk,gk,kps.ravno(),kprrz.ravno(),skl11,tipzz,dover.ravno(),nomnalnak.ravno());
  SOOB.VVOD_delete();
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одного документа !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == FK5 )
     goto lll;

  if(K == KEY_RESIZE)
    goto naz1;
  
  goto vper;
 }

naz:;
GDITE();

if(kolstr != 0 && pozz >= kolstr)
  pozz = kolstr-1;

spis_menu.spis_str.free_class();

cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
for(i=0; i<kls ; )
 {
  
  if(cur.read_cursor(&row) == 0)
    break;
/*
  printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  refresh();
*/
  pozz1++;

  if(sravdok(row,dn,mn,gn,dk,mk,gk,kps.ravno(),kprrz.ravno(),skl11,tipzz,nomdokv.ravno(),mpn,
  dover.ravno(),metpoi,nomnalnak.ravno()) != 0)
    continue;

  if(row[0][0] == '1')
   bros.new_plus("+");
  if(row[0][0] == '2')
   bros.new_plus("-"); 
  if(row[7][0] == '0')
   bros.plus("?");
  if(row[8][0] == '0')
   bros.plus("*");
  
  rsdat(&d,&m,&g,row[1],2);
  
  sprintf(strsql,"%02d.%02d.%d|%-*s|%-*s|%-*s|%-2s|%-*s|%-*s|%-3s|%s",
  d,m,g,
  iceb_tu_kolbait(8,row[4]),row[4],
  iceb_tu_kolbait(4,row[3]),row[3],
  iceb_tu_kolbait(3,row[6]),row[6],
  row[2],
  iceb_tu_kolbait(4,row[5]),row[5],
  iceb_tu_kolbait(6,row[11]),row[11],
  row[9],
  bros.ravno());

  spis_menu.spis_str.plus(strsql);
  i++;

  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
   mspz=1;

 }

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

if(i == 0)
 {
  if(kolstr > 0 && pozz > 0)
   {
    pozz--;
    goto naz;
   }
  goto lll;
 }
shsmn(mpn,metpoi,nomdokv.ravno(),dn,mn,gn,dk,mk,gk,kps.ravno(),kprrz.ravno(),skl11,tipzz,dover.ravno(),nomnalnak.ravno());

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,"",0);


if(kom == -7) //Изменение размера экрана
  goto naz1;

if(kom == -3) /*Стрелка вниз*/
 {
  for(i=0; i< mspz  ; )
   {
    pozz++;
    if(pozz >= kolstr)
     pozz=kolstr-1;
    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;

    if(sravdok(row,dn,mn,gn,dk,mk,gk,kps.ravno(),kprrz.ravno(),skl11,tipzz,nomdokv.ravno(),mpn,
    dover.ravno(),metpoi,nomnalnak.ravno()) != 0)
      continue;
    i++;
   }
  kom=kls-1;
  goto naz;
 }
if(kom == -2) /*Стрелка вверх*/
 {
  for(i=0; i < 1 ; )
   {

    pozz--;

    if(pozz <= 0)
     {
      beep();
      pozz=0;
      goto naz;
     }

    cur.poz_cursor(pozz);
    cur.read_cursor(&row);

    if(sravdok(row,dn,mn,gn,dk,mk,gk,kps.ravno(),kprrz.ravno(),skl11,tipzz,nomdokv.ravno(),mpn,
    dover.ravno(),metpoi,nomnalnak.ravno()) != 0)
      continue;
    i++;
   }

  goto naz;
 }
 
if(kom == -5) /*Страница вниз*/
 {
  if(pozz + kls == kolstr)
   {
    pozz=kolstr-1;
    beep();
    goto naz;  
   }
  if(i == kls)
    pozz+=pozz1;
  else
    beep();
/*  
  printw("\n вперед pozz=%d pozz1=%d i=%d kls=%d\n",pozz,pozz1,i,kls);
  OSTANOV();
*/
  goto naz;

 }

if(kom == -4) /*Страница вверх*/
 {
/*
  printw("\npozz=%d\n",pozz);
  refresh();
*/
  for(i=0; i<kls ; )
   {
    pozz--;

    if(pozz <= 0)
     {
      beep();
      pozz=0;
      goto naz;
     }

    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;
    if(sravdok(row,dn,mn,gn,dk,mk,gk,kps.ravno(),kprrz.ravno(),skl11,tipzz,nomdokv.ravno(),mpn,
    dover.ravno(),metpoi,nomnalnak.ravno()) != 0)
      continue;

    i++;
   }
  goto naz;
 }

if(kom >= 0)
 {
  class iceb_tu_str strvib(spis_menu.spis_str.ravno(kom));
  if(SRAV(gettext("Конец списка"),strvib.ravno(),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz;

  polen(strvib.ravno(),&bros,1,'|');
  rsdat(&dnp,&mnp,&gnp,bros.ravno(),0);

  polen(strvib.ravno(),&skld,5,'|');
//  skld=atoi(bros);

  polen(strvib.ravno(),&nomdokd,2,'|');
  polen(strvib.ravno(),&kontr,3,'|');
  polen(strvib.ravno(),&oper,4,'|');
  polen(strvib.ravno(),&skladch,5,'|');

  polen(strvib.ravno(),&ktozap,8,'|');

  polen(strvib.ravno(),&bros,9,'|');
  if(bros.ravno()[0] == '+')
    tipzd=1;  
  if(bros.ravno()[0] == '-')
    tipzd=2;  

 }

vper:;

class iceb_tu_str kods("");
class iceb_tu_str naims("");
switch(K)
 {  
  case FK1: /*Помощь*/
    GDITE();
    iceb_t_pdoc("matu2_3.txt");
    if(kolstr > 0)
      goto naz;
    else
      goto lll;

  case FK5: /*Расшифровка*/

    /*Читаем наименование организации*/
    if(kontr.ravno()[0] == '0' && kontr.ravno()[1] == '0' && kontr.ravno()[2] == '-')
      polen(kontr.ravno(),&bros,1,'-');
    else
      bros.new_plus(kontr.ravno());

    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
    bros.ravno());
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      beep();
      printw("\n%s %s\n",gettext("Не найден код контрагента"),kontr.ravno());
      OSTANOV();
      naimo.new_plus("");
     }
    else
     {
      naimo.new_plus(row1[0]);
     } 

    /*Читаем наименование операции*/
    if(tipzd == 1)
      bros.new_plus("Prihod");
    if(tipzd == 2)
      bros.new_plus("Rashod");
    sprintf(strsql,"select naik from %s where kod='%s'",
    bros.ravno(),oper.ravno());
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      beep();
      printw(gettext("Не найден код операции %s !"),oper.ravno());
      printw("\n");
      OSTANOV();
      naimpr.new_plus("");
     }
    else
     {
      naimpr.new_plus(row1[0]);
     } 

    /*Читаем наименование склада*/
    sprintf(strsql,"select naik from Sklad where kod=%s",
    skladch.ravno());
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      beep();
      printw(gettext("Не найден склад %d в списке складов !"),skladch.ravno_atoi());
      printw("\n");
      OSTANOV();
      naiskl.new_plus("");
     }
    else
     {
      naiskl.new_plus(row1[0]);
     } 

    SOOB.VVOD_delete();

    sprintf(strsql,"%s %d.%d.%d",gettext("Дата докум. :"),dnp,mnp,gnp);
    SOOB.VVOD_SPISOK_add_MD(strsql);

    sprintf(strsql,"%s %s",gettext("Документ    :"),nomdokd.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);

    sprintf(strsql,"%s %s %s",gettext("Склад       :"),skladch.ravno(),naiskl.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);

    sprintf(strsql,"%s %s %s",gettext("Контрагент  :"),kontr.ravno(),naimo.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);

    sprintf(strsql,"%s %s %s",gettext("Операция    :"),oper.ravno(),naimpr.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);
    if(ktozap.ravno_atoi() != 0)
     {
      sprintf(strsql,"%s %s",gettext("Записал     :"),iceb_t_kszap(ktozap.ravno()));
      SOOB.VVOD_SPISOK_add_MD(strsql);
             
     }
    /*Чітаем время записи*/
    sprintf(strsql,"select vrem from Dokummat where tip=%d and \
datd='%d-%d-%d' and sklad=%s and nomd='%s'",
    tipzd,gnp,mnp,dnp,skladch.ravno(),nomdokd.ravno());

    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
    if(atoi(row1[0]) > 0)
     {
      tmm=atoi(row1[0]);
      bf=localtime(&tmm);
      sprintf(strsql,"%s %d.%d.%d%s %s - %d:%d:%d",
      gettext("Дата записи :"),
      bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
      gettext("г."),
      gettext("Время"),
      bf->tm_hour,bf->tm_min,bf->tm_sec);
      SOOB.VVOD_SPISOK_add_MD(strsql);
     }
    
    soobshw(&SOOB,stdscr,-1,-1,0,1);

    goto naz;

  case FK4:  /*Поиск*/
    VV.VVOD_delete();
    VV.VVOD_SPISOK_add_ZAG(gettext("Введите данные для поиска документов"));

    VV.VVOD_SPISOK_add_MD(gettext("Номер документа..........."));
    VV.VVOD_SPISOK_add_MD(gettext("Дата начала поиска........"));
    VV.VVOD_SPISOK_add_MD(gettext("Дата конца поиска........."));
    VV.VVOD_SPISOK_add_MD(gettext("Код контрагента.....(,,).."));
    VV.VVOD_SPISOK_add_MD(gettext("Код прихода/расхода.(,,).."));
    VV.VVOD_SPISOK_add_MD(gettext("Номер склада.............."));
    VV.VVOD_SPISOK_add_MD(gettext("Приход/расход (+/-)......."));
    VV.VVOD_SPISOK_add_MD(gettext("Довереность..............."));
    VV.VVOD_SPISOK_add_MD(gettext("Номер нал. накладной......"));

    VV.VVOD_SPISOK_add_data(nomdokv.ravno(),10);
    VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
    VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
    VV.VVOD_SPISOK_add_data(kps.ravno(),10);
    VV.VVOD_SPISOK_add_data(kprrz.ravno(),10);
    VV.VVOD_SPISOK_add_data(10);
    if(skl11 != 0)
     {
      sprintf(strsql,"%d",skl11);
      VV.VVOD_SPISOK_zapis_data(5,strsql);
     }
    VV.VVOD_SPISOK_add_data(2);
    VV.VVOD_SPISOK_add_data(dover.ravno(),20);
    VV.VVOD_SPISOK_add_data(nomnalnak.ravno(),20);

    
naz2:;
    metpoi=0;

    helstr(LINES-1,0,"F2/+",gettext("поиск"),
    "F3",gettext("реквизиты"),
    "F4",gettext("очистить"),
    "F10",gettext("выход"),NULL);

    kl=VV.vvod(0,1,1,-1,-1);
    
    
    switch(kl)
     {
      case FK10:
      case ESC:
        break;
        
      case FK2:
      case PLU:
        GDITE();
        dat1.new_plus(VV.VVOD_SPISOK_return_data(1));
        dat2.new_plus(VV.VVOD_SPISOK_return_data(2));
        nomdokv.new_plus(VV.VVOD_SPISOK_return_data(0));

        if(dat1.ravno()[0] != '\0')
         {
          if(rsdat(&d,&m,&g,dat1.ravno(),0) != 0)
           {
            iceb_t_soob(gettext("Не верно введена дата начала !"));
            goto naz2;
           }
          dk=dn=d; mn=mk=m; gn=gk=g;
         }

        if(dat2.ravno()[0] == '\0')
         {
          dpm(&dk,&mk,&gk,5); 
         }
        else
         if(rsdat(&d,&m,&g,dat2.ravno(),0) != 0)
          {
           iceb_t_soob(gettext("Не верно введена дата конца !"));
           goto naz2;
          }
         else
          {
           dk=d; mk=m; gk=g;
          }
         

        kps.new_plus(VV.VVOD_SPISOK_return_data(3));
        kprrz.new_plus(VV.VVOD_SPISOK_return_data(4));
        skl11=(short)ATOFM(VV.VVOD_SPISOK_return_data(5));
        tipzz=0;
        if(VV.VVOD_SPISOK_return_data(6)[0] == '+')
          tipzz=1;
        if(VV.VVOD_SPISOK_return_data(6)[0] == '-')
          tipzz=2;
      
        dover.new_plus(VV.VVOD_SPISOK_return_data(7));
        nomnalnak.new_plus(VV.VVOD_SPISOK_return_data(8));

        if(VV.VVOD_SPISOK_return_data(0)[0] != '\0' || VV.VVOD_SPISOK_return_data(1)[0] != '\0' || VV.VVOD_SPISOK_return_data(2)[0] != '\0' || \
        VV.VVOD_SPISOK_return_data(3)[0] != '\0' || VV.VVOD_SPISOK_return_data(4)[0] != '\0' || VV.VVOD_SPISOK_return_data(5)[0] != '\0' || \
        VV.VVOD_SPISOK_return_data(6)[0] != '\0' || VV.VVOD_SPISOK_return_data(7)[0] != '\0' || VV.VVOD_SPISOK_return_data(8)[0] != '\0')
         metpoi=1;
        pozz=0;
        goto naz1;

      case FK3:
        MENU.VVOD_delete();
        MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка складов"));
        MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));
        MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций приходов"));
        MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций расходов"));
        MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

        clearstr(LINES-1,0);
        kom1=0;        
        while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);


        switch (kom1)
         {
          case 4 :
          case -1:
           goto naz2;

          case 0 :
            clear();
            
            if(dirsklad(1,&kods,&naims) == 0)
             VV.data_z_plus(5,kods.ravno());
            clear();
            goto naz2;

          case 1 :
            clear();
            if(vibrek("Kontragent",&kods) == 0)
             VV.data_z_plus(3,kods.ravno());
            clear();
            goto naz2;

          case 2 :
            clear();
            if(dirprihod(1,&kods,&naims) == 0)
             VV.data_z_plus(4,kods.ravno());
            clear();
            goto naz2;

          case 3 :
            clear();
            if(dirrashod(1,&kods,&naims) == 0)
              VV.data_z_plus(4,kods.ravno());
            clear();
            goto naz2;
         }

        clear();
        goto naz2;

      case FK4:
        VV.VVOD_clear_data();
        goto naz2;
                   
      default:
        goto naz2;
     }

    if(kolstr > 0)
      goto naz;
    else
      goto lll;

  case FK2:  /*Не подтвержденные накладные*/
    if(mpn == 1)
      mpn=0;
    else
      mpn=1;  

    if(kolstr > 0)
      goto naz;
    else
      goto lll;

  case FK3:  /*Накладные без проводок*/
    if(mpn == 2)
      mpn=0;
    else
      mpn=2;  

    if(kolstr > 0)
      goto naz;
    else
      goto lll;

  case ENTER:
  
     clear();
     dirdok1(dnp,mnp,gnp,tipzd,skld,&nomdokd,kasr);
     dnp=mnp=gnp=0;
     
     clear();
    
     goto naz1;

  case FK10:
  case ESC:
    return;
    
  default:
    if(kolstr > 0)
      goto naz;
    else
      goto lll; 
 }

}

/*******/
/*Шапка*/
/*******/
void shsmn(short mpn,short metpoi,const char *nomdokv,short dn,short mn,short gn,short dk,short mk,short gk,const char *kps,
const char *kprrz,int skl11,int tipzz,const char *dover,const char *nomnalnak)
{
int		X=COLS-21,Y=2;
clear();

move(0,0);

if(mpn == 0)
  printw("Просмотр документів");
if(mpn == 1)
 {
  attron(A_BLINK); /*Влючение мерцания*/
  printw("Просмотр непідтверджених документів");
  attroff(A_BLINK); /*Выключение мерцания*/
 }
if(mpn == 2)
 {
  attron(A_BLINK); /*Влючение мерцания*/
  printw("Просмотр документів без проводок");
  attroff(A_BLINK); /*Выключение мерцания*/
 }
move(1,0);
printw("      1          2     3    4  5   6     7     8");

move(Y++,X);
printw("Розшифровка полів:");
move(Y++,X);
printw("1 дата документа");
move(Y++,X);
printw("2 номер документа");
move(Y++,X);
printw("3 код контрагента");
move(Y++,X);
printw("4 код прибут./видатка");
move(Y++,X);
printw("5 код складу");
move(Y++,X);
printw("6 номер под. накл.");
move(Y++,X);
printw("7 номер зуст.докум.");
move(Y++,X);
printw("8 хто ввів");
move(Y++,X);
printw("+/- прібут./видаток");
move(Y++,X);
printw("? не підтверджен");
move(Y++,X);
printw("* не зро-ні проводки");

if(metpoi == 1)
 {
  move(Y++,X);
  attron(A_BLINK);
  printw("Поиск\n");
  if(nomdokv[0] != '\0')
   {
    move(Y++,X);
    printw("Н.д. %s",nomdokv);
   }

  if(dn != 0)
   {
    move(Y++,X);
    printw("Дата п. %d.%d.%d",dn,mn,gn);
   }

  if(dk != 0)
   {
    move(Y++,X);
    printw("Дата к. %d.%d.%d",dk,mk,gk);
   }

  if(kps[0] != '\0')
   {
    move(Y++,X);
    printw("Код кон. %s",kps);
   }
  if(kprrz[0] != '\0')
   {
    move(Y++,X);
    printw("Код оп. %s",kprrz);
   }
  if(skl11 != 0)
   {
    move(Y++,X);
    printw("Склад %d",skl11);
   }
  if(tipzz != 0)
   {
    move(Y++,X);
    if(tipzz == 1)
      printw("ТІЛЬКИ ПРИБУТКИ");
    if(tipzz == 2)
      printw("ТІЛЬКИ ВИДАТКИ");
   }
  if(dover[0] != '\0')
   {
    move(Y++,X);
    printw("Довір. %s",dover);
   }  
  attroff(A_BLINK);
 }

helstr(LINES-1,0,
"F1","допомога","F2"," ? ","F3"," * ","F4","пошук",
"F5","розшифровка","F10","вихід",NULL);
}

/*****************************
Проверка реквизитов поиска
Возвращяем 0- если все равно
           1-если не ыщмпадает
*******************************/           
int sravdok(SQL_str row,
short dn,short mn,short gn, //Дата начала поиска
short dk,short mk,short gk, //Дата конца поиска
const char *kps, //Код контрагента
const char *kprrz, //Код операции
int skl11, //Код склада
int tipzz, //Приход/расход
const char *nomdok, //Номер документа
int mpn, //Метка просмотра под/непод и пров/без пров
const char *dover, //Доверенность
short metpoi, //Метка поиска
const char *nomnalnak) //Номер налоговой накладной
{
short		d,m,g;
SQL_str         row1;
int		skl111;
int		tipz1;
char		strsql[100];
class iceb_tu_str bros("");
short		pro=0,pod=0;

pro=atoi(row[8]);
if(mpn == 2 && pro == 1)
   return(1);
  
if(mpn == 1 && pod == 1)
   return(1);

if(metpoi == 0)
 return(0);

tipz1=atoi(row[0]);
rsdat(&d,&m,&g,row[1],2);
skl111=atoi(row[2]);
pod=atoi(row[7]);

if(dn != 0)
 if(SRAV1(gn,mn,dn,g,m,d) < 0)
   return(1);

if(dk != 0)
 if(SRAV1(gk,mk,dk,g,m,d) > 0)
   return(1);
  
if(proverka(kps,row[3],0,0) != 0)
  return(1);
/***********
if(kps[0] != '\0' && SRAV(kps,row[3],0) != 0)
   return(1);
************/
if(proverka(kprrz,row[6],0,0) != 0)
  return(1);
/*********
if(kprrz[0] != '\0' && SRAV(row[6],kprrz,0) != 0)
   return(1);
**************/
if(skl11 != 0 && skl11 != skl111)
   return(1);

if(tipzz != 0 && tipzz != tipz1)
   return(1);


if(nomdok[0] != '\0' && SRAV(row[4],nomdok,1) != 0)
 if(nomdok[0] != '\0' && SRAV(row[11],nomdok,1) != 0)
   return(1);

if(dover[0] != '\0')
 {
  SQLCURSOR curr;
  /*Читаем номер доверенности*/
  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%s and nomerz=1",
  g,row[4],row[2]);

  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
   { 
/*
    printw("row=%s\n",row1[0]);
    OSTANOV();
*/
    polen(row1[0],&bros,1,'#');
    if(strstrm(bros.ravno(),dover) == 0)
        return(1);
   }
  else
   return(1);
 }

if(nomnalnak[0] != '\0')
 if(SRAV(row[5],nomnalnak,0) != 0)
   return(1);


return(0);

}
