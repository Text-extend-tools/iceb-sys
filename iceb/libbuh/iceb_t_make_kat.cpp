/*$Id: iceb_t_make_kat.c,v 5.7 2014/05/07 10:57:30 sasa Exp $*/
/*30.04.2013	19.02.2009	Белых А.И.	iceb_t_make_kat.c
Создание каталога и подкаталога
Если вернули 0-каталог создан или уже существует
             1-нет
*/
#include <pwd.h>
#include <sys/stat.h>
#include        <errno.h>
#include "buhl.h"


int iceb_t_make_kat(class iceb_tu_str *putnakat,const char *podkat)
{
char strsql[1024];

struct passwd *rek_login;
rek_login=getpwuid(getuid());

putnakat->new_plus(rek_login->pw_dir);


putnakat->plus("/iceB");

umask(0000); 

if(mkdir(putnakat->ravno(),0775) != 0)
 {
//  printw("%d\n%d\n%d\n%d\n%d\n%d\n",EPERM,EFAULT,EACCES,ENAMETOOLONG,ENOENT,EEXIST);
  if(errno != EEXIST) //Уже есть такой каталог
   { 
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не могу создать каталог !"));
    sprintf(strsql,"%d %s",errno,strerror(errno));
    SOOB.VVOD_SPISOK_add_MD(strsql);
    SOOB.VVOD_SPISOK_add_MD(putnakat->ravno());
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    umask(ICEB_UMASK); /*Установка маски для записи и чтения группы*/
    return(1);
   }
 }

if(podkat[0] != '\0')
 {
  putnakat->plus("/");
  putnakat->plus(podkat);

  if(mkdir(putnakat->ravno(),0775) != 0)
   {
  //  printw("%d\n%d\n%d\n%d\n%d\n%d\n",EPERM,EFAULT,EACCES,ENAMETOOLONG,ENOENT,EEXIST);
    if(errno != EEXIST) //Уже есть такой каталог
     { 
      VVOD SOOB(1);
      SOOB.VVOD_SPISOK_add_MD(gettext("Не могу создать каталог!"));
      sprintf(strsql,"%d %s",errno,strerror(errno));
      SOOB.VVOD_SPISOK_add_MD(strsql);
      SOOB.VVOD_SPISOK_add_MD(putnakat->ravno());
      soobshw(&SOOB,stdscr,-1,-1,0,1);
      umask(ICEB_UMASK); /*Установка маски для записи и чтения группы*/
      return(1);
     }
   }
 }
 
umask(ICEB_UMASK); /*Установка маски для записи и чтения группы*/

return(0);
}

