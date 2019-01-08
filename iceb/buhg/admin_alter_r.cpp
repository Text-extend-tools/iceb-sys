/*$Id: admin_alter_r.c,v 5.59 2014/08/31 06:18:17 sasa Exp $*/
/*07.12.2018	09.07.2010	Белых А.И.	admin_alter_r.c
Перобразование базы данных
*/
#define DVERSIQ "27.04.2018"
#include        <pwd.h>
#include <errno.h>
#include <sys/stat.h>
#include "buhg.h"

void alter_uosgrup_ta(const char *imabaz);
void rnn_ko(int metka,WINDOW *win1);
void alter_cp_alx(int metka,const char *imaf,const char *imabaz,const char *put_na_alx,WINDOW *win1);
void alter_cat_str(const char *stroka,const char *param,const char *imaf,WINDOW *win1);
void alter_icebuser(const char *imabaz,WINDOW *win1);
void admin_alter_r_zzb(const char *imabazz);
void admin_alter_r_alxin(const char *imabazz,WINDOW *win1);
void admin_alter_r_ins_str_alx(const char *imaf,const char *naststr,const char *znahnast,WINDOW *win1);
void admin_alter_r_nnni();
void admin_alter_r_krdh(const char *putalx,WINDOW *win1);
void admin_alter_r_svd(const char *putalx,WINDOW *win1);
void admin_alter_r_mzpm(const char *putalx,WINDOW *win1);
int admin_alter_r_crtb(const char *strsql,WINDOW *win1);
int admin_alter_r_vnk(const char *strsql,WINDOW *win1);
void admin_alter_r_sdzz(const char *putalx,WINDOW *win1);
void admin_alter_r_zrud(const char *putalx,WINDOW *win1);
void admin_alter_r_zsl(const char *putalx,WINDOW *win1);
void admin_alter_r_zargr(const char *putalx,WINDOW *win1);
void admin_alter_r_murashod();
void admin_alter_r_uslrashod();
void admin_alter_r_vnst0(WINDOW *win1);
void admin_alter_r_vnst(WINDOW *win1);
void admin_alter_r_vnstpl(WINDOW *win1);
void admin_alter_r_upldok2a(WINDOW *win1);
const char *iceb_t_imafnsi(const char *imaf);
int admin_alter_r_amort(const char *tabl,WINDOW *win1);
void admin_alter_r_zpktn(WINDOW *win1);
void admin_alter_r_skkvp(WINDOW *win1);
void admin_alter_r_zdo();
void admin_alter_r_ustdat(WINDOW *win1);

extern char *putnansi;
extern char *imabaz;

int admin_alter_r(const char *imabazz,
int kolvsehbaz,
const char *put_na_alx,
const char *put_na_doc)
{
char		strsql[4096];
WINDOW		*win1;
static int	nombaz=0;
class iceb_tu_str bros("");
SQL_str row;
class SQLCURSOR cur;

clear();
GDITE();

/*имя базы используется в подпрограммах глобально*/
if(imabaz != NULL)
 delete [] imabaz;

imabaz=new char[strlen(imabazz)+1];
strcpy(imabaz,imabazz);

sprintf(strsql,"USE %s",imabazz);
if(sql_zapis(strsql,1,0) != 0)
 return(1);

icebstart_rf(); //чтение и выполнение запросов к базе

if((win1=subwin(stdscr,LINES-3,COLS,3,0)) == (WINDOW*)ERR)
 {
  printw("\nОшибка создания окна win1!\n");
//  perror("*******");
  OSTANOV();
  return(1);
 }
scrollok(win1,TRUE); /*Разрешение ролинга на экране*/
move(0,0);
printw("%s N%d %s %s:%d\n",
gettext("Преобразование базы"),
++nombaz,imabazz,
gettext("Количество всех баз"),
kolvsehbaz);
printw("%s %s от %s\n",gettext("Версия"),VERSION,DVERSIQ);
printw("-----------------------------------------------------------------------\n");
refresh();

/*проверяем дату последненего обновления базы и перепрыгиваем через ненужный код*/
sprintf(strsql,"select str from Alx where fil='iceb_data_baz' and ns=1");
if(readkey(strsql,&row,&cur) == 1)
 {
  if(SRAV("13.8.2012",row[0],0) == 0)
   goto jump13082012;
  if(SRAV("05.09.2012",row[0],0) == 0)
   goto jump05092012;
  if(SRAV("06.11.2012",row[0],0) == 0)
   goto jump06112012;
  if(SRAV("28.11.2012",row[0],0) == 0)
   goto jump28112012;
  if(SRAV("30.11.2012",row[0],0) == 0)
   goto jump30112012;
  if(SRAV("05.12.2012",row[0],0) == 0)
   goto jump05122012;
  if(SRAV("02.01.2013",row[0],0) == 0)
   goto jump02012013;
  if(SRAV("11.01.2013",row[0],0) == 0)
   goto jump11012013;
  if(SRAV("11.02.2013",row[0],0) == 0)
   goto jump11022013;

  if(SRAV("06.03.2013",row[0],0) == 0)
   goto jump06032013;
  if(SRAV("05.04.2013",row[0],0) == 0)
   goto jump05042013;
  if(SRAV("21.06.2013",row[0],0) == 0)
   goto jump21062013;
  if(SRAV("04.09.2013",row[0],0) == 0)
   goto jump04092013;
  if(SRAV("18.09.2013",row[0],0) == 0)
   goto jump18092013;
  if(SRAV("21.09.2013",row[0],0) == 0)
   goto jump21092013;
  if(SRAV("28.09.2013",row[0],0) == 0)
   goto jump28092013;
  if(SRAV("31.10.2013",row[0],0) == 0)
   goto jump31102013;

  if(SRAV("24.11.2013",row[0],0) == 0)
   goto jump24112013;
  if(SRAV("27.12.2013",row[0],0) == 0)
   goto jump27122013;
  if(SRAV("10.01.2014",row[0],0) == 0)
   goto jump10012014;
  if(SRAV("29.01.2014",row[0],0) == 0)
   goto jump29012014;
  if(SRAV("20.02.2014",row[0],0) == 0)
   goto jump20022014;
  if(SRAV("25.02.2014",row[0],0) == 0)
   goto jump25022014;
  if(SRAV("12.03.2014",row[0],0) == 0)
   goto jump12032014;
  if(SRAV("08.04.2014",row[0],0) == 0)
   goto jump08042014;
  if(SRAV("01.05.2014",row[0],0) == 0)
   goto jump01052014;
  if(SRAV("14.05.2014",row[0],0) == 0)
   goto jump14052014;
  if(SRAV("30.05.2014",row[0],0) == 0)
   goto jump30052014;
  if(SRAV("03.06.2014",row[0],0) == 0)
   goto jump03062014;
  if(SRAV("03.07.2014",row[0],0) == 0)
   goto jump03072014;
  if(SRAV("19.07.2014",row[0],0) == 0)
   goto jump19072014;
  if(SRAV("31.07.2014",row[0],0) == 0)
   goto jump31072014;
  if(SRAV("12.08.2014",row[0],0) == 0)
   goto jump12082014;
  if(SRAV("28.08.2014",row[0],0) == 0)
   goto jump28082014;
  if(SRAV("27.11.2014",row[0],0) == 0)
   goto jump27112014;
  if(SRAV("26.01.2015",row[0],0) == 0)
   goto jump26012015;
  if(SRAV("01.02.2015",row[0],0) == 0)
   goto jump01022015;
  if(SRAV("09.02.2015",row[0],0) == 0)
   goto jump09022015;
  if(SRAV("02.03.2015",row[0],0) == 0)
   goto jump02032015;
  if(SRAV("21.04.2015",row[0],0) == 0)
   goto jump21042015;
  if(SRAV("09.05.2015",row[0],0) == 0)
   goto jump09052015;
  if(SRAV("03.07.2015",row[0],0) == 0)
   goto jump03072015;
  if(SRAV("28.07.2015",row[0],0) == 0)
   goto jump28072015;
  if(SRAV("01.09.2015",row[0],0) == 0)
   goto jump01092015;
  if(SRAV("07.09.2015",row[0],0) == 0)
   goto jump07092015;
  if(SRAV("22.09.2015",row[0],0) == 0)
   goto jump22092015;
  if(SRAV("02.10.2015",row[0],0) == 0)
   goto jump02102015;
  if(SRAV("03.12.2015",row[0],0) == 0)
   goto jump03122015;
  if(SRAV("12.01.2016",row[0],0) == 0)
   goto jump12012016;
  if(SRAV("18.01.2016",row[0],0) == 0)
   goto jump18012016;
  if(SRAV("04.02.2016",row[0],0) == 0)
   goto jump04022016;
  if(SRAV("08.02.2016",row[0],0) == 0)
   goto jump08022016;
  if(SRAV("24.02.2016",row[0],0) == 0)
   goto jump24022016;
  if(SRAV("09.03.2016",row[0],0) == 0)
   goto jump09032016;
  if(SRAV("16.03.2016",row[0],0) == 0)
   goto jump16032016;
  if(SRAV("30.03.2016",row[0],0) == 0)
   goto jump30032016;
  if(SRAV("01.04.2016",row[0],0) == 0)
   goto jump01042016;
  if(SRAV("25.05.2016",row[0],0) == 0)
   goto jump25052016;
  if(SRAV("31.05.2016",row[0],0) == 0)
   goto jump31052016;
  if(SRAV("24.06.2016",row[0],0) == 0)
   goto jump24062016;
  if(SRAV("16.11.2016",row[0],0) == 0)
   goto jump16112016;
  if(SRAV("22.11.2016",row[0],0) == 0)
   goto jump22112016;
  if(SRAV("24.11.2016",row[0],0) == 0)
   goto jump24112016;
  if(SRAV("13.02.2017",row[0],0) == 0)
   goto jump13022017;
  if(SRAV("24.02.2017",row[0],0) == 0)
   goto jump24022017;
  if(SRAV("06.03.2017",row[0],0) == 0)
   goto jump06032017;
  if(SRAV("22.03.2017",row[0],0) == 0)
   goto jump22032017;
  if(SRAV("18.05.2017",row[0],0) == 0)
   goto jump18052017;
  if(SRAV("04.09.2017",row[0],0) == 0)
   goto jump04092017;
  if(SRAV("26.09.2017",row[0],0) == 0)
   goto jump26092017;
  if(SRAV("09.01.2018",row[0],0) == 0)
   goto jump09012018;
  if(SRAV("10.01.2018",row[0],0) == 0)
   goto jump10012018;
  if(SRAV("27.04.2018",row[0],0) == 0)
   goto jump27042018;
 }
//printw("row[0]=%s\n",row[0]);
//OSTANOV();
/****************************************/
/*********** 9.9 05.11.2008 ************/
/****************************************/

wprintw(win1,"Добавляем индекс nb в таблицу Kasord\n");
wrefresh(win1);

sprintf(strsql,"alter table Kasord add index nb (nb)");

if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_KEYNAME)
   {
    wprintw(win1,"Индекс уже существует\n");
    wrefresh(win1);
   }
 else
  wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));

 }
/****************************************/
/*********** 9.12 29.11.2008 ************/
/****************************************/
wprintw(win1,"%s mi %s Uospod\n",gettext("Вставляем колонку"),gettext("в таблицу"));
wrefresh(win1);

sprintf(strsql,"alter table Uospod add mi tinyint not null default 0");
admin_alter_r_vnk(strsql,win1);

/****************************************/
/*********** 9.15 10.02.2009 ************/
/****************************************/
wprintw(win1,"Создание таблицы перечня драг-металлов Uosdm !\n");
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosdm \
(kod    smallint not null default 0 primary key,\
naik    varchar(100) not null default '',\
ei      varchar(20) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0) ENGINE = MYISAM");
/*
0 kod    Код драгоценного металла
1 naik   Наименование металла
2 ei     Единица измерения
2 ktoz   Кто записал
3 vrem   Время записи
*/
admin_alter_r_crtb(strsql,win1);


wprintw(win1,"Создание таблицы содержимого драг-металлов в инвентарном номере Uosindm !\n");
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosindm \
(innom int not null,\
kod    smallint not null default 0,\
ei     varchar(20) not null default '',\
ves    double(12,6) not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
unique(innom,kod)) ENGINE = MYISAM");
/*
0 innom    инвентарный номер
1 kod    Код драгоценного металла
2 ei     Единица измерения
3 ves    вес материала
4 ktoz   Кто записал
5 vrem   Время записи
*/

admin_alter_r_crtb(strsql,win1);

/****************************************/
/*********** 9.20 30.03.2009 ************/
/****************************************/
wprintw(win1,"Создание таблицы заголовков доверенностей Uddok !\n");
wrefresh(win1);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uddok \
(datd   DATE not null default '0000-00-00',\
nomd    varchar(20) not null default '',\
datds   DATE NOT NULL default '0000-00-00',\
komu    varchar(80) NOT NULL default '',\
post    varchar(80) NOT NULL default '',\
podok   varchar(50) NOT NULL default '',\
otis    varchar(100) NOT NULL default '',\
datn    DATE NOT NULL default '0000-00-00',\
nomn    varchar(100) NOT NULL default '',\
seriq varchar(20) NOT NULL default '',\
nomerd varchar(20) NOT NULL default '',\
datavd DATE NOT NULL default '0000-00-00',\
vidan  varchar(200) NOT NULL default '',\
dolg   varchar(50) NOT NULL default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
unique(datd,nomd),\
index(nomd)) ENGINE = MYISAM");

admin_alter_r_crtb(strsql,win1);

wrefresh(win1);
wprintw(win1,"Создание таблицы записей в доверенности Uddok1 !\n");
wrefresh(win1);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uddok1 \
(datd   DATE not null default '0000-00-00',\
nomd    varchar(20) not null default '',\
nz      smallint NOT NULL default 0,\
zapis   varchar(200) NOT NULL default '',\
ei      varchar(20) NOT NULL default '',\
kol     double(16,4) NOT NULL default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
unique(datd,nomd,nz)) ENGINE = MYISAM");
/*
0  datd Дата выдачи доверенности
1  nomd номер доверенности
2  nz    уникальный номер записи
3  zapis содержимое записи
4  ei    единица измерения
5  kol   количество
6  ktoz  кто записал
7  vrem  время записи
*/

admin_alter_r_crtb(strsql,win1);


/****************************************/
/*********** 9.24 12.04.2009 ************/
/****************************************/
wprintw(win1,"Удаляем колонку kof1 из Таблицы Uosgrup1\n");
wrefresh(win1);

sprintf(strsql,"alter table Uosgrup drop kof1");

if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) != ER_CANT_DROP_FIELD_OR_KEY)
    wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
  else
    wprintw(win1,"Колонка уже удалена\n");

 }


wprintw(win1,"Вставляем колонки ta,ar в таблицу Uosgrup\n");
wrefresh(win1);

sprintf(strsql,"alter table Uosgrup add ta tinyint not null default 0, add ar tinyint not null default 0");
admin_alter_r_vnk(strsql,win1);


wprintw(win1,"Вставляем колонку ar в таблицу Uosgrup1\n");
wrefresh(win1);

sprintf(strsql,"alter table Uosgrup1 add ar tinyint not null default 0");
if(admin_alter_r_vnk(strsql,win1) == 0)
 {
  alter_uosgrup_ta(imabazz);

 }
/****************************************/
/*********** 9.29 29.05.2009 ************/
/****************************************/

wprintw(win1,"Увеличиваем длинну колонки osnov в таблице Kasord\n");
wrefresh(win1);

sprintf(strsql,"alter table Kasord change osnov osnov varchar(250) not null default ''");

if(sql_zap(&bd,strsql) != 0)
 {
  wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
  OSTANOVW(win1);
 }

/****************************************/
/*********** 9.30 01.06.2009 ************/
/****************************************/

wprintw(win1,"Увеличиваем длинну колонки naik в таблице Kasop1\n");
wrefresh(win1);

sprintf(strsql,"alter table Kasop1 change naik naik varchar(250) not null default ''");

if(sql_zap(&bd,strsql) != 0)
 {
  wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
  OSTANOVW(win1);
 }

wprintw(win1,"Увеличиваем длинну колонки naik в таблице Kasop2\n");
wrefresh(win1);

sprintf(strsql,"alter table Kasop2 change naik naik varchar(250) not null");

if(sql_zap(&bd,strsql) != 0)
 {
  wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
  OSTANOVW(win1);
 }

/****************************************/
/*********** 9.31 09.06.2009 ************/
/****************************************/

wprintw(win1,"Меняем тип колонки nomv в таблице Kasnomved\n");
wrefresh(win1);

sprintf(strsql,"alter table Kasnomved change nomv nomv varchar(10) not null default ''");

if(sql_zap(&bd,strsql) != 0)
 {
  wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
  OSTANOVW(win1);
 }

/****************************************/
/*********** 9.35 23.06.2009 ************/
/****************************************/

wprintw(win1,"Добавляем колонку koment в таблицу Kasord1\n");
wrefresh(win1);

sprintf(strsql,"alter table Kasord1 add koment varchar(100) not null default ''");

admin_alter_r_vnk(strsql,win1);

/****************************************/
/*********** 9.37 29.06.2009 ************/
/****************************************/

wprintw(win1,"Добавляем колонку nomv в таблицу Kasord\n");
wrefresh(win1);

sprintf(strsql,"alter table Kasord add nomv varchar(20) not null default ''");

admin_alter_r_vnk(strsql,win1);

wprintw(win1,"Добавляем колонку nomd в таблицу Kasnomved\n");
wrefresh(win1);

sprintf(strsql,"alter table Kasnomved add nomd varchar(20) not null default ''");

admin_alter_r_vnk(strsql,win1);



/****************************************/
/*********** 10.2 09.09.2009 ************/
/****************************************/

wprintw(win1,"Создание таблицы списка банков Zarsb !\n");
wrefresh(win1);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zarsb \
(kod    int not null default 0 primary key,\
naik    varchar(80) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0) ENGINE = MYISAM");
/*
0 kod   код банка
1 naik  наименование банка
2 ktoz  кто записал
3 vrem  время записи
*/

admin_alter_r_crtb(strsql,win1);

wprintw(win1,"Добавляем колонку kb в таблицу  Kartb\n");
wrefresh(win1);

sprintf(strsql,"alter table Kartb add kb int not null default 0");

admin_alter_r_vnk(strsql,win1);

/****************************************/
/*********** 10.5 12.10.2009 ************/
/****************************************/
wprintw(win1,"Добавляем колонку mo в таблицу Dokummat\n");
wrefresh(win1);

sprintf(strsql,"alter table Dokummat add mo tinyint not null default 0");

admin_alter_r_vnk(strsql,win1);

wprintw(win1,"Добавляем колонку mo в таблицу Usldokum\n");
wrefresh(win1);

sprintf(strsql,"alter table Usldokum add mo tinyint not null default 0");

admin_alter_r_vnk(strsql,win1);

wprintw(win1,"Добавляем колонку mo в таблицу Uosdok\n");
wrefresh(win1);

sprintf(strsql,"alter table Uosdok add mo tinyint not null default 0");

admin_alter_r_vnk(strsql,win1);

/****************************************/
/*********** 10.10 03.12.2009 ************/
/****************************************/
wprintw(win1,"Добавляем колонку vd в таблицу Reenn\n");
wrefresh(win1);

sprintf(strsql,"alter table Reenn add vd char(8) not null default ''");

admin_alter_r_vnk(strsql,win1);

wprintw(win1,"Добавляем колонку vd в таблицу Reenn1\n");
wrefresh(win1);

sprintf(strsql,"alter table Reenn1 add vd char(8) not null default ''");

admin_alter_r_vnk(strsql,win1);

wprintw(win1,"Создание таблицы списка картсчетов работников Zarkh !\n");
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zarkh (\
tn int not null default 0,\
kb  int not null default 0,\
nks varchar(40) not null default '',\
ktoz smallint unsigned not null default 0,\
vrem int unsigned not null default 0,\
unique(tn,kb)) ENGINE = MYISAM");
/****
0 tn - табельный номер
1 kb - код банка
2 nks - номер карт-счёта
3 ktoz кто записал
4 vrem время записи
****/
admin_alter_r_crtb(strsql,win1);

/****************************************/
/*********** 10.10 30.01.2010 ************/
/****************************************/
/**************
alter_cp_alx(0,"rpvnn0_1.alx",imabazz,put_na_alx,win1);
alter_cp_alx(0,"rpvnn1_1.alx",imabazz,put_na_alx,win1);
alter_cp_alx(0,"rpvnn2_1.alx",imabazz,put_na_alx,win1);
alter_cp_alx(0,"rnnovdp.alx",imabazz,put_na_alx,win1);
alter_cp_alx(0,"rnnovdr.alx",imabazz,put_na_alx,win1);

alter_cp_alx(0,"zarbanks.alx",imabazz,put_na_alx,win1);
alter_cp_alx(0,"zarsrbold.alx",imabazz,put_na_alx,win1);
alter_cp_alx(0,"zarpensm.alx",imabazz,put_na_alx,win1);
alter_cp_alx(0,"zarindexv.alx",imabazz,put_na_alx,win1);
****************/
/****************************************/
/*********** 10.11 01.02.2010 ************/
/****************************************/
wprintw(win1,"Изменям размерность в колонке ves для таблицы Uosindm\n");
wrefresh(win1);

sprintf(strsql,"alter table Uosindm change ves ves double(13,8) not null default 0");

if(sql_zap(&bd,strsql) != 0)
 {
  wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
  OSTANOVW(win1);
 }


/****************************************/
/*********** 10.11 04.02.2010 ************/
/****************************************/
//alter_cat_str("Код филиала пенсионного фонда","|","zarpensm.alx",win1);


/****************************************/
/*********** 10.22 22.02.2010 ************/
/****************************************/
wprintw(win1,"Добавляем колонку kss в таблицу Kartb\n");
wrefresh(win1);

sprintf(strsql,"alter table Kartb add kss varchar(16) not null default ''");

admin_alter_r_vnk(strsql,win1);

//alter_cat_str("Коды звания неатестованых","|","zarpensm.alx",win1);

/****************************************/
/*********** 10.28 22.03.2010 ************/
/****************************************/
//alter_cp_alx(0,"nastud.alx",imabazz,put_na_alx,win1);


/****************************************/
/*********** 10.32 20.06.2010 ************/
/****************************************/
/**********
sprintf(strsql,"alter table Uosdok change podrv podrv smallint not null");

if(sql_zap(&bd,strsql) != 0)
 {
  wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
  OSTANOVW(win1);
 }
sprintf(strsql,"alter table Uosdok change osnov osnov varchar(60) not null");

if(sql_zap(&bd,strsql) != 0)
 {
  wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
  OSTANOVW(win1);
 }
***************/
/****************************************/
/*********** 10.35 05.08.2010 ************/
/****************************************/

wprintw(win1,"Добавляем колонку ss в таблицу Ukrdok1\n");
wrefresh(win1);
sprintf(strsql,"alter table Ukrdok1 add ss double(10,2) not null default 0");

admin_alter_r_vnk(strsql,win1);
//alter_cp_alx(1,"nakl.alx",imabazz,put_na_alx,win1);

/****************************************/
/*********** 11.00 01.09.2010 ************/
/****************************************/
wprintw(win1,"Создание таблицы операторов icebuser !\n");
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE icebuser \
(login char(32) not null default '' primary key,\
fio varchar(255) not null default '',\
un  int unsigned not null default 0,\
kom varchar(255) not null default '',\
logz   varchar(32) not null default '',\
vrem   int unsigned not null default 0,\
gkd tinyint not null default 0,\
gkb tinyint not null default 0,\
gkn tinyint not null default 0,\
mud tinyint not null default 0,\
mub tinyint not null default 0,\
mun tinyint not null default 0,\
pdd tinyint not null default 0,\
pdb tinyint not null default 0,\
pdn tinyint not null default 0,\
zpd tinyint not null default 0,\
zpb tinyint not null default 0,\
zpn tinyint not null default 0,\
osd tinyint not null default 0,\
osb tinyint not null default 0,\
osn tinyint not null default 0,\
uud tinyint not null default 0,\
uub tinyint not null default 0,\
uun tinyint not null default 0,\
kod tinyint not null default 0,\
kob tinyint not null default 0,\
kon tinyint not null default 0,\
krd tinyint not null default 0,\
krb tinyint not null default 0,\
krn tinyint not null default 0,\
pld tinyint not null default 0,\
plb tinyint not null default 0,\
pln tinyint not null default 0,\
nnd tinyint not null default 0,\
nnb tinyint not null default 0,\
nnn tinyint not null default 0,\
udd tinyint not null default 0,\
udb tinyint not null default 0,\
udn tinyint not null default 0,\
unique(un)) ENGINE = MYISAM");

/*
 0 login  логин оператора
 1 fio    фамилия имя отчество
 2 un     уникальный номер оператора
 3 kom    коментарий
 4 logz   логин сделавшего запись
 5 vrem   Время записи
 6 gkd    если равно 0 то разрешена работа с подсистемой "Главная книга"
 7 gkb    если равно 0 то разрешена блокировка подсистемы "Главная книга"
 8 gkn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Главная книга"
 9 mud    если равно 0 то разрешена работа с подсистемой "Материальный учёт"
10 mub    если равно 0 то разрешена блокировка подсистемы "Материальный учёт"
11 mun    если равно 0 то разрешена работа с настроечными файлами подсистемы "Материальный учёт"
12 pdd    если равно 0 то разрешена работа с подсистемой "Платёжные документы"
13 pdb    если равно 0 то разрешена блокировка подсистемы "Платёжные документы"
14 pdn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Платёжные документы"
15 zpd    если равно 0 то разрешена работа с подсистемой "Заработная плата"
16 zpb    если равно 0 то разрешена блокировка подсистемы "Заработная плата"
17 zpn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Заработная плата"
18 osd    если равно 0 то разрешена работа с подсистемой "Учёт основных средств"
19 osb    если равно 0 то разрешена блокировка подсистемы "Учёт основных средств"
20 osn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Учёт основных средств"
21 uud    если равно 0 то разрешена работа с подсистемой "Учёт услуг"
22 uub    если равно 0 то разрешена блокировка подсистемы "Учёт услуг"
23 uun    если равно 0 то разрешена работа с настроечными файлами подсистемы "Учёт услуг"
24 kod    если равно 0 то разрешена работа с подсистемой "Учёт кассовых ордеров"
25 kob    если равно 0 то разрешена блокировка подсистемы "Учёт кассовых ордеров"
26 kon    если равно 0 то разрешена работа с настроечными файлами подсистемы "Учёт кассовых ордеров"
27 krd    если равно 0 то разрешена работа с подсистемой "Учёт командировочных расходов"
28 krb    если равно 1 то разрешена блокировка подсистемы "Учёт командировочных расходов"
29 krn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Учёт командироваочных расходов"
30 pld    если равно 0 то разрешена работа с подсистемой "Учёт путевых листов"
31 plb    если равно 0 то разрешена блокировка подсистемы "Учёт путувых листов"
32 pln    если равно 0 то разрешена работа с настроечными файлами подсистемы "Учёт путевых листов"
33 nnd    если равно 0 то разрешена работа с подсистемой "Реестр налоговых накладных"
34 nnb    если равно 0 то разрешена блокировка подсистемы "Реестр налоговых накладных"
35 nnn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Реестр налоговых накладных"
36 udd    если равно 0 то разрешена работа с подсистемой "Учёт доверенностей"
37 udb    если равно 0 то разрешена блокировка подсистемы "Учёт доверенностей"
38 udn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Учёт доверенностей"
*/

admin_alter_r_crtb(strsql,win1);

wprintw(win1,"Создание таблицы блокировок Blok !\n");
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Blok \
(kod int not null default 0,\
god smallint unsigned not null default 0,\
mes smallint unsigned not null default 0,\
log varchar(255) not null default '',\
shet varchar(255) not null default '',\
ktoz int not null default 0,\
vrem int unsigned not null default 0,\
unique(kod,god,mes)) ENGINE = MYISAM");


/*
0 kod  - код подсистемы
1 god  - год
2 mes  - месяц
3 log  - логины операторов которым отменена блокировка
4 shet - счёта которые заблокированы (только для подсистемы "Главная книга*)
5 ktoz - номер оператора сделавшего запись
6 vrem - время записи
*/

admin_alter_r_crtb(strsql,win1);

wprintw(win1,"Создание таблицы настроек Alx !\n");
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Alx \
(fil varchar(255) not null default '',\
ns int not null default 0,\
ktoz int not null default 0,\
vrem int unsigned not null default 0,\
str TEXT not null default '',\
unique(fil,ns)) ENGINE = MYISAM");
/*
0 fil  - имя файла
1 ns   - номер строки
2 ktoz - номер оператора сделавшего запись
3 vrem - время записи
4 str  - строка текста
*/

admin_alter_r_crtb(strsql,win1);


wprintw(win1,"Создание таблицы документации Doc !\n");
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Doc \
(fil varchar(255) not null default '',\
ns int not null default 0,\
str TEXT not null default '',\
unique(fil,ns)) ENGINE = MYISAM");
/*
0 fil  - имя файла
1 ns   - номер строки
2 str  - строка текста
*/


admin_alter_r_crtb(strsql,win1);


wprintw(win1,"Создание таблицы налогов Nalog !\n");
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Nalog \
(vn int not null default 0,\
dndn DATE not null default 0,\
pr double not null default 0,\
kom varchar(255) not null default '',\
ktoz int not null default 0,\
vrem int unsigned not null default 0,\
unique(vn,dndn)) ENGINE = MYISAM");
/*
0 vn   - вид налога 0-НДС
1 dndn - Дата начала действия налога
2 pr   - процент
3 kom  - коментарий
4 ktoz - номер оператора сделавшего запись
5 vrem - время записи
*/


if(admin_alter_r_crtb(strsql,win1) == 0)
 {
  sprintf(strsql,"insert into Nalog values (0,'1990-1-1',20.,'',%d,%ld)",
  getuid(),time(NULL));

  sql_zapis(strsql,1,0);

  sprintf(strsql,"delete from Alx where fil='matnast.alx' and str like 'Н.Д.С.|%%'");
  sql_zapis(strsql,1,0);
 }


/****************************************/
/*********** 11.2 04.11.2010 ************/
/****************************************/
wprintw(win1,"Добавляем колонку datv в таблицу Reenn\n");
wrefresh(win1);
sprintf(strsql,"alter table Reenn add datv DATE not null default '0000-00-00'");

admin_alter_r_vnk(strsql,win1);

/****************************************/
/*********** 11.6 06.12.2010 ************/
/****************************************/

admin_alter_r_ins_str_alx("nastud.alx","Печать доверенности на одной стороне листа","Выключено",win1);

sprintf(strsql,"delete from Alx where fil='zarnast.alx' and str like 'Процент налога на добавленную стоимость|%%'");
sql_zapis(strsql,1,0);

sprintf(strsql,"deleteh from Alx where fil='zarnast.alx' and str like 'Список логинов доступ в программу которым разрешен|%%'");
sql_zapis(strsql,1,0);

/****************************************/
/*********** 11.10 14.01.2011 ************/
/****************************************/

wprintw(win1,"Добавляем колонку nzv в таблицу Zarsocz\n");
wrefresh(win1);
sprintf(strsql,"alter table Zarsocz add nz int unsigned PRIMARY KEY AUTO_INCREMENT NOT NULL");

if(admin_alter_r_vnk(strsql,win1) == 0)
 {
  sprintf(strsql,"alter table Zarsocz drop index datz");
  sql_zapis(strsql,1,0);
  sprintf(strsql,"alter table Zarsocz add index (datz,tabn,kodz)");
  sql_zapis(strsql,1,0);

 }
wprintw(win1,"Добавляем колонку shetb в таблицу Uder\n");
wrefresh(win1);
sprintf(strsql,"alter table Uder add shetb varchar(32) NOT NULL default ''");

admin_alter_r_vnk(strsql,win1);

wprintw(win1,"Добавляем колонку shetb в таблицу Zarsoc\n");
wrefresh(win1);
sprintf(strsql,"alter table Zarsoc add shetb varchar(32) NOT NULL default ''");

admin_alter_r_vnk(strsql,win1);


wprintw(win1,"Создание таблицы настроек Zaresv !\n");
wrefresh(win1);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zaresv \
(kf int not null default 0,\
datnd DATE not null default '0000-00-00',\
pr float not null default 0.,\
pr1 float not null default 0.,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
unique(kf,datnd)) ENGINE = MYISAM");
/*
0 kf    - код фонда
1 datnd - дата начала действия
2 pr    - процент начисления на зарплату
3 pr1   - прдцент с человека
4 ktoz  - кто записал
5 vrem  - время записи
*/


admin_alter_r_crtb(strsql,win1);


if(iceb_t_poldan("Код фонда единого социального взноса",strsql,"zarnast.alx") != 0)
 {
  int nom=0;
  for(nom=1;; nom++)
   {
    sprintf(strsql,"select kod from Zarsoc where kod=%d",nom);
    if(readkey(strsql) == 0)
     break;
   }
  sprintf(strsql,"insert into Zarsoc (kod,naik,shet,proc,kodn,ktoz,vrem) values (%d,'Єдиний соціальний внесок','',0.,'',%d,%ld)",nom,iceb_t_getuid(),time(NULL));

  sql_zapis(strsql,1,0);

  sprintf(strsql,"%d",nom);
  admin_alter_r_ins_str_alx("zarnast.alx","Код фонда единого социального взноса",strsql,win1);

  sprintf(strsql,"insert into Zaresv (kf,datnd,pr,pr1,ktoz,vrem) values (%d,'2011-1-1',38.52,3.6,%d,%ld)",nom,iceb_t_getuid(),time(NULL));
  sql_zapis(strsql,1,0);
 }

if(iceb_t_poldan("Код фонда единого социального взноса для инвалидов",strsql,"zarnast.alx") != 0)
 {
  int nom=0;
  for(nom=1;; nom++)
   {
    sprintf(strsql,"select kod from Zarsoc where kod=%d",nom);
    if(readkey(strsql) == 0)
     break;
   }
  sprintf(strsql,"insert into Zarsoc (kod,naik,shet,proc,kodn,ktoz,vrem) values (%d,'Єдиний соціальний внесок для інвалідів','',0.,'',%d,%ld)",nom,iceb_t_getuid(),time(NULL));

  sql_zapis(strsql,1,0);

  sprintf(strsql,"%d",nom);
  admin_alter_r_ins_str_alx("zarnast.alx","Код фонда единого социального взноса для инвалидов",strsql,win1);

  sprintf(strsql,"insert into Zaresv (kf,datnd,pr,pr1,ktoz,vrem) values (%d,'2011-1-1',8.41,2.85,%d,%ld)",nom,iceb_t_getuid(),time(NULL));
  sql_zapis(strsql,1,0);
 }

if(iceb_t_poldan("Код фонда единого социального взноса для больничных",strsql,"zarnast.alx") != 0)
 {
  int nom=0;
  for(nom=1;; nom++)
   {
    sprintf(strsql,"select kod from Zarsoc where kod=%d",nom);
    if(readkey(strsql) == 0)
     break;
   }
  sprintf(strsql,"insert into Zarsoc (kod,naik,shet,proc,kodn,ktoz,vrem) values (%d,'Єдиний соціальний внесок для лікарняних','',0.,'',%d,%ld)",nom,iceb_t_getuid(),time(NULL));

  sql_zapis(strsql,1,0);

  sprintf(strsql,"%d",nom);
  admin_alter_r_ins_str_alx("zarnast.alx","Код фонда единого социального взноса для больничных",strsql,win1);

  sprintf(strsql,"insert into Zaresv (kf,datnd,pr,pr1,ktoz,vrem) values (%d,'2011-1-1',33.2,2.,%d,%ld)",nom,iceb_t_getuid(),time(NULL));
  sql_zapis(strsql,1,0);
 }

if(iceb_t_poldan("Код фонда единого социального взноса для договоров",strsql,"zarnast.alx") != 0)
 {
  int nom=0;
  for(nom=1;; nom++)
   {
    sprintf(strsql,"select kod from Zarsoc where kod=%d",nom);
    if(readkey(strsql) == 0)
     break;
   }
  sprintf(strsql,"insert into Zarsoc (kod,naik,shet,proc,kodn,ktoz,vrem) values (%d,'Єдиний соціальний внесок для договорів','',0.,'',%d,%ld)",nom,iceb_t_getuid(),time(NULL));

  sql_zapis(strsql,1,0);

  sprintf(strsql,"%d",nom);
  admin_alter_r_ins_str_alx("zarnast.alx","Код фонда единого социального взноса для договоров",strsql,win1);

  sprintf(strsql,"insert into Zaresv (kf,datnd,pr,pr1,ktoz,vrem) values (%d,'2011-1-1',34.7,2.6,%d,%ld)",nom,iceb_t_getuid(),time(NULL));
  sql_zapis(strsql,1,0);
 }
 
if(iceb_t_poldan("Код удержания единого социального взноса",strsql,"zarnast.alx") != 0)
 {
  int nom=0;
  for(nom=1;; nom++)
   {
    sprintf(strsql,"select kod from Uder where kod=%d",nom);
    if(readkey(strsql) == 0)
     break;
   }
  sprintf(strsql,"insert into Uder (kod,naik,shet,ktoz,vrem,prov,shetb) values (%d,'Єдиний соціальний внесок','',%d,%ld,0,'')",nom,iceb_t_getuid(),time(NULL));
  sql_zapis(strsql,1,0);
  sprintf(strsql,"%d",nom);
  admin_alter_r_ins_str_alx("zarnast.alx","Код удержания единого социального взноса",strsql,win1);

  class iceb_tu_str kod_ob_ud("");
  iceb_t_poldan("Обязательные удержания",&kod_ob_ud,"zarnast.alx");
  kod_ob_ud.z_plus(nom);    

  sprintf(strsql,"update Alx set str='Обязательные удержания|%s\n' where fil='zarnast.alx' and str like 'Обязательные удержания|%%'",kod_ob_ud.ravno());
  sql_zapis(strsql,1,0);
 }

if(iceb_t_poldan("Код удержания единого социального взноса для больничных",strsql,"zarnast.alx") != 0)
 {
  int nom=0;
  for(nom=1;; nom++)
   {
    sprintf(strsql,"select kod from Uder where kod=%d",nom);
    if(readkey(strsql) == 0)
     break;
   }
  sprintf(strsql,"insert into Uder (kod,naik,shet,ktoz,vrem,prov,shetb) values (%d,'Єдиний соціальний внесок для лікарняних','',%d,%ld,0,'')",nom,iceb_t_getuid(),time(NULL));
  sql_zapis(strsql,1,0);
  sprintf(strsql,"%d",nom);
  admin_alter_r_ins_str_alx("zarnast.alx","Код удержания единого социального взноса для больничных",strsql,win1);

  class iceb_tu_str kod_ob_ud("");
  iceb_t_poldan("Обязательные удержания",&kod_ob_ud,"zarnast.alx");
  kod_ob_ud.z_plus(nom);    

  sprintf(strsql,"update Alx set str='Обязательные удержания|%s\n' where fil='zarnast.alx' and str like 'Обязательные удержания|%%'",kod_ob_ud.ravno());
  sql_zapis(strsql,1,0);
 }


if(iceb_t_poldan("Код удержания единого социального взноса для инвалидов",strsql,"zarnast.alx") != 0)
 {
  int nom=0;
  for(nom=1;; nom++)
   {
    sprintf(strsql,"select kod from Uder where kod=%d",nom);
    if(readkey(strsql) == 0)
     break;
   }
  sprintf(strsql,"insert into Uder (kod,naik,shet,ktoz,vrem,prov,shetb) values (%d,'Єдиний соціальний внесок для інвалідів','',%d,%ld,0,'')",nom,iceb_t_getuid(),time(NULL));
  sql_zapis(strsql,1,0);
  sprintf(strsql,"%d",nom);
  admin_alter_r_ins_str_alx("zarnast.alx","Код удержания единого социального взноса для инвалидов",strsql,win1);

  class iceb_tu_str kod_ob_ud("");
  iceb_t_poldan("Обязательные удержания",&kod_ob_ud,"zarnast.alx");
  kod_ob_ud.z_plus(nom);    

  sprintf(strsql,"update Alx set str='Обязательные удержания|%s\n' where fil='zarnast.alx' and str like 'Обязательные удержания|%%'",kod_ob_ud.ravno());
  sql_zapis(strsql,1,0);
 }



if(iceb_t_poldan("Код удержания единого социального взноса для договоров",strsql,"zarnast.alx") != 0)
 {
  int nom=0;
  for(nom=1;; nom++)
   {
    sprintf(strsql,"select kod from Uder where kod=%d",nom);
    if(readkey(strsql) == 0)
     break;
   }
  sprintf(strsql,"insert into Uder (kod,naik,shet,ktoz,vrem,prov,shetb) values (%d,'Єдиний соціальний внесок для договорів','',%d,%ld,0,'')",nom,iceb_t_getuid(),time(NULL));
  sql_zapis(strsql,1,0);
  sprintf(strsql,"%d",nom);
  admin_alter_r_ins_str_alx("zarnast.alx","Код удержания единого социального взноса для договоров",strsql,win1);

  class iceb_tu_str kod_ob_ud("");
  iceb_t_poldan("Обязательные удержания",&kod_ob_ud,"zarnast.alx");
  kod_ob_ud.z_plus(nom);    

  sprintf(strsql,"update Alx set str='Обязательные удержания|%s\n' where fil='zarnast.alx' and str like 'Обязательные удержания|%%'",kod_ob_ud.ravno());
  sql_zapis(strsql,1,0);
 }


admin_alter_r_ins_str_alx("zarnast.alx","Коды начислений не входящие в расчёт удержания единого социального взноса","",win1);

if(iceb_t_poldan("Процент от минимальной зароботной платы",strsql,"zarnast.alx") == 0)
 {
  float proc=atof(strsql);
  sprintf(strsql,"update Alx set str='Процент от минимальной заработной платы|%.2f\n' where fil='zarnast.alx' and str like 'Процент от минимальной зароботной платы|%%'",proc);
  sql_zapis(strsql,1,0);
 }
if(iceb_t_poldan("Процент от минимальной зароботной платы",strsql,"zarppn.alx") == 0)
 {
  float proc=atof(strsql);
  sprintf(strsql,"update Alx set str='Процент от минимальной заработной платы|%.2f\n' where fil='zarppn.alx' and str like 'Процент от минимальной зароботной платы|%%'",proc);
  sql_zapis(strsql,1,0);
 }
 


/****************************************/
/*********** 11.15 04.02.2011 ************/
/****************************************/
wprintw(win1,"Изменям размерность в колонке kss для таблицы Kartb\n");
wrefresh(win1);

sprintf(strsql,"alter table Kartb change kss kss varchar(128) not null default ''");

if(sql_zap(&bd,strsql) != 0)
 {
  wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
  OSTANOVW(win1);
 }

wprintw(win1,"Добавляем колонку art в таблицу Uslugi\n");
wrefresh(win1);
sprintf(strsql,"alter table Uslugi add art varchar(64) NOT NULL default ''");

admin_alter_r_vnk(strsql,win1);

/****************************************/
/*********** 11.17 15.02.2011 ************/
/****************************************/
wprintw(win1,"Добавляем колонку dnz в таблицу Reenn\n");
wrefresh(win1);
sprintf(strsql,"alter table Reenn add dnz int NOT NULL default 0");

admin_alter_r_vnk(strsql,win1);

wprintw(win1,"Создаём таблицу Kontragent2\n");
wrefresh(win1);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kontragent2 (\
nz int unsigned PRIMARY KEY AUTO_INCREMENT NOT NULL ,\
kodkon varchar(32) not null default '',\
datd DATE NOT NULL DEFAULT '0000-00-00',\
nomd VARCHAR (64) NOT NULL DEFAULT '',\
vidd VARCHAR (255) NOT NULL DEFAULT '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
index(kodkon)) ENGINE = MYISAM");
/*
0 nz      уникальный номер записи
1 kodkon  код контрагента
2 datd    дата договора
3 nomd    номер договора
4 vidd    вид договора
5 ktoz    Кто записал
6 vrem    Время записи
*/


admin_alter_r_crtb(strsql,win1);


/****************************************/
/*********** 11.19 11.03.2011 ************/
/****************************************/

admin_alter_r_ins_str_alx("zarnast.alx","Автоматическое выполнение проводок с использованием таблицы","Включено",win1);

/****************************************/
/*********** 11.20 16.03.2011 ************/
/****************************************/

admin_alter_r_ins_str_alx("zarnast.alx","Код подоходного налога с больничного","",win1);

wprintw(win1,"%s nnni %s Reenn\n",gettext("Добавляем колонку"),gettext("в таблицу"));
wrefresh(win1);
sprintf(strsql,"alter table Reenn add nnni int NOT NULL default 0");

if(admin_alter_r_vnk(strsql,win1) == 0)
 admin_alter_r_nnni();

wprintw(win1,"Создаём таблицу Uosls\n");
wrefresh(win1);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uosls (\
inn INT NOT NULL DEFAULT 0,\
data DATE NOT NULL DEFAULT '0000-00-00',\
lsnu DOUBLE (16,2) NOT NULL DEFAULT 0.,\
lsbu DOUBLE (16,2) NOT NULL DEFAULT 0.,\
kom VARCHAR(255) NOT NULL DEFAULT '',\
ktoz smallint unsigned not null default 0,\
vrem int unsigned not null default 0,\
index(inn,data)) ENGINE = MYISAM");
/*
0 inn    инвентарный номер
1 data  дата 
2 lsnu  ликвидационная стоимость для налогового учёта
3 lsbu  ликвидационная стоимость для бухгалетского учёта
4 kom   коментарий
5 ktoz  Кто записал
6 vrem  Время записи
*/

admin_alter_r_crtb(strsql,win1);


/****************************************/
/*********** 11.24 14.04.2011 ************/
/****************************************/

admin_alter_r_ins_str_alx("zarnast.alx","Отключение/включение применения социальной льготы при расчёте подоходного","Выключено",win1);

/****************************************/
/*********** 11.33 19.07.2011 ************/
/****************************************/

wprintw(win1,"%s popkf %s Uosinp\n",gettext("Увеличиваем размерность колонки"),gettext("в таблице"));
wrefresh(win1);

sprintf(strsql,"alter table Uosinp change popkof popkof double(12,6) not null default 0.");

if(sql_zap(&bd,strsql) != 0)
 {
  wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
  OSTANOVW(win1);
 }

wprintw(win1,"%s popkfby %s Uosinp\n",gettext("Увеличиваем размерность колонки"),gettext("в таблице"));
wrefresh(win1);

sprintf(strsql,"alter table Uosinp change popkofby popkofby double(12,6) not null default 0.");

if(sql_zap(&bd,strsql) != 0)
 {
  wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
  OSTANOVW(win1);
 }

/****************************************/
/*********** 11.34 10.08.2011 ************/
/****************************************/

wprintw(win1,"Вставляем колонку vd в таблицу Uosdok\n");
wrefresh(win1);

sprintf(strsql,"alter table Uosdok add vd varchar(255) not null default ''");

admin_alter_r_vnk(strsql,win1);


/****************************************/
/*********** 12.0 32.08.2011 ************/
/****************************************/

admin_alter_r_ins_str_alx("matnast.alx","Автоматическое выполнение проводок по списку сделать основным","Выключено",win1);

/****************************************/
/*********** 12.3 13.09.2011 ************/
/****************************************/

wprintw(win1,"%s\n",gettext("Увеличиваем размерность полного наименования"));
wrefresh(win1);

sprintf(strsql,"alter table Kontragent change pnaim pnaim varchar(255) not null default ''");

if(sql_zap(&bd,strsql) != 0)
 {
  wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
  OSTANOVW(win1);
 }

sprintf(strsql,"alter table Reenn change nkontr nkontr varchar(255) not null default ''");

if(sql_zap(&bd,strsql) != 0)
 {
  wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
  OSTANOVW(win1);
 }

sprintf(strsql,"alter table Reenn1 change nkontr nkontr varchar(255) not null default ''");

if(sql_zap(&bd,strsql) != 0)
 {
  wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
  OSTANOVW(win1);
 }

admin_alter_r_ins_str_alx("zarpensm.alx","Коды табеля больничных","",win1);

/****************************************/
/*********** 12.4 14.09.2011 ************/
/****************************************/
admin_alter_r_ins_str_alx("zarpensm.alx","Коды табеля без сохранения зарплаты","",win1);

sprintf(strsql,"delete from Alx where fil='matnast.alx' and str like 'Код операции расчёта средневзвешенной цены|%%'");
sql_zapis(strsql,1,0);


/****************************************/
/*********** 12.5 06.10.2011 ************/
/****************************************/
wprintw(win1,"%s rsnds %s Kontragent\n",gettext("Удаляем колонку"),gettext("из таблицы"));
wrefresh(win1);

sprintf(strsql,"alter table Kontragent drop rsnds");

if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) != ER_CANT_DROP_FIELD_OR_KEY)
    wprintw(win1,"%s ! %d\n%s\n",gettext("Ошибка"),sql_nerror(&bd),sql_error(&bd));
  else
    wprintw(win1,"%s\n",gettext("Колонка уже удалена"));

 }
else
 {
  wprintw(win1,"%s gk %s Kontragent\n",gettext("Вставляем колонку"),gettext("в таблицу"));
  wrefresh(win1);

  sprintf(strsql,"alter table Kontragent add gk varchar(255) not null default ''");

  admin_alter_r_vnk(strsql,win1);

 }

wprintw(win1,"%s np %s Sklad\n",gettext("Вставляем колонку"),gettext("в таблицу"));
wrefresh(win1);

sprintf(strsql,"alter table Sklad add np varchar(255) not null default ''");
admin_alter_r_vnk(strsql,win1);

wprintw(win1,"%s np %s Uplpod\n",gettext("Вставляем колонку"),gettext("в таблицу"));
wrefresh(win1);

sprintf(strsql,"alter table Uslpodr add np varchar(255) not null default ''");

admin_alter_r_vnk(strsql,win1);

wprintw(win1,"%s fo %s Uplpod\n",gettext("Вставляем колонку"),gettext("в таблицу"));
wrefresh(win1);
sprintf(strsql,"alter table Uslpodr add fo varchar(128) not null default ''");

admin_alter_r_vnk(strsql,win1);


wprintw(win1,"%s dol %s Uplpod\n",gettext("Вставляем колонку"),gettext("в таблицу"));
wrefresh(win1);
sprintf(strsql,"alter table Uslpodr add dol varchar(255) not null default ''");

admin_alter_r_vnk(strsql,win1);

/****************************************/
/*********** 12.7 13.10.2011 ************/
/****************************************/
wprintw(win1,"%s %s\n",gettext("Создание таблицы справочника количества рабочих дней и часов"),"Zarskrdh");
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Zarskrdh \
(data DATE NOT NULL DEFAULT '0000-00-00' primary key,\
krd FLOAT NOT NULL DEFAULT 0.,\
krh FLOAT NOT NULL DEFAULT 0.,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0) ENGINE = MYISAM");
/*
0 data   месяц и god день всегда равен 1
1 krd    количество рабочих дней
2 krh    количество рабочих часов
2 ktoz   Кто записал
3 vrem   Время записи
*/

if(admin_alter_r_crtb(strsql,win1) == 0)
  admin_alter_r_krdh(put_na_alx,win1); /*загрузка в таблицу*/
wrefresh(win1);

wprintw(win1,"%s %s\n",gettext("Создание таблицы справочника выходных дней"),"Zarsvd");
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Zarsvd \
(data DATE NOT NULL DEFAULT '0000-00-00' primary key,\
kom VARCHAR(255) NOT NULL DEFAULT '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0) ENGINE = MYISAM");
/*
0 data   дата выходного дня
1 kom    коментарий
2 ktoz   Кто записал
3 vrem   Время записи
*/

if(admin_alter_r_crtb(strsql,win1) == 0)
  admin_alter_r_svd(put_na_alx,win1); /*загрузка в таблицу*/

wprintw(win1,"%s %s\n",gettext("Создание таблицы справочника минимальной зарплаты и прожиточного минимума"),"Zarmzpm");
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Zarmzpm \
(data DATE NOT NULL DEFAULT '0000-00-00' primary key,\
mz FLOAT NOT NULL DEFAULT 0.,\
pm FLOAT NOT NULL DEFAULT 0.,\
ms FLOAT NOT NULL DEFAULT 0.,\
ktoz SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");
/*
0 data   дата
1 mz     минимальная зарплата
2 pm     прожиточный минимум
3 ms     максимальная сумма с которой берутся соц-отчисления
4 ktoz   Кто записал
5 vrem   Время записи
*/

if(admin_alter_r_crtb(strsql,win1) == 0)
  admin_alter_r_mzpm(put_na_alx,win1);

/****************************************/
/*********** 12.8 21.10.2011 ************/
/****************************************/
wprintw(win1,"%s Zvan\n",gettext("Вставляем колонку sdzz в таблицу"));
wrefresh(win1);
sprintf(strsql,"alter table Zvan add sdzz double(12,2) not null default 0.");

if(admin_alter_r_vnk(strsql,win1) == 0)
 admin_alter_r_sdzz(put_na_alx,win1);

/****************************************/
/*********** 12.9 02.11.2011 ************/
/****************************************/
wprintw(win1,"%s Zarrud\n",gettext("Создание таблицы регулярных удержаний с работника"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Zarrud \
(tn INT NOT NULL DEFAULT 0 primary key,\
ku int  NOT NULL DEFAULT 0,\
sp DOUBLE(10,2) NOT NULL DEFAULT 0.,\
mu TINYINT NOT NULL DEFAULT 0,\
dd DATE NOT NULL DEFAULT '0000-00-00',\
kom VARCHAR(255) NOT NULL DEFAULT '',\
ktoz SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");
/*
0 tn  табельный номер
1 ku  код удержания
2 sp  сумма/процент
3 mu  метка 0-сумма 1-процент
4 sd  дата действия
5 kom коментарий
6 ktoz   Кто записал
7 vrem   Время записи
*/

if(admin_alter_r_crtb(strsql,win1) == 0)
  admin_alter_r_zrud(put_na_alx,win1);

/****************************************/
/*********** 12.10 09.11.2011 ************/
/****************************************/
wprintw(win1,"%s Zarlgot\n",gettext("Создание таблицы льгот"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Zarlgot \
(tn INT NOT NULL DEFAULT 0,\
dndl DATE NOT NULL DEFAULT '0000-00-00',\
pl FLOAT(7,2) NOT NULL DEFAULT 0.,\
koll INT NOT NULL DEFAULT 0,\
kodl CHAR(16) NOT NULL DEFAULT '',\
kom VARCHAR(255) NOT NULL DEFAULT '',\
ktoz SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0,\
unique(tn,dndl)) ENGINE = MYISAM");
/*
0 tn   табельный номер
1 dndl Дата начала действия льготы
2 pl   процент льготы
3 koll количество льгот
4 kodl код льготы
5 kom  коментарий
6 ktoz Кто записал
7 vrem Время записи
*/

if(admin_alter_r_crtb(strsql,win1) == 0)
  admin_alter_r_zsl(put_na_alx,win1);


wprintw(win1,"%s Zargr\n",gettext("Создание таблицы групп работников"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Zargr \
(kg INT NOT NULL DEFAULT 0,\
tn INT NOT NULL DEFAULT 0,\
dnd DATE NOT NULL DEFAULT '0000-00-00',\
dkd DATE NOT NULL DEFAULT '0000-00-00',\
kom VARCHAR(255) NOT NULL DEFAULT '',\
ktoz SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0,\
unique(kg,tn,dnd)) ENGINE = MYISAM");
/*
0 kg   код группы работкика 1-пенсионер 2-инвалид 3-работающий по договору подряда
1 tn   табельный номер
2 dnd  Дата начала действия
3 dkd  дата конца действия
4 kom  коментарий
5 ktoz Кто записал
6 vrem Время записи
*/

if(admin_alter_r_crtb(strsql,win1) == 0)
 {
  admin_alter_r_zargr(put_na_alx,win1);

  sprintf(strsql,"delete from Alx where fil='zardod9.alx'");
  sql_zapis(strsql,1,0);

  sprintf(strsql,"delete from Alx where fil='zardod13.alx'");
  sql_zapis(strsql,1,0);

  sprintf(strsql,"%s/zardod13.alx",put_na_alx);
  unlink(strsql);
  sprintf(strsql,"%s/zardod9.alx",put_na_alx);
  unlink(strsql);
 }
 
/****************************************/
/*********** 12.11 26.11.2011 ************/
/****************************************/
wprintw(win1,"%s Zarpr\n",gettext("Создание таблицы протоколов хода расчёта"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Zarpr \
(tn INT NOT NULL DEFAULT 0,\
tz TINYINT NOT NULL DEFAULT 0,\
dr DATE NOT NULL DEFAULT '0000-00-00',\
nz INT NOT NULL DEFAULT 0,\
sz TEXT NOT NULL DEFAULT '',\
unique(tn,tz,dr,nz)) ENGINE = MYISAM");
/*
0 tn   табельный номер
1 tz  1-протокол расчёта начислений 2-удержаний 3-начислений на фонд зарплаты
2 dr  Дата расчёта год и месяц день всегда 1
3 nz   номер записи
4 sd  содержимое записи
*/

admin_alter_r_crtb(strsql,win1);

 /****************************************/
/*********** 12.13 14.12.2011 ************/
/****************************************/
wprintw(win1,"%s ku %s Material\n",gettext("Вставляем колонку"),gettext("в таблицу"));
wrefresh(win1);
sprintf(strsql,"alter table Material add ku char(32) not null default ''");

admin_alter_r_vnk(strsql,win1);




/****************************************/
/*********** 12.14 20.12.2011 ************/
/****************************************/


if(iceb_t_poldan("Код фонда единого социального взноса с больничных для инвалидов",strsql,"zarnast.alx") != 0)
 {
  int nom=0;
  for(nom=1;; nom++)
   {
    sprintf(strsql,"select kod from Zarsoc where kod=%d",nom);
    if(readkey(strsql) == 0)
     break;
   }
  sprintf(strsql,"insert into Zarsoc (kod,naik,shet,proc,kodn,ktoz,vrem) values (%d,'ЄСВ з лікарняних для інвалідів','',0.,'',%d,%ld)",nom,iceb_t_getuid(),time(NULL));

  sql_zapis(strsql,1,0);

  sprintf(strsql,"%d",nom);
  admin_alter_r_ins_str_alx("zarnast.alx","Код фонда единого социального взноса с больничных для инвалидов",strsql,win1);

  sprintf(strsql,"insert into Zaresv (kf,datnd,pr,pr1,ktoz,vrem) values (%d,'2011-12-1',8.41,2.0,%d,%ld)",nom,iceb_t_getuid(),time(NULL));
  sql_zapis(strsql,1,0);
 }
 
if(iceb_t_poldan("Код удержания единого социального взноса с больничных для инвалидов",strsql,"zarnast.alx") != 0)
 {
  int nom=0;
  for(nom=1;; nom++)
   {
    sprintf(strsql,"select kod from Uder where kod=%d",nom);
    if(readkey(strsql) == 0)
     break;
   }
  sprintf(strsql,"insert into Uder (kod,naik,shet,ktoz,vrem,prov,shetb) values (%d,'ЄСВ з лікарняних для інвалідів','',%d,%ld,0,'')",nom,iceb_t_getuid(),time(NULL));
  sql_zapis(strsql,1,0);
  sprintf(strsql,"%d",nom);
  admin_alter_r_ins_str_alx("zarnast.alx","Код удержания единого социального взноса с больничных для инвалидов",strsql,win1);

  class iceb_tu_str kod_ob_ud("");
  iceb_t_poldan("Обязательные удержания",&kod_ob_ud,"zarnast.alx");
  kod_ob_ud.z_plus(nom);    

  sprintf(strsql,"update Alx set str='Обязательные удержания|%s\n' where fil='zarnast.alx' and str like 'Обязательные удержания|%%'",kod_ob_ud.ravno());
  sql_zapis(strsql,1,0);
  
 }

/****************************************/
/*********** 12.16 28.12.2011 ************/
/****************************************/


wprintw(win1,"%s kk %s Rashod\n",gettext("Вставляем колонку"),gettext("в таблицу"));
wrefresh(win1);
sprintf(strsql,"alter table Rashod add kk char(16) not null default '00'");

if(admin_alter_r_vnk(strsql,win1) == 0)
 {
  admin_alter_r_murashod();


  admin_alter_r_ins_str_alx("nastsys.alx","Количество строк на стандартном листе для системы с графическим интерфейсом","64",win1);
  admin_alter_r_ins_str_alx("nastsys.alx","Количество строк на стандартном листе в ориентации ландшафт для системы с графическим интерфейсом","46",win1);

 }

wprintw(win1,"%s kk %s Usloper2\n",gettext("Вставляем колонку"),gettext("в таблицу"));
wrefresh(win1);
sprintf(strsql,"alter table Usloper2 add kk char(16) not null default '00'");

if(admin_alter_r_vnk(strsql,win1) == 0)
 admin_alter_r_uslrashod();


wprintw(win1,"%s kk %s Uosras\n",gettext("Вставляем колонку"),gettext("в таблицу"));
wrefresh(win1);
sprintf(strsql,"alter table Uosras add kk char(16) not null default '00'");

admin_alter_r_vnk(strsql,win1); /*В учёте услуг небыло настройки на коды операций*/

/****************************************/
/*********** 12.18 05.01.2012 ************/
/****************************************/

wprintw(win1,"%s md %s Uosin1\n",gettext("Вставляем колонку"),gettext("в таблицу"));
wrefresh(win1);
sprintf(strsql,"alter table Uosin1 add md tinyint not null default 0");

if(admin_alter_r_vnk(strsql,win1) == 0)
 {
  sprintf(strsql,"alter table Uosin1 change zapis zapis TEXT not null default ''");
  sql_zapis(strsql,1,0);

  sprintf(strsql,"alter table Uosin1 drop index innom");
  sql_zapis(strsql,1,0);

  sprintf(strsql,"alter table Uosin1 add unique (innom,md,nomz)");
  sql_zapis(strsql,1,0);
  
  sprintf(strsql,"select * from Uosin1");
  
  class iceb_tu_str zapis("");  
  if(cur.make_cursor(&bd,strsql) < 0)
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  else
   while(cur.read_cursor(&row) != 0)
    {
     zapis.new_plus(row[2]);
     zapis.plus("\n");
     sprintf(strsql,"update Uosin1 set zapis='%s' where innom=%s and md=%s and nomz=%s",zapis.ravno_filtr(),row[0],row[3],row[1]);
     sql_zapis(strsql,1,0);
    }  

  sprintf(strsql,"%s/uoskar.alx",put_na_alx);
  unlink(strsql);

  sql_zapis("delete from Alx where fil='uoskar.alx'",1,0);

  sql_zapis("delete from Alx where fil='nn4_hap.alx'",1,0); /*обновляем внесены исправления*/
  sql_zapis("delete from Alx where fil='zarsvfz_end.alx'",1,0); /*обновляем внесены исправления*/

  admin_alter_r_ins_str_alx("matnast.alx","Дата отгрузки в налоговой накладной без разделительных точек","Выключено",win1);

  /****************************************вложили сюда чтобы отработало только один раз*/
  /*********** 12.19 13.01.2012 ************/
  /****************************************/
  int razmer_otstupa=0;
  if(iceb_t_poldan("Отступ от левого края",&razmer_otstupa,"matnast.alx") == 0)
   {
    wprintw(win1,"razmer_otstupa=%d\n",razmer_otstupa);
    if(razmer_otstupa > 5)
     {
      sprintf(strsql,"update Alx set str='Отступ от левого края|5\n' where fil='matnast.alx' and str like 'Отступ от левого края|%%'");
      sql_zapis(strsql,1,0);
     }
   } 

  if(iceb_t_poldan("Отступ налоговой накладной",&razmer_otstupa,"matnast.alx") == 0)
   {
    wprintw(win1,"razmer_otstupa=%d\n",razmer_otstupa);
    if(razmer_otstupa > 5)
     {
      sprintf(strsql,"update Alx set str='Отступ налоговой накладной|5\n' where fil='matnast.alx' and str like 'Отступ налоговой накладной|%%'");
      sql_zapis(strsql,1,0);
     }
   } 

  if(iceb_t_poldan("Отступ от левого края",&razmer_otstupa,"uslnast.alx") == 0)
   {
    wprintw(win1,"razmer_otstupa=%d\n",razmer_otstupa);
    if(razmer_otstupa > 2)
     {
      sprintf(strsql,"update Alx set str='Отступ от левого края|2\n' where fil='uslnast.alx' and str like 'Отступ от левого края|%%'");
      sql_zapis(strsql,1,0);
     }
   } 

  if(iceb_t_poldan("Отступ от левого края",&razmer_otstupa,"nastdok.alx") == 0)
   {
    wprintw(win1,"razmer_otstupa=%d\n",razmer_otstupa);
    if(razmer_otstupa > 4)
     {
      sprintf(strsql,"update Alx set str='Отступ от левого края|4\n' where fil='nastdok.alx' and str like 'Отступ от левого края|%%'");
      sql_zapis(strsql,1,0);
     }
   } 

  admin_alter_r_ins_str_alx("nastsys.alx","Количество строк на А4 с уменьшенным межстрочным расстоянием для системы с графическим интерфейсом","76",win1);
  admin_alter_r_ins_str_alx("nastsys.alx","Количество строк на А4 с уменьшенным межстрочным расстоянием в ориентации ландшафт для системы с графическим интерфейсом","56",win1);

 }

/****************************************/
/*********** 12.22 20.02.2012 ************/
/****************************************/

wprintw(win1,"%s en %s Kontragent\n",gettext("Вставляем колонку"),gettext("в таблицу"));
wrefresh(win1);
sprintf(strsql,"alter table Kontragent add en tinyint not null default 0");

if(admin_alter_r_vnk(strsql,win1) == 0)
 {

  wprintw(win1,"%s nais %s Plansh\n",gettext("Увеличиваем длинну колонки"),gettext("в таблице"));
  wrefresh(win1);

  sprintf(strsql,"alter table Plansh change nais nais varchar(255) not null default ''");

  if(sql_zap(&bd,strsql) != 0)
   {
    wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
    OSTANOVW(win1);
   }

  wprintw(win1,"%s dokum %s Kasord\n",gettext("Увеличиваем длинну колонки"),gettext("в таблице"));
  wrefresh(win1);

  sprintf(strsql,"alter table Kasord change dokum dokum varchar(255) not null default ''");

  if(sql_zap(&bd,strsql) != 0)
   {
    wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
    OSTANOVW(win1);
   }

  sql_zapis("delete from Alx where fil='nn4_start.alx'",1,0); /*обновляем внесены исправления*/

  /*ставим метку единого налога всем частным предпринимателям*/
  sprintf(strsql,"update Kontragent set en=1 where regnom <> ''");
  sql_zapis(strsql,1,0);

  /****************************************/
  /*********** 12.22 20.02.2012 ************/
  /****************************************/

  wprintw(win1,"%s naik %s Sklad\n",gettext("Увеличиваем длинну колонки"),gettext("в таблице"));
  wrefresh(win1);

  sprintf(strsql,"alter table Sklad change naik naik varchar(124) not null default ''");
  if(sql_zap(&bd,strsql) != 0)
   {
    wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
    OSTANOVW(win1);
   }

  wprintw(win1,"%s fmol %s Sklad\n",gettext("Увеличиваем длинну колонки"),gettext("в таблице"));
  wrefresh(win1);

  sprintf(strsql,"alter table Sklad change fmol fmol varchar(255) not null default ''");
  if(sql_zap(&bd,strsql) != 0)
   {
    wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
    OSTANOVW(win1);
   }


  wprintw(win1,"%s dolg %s Sklad\n",gettext("Увеличиваем длинну колонки"),gettext("в таблице"));
  wrefresh(win1);

  sprintf(strsql,"alter table Sklad change dolg dolg varchar(124) not null default ''");
  if(sql_zap(&bd,strsql) != 0)
   {
    wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
    OSTANOVW(win1);
   }
 }



/****************************************/
/*********** 12.23 06.05.2012 ************/
/****************************************/

wprintw(win1,"%s Uplnst\n",gettext("Создание таблицы норм списания топлива"));

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uplnst (\
kod   char(16) not null default '' primary key,\
naik  char(124) not null default '',\
ktoz  smallint unsigned not null default 0,\
vrem  int unsigned not null default 0) ENGINE = MYISAM");

/*
0  kod   код нормы списания топлива
1  naik  наименование нормы списания топлива
2  ktoz  кто записал
3  vrem  время записи
*/

if(admin_alter_r_crtb(strsql,win1) == 0)
 {
  admin_alter_r_vnst0(win1);

  wprintw(win1,"%s Uplavt1\n",gettext("Создание таблицы норм списания топлива к автомобилю"));

  memset(strsql,'\0',sizeof(strsql));
  strcpy(strsql,"CREATE TABLE Uplavt1 (\
  kod   smallint not null default 0,\
  knst  char(16) not null default '',\
  nst   float(8,3) not null default 0.,\
  ei    varchar(16) not null default '',\
  ktoz  smallint unsigned not null default 0,\
  vrem  int unsigned not null default 0,\
  unique(kod,knst)) ENGINE = MYISAM");

  /*
  0  kod   код автомобиля
  1  knst  код нормы списания топлива
  2  nst   норма списания топлива
  3  ei    единица изменения
  3  ktoz  кто записал
  4  vrem  время записи
  */
  if(admin_alter_r_crtb(strsql,win1) == 0)
    admin_alter_r_vnst(win1);


  wprintw(win1,"%s Upldok3\n",gettext("Создание таблицы списания топлива к путевому листу"));

  memset(strsql,'\0',sizeof(strsql));
  strcpy(strsql,"CREATE TABLE Upldok3 (\
  datd  DATE not null default '0000-00-00',\
  kp smallint not null default 0,\
  nomd varchar(16) not null default '',\
  knst varchar(16) not null default '',\
  kl   double(10,3) not null default 0.,\
  nst  float(8,3) not null default 0.,\
  zn   float(8,3) not null default 0.,\
  zf   float(8,3) not null default 0.,\
  ei    varchar(16) not null default '',\
  kom varchar(255) not null default '',\
  ktoz  smallint unsigned not null default 0,\
  vrem  int unsigned not null default 0,\
  unique(datd,kp,nomd,knst)) ENGINE = MYISAM");

  /*
  0  datd  дата документа
  1  kp    код подразделения (если равно 0 - акт списания)
  2  nomd  номер документа
  3  knst  код нормы списания топлива
  4  kl    количество
  5  nst   норма списания топлива
  6  zn    затраты по норме
  7  zf    затраты по факту
  8  ei    единица измерения
  9 kom  коментарий
  10  ktoz  кто записал
  11 vrem  время записи
  */
  if(admin_alter_r_crtb(strsql,win1) == 0)
    admin_alter_r_vnstpl(win1);


  wprintw(win1,"%s Upldok2a\n",gettext("Создание таблицы списания топлива по счетам"));

  memset(strsql,'\0',sizeof(strsql));
  strcpy(strsql,"CREATE TABLE Upldok2a (\
  datd  DATE not null default '0000-00-00',\
  kp smallint not null default 0,\
  nomd varchar(32) not null default '',\
  kt varchar(32) not null default '',\
  shet varchar(10) not null default '',\
  zt float(8,3) not null default 0,\
  zpn float(8,3) not null default 0.,\
  koment varchar(255) not null default '',\
  ktoz  smallint unsigned not null default 0,\
  vrem  int unsigned not null default 0,\
  unique(datd,kp,nomd,kt,shet)) ENGINE = MYISAM");

  /*
  0 datd  дата документа
  1 kp    код подразделения
  2 nomd  номер документа
  3 kt    код топлива
  4 shet  счёт списания
  5 zt    затраты топлива
  6 zpn  затраты по норме
  7 koment коментарий
  8 ktoz  кто записал
  9 vrem  время записи
  */

  if(admin_alter_r_crtb(strsql,win1) == 0)
   admin_alter_r_upldok2a(win1);

  sprintf(strsql,"delete from Alx where fil='uplnast.alx' and str like 'Летняя норма расхода топлива|%%'");
  sql_zapis(strsql,1,0);

  wprintw(win1,"%s Uplost\n",gettext("Удаляем таблицу"));
  wrefresh(win1);

  sprintf(strsql,"drop table Uplost");
  if(sql_zap(&bd,strsql) != 0)
   {
    if(sql_nerror(&bd) != ER_BAD_TABLE_ERROR)
      wprintw(win1,"%s ! %d\n%s\n",gettext("Ошибка"),sql_nerror(&bd),sql_error(&bd));
    else
      wprintw(win1,"%s\n",gettext("Таблица не обнаружена"));

   }

  wprintw(win1,"%s Upldok2\n",gettext("Удаляем таблицу"));
  wrefresh(win1);

  sprintf(strsql,"drop table Upldok2");
  if(sql_zap(&bd,strsql) != 0)
   {
    if(sql_nerror(&bd) != ER_BAD_TABLE_ERROR)
      wprintw(win1,"%s ! %d\n%s\n",gettext("Ошибка"),sql_nerror(&bd),sql_error(&bd));
    else
      wprintw(win1,"%s\n",gettext("Таблица не обнаружена"));

   }

  wprintw(win1,"%s Uplgost\n",gettext("Удаляем таблицу"));
  wrefresh(win1);

  sprintf(strsql,"drop table Uplgost");
  if(sql_zap(&bd,strsql) != 0)
   {
    if(sql_nerror(&bd) != ER_BAD_TABLE_ERROR)
      wprintw(win1,"%s ! %d\n%s\n",gettext("Ошибка"),sql_nerror(&bd),sql_error(&bd));
    else
      wprintw(win1,"%s\n",gettext("Таблица не обнаружена"));

   }


  wprintw(win1,"%s Uplavt\n",gettext("Удаляем колонки из таблицы"));
  wrefresh(win1);

  sprintf(strsql,"alter table Uplavt drop nrmh,drop nrtk,drop znrzg,drop lnrzg,drop lnr,drop znr");

  if(sql_zap(&bd,strsql) != 0)
   {
    if(sql_nerror(&bd) != ER_CANT_DROP_FIELD_OR_KEY)
      wprintw(win1,"%s ! %d\n%s\n",gettext("Ошибка"),sql_nerror(&bd),sql_error(&bd));
    else
      wprintw(win1,"%s\n",gettext("Колонки уже удалены"));

   }

  wprintw(win1,"%s Upldok\n",gettext("Удаляем колонки из таблицы"));
  wrefresh(win1);

  sprintf(strsql,"alter table Upldok drop tk,drop ztvrf,drop ztvrn,drop vrr,drop ztvsf,drop ztvsn,drop vesg,drop nstzg,drop probzg,drop ztfazg,drop ztpnzg,drop nst,drop ztpn,drop ztfa,drop prob");

  if(sql_zap(&bd,strsql) != 0)
   {
    if(sql_nerror(&bd) != ER_CANT_DROP_FIELD_OR_KEY)
      wprintw(win1,"%s ! %d\n%s\n",gettext("Ошибка"),sql_nerror(&bd),sql_error(&bd));
    else
      wprintw(win1,"%s\n",gettext("Колонки уже удалены"));

   }


 }

/****************************************/
/*********** 12.27 31.05.2012 ************/
/****************************************/

wprintw(win1,"%s k00 %s Dokummat\n",gettext("Вставляем колонку"),gettext("в таблицу"));
wrefresh(win1);
sprintf(strsql,"alter table Dokummat add k00 char(16) not null default '00'");

if(admin_alter_r_vnk(strsql,win1) == 0)
 {
 
  wprintw(win1,"%s k00 %s Usldokum\n",gettext("Вставляем колонку"),gettext("в таблицу"));
  wrefresh(win1);
  sprintf(strsql,"alter table Usldokum add k00 char(16) not null default '00'");

  admin_alter_r_vnk(strsql,win1);

 }

/****************************************/
/*********** 12.31 27.07.2012 ************/
/****************************************/
admin_alter_r_ins_str_alx("matnast.alx","Системная подпись в налоговой накладной","Включено",win1);

/****************************************/
/*********** 12.32 13.08.2012 ************/
/****************************************/
admin_alter_r_ins_str_alx("rnn_nast.alx","Экспортировать для регистрации в налоговой свою налоговую накладную","Выключено",win1);

jump13082012:;

/****************************************/
/*********** 13.1 05.09.2012 ************/
/****************************************/

wprintw(win1,"%s \"Код операции автоматической дооценки\" %s matnast.alx\n",gettext("Удаляем настройку"),gettext("в файле"));
wrefresh(win1);

sprintf(strsql,"delete from Alx where fil='matnast.alx' and str like 'Код операции автоматической дооценки|%%'");
sql_zapis(strsql,1,0);

jump05092012:;

/****************************************/
/*********** 13.7 06.11.2012 ************/
/****************************************/
wprintw(win1,"%s\n",gettext("Добавляем код оператора для действий выполненных программой"));
wrefresh(win1);

sprintf(strsql,"insert into icebuser (login,fio,un,kom,logz,vrem) values('%s','%s',%d,'%s','%s',%ld)",
ICEB_LOGIN_OP_PROG,
gettext("Программа"),
ICEB_KOD_OP_PROG,
gettext("Для действий выполненных без участия оператора"),
iceb_tu_getlogin(),time(NULL));

sql_zapis(strsql,1,1);


jump06112012:;

/****************************************/
/*********** 13.7 28.11.2012 ************/
/****************************************/
wprintw(win1,"%s \"Копирование на ДОС дискету\" %s nastsys.alx\n",gettext("Удаляем настройку"),gettext("в файле"));
wrefresh(win1);

sprintf(strsql,"delete from Alx where fil='nastsys.alx' and str like 'Копирование на ДОС дискету|%%'");
sql_zapis(strsql,1,0);


wprintw(win1,"%s \"Устройство ДОС для копирования\" %s nastsys.alx\n",gettext("Удаляем настройку"),gettext("в файле"));
wrefresh(win1);

sprintf(strsql,"delete from Alx where fil='nastsys.alx' and str like 'Устройство ДОС для копирования|%%'");
sql_zapis(strsql,1,0);



jump28112012:;

/****************************************/
/*********** 13.8 30.11.2012 ************/
/****************************************/
wprintw(win1,"%s pp03072009.alx\n",gettext("Удаляем файл"));
wrefresh(win1);

sprintf(strsql,"delete from Alx where fil='pp03072009.alx'");
sql_zapis(strsql,1,0);

wprintw(win1,"%s pt03072009.alx\n",gettext("Удаляем файл"));
wrefresh(win1);

sprintf(strsql,"delete from Alx where fil='pt03072009.alx'");
sql_zapis(strsql,1,0);

jump30112012:;

/****************************************/
/*********** 13.9 05.12.2012 ************/
/****************************************/
wprintw(win1,"%s zarspv2.alx\n",gettext("Удаляем файл"));
wrefresh(win1);

sprintf(strsql,"delete from Alx where fil='zarspv2.alx'");
sql_zapis(strsql,1,0);

wprintw(win1,"%s zarspv1.alx\n",gettext("Удаляем файл"));
wrefresh(win1);

sprintf(strsql,"delete from Alx where fil='zarspv1.alx'");
sql_zapis(strsql,1,0);

jump05122012:;

/****************************************/
/*********** 13.9 01.01.2013 ************/
/****************************************/
wprintw(win1,"%s nn4_start.alx\n",gettext("Удаляем файл"));
wrefresh(win1);

sprintf(strsql,"delete from Alx where fil='nn4_start.alx'");
sql_zapis(strsql,1,0);

wprintw(win1,"%s nn4_end.alx\n",gettext("Удаляем файл"));
wrefresh(win1);

sprintf(strsql,"delete from Alx where fil='nn4_end.alx'");
sql_zapis(strsql,1,0);

jump02012013:;

/****************************************/
/*********** 13.11 11.01.2013 ************/
/****************************************/
admin_alter_r_ins_str_alx("zarnast.alx","Табельный номер выплачивающего зарплату","",win1);
admin_alter_r_ins_str_alx("zarnast.alx","Табельный номер проверяющего ведомости на зарплату","",win1);

jump11012013:;

/****************************************/
/*********** 13.12 11.02.2013 ************/
/****************************************/
admin_alter_r_ins_str_alx("zarpensm.alx","Коды табеля прeбывания в трудовых отношениях","",win1);

jump11022013:;

/****************************************/
/*********** 13.13 06.03.2013 ************/
/****************************************/
/*загрузка новых настроечных файлов*/

jump06032013:;

/****************************************/
/*********** 13.13 05.04.2013 ************/
/****************************************/
wprintw(win1,"Увеличиваем длинну колонки vd в таблице Reenn\n");
wrefresh(win1);

sprintf(strsql,"alter table Reenn change vd vd varchar(32) not null default ''");

if(sql_zap(&bd,strsql) != 0)
 {
  wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
  OSTANOVW(win1);
 }

wprintw(win1,"Увеличиваем длинну колонки vd в таблице Reenn1\n");
wrefresh(win1);

sprintf(strsql,"alter table Reenn1 change vd vd varchar(32) not null default ''");

if(sql_zap(&bd,strsql) != 0)
 {
  wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
  OSTANOVW(win1);
 }

jump05042013:;

/****************************************/
/*********** 13.16 21.06.2013 ************/
/****************************************/

wprintw(win1,"Переименовываем название колонки dogov на na и увеличиваем длинну в таблице Kontragent\n");
wrefresh(win1);

sprintf(strsql,"alter table Kontragent change dogov na varchar(255) not null default ''");

if(sql_zap(&bd,strsql) != 0)
 {
  wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
  OSTANOVW(win1);
 }
else
 {
  sprintf(strsql,"update Kontragent set na=''");
  sql_zapis(strsql,1,0);
 }

jump21062013:;

/****************************************/
/*********** 14.3 03.09.2013 ************/
/****************************************/
wprintw(win1,"Размечаем таблицу Glksval\n");
wrefresh(win1);

strcpy(strsql,"CREATE TABLE Glksval (\
kod  char(16) not null primary key default '',\
naik varchar(128) not null default '',\
kom  varchar(255) NOT NULL DEFAULT '',\
ktoz smallint unsigned not null default 0,\
vrem int unsigned not null default 0) ENGINE = MYISAM");

/*
0 kod   код валюты
1 naik  наименование валюты
2 kom   коментарий
3 ktoz  кто записал
4 vrem  время записи
*/
if(admin_alter_r_crtb(strsql,win1) == 0)
 {
  sprintf(strsql,"insert into Glksval values ('%s','%s','',%d,%ld)",gettext("дол"),gettext("Доллар"),iceb_t_getuid(),time(NULL));
  sql_zapis(strsql,1,0);
  sprintf(strsql,"insert into Glksval values ('%s','%s','',%d,%ld)",gettext("евр"),gettext("Евро"),iceb_t_getuid(),time(NULL));
  sql_zapis(strsql,1,0);
  sprintf(strsql,"insert into Glksval values ('%s','%s','',%d,%ld)",gettext("руб"),gettext("Рубли"),iceb_t_getuid(),time(NULL));
  sql_zapis(strsql,1,0);
 }

wprintw(win1,"Размечаем таблицу Glkkv\n");
wrefresh(win1);

strcpy(strsql,"CREATE TABLE Glkkv (\
kod  char(16) not null default '',\
datk DATE not null default '0000-00-00',\
kurs double(12,4) NOT NULL DEFAULT 0.,\
kom  varchar(255) NOT NULL DEFAULT '',\
ktoz smallint unsigned not null default 0,\
vrem int unsigned not null default 0,\
unique(kod,datk)) ENGINE = MYISAM");

/*
0 kod   код валюты
1 datk  дата 
2 kurs  курс валюты
3 kom   коментарий
4 ktoz  кто записал
5 vrem  время записи
*/
admin_alter_r_crtb(strsql,win1);


wprintw(win1,"Вставляем колонку kv в таблицу Kas\n");
wrefresh(win1);

sprintf(strsql,"alter table Kas add kv char(16) not null default ''");
admin_alter_r_vnk(strsql,win1);

jump04092013:;

sprintf(strsql,"update Alx set str='Печатать наименование счёта корреспондента в Главной книге|Включено\n' where fil='nastrb.alx' and str like 'Печатать наименование счёта кореспондента в Главной книге|%%'");
sql_zapis(strsql,1,0);

sprintf(strsql,"update Alx set str='Печатать наименование счёта корреспондента в журналах-ордерах для счетов с развёрнутым сальдо|Включено\n' where fil='nastrb.alx' and str like 'Печатать наименование счёта кореспондента в журналах-ордерах для счетов с развёрнутым сальдо|%%'");
sql_zapis(strsql,1,0);

sprintf(strsql,"update Alx set str='Печатать фамилию оператора в распечатках отчётов|Включено\n' where fil='nastsys.alx' and str like 'Печатать фамилию оператора в распечатках отчетов|%%'");
sql_zapis(strsql,1,0);


if(iceb_t_poldan("Коды начислений не входящие в расчет подоходного налога",&bros,"zarnast.alx") == 0)
 {
  sprintf(strsql,"update Alx set str='Коды начислений не входящие в расчёт подоходного налога|%s\n' where fil='zarnast.alx' and str like 'Коды начислений не входящие в расчет подоходного налога|%%'",bros.ravno());
  sql_zapis(strsql,1,0);
  
 }

if(iceb_t_poldan("Коды не входящие в расчет доплаты до минимальной зарплаты",&bros,"zarnast.alx") == 0)
 {
  sprintf(strsql,"update Alx set str='Коды не входящие в расчёт доплаты до минимальной зарплаты|%s\n' where fil='zarnast.alx' and str like 'Коды не входящие в расчет доплаты до минимальной зарплаты|%%'",bros.ravno());
  sql_zapis(strsql,1,0);
  
 }

if(iceb_t_poldan("Коды отдельного расчета доплаты до минимальной зарплаты",&bros,"zarnast.alx") == 0)
 {
  sprintf(strsql,"update Alx set str='Коды отдельного расчёта доплаты до минимальной зарплаты|%s\n' where fil='zarnast.alx' and str like 'Коды отдельного расчета доплаты до минимальной зарплаты|%%'",bros.ravno());
  sql_zapis(strsql,1,0);
  
 }

if(iceb_t_poldan("Коды удержаний входящие в расчет подоходного налога",&bros,"zarnast.alx") == 0)
 {
  sprintf(strsql,"update Alx set str='Коды удержаний входящие в расчёт подоходного налога|%s\n' where fil='zarnast.alx' and str like 'Коды удержаний входящие в расчет подоходного налога|%%'",bros.ravno());
  sql_zapis(strsql,1,0);
  
 }

if(iceb_t_poldan("Дополнительные зарплатные счёта",&bros,"zarnast.alx") == 0)
 {
  sprintf(strsql,"update Alx set str='Дополнительные зарплатные счета|%s\n' where fil='zarnast.alx' and str like 'Дополнительные зарплатные счёта|%%'",bros.ravno());
  sql_zapis(strsql,1,0);
  
 }

if(iceb_t_poldan("Коды удержаний входящие в расчет алиментов",&bros,"zaralim.alx") == 0)
 {
  sprintf(strsql,"update Alx set str='Коды удержаний входящие в расчёт алиментов|%s\n' where fil='zaralim.alx' and str like 'Коды удержаний входящие в расчет алиментов|%%'",bros.ravno());
  sql_zapis(strsql,1,0);
 }

jump18092013:; /*****************************************/

if(iceb_t_poldan("Код перерасчета подоходного налога",&bros,"zarnast.alx") == 0)
 {
  sprintf(strsql,"update Alx set str='Код перерасчёта подоходного налога|%s\n' where fil='zarnast.alx' and str like 'Код перерасчета подоходного налога|%%'",bros.ravno());
  sql_zapis(strsql,1,0);
  
 }

if(iceb_t_poldan("Банковский счет",&bros,"nastdok.alx") == 0)
 {
  sprintf(strsql,"update Alx set str='Банковский счёт|%s\n' where fil='nastdok.alx' and str like 'Банковский счет|%%'",bros.ravno());
  sql_zapis(strsql,1,0);
  
 }

if(iceb_t_poldan("Расчетный счет казначейства",&bros,"uslnastk.alx") == 0)
 {
  sprintf(strsql,"update Alx set str='Расчётный счёт казначейства|%s\n' where fil='uslnastk.alx' and str like 'Расчетный счет казначейства|%%'",bros.ravno());
  sql_zapis(strsql,1,0);
  
 }
if(iceb_t_poldan("Регистрационный счет в казначействе",&bros,"uslnast.alx") == 0)
 {
  sprintf(strsql,"update Alx set str='Регистрационный счёт в казначействе|%s\n' where fil='uslnastk.alx' and str like 'Регистрационный счет в казначействе|%%'",bros.ravno());
  sql_zapis(strsql,1,0);
  
 }
if(iceb_t_poldan("Коды начислений не входящие в расчет",&bros,"zaralim.alx") == 0)
 {
  sprintf(strsql,"update Alx set str='Коды начислений не входящие в расчёт|%s\n' where fil='zaralim.alx' and str like 'Коды начислений не входящие в расчет|%%'",bros.ravno());
  sql_zapis(strsql,1,0);
 }

if(iceb_t_poldan("Профсоюзный фонд-коды начислений не входящие в расчет",&bros,"zarnast.alx") == 0)
 {
  sprintf(strsql,"update Alx set str='Профсоюзный фонд-коды начислений не входящие в расчёт|%s\n' where fil='zarnast.alx' and str like 'Профсоюзный фонд-коды начислений не входящие в расчет|%%'",bros.ravno());
  sql_zapis(strsql,1,0);
 }

if(iceb_t_poldan("Коды не входящие в расчет индексации",&bros,"zarnast.alx") == 0)
 {
  sprintf(strsql,"update Alx set str='Коды не входящие в расчёт индексации|%s\n' where fil='zarnast.alx' and str like 'Коды не входящие в расчет индексации|%%'",bros.ravno());
  sql_zapis(strsql,1,0);
 }

if(iceb_t_poldan("Коды начислений не входящие в расчет командировочных",&bros,"zarnast.alx") == 0)
 {
  sprintf(strsql,"update Alx set str='Коды начислений не входящие в расчёт командировочных|%s\n' where fil='zarnast.alx' and str like 'Коды начислений не входящие в расчет командировочных|%%'",bros.ravno());
  sql_zapis(strsql,1,0);
 }

if(iceb_t_poldan("Коды табеля не входящие в расчет командировочных",&bros,"zarnast.alx") == 0)
 {
  sprintf(strsql,"update Alx set str='Коды табеля не входящие в расчёт командировочных|%s\n' where fil='zarnast.alx' and str like 'Коды табеля не входящие в расчет командировочных|%%'",bros.ravno());
  sql_zapis(strsql,1,0);
 }

if(iceb_t_poldan("Код перечисления зарплаты на счет",&bros,"zarnast.alx") == 0)
 {
  sprintf(strsql,"update Alx set str='Код перечисления зарплаты на счёт|%s\n' where fil='zarnast.alx' and str like 'Код перечисления зарплаты на счет|%%'",bros.ravno());
  sql_zapis(strsql,1,0);
 }

if(iceb_t_poldan("Счет расчетов по зарплате",&bros,"zarnast.alx") == 0)
 {
  sprintf(strsql,"update Alx set str='Счёт расчетов по зарплате|%s\n' where fil='zarnast.alx' and str like 'Счет расчетов по зарплате|%%'",bros.ravno());
  sql_zapis(strsql,1,0);
 }

if(iceb_t_poldan("Коды не входящие в расчет отпускных",&bros,"zarotp.alx") == 0)
 {
  sprintf(strsql,"update Alx set str='Коды не входящие в расчёт отпускных|%s\n' where fil='zarotp.alx' and str like 'Коды не входящие в расчет отпускных|%%'",bros.ravno());
  sql_zapis(strsql,1,0);
 }

if(iceb_t_poldan("Коды не входящие в расчет индексации отпускных",&bros,"zarotp.alx") == 0)
 {
  sprintf(strsql,"update Alx set str='Коды не входящие в расчёт индексации отпускных|%s\n' where fil='zarotp.alx' and str like 'Коды не входящие в расчет индексации отпускных|%%'",bros.ravno());
  sql_zapis(strsql,1,0);
 }

if(iceb_t_poldan("Коды не входящие в расчет средней зарплаты для начисления больничного",&bros,"zarsrbol.alx") == 0)
 {
  sprintf(strsql,"update Alx set str='Коды не входящие в расчёт средней зарплаты для начисления больничного|%s\n' where fil='zarsrbol.alx' and str like 'Коды не входящие в расчет средней зарплаты для начисления больничного|%%'",bros.ravno());
  sql_zapis(strsql,1,0);
 }

if(iceb_t_poldan("Печатать коды невходящие в расчет",&bros,"zarsrbol.alx") == 0)
 {
  sprintf(strsql,"update Alx set str='Печатать коды невходящие в расчёт|%s\n' where fil='zarsrbol.alx' and str like 'Печатать коды невходящие в расчет|%%'",bros.ravno());
  sql_zapis(strsql,1,0);
 }

if(iceb_t_poldan("Печатать коды невходящие в расчет",&bros,"zarsrbold.alx") == 0)
 {
  sprintf(strsql,"update Alx set str='Печатать коды невходящие в расчёт|%s\n' where fil='zarsrbold.alx' and str like 'Печатать коды невходящие в расчет|%%'",bros.ravno());
  sql_zapis(strsql,1,0);
 }

if(iceb_t_poldan("Округление 1",&bros,"matnast.alx") == 0)
 {
  sprintf(strsql,"update Alx set str='Округление суммы|%s\n' where fil='matnast.alx' and str like 'Округление 1|%%'",bros.ravno());
  sql_zapis(strsql,1,0);
 }

admin_alter_r_ins_str_alx("kasnast.alx","Максимальная сумма за день от контрагента","10000",win1);

jump21092013:; /********14.3*******************************/

if(iceb_t_poldan("Счёт расчетов по зарплате",&bros,"zarnast.alx") == 0)
 {
  sprintf(strsql,"update Alx set str='Счёт расчётов по зарплате|%s\n' where fil='zarnast.alx' and str like 'Счёт расчетов по зарплате|%%'",bros.ravno());
  sql_zapis(strsql,1,0);
 }

if(iceb_t_poldan("Счет подоходного налога",&bros,"zarnast.alx") == 0)
 {
  sprintf(strsql,"update Alx set str='Счёт подоходного налога|%s\n' where fil='zarnast.alx' and str like 'Счет подоходного налога|%%'",bros.ravno());
  sql_zapis(strsql,1,0);
 }

wprintw(win1,"Увеличиваем длинну колонки sodz в таблице Dokummat2\n");
wrefresh(win1);

sprintf(strsql,"alter table Dokummat2 change sodz sodz varchar(255) not null default ''");

if(sql_zap(&bd,strsql) != 0)
 {
  wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
  OSTANOVW(win1);
 }

jump28092013:; /********14.4*******************************/

//sprintf(strsql,"delete from Alx where fil='ttnl1.alx'");
//sql_zapis(strsql,1,0);

jump31102013:; /********14.7*******************************/

if(iceb_t_poldan("Экспортировать для регистрации в налоговой свою налоговую накладную",&bros,"rnn_nast.alx") == 0)
 {
  sprintf(strsql,"update Alx set str='Экспорт в ОПЗ|%s\n' where fil='rnn_nast.alx' and str like 'Экспортировать для регистрации в налоговой свою налоговую накладную|%%'",bros.ravno());
  sql_zapis(strsql,1,0);
 }

jump24112013:; /********14.9*******************************/

sprintf(strsql,"delete from Alx where fil='nastdok.alx' and str like 'Разделитель копеек|%%'");
sql_zapis(strsql,1,0);

jump27122013:; /********14.9*******************************/
sprintf(strsql,"delete from Alx where fil='nastsys.alx' and str like 'Кодировка в которой работает iceB|%%'");
sql_zapis(strsql,1,0);

wprintw(win1,"%s vnn %s Kontragent2\n",gettext("Вставляем колонку"),gettext("в таблицу"));
wrefresh(win1);

sprintf(strsql,"alter table Kontragent2 add vnn VARCHAR(16) NOT NULL DEFAULT ''");
admin_alter_r_vnk(strsql,win1);

jump10012014:; /********14.10*******************************/

admin_alter_r_ins_str_alx("zarnast.alx","Коды начислений дополнительно входящие в расчёт удержания ЕСВ для больничных","",win1);

jump29012014:; /********14.10*******************************/
/*загрузка новых настроечных файлов*/

jump20022014:; /********14.11*******************************/
admin_alter_r_ins_str_alx("rnn_nimp1.alx","Материальный учёт колонки 11-12","",win1);
admin_alter_r_ins_str_alx("rnn_nimp1.alx","Учёт услуг колонки 11-12","",win1);
admin_alter_r_ins_str_alx("rnn_nimp1.alx","Учёт основных средств колонки 11-12","",win1);
admin_alter_r_ins_str_alx("rnn_nimp1.alx","Процент расчёта колонок 11-12","",win1);

jump25022014:; /********14.11*******************************/
/*загрузка новых настроечных файлов*/

/*Удаляем файлы для того чтобы загрузились новые версии этих файлов*/
wprintw(win1,"%s nn5_start.alx\n",gettext("Удаляем файл"));
wrefresh(win1);

sprintf(strsql,"delete from Alx where fil='nn5_start.alx'");
sql_zapis(strsql,1,0);

wprintw(win1,"%s nn4_hap.alx\n",gettext("Удаляем файл"));
wrefresh(win1);
sprintf(strsql,"delete from Alx where fil='nn4_hap.alx'");
sql_zapis(strsql,1,0);

jump12032014:; /********14.12*******************************/


/*Удаляем как не нужные*/
wprintw(win1,"%s rnnd5_start.alx\n",gettext("Удаляем файл"));
wrefresh(win1);
sprintf(strsql,"delete from Alx where fil='rnnd5_start.alx'");
sql_zapis(strsql,1,0);

wprintw(win1,"%s rnnd2_start.alx\n",gettext("Удаляем файл"));
wrefresh(win1);
sprintf(strsql,"delete from Alx where fil='rnnd2_start.alx'");
sql_zapis(strsql,1,0);

wprintw(win1,"%s rnnd5_end.alx\n",gettext("Удаляем файл"));
wrefresh(win1);
sprintf(strsql,"delete from Alx where fil='rnnd5_end.alx'");
sql_zapis(strsql,1,0);

/*заменяем на новые*/
wprintw(win1,"%s nn4_hap.alx\n",gettext("Удаляем файл"));
wrefresh(win1);
sprintf(strsql,"delete from Alx where fil='nn4_hap.alx'");
sql_zapis(strsql,1,0);

wprintw(win1,"%s nn4_end.alx\n",gettext("Удаляем файл"));
wrefresh(win1);
sprintf(strsql,"delete from Alx where fil='nn4_end.alx'");
sql_zapis(strsql,1,0);

jump08042014:; /********14.13*******************************/

wprintw(win1,"%s nn4_end.alx\n",gettext("Удаляем файл"));
wrefresh(win1);
sprintf(strsql,"delete from Alx where fil='nn4_end.alx'");
sql_zapis(strsql,1,0);
wprintw(win1,"%s nn5_start.alx\n",gettext("Удаляем файл"));
wrefresh(win1);
sprintf(strsql,"delete from Alx where fil='nn5_start.alx'");
sql_zapis(strsql,1,0);
/*новые настроечные файлы*/
jump01052014:; /********14.14*******************************/

wprintw(win1,"%s nn5_start.alx\n",gettext("Удаляем файл"));
wrefresh(win1);
sprintf(strsql,"delete from Alx where fil='nn5_start.alx'");
sql_zapis(strsql,1,0);

jump14052014:; /********14.15*******************************/

wprintw(win1,"%s nn5_start.alx\n",gettext("Удаляем файл"));
wrefresh(win1);
sprintf(strsql,"delete from Alx where fil='nn5_start.alx'");
sql_zapis(strsql,1,0);

jump30052014:; /********14.15*******************************/

admin_alter_r_ins_str_alx("zarnast.alx","Коды табеля входящие в расчёт индексации","",win1);

jump03062014:; /********14.15*******************************/

wprintw(win1,"%s nn5_start.alx\n",gettext("Удаляем файл"));
wrefresh(win1);
sprintf(strsql,"delete from Alx where fil='nn5_start.alx'");
sql_zapis(strsql,1,0);

wprintw(win1,"%s nn4_end.alx\n",gettext("Удаляем файл"));
wrefresh(win1);
sprintf(strsql,"delete from Alx where fil='nn4_end.alx'");
sql_zapis(strsql,1,0);

wprintw(win1,"%s nn4_hap.alx\n",gettext("Удаляем файл"));
wrefresh(win1);
sprintf(strsql,"delete from Alx where fil='nn4_hap.alx'");
sql_zapis(strsql,1,0);

jump03072014:; /********14.16*******************************/

wprintw(win1,"%s rpvnn1_3.alx\n",gettext("Удаляем файл"));
wrefresh(win1);
sprintf(strsql,"delete from Alx where fil='rpvnn1_3.alx'");
sql_zapis(strsql,1,0);


wprintw(win1,"%s rpvnn2_3.alx\n",gettext("Удаляем файл"));
wrefresh(win1);
sprintf(strsql,"delete from Alx where fil='rpvnn2_3.alx'");
sql_zapis(strsql,1,0);

jump19072014:; /********14.16*******************************/

wprintw(win1,"%s vsb,vsn,vdb %s icebuser\n",gettext("Вставляем колонку"),gettext("в таблицу"));
wrefresh(win1);

sprintf(strsql,"alter table icebuser add \
vsd tinyint not null default 0,\
add vsb tinyint not null default 0,\
add vsn tinyint not null default 0");

admin_alter_r_vnk(strsql,win1);


wprintw(win1,"%s vdb %s icebuser\n",gettext("Вставляем колонку"),gettext("в таблицу"));
wrefresh(win1);

sprintf(strsql,"alter table icebuser add vdb tinyint not null default 0");
admin_alter_r_vnk(strsql,win1);


jump31072014:; /********14.17*******************************/

admin_alter_r_ins_str_alx("zarnast.alx","Код военного налога","",win1);
admin_alter_r_ins_str_alx("zarnast.alx","Коды не входящие в расчёт военного налога","",win1);

jump12082014:; /********14.17*******************************/

wprintw(win1,"%s post %s Uddok\n",gettext("Увеличиваем размер колонки"),gettext("в таблице"));
wrefresh(win1);

sprintf(strsql,"alter table Uddok change post post varchar(255) not null default ''");

if(sql_zap(&bd,strsql) != 0)
 {
  wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
  OSTANOVW(win1);
 }

jump28082014:; /********14.17*******************************/

/*загрузка новых настроечных файлов*/

jump27112014:; /********15.1*******************************/

/*загрузка новых настроечных файлов*/
sprintf(strsql,"delete from Alx where fil='rpvnn0.alx'");
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Alx where fil='rpvnn0_1.alx'");
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Alx where fil='rpvnn0_2.alx'");
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Alx where fil='rpvnn0_3.alx'");
sql_zapis(strsql,1,0);


jump26012015:; /********15.4*******************************/

wprintw(win1,"%s Dokummat\n",gettext("Преобразуем колонки в таблице"));
wrefresh(win1);

sprintf(strsql,"alter table Dokummat change tip tip smallint not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Dokummat change kodop kodop char(8) not null default ''");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Dokummat change pod pod smallint not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Dokummat change pro pro smallint not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Dokummat change ktoi ktoi smallint unsigned not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Dokummat change vrem vrem int unsigned not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Dokummat change nomon nomon char(16) not null default ''");
sql_zapis(strsql,1,0);

wprintw(win1,"%s Dokummat1\n",gettext("Преобразуем колонки в таблице"));
wrefresh(win1);

sprintf(strsql,"alter table Dokummat1 change kolih kolih double(16,6) not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Dokummat1 change cena cena double(16,6) not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Dokummat1 change voztar voztar smallint not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Dokummat1 change nds nds float(2) not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Dokummat1 change mnds mnds smallint not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Dokummat1 change ktoi ktoi smallint unsigned not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Dokummat1 change vrem vrem int unsigned not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Dokummat1 change tipz tipz smallint not null default 0");
sql_zapis(strsql,1,0);

wprintw(win1,"%s Material\n",gettext("Преобразуем колонки в таблице"));
wrefresh(win1);

sprintf(strsql,"alter table Material change krat krat double not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Material change fasv fasv double not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Material change kodt kodt int not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Material change ktoz ktoz smallint unsigned not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Material change vrem vrem int unsigned not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Material change nds nds  real not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Material change msp msp smallint not null default 0");
sql_zapis(strsql,1,0);


wprintw(win1,"%s Usldokum\n",gettext("Преобразуем колонки в таблице"));
wrefresh(win1);

sprintf(strsql,"alter table Usldokum change nds nds smallint not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Usldokum change forop forop  varchar(20) not null default ''");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Usldokum change datop datop  DATE not null default '0000-00-00'");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Usldokum change uslpr uslpr  varchar(80) not null default ''");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Usldokum change sumkor sumkor double(10,2) not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Usldokum change pod pod    smallint not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Usldokum change pro pro    smallint not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Usldokum change oplata oplata smallint not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Usldokum change blokir blokir smallint unsigned not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Usldokum change nomdp nomdp  varchar(20) not null default ''");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Usldokum change datdp datdp  DATE not null default '0000-00-00'");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Usldokum change dover dover  varchar(20) not null default ''");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Usldokum change datdov datdov DATE not null default '0000-00-00'");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Usldokum change sherez sherez varchar(40) not null default ''");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Usldokum change ktoi ktoi   smallint unsigned not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Usldokum change vrem vrem   int unsigned not null default 0");
sql_zapis(strsql,1,0);

wprintw(win1,"%s Usldokum1\n",gettext("Преобразуем колонки в таблице"));
wrefresh(win1);

sprintf(strsql,"alter table Usldokum1 change cena cena   double(16,6) not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Usldokum1 change ktoi ktoi   smallint unsigned not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Usldokum1 change vrem vrem   int unsigned not null default 0");
sql_zapis(strsql,1,0);

wprintw(win1,"%s Uslugi\n",gettext("Преобразуем колонки в таблице"));
wrefresh(win1);

sprintf(strsql,"alter table Uslugi change kodgr kodgr   smallint not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Uslugi change naius naius   varchar(80) not null default ''");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Uslugi change ei ei varchar(20) not null default ''");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Uslugi change cena cena    double(10,2) not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Uslugi change nds nds real not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Uslugi change ktoz ktoz	smallint unsigned not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Uslugi change vrem vrem	int unsigned not null default 0");
sql_zapis(strsql,1,0);

jump01022015:; /********15.5*******************************/


wprintw(win1,"%s kei %s Edizmer\n",gettext("Вставляем колонку"),gettext("в таблицу"));
wrefresh(win1);

sprintf(strsql,"alter table Edizmer add kei char(16) not null default ''");
admin_alter_r_vnk(strsql,win1);

sprintf(strsql,"alter table Kontragent change ktoz ktoz	smallint unsigned not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"alter table Kontragent change vrem vrem	int unsigned not null default 0");
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Alx where fil='nn7_hap.alx'");
sql_zapis(strsql,1,0);
sprintf(strsql,"delete from Alx where fil='nn7_end.alx'");
sql_zapis(strsql,1,0);

jump09022015:; /********15.6*******************************/

/*новый настроечный файл*/

jump02032015:; /********15.7*******************************/

wprintw(win1,"%s F1dfvs\n",gettext("Создаём новую таблицу"));
wrefresh(win1);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE F1dfvs (\
nomd   varchar(20) not null default '',\
inn    char(10)  not null default '',\
fio    varchar(60) not null default '',\
sfr  double(10,2) not null default 0,\
svs  double(8,2) not null default 0,\
sfrv  double(10,2) not null default 0,\
svsv  double(8,2) not null default 0,\
ktoz   smallint unsigned not null default 0,\
vrem   int unsigned not null default 0,\
unique(nomd,inn)) ENGINE = MYISAM");

/*
0 nomd   номер документа
1 inn    индивидуальный налоговый номер
2 fio    фамилия для произвольного ввода записи
3 sfr    сумма с которой был сделан расчёт
4 svs    сумма военного сбора
5 sfrv   сумма с коророй был сделан расчёт выплаченная
6 svsv   сумма военного сбора выплаченная
7 ktoz  кто записал
8 vrem  время записи
*/

admin_alter_r_crtb(strsql,win1);

wprintw(win1,"%s kr %s F8dr\n",gettext("Вставляем колонку"),gettext("в таблицу"));
wrefresh(win1);

sprintf(strsql,"alter table F8dr add kr INT NOT NULL DEFAULT 0");
admin_alter_r_vnk(strsql,win1);

wprintw(win1,"%s ks %s F8dr\n",gettext("Вставляем колонку"),gettext("в таблицу"));
wrefresh(win1);

sprintf(strsql,"alter table F8dr add ks INT NOT NULL DEFAULT 0");
admin_alter_r_vnk(strsql,win1);

if(iceb_t_poldan("Код военного налога",&bros,"zarnast.alx") == 0)
 {

  sprintf(strsql,"delete from Alx where fil='zarnast.alx' and str like 'Код военного налога|%%'");
  sql_zapis(strsql,1,0);

  admin_alter_r_ins_str_alx("zarnast.alx","Код военного сбора",bros.ravno(),win1);
 }

if(iceb_t_poldan("Коды не входящие в расчёт военного налога",&bros,"zarnast.alx") == 0)
 {

  sprintf(strsql,"delete from Alx where fil='zarnast.alx' and str like 'Коды не входящие в расчёт военного налога|%%'");
  sql_zapis(strsql,1,0);

  admin_alter_r_ins_str_alx("zarnast.alx","Коды не входящие в расчёт военного сбора",bros.ravno(),win1);
 }

jump21042015:; /********15.9*******************************/

admin_alter_r_ins_str_alx("nastsys.alx","Перекодировка XML файлов при копировании на Windows машину","Выключено",win1);

sprintf(strsql,"update Alx set str='Кодовая таблица|%s\n' where fil='rnn_nast.alx' and str like 'Кодовая таблица|%%'","utf-8");
sql_zapis(strsql,1,0);

jump09052015:; /********15.10*******************************/



sprintf(strsql,"alter table Uoamor add kmo INT NOT NULL DEFAULT 0");
if(admin_alter_r_vnk(strsql,win1) == 0)
 {
  wprintw(win1,"%s Uosamor\n",gettext("Преобразуем таблицу"));
  wrefresh(win1);

  admin_alter_r_amort("Uosamor",win1);
 }
sprintf(strsql,"alter table Uoamor1 add kmo INT NOT NULL DEFAULT 0");
if(admin_alter_r_vnk(strsql,win1) == 0)
 {
  wprintw(win1,"%s Uosamor1\n",gettext("Преобразуем таблицу"));
  wrefresh(win1);

  admin_alter_r_amort("Uosamor1",win1);
 }

jump03072015:; /********15.14*******************************/

sprintf(strsql,"delete from Alx where fil='zarnast.alx' and str like 'Максимальная сумма необлагаемой благотворительной помощи|%%'");
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Alx where fil='zarnast.alx' and str like 'Счёт подоходного налога|%%'");
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Alx where fil='zarnast.alx' and str like 'Автоматическая вставка кода контрагента в список счета|%%'");
sql_zapis(strsql,1,0);

admin_alter_r_ins_str_alx("zarotp.alx","Коды табеля не входящие в расчёт","",win1);

jump28072015:; /********15.16*******************************/

admin_alter_r_ins_str_alx("zarsdf1df.alx","Коды командировочных расходов для кода дохода NN","",win1);

iceb_t_poldan("Код контрагента с нужными банковскими реквизитами",&bros,"nastud.alx");

admin_alter_r_ins_str_alx("nastud.alx","Код контрагента получателя",bros.ravno(),win1);
admin_alter_r_ins_str_alx("nastud.alx","Код контрагента плательщика","",win1);

sprintf(strsql,"delete from Alx where fil='nastud.alx' and str like 'Код контрагента с нужными банковскими реквизитами|%%'");
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Alx where fil='zarnast.alx' and str like 'Дополнительные зарплатные счёта|%%'");
sql_zapis(strsql,1,0);
sprintf(strsql,"delete from Alx where fil='zarnast.alx' and str like 'Дополнительные зарплатные счета|%%'");
sql_zapis(strsql,1,0);
sprintf(strsql,"delete from Alx where fil='zarnast.alx' and str like 'Начисления, для которых проводки выполняются по дополнительному счёту%%'");
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Alx where fil='zarsrbol.alx' and str like 'Коды начисления отпускных|%%'");
sql_zapis(strsql,1,0);
sprintf(strsql,"delete from Alx where fil='zarsrbol.alx' and str like 'Коды видов табеля отпускных|%%'");
sql_zapis(strsql,1,0);

jump01092015:; /********16.0*******************************/

admin_alter_r_ins_str_alx("zarpensm.alx","Коды начисления помощи по беременности и родам","",win1);
admin_alter_r_ins_str_alx("zarpensm.alx","Коды табеля по беременности и родам","",win1);

jump07092015:; /********16.0*******************************/
/*Новые настроечные файлы*/
jump22092015:; /********16.0*******************************/

wprintw(win1,"%s dolg %s Uosol\n",gettext("Вставляем колонку"),gettext("в таблицу"));
wrefresh(win1);

sprintf(strsql,"alter table Uosol add dolg varchar(100) NOT NULL DEFAULT ''");
admin_alter_r_vnk(strsql,win1);

jump02102015:; /********16.1*******************************/

/*Новые настроечные файлы*/

jump03122015:; /********16.3*******************************/

sprintf(strsql,"delete from Alx where fil='invmu572z.alx'");
sql_zapis(strsql,1,0);

if(iceb_t_poldan("Код фонда единого социального взноса",&bros,"zarnast.alx") == 0)
 {
  if(bros.ravno_atoi() > 0)
   {
    wprintw(win1,"Код фонда единого социального взноса-%s Zaresv\n",gettext("Вставляем запись с новыми процентами в таблицу"));
    wrefresh(win1);
    sprintf(strsql,"insert into Zaresv (kf,datnd,pr,pr1,ktoz,vrem) values (%d,'2016-01-01',22.,0.,%d,%ld)",bros.ravno_atoi(),iceb_t_getuid(),time(NULL));

    sql_zapis(strsql,1,0);
   }
 }

if(iceb_t_poldan("Код фонда единого социального взноса для инвалидов",&bros,"zarnast.alx") == 0)
 {
  if(bros.ravno_atoi() > 0)
   {
    wprintw(win1,"Код фонда единого социального взноса для инвалидов-%s Zaresv\n",gettext("Вставляем запись с новыми процентами в таблицу"));
    wrefresh(win1);
    sprintf(strsql,"insert into Zaresv (kf,datnd,pr,pr1,ktoz,vrem) values (%d,'2016-01-01',8.41,0.,%d,%ld)",bros.ravno_atoi(),iceb_t_getuid(),time(NULL));

    sql_zapis(strsql,1,0);
   }
 }


jump12012016:; /********16.4*******************************/

if(iceb_t_poldan("Код фонда единого социального взноса для больничных",&bros,"zarnast.alx") == 0)
 {
  if(bros.ravno_atoi() > 0)
   {
    wprintw(win1,"Код фонда единого социального взноса для больничных-%s Zaresv\n",gettext("Вставляем запись с новыми процентами в таблицу"));
    wrefresh(win1);
    sprintf(strsql,"insert into Zaresv (kf,datnd,pr,pr1,ktoz,vrem) values (%d,'2016-01-01',22.,0.,%d,%ld)",bros.ravno_atoi(),iceb_t_getuid(),time(NULL));

    sql_zapis(strsql,1,0);
   }
 }

if(iceb_t_poldan("Код фонда единого социального взноса с больничных для инвалидов",&bros,"zarnast.alx") == 0)
 {
  if(bros.ravno_atoi() > 0)
   {
    wprintw(win1,"Код фонда единого социального взноса с больничных для инвалидов-%s Zaresv\n",gettext("Вставляем запись с новыми процентами в таблицу"));
    wrefresh(win1);
    sprintf(strsql,"insert into Zaresv (kf,datnd,pr,pr1,ktoz,vrem) values (%d,'2016-01-01',8.41,0.,%d,%ld)",bros.ravno_atoi(),iceb_t_getuid(),time(NULL));

    sql_zapis(strsql,1,0);
   }
 }
if(iceb_t_poldan("Код фонда единого социального взноса для договоров",&bros,"zarnast.alx") == 0)
 {
  if(bros.ravno_atoi() > 0)
   {
    wprintw(win1,"Код фонда единого социального взноса для договоров-%s Zaresv\n",gettext("Вставляем запись с новыми процентами в таблицу"));
    wrefresh(win1);
    sprintf(strsql,"insert into Zaresv (kf,datnd,pr,pr1,ktoz,vrem) values (%d,'2016-01-01',22.,0.,%d,%ld)",bros.ravno_atoi(),iceb_t_getuid(),time(NULL));

    sql_zapis(strsql,1,0);
   }
 }
if(iceb_t_poldan("Код фонда единого социального взноса для военных",&bros,"zarnast.alx") == 0)
 {
  if(bros.ravno_atoi() > 0)
   {
    wprintw(win1,"Код фонда единого социального взноса для военных-%s Zaresv\n",gettext("Вставляем запись с новыми процентами в таблицу"));
    wrefresh(win1);
    sprintf(strsql,"insert into Zaresv (kf,datnd,pr,pr1,ktoz,vrem) values (%d,'2016-01-01',22.,0.,%d,%ld)",bros.ravno_atoi(),iceb_t_getuid(),time(NULL));

    sql_zapis(strsql,1,0);
   }
 }

jump18012016:; /********16.4*******************************/

wprintw(win1,"Увеличиваем длинну колонки naikon в таблице Kontragent\n");
wrefresh(win1);

sprintf(strsql,"alter table Kontragent change naikon naikon varchar(255) not null default ''");

if(sql_zap(&bd,strsql) != 0)
 {
  wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
  OSTANOVW(win1);
 }

jump04022016:; /********16.5*******************************/

wprintw(win1,"Добавляем индекс pnaim в таблицу Kontragent\n");
wrefresh(win1);

sprintf(strsql,"alter table Kontragent add index pnaim (pnaim)");

if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_KEYNAME)
   {
    wprintw(win1,"Индекс уже существует\n");
    wrefresh(win1);
   }
 else
  wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));

 }

jump08022016:; /********16.5*******************************/

sprintf(strsql,"delete from Alx where fil='zarsns_end.alx'");
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Alx where fil='zarsns_start.alx'");
sql_zapis(strsql,1,0);

jump24022016:; /********16.5*******************************/

sprintf(strsql,"delete from Alx where fil='glk_balans2.alx'");
sql_zapis(strsql,1,0);

wprintw(win1,"%s bs %s Plansh\n",gettext("Вставляем колонку"),gettext("в таблицу"));
wrefresh(win1);

sprintf(strsql,"alter table Plansh add bs tinyint not null default 0");
admin_alter_r_vnk(strsql,win1);


jump09032016:; /********16.7*******************************/

wprintw(win1,"%s tn %s icebuser\n",gettext("Вставляем колонку"),gettext("в таблицу"));
wrefresh(win1);

sprintf(strsql,"alter table icebuser add tn int unsigned not null default 0");
admin_alter_r_vnk(strsql,win1);

jump16032016:; /********16.7*******************************/
/***********

sprintf(strsql,"delete from Alx where fil='nn8_start.alx'");
sql_zapis(strsql,1,0);
sprintf(strsql,"delete from Alx where fil='nn8_end.alx'");
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Alx where fil='nn8_hap.alx'");
sql_zapis(strsql,1,0);
***************************/

admin_alter_r_zpktn(win1);

sprintf(strsql,"delete from Alx where fil='zarnast.alx' and str like 'Приставка к табельному номеру|%%'");
sql_zapis(strsql,1,0);

jump30032016:; /********16.7*******************************/

admin_alter_r_skkvp(win1);

jump01042016:; /********16.9*******************************/

sprintf(strsql,"delete from Alx where fil='rnn_nast.alx' and str like 'Экспорт в ОПЗ|%%'");
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Alx where fil='nakl.alx'");
sql_zapis(strsql,1,0);
sprintf(strsql,"delete from Alx where fil='nakls.alx'");
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Alx where fil='nalnakst.alx'");
sql_zapis(strsql,1,0);
sprintf(strsql,"delete from Alx where fil='nalnaks1.alx'");
sql_zapis(strsql,1,0);
sprintf(strsql,"delete from Alx where fil='nalnake.alx'");
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Alx where fil='nn4_hap.alx'");
sql_zapis(strsql,1,0);
sprintf(strsql,"delete from Alx where fil='nn4_end.alx'");
sql_zapis(strsql,1,0);
sprintf(strsql,"delete from Alx where fil='nn4_start.alx'");
sql_zapis(strsql,1,0);
sprintf(strsql,"delete from Alx where fil='nn5_start.alx'");
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Alx where fil='nn6_hap.alx'");
sql_zapis(strsql,1,0);
sprintf(strsql,"delete from Alx where fil='nn6_end.alx'");
sql_zapis(strsql,1,0);
sprintf(strsql,"delete from Alx where fil='nn6_start.alx'");
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Alx where fil='nastsys.alx' and str like 'Шрифт с одинаковой шириной всех символов|%%'");
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Alx where fil='nastsys.alx' and str like 'Шрифт реквизитов разработчика|%%'");
sql_zapis(strsql,1,0);


admin_alter_r_ins_str_alx("zarnast.alx","Коды премии за квартал","",win1);

wprintw(win1,"%s kpd %s Zarskrdh\n",gettext("Вставляем колонку"),gettext("в таблицу"));
wrefresh(win1);

sprintf(strsql,"alter table Zarskrdh add kpd int unsigned not null default 0");
admin_alter_r_vnk(strsql,win1);

jump25052016:; /********16.13*******************************/


admin_alter_r_ins_str_alx("zarotp.alx","Дополнительные коды не входящие в расчёт","",win1);
admin_alter_r_ins_str_alx("zarotp.alx","Коды начислений для дополнительных кодов","",win1);



wprintw(win1,"Создание таблицы справочника должностных окладов Zarsdo\n");
memset(strsql,'\0',sizeof(strsql));


strcpy(strsql,"CREATE TABLE Zarsdo \
(tn INT NOT NULL DEFAULT 0,\
dt DATE NOT NULL DEFAULT '0000-00-00',\
kn INT NOT NULL DEFAULT 0,\
sm DOUBLE(12,2) NOT NULL DEFAULT 0.,\
mt SMALLINT NOT NULL DEFAULT 0,\
kh FLOAT(6,2) NOT NULL DEFAULT 0.,\
ks FLOAT(6,2) NOT NULL DEFAULT 1.,\
sh CHAR(32) NOT NULL DEFAULT '',\
km CHAR(100) NOT NULL DEFAULT '',\
ktoz SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0,\
unique(tn,dt,kn)) ENGINE = MYISAM");
/*
0  tn   табельный номер
1  dt   дата - день всегда 1
2  kn   код начисления
3  sm   сумма 
4  mt   метка суммы
        0 - оплата за месяц расчётная по отработанным дням
        1 - оплата за месяц расчётная по отработанным часам
        2 - оплата за месяц постоянная
        3 - оплата за час
        4 - оплата за день
        5 - оплата по тарифу (минимальная зарплата умноженная на тарифный коэффициент)
5  kh   количество часов в рабочем дне
6  ks   коэффициент ставки по умолчанию 1.
7  sh   бухгалтерский счёт 
8  km   коментарий
9  ktoz	Кто записал
10 vrem	Время записи

*/

if(admin_alter_r_crtb(strsql,win1) == 0)
 admin_alter_r_zdo();

jump31052016:; /********16.13*******************************/

/*обновляем файл*/
sprintf(strsql,"delete from Alx where fil='zarsns_start.alx'");
sql_zapis(strsql,1,0);

jump24062016:; /********16.15*******************************/

if(iceb_t_poldan("Код военного сбора с больничного",strsql,"zarnast.alx") != 0)
 {
  int nom=0;
  for(nom=1;; nom++)
   {
    sprintf(strsql,"select kod from Uder where kod=%d",nom);
    if(readkey(strsql) == 0)
     break;
   }

  class iceb_tu_str shet("");
  class iceb_tu_str shetb("");
  int kodvs=0;
  if(iceb_t_poldan("Код военного сбора",&kodvs,"zarnast.alx") == 0)
   {
    sprintf(strsql,"select shet,shetb from Uder where kod=%d",kodvs);
    if(readkey(strsql,&row,&cur) == 1)
     {
      shet.new_plus(row[0]);
      shetb.new_plus(row[1]);
     }
   }

  sprintf(strsql,"insert into Uder (kod,naik,shet,ktoz,vrem,prov,shetb) values (%d,'%s','%s',%d,%ld,0,'%s')",nom,gettext("Военный сбор c больничного"),shet.ravno(),iceb_t_getuid(),time(NULL),shetb.ravno());
  sql_zapis(strsql,1,0);
  sprintf(strsql,"%d",nom);
  admin_alter_r_ins_str_alx("zarnast.alx","Код военного сбора с больничного",strsql,win1);

  class iceb_tu_str kod_ob_ud("");
  iceb_t_poldan("Обязательные удержания",&kod_ob_ud,"zarnast.alx");
  kod_ob_ud.z_plus(nom);    

  sprintf(strsql,"update Alx set str='Обязательные удержания|%s\n' where fil='zarnast.alx' and str like 'Обязательные удержания|%%'",kod_ob_ud.ravno());
  sql_zapis(strsql,1,0);
 }

jump16112016:; /********17.3*******************************/
/*Загрузка новых настроечных файлов*/


jump22112016:; /********17.3*******************************/

sprintf(strsql,"delete from Alx where fil='zarsns_start.alx'");
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Alx where fil='zarsns_end.alx'");
sql_zapis(strsql,1,0);

wprintw(win1,"Добавляем колонку datsd в таблицу Zardok\n");
wrefresh(win1);
sprintf(strsql,"alter table Zardok add datsd DATE not null default '0000-00-00'");

admin_alter_r_vnk(strsql,win1);

admin_alter_r_ustdat(win1);

jump24112016:; /********17.3*******************************/


wprintw(win1,"Добавляем колонку ku в таблицу Uslugi\n");
wrefresh(win1);
sprintf(strsql,"alter table Uslugi add ku CHAR(32) NOT NULL DEFAULT ''");

admin_alter_r_vnk(strsql,win1);

jump13022017:; /********17.8*******************************/


wprintw(win1,"%s Dokummat4\n",gettext("Создание таблицы списания материалов на детали"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Dokummat4 \
(datd  DATE NOT NULL default '0000-00-00',\
skl    smallint NOT NULL default 0,\
nomd   char(16) NOT NULL default '',\
km     int NOT NULL default 0,\
nk     int NOT NULL default 0,\
kd     int NOT NULL default 0,\
kol    double(20,10) NOT NULL default 0,\
kom    varchar(60) NOT NULL default '',\
ktoz   smallint unsigned NOT NULL default 0,\
vrem   int unsigned NOT NULL default 0,\
nz int unsigned PRIMARY KEY AUTO_INCREMENT NOT NULL,\
index(datd,skl,nomd)) ENGINE = MYISAM");

/*
 0 datd дата документа
 1 skl  код склада
 2 nomd номер документа
 3 km   код материала в документе
 4 nk   номер карточки
 5 kd   код детали на изготовление которой будет использоваться списанный материал
 6 kol  количество материала на указанный код детали (может быть списание на несколько деталей)
 7 kom  коментарий 
 8 ktoz	кто записал
 9 vrem	время записи
10 nz   уникальный номер записи
*/

admin_alter_r_crtb(strsql,win1);

jump24022017:; /********17.8*******************************/



admin_alter_r_ins_str_alx("zarotp.alx","Печать наименований кодов не входящих в расчёт","Включена",win1);

wprintw(win1,"%s Musnrm\n",gettext("Создание таблицы норм расхода материалов"));
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Musnrm \
(nz INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,\
kd   INT NOT NULL DEFAULT 0,\
eid  VARCHAR(10) NOT NULL DEFAULT '',\
km    INT NOT NULL DEFAULT 0,\
kol   DOUBLE(20,10) NOT NULL DEFAULT 0,\
eim   VARCHAR(10) NOT NULL DEFAULT '',\
vs    TINYINT NOT NULL DEFAULT 0,\
vz    TINYINT NOT NULL DEFAULT 0,\
kom   VARCHAR(60) NOT NULL DEFAULT '',\
ktoz  SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem  INT UNSIGNED NOT NULL DEFAULT 0,\
INDEX(kd,eid,km)) ENGINE = MYISAM");

/*
 0 nz    уникальный номер записи
 1 kd    код детали
 2 eid   единица измерения детали
 3 km    код материала заготовки
 4 kol   количество материала заготовки на единицу детали
 5 eim   единица измерения материала заготовки
 6 vs    вид списания 0-основной 1-альтернативный
 7 vz    вид заготовки 0-материал 1-покупная заготовка 
 8 kom   комментарий
 9 ktoz  кто записал
10 vrem	 время записи
*/

admin_alter_r_crtb(strsql,win1);

jump06032017:; /********17.9*******************************/


wprintw(win1,"%s\n","Удаляем старые бланки налоговых накладных");
wrefresh(win1);
sprintf(strsql,"delete from Alx where fil like 'nn7_%%' ");
sql_zapis(strsql,1,0);

wprintw(win1,"%s\n","Добавляем колонки mi,kdstv таблицу Dokummat1");
wrefresh(win1);
sprintf(strsql,"alter table Dokummat1 add mi TINYINT NOT NULL DEFAULT 0,add kdstv VARCHAR(32) NOT NULL DEFAULT ''");

admin_alter_r_vnk(strsql,win1);


jump22032017:; /********17.9*******************************/


sprintf(strsql,"delete from Alx where fil='plansh_bd_ua.alx'"); /*грузим новый план счетов*/
sql_zapis(strsql,1,0);


jump18052017:; /********17.11*******************************/

sprintf(strsql,"delete from Alx where fil='uosavp_end.alx'"); /*грузим исправленный файл*/
sql_zapis(strsql,1,0);


admin_alter_r_ins_str_alx("zarotp.alx","Коды начислений отпускных","",win1);

jump04092017:; /********17.14*******************************/

wprintw(win1,"Увеличиваем количество знаков после запятой колонки cena в таблице Kart\n");
wrefresh(win1);

sprintf(strsql,"alter table Kart change cena cena double(17,7) not null default 0.");
admin_alter_r_vnk(strsql,win1);

wprintw(win1,"Увеличиваем количество знаков после запятой колонки cenap в таблице Kart\n");
wrefresh(win1);
sprintf(strsql,"alter table Kart change cenap cenap double(17,7) not null default 0.");
admin_alter_r_vnk(strsql,win1);

wprintw(win1,"Увеличиваем количество знаков после запятой колонки cena в таблице Zkart\n");
wrefresh(win1);
sprintf(strsql,"alter table Zkart change cena cena double(17,7) not null default 0.");
admin_alter_r_vnk(strsql,win1);

wprintw(win1,"Увеличиваем количество знаков после запятой колонки cena в таблице Dokummat1\n");
wrefresh(win1);
sprintf(strsql,"alter table Dokummat1 change cena cena double(17,7) not null default 0.");
admin_alter_r_vnk(strsql,win1);

sprintf(strsql,"update Alx set str='Округление цены|0.0000001\n' where fil='matnast.alx' and str like 'Округление цены|%%'");
sql_zapis(strsql,1,0);

jump26092017:; /********17.14*******************************/
/*загрузка новых настроечных файлов*/

jump09012018:; /********17.14*******************************/

wprintw(win1,"Увеличиваем количество знаков после запятой колонки kurs в таблице Glkkv\n");
wrefresh(win1);
sprintf(strsql,"alter table Glkkv change kurs kurs double(14,6) not null default 0.");
admin_alter_r_vnk(strsql,win1);

jump10012018:; /********17.14*******************************/


sprintf(strsql,"delete from Alx where fil='zarspv.alx'");
sql_zapis(strsql,1,0);
sprintf(strsql,"delete from Alx where fil='zarspv1.alx'");
sql_zapis(strsql,1,0);
sprintf(strsql,"delete from Alx where fil='zarspv2.alx'");
sql_zapis(strsql,1,0);
sprintf(strsql,"delete from Alx where fil='rnn_nimp1.alx'");
sql_zapis(strsql,1,0);

wprintw(win1,"Увеличиваем количество знаков после запятой колонки kolih в таблице Dokummat1\n");
wrefresh(win1);
sprintf(strsql,"alter table Dokummat1 change kolih kolih double(17,7) not null default 0.");
admin_alter_r_vnk(strsql,win1);

wprintw(win1,"%s pu %s Kontragent2\n",gettext("Вставляем колонку"),gettext("в таблицу"));
wrefresh(win1);

sprintf(strsql,"alter table Kontragent2 add pu tinyint not null default 0");
admin_alter_r_vnk(strsql,win1);

jump27042018:; /********18.4*******************************/

sprintf(strsql,"delete from Alx where fil like 'nn8_%%' ");
sql_zapis(strsql,1,0);


/***************************************************************/

/*Догрузит новые настроечные файлы*/
wprintw(win1,"%s\n",gettext("Загрузка новых настроечных файлов"));
wrefresh(win1);
alxin(put_na_alx,0,win1);


/*Обновляем документцию*/
wprintw(win1,"%s\n",gettext("Обновляем документацию"));
wrefresh(win1);
docin(put_na_doc,win1);

/*Записываем дату версии базы данных*/
sprintf(strsql,"replace into Alx (fil,ns,ktoz,vrem,str) values('iceb_data_baz',1,%d,%ld,'%s')",iceb_t_getuid(),time(NULL),ICEB_DATA_BAZ);
sql_zapis(strsql,0,0);



/*************************/
/*************************/


sql_zapis("USE mysql",1,0);

wrefresh(win1);

return(0);
}

/********************************************************/
/*Подпрограмма отметки групп с пообъектной амортизацией*/
/********************************************************/

void alter_uosgrup_ta(const char *imabaz)
{
char strsql[1024];
char imaf_nast[512];
class iceb_tu_str sp_grup("");
sprintf(imaf_nast,"/%s/%s/uosnast.alx",putnansi,imabaz);

if(poldan("Список групп налогового учёта с не пообъектной амортизацией",&sp_grup,imaf_nast) != 0)
 {
//  iceb_t_soob("Не найден \"Список групп налогового учёта с не пообъектной амортизацией\"!");
 }
else
 { 
  class iceb_tu_str kodgr("");
  for(int ii=1;;ii++)
   {
    if(polen(sp_grup.ravno(),&kodgr,ii,',') != 0)
     break;

    if(kodgr.getdlinna() <= 1)
     break;      

    sprintf(strsql,"update Uosgrup set ta=1 where kod='%s'",kodgr.ravno());
    sql_zapis(strsql,0,0);
   }
 }

if(poldan("Список всех групп налогового учёта",&sp_grup,imaf_nast) != 0)
 {
//  iceb_t_soob("Не найден \"Список всех групп налогового учёта\"!");
 }
else
 { 
  class iceb_tu_str kodgr("");
  for(int ii=1;;ii++)
   {
    if(polen(sp_grup.ravno(),&kodgr,ii,',') != 0)
     break;

    if(kodgr.getdlinna() <= 1)
     break;      

    sprintf(strsql,"update Uosgrup set ar=1 where kod='%s'",kodgr.ravno());
    sql_zapis(strsql,0,0);
   }
 }

if(poldan("Вариант расчёта амортизации для бух. учета",&sp_grup,imaf_nast) != 0)
 {
//  iceb_t_soob("Не найден \"Вариант расчёта амортизации для бух. учета\"!");
 }
else
 {
  int varr=sp_grup.ravno_atoi();
  
  if(varr == 1) /* от остаточной*/
    sprintf(strsql,"update Uosgrup1 set ar=1");  
  if(varr == 2) /* от начальной*/
    sprintf(strsql,"update Uosgrup1 set ar=0");  

  sql_zapis(strsql,1,0);

  if(poldan("Группы бух. учёта с альтернативным вариантом расчёта",&sp_grup,imaf_nast) != 0)
   {
//    iceb_t_soob("Не найден \"Группы бух. учёта с альтернативным вариантом расчёта\"!");
   }
  if(sp_grup.getdlinna() > 1)
   {
    
    class iceb_tu_str kodgr("");
    for(int ii=1;;ii++)
     {
      if(polen(sp_grup.ravno(),&kodgr,ii,',') != 0)
       break;

      if(kodgr.getdlinna() <= 1)
       break;      

      if(varr == 1)
        sprintf(strsql,"update Uosgrup1 set ar=1 where kod='%s'",kodgr.ravno());
      if(varr == 2)
        sprintf(strsql,"update Uosgrup1 set ar=0 where kod='%s'",kodgr.ravno());

      sql_zapis(strsql,0,0);
    }    
   }
 }
}
/****************************/
/*Копирование новых настроечных файлов*/
/**************************************/

void alter_cp_alx(int metka, /*0-копировать если файла там нет 1-всегда копировать*/
const char *imaf,const char *imabaz,const char *put_na_alx,WINDOW *win1)
{
struct stat buff;

class iceb_tu_str imafout; /*откуда копируем*/
class iceb_tu_str imafin;  /*куда копируем*/
imafout.plus(put_na_alx);
imafout.plus(G_DIR_SEPARATOR_S,imaf);

imafin.plus(putnansi);
imafin.plus(G_DIR_SEPARATOR_S,imabaz);
imafin.plus(G_DIR_SEPARATOR_S,imaf);

if(SRAV(imafout.ravno(),imafin.ravno(),0) == 0) /*не нужно копировать в самого себя*/
 return; 

wprintw(win1,"Копируем файл %s > %s\n",imafout.ravno(),imafin.ravno());
wrefresh(win1);

/*проверяем есть ли этот файл*/
if(stat(imafout.ravno(),&buff) == -1) /*файл там нет*/
 {
  wprintw(win1,"Не найден файл для копирования %s\n",imafout.ravno());
  OSTANOVW(win1);
  return;
 }


/*проверяем может файл там уже есть*/
if(metka == 0)
if(stat(imafin.ravno(),&buff) == 0) /*файл там уже есть*/
 {
  wprintw(win1,"Файл %s там уже есть!\n",imafin.ravno());
  wrefresh(win1);
  return;
 }

iceb_t_cp(imafout.ravno(),imafin.ravno(),0);
}


/*************************************************/
/*Добавить строку настройки в существующий файл*/
/*************************************************/
void alter_cat_str(const char *stroka,const char *param,const char *imaf,WINDOW *win1)
{
FILE *ff;
struct stat buff;
class iceb_tu_str imaf_nsi(iceb_t_imafnsi(imaf));
wprintw(win1,"Вставляем строку \"%s%s\"\nв файл %s\n",stroka,param,imaf_nsi.ravno());
wrefresh(win1);

/*проверяем есть ли этот файл*/
if(stat(imaf_nsi.ravno(),&buff) == -1) /*файл там нет*/
 {
  wprintw(win1,"Не найден файл для вставки строки %s\n",imaf_nsi.ravno());
  OSTANOVW(win1);
  return;
 }
class iceb_tu_str nast;
if(poldan(stroka,&nast,iceb_t_imafnsi(imaf)) == 0)
 {
  wprintw(win1,"Эта строка там уже есть!\n");
  wrefresh(win1);
  return;
 } 

if((ff = fopen(imaf_nsi.ravno(),"a")) == NULL)
 {
  error_op_nfil(imaf_nsi.ravno(),errno,"");
  return;
 }
fprintf(ff,"%s%s\n",stroka,param);
fclose(ff);

}


/************************************/
/*Вставить записи в таблицу icebuser*/
/************************************/
void alter_icebuser(const char *imabaz,WINDOW *win1)
{
char strsql[512];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
struct  passwd  *ktor; /*Кто работает*/
class iceb_tu_str log_op;
ktor=getpwuid(getuid());
log_op.plus(ktor->pw_name);

sprintf(strsql,"select distinct User from mysql.db where Db='%s'",imabaz);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,win1);
  return;
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"),win1);
  return;
 }
class iceb_tu_str slsd("");
poldan("Список логинов доступ в программу которым разрешен",&slsd,iceb_t_imafnsi("zarnast.alx"));

int zpd=0;
int metka_zap=0;
while(cur.read_cursor(&row) != 0)
 {
  metka_zap=0;
  if((ktor=getpwnam(row[0])) == NULL) /*узнаём реквизиты оператора*/
   {
    wprintw(win1,"Неизвестный логин-%s\n\n",row[0]);
    OSTANOVW(win1);        
    metka_zap=1;
   }
  zpd=0; 

  if(slsd.getdlinna() > 1)
   {
    if(proverka(slsd.ravno(),row[0],0,0) == 0)
     zpd=0;
    else
     zpd=1;
   }

  if(metka_zap == 0)
    sprintf(strsql,"insert into icebuser (login,fio,un,logz,vrem,zpd) values('%s','%s',%d,'%s',%ld,%d)",
    row[0],ktor->pw_gecos,ktor->pw_uid,log_op.ravno(),time(NULL),zpd);
  else
   {
    int nomer=0;
    /*Узнаём свободный номер оператора*/
    for(nomer=1;;nomer++)
     {
      sprintf(strsql,"select un from %s.icebuser where un=%d",imabaz,nomer);
      if(readkey(strsql) == 0)
       break;
     }

    sprintf(strsql,"insert into icebuser (login,fio,un,logz,vrem,zpd) values('%s','%s',%d,'%s',%ld,%d)",
    row[0],"",nomer,log_op.ravno(),time(NULL),zpd);
   }  
  sql_zapis(strsql,1,0,win1);
    
 }
 
}


/***************************/
/*Загрузка файлов блокировок*/
/****************************/
void admin_alter_r_zb(const char *imaf,int kod,int metka)
{
FILE *ff;
char strsql[512];
char stroka[1024];
class iceb_tu_str god;
class iceb_tu_str mes;
class iceb_tu_str shet;
class iceb_tu_str login;

if((ff = fopen(imaf,"r")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

while(fgets(stroka,sizeof(stroka),ff) != NULL)
 {
  if(stroka[0] == '#')
   continue;

  shet.new_plus("");
  login.new_plus("");

  if(polen(stroka,&mes,1,'.') != 0)
   continue;

  polen(stroka,&god,2,'.');
  if(god.ravno_atoi() == 0)
   continue;

  if(metka == 0)
   {
    polen(stroka,&login,2,'|');
   }
  else
   {
    polen(stroka,&shet,2,'|');
    polen(stroka,&login,3,'|');
   }
  sprintf(strsql,"insert into Blok values(%d,%d,%d,'%s','%s',%d,%ld)",
  kod,god.ravno_atoi(),mes.ravno_atoi(),login.ravno(),shet.ravno(),iceb_t_getuid(),time(NULL));

  sql_zapis(strsql,1,1);

 }
fclose(ff); 

}
/**********************************/
/*запуск загрузки файлов блокировок*/
/*************************************/
void admin_alter_r_zzb(const char *imabazp)
{

class iceb_tu_str imaf;

imaf.new_plus(putnansi);
imaf.plus(G_DIR_SEPARATOR_S,imabazp);
imaf.plus(G_DIR_SEPARATOR_S,"blok.alx");
admin_alter_r_zb(imaf.ravno(),1,0);
unlink(imaf.ravno());

imaf.new_plus(putnansi);
imaf.plus(G_DIR_SEPARATOR_S,imabazp);
imaf.plus(G_DIR_SEPARATOR_S,"blokshet.alx");
admin_alter_r_zb(imaf.ravno(),1,1);
unlink(imaf.ravno());

imaf.new_plus(putnansi);
imaf.plus(G_DIR_SEPARATOR_S,imabazp);
imaf.plus(G_DIR_SEPARATOR_S,"blokmak.alx");
admin_alter_r_zb(imaf.ravno(),2,0);
unlink(imaf.ravno());

imaf.new_plus(putnansi);
imaf.plus(G_DIR_SEPARATOR_S,imabazp);
imaf.plus(G_DIR_SEPARATOR_S,"blokdok.alx");
admin_alter_r_zb(imaf.ravno(),3,0);
unlink(imaf.ravno());

imaf.new_plus(putnansi);
imaf.plus(G_DIR_SEPARATOR_S,imabazp);
imaf.plus(G_DIR_SEPARATOR_S,"zarblok.alx");
admin_alter_r_zb(imaf.ravno(),4,0);
unlink(imaf.ravno());

imaf.new_plus(putnansi);
imaf.plus(G_DIR_SEPARATOR_S,imabazp);
imaf.plus(G_DIR_SEPARATOR_S,"bluos.alx");
admin_alter_r_zb(imaf.ravno(),5,0);
unlink(imaf.ravno());

imaf.new_plus(putnansi);
imaf.plus(G_DIR_SEPARATOR_S,imabazp);
imaf.plus(G_DIR_SEPARATOR_S,"uslugbl.alx");
admin_alter_r_zb(imaf.ravno(),6,0);
unlink(imaf.ravno());

imaf.new_plus(putnansi);
imaf.plus(G_DIR_SEPARATOR_S,imabazp);
imaf.plus(G_DIR_SEPARATOR_S,"kasblok.alx");
admin_alter_r_zb(imaf.ravno(),7,0);
unlink(imaf.ravno());

imaf.new_plus(putnansi);
imaf.plus(G_DIR_SEPARATOR_S,imabazp);
imaf.plus(G_DIR_SEPARATOR_S,"ukrblok.alx");
admin_alter_r_zb(imaf.ravno(),8,0);
unlink(imaf.ravno());

imaf.new_plus(putnansi);
imaf.plus(G_DIR_SEPARATOR_S,imabazp);
imaf.plus(G_DIR_SEPARATOR_S,"uplblok.alx");
admin_alter_r_zb(imaf.ravno(),9,0);
unlink(imaf.ravno());

imaf.new_plus(putnansi);
imaf.plus(G_DIR_SEPARATOR_S,imabazp);
imaf.plus(G_DIR_SEPARATOR_S,"rnn_blok.alx");
admin_alter_r_zb(imaf.ravno(),10,0);
unlink(imaf.ravno());

imaf.new_plus(putnansi);
imaf.plus(G_DIR_SEPARATOR_S,imabazp);
imaf.plus(G_DIR_SEPARATOR_S,"dovblok.alx");
admin_alter_r_zb(imaf.ravno(),11,0);
unlink(imaf.ravno());
}

/***********************************/
/*Загрузка файлов alx в базу*/
/**********************************/
void admin_alter_r_alxin(const char *imabazz,WINDOW *win1)
{
class iceb_tu_str allput(putnansi);
allput.plus(G_DIR_SEPARATOR_S,imabazz);

GDir *dirp;
if((dirp=g_dir_open(allput.ravno(),0,NULL)) == NULL)
 {
  perror(__FUNCTION__);
  class iceb_tu_str repl;
  repl.plus(__FUNCTION__);
  repl.ps_plus(gettext("Не могу открыть каталог !"));
  repl.ps_plus(allput.ravno());
  repl.ps_plus(strerror(errno));
  iceb_t_soob(repl.ravno());
  
  return;
 }
class iceb_tu_str imafil;
class iceb_tu_str rashir;
const char *imafil_read;

while((imafil_read=g_dir_read_name(dirp)) != NULL)
 {
  if(polen(imafil_read,&rashir,2,'.') != 0)
   continue;

  if(SRAV("alx",rashir.ravno(),0) != 0 && SRAV("ALX",rashir.ravno(),0) != 0)
   continue;

  if(SRAV("icebstart",imafil_read,1) == 0 || SRAV("ICEBSTART",imafil_read,1) == 0)
   continue;

  imafil.new_plus(allput.ravno());
  imafil.plus(G_DIR_SEPARATOR_S,imafil_read);
  wprintw(win1,"%s-%s\n",gettext("Загрузка файла"),imafil.ravno());
  wrefresh(win1);
  iceb_t_alxin(imafil.ravno());

  unlink(imafil.ravno());

 }
g_dir_close(dirp);
}
/*************************************/
/*Вставить новую строку настройки в файл в таблице Alx*/
/********************************************************/

void admin_alter_r_ins_str_alx(const char *imaf,const char *naststr,const char *znahnast,WINDOW *win1)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

/*Проверяем может такая настройка уже есть*/

sprintf(strsql,"select ns from Alx where fil='%s' and str like '%s|%%'",imaf,naststr);
if(readkey(strsql) > 0)
 return;

/*Узнаём номер последней строки*/
sprintf(strsql,"select ns from Alx where fil='%s' order by ns desc limit 1",imaf);
if(readkey(strsql,&row,&cur) <= 0)
 return;

wprintw(win1,"%s %s:\n%s|%s\n",gettext("Вставляем настройку в файл"),imaf,naststr,znahnast);

sprintf(strsql,"insert into Alx values('%s',%d,%d,%ld,'\n')",
imaf,
atoi(row[0])+1,
getuid(),
time(NULL));

sql_zapis(strsql,1,0);

sprintf(strsql,"insert into Alx values('%s',%d,%d,%ld,'%s|%s\n')",
imaf,
atoi(row[0])+2,
getuid(),
time(NULL),
naststr,
znahnast);

sql_zapis(strsql,1,0);

}
/********************************************/
/*Записываем номера налоговых накладных в цифровом виде*/
/********************************************************/

void admin_alter_r_nnni()
{
char strsql[512];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select nz,nnn from Reenn");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"update Reenn set nnni=%d where nz=%s",atoi(row[1]),row[0]);
  sql_zapis(strsql,1,0);
 }

}
/*****************************************/
/*загрузка количества рабочих дней и часов из файла в таблицу*/
/******************************************/
void admin_alter_r_krdh(const char *putalx,WINDOW *win1)
{
char strsql[512];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
wprintw(win1,"%s\n",gettext("Загрузка количества рабочих дней и часов в месяце в таблицу"));
wrefresh(win1);

sprintf(strsql,"select str from Alx where fil='zarkalndr.alx'");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;
 }
class iceb_tu_str data_zap("");
class iceb_tu_str krd("");
class iceb_tu_str krh("");
short m,g;
while(cur.read_cursor(&row) != 0)
 {
  if(row[0][0] == '#')
   continue;
  if(polen(row[0],&data_zap,1,'|') != 0)
   continue;     
  if(polen(row[0],&krd,2,'|') != 0)
   continue;     
  if(polen(row[0],&krh,3,'|') != 0)
   continue;     

  if(krd.ravno_atof() == 0. && krh.ravno_atof() == 0.)
   continue;

  if(rsdat1(&m,&g,data_zap.ravno()) != 0)
   continue;
  
  sprintf(strsql,"insert into Zarskrdh values('%04d-%02d-01',%.10g,%.10g,%d,%ld)",
  g,m,
  krd.ravno_atof(),
  krh.ravno_atof(),
  iceb_t_getuid(),
  time(NULL));
  
  sql_zapis(strsql,0,0);
  


 }

sprintf(strsql,"delete from Alx where fil='zarkalndr.alx'");
sql_zapis(strsql,0,0);

sprintf(strsql,"%s/zarkalndr.alx",putalx);
unlink(strsql);

}
/*****************************************/
/*загрузка справочника выходных дней*/
/******************************************/
void admin_alter_r_svd(const char *putalx,WINDOW *win1)
{
char strsql[512];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
wprintw(win1,"%s\n",gettext("Загрузка справочника выходных дней в таблицу"));
wrefresh(win1);

sprintf(strsql,"select str from Alx where fil='kalend.alx'");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;
 }

short d,m,g;
while(cur.read_cursor(&row) != 0)
 {
  if(row[0][0] == '#')
   continue;


  if(rsdat(&d,&m,&g,row[0],1) != 0)
   continue;
  
  sprintf(strsql,"insert into Zarsvd values('%04d-%02d-%02d','%s',%d,%ld)",
  g,m,d,
  "",
  iceb_t_getuid(),
  time(NULL));
  
  sql_zapis(strsql,0,0);
  


 }

sprintf(strsql,"delete from Alx where fil='kalend.alx'");
sql_zapis(strsql,0,0);

sprintf(strsql,"%s/kalend.alx",putalx);
unlink(strsql);

}
/*****************************************/
/*загрузка доплат за звание*/
/******************************************/
void admin_alter_r_sdzz(const char *putalx,WINDOW *win1)
{
char strsql[512];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
wprintw(win1,"%s\n",gettext("Загрузка справочника выходных дней в таблицу"));
wrefresh(win1);

sprintf(strsql,"select str from Alx where fil='zarzvanie.alx'");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;
 }
double sdzz=0.;
int kodzv=0;
while(cur.read_cursor(&row) != 0)
 {
  if(row[0][0] == '#')
   continue;

  if(polen(row[0],&kodzv,2,'|') != 0)  
    continue;  

  if(kodzv == 0)
   continue;

  if(polen(row[0],&sdzz,3,'|') != 0)  
    continue;  

  sprintf(strsql,"update Zvan set sdzz=%.2f where kod=%d",sdzz,kodzv);
    
  sql_zapis(strsql,0,0);
  


 }

iceb_t_poldan("Код начисления за звание",strsql,"zarzvanie.alx");


admin_alter_r_ins_str_alx("zarnast.alx","Код начисления за звание",strsql,win1);


sprintf(strsql,"delete from Alx where fil='zarzvanie.alx'");
sql_zapis(strsql,0,0);

sprintf(strsql,"%s/zarzvanie.alx",putalx);
unlink(strsql);
}
/*****************************************/
/*загрузка минимальной зарплаты и прожиточного минимума*/
/******************************************/
void admin_alter_r_mzpm(const char *putalx,WINDOW *win1)
{
char strsql[512];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
wprintw(win1,"%s\n",gettext("Загрузка минимальной зарплаты и прожиточного минимума в таблицу"));
wrefresh(win1);

sprintf(strsql,"select str from Alx where fil='zar_mz_pm.alx'");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;
 }

short d,m,g;
double min_zar=0.;
double prog_min=0.;
double max_sum=0.;

while(cur.read_cursor(&row) != 0)
 {
  if(row[0][0] == '#')
   continue;

  if(polen(row[0],strsql,sizeof(strsql),1,'|') != 0 )
   continue;

  if(rsdat(&d,&m,&g,strsql,1) != 0)
   continue;

  polen(row[0],&min_zar,2,'|');
  polen(row[0],&prog_min,3,'|');
  polen(row[0],&max_sum,4,'|');
    
  sprintf(strsql,"insert into Zarmzpm values('%04d-%02d-%02d',%.10g,%.10g,%.10g,%d,%ld)",
  g,m,d,
  min_zar,
  prog_min,
  max_sum,
  iceb_t_getuid(),
  time(NULL));
  
  sql_zapis(strsql,0,0);
  


 }

sprintf(strsql,"delete from Alx where fil='zar_mz_pm.alx'");
sql_zapis(strsql,0,0);

sprintf(strsql,"%s/zar_mz_pm.alx",putalx);
unlink(strsql);

}
/**********************************/
/*создание новых таблиц*/
/*******************************/
int admin_alter_r_crtb(const char *strsql,WINDOW *win1)
{
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_TABLE_EXISTS_ERROR) /*Таблица уже существует*/
   {
    wprintw(win1,"%s\n",gettext("Таблица уже существует"));
    wrefresh(win1);
    return(1);
   }
  else
   {
    beep();
    wprintw(win1,"%s ! %d\n%s\n%s-%s\n",gettext("Ошибка"),sql_nerror(&bd),sql_error(&bd),gettext("Запрос"),strsql);
    OSTANOVW(win1);
    return(2);
   }
 }
wprintw(win1,"%s\n",gettext("Таблица создана"));
wrefresh(win1);
return(0);
}
/***************************************/
/*вставка новой колонки в таблицу*/
/***************************************/

int admin_alter_r_vnk(const char *strsql,WINDOW *win1)
{

if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_FIELDNAME)
   {
    wprintw(win1,"%s\n",gettext("Колонкa в таблице уже существуeт"));
    wrefresh(win1);
    return(1);
   }
  else
   {
    wprintw(win1,"%s ! %d\n%s\n",gettext("Ошибка"),sql_nerror(&bd),sql_error(&bd));
    return(2);
    OSTANOVW(win1);
   }
 }

return(0);
}
/********************************/
/*загрузка регулярных удержаний в таблицу*/
/******************************************/
void admin_alter_r_zrud(const char *putalx,WINDOW *win1)
{
char strsql[512];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
wprintw(win1,"%s\n",gettext("Загрузка регулярных удержаний с работника"));
wrefresh(win1);

sprintf(strsql,"select str from Alx where fil='zarkredit.alx'");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;
 }

int tabnom=0;
int kod_ud=0;
int metka_sum=0;
class iceb_tu_str sumapr("");
class iceb_tu_str datad("");
while(cur.read_cursor(&row) != 0)
 {
  if(row[0][0] == '#')
   continue;

  if(polen(row[0],&tabnom,1,'|') != 0 )
   continue;
  if(tabnom == 0)
   continue;

  polen(row[0],&kod_ud,2,'|');
  polen(row[0],&sumapr,3,'|');
  polen(row[0],&datad,4,'|');
  metka_sum=0;
  if(pole2(sumapr.ravno(),'\%') > 0)
   metka_sum=1;
       
  sprintf(strsql,"insert into Zarrud values(%d,%d,%.2f,%d,'%s','',%d,%ld)",
  tabnom,
  kod_ud,
  sumapr.ravno_atof(),
  metka_sum,
  datad.ravno_sqldata(),
  iceb_t_getuid(),
  time(NULL));
  
  sql_zapis(strsql,0,0);
  


 }

sprintf(strsql,"delete from Alx where fil='zarkredit.alx'");
sql_zapis(strsql,0,0);

sprintf(strsql,"%s/zarkredit.alx",putalx);
unlink(strsql);

}
/********************************/
/*загрузка справочника льгот в таблицу*/
/******************************************/
void admin_alter_r_zsl(const char *putalx,WINDOW *win1)
{
char strsql[512];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
wprintw(win1,"%s\n",gettext("Загрузка справочника льгот"));
wrefresh(win1);

sprintf(strsql,"select str from Alx where fil='zarlgot.alx'");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;
 }

int tabnom=0;
class iceb_tu_str datad("");
class iceb_tu_str procl("");
class iceb_tu_str kol_lgot("");
class iceb_tu_str kod_lgot("");

while(cur.read_cursor(&row) != 0)
 {
  if(row[0][0] == '#')
   continue;

  if(polen(row[0],&tabnom,1,'|') != 0 )
   continue;
  if(tabnom == 0)
   continue;

  if(polen(row[0],&procl,2,'|') != 0)
   continue;

  polen(row[0],&datad,3,'|');
  polen(row[0],&kol_lgot,4,'|');
  polen(row[0],&kod_lgot,5,'|');

  if(kod_lgot.getdlinna() <= 1)
   kod_lgot.new_plus("01");

  if(kol_lgot.ravno_atoi() == 0)
   kol_lgot.new_plus("1");
   
  sprintf(strsql,"insert into Zarlgot values(%d,'%s',%.2f,%d,'%s','',%d,%ld)",
  tabnom,
  datad.ravno_sqldata(),
  procl.ravno_atof(),
  kol_lgot.ravno_atoi(),
  kod_lgot.ravno(),
  iceb_t_getuid(),
  time(NULL));

  
  sql_zapis(strsql,0,0);
  


 }

sprintf(strsql,"delete from Alx where fil='zarlgot.alx'");
sql_zapis(strsql,0,0);

sprintf(strsql,"%s/zarlgot.alx",putalx);
unlink(strsql);

}

/********************************/
/*загрузка справочника пенсионеров инвалидов и работающих по договорам подряда*/
/******************************************/
void admin_alter_r_zargr1(int metka_gr,const char *imaf,const char *putalx,WINDOW *win1)
{
char strsql[1024];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select str from Alx where fil='%s'",imaf);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
//  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;
 }

int tabnom=0;
class iceb_tu_str dnd("");
class iceb_tu_str dkd("");

while(cur.read_cursor(&row) != 0)
 {
  if(row[0][0] == '#')
   continue;

  tabnom=atoi(row[0]);
  if(tabnom == 0)
   continue;

  polen(row[0],&dnd,2,'|');
  polen(row[0],&dkd,3,'|');

  sprintf(strsql,"insert into Zargr values(%d,%d,'%s','%s','',%d,%ld)",
  metka_gr,
  tabnom,
  dnd.ravno_sqldata(),
  dkd.ravno_sqldata(),
  iceb_t_getuid(),
  time(NULL));

  
  sql_zapis(strsql,0,0);
  


 }

sprintf(strsql,"delete from Alx where fil='%s'",imaf);
sql_zapis(strsql,0,0);

sprintf(strsql,"%s/%s",putalx,imaf);
unlink(strsql);

}
/************************************************/
void admin_alter_r_zargr(const char *putalx,WINDOW *win1)
{
wprintw(win1,"%s\n",gettext("Загрузка справочника пенсионеров"));
wrefresh(win1);

admin_alter_r_zargr1(1,"zarpens.alx",putalx,win1);

wprintw(win1,"%s\n",gettext("Загрузка справочника инвалидов"));
wrefresh(win1);

admin_alter_r_zargr1(2,"zarinv.alx",putalx,win1);


wprintw(win1,"%s\n",gettext("Загрузка справочника работающих по договорам подряда"));
wrefresh(win1);

admin_alter_r_zargr1(3,"zardog.alx",putalx,win1);


}
/*******************************************/
/*чтение настроек на операцию в мат-учёте*/
/********************************************/

void admin_alter_r_murashod()
{
char strsql[512];
class iceb_tu_str kontr("");
SQL_str row;
class SQLCURSOR cur;
sprintf(strsql,"select kod from Rashod");
if(cur.make_cursor(&bd,strsql) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  if(iceb_t_poldan(row[0],&kontr,"matnast.alx") != 0)
   continue;   

  sprintf(strsql,"update Rashod set kk='%s' where kod='%s'",kontr.ravno_filtr(),row[0]);
  sql_zapis(strsql,1,0);

  sprintf(strsql,"delete from Alx where fil='matnast.alx' and str like '%s|%%'",row[0]);
  sql_zapis(strsql,1,0);
 }

}
/*******************************************/
/*чтение настроек на операцию в учёте услуг*/
/********************************************/

void admin_alter_r_uslrashod()
{
char strsql[512];
class iceb_tu_str kontr("");
SQL_str row;
class SQLCURSOR cur;
sprintf(strsql,"select kod from Usloper2");
if(cur.make_cursor(&bd,strsql) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  if(iceb_t_poldan(row[0],&kontr,"uslnast.alx") != 0)
   continue;   

  sprintf(strsql,"update Usloper2 set kk='%s' where kod='%s'",kontr.ravno_filtr(),row[0]);
  sql_zapis(strsql,1,0);

  sprintf(strsql,"delete from Alx where fil='ulsnast.alx' and str like '%s|%%'",row[0]);
  sql_zapis(strsql,1,0);
 }

}
/**********************************************************/
/***********************************************************/
void admin_alter_r_vnst0(WINDOW *win1)
{
char strsql[1024];

sprintf(strsql,"insert into Uplnst values('%s','%s',%d,%ld)",gettext("пг"),gettext("Норма по городу"),iceb_t_getuid(),time(NULL));
if(sql_zap(&bd,strsql) != 0)
 {
  wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
  OSTANOVW(win1);
 }
sprintf(strsql,"insert into Uplnst values('%s','%s',%d,%ld)",gettext("зг"),gettext("Норма за городом"),iceb_t_getuid(),time(NULL));
if(sql_zap(&bd,strsql) != 0)
 {
  wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
  OSTANOVW(win1);
 }

sprintf(strsql,"insert into Uplnst values('%s','%s',%d,%ld)",gettext("лпг"),gettext("Летняя норма по городу"),iceb_t_getuid(),time(NULL));
if(sql_zap(&bd,strsql) != 0)
 {
  wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
  OSTANOVW(win1);
 }

sprintf(strsql,"insert into Uplnst values('%s','%s',%d,%ld)",gettext("лзг"),gettext("Летняя норма за городом"),iceb_t_getuid(),time(NULL));
if(sql_zap(&bd,strsql) != 0)
 {
  wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
  OSTANOVW(win1);
 }

sprintf(strsql,"insert into Uplnst values('%s','%s',%d,%ld)",gettext("зпг"),gettext("Зимняя норма по городу"),iceb_t_getuid(),time(NULL));
if(sql_zap(&bd,strsql) != 0)
 {
  wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
  OSTANOVW(win1);
 }

sprintf(strsql,"insert into Uplnst values('%s','%s',%d,%ld)",gettext("ззг"),gettext("Зимняя норма за городом"),iceb_t_getuid(),time(NULL));
if(sql_zap(&bd,strsql) != 0)
 {
  wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
  OSTANOVW(win1);
 }

sprintf(strsql,"insert into Uplnst values('%s','%s',%d,%ld)",gettext("пегр"),gettext("Перевозка груза"),iceb_t_getuid(),time(NULL));
if(sql_zap(&bd,strsql) != 0)
 {
  wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
  OSTANOVW(win1);
 }
sprintf(strsql,"insert into Uplnst values('%s','%s',%d,%ld)",gettext("рд"),gettext("Работа двигателя"),iceb_t_getuid(),time(NULL));
if(sql_zap(&bd,strsql) != 0)
 {
  wprintw(win1,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
  OSTANOVW(win1);
 }


}





/*********************************************/
/*вставка норм списания топлива в новую таблицу*/
/***********************************************/
void admin_alter_r_vnst(WINDOW *win1)
{

char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
int kolstr=0;


/*вставляем нужные единицы измерения*/
sprintf(strsql,"select kod from Edizmer where kod='%s'",gettext("Км"));
if(readkey(strsql,win1) != 1)
 {
  sprintf(strsql,"insert into Edizmer values('%s','%s',%d,%ld)",gettext("Км"),gettext("Киллометры"),iceb_t_getuid(),time(NULL));
  sql_zapis(strsql,1,0,win1);
 }

sprintf(strsql,"select kod from Edizmer where kod='%s'",gettext("Ткм"));
if(readkey(strsql,win1) != 1)
 {
  sprintf(strsql,"insert into Edizmer values('%s','%s',%d,%ld)",gettext("Ткм"),gettext("Тонно-киллометры"),iceb_t_getuid(),time(NULL));
  sql_zapis(strsql,1,0,win1);
 }

sprintf(strsql,"select kod from Edizmer where kod='%s'",gettext("Мч"));
if(readkey(strsql,win1) != 1)
 {
  sprintf(strsql,"insert into Edizmer values('%s','%s',%d,%ld)",gettext("Мч"),gettext("Моточасы"),iceb_t_getuid(),time(NULL));
  sql_zapis(strsql,1,0,win1);
 }

sprintf(strsql,"select kod,lnr,znr,lnrzg,znrzg,nrtk,nrmh from Uplavt");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,win1);
  return;
 }

if(kolstr == 0)
 {
  return;
 }

/***********
лпг
зпг
лзг
ззг
пeгр
рд
*************/

while(cur.read_cursor(&row) != 0)
 {
  if(atof(row[1]) != 0.)
   {
    sprintf(strsql,"insert into Uplavt1 values(%s,'%s',%.3f,'%s',%d,%ld)",row[0],gettext("лпг"),atof(row[1]),gettext("Км"),iceb_t_getuid(),time(NULL));
    sql_zapis(strsql,1,0,win1);
   }
  if(atof(row[2]) != 0.)
   {
    sprintf(strsql,"insert into Uplavt1 values(%s,'%s',%.3f,'%s',%d,%ld)",row[0],gettext("зпг"),atof(row[2]),gettext("Км"),iceb_t_getuid(),time(NULL));
    sql_zapis(strsql,1,0,win1);
   }
  if(atof(row[3]) != 0.)
   {
    sprintf(strsql,"insert into Uplavt1 values(%s,'%s',%.3f,'%s',%d,%ld)",row[0],gettext("лзг"),atof(row[3]),gettext("Км"),iceb_t_getuid(),time(NULL));
    sql_zapis(strsql,1,0,win1);
   }
  if(atof(row[4]) != 0.)
   {
    sprintf(strsql,"insert into Uplavt1 values(%s,'%s',%.3f,'%s',%d,%ld)",row[0],gettext("ззг"),atof(row[4]),gettext("Км"),iceb_t_getuid(),time(NULL));
    sql_zapis(strsql,1,0,win1);
   }
  if(atof(row[5]) != 0.)
   {
    sprintf(strsql,"insert into Uplavt1 values(%s,'%s',%.3f,'%s',%d,%ld)",row[0],gettext("пегр"),atof(row[5]),gettext("Ткм"),iceb_t_getuid(),time(NULL));
    sql_zapis(strsql,1,0,win1);
   }
  if(atof(row[6]) != 0.)
   {
    sprintf(strsql,"insert into Uplavt1 values(%s,'%s',%.3f,'%s',%d,%ld)",row[0],gettext("рд"),atof(row[6])*100.,gettext("Мч"),iceb_t_getuid(),time(NULL));
    sql_zapis(strsql,1,0,win1);
   }


 }


}
/***************************************************/
/*переписать в таблицу нормы которые были в шапке путевого листа*/
/**************************************************/

void admin_alter_r_vnstpl(WINDOW *win1)
{

char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
int kolstr=0;

sprintf(strsql,"select datd,kp,nomd,ka,ztpn,ztfa,prob,nst,ztpnzg,ztfazg,probzg,nstzg,vesg,ztvsn,ztvsf,vrr,ztvrn,ztvrf,tk from Upldok");
/*                      0   1   2   3   4    5    6    7    8      9     10     11    12   13    14   15   16    17   18 */
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,win1);
  return;
 }
if(kolstr == 0)
  return;
while(cur.read_cursor(&row) != 0)
 {
  /*так как в шапке не указано летняя или зимняя норма то всё записываем на летнюю*/
  
  if(atof(row[4]) != 0. || atof(row[5]) != 0. || atof(row[6]) != 0.)
   {
    sprintf(strsql,"insert into Upldok3 values('%s',%s,'%s','%s',%.3f,%.3f,%.3f,%.3f,'%s','',%d,%ld)",
    row[0],row[1],row[2],
    gettext("пг"),
    atof(row[6]),
    atof(row[7]),
    atof(row[4]),
    atof(row[5]),
    gettext("Км"),
    iceb_t_getuid(),
    time(NULL));    

    sql_zapis(strsql,1,0,win1);
   }

  if(atof(row[8]) != 0. || atof(row[9]) != 0. || atof(row[10]) != 0.)
   {
    sprintf(strsql,"insert into Upldok3 values('%s',%s,'%s','%s',%.3f,%.3f,%.3f,%.3f,'%s','',%d,%ld)",
    row[0],row[1],row[2],
    gettext("зг"),
    atof(row[10]),
    atof(row[11]),
    atof(row[8]),
    atof(row[9]),
    gettext("Км"),
    iceb_t_getuid(),
    time(NULL));    

    sql_zapis(strsql,1,0,win1);
   }

  if(atof(row[18]) != 0. || atof(row[13]) != 0. || atof(row[14]) != 0.)
   {
    sprintf(strsql,"insert into Upldok3 values('%s',%s,'%s','%s',%.3f,%.3f,%.3f,%.3f,'%s','',%d,%ld)",
    row[0],row[1],row[2],
    gettext("пегр"),
    atof(row[18]),
    atof(row[13]),
    atof(row[14]),
    atof(row[14]),
    gettext("Ткм"),
    iceb_t_getuid(),
    time(NULL));    

    sql_zapis(strsql,1,0,win1);
   }

  if(atof(row[15]) != 0. || atof(row[16]) != 0. || atof(row[17]) != 0.)
   {
    char ves[512];
    memset(ves,'\0',sizeof(ves));
    if(atof(row[12]) != 0.)
      sprintf(ves,"%s:%s",gettext("Вес"),row[12]);

    sprintf(strsql,"insert into Upldok3 values('%s',%s,'%s','%s',%.3f,%.3f,%.3f,%.3f,'%s','%s',%d,%ld)",
    row[0],row[1],row[2],
    gettext("рд"),
    atof(row[15]),
    atof(row[16]),
    atof(row[17]),
    atof(row[17]),
    gettext("Мч"),
    ves,        
    iceb_t_getuid(),
    time(NULL));    

    sql_zapis(strsql,1,0,win1);
   }
 }

}
/********************************/
/*заполнение новой таблицы списания топлива по счетам*/
/******************************************************/
void admin_alter_r_upldok2a(WINDOW *win1)
{
char strsql[1024];
SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kolstr=0;
int kolstr1=0;
int nomer=0;
class iceb_tu_str kod_top;
class iceb_tu_str shet;
double zatr_top=0.;

//sprintf(strsql,"select datd,kp,nomd from Upldok");
sprintf(strsql,"select * from Upldok");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,win1);
  return;
 }
if(kolstr == 0)
  return;

while(cur.read_cursor(&row) != 0)
 {
  class SPISOK kodtop_shet;
  class masiv_din_double kolih_top;
  class masiv_din_double zatpn; //*Затраты по норме
  
//  sprintf(strsql,"select kt,shet,zt,ztzg,ztgr,ztmh from Upldok2 where datd='%s' and kp=%s and nomd='%s'",row[0],row[1],row[2]);
  sprintf(strsql,"select * from Upldok2 where datd='%s' and kp=%s and nomd='%s'",row[0],row[1],row[2]);

  if((kolstr1=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,win1);
    return;
   }
  if(kolstr1 == 0)
    continue;
  double zatr_pn=0.;
  while(cur1.read_cursor(&row1) != 0)
   {
    sprintf(strsql,"%s|%s",row1[3],row1[4]);
    if((nomer=kodtop_shet.find(strsql)) < 0)
     kodtop_shet.plus(strsql);

    zatr_top=atof(row1[7])+atof(row1[14])+atof(row1[15])+atof(row1[16]);
    zatr_pn=atof(row1[6])*atof(row[19])/100.+atof(row1[13])*atof(row[25])/100.+atof(row1[17])*atof(row[27])/100.+atof(row1[18])*atof(row[30]);

    kolih_top.plus(zatr_top,nomer);
    zatpn.plus(zatr_pn,nomer);
   }  
  for(nomer=0; nomer < kodtop_shet.kolih(); nomer++)
   {  
    polen(kodtop_shet.ravno(nomer),&kod_top,1,'|');
    polen(kodtop_shet.ravno(nomer),&shet,2,'|');
     
    sprintf(strsql,"insert into Upldok2a values('%s',%s,'%s','%s','%s',%.3f,%.3f,'',%d,%ld)",
    row[0],row[1],row[2],
    kod_top.ravno_filtr(),
    shet.ravno_filtr(),
    kolih_top.ravno(nomer),
    zatpn.ravno(nomer),
    iceb_t_getuid(),
    time(NULL));

    sql_zapis(strsql,1,0,win1);   

   }  
 }
}
/**************************************/

void imafn(const char *imaf,char *imaf_p)
{
sprintf(imaf_p,"%s%s%s%s%s",putnansi,G_DIR_SEPARATOR_S,imabaz,G_DIR_SEPARATOR_S,imaf);
}
void imafn(const char *imaf,class iceb_tu_str *imaf_p)
{
imaf_p->new_plus(putnansi);
imaf_p->plus(G_DIR_SEPARATOR_S,imabaz);
imaf_p->plus(G_DIR_SEPARATOR_S,imaf);
}
/******************************/
const char *iceb_t_imafnsi(const char *imaf)
{
static class iceb_tu_str full_name_nsi_fil;
imafn(imaf,&full_name_nsi_fil);
return(full_name_nsi_fil.ravno());

}

/*********************************/
/*преобразование таблиц амортизации*/
/**************************************/
int poiin(int in,short d,short m,short g, int *podr,int *kodotl);

int admin_alter_r_amort(const char *tabl,WINDOW *win1)
{

char strsql[1024];
SQL_str row;
class SQLCURSOR cur;

class iceb_t_tmptab tabtmp;
const char *imatmptab={"amort"};
char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
innom   int not null default 0,\
da DATE NOT NULL DEFAULT '0000-00-00',\
podr	smallint not null default 0,\
hzt	varchar(20) not null default '',\
hna	varchar(20) not null default '',\
suma	double (14,2) not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
kmo INT NOT NULL DEFAULT 0,\
unique(innom,da)) ENGINE = MYISAM",imatmptab);

/****************
sprintf(zaprostmp,"CREATE TABLE %s (\
innom   int not null default 0,\
da DATE NOT NULL DEFAULT '0000-00-00',\
podr	smallint not null default 0,\
hzt	varchar(20) not null default '',\
hna	varchar(20) not null default '',\
suma	double (14,2) not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
kmo INT NOT NULL DEFAULT 0,\
unique(innom,da)) ENGINE = MYISAM",imatmptab);
sql_zapis(zaprostmp,1,0);
*******************/

if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  
sprintf(strsql,"select * from %s order by innom asc",tabl);
//wprintw(win1,"%s\n",strsql);

if(cur.make_cursor(&bd,strsql) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
 
int innom=0;
short d=1,m=0,g=0;
int podr=0;
int kodmo=0;
while(cur.read_cursor(&row) != 0)
 {
  innom=atoi(row[0]);    

  m=atoi(row[1]);
  g=atoi(row[2]);
    
  poiin(innom,d,m,g,&podr,&kodmo);
  podr=atoi(row[3]); /*на всякий случай оставляем подразделение как было*/

  sprintf(strsql,"insert into %s values (%d,'%04d-%02d-%02d',%d,'%s','%s',%s,%s,%s,%d)",imatmptab,innom,g,m,d,podr,row[4],row[5],row[6],row[7],row[8],kodmo);
  sql_zapis(strsql,1,0);
//  OSTANOVW(win1);
 }

sprintf(strsql,"drop table %s",tabl);
sql_zapis(strsql,1,0);



sprintf(zaprostmp,"CREATE TABLE %s (\
innom   int not null default 0,\
da DATE NOT NULL DEFAULT '0000-00-00',\
podr	smallint not null default 0,\
hzt	varchar(20) not null default '',\
hna	varchar(20) not null default '',\
suma	double (14,2) not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
kmo INT NOT NULL DEFAULT 0,\
unique(innom,da)) ENGINE = MYISAM",tabl);

sql_zapis(zaprostmp,1,0);

sprintf(strsql,"INSERT INTO %s SELECT * FROM %s",tabl,imatmptab);

sql_zapis(strsql,1,0);

return(0);
}
/***************************************/
/*замена приставки к табельному номеру*/
/*************************************/
void admin_alter_r_zpktn(WINDOW *win1)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
class iceb_tu_str pris_k_tn("");
int kolstr=0;

if(iceb_t_poldan("Приставка к табельному номеру",&pris_k_tn,"zarnast.alx") != 0)
  return;

wprintw(win1,"Приставка к табельному номеру=%s\n",pris_k_tn.ravno());
wrefresh(win1);
if(SRAV(ICEB_ZAR_PKTN,pris_k_tn.ravno(),0) == 0)
 return;

wprintw(win1,"%s %s на %s\n",gettext("Меняем приставку к табельному номеру"),pris_k_tn.ravno(),ICEB_ZAR_PKTN);
 
sprintf(strsql,"select kodkon from Kontragent where kodkon like '%s%%'",pris_k_tn.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql,win1);
  return;
 }

class iceb_tu_str kod_kontr_new("");

while(cur.read_cursor(&row) != 0)
 {
  wprintw(win1,"%s -> ",row[0]);

  sprintf(strsql,"%s%s",ICEB_ZAR_PKTN,iceb_tu_adrsimv(iceb_tu_strlen(pris_k_tn.ravno()),row[0]));
 
  wprintw(win1,"%s\n",strsql);
  wrefresh(win1);
  
  kod_kontr_new.new_plus(strsql);
     
  sprintf(strsql,"update Kontragent \
set \
kodkon='%s',\
ktoz=%d,\
vrem=%ld \
where kodkon='%s'",
  kod_kontr_new.ravno_filtr(),
  iceb_t_getuid(),time(NULL),
  row[0]);

  sql_zapis(strsql,1,0);

  sprintf(strsql,"update Skontr \
set \
kodkon='%s',\
ktoi=%d,\
vrem=%ld \
where kodkon='%s'",
  kod_kontr_new.ravno_filtr(),
  iceb_t_getuid(),time(NULL),
  row[0]);

  sql_zapis(strsql,1,0);
  smenkodk(row[0],kod_kontr_new.ravno());
  
 }

/*есть проводки в которых есть код контрагента и его нет в списке контрагентов*/

sprintf(strsql,"select distinct kodkon from Prov where kodkon like '%s%%'",pris_k_tn.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql,win1);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {

  if(SRAV("тн",row[0],1) == 0)
   continue;
   
  wprintw(win1,"%s -> ",row[0]);

  sprintf(strsql,"%s%s",ICEB_ZAR_PKTN,iceb_tu_adrsimv(iceb_tu_strlen(pris_k_tn.ravno()),row[0]));
 
  wprintw(win1,"%s\n",strsql);
  wrefresh(win1);
  
  kod_kontr_new.new_plus(strsql);
  
  sprintf(strsql,"update Prov set kodkon='%s' where kodkon='%s'",kod_kontr_new.ravno(),row[0]);
  sql_zapis(strsql,1,0);
 }

}
/*****************************************/
/*смена кода контрагента в проводках может быть в проводке и не быть в общем списке*/
/****************************************/
void admin_alter_r_skkvp(WINDOW *win1)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
class iceb_tu_str pris_k_tn("ТН");
int kolstr=0;

sprintf(strsql,"select distinct kodkon from Prov where kodkon like 'ТН%%'");
class iceb_tu_str kod_kontr_new("");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql,win1);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {

  if(SRAV("тн",row[0],1) == 0)
   continue;
   
  wprintw(win1,"%s -> ",row[0]);

  sprintf(strsql,"%s%s",ICEB_ZAR_PKTN,iceb_tu_adrsimv(iceb_tu_strlen(pris_k_tn.ravno()),row[0]));
 
  wprintw(win1,"%s\n",strsql);
  wrefresh(win1);
  
  kod_kontr_new.new_plus(strsql);
  
  sprintf(strsql,"update Prov set kodkon='%s' where kodkon='%s'",kod_kontr_new.ravno(),row[0]);
  sql_zapis(strsql,1,0);
 }

}
/********************************/
/*загрузка таблицы должностных окладов*/
/*********************************/
void admin_alter_r_zdo()
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
class iceb_tu_str kod_do(""); /*код дожностного оклада*/
class iceb_tu_str tabnom("");
class iceb_tu_str oklad("");
class iceb_tu_str hasi("");
class iceb_tu_str koef("");
class iceb_tu_str shet("");
class iceb_tu_str bros("");
int metka_sp=0;

short mt=1,gt=2016;



sprintf(strsql,"select str from Alx where fil='zardolhn.alx' order by ns asc");




if(cur.make_cursor(&bd,strsql) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  metka_sp=1; /*месячный расчёт по часам*/
  
  if(SRAV("Код должносного оклада|",row[0],1) == 0)
   {
    polen(row[0],&kod_do,2,'|');
   }

  if(polen(row[0],&tabnom,1,'|') != 0)
   continue;

  if(tabnom.ravno_atoi() == 0)
   continue;  

  polen(row[0],&oklad,2,'|');
  
  if(oklad.ravno_atof() == 0.)
   continue;

 if(strstrm(oklad.ravno(),"Ч") == 1)
   metka_sp=3; /*почасовая ставка*/
 if(strstrm(oklad.ravno(),"Ч") == 1)
   metka_sp=4; /*дневная ставка*/
 if(strstrm(oklad.ravno(),"r") == 1)
   metka_sp=5; /*тариф*/

  polen(row[0],&bros,4,'|');
  if(strstrm(bros.ravno(),"Д") == 1)
   metka_sp=0; /*месячный по дням*/
   
  polen(row[0],&shet,5,'|');
  
  polen(row[0],&bros,3,'|');
  
  if(bros.ravno()[0] == '*')
   metka_sp=2;  /*месячный без расчёта*/
  else  
   if(strstrm(bros.ravno(),"*") == 1)
    {
     polen(bros.ravno(),&hasi,1,'*');
     polen(bros.ravno(),&koef,2,'*');
    } 
   else
    {
     hasi.new_plus(bros.ravno());
     koef.new_plus("1.");
    }

  sprintf(strsql,"insert into Zarsdo values(%d,'%04d-%02d-01',%d,%.2f,%d,%.2f,%.2f,'%s','%s',%d,%ld)",
  tabnom.ravno_atoi(),
  gt,mt,
  kod_do.ravno_atoi(),
  oklad.ravno_atof(),
  metka_sp,
  hasi.ravno_atof(),
  koef.ravno_atof(),
  shet.ravno_filtr(),
  "",
  iceb_t_getuid(),
  time(NULL));

  sql_zapis(strsql,1,0);
 }  




}
/**********************************/
/*установка даты*/
/*************************************/
void admin_alter_r_ustdat(WINDOW *win1)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
short d=0,m=0,g=0;
sprintf(strsql,"select god,datd,nomd from Zardok");




if(cur.make_cursor(&bd,strsql) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  wprintw(win1,"%s %s %s\n",row[0],row[1],row[2]);
  wrefresh(win1);
  rsdat(&d,&m,&g,row[1],2);
  sprintf(strsql,"update Zardok set datsd='%s',god=%d where god='%s' and nomd='%s'",row[1],g,row[0],row[2]);
  
  sql_zapis(strsql,1,0);
 }
}
