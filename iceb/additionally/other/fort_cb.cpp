/*$Id:$*/
/*01.03.2017	26.11.2016	Белых А.И.	fort_cb.c
Создание таблиц для расчёта нарядов
*/
#include        <locale.h>
#include        <sys/stat.h>
#include <buhg.h>

SQL_baza	bd;

int             main(int argc,char **argv,char **envp)
{
char		imabaz[20];
char		parol[20];
char		host[80];
char		kto[20];
char strsql[2048];

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
VV.VVOD_SPISOK_add_ZAG("Программа создания таблиц");

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
clear();

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
icebstart_rf();

/************************
printw("Создаем базу данных\n");
char strsql[2048];

sprintf(strsql,"create database %s",imabaz);

if(sql_zap(&bd,strsql) != 0)
  printw("%s\n",sql_error(&bd));

***************************/

if(mysql_select_db(&bd,imabaz) < 0)
 { 
  printw("%s\n",sql_error(&bd));
  OSTANOV();
  endwin();
  exit(10);
 } 

printw("Создание таблицы шапки нарядов Forthn\n");
refresh();

sprintf(strsql,"\
CREATE TABLE Forthn\
(datan DATE NOT NULL DEFAULT '0000-00-00',\
nn INT NOT NULL DEFAULT 0,\
pd INT NOT NULL DEFAULT 0,\
zak CHAR(64) NOT NULL DEFAULT '',\
isp CHAR(64) NOT NULL DEFAULT '',\
kom CHAR(128) NOT NULL DEFAULT '',\
ktoz smallint unsigned not null default 0,\
vrem int unsigned not null default 0,\
unique(datan,nn,pd)) ENGINE = MYISAM");
sql_zapis(strsql,1,0);
/*
0 datan - дата наряда, день всегда 1
1 nn    - номер наряда
2 pd    - подразделение
3 zak   - заказ
4 isp   - исполнитель
5 kom   - коментарий
6 ktoz  - кто записал
7 vrem  - время записи
*/

printw("Создание таблицы записей в наряде Fortzn\n");
refresh();
sprintf(strsql,"\
CREATE TABLE Fortzn\
(nz INT UNSIGNED AUTO_INCREMENT NOT NULL,\
datan DATE NOT NULL DEFAULT '0000-00-00',\
nn INT NOT NULL DEFAULT 0,\
pd INT NOT NULL DEFAULT 0,\
hz char(10) NOT NULL DEFAULT '',\
ts INT NOT NULL DEFAULT 0,\
nv DOUBLE(8,3) NOT NULL DEFAULT 0.,\
rc DOUBLE(8,3) NOT NULL DEFAULT 0.,\
ko INT NOT NULL DEFAULT 0.,\
kol DOUBLE(12,2) NOT NULL DEFAULT 0.,\
ktoz SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0,\
PRIMARY KEY (nz),\
INDEX(datan,nn,pd))  ENGINE = MYISAM");

sql_zapis(strsql,1,0);

/*
0  nz    - уникальный номер записи
1  datan - дата накладной, день всегда 1
2  nn    - номер накладной
3  pd    - код подразделения
4  hz    - шифр затрат
5  ts    - тарифная ставка
6  nv    - нома времени в минутах
7  rc    - расценка в копейках
8  ko    - код операции
9  kol   - количество
10 ktoz  - кто записал
11 vrem  - время записи
*/


printw("Создание таблицы незавершённого производства Fortnp\n");
refresh();

sprintf(strsql,"\
CREATE TABLE Fortnp\
(nz INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,\
ceh CHAR(20) NOT NULL DEFAULT '',\
kd INT NOT NULL DEFAULT 0,\
ei CHAR(12) NOT NULL DEFAULT '',\
kom VARCHAR(60) NOT NULL DEFAULT '',\
ktoz SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0,\
INDEX(ceh,kd)) ENGINE = MYISAM");

sql_zapis(strsql,1,0);

/*
 0 -  nz   - уникальный номер записи
 1 -  ceh   - цех
 2 -  kd   - код детали
 3 -  ei   - единица измерения
 4 -  kom  - коментарий  
 5 -  ktoz - кто записал
 6 -  vrem - время записи
*/

printw("Создание таблицы незавершённого производства Fortnpz\n");
refresh();
sprintf(strsql,"\
CREATE TABLE Fortnpz\
(nzh INT UNSIGNED NOT NULL DEFAULT 0,\
nz INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,\
data DATE NOT NULL DEFAULT '0000-00-00',\
kol DOUBLE(16,6) NOT NULL DEFAULT 0.,\
pr  FLOAT(6,2) NOT NULL DEFAULT 0.,\
kom VARCHAR(60) NOT NULL DEFAULT '',\
ktoz SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0,\
INDEX(nzh,nz))  ENGINE = MYISAM");

sql_zapis(strsql,1,0);

/*
 0 - nzh  - уникальный номер основной записи
 1 - nz   - уникальный номер записи
 2 - data - дата 
 3 - kol  - количество
 4 - pr   - процент готовности
 5 - kom  - коментарий  
 6 - ktoz - кто записал
 7 - vrem - время записи
*/

printw("Создание таблицы трудоёмкости изготовления Fortti\n");
refresh();
sprintf(strsql,"\
CREATE TABLE Fortti\
(ceh CHAR(20) NOT NULL DEFAULT '',\
kd INT UNSIGNED NOT NULL DEFAULT 0,\
min FLOAT(8,2) NOT NULL DEFAULT 0.,\
kop FLOAT(8,2) NOT NULL DEFAULT 0.,\
ktoz SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0,\
UNIQUE(ceh,kd))  ENGINE = MYISAM");


sql_zapis(strsql,1,0);

/*
 0 - ceh  - код цеха
 1 - kd   - код детали
 2 - min  - трудоёмкость в минутах
 3 - kop  - трудоёмкость в копейках
 4 - ktoz - кто записал
 5 - vrem - время записи
*/

printw("Создание таблицы справочника цехов Fortsc\n");
refresh();
sprintf(strsql,"\
CREATE TABLE Fortsc\
(kod CHAR(20) NOT NULL DEFAULT '',\
naik VARCHAR(100) NOT NULL DEFAULT '',\
ktoz SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0,\
UNIQUE(kod))  ENGINE = MYISAM");

sql_zapis(strsql,1,0);


/*
 0 - kod  - код цеха
 1 - naik - наименование цеха
 2 - ktoz - кто записал
 3 - vrem - время записи
*/


printw("Разметка базы завершена\n");
OSTANOV();

endwin();


}
