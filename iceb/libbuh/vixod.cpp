/*$Id: vixod.c,v 5.11 2013-04-07 10:22:55 sasa Exp $*/
/*28.03.2013    12.05.1994      Белых А.И.      vixod.c
Проверка даты
Возвращием 0 - если дата выходная
	   1 - нет
*/
#include	"buhl.h"


int vixod(short d,short m,short g)
{
char strsql[512];

sprintf(strsql,"select data from Zarsvd where data='%04d-%02d-%02d'",g,m,d);
if(readkey(strsql) == 1)
 return(0);

return(1);

}
