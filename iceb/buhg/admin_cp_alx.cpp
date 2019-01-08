/*$Id: admin_cp_alx.c,v 5.7 2013/05/17 14:55:55 sasa Exp $*/
/*09.09.2010	12.03.2010	Белых А.И.	admin_cp_alx.c
Установка настроечных файлов в нужный каталог
*/
#include <glib.h>
#include <sys/stat.h>
#include        <errno.h>
#include "buhg.h"


int admin_cp_alx(const char *from_alx,const char *to_alx)
{
char stroka[1024];

umask(0000); 

if(mkdir(to_alx,0775) != 0)
 {
//  printw("%d\n%d\n%d\n%d\n%d\n%d\n",EPERM,EFAULT,EACCES,ENAMETOOLONG,ENOENT,EEXIST);
  if(errno != EEXIST) //Уже есть такой каталог
   { 
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не могу создать каталог !"));
    sprintf(stroka,"%d %s",errno,strerror(errno));
    SOOB.VVOD_SPISOK_add_MD(stroka);
    SOOB.VVOD_SPISOK_add_MD(to_alx);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    umask(ICEB_UMASK); /*Установка маски для записи и чтения группы*/
    return(1);
   }
 }

umask(ICEB_UMASK); /*Установка маски для записи и чтения группы*/

class iceb_tu_str imafil_from;
class iceb_tu_str imafil_to;

imafil_from.new_plus(from_alx);
imafil_from.plus(G_DIR_SEPARATOR_S,"icebstart.alx");

imafil_to.new_plus(to_alx);
imafil_to.plus(G_DIR_SEPARATOR_S,"icebstart.alx");

printw("%s -> %s\n",imafil_from.ravno(),to_alx);
refresh();

iceb_t_cp(imafil_from.ravno(),imafil_to.ravno(),2);



imafil_from.new_plus(from_alx);
imafil_from.plus(G_DIR_SEPARATOR_S,"icebstartw.alx");

imafil_to.new_plus(to_alx);
imafil_to.plus(G_DIR_SEPARATOR_S,"icebstartw.alx");


printw("%s -> %s\n",imafil_from.ravno(),to_alx);
refresh();
iceb_t_cp(imafil_from.ravno(),imafil_to.ravno(),2);

#if 0
###############################33
GDir *dirp;
if((dirp=g_dir_open(from_alx,0,NULL)) == NULL)
 {
  perror(__FUNCTION__);
  class iceb_tu_str repl;
  repl.plus(__FUNCTION__);
  repl.ps_plus(gettext("Не могу открыть каталог !"));
  repl.ps_plus(from_alx);
  repl.ps_plus(strerror(errno));
  iceb_t_soob(repl.ravno());
  
  return(1);
 }
const char *imafil_read;

while((imafil_read=g_dir_read_name(dirp)) != NULL)
 {
  

  imafil_from.new_plus(from_alx);
  imafil_from.plus(G_DIR_SEPARATOR_S,imafil_read);

  imafil_to.new_plus(to_alx);
  imafil_to.plus(G_DIR_SEPARATOR_S,imafil_read);
  
  printw("%s -> %s\n",imafil_from.ravno(),to_alx);
  refresh();
  iceb_t_cp(imafil_from.ravno(),imafil_to.ravno(),2);
 }
g_dir_close(dirp);
##########################3333
#endif

return(0);
}