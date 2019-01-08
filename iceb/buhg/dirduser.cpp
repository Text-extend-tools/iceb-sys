/*$Id: dirduser.c,v 5.12 2013/05/17 14:55:57 sasa Exp $*/
/*13.07.2015	18.09.2009	Белых А.И.	dirduser.c
Просмотр списка пользователей с правами доступа к базам данных
*/
#include "buhg.h"

class dirduser_poi
 {
  public:
   class iceb_tu_str login;
   class iceb_tu_str imabaz;
   class iceb_tu_str host;
   short metka_poi;
   
  dirduser_poi()
   {
    clear();
    metka_poi=0;
   }
  void clear()
   {
    login.new_plus("");
    imabaz.new_plus("");
    host.new_plus("");
   }
 };


void dirduser_sap(class dirduser_poi *rp);
int dirdousr_prov(SQL_str row,class dirduser_poi *rp);
int dirduser_p(class dirduser_poi *rp);

extern class iceb_tu_str kodirovka_iceb;

int dirduser()
{
class dirduser_poi rp; /*Реквизиты поиска*/
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
class SQLCURSOR cur,cur1;
class iceb_tu_str kodirovka("");
class iceb_tu_str naim_org("");
class iceb_tu_str host("");
class iceb_tu_str imabaz("");
class iceb_tu_str login("");
int metka_sort=0; //0-по логинам 1-по базам

//Создаем класс меню
class VVOD DANET(1);
class VVOD VVOD1(2);

naz1:;

/*устанавливаем базу данных по умолчанию*/
sql_zapis("USE mysql",1,1);
sprintf(strsql,"SET NAMES %s",kodirovka_iceb.ravno_udsimv("-"));
sql_zap(&bd,strsql);

short           kls=LINES-6;

if(metka_sort == 0)
  sprintf(strsql,"select * from db order by User");
else
  sprintf(strsql,"select * from db order by Db");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;

   
  dirduser_sap(&rp);

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);


  if(K == SFK2 || K == ENTER || K == FK3 || K == FK5 || K == FK6)
    goto lll;

  if(K == KEY_RESIZE)
   goto naz1;   

  goto vper;
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

  if(dirdousr_prov(row,&rp) != 0)
   continue;

  /*Устанавливаем базу данных для того чтобы прочесть какая у неё кодировка по умолчанию*/
  sprintf(strsql,"USE %s",row[1]);
  sql_zap(&bd,strsql);

  /*читаем кодировку базы данных*/  
  kodirovka.new_plus("");
  if(sql_readkey(&bd,"SELECT @@character_set_database",&row1,&cur1) > 0)
   kodirovka.new_plus(row1[0]);
  /*Узнаём наименование организации если это iceB база*/

  naim_org.new_plus("");

  sprintf(strsql,"select naikon from %s.Kontragent where kodkon='00'",row[1]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) > 0)
   naim_org.new_plus(row1[0]);

  
  sprintf(strsql,"%-6s|%-6s|%-6s|%-10s|%s",row[2],row[1],kodirovka.ravno(),row[0],naim_org.ravno());
  

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


dirduser_sap(&rp);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,Y,X,NULL,strsql,prc,&K,"",0);


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
    if(dirdousr_prov(row,&rp) != 0)
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
    if(dirdousr_prov(row,&rp) != 0)
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
    if(dirdousr_prov(row,&rp) != 0)
     continue;
    i++;
   }
  goto naz;
 }

if(kom >= 0)
 {
  class iceb_tu_str str_vib(spis_menu.spis_str.ravno(kom));
  if(SRAV(gettext("Конец списка"),str_vib.ravno(),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/

  polen(str_vib.ravno(),&login,1,'|');
  polen(str_vib.ravno(),&imabaz,2,'|');
  polen(str_vib.ravno(),&host,4,'|');

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
//    iceb_t_pdoc("admin3.txt"); БАЗА mysql в ней нет документации
    goto naz;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      goto naz;


    sprintf(strsql,"delete from db where Host='%s' and Db='%s' and User='%s'",
    host.ravno(),imabaz.ravno(),login.ravno());

    sql_zapis(strsql,0,0);

    sql_zapis("FLUSH PRIVILEGES",0,0);

    /*если нет больше записей с этим логином то проверям возможность удаления в icebuser*/
    sprintf(strsql,"select User from db where Db='%s' and User='%s'",imabaz.ravno(),login.ravno());
    if(sql_readkey(&bd,strsql) > 0) /*Запись есть*/
     goto naz1;

    if(prov_uduser(login.ravno(),imabaz.ravno(),0) != 0)
     goto naz1;

    sprintf(strsql,"delete from %s.icebuser where login='%s'",imabaz.ravno(),login.ravno());
    sql_zapis(strsql,1,0);
    goto naz1;

  case FK4: /*Поиск*/
     dirduser_p(&rp);
     goto naz1;

  case FK6: /*Сортировка*/
     metka_sort++;
     if(metka_sort > 1)
      metka_sort=0;
     goto naz1;


  default:
    goto naz;
    break;
 }

return(1);

}
/*********/
/*Шапка*/
/*********/
void dirduser_sap(class dirduser_poi *rp)
{
short		y=2;
short           x=COLS-20;

clear();
printw("%s",gettext("Просмотр списка разрешённых доступов к базам данных"));



mvprintw(2,0,"   1        2     3        4         5");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("логин"));
mvprintw(y++,x,"2-%s",gettext("база"));
mvprintw(y++,x,"3-%s",gettext("кодировка"));
mvprintw(y++,x,"4-%s",gettext("хост"));
mvprintw(y++,x,"5-%s",gettext("организация"));


if(rp->metka_poi == 1)
 {
  move(y+=2,x=COLS-21);
  attron(A_BLINK);
  printw(gettext("Поиск:"));

  if(rp->login.getdlinna() > 1)
   {
    move(++y,x);
    printw("%s:%s",gettext("Логин"),rp->login.ravno());
   }
  if(rp->imabaz.getdlinna() > 1)
   {
    move(++y,x);
    printw("%s:%s",gettext("База"),rp->imabaz.ravno());
   }
  if(rp->host.getdlinna() > 1)
   {
    move(++y,x);
    printw("%s:%s",gettext("Хост"),rp->host.ravno());
   }
  attroff(A_BLINK);
 }

helstr(LINES-1,0,
//"F1",gettext("помощь"),
//"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
//"F5",gettext("печать"),
"F6",gettext("сортировка"),
"F10",gettext("выход"),NULL);
 

}

/***********************************/
/*ввод реквизитов поиска*/
/**********************************/
int dirduser_p(class dirduser_poi *rp)
{
int kom=0;

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Введите данные для поиска"));

VV.VVOD_SPISOK_add_MD(gettext("Логин оператора.."));
VV.VVOD_SPISOK_add_MD(gettext("Имя базы........."));
VV.VVOD_SPISOK_add_MD(gettext("Хост оператора..."));

VV.VVOD_SPISOK_add_data(rp->login.ravno(),112);
VV.VVOD_SPISOK_add_data(rp->imabaz.ravno(),112);
VV.VVOD_SPISOK_add_data(rp->host.ravno(),112);

naz:;

helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("поиск"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

rp->login.new_plus(VV.VVOD_SPISOK_return_data(0));
rp->imabaz.new_plus(VV.VVOD_SPISOK_return_data(1));
rp->host.new_plus(VV.VVOD_SPISOK_return_data(2));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   goto naz;

  case ESC:
  case FK10:
   rp->metka_poi=0;
   return(1);

  case FK2:
  case PLU:
     rp->metka_poi=1;
    return(0);



  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }


return(0);

}
/*************************************/
/*проверка соответствия реквизитам поиска*/
/****************************************/

int dirdousr_prov(SQL_str row,class dirduser_poi *rp)
{
if(rp->metka_poi == 0)
 return(0);
 
if(proverka(rp->login.ravno(),row[2],0,0) != 0)
 return(1);

if(proverka(rp->imabaz.ravno(),row[1],0,0) != 0)
 return(1);

if(proverka(rp->host.ravno(),row[0],0,0) != 0)
 return(1);

return(0);

}
