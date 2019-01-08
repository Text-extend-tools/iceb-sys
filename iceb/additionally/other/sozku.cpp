/*31.08.2017	10.06.2004	Белых 	sozku.c*/
#include <stdio.h>
#include        <pwd.h>
#include        <unistd.h>
#include        <locale.h>
#include        <sys/stat.h>
#include <iceb.h>
#include <buhg.h>

SQL_baza	bd;

int             main(int argc,char **argv,char **envp)
{
char		imabaz[20];
char		parol[20];
char		host[80];
char		kto[20];

strncpy(kto,"root",sizeof(kto)-1);
memset(parol,'\0',sizeof(parol));
memset(host,'\0',sizeof(host));
sprintf(host,"localhost");

(void)setlocale(LC_ALL,"");
(void)setlocale(LC_NUMERIC,"C"); //Переменная среды определяющая чем 
                                 //разделяется целая и дробная часть числа

init_screen();  /*Запуск экранного режима*/
VVOD MENU(4);

umask(0117); /*Установка маски для записи и чтения группы*/



VVOD  VV(0);
VV.VVOD_SPISOK_add_ZAG("Программа создания таблиц для программы xkus");

VV.VVOD_SPISOK_add_MD("Имя базы..");
VV.VVOD_SPISOK_add_MD("Хост......");

VV.VVOD_SPISOK_add_data(sizeof(imabaz));
VV.VVOD_SPISOK_add_data(host,sizeof(host));

naz:;

helstr(LINES-1,0,
//"F1","помощь",
"F2/+","создать таблицы",
"F10","выход",NULL);


int kom=VV.vvod(0,1,1,-1,-1);

strncpy(imabaz,VV.VVOD_SPISOK_return_data(0),sizeof(imabaz)-1);
strncpy(host,VV.VVOD_SPISOK_return_data(1),sizeof(host)-1);

switch(kom)
 {
  case FK1:
    goto naz;

  case FK2:
    break;

  case FK10:
     endwin();
     return(0);


 }

VVOD PAROL(2);
PAROL.VVOD_SPISOK_add_MD("Введите пароль администратора базы");
vparol(&PAROL,parol,sizeof(parol),stdscr,-1,-1,1);

if(mysql_init(&bd) == NULL)
  printw("%s\n",sql_error(&bd));

printw("Связываемся с демоном базы\n");
/*                                                   порт */
if(mysql_real_connect(&bd,host,"root",parol,"mysql",0,NULL,0) == NULL)
/*                                                   именованій порт*/
 { 
  printw("%s\n",sql_error(&bd));
  OSTANOV();
  endwin();
  exit(10);
 } 

printw("Создаем базу данных\n");
char strsql[2048];

sprintf(strsql,"create database %s",imabaz);

if(sql_zap(&bd,strsql) != 0)
  printw("%s\n",sql_error(&bd));

if(mysql_select_db(&bd,imabaz) < 0)
 { 
  printw("%s\n",sql_error(&bd));
  OSTANOV();
  endwin();
  exit(10);
 } 

printw("Создание таблицы списка квартир Kuskvart\n");
refresh();

sprintf(strsql,"\
create table Kuskvart\
(nd varchar(10) NOT NULL default '',\
nk   varchar(16) NOT NULL DEFAULT '',\
fio  varchar (100) not NULL default '',\
kj   tinyint not NULL default 0,\
hsq  float (5,2) not NULL default 0.,\
csq  float (4,2) not NULL default 0.,\
van  tinyint not NULL default 0,\
ktoi smallint unsigned not null default 0,\
vrem int unsigned not null default 0,\
rt   tinyint not null default 0,\
kk   varchar(20) not null default '',\
np   float(6,2) not null default 0.,\
mo   tinyint not null default 0,\
sv   tinyint not null default 0,\
unique(nd,nk))");

/*
0 nd - Номер дома
1 nk - номер квартиры
2 fio - Фамилия, имя, отчество
3 kj -  Количество жильцов
4 hsq - Площадь квартиры отапливаемая
5 csq - Площадь кухни
6 van - Наличие ванны (0 - ванны нету, 1 - ванна есть)
7 ktoi - Кто записал
8 vrem - Время этой записи
9 rt   - радиоточка 0-нет 1-есть
10 kk - код контрагента
11 np - неотапливаемая площадь
12 mo - 0-общее отопление 1-индивидуальное
13 sv - 0-счётчика воды нет 1-есть
*/

sql_zapis(strsql,1,0);

printw("Создание таблицы показаний счётчиков по квартирам Kuspm\n");
refresh();

sprintf(strsql,"create table Kuspm \
(nd varchar(10) NOT NULL default '',\
nk varchar(16) NOT NULL DEFAULT '',\
data DATE not NULL default '0000-00-00',\
ps   int not NULL default 0,\
ktoi smallint unsigned not null default 0,\
vrem int unsigned not null default 0,\
voda float(6,2) not null default 0.,\
unique(nd,nk,data))");

/*
0 nd - Номер дома
1 nk - Номер квартиры
2 data - дата показаний
3 ps - Показания счётчика электричества
4 ktoi - Кто записал
5 vrem - Время этой записи
6 voda - показания счётчиков по воде
*/

sql_zapis(strsql,1,0);

printw("Создание таблицы льгот Kuslg\n");
refresh();

sprintf(strsql,"create table Kuslg \
(nd varchar(10) NOT NULL default '',\
nk varchar(16) NOT NULL DEFAULT '',\
dt DATE not NULL default '0000-00-00',\
ub double (10,2) NOT NULL DEFAULT 0.,\
op double (10,2) NOT NULL DEFAULT 0.,\
ee double (10,2) NOT NULL DEFAULT 0.,\
vd double (10,2) NOT NULL DEFAULT 0.,\
gz double (10,2) NOT NULL DEFAULT 0.,\
ktoi smallint unsigned not null default 0,\
vrem int unsigned not null default 0,\
unique(nd,nk))");

/*
0 nd - Номер дома
1 nk - Номер квартиры
2 dt - дата начала действия льгот
3 ub - утримання будинку
4 op - опалення
5 ee - електро-енергия
6 vd - вода
7 gz - газ
8 ktoi - Кто записал
9 vrem - Время этой записи
*/

//printw ("%s\n", strsql);
sql_zapis(strsql,1,0);

sql_closebaz(&bd);

printw("Разментка базы завершена\n");
OSTANOV();

endwin();

}
