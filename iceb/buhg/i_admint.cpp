/*$Id: i_admint.c,v 5.49 2014/08/31 06:18:18 sasa Exp $*/
/*08.06.2017	18.09.2009	Белых А.И.	i_admint.c
 Администриврование баз 
*/
#define DVERSIQ "27.04.2018"
#include <pwd.h>
#include <errno.h>
#include        <locale.h>
#include "buhg.h"

void  i_admint_rb();
void crtabl(const char *imabaz);
void i_admint_sd(const char *host);
int admin_cp_alx(const char *from_alx,const char *to_alx);
void admin_alter();
void admin_newuser(const char *imabazr,const char *user);
void i_admin_infosys(int y,int x,const char *host,const char *verss,const char *dvers,short ddd,short mmm,short ggg,const char *imabaz,const char *nameprinter);
int i_admint_pb(const char *imapb);
void admin_kodkl();
void admin_klaviat();
void i_admint_password(const char *host);
void i_admint_user_del(const char *host);

int dirduser();
int dirbaz(const char *login,const char *parol,const char *hostname);
SQL_baza	bd;
char *putnansi=NULL;
extern int iceb_kod_podsystem;
extern int metka_utf8;
extern class iceb_tu_str kodirovka_iceb;
  
int main(int argc,char **argv,char **envp)
{
iceb_kod_podsystem=ICEB_PS_ABD;
char strsql[512];
int kom=0;
class iceb_tu_str login("root");
char rootparol[64];
class iceb_tu_str host("localhost");
class iceb_tu_str locale;
memset(rootparol,'\0',sizeof(rootparol));
metka_utf8=0;

setlocale(LC_ALL,"");/*Устанавливает переменные определяющие локаль окружения*/

locale.plus(setlocale(LC_MESSAGES,""));

if(polen(locale.ravno(),&kodirovka_iceb,2,'.') == 0)
 {
  if(strstrm(kodirovka_iceb.ravno(),"utf") == 1)
      metka_utf8=1;
 }
else
 {
  if(SRAV(locale.ravno(),"ru",0) == 0)
   kodirovka_iceb.new_plus("koi8r");
  if(SRAV(locale.ravno(),"ru_UA",0) == 0)
   kodirovka_iceb.new_plus("koi8u");
  if(SRAV(locale.ravno(),"uk_UA",0) == 0)
   kodirovka_iceb.new_plus("koi8u");
 }


(void)setlocale(LC_NUMERIC,"C"); //Переменная среды определяющая чем 
                                 //разделяется целая и дробная часть числа
iceb_t_locale(); /*Включаю локализацию*/
init_screen();

const char *nsi={"/etc/iceB"};
/*Определяем путь на файлы настройки*/
if((putnansi=getenv("PUTNANSI")) == NULL)
 {
  int i=strlen(nsi)+1;
  if((putnansi=(char *)malloc(i*sizeof(char))) == NULL)
    iceb_t_soob("Не могу выделить пямять для пути !");
  else
    strcpy(putnansi,nsi);
 }  




VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Администрирование баз данных"));

sprintf(strsql,"%s %s %s",gettext("Версия"),gettext("от"),DVERSIQ);
VV.VVOD_SPISOK_add_ZAG(strsql);

VV.VVOD_SPISOK_add_MD(gettext("Введите логин................"));
VV.VVOD_SPISOK_add_MD(gettext("Хост на базу................."));
VV.VVOD_SPISOK_add_MD(gettext("Кодировка запросов к базе...."));

VV.VVOD_SPISOK_add_data(login.ravno(),64);
VV.VVOD_SPISOK_add_data(host.ravno(),128);
VV.VVOD_SPISOK_add_data(kodirovka_iceb.ravno_udsimv("-"),20);

naz:;

clear();
 
helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+","ввод",
"F4","очистить",
"F10","выход",NULL);

kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

login.new_plus(VV.VVOD_SPISOK_return_data(0));
host.new_plus(VV.VVOD_SPISOK_return_data(1));
kodirovka_iceb.new_plus(VV.VVOD_SPISOK_return_data(2));


switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   prosf(strsql);
   goto naz;

  case ESC:
  case FK10:
    refresh();
    endwin();
    return(0);

  case FK2:
  case PLU:
     break;


  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

VVOD PAROL(2);
PAROL.VVOD_SPISOK_add_ZAG(gettext("Введите пароль для доступа к базе данных mysql"));
if(vparol(&PAROL,rootparol,20,stdscr,-1,-1,1) != 0)
 goto naz;





if(mysql_init(&bd) == NULL)
 {
  msql_error(&bd,"Ошибка инициализации базы данных !","");
  endwin();
  exit(10);
 }

//printw("Связываемся с демоном базы.\n");
/*                                                   порт */
if(mysql_real_connect(&bd,host.ravno(),login.ravno(),rootparol,"mysql",0,NULL,0) == NULL)
/*                                                   именованій порт*/
 { 
  msql_error(&bd,"Ошибка установки связи с демоном базы !","");
  memset(rootparol,'\0',sizeof(rootparol));
  goto naz;
 } 

icebstart_rf();

//sprintf(strsql,"SET NAMES %s",kzkb.ravno());
//sql_zap(&bd,strsql);

SQL_str row;
class SQLCURSOR cur;
class iceb_tu_str version("");
if(readkey("select VERSION()",&row,&cur) > 0)
 version.new_plus(row[0]);


VVOD MENU(4);

for(;;)
 {
  clear();

  MENU.VVOD_delete();

  MENU.VVOD_SPISOK_add_ZAG(gettext("Администрирование баз данных"));
  sprintf(strsql,"%s MySQL:%s %s/%d",gettext("Версия"),version.ravno(),kodirovka_iceb.ravno(),metka_utf8);
  MENU.VVOD_SPISOK_add_ZAG(strsql);
   
  MENU.VVOD_SPISOK_add_MD(gettext("Разметить новую базу данных для iceB")); //0
  MENU.VVOD_SPISOK_add_MD(gettext("Разрешить доступ к базе данных"));     //1
  MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка разрешённых доступов к базе данных"));//2
  MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка существующих баз"));   //3
  MENU.VVOD_SPISOK_add_MD(gettext("Преобразование баз для новой версии системы"));//4
  MENU.VVOD_SPISOK_add_MD(gettext("Коды функциональных клавиш применяемые в программе"));//5
  MENU.VVOD_SPISOK_add_MD(gettext("Получить коды генерируемые нажатием клавиш на клавиатуре"));//6
  MENU.VVOD_SPISOK_add_MD(gettext("Установить пароль для доступа к базе данных"));//7
  MENU.VVOD_SPISOK_add_MD(gettext("Удалить доступ к базе"));//8
  MENU.VVOD_SPISOK_add_MD(gettext("Конец работы"));//9

  znak(0,COLS-26);
  i_admin_infosys(12,COLS-27,host.ravno(),VERSION,DVERSIQ,0,0,0,"","");

  if(menu3(&MENU,&kom,1,0,0) != 0)
   continue;
   
  switch (kom)
   {

    case 9 :
    case -1 :
      exit_iceb(0);
      break;

    case 0:
      i_admint_rb();
      break;

    case 1:
      i_admint_sd(host.ravno());
      break;
    case 2:
      dirduser();
      break;
    case 3:
      dirbaz(login.ravno(),rootparol,host.ravno());
      break;

    case 4:
      admin_alter();
      break;

    case 5:
     admin_kodkl();
     break;

    case 6:
     admin_klaviat();
     break;

    case 7:
     i_admint_password(host.ravno());
     break;

    case 8:
     i_admint_user_del(host.ravno());
     break;
     
    default:
       kom=0;
       break;   
   }
 }
exit_iceb(0);



return(0);

}
/************************************/
/*Разметить базу*/
/*****************************/
void  i_admint_rb()
{
class iceb_tu_str imabaz("");
class iceb_tu_str kodirovka("utf8");
static class iceb_tu_str put_from_alx("/usr/share/iceB/alx");
static class iceb_tu_str put_to_alx;
static class iceb_tu_str put_from_doc("/usr/share/iceB/doc");

put_to_alx.plus(putnansi);

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Разметить новую базу данных"));

VV.VVOD_SPISOK_add_MD(gettext("Имя базы........................................"));
VV.VVOD_SPISOK_add_MD(gettext("Кодировка хранения данных......................."));
VV.VVOD_SPISOK_add_MD(gettext("Каталог откуда импортировать настроечные файлы.."));
VV.VVOD_SPISOK_add_MD(gettext("Каталог откуда импортировать документацию......."));

VV.VVOD_SPISOK_add_data(imabaz.ravno(),64);
VV.VVOD_SPISOK_add_data(kodirovka.ravno(),32);
VV.VVOD_SPISOK_add_data(put_from_alx.ravno(),112);
VV.VVOD_SPISOK_add_data(put_from_doc.ravno(),112);

naz:;

clear();
 
helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("разметить"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

int kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

imabaz.new_plus(VV.VVOD_SPISOK_return_data(0));
kodirovka.new_plus(VV.VVOD_SPISOK_return_data(1));
put_from_alx.new_plus(VV.data_ravno(2));
put_from_doc.new_plus(VV.data_ravno(3));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   /*Не выбрана база данных -негде взять файл с документацией*/
//   iceb_t_pdoc("admin1.txt");
   goto naz;

  case ESC:
  case FK10:
    return;

  case FK2:
  case PLU:
     break;


  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

GDITE();
/*проверяем существует ли база данных с таким именем*/
if(i_admint_pb(imabaz.ravno()) == 0)
 {

  class VVOD DANET(1);
  DANET.VVOD_SPISOK_add_MD(gettext("База с таким именем уже есть! Продолжить разметку?"));

  if(danet(&DANET,2,stdscr) == 2)
   goto naz;
 }

/*Проверяем возможность открытия настроечного файла*/   
class iceb_tu_str imafilprov(put_from_alx.ravno());
imafilprov.plus(G_DIR_SEPARATOR_S,"nastsys.alx");

FILE *ff;
if((ff = fopen(imafilprov.ravno(),"r")) == NULL)
 {
  error_op_nfil(imafilprov.ravno(),errno,gettext("Неправильно указан путь откуда брать настроечные файлы!"));
  goto naz;
 }
fclose(ff);

/*Проверяем возможность открытия файла с документацией*/   
imafilprov.new_plus(put_from_doc.ravno());
imafilprov.plus(G_DIR_SEPARATOR_S,"buhg.txt");

if((ff = fopen(imafilprov.ravno(),"r")) == NULL)
 {
  error_op_nfil(imafilprov.ravno(),errno,gettext("Неправильно указан путь на документацию!"));
  goto naz;
 }
fclose(ff);

/*Настроечные файлы теперь не устанавливаются в отдельный каталог*/
//admin_cp_alx(put_from_alx.ravno(),put_to_alx.ravno());

char strsql[512];


printw("%s\n",gettext("Создаем базу данных"));
if(kodirovka.ravno()[0] == '\0')
  sprintf(strsql,"create database %s",imabaz.ravno());
else
 sprintf(strsql,"create database %s DEFAULT CHARACTER SET '%s'",imabaz.ravno(),kodirovka.ravno());

if(sql_zapis(strsql,0,0) != 0)
 {
  if(sql_nerror(&bd) != ER_DB_CREATE_EXISTS) /*База уже есть*/
   goto naz;
 }


crtabl(imabaz.ravno()); /*размечаем таблицы*/

/*Загружаем настроечные файлы*/
alxin(put_from_alx.ravno(),0,stdscr);
/*Загружаем документацию*/
printw("%s\n",gettext("Загружаем документацию"));
refresh();
docin(put_from_doc.ravno(),stdscr);

iceb_t_soob(gettext("Разметка базы данных закончена"));  

}

/*********************************/
/*Создать доступ к базе данных*/
/*******************************/

void i_admint_sd(const char *host)
{
char strsql[1024];
int par=0;
class iceb_tu_str parolpol("");
class iceb_tu_str hostpol("");
class iceb_tu_str imapol("");
class iceb_tu_str dostup("");
class iceb_tu_str imabaz("");
int fk4=0;
VVOD VV(0);

VV.VVOD_SPISOK_add_data(32);
VV.VVOD_SPISOK_add_data(32);
VV.VVOD_SPISOK_add_data(32);
VV.VVOD_SPISOK_add_data("localhost",128);
VV.VVOD_SPISOK_add_data(2);

VV.VVOD_SPISOK_add_ZAG(gettext("Установка полномочий для работы с базой данных"));
sprintf(strsql,"host:%s",host);
VV.VVOD_SPISOK_add_ZAG(strsql);

VV.VVOD_SPISOK_add_MD(gettext("Логин оператора............."));//0
VV.VVOD_SPISOK_add_MD(gettext("Пароль оператора............"));//1
VV.VVOD_SPISOK_add_MD(gettext("Имя базы...................."));//2
VV.VVOD_SPISOK_add_MD(gettext("Хост доступа к базе данных.."));//3
VV.VVOD_SPISOK_add_MD(gettext("Доступ.............(r,w)...."));//4

naz:;
clear();

fk4=0;

helstr(LINES-1,0,
"F1",gettext("помощь"),
"F2/+",gettext("установить"),
"F4",gettext("проверить"),
"F10",gettext("выход"),NULL);

par=VV.vvod(0,1,1,-1,-1);

switch(par)
 {
  case FK10:
  case ESC:
    return;
  case FK1:
   clear();
   printw("\n\n\
    Программа позволяет как ввести нового пользователя с нужными полномочиями,\n\
  так и перустановить пароль и права доступа для уже введенного\n\
  пользователя.\n\
    В реквизит \"Хост пользователя\" вводиться хост с которого пользователю\n\
  будет разрешен доступ к базе данных. База данных MySQL позволяет ввести\n\
  любое количество хостов, с которых пользователь может получить доступ к базе\n\
  данных.\n\
    Если вы вводите нового пользователя и хост пользователя не будет введен,\n\
  то пользователь получит доступ к базе с любого хоста.\n\
    При переустановке пароля или прав доступа должен быть обязательно правильно\n\
  введён хост, с которого пользователю уже разрешен доступ, иначе полномочия\n\
  не будут переустановлены.\n");

    
    OSTANOV();
   goto naz;
   
  case PLU:
  case FK2:
   break;

  case FK4: /*проверить есть ли введённый пользоватеть для введённой базы*/
    fk4=1;
    break;

  default:
    goto naz;     
 }

GDITE();

imapol.new_plus(VV.VVOD_SPISOK_return_data(0));
parolpol.new_plus(VV.VVOD_SPISOK_return_data(1));
imabaz.new_plus(VV.VVOD_SPISOK_return_data(2));
hostpol.new_plus(VV.VVOD_SPISOK_return_data(3));
dostup.new_plus(VV.VVOD_SPISOK_return_data(4));

if(VV.VVOD_SPISOK_return_data(2)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введено имя базы данных !"));  
  goto naz;     
 }
if(VV.VVOD_SPISOK_return_data(0)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введён логин оператора!"));  
  goto naz;     
 }

/*проверяем существует ли база данных с таким именем*/
if(i_admint_pb(imabaz.ravno()) != 0)
 {
  iceb_t_soob(gettext("Нет базы данных с таким именем!"));
  goto naz;
 }


if(fk4 != 0)
 {
  
  sprintf(strsql,"select User from db where Db='%s' and User='%s'",imabaz.ravno(),imapol.ravno());
  if(readkey(strsql) > 0) /*Запись уже есть*/
    iceb_t_soob(gettext("Такой логин для этой базы уже введён!"));
  else
    iceb_t_soob(gettext("Такого логина для этой базы ещё нет"));
  
  goto naz;
 }

if(VV.VVOD_SPISOK_return_data(1)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введён пароль оператора !"));
  goto naz;     
 }

if(VV.VVOD_SPISOK_return_data(4)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введён реквизит Доступ !"));  
  goto naz;     
 }

if(VV.VVOD_SPISOK_return_data(4)[0] != 'r' && \
VV.VVOD_SPISOK_return_data(4)[0] != 'w' && \
VV.VVOD_SPISOK_return_data(4)[0] != 'R' && \
VV.VVOD_SPISOK_return_data(4)[0] != 'W')
 {
  iceb_t_soob(gettext("Не правильный код доступа !"));
  goto naz;     
 }

if(hostpol.ravno()[0] != '\0')
 {
  if(dostup.ravno()[0] == 'w')
   {
    
    sprintf(strsql,"GRANT INSERT,SELECT,DELETE,UPDATE,CREATE,DROP,LOCK TABLES,CREATE TEMPORARY TABLES ON \
%s.* TO '%s'@'%s' IDENTIFIED BY '%s'",imabaz.ravno(),imapol.ravno(),hostpol.ravno(),parolpol.ravno());
   }
  if(dostup.ravno()[0] == 'r')
    sprintf(strsql,"GRANT SELECT ON \
%s.* TO '%s'@'%s' IDENTIFIED BY '%s'",imabaz.ravno(),imapol.ravno(),hostpol.ravno(),parolpol.ravno());
 }
else
 {
  if(dostup.ravno()[0] == 'w')
   {
    sprintf(strsql,"GRANT INSERT,SELECT,DELETE,UPDATE,CREATE,DROP,LOCK TABLES,CREATE TEMPORARY TABLES ON \
%s.* TO '%s' IDENTIFIED BY '%s'",imabaz.ravno(),imapol.ravno(),parolpol.ravno());
   }
  if(dostup.ravno()[0] == 'r')
    sprintf(strsql,"GRANT SELECT ON %s.* TO '%s' IDENTIFIED BY '%s'",imabaz.ravno(),imapol.ravno(),parolpol.ravno());
 }
  
 
GDITE();
if(sql_zapis(strsql,1,1) == 0)
 {
  iceb_t_soob(gettext("Полномочия оператору установлены"));
  /*Записываем оператора в таблицу операторов если его там нет*/
  admin_newuser(imabaz.ravno(),imapol.ravno());
 }
sql_zapis("FLUSH PRIVILEGES",1,1);

goto naz;

}

/****************************/
/******************************/
void i_admin_infosys(int y,int x,const char *host,const char *verss,
const char *dvers,short ddd,short mmm,short ggg,
const char *imabaz,const char *nameprinter)
{

struct  passwd  *ktor; /*Кто работает*/
pid_t	mypid;

ktor=getpwuid(getuid());

mypid=getpid();
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

move(y=12,x=COLS-29);
if(ddd == 0 && mmm == 0 && ggg == 0)
 {
  printw("%s %d.%d.%d%s",gettext("Текущая дата"),dt,mt,gt,
  gettext("г."));
 }
else
 if(ddd != 0)
  {
   printw("%s %d.%d.%d%s",gettext("Текущая дата"),ddd,mmm,ggg,
   gettext("г."));
  }
 else
  printw("%s %d.%d%s",gettext("Текущая дата"),mmm,ggg,
  gettext("г."));

if(imabaz[0] != '\0')
 {
  move(++y,x);
  printw("%s%s",gettext("База:"),imabaz);
 }
move(++y,x);
printw("%s:%s",gettext("Хост"),host);

move(++y,x);
printw("%s:%d",gettext("Номер процесса"),mypid);

move(++y,x);
printw("%s:%d %s",gettext("Логин"),ktor->pw_uid,ktor->pw_name);

move(++y,x);
printw("%s:%-20.20s",gettext("Имя"),ktor->pw_gecos);

move(++y,x);
if(strlen(nameprinter) > 0)
 printw("%s:%-.20s",gettext("Принтер"),nameprinter);
else
 printw("%s:%-.*s",gettext("Принтер"),iceb_tu_kolbait(20,gettext("По умолчанию")),gettext("По умолчанию"));

move(y+=2,x);
printw("copyright © %s 2010",gettext("Белых А.И."));

move(++y,x);
printw("%s/0432/512973",gettext("г. Винница т."));

move(++y,x);
printw("%s +380676654526",gettext("Мобиль. тел."));

move(++y,x);
printw("%s",ICEBSAIT);

mvprintw(LINES-1,0,"%s %s %s %s%s",gettext("Версия"),verss,
gettext("от"),dvers,
gettext("г."));

//move(LINES-1,x-2);
//printw("http://www.iceb.vinnitsa.com");

}
/***************************/
/*преверка наличия базы данных*/
/*******************************/
/*Если вернули 0- база есть 1-нет*/
int i_admint_pb(const char *imapb)
{

/*проверяем существует ли база данных с таким именем*/

SQL_str row;
int kolstr=0;
class SQLCURSOR cur;
const char *strsql={"show databases"};

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(-1);
 }

while(cur.read_cursor(&row) != 0)
 {
  if(SRAV(imapb,row[0],0) == 0)
    return(0);
 }

return(1);
}
/*********************************/
/*Создать доступ к базе данных*/
/*******************************/

void i_admint_password(const char *host)
{
char strsql[1024];
int par=0;
class iceb_tu_str parolpol("");
class iceb_tu_str host_dostupa("");
class iceb_tu_str imapol("");

VVOD VV(0);

VV.VVOD_SPISOK_add_data(32);
VV.VVOD_SPISOK_add_data(32);
VV.VVOD_SPISOK_add_data("localhost",128);

VV.VVOD_SPISOK_add_ZAG(gettext("Установка пароля для доступа к базе данных"));
sprintf(strsql,"%s:%s",gettext("Хост базы с которой работаем"),host);
VV.VVOD_SPISOK_add_ZAG(strsql);

VV.VVOD_SPISOK_add_MD(gettext("Логин оператора..."));//0
VV.VVOD_SPISOK_add_MD(gettext("Пароль оператора.."));//1
VV.VVOD_SPISOK_add_MD(gettext("Хост доступа......"));//2

naz:;
clear();


helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("установить"),
"F10",gettext("выход"),NULL);

par=VV.vvod(0,1,1,-1,-1);

switch(par)
 {
  case FK10:
  case ESC:
    return;
   
  case PLU:
  case FK2:
   break;


  default:
    goto naz;     
 }

GDITE();

imapol.new_plus(VV.VVOD_SPISOK_return_data(0));
parolpol.new_plus(VV.VVOD_SPISOK_return_data(1));
host_dostupa.new_plus(VV.VVOD_SPISOK_return_data(2));

if(imapol.getdlinna() <= 1)
 {
  iceb_t_soob(gettext("Не введён логин оператора!"));  
  goto naz;     
 }
if(host_dostupa.getdlinna() <= 1)
 {
  iceb_t_soob(gettext("Не введён хост!"));  
  goto naz;     
 }
if(parolpol.getdlinna() <= 1)
 {
  iceb_t_soob(gettext("Не введён пароль!"));  
  goto naz;     
 }
/****************
Пароль переназначается для всех баз к которым у пользователя есть доступ
Если нужно запретить пользователю работать с какойто из баз к которым у него 
есть доступ, то нужно удалить соответствующюю запись в таблице db в базе mysql
******************/

GDITE();
sprintf(strsql,"SET PASSWORD FOR '%s'@'%s' = PASSWORD('%s')",imapol.ravno(),host_dostupa.ravno(),parolpol.ravno()); /*тут только host*/

if(sql_zapis(strsql,1,0) == 0)
 {
  iceb_t_soob(gettext("Пароль установлен"));
 }

sql_zapis("FLUSH PRIVILEGES",1,1);

}
/*********************************/
 /*Удалить доступ к базе данных*/
/*******************************/

void i_admint_user_del(const char *host)
{
char strsql[1024];
int par=0;
class iceb_tu_str host_dostupa("");
class iceb_tu_str imapol("");

VVOD VV(0);

VV.VVOD_SPISOK_add_data(32);
VV.VVOD_SPISOK_add_data("localhost",64);

VV.VVOD_SPISOK_add_ZAG(gettext("Удаление доступа к базе данных"));


VV.VVOD_SPISOK_add_MD(gettext("Логин оператора..."));//0
VV.VVOD_SPISOK_add_MD(gettext("Хост доступа......"));//1

naz:;
clear();


helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("установить"),
"F10",gettext("выход"),NULL);

par=VV.vvod(0,1,1,-1,-1);

switch(par)
 {
  case FK10:
  case ESC:
    return;
   
  case PLU:
  case FK2:
   break;


  default:
    goto naz;     
 }

GDITE();

imapol.new_plus(VV.VVOD_SPISOK_return_data(0));
host_dostupa.new_plus(VV.VVOD_SPISOK_return_data(1));

if(imapol.getdlinna() <= 1)
 {
  iceb_t_soob(gettext("Не введён логин оператора!"));  
  goto naz;     
 }

if(host_dostupa.getdlinna() <= 1)
 {
  iceb_t_soob(gettext("Не введён хост!"));  
  goto naz;     
 }

GDITE();
sprintf(strsql,"DROP USER '%s'@'%s'",imapol.ravno(),host_dostupa.ravno());

if(sql_zapis(strsql,1,0) == 0)
 {
  iceb_t_soob(gettext("Удаление выполнено"));
 }

sql_zapis("FLUSH PRIVILEGES",1,1);

}
