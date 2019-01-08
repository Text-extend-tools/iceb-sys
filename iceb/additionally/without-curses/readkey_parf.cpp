/*$Id: readkey_par.c,v 5.11 2013/05/17 14:56:31 sasa Exp $*/
/*31.10.2013	22.12.1999	Белых А.И.	readkey_parf.c
Чтение ключей запуска программы

Давайте разберемся с работой первой функции - getopt(...). Ее определение выглядит следующим образом:

#include <unistd.h>

int getopt(int argc, char * const argv[],const char *optstring);
 
 extern char *optarg;
 extern int optind, opterr, optopt;
 
Эта функция последовательно перебирает переданные параметры в программу. 
Для работы в функцию передается количество параметров argc, массив параметров argv[] и специальная строка optstring,
в которой перечисляются названия коротких параметров и признаки того, что параметры должны иметь значение. 
Например, если программа должна воспринимать три параметра a, b, F , то такая строка бы выглядела как "abF".
Если параметр должен иметь значение, то после буквы параметра ставится двоеточие, например параметр F и d имеют значения,
а параметры e, a и b не имеют, тогда эта строка могла бы выглядеть как "eF:ad:b". Если параметр может иметь (т.е. может и не иметь) значение,
то тогда ставится два знака двоеточия, например "a::" (это специальное расширение GNU). 
Если optstring содержит "W:", то тогда параметр -W opt переданный в программу, будет восприниматься как длинный параметр --opt. 
Это связано с тем, что параметр W зарезервирован в POSIX.2 для расширения возможностей.
 
Для перебора параметров функцию getopt() надо вызывать в цикле. 
В качестве результата возвращется буква названия параметра, если же параметры кончились, то функция возвращает -1. 
Индекс текущего параметра хранится в optind, а значение параметра помещается в optarg (указатель просто указывает на элемент массива argv[]). 
Если функция находит параметр не перечисленный в списке, то выводится сообщение об ошибке в stderr и код ошибки сохраняется в opterr, 
при этом в качестве значения возврящается "?". Вывод ошибки можно запретить, если установить opterr в 0. 

*/
#include "bezcurses.h"
#include <unistd.h>

int iceb_t_vb(const char *host,const char *parol,char **imabaz);

extern int optind, opterr, optopt;
extern char *optarg;

class iceb_tu_str start_name_fil; /*имя файла с документами для загрузки*/
class iceb_tu_str login_op; /*логин оператора*/
int iceb_rz; /*режим запуска программы*/
extern int iceb_kod_podsystem;
extern class iceb_tu_str kodirovka_iceb;

void readkey_par(int argc,char **argv,
char **parol, //Паpоль
char **imabaz, //Имя базы
char **host) //Хост
{
int		i;
char		bros[256];

/* Просмотр ключей    */
/*
b - имя базы
h - хост
p - пароль
r - режим работы программы, для разных подсистем по разному
*/
while ((i = getopt(argc, argv, "b:h:p:r:f:l:k:")) != EOF) 
 {
  switch (i) 
   {
    case 'b': /* Имя базы*/
      (*imabaz)=new char[strlen(optarg)+1];
      strcpy(*imabaz,optarg);
      break;

    case 'h': /* Хост*/
      (*host)=new char[strlen(optarg)+1];
      strcpy(*host,optarg);
      break;

    case 'p': /* Пароль*/
      (*parol)=new char[strlen(optarg)+1];
      strcpy(*parol,optarg);
      break;

    case 'r': /* Режим работы программы*/
      iceb_rz=atoi(optarg);
      break;

    case 'f': /*имя файла*/
      start_name_fil.plus(optarg);
      break;

    case 'l': /*имя файла*/
      if(iceb_kod_podsystem == 0)
        login_op.plus(optarg);
      break;

    case 'k': /*кодировка запросов к базе данных*/
      kodirovka_iceb.new_plus(optarg);
      break;

   }
 }

if(*parol == NULL)
 {
  if((*parol = getenv("PAROLICEB")) == NULL)
    {
     printf("%s-%s!\n",__FUNCTION__,gettext("Не задан пароль"));
     exit(1);
    }
 }
if(*host == NULL)
 {
  strcpy(bros,"localhost");
  (*host)=new char[strlen(bros)+1];
  strcpy(*host,bros);
 }


if(*imabaz == NULL)
 {
  printf("%s-%s!\n",__FUNCTION__,gettext("Не задано имя базы"));
  exit(1);
 }


}
