/*$Id: iceb_tu_strspisok.c,v 5.2 2013/05/17 14:56:41 sasa Exp $*/
/*28.03.2012	28.03.2012	Белых А.И.	iceb_tu_strspisok.c
Взятие данных из строки разделённых символом разделителем в список
*/

#include "util.h"

void iceb_tu_strspisok(const char *stroka,int razdelitel,class SPISOK *spisok)
{
int kolih_razdel=pole2(stroka,razdelitel);
if(kolih_razdel == 0)
 {
  spisok->plus(stroka);
  return;
 } 
class iceb_tu_str pole("");
for(int nom=1; nom <= kolih_razdel; nom++)
 {
  polen(stroka,&pole,nom,razdelitel);
  spisok->plus(pole.ravno());
 }
 
}
