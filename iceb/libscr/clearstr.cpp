/*$Id: clearstr.c,v 5.4 2013/05/17 14:56:35 sasa Exp $*/
/*22.04.1997    22.04.1997      Белых А.И.      clearstr.c
Очистка строки и установка в нужную позицию
*/
#include        "scr.h"

void            clearstr(short y,short x)
//    y,x; /Координаты строки
{
move(y,x);
clrtoeol();  /*Очистить стороку*/

}
