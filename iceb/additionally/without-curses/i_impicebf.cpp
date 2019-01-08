/*$Id:$*/
/*03.02.2016	08.09.2013	Белых А.И.	i_impicebf.c
Импорт документов в iceB
*/
#define VERSION "1.16"
#define DATA_VERSION "12.03.2017"
#include        <locale.h>
#include "bezcurses.h"
SQL_baza	bd;
extern int iceb_rz;
extern int iceb_kod_podsystem;
extern class iceb_tu_str start_name_fil;
extern class iceb_tu_str kodirovka_iceb;

int impmatdok_rf(const char *imafim,int metkaproh); /*0-проверка 1-загрузка*/

int imp_kasord_rf(const char *imafim,int metka);
int impprov_rf(const char *imafim);
int impusldokt_r(int metkaproh,const char *imaf);
int nastimp();

extern char		*imabaz;
extern char		*host;
extern char	*parol_pol;
extern int metka_utf8; /*0-односимвольная кодировка 1-utf8*/
extern class iceb_tu_str kodirovka_iceb;
extern class iceb_tu_str login_op; /*логин оператора*/

int iceb_kod_podsystem = 0;

int main(int argc,char **argv,char **envp)
{
setlocale(LC_ALL,"");/*Устанавливает переменные определяющие локаль окружения*/

iceb_t_locale(); /*включаем локализацию*/
  
printf("%s\n%s iceB.\n\
%s %s %s %s %s\n",
argv[0],
gettext("Программа импорта документов в систему бухгалтерского учёта"),
gettext("Версия"),
VERSION,
gettext("от"),
DATA_VERSION,
gettext("г."));


if(argc == 1) /*Запуск без ключей*/
 {
  printf("\n\
%s:\n\
-h - %s\n\
-l - %s\n\
-f - %s\n\
-r - %s 1-%s\n\
                  2-%s\n\
                  3-%s\n\
                  4-%s\n\
-k - %s\n\
-b - %s\n\
-p - %s\n\
     %s PAROLICEB\n\n\
%s:\n\
0 - %s\n\
1 - %s\n\
2 - %s\n\
3 - %s\n",
  gettext("Ключи для запуска программы"),
  gettext("хост базы данных"),
  gettext("логин оператора"),
  gettext("имя файла для импорта"),
  gettext("режим работы"),gettext("импорт документов в материальный учёт"),
  gettext("импорт документов в учёт кассовых ордеров"),
  gettext("импорт проводок в главную книгу"),
  gettext("импорт документов в учёт услуг"),
  gettext("кодировка данных"),
  gettext("имя базы данных"),
  gettext("пароль для доступа к базе данных"),
  gettext("пароль также может быть передан программе через переменную среды"),
  gettext("Программа возвращает"),
  gettext("если ошибок небыло"),
  gettext("если ошибки были на этапе проверки"),
  gettext("если ошибки были на этапе загрузки"),
  gettext("если не найдены нужные настройки"));
  return(1); 
                    
 }



icebstart(argc,argv);

printf("%s:%s host=%s login=%s %s=%s\n",
gettext("База"),
imabaz,
host,
login_op.ravno(),
gettext("Кодировка"),
kodirovka_iceb.ravno());

if(start_name_fil.getdlinna() <= 1)
 {
  printf("%s\n",gettext("Не задано имя файла для импорта!"));
  sql_closebaz(&bd);
  return(1); 
 }
int voz=0;

if(nastimp() != 0) /*чтение настроек*/
 {
  sql_closebaz(&bd);
  return(3);
 }
 
switch(iceb_rz)
 {
  case 1: /*материальный учёт*/

   iceb_kod_podsystem=ICEB_PS_MU;
   printf("\n%s-%s\n",argv[0],gettext("Импорт в подсистему \"Материальный учёт\""));

    
   if(impmatdok_rf(start_name_fil.ravno(),0) == 0)
    {
     if(impmatdok_rf(start_name_fil.ravno(),1) != 0)
      voz=2;
    }
   else
     voz=1;
    break;

  case 2: /*учёт кассовых ордеров*/
    iceb_kod_podsystem=ICEB_PS_UKO;
    printf("\n%s-%s\n",argv[0],gettext("Импорт в подсистему \"Учёт кассовых ордеров\""));


   if(imp_kasord_rf(start_name_fil.ravno(),0) == 0)
    {
     if(imp_kasord_rf(start_name_fil.ravno(),1) != 0)
      voz=2;
    }
   else
     voz=1;
    break;

  case 3: /*главная книга*/
    printf("\n%s-%s\n",argv[0],gettext("Импорт в подсистему \"Главная книга\""));

    iceb_kod_podsystem=ICEB_PS_GK;

    if(impprov_rf(start_name_fil.ravno())  != 0)
     voz=1;

    break;

  case 4: /*учёт услуг*/
   printf("\n%s-%s\n",argv[0],gettext("Импорт в подсистему \"Учёт услуг\""));

   iceb_kod_podsystem=ICEB_PS_UU;

   if(impusldokt_r(0,start_name_fil.ravno()) == 0)
    {
     if(impusldokt_r(1,start_name_fil.ravno()) != 0)
      voz=2;      
    }
   else
    voz=1;
   break;
       
  default:
    printf("\n%s-%s %d\n",argv[0],gettext("Не известный режим работы"),iceb_rz);
    voz=1;
    break;
 };
 

sql_closebaz(&bd);
printf("Код возврата %d\n",voz);

return(voz);

}
