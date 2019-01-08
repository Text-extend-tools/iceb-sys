/*$Id: printw_vr.c,v 5.6 2013/09/26 09:43:55 sasa Exp $*/
/*05.11.2007	21.12.2003	Белых А.И.	printw_vr.c
Вывод в окно времени расчёта
*/
#include <math.h>
#include "buhl.h"


void printw_vr(time_t vrem_n) //Время начала расчёта
{
time_t vrem_k;

time(&vrem_k);

double kii;
double dii=(vrem_k-vrem_n)/60.;

dii=modf(dii,&kii);

printw("\n%s %.f%s %.f%s\n",
gettext("Время расчёта"),
kii,
gettext("мин."),
dii*60.,
gettext("сек."));

refresh();
}

