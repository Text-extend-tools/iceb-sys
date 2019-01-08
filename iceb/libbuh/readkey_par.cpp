/*$Id: readkey_par.c,v 5.14 2014/01/31 12:11:40 sasa Exp $*/
/*05.01.2014	22.12.1999	Белых А.И.	readkey_par.c
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
#include	"buhl.h"

int iceb_t_vb(const char *host,const char *parol,char **imabaz);

extern int optind, opterr, optopt;
extern char *optarg;
class iceb_tu_str start_name_fil; /*имя файла с документами для загрузки*/
class iceb_tu_str login_op; /*логин оператора*/
int iceb_rz=0; /*режим запуска программы*/
extern int iceb_kod_podsystem;

void readkey_par(int argc,char **argv,
char **parol, //Паpоль
char **imabaz, //Имя базы
char **host) //Хост
{
int		i;
char		bros[512];

/* Просмотр ключей    */
/*
b - имя базы
h - хост
p - пароль
r - режим работы программы, для разных подсистем по разному
*/
while ((i = getopt(argc, argv, "b:h:p:r:f:l:")) != EOF) 
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


    case 'f': /*имя файла*/
      start_name_fil.plus(optarg);
      break;
/*******************
    case 'l':
      if(iceb_kod_podsystem == 0)
        login_op.plus(optarg);
      break;
*******************/
   }
 }

if(*parol == NULL)
 {
  if((*parol = getenv("PAROLICEB")) == NULL)
   {
    memset(bros,'\0',sizeof(bros));
    sprintf(bros,"%s:%s",gettext("Ваш логин"),login_op.ravno());
    VVOD VPR(2);
    VPR.VVOD_SPISOK_add_MD(bros);
    
    VPR.VVOD_SPISOK_add_MD(gettext("Введите пароль для доступа к базе данных"));
    
    memset(bros,'\0',sizeof(bros));
    vparol(&VPR,bros,20,stdscr,-1,-1,1);
    if(bros[0] != '\0')
     {
      (*parol)=new char[strlen(bros)+1];
      strcpy((*parol),bros);
     }
   }

 }
if(*host == NULL)
 {
  strcpy(bros,"localhost");
  (*host)=new char[strlen(bros)+1];
  strcpy(*host,bros);
 }

if(*parol == NULL)
 {
  *parol=new char[4];
  strcpy((*parol),"aaa");
 }

if(*imabaz == NULL)
 {
  if(iceb_t_vb(*host,*parol,&*imabaz) != 0)
   {
    refresh();
    endwin();
    exit(0);
   }  
 }


if(*imabaz == NULL)
 {
  strcpy(bros,"bx");
  (*imabaz)=new char[strlen(bros)+1];
  strcpy(*imabaz,bros);
 }
/*
printw("readkey parol=%s\n",*parol);
OSTANOV();
*/
//clear();
//refresh();
}

/************************************************/
//Если вернули 0 - все в порядке
//             1 - не все впорядке       

int readkey_par(int argc,char **argv,
char **parol, //Паpоль
char **imabaz, //Имя базы
char **host, //Хост
FILE *ff) //Файл в который выгружаются сообщения
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
while ((i = getopt(argc, argv, "b:h:p:r:")) != EOF) 
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

   }
 }

if(*parol == NULL)
 {
  if((*parol = getenv("PAROLICEB")) == NULL)
   {
    if(ff != NULL)
     fprintf(ff,"Не задан пароль доступа к базе данных !\n");
    return(1);
   }

 }
if(*imabaz == NULL)
 {
  if(ff != NULL)
   fprintf(ff,"Не задано имя базы данных !\n");
  return(1);
 }

if(*host == NULL)
 {
  strcpy(bros,"localhost");
  (*host)=new char[strlen(bros)+1];
  strcpy(*host,bros);
 }
if(ff != NULL)
 fprintf(ff,"Хост:%s\n",*host);
return(0); 
}
