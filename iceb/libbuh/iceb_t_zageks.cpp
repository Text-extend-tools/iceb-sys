/*$Id: iceb_t_zageks.c,v 5.1 2013/10/04 07:25:12 sasa Exp $*/
/*29.09.2013	29.09.2013	Белых А.И.	iceb_t_zageks.c
Заголовок файлов экспорта данных
*/
#include "buhl.h"

void iceb_t_zageks(const char *naim,FILE *ff)
{

fprintf(ff,"#%s\n#%s %s %s\n",
naim,
gettext("Выгружено из базы"),
iceb_t_get_namebase(),
iceb_t_get_pnk("00",1));

time_t vrem;
time(&vrem);
struct tm *bf;
bf=localtime(&vrem);

fprintf(ff,"\
#%s %d.%d.%d%s  %s:%02d:%02d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ff,"#%s:%d %s\n",gettext("Оператор"),iceb_t_getuid(),iceb_t_getfioop());
}
