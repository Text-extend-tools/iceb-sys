/*$Id: hrvnks.c,v 5.10 2013/05/17 14:56:02 sasa Exp $*/
/*14.07.2015	28.08.2003	Белых А.И.	hrvnks.c
Шапка 
*/
#include "buhg.h"

void	hrvnks(FILE *ff,int *kolstrok)
{
*kolstrok=6;
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

fprintf(ff,"\
                  С П И С О К  N1 від %02d.%02d.%dp.\n\
    на зарахування зарплати працівникам %s\n\
    за %02d.%dp. перерахованої на карткові рахунки в філію\n\
   \"Відділення ПАТ Промінвестбанк в м.Вінниця\"\n\
    згідно платіжного доручення N____ від %02d.%02d.%dр.\n\n",
dt,mt,gt,
iceb_t_get_pnk("00",0),
mt,gt,
dt,mt,gt);

}
