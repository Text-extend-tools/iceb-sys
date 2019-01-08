/*$Id:$*/
/*27.10.2016	27.10.2016	Белых А.И.	iceb_t_get_fiotbn.c
Получить фамилию по табельному номеру
*/
#include "buhl.h"

const char *iceb_t_get_fiotbn(int tabnom)
{
static class iceb_tu_str fio("");
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select fio from Kartb where tabn = %d",tabnom);
if(readkey(strsql,&row,&cur) == 1)
 fio.new_plus(row[0]);
else
 {
  sprintf(strsql,"%s-%s %d\n",__FUNCTION__,gettext("Не нашли карточку для табельного номера"),tabnom);
  fio.new_plus(strsql);
 }
return(fio.ravno());
}
