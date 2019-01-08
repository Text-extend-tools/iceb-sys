/* $Id: smpld.c,v 5.39 2013/12/31 11:48:11 sasa Exp $ */
/*26.12.2017    15.04.1997      Белых А.И.      smpld.c
Просмотр списка платежных документов
*/
#include        <math.h>
#include        "buhg.h"
#include        "dok4.h"

class smpld_poi
 {
  public:
  class iceb_tu_str nomdokv;
  class iceb_tu_str dat1;
  class iceb_tu_str dat2;
  class iceb_tu_str kps;
  class iceb_tu_str kopp;
  class iceb_tu_str sumapoi;
  short	mpn; /*0-все 1-без проводок*/
  short metpoi; /*0-просмотр без поиска 1-с поиском*/
  short metka_pp; /*если равна 1 то только платёжки отмеченные для передачи в банк*/
  short metkap; //0- за год 1-за текущюю дату
  smpld_poi()
   {
    mpn=0;
    metpoi=0;
    metka_pp=0;
    metkap=0;
    clear();
   }
  void clear()
   {
    nomdokv.new_plus("");
    dat1.new_plus("");
    dat2.plus_tek_dat();
    kopp.new_plus("");
    kps.new_plus("");
    sumapoi.new_plus("");
   }   
 };
 
void shsmn1(short dn,short mn,short gn,const char *tabl,class smpld_poi *rekpoi);
int provrp(SQL_str row,short dn,short mn,short gn,short dk,short mk,short gk,class smpld_poi *rekpoi,const char *kpos);

extern class REC rec;
extern short    kp;    /*Количество повторов*/

void smpld(const char *tabl,short god,
int metkap) //0- за год 1-за текущюю дату
{
static class smpld_poi rekpoi;
class iceb_t_mstr spis_menu(0);
class iceb_tu_str bros("");
int		ktoz1=0;
struct  tm      *bf;
short           voz;  /*Возврат*/
short           d,m,g,dn,mn,gn,dk,mk,gk;
int             K;
short           x=0,y=2;
int             i=0,kom=0;
short		kl;
class iceb_tu_str npp("");
class iceb_tu_str naimor(""); /*наименование организации*/
class iceb_tu_str naimo("");
class iceb_tu_str kpos("");  /*Код организации*/
class iceb_tu_str datpr("");
class iceb_tu_str kop("");
long		pozz,pozz1;
long		kolstr;
SQL_str         row,row1;
char		strsql[1024];
class iceb_tu_str kto("");
double		sum;
int		prc;
short		mspz; /*метка соответствия первой записи*/
time_t          tmm;
class iceb_tu_str kodkontr("");
class iceb_tu_str naimkontr("");
short metka_sort_dat=0; /*0-по возростанию дат 2-по убыванию дат*/
SQLCURSOR cur1;
kom=voz=0;
rekpoi.metkap=metkap;
rekpoi.mpn=0;
rekpoi.metpoi=0;
rekpoi.metka_pp=0;
rekpoi.metkap=0;
if(rekpoi.dat1.getdlinna() <= 1)
 {
  sprintf(strsql,"01.01.%d",god);
  rekpoi.dat1.new_plus(strsql);
 }
dn=dk=0;

//Создаем класс меню
VVOD SOOB(1);
VVOD VV(0);
VVOD DANET(1);
 
pozz=dn=mn=gn=dk=mk=gk=0;

gn=god;
dn=1;
mn=1;
if(metkap == 1)
 {
  time(&tmm);
  bf=localtime(&tmm);
  dn=bf->tm_mday;
  mn=bf->tm_mon+1;
  gn=bf->tm_year+1900;
 }

naz1:;
GDITE();
clear();

short           kls=LINES-5;
time_t		vremz[kls];

if(metkap == 0)
 sprintf(strsql,"select * from %s where datd>='%04d-%02d-%02d'",tabl,gn,mn,dn);
else
 sprintf(strsql,"select * from %s where datd='%04d-%02d-%02d'",tabl,gn,mn,dn);

if(metka_sort_dat == 0)
 strcat(strsql," order by datd asc,nomd asc");
else
 strcat(strsql," order by datd desc,nomd asc");

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {

lll:;

   shsmn1(dn,mn,gn,tabl,&rekpoi);

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == FK5 || K == FK9)
    goto lll;

  if(K == KEY_RESIZE)
    goto naz1;

  goto vper;
    
 }


naz:;
GDITE();
clear();

cur.poz_cursor(pozz);
pozz1=0;
mspz=2;
spis_menu.spis_str.free_class();
for(i=0;i<=kls-1;)
 {
  
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;

  rsdat(&d,&m,&g,row[0],2);
  npp.new_plus(row[1]);
  kop.new_plus(row[2]);
  sum=atof(row[3]);  
  polen(row[8],&kpos,1,'#');
  polen(row[8],&naimor,2,'#');
  rec.podt=atoi(row[9]);
  rec.prov=atoi(row[10]);
  kto.new_plus(row[12]);
  vremz[i]=atol(row[13]);

  if(provrp(row,dn,mn,gn,dk,mk,gk,&rekpoi,kpos.ravno()) != 0)
   continue;
  bros.new_plus("");
  if(rec.prov == 1)
   bros.new_plus("*");
  if(rec.podt == 0)
   bros.plus("?");
  if(row[11][0] == '1')
   bros.plus("#");
   
  sprintf(strsql,"%02d.%02d.%d|%-*s|%-*s|%-*s|%-*.*s|%-4s|%8.2f|%s",
  d,m,g,
  iceb_tu_kolbait(3,kop.ravno()),kop.ravno(),
  iceb_tu_kolbait(4,npp.ravno()),npp.ravno(),
  iceb_tu_kolbait(4,kpos.ravno()),kpos.ravno(),
  iceb_tu_kolbait(15,naimor.ravno()),iceb_tu_kolbait(15,naimor.ravno()),naimor.ravno(),
  kto.ravno(),sum,bros.ravno());

  spis_menu.spis_str.plus(strsql);
  i++;

  if(pozz1 == 1)
    mspz=1;
 }

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;
if(i == 0)
 {
  if(kolstr > 0 && pozz > 0)
   {
    pozz--;
    goto naz;
   }
  goto lll;
 }

if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

shsmn1(dn,mn,gn,tabl,&rekpoi);


prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,"",0);


if(kom == -7) //Изменен размер экрана
  goto naz1;
   
if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
//  printw("pozz=%d mspz=%d\n",pozz,mspz);
  for(i=0; i < mspz  ; )
   {
    pozz++;

    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;

    rsdat(&d,&m,&g,row[0],2);
    polen(row[8],&kpos,1,'#');
    rec.podt=atoi(row[9]);
    rec.prov=atoi(row[10]);
    if(provrp(row,dn,mn,gn,dk,mk,gk,&rekpoi,kpos.ravno()) != 0)
     continue;

    i++;
   }
  kom=kls;
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

    rsdat(&d,&m,&g,row[0],2);
    polen(row[8],&kpos,1,'#');
    rec.podt=atoi(row[9]);
    rec.prov=atoi(row[10]);
    if(provrp(row,dn,mn,gn,dk,mk,gk,&rekpoi,kpos.ravno()) != 0)
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
  if(pozz >= kolstr)
   {
    pozz=kolstr-pozz1;
    beep();
    goto naz;  
   }
/*  
  printw("\n вперед pozz=%d\n",pozz);
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
    cur.read_cursor(&row);

    rsdat(&d,&m,&g,row[0],2);
    polen(row[8],&kpos,1,'#');
    rec.podt=atoi(row[9]);
    rec.prov=atoi(row[10]);
    if(provrp(row,dn,mn,gn,dk,mk,gk,&rekpoi,kpos.ravno()) != 0)
     continue;
    i++;
   }
  goto naz;
 }

if(kom == -1)
  voz=1;
else
 {
  if(SRAV(gettext("Конец списка"),spis_menu.spis_str.ravno(kom),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz;

  polen(spis_menu.spis_str.ravno(kom),&datpr,1,'|');
  rsdat(&d,&m,&g,datpr.ravno(),1);
  polen(spis_menu.spis_str.ravno(kom),&kop,2,'|');
  polen(spis_menu.spis_str.ravno(kom),&npp,3,'|');
  polen(spis_menu.spis_str.ravno(kom),&kpos,4,'|');
  polen(spis_menu.spis_str.ravno(kom),&ktoz1,6,'|');
//  ktoz1=(int)ATOFM(bros);

vper:;

  switch(K)
   {       
    case FK10:
    case ESC:
      return;

    case ENTER:  /*выбор строки для входа*/

      clear();

      /*Читаем платежку*/
      rsdat(&d,&m,&g,datpr.ravno(),1);
      if(readpdok(tabl,g,npp.ravno()) != 0)
       goto naz;

      pld(kp,tabl);

      clear();
      goto naz1;

    case FK1:   /*Помощь*/
      GDITE();
      iceb_t_pdoc("dokum2_4.txt");
      clear();
      if(kolstr > 0)
        goto naz;
      else
        goto lll;
      

    case FK4:  /*Поиск*/
      VV.VVOD_delete();
      VV.VVOD_SPISOK_add_ZAG(gettext("Поиск документов"));
      VV.VVOD_SPISOK_add_ZAG(gettext("Введите нужные реквизиты"));

      VV.VVOD_SPISOK_add_MD(gettext("Номер документа.."));
      VV.VVOD_SPISOK_add_MD(gettext("Дата начала......"));
      VV.VVOD_SPISOK_add_MD(gettext("Дата конца......."));
      VV.VVOD_SPISOK_add_MD(gettext("Код контрагента.."));
      VV.VVOD_SPISOK_add_MD(gettext("Код операции....."));
      VV.VVOD_SPISOK_add_MD(gettext("Сумма............"));

      VV.VVOD_SPISOK_add_data(rekpoi.nomdokv.ravno(),11);
      VV.VVOD_SPISOK_add_data(rekpoi.dat1.ravno(),11);
      VV.VVOD_SPISOK_add_data(rekpoi.dat2.ravno(),11);
      VV.VVOD_SPISOK_add_data(rekpoi.kps.ravno(),112);
      VV.VVOD_SPISOK_add_data(rekpoi.kopp.ravno(),112);
      VV.VVOD_SPISOK_add_data(rekpoi.sumapoi.ravno(),20);

naz2:;

      helstr(LINES-1,0,"F2/+",gettext("поиск"),
      "F3",gettext("контрагенты"),
      "F4",gettext("очистить"),
      "F10",gettext("выход"),NULL);

      kl=VV.vvod(0,1,1,-1,-1);
      rekpoi.metpoi=0;
      switch(kl)
       {
        case FK10:
        case ESC:
          break;
          
        case FK2:
        case PLU:
          rekpoi.nomdokv.new_plus(VV.VVOD_SPISOK_return_data(0));
          rekpoi.dat1.new_plus(VV.VVOD_SPISOK_return_data(1));
          rekpoi.dat2.new_plus(VV.VVOD_SPISOK_return_data(2));
          dn=mn=gn=dk=mk=gk=0;
          if(rekpoi.dat1.getdlinna() > 1)
           {
            rsdat(&dn,&mn,&gn,rekpoi.dat1.ravno(),1);
            if(rekpoi.dat2.getdlinna() <= 1)
             {
              dk=dn; mk=mn; gk=gn;
              dpm(&dk,&mk,&gk,5); 
             }
            else
              rsdat(&dk,&mk,&gk,rekpoi.dat2.ravno(),1);
           }

          rekpoi.kps.new_plus(VV.VVOD_SPISOK_return_data(3));
          rekpoi.kopp.new_plus(VV.VVOD_SPISOK_return_data(4));
          rekpoi.sumapoi.new_plus(VV.VVOD_SPISOK_return_data(5));
          rekpoi.metpoi=1;
          pozz=0;
          goto naz1;

      case FK3:
        clear();
        
        rekpoi.nomdokv.new_plus(VV.VVOD_SPISOK_return_data(0));
        rekpoi.dat1.new_plus(VV.VVOD_SPISOK_return_data(1));
        rekpoi.dat2.new_plus(VV.VVOD_SPISOK_return_data(2));
        kodkontr.new_plus("");
        naimkontr.new_plus("");
        if(dirkontr(&kodkontr,&naimkontr,1) == 0)
         {
          VV.VVOD_SPISOK_zapis_data(3,kodkontr.ravno());
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

    case FK2:  /*Не подтвержденные*/

      if(rekpoi.mpn == 2)
        rekpoi.mpn=0;
      else
        rekpoi.mpn=2;  
      if(kolstr > 0)
        goto naz;
      else
        goto lll;
        
    case FK3:  /*без проводок*/
    
      if(rekpoi.mpn == 1)
        rekpoi.mpn=0;
      else
        rekpoi.mpn=1;  

      if(kolstr > 0)
        goto naz;
      else
        goto lll;

    case SFK2:  /*Снять метку не подтвержденного документа*/
      DANET.VVOD_delete();
      DANET.VVOD_SPISOK_add_MD(gettext("Снять отметку не подтвержденного документа ? Вы уверены ?"));

      if(danet(&DANET,2,stdscr) == 2)
       goto naz;


      sprintf(strsql,"update %s set podt=1 where datd='%04d-%02d-%02d' and nomd='%s'",
      tabl,g,m,d,npp.ravno());

      sql_zapis(strsql,0,0);
      goto naz1;

    case SFK3:  /*Снять отметку о не выполненых проводках*/
      DANET.VVOD_delete();
      DANET.VVOD_SPISOK_add_MD(gettext("Снять отметку о невыполненых проводках ? Вы уверены ?"));

      if(danet(&DANET,2,stdscr) == 2)
       goto naz;


      sprintf(strsql,"update %s set prov=0 where datd='%04d-%02d-%02d' and nomd='%s'",
      tabl,g,m,d,npp.ravno());

      sql_zapis(strsql,1,0);
      goto naz1;

    case FK5: /*Расшифровка*/
      /*Читаем наименование организации*/

      if(kpos.ravno()[0] == '0' && kpos.ravno()[1] == '0' && kpos.ravno()[2] == '-')
        bros.new_plus("00");
      else
        bros.new_plus(kpos.ravno());


      if(bros.ravno()[0] != '\0')
       {       
        sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",bros.ravno());
        if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
         {
          beep();
          move(20,0);
          printw("%s - %s\n",gettext("Не найден код контрагента"),kpos.ravno());
          OSTANOV();
          naimor.new_plus("");
         }
        else
         naimor.new_plus(row1[0]);
       }

     /*Читаем наименование операции*/
     if(SRAV(tabl,"Pltp",0) == 0)
       sprintf(strsql,"select naik from Opltp where kod='%s'",kop.ravno());
     if(SRAV(tabl,"Pltt",0) == 0)
       sprintf(strsql,"select naik from Opltt where kod='%s'",kop.ravno());


     if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
      {
       move(20,0);
       beep();
       printw(gettext("Не нашли наименование операции %s !"),kop.ravno());
       printw("\n");
       OSTANOV();
       naimo.new_plus("");
      }
     else
       naimo.new_plus(row1[0]);
      SOOB.VVOD_delete();

      sprintf(strsql,"%s %s %s",gettext("Контрагент :"),kpos.ravno(),naimor.ravno());
      SOOB.VVOD_SPISOK_add_MD(strsql);
      sprintf(strsql,"%s %s %s",gettext("Операция   :"),kop.ravno(),naimo.ravno());
      SOOB.VVOD_SPISOK_add_MD(strsql);

      if(ktoz1 != 0)
       {
        sprintf(strsql,"%s %s",gettext("Записал    :"),iceb_t_kszap(ktoz1));
        SOOB.VVOD_SPISOK_add_MD(strsql);
       }
  
      if(vremz[kom] > 0)
       {
        bf=localtime(&vremz[kom]);
        sprintf(strsql,gettext("Дата записи %d.%d.%dг. Время %d:%d:%d"),
        bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,bf->tm_hour,bf->tm_min,bf->tm_sec);
        SOOB.VVOD_SPISOK_add_MD(strsql);
       }
  
      soobshw(&SOOB,stdscr,-1,-1,0,1);
      goto naz;

    case FK6: //Увеличить на день
      if(metkap == 1)
       dpm(&dn,&mn,&gn,1);
      goto naz1;

    case FK7: //Уменьшить на день
      if(metkap == 1)
        dpm(&dn,&mn,&gn,2);
      goto naz1;

    case FK8: /*Включить/выключить просмотр только отмеченных документов для передачи в банк*/
      rekpoi.metka_pp++;
      if(rekpoi.metka_pp > 1)
       rekpoi.metka_pp=0;
       
      goto naz1;


    case TAB: /*Включить/выключить просмотр по убыванию дат*/
      metka_sort_dat++;
      if(metka_sort_dat >= 2)
       metka_sort_dat=0;
       
      goto naz1;

    case FK9: /*Установить/снять метку для передачи в банк*/
      sprintf(strsql,"select vidpl from %s where datd='%04d-%02d-%02d' and nomd='%s'",
      tabl,g,m,d,npp.ravno());
      if(readkey(strsql,&row,&cur) != 1)
       {
        iceb_t_soob(gettext("Ненашли запись!"));
        goto naz;
       }
      if(atoi(row[0]) == 1)
       sprintf(strsql,"update %s set vidpl='' where datd='%04d-%02d-%02d' and nomd='%s'",
       tabl,g,m,d,npp.ravno());
      else
       sprintf(strsql,"update %s set vidpl='1' where datd='%04d-%02d-%02d' and nomd='%s'",
       tabl,g,m,d,npp.ravno());

      sql_zapis(strsql,1,0);
      goto naz1;

      
    default:
      goto naz;
   }
 }

}

/*******/
/*Шапка*/
/*******/
void shsmn1(short dn,short mn,short gn,const char *tabl,class smpld_poi *rekpoi)
{
int		i1;
int		X,Y;
clear();

move(0,0);
if(rekpoi->mpn == 0 && SRAV(tabl,"Pltp",0) == 0)
   printw(gettext("Просмотр платёжных поручений"));
if(rekpoi->mpn == 0 && SRAV(tabl,"Pltt",0) == 0)
   printw(gettext("Просмотр платёжных требований"));
if(rekpoi->metkap == 0)
  printw(" %s:%d%s",gettext("Стартовый год"),gn,
  gettext("г."));
if(rekpoi->metkap == 1)
  printw(" %s:%d.%d.%d%s",gettext("Дата"),dn,mn,gn,
  gettext("г."));

if(rekpoi->mpn == 1)
 {
  attron(A_BLINK); /*Влючение мерцания*/
  printw(gettext("Просмотр документов без проводок"));
  attroff(A_BLINK); /*Выключение мерцания*/
 }

if(rekpoi->mpn == 2)
 {
  attron(A_BLINK); /*Влючение мерцания*/
  printw(gettext("Просмотр не подтвержденных документов"));
  attroff(A_BLINK); /*Выключение мерцания*/
 }

if(rekpoi->metpoi == 1)
 {
  move(i1=13,X=COLS-19);
  attron(A_BLINK);
  printw(gettext("Поиск:"));
  if(rekpoi->nomdokv.getdlinna() > 1)
   {
    move(++i1,X);
    printw("%s %s",gettext("Н.д."),rekpoi->nomdokv.ravno());
   }

  if(rekpoi->dat1.getdlinna() > 1)
   {
    move(++i1,X);
    printw("%s %s",gettext("Дата н."),rekpoi->dat1.ravno());
   }
  if(rekpoi->dat1.getdlinna() > 1)
   {
    move(++i1,X);
    printw("%s %s",gettext("Дата к."),rekpoi->dat2.ravno());
   }

  if(rekpoi->kps.getdlinna() > 1)
   {
    move(++i1,X);
    printw("%s %s",gettext("Код кон."),rekpoi->kps.ravno());
   }

  if(rekpoi->kopp.getdlinna() > 1)
   {
    move(++i1,X);
    printw("%s %s",gettext("Код оп."),rekpoi->kopp.ravno());
   }
  if(rekpoi->sumapoi.getdlinna()  > 1)
   {
    move(++i1,X);
    printw("%s %.2f",gettext("Сумма"),rekpoi->sumapoi.ravno_atof());
   }
  attroff(A_BLINK);
 }



move(1,0);
printw("        1     2    3   4          5         6       7");

move(Y=2,X=COLS-18);
printw(gettext("Расшифровка полей:"));
move(++Y,X);
printw(gettext("1 дата документа"));
move(++Y,X);
printw(gettext("2 код операции"));
move(++Y,X);
printw(gettext("3 номер документа"));
move(++Y,X);
printw(gettext("4 код контрагента"));
move(++Y,X);
printw(gettext("5 назв. контраг."));
move(++Y,X);
printw(gettext("6 кто записал"));
move(++Y,X);
printw(gettext("7 сумма платежа"));
move(++Y,X);
printw(gettext("* не выпол. пров."));
move(++Y,X);
printw(gettext("? не подтв. док."));
move(++Y,X);
printw("# %s",gettext("передать"));

if(rekpoi->metkap == 0)
 helstr(LINES-1,0,"F1",gettext("помощь"),
"F2","?","F3","*",
"F4",gettext("поиск"),
"F5",gettext("расшифровка"),
"F8","#",
"F9",gettext("пас"),
"F10",gettext("выход"),NULL);
else
 helstr(LINES-1,0,"F1",gettext("помощь"),
"F2","?","F3","*",
"F4",gettext("поиск"),
"F5",gettext("расшифровка"),
"F6",gettext("след.день"),
"F7",gettext("пред.день"),
"F8","#",
"F9",gettext("пас"),
"F10",gettext("выход"),NULL);

}

/***************************/
/*Проверка реквизитов поиска*/
/*****************************/
int provrp(SQL_str row,short dn,short mn,short gn,short dk,short mk,short gk,class smpld_poi *rekpoi,const char *kpos)
{
short d,m,g;

if(rekpoi->metka_pp == 1 && atoi(row[11]) == 0)
 return(1);

if(rekpoi->mpn == 1 && atoi(row[10]) == 0)
  return(1);

if(rekpoi->mpn == 2 && atoi(row[9]) != 0)
     return(1);

if(rekpoi->metpoi == 0)
  return(0);

rsdat(&d,&m,&g,row[0],2);
  
if(dn != 0)
if(SRAV1(gn,mn,dn,g,m,d) < 0)
  return(1);

if(dk != 0)
if(SRAV1(gk,mk,dk,g,m,d) > 0)
  return(1);
if(proverka(rekpoi->kps.ravno(),kpos,0,0) != 0)
  return(1);

if(proverka(rekpoi->kopp.ravno(),row[2],0,0) != 0)
 return(1);
if(proverka(rekpoi->nomdokv.ravno(),row[1],0,0) != 0)
 return(1);


if(rekpoi->sumapoi.ravno_atof() != 0.)
 {
  double sum=atof(row[3]);
  if(fabs(sum - rekpoi->sumapoi.ravno_atof()) > 0.009)
   return(1);
 }

return(0);
}
