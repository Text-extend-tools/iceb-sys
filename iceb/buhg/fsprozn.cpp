/*$Id: fsprozn.c,v 5.19 2013/11/05 10:50:48 sasa Exp $*/
/*14.07.2015	08.07.2003	Белых А.И.	fsprozn.c
Программа списания розницы
*/
#include <pwd.h>
#include        <sys/stat.h>
#include        <errno.h>
#include        <locale.h>
#include        "buhg.h"

extern char *imabaz;
SQL_baza	bd;
short		ddd,mmm,ggg;

int             main(int argc,char **argv,char **envp)
{
char		*host=NULL;
char		*parol=NULL;
struct  passwd  *ktor; /*Кто работает*/
char		kto[64];
FILE		*ff=NULL;
char		imaf[64];
struct  tm      *bf;
time_t          tmm;

ktor=getpwuid(getuid());

(void)setlocale(LC_ALL,"");
(void)setlocale(LC_NUMERIC,"C"); //Переменная среды определяющая чем 
                                 //разделяется целая и дробная часть числа

init_screen();  /*Запуск экранного режима*/

umask(ICEB_UMASK); /*Установка маски для записи и чтения группы*/

strcpy(kto,ktor->pw_name);

time(&tmm);
bf=localtime(&tmm);

sprintf(imaf,"%s%siceb_sr%02d%d.pr",g_get_tmp_dir(),G_DIR_SEPARATOR_S,bf->tm_mon+1,bf->tm_year+1900);
if(ff == NULL)
 if((ff = fopen(imaf,"a")) == NULL)
  fprintf(stderr,"\nПрограмма xmu.\nОшибка открытия файла %s:%d\n",imaf,errno);

if(ff != NULL)
 {
  fprintf(ff,"----------------------------------------------\n");
  fprintf(ff,"Списание из fsprozn. Запущена %d %s Дата:%d.%d.%dг. Время:%02d:%02d:%02d\n",
  iceb_t_getuid(),ktor->pw_gecos,bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,bf->tm_hour,bf->tm_min,bf->tm_sec);
 }

/*Читаем ключи запуска программы*/
if(readkey_par(argc,argv,&parol,&imabaz,&host,ff) != 0)
 goto kon;
 


if(matnast() != 0)
 {
  if(ff != NULL)
    fprintf(ff,"Не прочитан файл настроек matnast.alx\n");
  goto kon;
 }

if(sql_openbaz(&bd,imabaz,host,kto,parol) != 0)
  eropbaz(imabaz,ktor->pw_uid,ktor->pw_name,0,ff);

icebstart_rf();

sprozn(1,ff);



sql_closebaz(&bd);

kon:;

if(ff != NULL)
 {
  time(&tmm);
  bf=localtime(&tmm);
  fprintf(ff,"Программа завершила работу. Дата:%d.%d.%dг. Время:%02d:%02d:%02d\n",
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,bf->tm_hour,bf->tm_min,bf->tm_sec);
  fprintf(ff,"----------------------------------------------\n");
 }

if(ff != NULL)
 fclose(ff);

refresh();
endwin();

}
