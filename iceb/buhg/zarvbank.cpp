/*$Id: zarvbank.c,v 5.3 2013/05/17 14:56:18 sasa Exp $*/
/*06.12.2010	06.12.2010	Белых А.И.	zarvbank.c
Выбор банка из списка банков
*/

#include "buhg.h"

int zarvbank()
{
SQL_str row;
class SQLCURSOR cur;
int kolstr=0;
char strsql[112]; 
class iceb_tu_str kod_banka("");
class iceb_tu_str naim_banka("");

sprintf(strsql,"select kod from Zarsb");
if((kolstr=readkey(strsql,&row,&cur)) > 0)
 {
  if(kolstr == 1)
   kod_banka.new_plus(row[0]);
  else
    if(dirsb(2,&kod_banka,&naim_banka) != 0)
      return(-1);
 }
return(kod_banka.ravno_atoi());
}
