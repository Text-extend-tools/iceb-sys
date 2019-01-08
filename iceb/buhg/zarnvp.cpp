/*$Id: zarnvp.c,v 5.5 2013/11/05 10:50:50 sasa Exp $*/
/*12.10.2013	11.03.2011	Белых А.И.	zarnvp.c
Настройка выполнения проводок
*/
#include "buhg.h"

void zarnvp()
{

int metka_prov=0;
class iceb_tu_str stroka("");
char strsql[512];

sprintf(strsql,"select ns from Alx where fil='zarnpr.alx' limit 1");
if(readkey(strsql) > 0)
 {

  if(iceb_t_poldan("Автоматическое выполнение проводок с использованием таблицы",&stroka,"zarnast.alx") == 0)
   if(SRAV(stroka.ravno(),"Вкл",1) == 0)
    metka_prov=1;
 }
 
if(metka_prov == 1)
 {
  iceb_redfil("zarnpr.alx",0);
  return;
 }

int		kom;

VVOD MENU(3);

kom=0;



MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное"));

MENU.VVOD_SPISOK_add_MD(gettext("Настройка выполнения проводок для начислений"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Настройка выполнения проводок для удержаний"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Настройка выполнения проводок для соц. начислений"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//3

naz:;

clear();

while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

switch (kom)
 {
  case 3 :
  case -1 :
    return;

  case 0 :
    iceb_redfil("avtprozar1.alx",0);
    goto naz;

  case 1:
    iceb_redfil("avtprozar2.alx",0);
    goto naz;

  case 2:
    iceb_redfil("avtprozar3.alx",0);
    goto naz;
    
  default:
    kom=0;
    goto naz;
 }

}

