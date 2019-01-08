/*$Id: uosvibu.c,v 5.3 2013/05/17 14:56:14 sasa Exp $*/
/*05.01.2012	05.01.2012	Белых А.И.	uosvibu.c
Меню выбора вида учёта
Если вернули 0-выбрали
             1-нет
*/
#include "buhg.h"

int uosvibu(int *vidu) /*0-налоговый 1-бухгалтерский*/
{
class VVOD MENU(3);
int kom=0;
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите из какого учёта брать данные"));

MENU.VVOD_SPISOK_add_MD(gettext("Налоговый учёт"));
MENU.VVOD_SPISOK_add_MD(gettext("Бухгалтерский учёт"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
*vidu=kom;
switch (kom)
 {
  case 2 :
  case -1 :
    return(1);
  case 0 :
    return(0);
  case 1 :
    return(0);
 }

return(1);
}

