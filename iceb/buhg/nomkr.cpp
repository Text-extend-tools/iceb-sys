/* $Id: nomkr.c,v 5.5 2013-04-07 10:22:43 sasa Exp $ */
/*23.06.2001    22.01.1994      Белых А.И.      nomkr.c
Определение последнего номера карточки на складе
*/
#include	"buhg.h"

int             nomkr(int sklad)
{
int		i;
char		strsql[100];
SQL_str         row;
long		kolstr;

GDITE();

sprintf(strsql,"select nomk from Kart where sklad=%d \
order by nomk asc",sklad);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return(0);
 }
if(kolstr == 0)
 return(1);

i=0;
while(cur.read_cursor(&row) != 0)
 {
  i++;
  if(i != atoi(row[0]))
   {
    return(i);
   }
 }
i++;

return(i);

}
