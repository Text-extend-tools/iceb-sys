/*$Id: impusldok.c,v 5.25 2014/07/31 07:09:16 sasa Exp $*/
/*04.02.2015	06.05.2004	Белых А.И.	impusldok.c
Импорт документов в "Учет услуг
*/
#include        <sys/stat.h>
#include        "buhg.h"

int impusldokt_r(int metkaproh,const char *imaf,int *koldok);
int imulsdok_xml_r(const char *imaf);

extern double	okrcn;  /*Округление цены*/



void impusldok()
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
 if(iceb_t_menu_import(&imafim,gettext("Импорт документов"),"impusldok.alx","usl2_5.txt") != 0)
  return;
if(kom == 1)
 if(iceb_t_menu_import(&imafim,gettext("Импорт полученной налоговой накладной из файла"),"","") != 0)
  return;

clear();
move(LINES-1,0);
GDITE();

if(kom == 0)
 {
  if(stat(imafim.ravno(),&work) != 0)
   {
    sprintf(strsql,"%s !",gettext("Нет такого файла"));
    iceb_t_soob(strsql);
    goto naz;
   }
  int koldok=0;
  int koloh=0;
  if((koloh=impusldokt_r(0,imafim.ravno(),&koldok)) == 0)
   {
    impusldokt_r(1,imafim.ravno(),&koldok);
    sprintf(strsql,"%s %d !",gettext("Количество загруженых документов"),koldok);
    iceb_t_soob(strsql);
   }
  else
   {
    VVOD SOOB(1);
    sprintf(strsql,"%s %d !",gettext("Количество ошибок"),koloh);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    SOOB.VVOD_SPISOK_add_MD(gettext("Импорт документов осуществляется если нет ошибок !"));
    soobshw(&SOOB,stdscr,-1,-1,0,1);
   } 
 }

if(kom == 1)
 {
  imulsdok_xml_r(imafim.ravno());
 }
}
