/*$Id: alxin.c,v 5.4 2013/05/17 14:55:55 sasa Exp $*/
/*13.07.2015	09.09.2010	Белых А.И.	alxin.c
Загрузка настроечных файлов в базу
*/
#include <errno.h>
#include "buhg.h"


int alxin(const char *put_alx,
int metka, /*0-загрузить если в базе нет 1-всегда грузить*/
WINDOW *win) 
{
char strsql[512];
class iceb_tu_str allput(put_alx);
//allput.plus(G_DIR_SEPARATOR_S);

GDir *dirp;
if((dirp=g_dir_open(allput.ravno(),0,NULL)) == NULL)
 {
  perror(__FUNCTION__);
  class iceb_tu_str repl;
  repl.plus(__FUNCTION__);
  repl.ps_plus(gettext("Не могу открыть каталог !"));
  repl.ps_plus(allput.ravno());
  repl.ps_plus(strerror(errno));
  iceb_t_soob(repl.ravno());
  
  return(1);
 }
class iceb_tu_str imafil;
class iceb_tu_str rashir;
const char *imafil_read;

while((imafil_read=g_dir_read_name(dirp)) != NULL)
 {
  if(polen(imafil_read,&rashir,2,'.') != 0)
   continue;

  if(SRAV("alx",rashir.ravno(),0) != 0)
   continue;

  /*Проверяем есть ли там этот файл*/
  if(metka == 0)
   {
    sprintf(strsql,"select fil from Alx where fil='%s' limit 1",imafil_read);
    if(readkey(strsql) > 0)
     continue;
   }
  if(SRAV("icebstart",imafil_read,1) == 0)
   continue;
  
  imafil.new_plus(allput.ravno());
  imafil.plus(G_DIR_SEPARATOR_S,imafil_read);
  wprintw(win,"%s-%s\n",gettext("Загрузка файла"),imafil.ravno());
  wrefresh(win);
  iceb_t_alxin(imafil.ravno());

 }
g_dir_close(dirp);
return(0);
}
