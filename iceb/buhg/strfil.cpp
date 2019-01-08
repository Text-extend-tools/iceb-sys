/*$Id: strfil.c,v 5.5 2013/05/17 14:56:12 sasa Exp $*/
/*30.09.2009	06.09.2006	Белых А.И.	strfil.c
Выдача строки в корешок
*/
#include "buhg.h"

void strfil(const char *a1,const char *a2,short mt,FILE *kaw)
{
short           i;
if(kaw == NULL)
 return;
for(i=0;i < mt;i++)
  fprintf(kaw,"\n");
//fprintf(kaw,"%-62s || %s\n",a1,a2);
fprintf(kaw,"%-*s || %s\n",iceb_tu_kolbait(62,a1),a1,a2);
}
