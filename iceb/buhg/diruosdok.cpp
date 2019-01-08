/* $Id: diruosdok.c,v 5.46 2013/12/31 11:48:10 sasa Exp $ */
/*17.03.2018    30.06.1996      Белых А.И.      diruosdok.c
Просмотр всех документов, без проводок, не подтвержденных
Учет основных средств
*/
#include        "buhg.h"

class diruosdok_poi
 {
  public:
   class iceb_tu_str datan;
   class iceb_tu_str datak;
   class iceb_tu_str nomdok;
   class iceb_tu_str kontr;
   class iceb_tu_str kodpr;
   class iceb_tu_str kodpodr;
   class iceb_tu_str pr;
   class iceb_tu_str kodmo;
   short mpn;
   short metka_opl; /*0-все 1-не подтвержденный 2-без проводок*/
   short metpoi;

  diruosdok_poi()
   {
    clear();
    mpn=0;
    metka_opl=0;
    metpoi=0;
   }

  void clear()
   {
    datan.new_plus("");
    datak.plus_tek_dat();
    nomdok.new_plus("");
    kontr.new_plus("");
    kodpr.new_plus("");
    kodpodr.new_plus("");
    pr.new_plus("");
    kodmo.new_plus("");
   }   
 };
 

int provpoi(SQL_str row,class diruosdok_poi *poi);
void sduosd(class diruosdok_poi *poi);

extern short    mvnp; /*0-внешняя 1 -внутреняя 2-изменение стоимости*/
extern short    mdd;  /*0-обычный документ 1-двойной*/
extern short	startgoduos; /*Стартовый год*/

void            diruosdok()
{
static class diruosdok_poi poi;
struct  tm      *bf;
class iceb_tu_str bros("");
short           d,m,g;
short           dn,mn,gn;
short           dk,mk,gk;
int             K,i;
int           kom,kom1;
class iceb_tu_str md("");
int		kolstr;
long		pozz=0; /*Позиция первой строчки меню*/
long            pozz1=0; /*Количество строк прочитанных для заполнения меню*/
SQL_str         row,row1;
char		strsql[1024];
short metka_sort_den=0; /*0-в порядке возростания дат 1-в порядке убывания дат*/
short		mspz;
int		prc;
class iceb_tu_str kpos("");
class iceb_tu_str kprr("");
class iceb_tu_str pod("");
class iceb_tu_str nomdok("");
short		podt=0,prov=0,tz=0;
int		kl;
class iceb_tu_str naimo("");
class iceb_tu_str kor("");
int		poz;
int 	        komv;
class iceb_tu_str naimpr(""),naip("");
class iceb_tu_str kto("");
class iceb_tu_str kmatot("");
//int metka_opl=0;
class iceb_t_mstr spis_menu(0);
class iceb_tu_str naimmotv("");
class iceb_tu_str kodstr("");
class iceb_tu_str naimstr("");

if(poi.datan.getdlinna() <= 1)
 {
  sprintf(strsql,"01.01.%d",startgoduos);
  poi.datan.new_plus(strsql);
 }
 
SQLCURSOR curr;
poi.mpn=0;
poi.metka_opl=0;
poi.metpoi=0;

//Создаем класс меню
VVOD SOOB(1);
VVOD DANET(1);
VVOD MENU(3);
VVOD VV(0);
kom=komv=poz=pozz=0;
dn=mn=gn=dk=mk=gk=0;

naz1:;

clear();
GDITE();
short           kls=LINES-5;
time_t		vrem[LINES];

memset(strsql,'\0',sizeof(strsql));
if(dn == 0)
  sprintf(strsql,"select datd,tipz,kodop,kontr,nomd,podr,nomdv,podt,prov,ktoz,vrem,kodol,mo,nomnn from Uosdok where datd >= '%04d-%d-%d'",startgoduos,1,1);
if(dn != 0)
  sprintf(strsql,"select datd,tipz,kodop,kontr,nomd,podr,nomdv,podt,prov,ktoz,vrem,kodol,mo,nomnn from Uosdok where datd >= '%04d-%d-%d' and datd <= '%04d-%d-%d'",gn,mn,dn,gk,mk,dk);

if(metka_sort_den == 0)
 strcat(strsql," order by datd asc,nomd asc");
else
 strcat(strsql," order by datd desc,nomd asc");

class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {

lll:;
  sduosd(&poi);

  SOOB.VVOD_delete();
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);


  if(K == FK3 || K == FK2 || K == ESC || K == FK10)
     goto vper;
  if(K == FK4)
     goto fk4;
  if(K == KEY_RESIZE)
   goto naz1;
  goto lll;
 }

naz:;
GDITE();

if(kolstr != 0 && pozz >= kolstr)
  pozz = kolstr-1;



cur.poz_cursor(pozz);
mspz=2;
pozz1=0;

spis_menu.spis_str.free_class();
for(i=0; i<kls ;)
 {
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;

  if(provpoi(row,&poi) != 0)
    continue;

  rsdat(&d,&m,&g,row[0],2);
  tz=atoi(row[1]);
  podt=atoi(row[7]);
  prov=atoi(row[8]);

  md.new_plus("");
  if(tz == 1)
   md.plus("+");
  if(tz == 2)
   md.plus("-");

  if(prov == 1)
   md.plus("*");

  if(podt == 0)
   md.plus("?");
  if(row[12][0] == '0')
   md.plus("$");
   
  vrem[i]=atol(row[10]);

  if(row[6] == NULL)
   bros.new_plus("");
  else
   bros.new_plus(row[6]);
   
  sprintf(strsql,"%02d.%02d.%d|%-*s|%-*s|%-*s|%-2s|%-3s|%s|%4s|%*s|%s",
  d,m,g,
  iceb_tu_kolbait(5,row[4]),row[4],
  iceb_tu_kolbait(5,row[3]),row[3],
  iceb_tu_kolbait(3,row[2]),row[2],
  row[5],bros.ravno(),row[9],row[11],
  iceb_tu_kolbait(3,row[13]),row[13],
  md.ravno());

  spis_menu.spis_str.plus(strsql);
  i++;

 }
/*OSTANOV();*/

if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

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

sduosd(&poi);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,2,0,NULL,strsql,prc,&K,"",0);

if(kom == -7) //Изменение размера экрана
 goto naz1;
 
if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  for(i=0; i< mspz  ; )
   {
    pozz++;
    if(pozz >= kolstr)
     pozz=kolstr-1;
    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;

    podt=atoi(row[3]);
    prov=atoi(row[2]);
    if(provpoi(row,&poi) != 0)
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

    podt=atoi(row[3]);
    prov=atoi(row[2]);
    if(provpoi(row,&poi) != 0)
      continue;
    i++;
   }

  goto naz;
 }
 
if(kom == -5) /*Страница вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
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
  printw("\n вперед pozz=%d pozz1=%d i=%d kls=%d\n"k,pozz,pozz1,i,kls);
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
    podt=atoi(row[3]);
    prov=atoi(row[2]);
    if(provpoi(row,&poi) != 0)
      continue;

    i++;
   }
  goto naz;
 }

if(kom >= 0)
 {
  class iceb_tu_str strout(spis_menu.spis_str.ravno(kom));
  if(SRAV(gettext("Конец списка"),strout.ravno(),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/


  polen(strout.ravno(),&bros,1,'|');
  rsdat(&d,&m,&g,bros.ravno(),1);
  polen(strout.ravno(),&nomdok,2,'|');
  polen(strout.ravno(),&kpos,3,'|');
  polen(strout.ravno(),&kprr,4,'|');
  polen(strout.ravno(),&pod,5,'|');
  polen(strout.ravno(),&kto,7,'|');
  polen(strout.ravno(),&kmatot,8,'|');

  polen(strout.ravno(),&bros,9,'|');
  if(bros.ravno()[0] == '+')
   tz=1;
  if(bros.ravno()[0] == '-')
   tz=2;
      
 }

vper:;

switch (K)
 {
  case FK10:
  case ESC:
    return;
    
  case FK1: /*Помощь*/
   GDITE();
   iceb_t_pdoc("uos2_2.txt");
   clear();
   if(kolstr > 0)
    goto naz;
   else
    goto lll;

  case FK4:  /*Поиск*/

fk4:;

    VV.VVOD_delete();

    VV.VVOD_SPISOK_add_ZAG(gettext("Введите данные для поиска документов"));

/*******************
    VV.VVOD_SPISOK_add_data(11);
    VV.VVOD_SPISOK_add_data(11);
    if(dn != 0 && mn != 0 && gn != 0)
     {
      sprintf(strsql,"%d.%d.%d",dn,mn,gn);
      VV.VVOD_SPISOK_zapis_data(0,strsql);
     }
    if(dk != 0 && mk != 0 && gk != 0)
     {
      sprintf(strsql,"%d.%d.%d",dk,mk,gk);
      VV.VVOD_SPISOK_zapis_data(1,strsql);
     }
*******************/
    VV.VVOD_SPISOK_add_data(poi.datan.ravno(),11);
    VV.VVOD_SPISOK_add_data(poi.datak.ravno(),11);
    VV.VVOD_SPISOK_add_data(poi.nomdok.ravno(),128);
    VV.VVOD_SPISOK_add_data(poi.kontr.ravno(),128);
    VV.VVOD_SPISOK_add_data(poi.kodpr.ravno(),128);
    VV.VVOD_SPISOK_add_data(poi.kodpodr.ravno(),128);
    VV.VVOD_SPISOK_add_data(poi.pr.ravno(),2);
    VV.VVOD_SPISOK_add_data(poi.kodmo.ravno(),128);

    VV.VVOD_SPISOK_add_MD(gettext("Дата начала.............."));//0
    VV.VVOD_SPISOK_add_MD(gettext("Дата конца..............."));//1
    VV.VVOD_SPISOK_add_MD(gettext("Номер документа.....(,,)."));//2
    VV.VVOD_SPISOK_add_MD(gettext("Код контрагента.....(,,)."));//3
    VV.VVOD_SPISOK_add_MD(gettext("Код прихода/расхода.(,,)."));//4
    VV.VVOD_SPISOK_add_MD(gettext("Код подразделения...(,,)."));//5
    VV.VVOD_SPISOK_add_MD(gettext("Приход/расход (+/-)......"));//6
    VV.VVOD_SPISOK_add_MD(gettext("Код мат.ответ-ного..(,,)."));//7
naz2:;

   
    helstr(LINES-1,0,"F2/+",gettext("поиск"),
    "F3",gettext("реквизиты"),
    "F4",gettext("очистить"),
    "F10",gettext("выход"),NULL);


    kl=VV.vvod(0,1,1,-1,-1);

    poi.metpoi=0;

   switch(kl)
    {
     case FK10:
     case ESC:
       if(kolstr > 0)
         goto naz;
       else
         goto lll;

     case FK2:
     case PLU:
       GDITE();
       poi.metpoi=1;

       if(VV.VVOD_SPISOK_return_data(0)[0] != '\0')
        {
         if(rsdat(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(0),0) != 0)
          {
           iceb_t_soob(gettext("Не верно введена дата начала !"));
           goto naz2;
          }
        }
      
       if(VV.VVOD_SPISOK_return_data(1)[0] == '\0')
        {
         dk=31; mk=12; gk=gn;
        }
       else
        if(rsdat(&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(1),0) != 0)
         {
          iceb_t_soob(gettext("Не верно введена дата конца !"));
          goto naz2;
         }

       poi.datan.new_plus(VV.data_ravno(0));
       poi.datak.new_plus(VV.data_ravno(1));
       poi.nomdok.new_plus(VV.data_ravno(2));
       poi.kontr.new_plus(VV.data_ravno(3));
       poi.kodpr.new_plus(VV.data_ravno(4));
       poi.kodpodr.new_plus(VV.data_ravno(5));
       poi.pr.new_plus(VV.data_ravno(6));
       poi.kodmo.new_plus(VV.data_ravno(7));
       pozz=0;
       goto naz1;

     case FK3:     
       MENU.VVOD_delete();
       MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));//0
       MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций приходов"));//1
       MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операцій расходов"));//2
       MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));//3
       MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка мат.ответственных"));//4
       MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

       kom1=0;
       while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

       switch (kom1)
        {
         case -1:
         case 5:
           goto naz2;
             
         case 0: 
           clear();
           naimstr.new_plus("");
           kodstr.new_plus("");
           if(dirkontr(&kodstr,&naimstr,1) == 0)
            {
             VV.data_z_plus(3,kodstr.ravno());
             goto naz2;
            }

           clear();
           goto naz2;
      
         case 1 :
           if(diruospri(1,&kor,&naimo) == 0)
             VV.data_z_plus(4,kor.ravno());
           clear();
           goto naz2;

         case 2 :
           if(diruosras(1,&kor,&naimo) == 0)
             VV.data_z_plus(4,kor.ravno());
           clear();

           goto naz2;

         case 3 :
           kodstr.new_plus("");
           naimstr.new_plus("");
           if(diruospod(1,&kodstr,&naimstr,0) == 0)
             VV.data_z_plus(5,kodstr.ravno());
           clear();
           goto naz2;

         case 4 :
           kodstr.new_plus("");
           naimstr.new_plus("");
           if(dirmatot(1,&kodstr,&naimstr,0) == 0)
             VV.data_z_plus(7,kodstr.ravno());
            
           clear();
           goto naz2;
        }
       break;
       
     case FK4:    /*Очистить меню*/
       VV.VVOD_clear_data();
       goto naz2;
        
     default:
      goto naz2;
    }

   case FK2: /*Не подтвержденные документы*/

     if(poi.mpn != 1)
       poi.mpn=1;
     else
       poi.mpn=0;  

     if(kolstr > 0)
       goto naz;
     else
       goto lll;

   case FK3: /*Документы без проводок*/
     if(poi.mpn != 2)
       poi.mpn=2;
     else
       poi.mpn=0;  

     if(kolstr > 0)
       goto naz;
     else
       goto lll;

   case SFK2:  /*Принудительное снятие отметки*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Снять отметку не подтвержденного документа ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
     goto naz;


    sprintf(strsql,"update Uosdok \
set \
podt=1 \
where datd='%d-%02d-%02d' and nomd='%s'",
    g,m,d,nomdok.ravno());

    sql_zapis(strsql,0,0);
    goto naz1;

  case SFK3:  /*Снять отметку о не выполненых проводках*/
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Снять отметку о невыполненых проводках ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
     goto naz;


    sprintf(strsql,"update Uosdok \
set \
prov=0 \
where datd='%d-%02d-%02d' and nomd='%s'",
    g,m,d,nomdok.ravno());

    sql_zapis(strsql,0,0);
    goto naz1;


   case ENTER:

    clear();

    uosdok(d,m,g,&nomdok);
    clear();
    goto naz1;

  case FK5: /*Расшифровка*/

    /*Читаем наименование организации*/
    naimo.new_plus("");
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kpos.ravno());
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      beep();
      printw("%s %s\n",gettext("Не найден код контрагента"),kpos.ravno());
      OSTANOV();
     }
    else
     {
      naimo.new_plus(row1[0]);
     } 
    /*Читаем наименование операции*/
    if(tz == 1)
      bros.new_plus("Uospri");
    if(tz == 2)
      bros.new_plus("Uosras");
    naimpr.new_plus("");
    sprintf(strsql,"select naik from %s where kod='%s'",bros.ravno(),kprr.ravno());
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      beep();
      printw(gettext("Не найден код операции %s !"),row[6]);
      printw("\n");
      OSTANOV();
     }
    else
     {
      naimpr.new_plus(row1[0]);
     } 



    /*Читаем наименование подразделения*/
    naip.new_plus("");
    sprintf(strsql,"select naik from Uospod where kod='%s'",pod.ravno());
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      beep();
      printw("%s %s !",gettext("Не найден код подразделения"),pod.ravno());
      printw("\n");
      OSTANOV();
     }
    else
     {
      naip.new_plus(row1[0]);
     } 

    //Читаем код материально-ответственного
    naimmotv.new_plus("");
    sprintf(strsql,"select naik from Uosol where kod=%s",
    kmatot.ravno());
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      beep();
      printw("%s %s !","Не найден код материально-ответственного",kmatot.ravno());
      printw("\n");
      OSTANOV();
     }
    else
     {
      naimmotv.new_plus(row1[0]);
     } 

    SOOB.VVOD_delete();

    sprintf(strsql,"%s %d.%d.%d",gettext("Дата докумен.:"),d,m,g);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    sprintf(strsql,"%s %s",gettext("Документ     :"),nomdok.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);
    sprintf(strsql,"%s %s %s",gettext("Подразделениe:"),pod.ravno(),naip.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);
    sprintf(strsql,"%s %s %s",gettext("Контрагент   :"),kpos.ravno(),naimo.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);
    sprintf(strsql,"%s %s %s",gettext("Операция     :"),kprr.ravno(),naimpr.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);

    sprintf(strsql,"%s %s %s",gettext("Мат.отве-ный :"),kmatot.ravno(),naimmotv.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);
    if(kto.ravno()[0] != '\0')
     {
      sprintf(strsql,"%s %s",gettext("Записал     :"),iceb_t_kszap(kto.ravno()));
      SOOB.VVOD_SPISOK_add_MD(strsql);
     }

    if(vrem[kom] > 0)
     {
      bf=localtime(&vrem[kom]);
      sprintf(strsql,"%s %d.%d.%d%s %s: %d:%d:%d",
      gettext("Дата записи :"),
      bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
      gettext("г."),
      gettext("Время"),
      bf->tm_hour,bf->tm_min,bf->tm_sec);
      SOOB.VVOD_SPISOK_add_MD(strsql);
     }
    
    soobshw(&SOOB,stdscr,-1,-1,0,1);

    
    clear();

    goto naz;

  case FK9: /*переключение просмотра оплаченных и неоплаченных документов*/
    poi.metka_opl++;
    if(poi.metka_opl > 2)
     poi.metka_opl=0;
    goto naz1;

  case TAB: /*Включение/выключение сортировки записей в порядке убывания дат*/
    metka_sort_den++;
    if(metka_sort_den >= 2)
     metka_sort_den=0;
    goto naz1;

  default:
   if(kolstr > 0)
     goto naz;
   else
    goto lll;

 }

}

/*************************/
/*Проверка условий поиска*/
/**************************/
/*Если вернули 0 то запись подходит*/

int provpoi(SQL_str row,class diruosdok_poi *poi)
{
if(poi->metka_opl == 1 && atoi(row[12]) != 1)
 return(1);
if(poi->metka_opl == 2 && atoi(row[12]) != 0)
 return(1);
 
 
if(poi->mpn == 1 && atoi(row[7]) == 1)
  return(5);

if(poi->mpn == 2 && atoi(row[8]) == 0)
  return(6);

if(poi->metpoi == 0)  
  return(0);

if(proverka(poi->kontr.ravno(),row[3],0,0) != 0)
 return(1);

if(proverka(poi->kodpr.ravno(),row[2],0,0) != 0)
 {
  return(1);
 }
if(proverka(poi->kodpodr.ravno(),row[5],0,0) != 0)
 return(1);


if(proverka(poi->nomdok.ravno(),row[4],0,0) != 0)
 if(proverka(poi->nomdok.ravno(),row[6],0,0) != 0)
  return(1);

if(poi->pr.getdlinna() > 1)
 {
  if(poi->pr.ravno_pr() == 1)
   if(row[1][0] != '1')
    return(5);
  if(poi->pr.ravno_pr() == 2)
   if(row[1][0] != '2')
    return(5);
 }
if(proverka(poi->kodmo.ravno(),row[11],0,0) != 0)
 return(1);

/*
printw("%s %s %s\n",ndk,ndk1,ndk2);
OSTANOV();
*/


return(0);

}

/*********/
/*Шапка */
/*********/
void sduosd(class diruosdok_poi *poi)
{
short		Y,X;

clear();

Y=1; X=COLS-27;
move(Y++,X);
printw(gettext("Расшифровка полей:"));
move(Y++,X);
printw(gettext("1-дата документа"));
move(Y++,X);
printw(gettext("2-номер документа"));
move(Y++,X);
printw(gettext("3-контрагент"));
move(Y++,X);
printw(gettext("4-код операции"));
move(Y++,X);
printw(gettext("5-подразделение"));
move(Y++,X);
printw(gettext("6-номер встреч. документа"));
move(Y++,X);
printw(gettext("7-кто ввел"));
move(Y++,X);
printw(gettext("8-мат.ответственный"));
move(Y++,X);
printw("9-%s",gettext("номер нал.нак."));
move(Y++,X);
printw(gettext("+/- приход/расход"));
move(Y++,X);
printw(gettext("*-не выполнены проводки"));
move(Y++,X);
printw(gettext("?-не подтвержден документ"));
move(Y++,X);
printw("$ %s",gettext("не оплачено"));

move(Y+=2,X);
if(poi->metpoi == 1)
 {
  attron(A_BLINK); /*Влючение мерцания*/
  printw(gettext("Поиск:"));
  if(poi->datan.getdlinna() > 1)
   {
    move(++Y,X);
    printw("%s:%s",gettext("Дата н."),poi->datan.ravno());
   }
  if(poi->datak.getdlinna() > 1)
   {
    move(++Y,X);
    printw("%s:%s",gettext("Дата к."),poi->datak.ravno());
   }
  if(poi->nomdok.getdlinna() > 1)
   {
    move(++Y,X);
    printw("%s:%s",gettext("Ном.док."),poi->nomdok.ravno());
   }
  if(poi->kontr.getdlinna() > 1)
   {
    move(++Y,X);
    printw("%s:%s",gettext("Код.конт."),poi->kontr.ravno());
   }
  if(poi->kodpr.getdlinna() > 1)
   {
    move(++Y,X);
    printw("%s:%s",gettext("Код оп."),poi->kodpr.ravno());
   }
  if(poi->kodpodr.getdlinna() > 1)
   {
    move(++Y,X);
    printw("%s:%s",gettext("Код под."),poi->kodpodr.ravno());
   }
  if(poi->pr.getdlinna() > 1)
   {
    move(++Y,X);
    printw("%s:%s",gettext("Прих./расх."),poi->pr.ravno());
   }
  if(poi->kodmo.getdlinna() > 1)
   {
    move(++Y,X);
    printw("%s:%s",gettext("Код мат.отв."),poi->kodmo.ravno());
   }
  attroff(A_BLINK); /*Выключение мерцания*/
 }

if(poi->metka_opl == 1)
 {
  move(++Y,X);
  printw("%s",gettext("Только оплаченные"));
 }

if(poi->metka_opl == 2)
 {
  move(++Y,X);
  printw("%s",gettext("Только неоплаченные"));
 }
 
move(0,0);

if(poi->mpn == 0)
  printw(gettext("Просмотр всех документов"));
if(poi->mpn == 1)
 {
  attron(A_BLINK); /*Влючение мерцания*/
  printw(gettext("Просмотр не подтвержденных документов"));
  attroff(A_BLINK); /*Выключение мерцания*/
 }
if(poi->mpn == 2)
 {
  attron(A_BLINK); /*Влючение мерцания*/
  printw(gettext("Просмотр документов без проводок"));
  attroff(A_BLINK); /*Выключение мерцания*/
 }
move(1,0);
printw("      1        2     3    4   5  6   7   8    9");

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2","?","F3","*",
"F4",gettext("поиск"),
"F5",gettext("расшифровка"),
"F9",gettext("оплата"),
"F10",gettext("выход"),NULL);
}
