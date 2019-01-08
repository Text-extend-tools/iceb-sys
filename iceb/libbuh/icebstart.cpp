/*$Id: icebstart.c,v 5.22 2014/01/31 12:11:40 sasa Exp $*/
/*18.01.2016	29.03.2005	Белых А.И. 	icebstart.c
Начало работы подсистем 
*/

#include        <locale.h>
#include        <sys/stat.h>
#include        <pwd.h>
#include "buhl.h"

char		*imabaz=NULL;
char		*host=NULL;
char		*parol_pol=NULL;
int metka_utf8=0; /*0-односимвольная кодировка 1-utf8*/
class iceb_tu_str kodirovka_iceb;
extern class iceb_tu_str login_op; /*логин оператора*/

void icebstart(int argc,char **argv)
{
struct  passwd  *ktor; /*Кто работает*/
class iceb_tu_str locale;

ktor=getpwuid(getuid());
login_op.new_plus(ktor->pw_name);

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


init_screen();  /*Запуск экранного режима*/
iceb_t_locale(); /*включаем локализацию*/


umask(ICEB_UMASK); /*Установка маски для файлов для записи и чтения группы*/

/*Читаем ключи запуска программы*/
readkey_par(argc,argv,&parol_pol,&imabaz,&host);


/* прочитать инициализационный файл (параметры устройств) */
//read_ini();


if(sql_openbaz(&bd,imabaz,host,login_op.ravno(),parol_pol) != 0)
  eropbaz(imabaz,ktor->pw_uid,login_op.ravno(),0);

icebstart_rf(); //выполняем команды после подключения

char strsql[512];
SQL_str row;
class SQLCURSOR cur;
/*Читаем наименование организации*/

sprintf(strsql,"select naikon from Kontragent where kodkon='00'");
if(readkey(strsql,&row,&cur) != 1)
 {
  iceb_t_vkk00();
 }


}
