/*$Id: impmatdok.c,v 5.57 2013/09/26 09:43:33 sasa Exp $*/
/*16.04.2017	06.07.2001	Белых А.И.	impmatdok.c
Импорт документов в материальный учет из файла
*/
#include        <sys/stat.h>
#include        "buhg.h"

int impmatdok_xml_rt(const char *imaf);

void	impmatdok()
{
struct stat work;
char		strsql[1024];
static class iceb_tu_str imafim("");
int kom=0;



VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Импорт документов"));

MENU.VVOD_SPISOK_add_MD(gettext("Импорт документов в формате iceB"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Импорт полученных налоговых накладных в формате XML"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2

clear();

if(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

class spis_oth oth;   
switch (kom)
 {
  case 2:
  case -1 :
    return;

  case 0:
    break;

  case 1:
    break;

  default:
    return;
    break;
 }

naz:;

if(kom == 0)
 if(iceb_t_menu_import(&imafim,gettext("Импорт документов"),"impmatdok.alx","matu2_5.txt") != 0)
  return;
if(kom == 1)
 if(iceb_t_menu_import(&imafim,gettext("Импорт полученной налоговой накладной"),"","") != 0)
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

if(kom == 0)
 if(impmatdok_r(imafim.ravno(),0) == 0)
  impmatdok_r(imafim.ravno(),1);

if(kom == 1)
 impmatdok_xml_rt(imafim.ravno());
}
