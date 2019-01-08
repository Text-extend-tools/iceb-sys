/*$Id: uosgetls.c,v 5.3 2013/05/17 14:56:13 sasa Exp $*/
/*19.03.2011	19.03.2011	Белых А.И.	uosgetls.c
Получение ликвидационной стоимости
*/
#include "buhg.h"

double uosgetls(int innom,short d,short m,short g,int metka_u) /*0-для налогового 1-для бух.учёта*/
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select lsnu,lsbu from Uosls where inn=%d and data <= '%04d-%02d-%02d' order by data desc limit 1",
innom,g,m,d);

if(readkey(strsql,&row,&cur) == 1)
 {
  if(metka_u == 0)
   return(atof(row[0]));
  else
   return(atof(row[1]));
 }
return(0.);
}
