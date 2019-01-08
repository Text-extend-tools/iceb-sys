/*$Id: dirbaz.c,v 5.19 2013/08/13 05:49:47 sasa Exp $*/
/*13.07.2015	18.09.2009	Белых А.И.	dirbaz.c
Просмотр списка баз данных
*/
#include <sys/stat.h>
#include <errno.h>
#include "buhg.h"

class dirbaz_poi
 {
  public:
   class iceb_tu_str imabaz;
   class iceb_tu_str naim00;
   int metka_poi;

  dirbaz_poi()
   {
    metka_poi=0;
    clear();
   }   
  void clear()
   {
    imabaz.new_plus("");
    naim00.new_plus("");
   }
   
 };
 
void dirbaz_sap(class dirbaz_poi *poisk);
int admin_del_alx(const char *put_na_kat);
void dirbuser(const char *imabazv);
void diralx(const char *imabazv);
void dirbaz_dumpbazin(const char *login,const char *parol,const char *hostname);
void dirbaz_dumpbazout(const char *namebases,const char *login,const char *parol,const char *hostname);
int dirbaz_prov(SQL_str row,class dirbaz_poi *poisk);
int dirbaz_f4(class dirbaz_poi *poisk);
void dirbaz_check_repair(const char *namebase);

extern class iceb_tu_str kodirovka_iceb;
extern char *imabaz;

int dirbaz(const char *login,const char *parol,const char *hostname)
{
class iceb_t_mstr spis_menu(0);
char strsql[512];
int  Y=3;
int X=0;
int kolstr=0;
int K=0;
int pozz=0,pozz1=0;
int mspz=0;
SQL_str row,row1;
int prc;
int kom=0;
int i=0;
int voz=0;
unsigned razmer_pol_imabaz=0;
class SQLCURSOR cur,cur1;
class iceb_tu_str kodirovka("");
class iceb_tu_str naim_org("");
class iceb_tu_str host("");
class iceb_tu_str imabaz_v("");
class iceb_tu_str put_na_alx("");
class dirbaz_poi poisk;


//Создаем класс меню
VVOD DANET(1);
VVOD VVOD1(2);
class VVOD PAR(2);
PAR.VVOD_SPISOK_add_MD(gettext("Введите пароль"));

naz1:;

/*устанавливаем базу данных по умолчанию*/
sql_zapis("USE mysql",1,1);
sprintf(strsql,"SET NAMES %s",kodirovka_iceb.ravno_udsimv("-"));
sql_zap(&bd,strsql);

short           kls=LINES-6;
int metkabaz[kls];

sprintf(strsql,"SHOW DATABASES");

  

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;

   
  dirbaz_sap(&poisk);

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);


  if(K == SFK2 || K == ENTER || K == FK3 || K == FK5 || K == FK6)
    goto lll;

  if(K == KEY_RESIZE)
   goto naz1;   

  goto vper;
 }

razmer_pol_imabaz=0;
while(cur.read_cursor(&row) != 0)
 {
  if(razmer_pol_imabaz < strlen(row[0]))
    razmer_pol_imabaz=strlen(row[0]);
 }

if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;

naz:;

cur.poz_cursor(pozz);
mspz=2;
pozz1=0;

spis_menu.spis_str.free_class();
for(i=0;i<=kls-1;)
 {
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;

  if(SRAV("mysql",row[0],0) == 0)
   continue;

  if(dirbaz_prov(row,&poisk) != 0)
   continue;

  /*Устанавливаем базу данных для того чтобы прочесть какая у неё кодировка по умолчанию*/
  sprintf(strsql,"USE %s",row[0]);
  sql_zap(&bd,strsql);

  /*читаем кодировку базы данных*/  
  kodirovka.new_plus("");
  if(sql_readkey(&bd,"SELECT @@character_set_database",&row1,&cur1) > 0)
   kodirovka.new_plus(row1[0]);
  /*Узнаём наименование организации если это iceB база*/

  naim_org.new_plus("");

  sprintf(strsql,"select naikon from %s.Kontragent where kodkon='00'",row[0]);
  if((voz=sql_readkey(&bd,strsql,&row1,&cur1)) > 0)
   naim_org.new_plus(row1[0]);

  metkabaz[i]=0;
  if(voz < 0)
   {
    naim_org.new_plus(gettext("Это не iceB база данных"));
    metkabaz[i]=1;
   }
  if(voz == 0)
   naim_org.new_plus(gettext("Не введён контрагент с кодом 00"));

  
  sprintf(strsql,"%-*s|%-*s|%s",razmer_pol_imabaz,
  row[0],
  iceb_tu_kolbait(6,kodirovka.ravno()),kodirovka.ravno(),
  naim_org.ravno());
  

  spis_menu.spis_str.plus(strsql);
  i++;
  
  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
   mspz=1;

 }

sql_zapis("USE mysql",1,1);

if(i == 0)
 {
  if(kolstr != 0 && pozz > 0)
   {
    pozz--;
    goto naz;
   }
  goto lll;
 }  
if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));


dirbaz_sap(&poisk);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,Y,X,NULL,strsql,prc,&K,"",COLS-24);


if(kom == -7) //Изменение границы экрана
  goto naz1;

if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  for(i=0; i< mspz   ; )
   {
    pozz++;
    if(pozz >= kolstr)
     {
      pozz=kolstr-1;
      break;
     }
    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;
/****
    if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[5],poisk.naimpoi.ravno()) == 0)
     continue;
********/
    if(dirbaz_prov(row,&poisk) != 0)
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
/*******
    if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[5],poisk.naimpoi.ravno()) == 0)
     continue;
*******/
    if(dirbaz_prov(row,&poisk) != 0)
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
  printw("\n вперед pozz=%d kolstr=%ld\n",pozz,kolstr);
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
/*********
    if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[5],poisk.naimpoi.ravno()) == 0)
     continue;
**************/
    if(dirbaz_prov(row,&poisk) != 0)
     continue;
    i++;
   }
  goto naz;
 }

if(kom >= 0)
 {
  if(SRAV(gettext("Конец списка"),spis_menu.spis_str.ravno(kom),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/

  polen(spis_menu.spis_str.ravno(kom),&imabaz_v,1,'|');

 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return(1);
     break;

  case FK1:  /*Получить подсказку*/
    GDITE();
//    prosf(strsql);
    goto naz;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      goto naz;
    if(parolv(&PAR,0,0,0,1) != 0)
     goto naz;
    sprintf(strsql,"drop database %s",imabaz_v.ravno());
    sql_zapis(strsql,0,0);
    
    goto naz1;

  case FK4:
   dirbaz_f4(&poisk); /*поиск*/
   goto naz;
   
  case ENTER:
  case FK5:       /*Просмотр списка файлов настройки*/
    if(metkabaz[kom] == 1)
     {
      iceb_t_soob(gettext("Это не iceB база данных"));
      goto naz;      
     }
    if(imabaz != NULL)
     delete [] imabaz;   

    imabaz=new char[imabaz_v.getdlinna()];
    strcpy(imabaz,imabaz_v.ravno());

    diralx(imabaz);

    sql_zapis("USE mysql",1,0);
 
    goto naz;

  case FK6:       /*Просмотр сриска операторов*/
    if(metkabaz[kom] == 1)
     {
      iceb_t_soob(gettext("Это не iceB база данных"));
      goto naz;      
     }
    dirbuser(imabaz_v.ravno());
    goto naz;

   case FK7:
    dirbaz_dumpbazout(imabaz_v.ravno(),login,parol,hostname);
    goto naz;

  case FK8:


    dirbaz_dumpbazin(login,parol,hostname);

    sql_zapis("USE mysql",1,0);

    goto naz1;
     
  case FK9:
    if(metkabaz[kom] == 1)
     {
      iceb_t_soob(gettext("Это не iceB база данных"));
      goto naz;      
     }
   
   sprintf(strsql,"USE %s",imabaz_v.ravno());
   sql_zapis(strsql,1,0);

   dirbaz_check_repair(imabaz_v.ravno());

   sql_zapis("USE mysql",1,0);
   break;
   
  default:
    goto naz;
    break;
 }

return(1);

}
/*********/
/*Шапка*/
/*********/
void dirbaz_sap(class dirbaz_poi *poisk)
{
short		y=2;
short           x=COLS-20;

clear();
printw(gettext("Список баз данных"));



mvprintw(2,0,"          1             2     3");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("база"));
mvprintw(y++,x,"2-%s",gettext("кодировка"));
mvprintw(y++,x,"3-%s",gettext("организация"));

if(poisk->metka_poi == 1)
 {
  y++;
  attron(A_BLINK);
  mvprintw(y++,x,gettext("Поиск:"));
  if(poisk->imabaz.getdlinna() > 1)
    mvprintw(y++,x,"%s:%s",gettext("имя базы"),poisk->imabaz.ravno());
  if(poisk->naim00.getdlinna() > 1)
    mvprintw(y++,x,"%s:%s",gettext("орг-ция"),poisk->naim00.ravno());
  attroff(A_BLINK);
  
 }


helstr(LINES-1,0,
//"F1",gettext("помощь"),
//"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("настройка"),
"F6",gettext("операторы"),
"F7",gettext("выгрузить"),
"F8",gettext("загрузить"),
"F9",gettext("проверка"),
"F10",gettext("выход"),NULL);
 

}
/***************************************************/
/*Загрузка дампа базы данных*/
/*******************************/
void dirbaz_dumpbazin(const char *login,const char *parol,const char *hostname)
{
char strsql[2048];

sprintf(strsql,"%s\n%s",gettext("Внимание!!!\n\
Если имя загружаемой базы совпадёт с уже имеющимся в списке баз,\n\
то содержимое новой базы данных заменит содержимое существующей базы."),gettext("Введите пароль"));

if(parolv(strsql,0) != 0)
 return;



class iceb_tu_str imaf("");

class VVOD VVOD1(2);

VVOD1.VVOD_SPISOK_add_MD(gettext("Введите имя файла с дампом базы"));
int kom1=0;

naz:;
clear();

helstr(LINES-1,0,
"F2/+",gettext("загруз."),
"F10",gettext("выход"),NULL);

if((kom1=vvod1(&imaf,64,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
  return;
if(kom1 == ESC)
  return;

switch(kom1)
 {
  case FK10:
  case ESC:
   return;

  case ENTER:
  case FK2:
  case PLU:
    if(imaf.ravno()[0] == '\0')
     {
      dirkatl(&imaf);
      goto naz;
     }
    break;

 };
 
time_t time_start=time(NULL); 

struct stat bufstat;
if(stat(imaf.ravno(),&bufstat) != 0)
 {
  error_op_nfil(imaf.ravno(),errno,"");
  goto naz;
 }

int kom=0;

class VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите режим загрузки дамба базы"));

MENU.VVOD_SPISOK_add_MD(gettext("Грузить до первой ошибки"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Грузить до конца"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2

if(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
switch (kom)
 {
  case 2:
  case -1 :
    return;

  case 0:
  case 1:
    break;

  default:
   goto naz;
 }
clear();
GDITE();

class iceb_tu_str imafin;
class iceb_tu_str syfix;

imafin.new_plus(imaf.ravno());

if(polen(imaf.ravno(),&syfix,2,'.') == 0)
 {
  if(SRAV("bz2",syfix.ravno(),0) == 0 || SRAV("BZ2",syfix.ravno(),0) == 0)
   {
    polen(imaf.ravno(),&imafin,1,'.');
    printw("%s bunzip2\n",gettext("Разжимаем файл программой"));
    refresh();

    sprintf(strsql,"bunzip2 %s 2>&1",imaf.ravno());

    iceb_t_system(strsql,0);
   }
 }

printw("%s %s\n",gettext("Загружаем дамп базы из файла"),imafin.ravno_shell_quote());
refresh();
if(kom == 0)
  sprintf(strsql,"mysql -u %s -p'%s' -h %s -f 2>&1 < %s",login,parol,hostname,imafin.ravno_shell_quote());
if(kom == 1)
  sprintf(strsql,"mysql -u %s -p'%s' -h %s -f 2>&1 -f < %s",login,parol,hostname,imafin.ravno_shell_quote());

iceb_t_system(strsql,0);


printw_vr(time_start);
OSTANOV();



}
/***********************************************/
/*Выгрузка дама базы данных*/
/*********************************/
void dirbaz_dumpbazout(const char *namebases,const char *login,const char *parol,const char *hostname)
{
int		kom;
char strsql[512];
VVOD MENU(3);

kom=0;



MENU.VVOD_SPISOK_add_ZAG(gettext("Выгрузка дампа баз данных"));

sprintf(strsql,"%s %s",gettext("Выгрузить дамп базы"),namebases);
MENU.VVOD_SPISOK_add_MD(gettext(strsql));//0
MENU.VVOD_SPISOK_add_MD(gettext("Выгрузить дамп всех баз данных"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2

naz:;

clearstr(LINES-1,0);
while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

switch (kom)
 {
  case 2 :
  case -1 :
    return;

  case 0 :
    sql_zapis("FLUSH TABLES",1,0);
    iceb_t_dumpbazout(namebases,login,parol,hostname);
    return;

  case 1:
    sql_zapis("FLUSH TABLES",1,0);
    iceb_t_dumpbazout("",login,parol,hostname);
    return;

  default:
    kom=0;
    goto naz;
 }
}
/***************************************/
/*поиск*/
/*****************************************/
int dirbaz_f4(class dirbaz_poi *poisk)
{
int kom=0;
class VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Поиск"));

VV.VVOD_SPISOK_add_MD(gettext("Имя базы данных..........(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Наименование организации.(,,).."));

VV.VVOD_SPISOK_add_data(poisk->imabaz.ravno(),256);
VV.VVOD_SPISOK_add_data(poisk->naim00.ravno(),256);

naz:;

 
helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("поиск"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

poisk->imabaz.new_plus(VV.VVOD_SPISOK_return_data(0));
poisk->naim00.new_plus(VV.VVOD_SPISOK_return_data(1));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   iceb_t_pdoc("buhg5_1.txt");
   goto naz;

  case ESC:
  case FK10:
    poisk->metka_poi=0;
    return(1);

  case FK2:
  case PLU:
    poisk->metka_poi=1;
    break;


  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }


return(0);
}
/***************************/
/*проверка условий поиска*/
/**************************/
int dirbaz_prov(SQL_str row,class dirbaz_poi *poisk)
{

if(poisk->metka_poi == 0)
 return(0);

if(proverka(poisk->imabaz.ravno(),row[0],4,0) != 0)
 return(1);
 
if(poisk->naim00.getdlinna() > 1)
 {
  char strsql[512];
  SQL_str row1;
  class SQLCURSOR cur1;
  
  class iceb_tu_str naim_org("");
  sprintf(strsql,"select naikon from %s.Kontragent where kodkon='00'",row[0]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) > 0)
   naim_org.new_plus(row1[0]);
  if(proverka(poisk->naim00.ravno(),naim_org.ravno(),4,0) != 0)
   return(1);
 }
return(0); 
}
/*****************************************/
/*Физическая целосность базы*/
/******************************************/
void dirbaz_check_repair(const char *namebase)
{
int kom=0;
VVOD MENU(3);

MENU.VVOD_SPISOK_add_ZAG(gettext("Инстументы"));


MENU.VVOD_SPISOK_add_MD(gettext("Проверка физической целосности таблиц базы данных"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Восстановление таблиц базы данных"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2

clear();

if(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0)
 return;

class spis_oth oth;   

switch (kom)
 {
  case 2:
  case -1 :
    return;

   case 0:
    if(check_table(&oth,namebase) == 0)
       iceb_t_rabfil(&oth,"");
     break;

   case 1:

    if(repair_table(&oth,namebase) == 0) 
      iceb_t_rabfil(&oth,"");
     break;
         
  default:
     kom=0;
     break;

 }


}
