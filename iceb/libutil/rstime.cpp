/*$Id: rstime.c,v 5.10 2013/08/13 05:50:32 sasa Exp $*/
/*22.07.2013	16.04.2003	Белых А.И.	rstime.c
Расшифровка времени. Если вернули 0 - время введено правильно
                                  1 - нет
*/
#include "util.h"

int    rstime(short *has,short *min,short *sek,const char *vremq)
{
class iceb_tu_str bros("");

*has=*min=*sek=0;
char vrem1[strlen(vremq)+1];
strcpy(vrem1,vremq);

for(int i=0; i < (int)strlen(vrem1); i++)
  if(vrem1[i] == '.' || vrem1[i] == ',')
    vrem1[i]=':';
     
if(polen(vrem1,&bros,1,':') != 0)
 bros.new_plus(vrem1);

*has=bros.ravno_atoi();

polen(vrem1,min,2,':');
//*min=bros.ravno_atoi();

polen(vrem1,sek,3,':');
//*sek=atoi(bros);

if(*has < 0 || *has > 24)
 return(1);

if(*min < 0 || *min > 60)
 return(1);

if(*sek < 0 || *sek > 60)
 return(1);

return(0);
}
