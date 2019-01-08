/*$Id: dirfajl.c,v 5.11 2013-04-07 10:22:54 sasa Exp $*/
/*23.05.2018    15.12.1992      Белых А.И.   dirfajl.c
Программа просмотра имен файлов заданных по
трафарету
Если вернули 0 значит файлов не нашли

*/
#include	"buhl.h"
#include 	<dirent.h>


DIR             *dirp;
struct dirent *dppp;


int dirfajl(const char *name,
int mi,   //Метка поиска 1-до точки , 2-росле точки, 3 полное имя файла
class SPISOK *mfa) /*Массив с именами файлов*/
{
//int             len;
class iceb_tu_str bros("");
int             voz;

//len = strlen(name);

if((dirp = opendir(".")) == NULL) 
 {
  perror(".");
  iceb_t_soob(gettext("Не могу открыть каталог !"));

  return(0);
 }
if((dppp = readdir(dirp)) == NULL) 
 {
  perror(__FUNCTION__);
  iceb_t_soob(gettext("Не могу прочесть каталог !"));
  return(0);
 }
voz=0;

for (; dppp != NULL; )
 {
  if(SRAV(name,"*",0) == 0)
    goto vper;
      
  if(polen(dppp->d_name,&bros,3,'.') != 0)
   if(polen(dppp->d_name,&bros,mi,'.') == 0)
    if(proverka(name,bros.ravno(),2,1) == 0)
//    if(strcmp(bros.ravno(),name) == 0 &&  (int)strlen(bros.ravno()) == len) 
     {
vper:;

       voz++;
       mfa->plus(dppp->d_name);
     }
   dppp = readdir(dirp);
 }
closedir(dirp);

return(voz);

}
