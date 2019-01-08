/*$Id: iceb_t_delfil.c,v 5.7 2013-04-07 10:22:54 sasa Exp $*/
/*20.11.2014	03.04.2008	Белых А.И.	iceb_t_delfil.c
Удаление всех файлов в каталоге по заданному расширению
Если вернули 0-удалили
             1-нет
*/
#include <errno.h>
#include "buhl.h"
#include <dirent.h>




int iceb_t_delfil(const char *katalog,const char *rashir)
{
DIR             *dirp;
struct dirent *dppp;


if((dirp = opendir(katalog)) == NULL) 
 {
  perror(__FUNCTION__);
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(__FUNCTION__);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не могу открыть каталог !"));
  SOOB.VVOD_SPISOK_add_MD(katalog);
  SOOB.VVOD_SPISOK_add_MD(strerror(errno));

  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(1);
 }

int kol_pol=0;
class iceb_tu_str filr("");
class iceb_tu_str imafil("");
while((dppp = readdir(dirp)) != NULL) 
 {
  if(dppp->d_name[0] == '/') /*пропускаем каталоги*/
   continue;
  /*Пропускаем точку и две точки*/
  if(SRAV(dppp->d_name,".",0) == 0)
   continue;  
  if(SRAV(dppp->d_name,"..",0) == 0)
   continue;  
  
  if((kol_pol=pole2(dppp->d_name,'.')) == 0)
   continue;
  polen(dppp->d_name,&filr,kol_pol,'.');

  if(proverka(rashir,filr.ravno(),0,1) != 0)
   continue;

//  sprintf(imafil,"%s/%s",katalog,dppp->d_name);  
  imafil.new_plus(katalog);
  imafil.plus("/",dppp->d_name);
  unlink(imafil.ravno());
 }

closedir(dirp);

return(0);


}
