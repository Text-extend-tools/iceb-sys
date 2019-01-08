/*$Id: iceb_t_rstrf.c,v 5.8 2013-04-07 10:22:54 sasa Exp $*/
/*27.03.2013	25.03.2007	Белых А.И.	iceb_t_rstrf.c
Получить строку по заданному номеру строки в файле
Ели вернули 0-порядок
            1-нет
*/
#include "buhl.h"


int iceb_t_rstrf(int nomer_str,const char *imaf,class iceb_tu_str *str_fil)
{
int metka=1;
SQL_str row_alx;
class SQLCURSOR cur_alx;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='%s' and ns=%d",imaf,nomer_str);
if(readkey(strsql,&row_alx,&cur_alx) == 1)
 {
  metka=0;
  str_fil->new_plus(row_alx[0]);
 }

return(metka);
}
