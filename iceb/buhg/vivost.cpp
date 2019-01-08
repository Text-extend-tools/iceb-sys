/* $Id: vivost.c,v 5.8 2013/05/17 14:56:15 sasa Exp $ */
/*30.01.2011    21.09.1994      Белых А.И.      vivost.c
Вывод остатков на экран
*/
#include	"buhg.h"


void vivost(short y,short x, //Координаты вывода
short mt, //1-вывод остатков по карточке 2- по документам
class ostatokt *ost)
{

move(y,x);
printw("%-*s:%10.10g %10.2f %10.10g %10.2f\n",iceb_tu_kolbait(10,gettext("Остаток")),gettext("Остаток"),ost->ostg[0],ost->ostgc[0],ost->ostm[0],ost->ostmc[0]);
printw("%-*s:%10.10g %10.2f %10.10g %10.2f\n",iceb_tu_kolbait(10,gettext("Приход")),gettext("Приход"),ost->ostg[1],ost->ostgc[1],ost->ostm[1],ost->ostmc[1]);
printw("%-*s:%10.10g %10.2f %10.10g %10.2f\n",iceb_tu_kolbait(10,gettext("Расход")),gettext("Расход"),ost->ostg[2],ost->ostgc[2],ost->ostm[2],ost->ostmc[2]);
printw("%-*s:%10.10g %10.2f %10.10g %10.2f\n",iceb_tu_kolbait(10,gettext("Остаток")),gettext("Остаток"),ost->ostg[3],ost->ostgc[3],ost->ostm[3],ost->ostmc[3]);

move(y+1,x+55);
if(mt == 1)
 {
  printw(gettext("Остатки по карт."));
 }
if(mt == 2)
 {
  attron(A_BLINK); /*Включить мерцание*/
  printw(gettext("Остатки по докум."));
  attroff(A_BLINK); /*Выключить мерцание*/
 }
refresh();
}
