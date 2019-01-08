/*$Id: iceb_t_get_pnk.c,v 5.4 2013/05/17 14:56:28 sasa Exp $*/
/*03.10.2014	01.09.2011	Белых А.И.	iceb_t_get_pnk.c
Получение наименования контрагента короткого или полного если полного нет выдаётся короткое
*/
#include "buhl.h"

const char *iceb_t_get_pnk(const char *kodkontr,
int metka) /*0-короткое наименование 1-длинное*/
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
static class iceb_tu_str pnaim;
pnaim.new_plus("");

if(SRAV(kodkontr,"00-",1) == 0)
  sprintf(strsql,"select naikon,pnaim from Kontragent where kodkon='%s'","00");
else
  sprintf(strsql,"select naikon,pnaim from Kontragent where kodkon='%s'",kodkontr);

if(readkey(strsql,&row,&cur) == 1)
 {
  if(metka == 0)
   {
    if(polen(row[0],&pnaim,1,'(') != 0)
       pnaim.new_plus(row[0]);
   }
  else
   {  
    if(row[1][0] == '\0')
     {
      if(polen(row[0],&pnaim,1,'(') != 0)
       pnaim.new_plus(row[0]);
     }
    else
     pnaim.new_plus(row[1]);
   }
 } 
else
 {
  pnaim.new_plus(gettext("Не найден код контрагента"));
  pnaim.plus(" ",kodkontr);
 }
return(pnaim.ravno());
}