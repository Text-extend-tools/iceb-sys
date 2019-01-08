/*$Id: impprov.c,v 5.29 2013/09/26 09:43:34 sasa Exp $*/
/*04.02.2016	10.07.2002	Белых А.И.	impprov.c
Импорт проводок из файла в "Главную книгу"
*/
#include        <errno.h>
#include        <sys/stat.h>
#include        "buhg.h"

extern char	*imabaz;

void	impprov(void)
{
struct stat work;
char		strsql[1024];
static iceb_tu_str imafim("");

naz:;

if(iceb_t_menu_import(&imafim,gettext("Импорт проводок из файла"),"impprov.alx","buhg3_5.txt") != 0)
  return;


clear();
move(LINES-1,0);
GDITE();

if(stat(imafim.ravno(),&work) != 0)
 {
  sprintf(strsql,"%s !",gettext("Нет такого файла"));
  iceb_t_soob(strsql);
  goto naz;
 }

if(impprov_r(imafim.ravno()) != 0)
 goto naz;
}
