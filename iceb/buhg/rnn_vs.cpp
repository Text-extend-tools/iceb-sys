/*$Id: rnn_vs.c,v 5.4 2013/05/17 14:56:10 sasa Exp $*/
/*15.02.2011	15.02.2011	Белых А.И.	rnn_vs.c
Выбор вида сортирвки выданных налоговых накладных
*/
#include "buhg.h"

int rnn_vs()
{

int metka_op=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите сортировку выданных накладных"));

MENU.VVOD_SPISOK_add_MD(gettext("Cортировать по номерам накладных"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Сортировать по номерам записей"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2

if(menu3w(stdscr,&MENU,&metka_op,-1,-1,0) != 0);

switch (metka_op)
 {
  case 2:
  case -1 :
    return(-1);

  case 0:
   break;

  case 1:
   break;

 }

return(metka_op);
}
