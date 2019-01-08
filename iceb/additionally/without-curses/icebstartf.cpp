/*$Id: icebstart.c,v 5.19 2013/05/17 14:56:29 sasa Exp $*/
/*12.03.2017	29.03.2005	Белых А.И. 	icebstartf.c
Начало работы подсистем 
для работы без ncurses
*/

#include        <locale.h>
#include        <sys/stat.h>
#include        <pwd.h>
#include "bezcurses.h"
#include        <unistd.h>

void icebstart_rff();

void readkey_par(int argc,char **argv,char **parol,char **imabaz,char **host);
char		*imabaz;
char		*host;
char		*parol_pol;
int metka_utf8; /*0-односимвольная кодировка 1-utf8*/
class iceb_tu_str kodirovka_iceb;


extern class iceb_tu_str login_op; /*логин оператора*/

void icebstart(int argc,char **argv)
{
struct  passwd  *ktor; /*Кто работает*/
class iceb_tu_str locale;
ktor=getpwuid(getuid());

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


iceb_t_locale(); /*включаем локализацию*/


umask(ICEB_UMASK); /*Установка маски для файлов для записи и чтения группы*/

/*Читаем ключи запуска программы*/
readkey_par(argc,argv,&parol_pol,&imabaz,&host);

if(login_op.getdlinna() <= 1)
 login_op.new_plus(ktor->pw_name);

if(sql_openbaz(&bd,imabaz,host,login_op.ravno(),parol_pol) != 0)
 {
  printf("%s:%s",gettext("Логин"),login_op.ravno());
  printf("%s %s:",gettext("Ошибка открытия базы данных"),imabaz);
  printf("%d %s\n",sql_nerror(&bd),sql_error(&bd));
  exit(1);
 }
icebstart_rff();


}
