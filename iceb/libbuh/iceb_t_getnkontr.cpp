/*$Id: iceb_t_getnkontr.c,v 5.3 2013/05/17 14:56:28 sasa Exp $*/
/*01.10.2010	01.10.2010	Белых А.И.	iceb_t_getnkontr.c
Получение свободного номера контрагента
*/
#include "buhl.h"

int iceb_t_getnkontr(int nom_start)
{
char strsql[112];
if(nom_start <= 0)
 nom_start=1;
for(int nom=nom_start; ; nom++)
 {
  sprintf(strsql,"select kodkon from Kontragent where kodkon='%d'",nom);
  if(readkey(strsql) == 0)
   return(nom);
 }
return(0);

}