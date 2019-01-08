/*$Id:$*/
/*22.11.2016	22.11.2016	Белых А.И.	iceb_t_get_edrpou.c
Получение кода ЄДРПОУ
*/

#include "buhl.h"

const char *iceb_t_get_edrpou(const char *kod_kontr)
{
static class iceb_tu_str edrpou("");
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select kod from Kontragent where kodkon='%s'",kod_kontr);
if(readkey(strsql,&row,&cur) == 1)
 edrpou.new_plus(row[0]);
else
 {
  sprintf(strsql,"%s-%s %s!",__FUNCTION__,gettext("Не найден код контрагента"),kod_kontr);
  edrpou.new_plus(strsql);
 }
return(edrpou.ravno());
}
