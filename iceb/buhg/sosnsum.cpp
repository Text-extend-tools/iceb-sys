/*$Id: sosnsum.c,v 5.6 2013/05/17 14:56:11 sasa Exp $*/
/*29.11.2006	29.11.2006	Белых А.И.	sosnsum.c
Получение суммы начислений
*/
#include "buhg.h"


double	sosnsum(char *tabn,short dn,short mn,short gn,
short dk,short mk,short gk)
{
char	strsql[512];
int	kolstr=0;
SQL_str	row;
double suma=0.;

sprintf(strsql,"select suma from Zarp where tabn=%s \
and datz >= '%04d-%d-%d' and datz <= '%04d-%d-%d' and prn='1'",
tabn,gn,mn,dn,gk,mk,dk);

class SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(0.);
 }
while(cur.read_cursor(&row) != 0)
  suma+=atof(row[0]);
return(suma);

}
