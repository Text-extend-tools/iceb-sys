/*$Id: rsdat1.c,v 5.8 2012-10-30 09:24:34 sasa Exp $*/
/*29.10.2012    11.11.1993      Белых А.И.      rsdat1.c
Подпрограмма расшифровки символьной строки с датой
Если вернули 0 - дата введена правильно
*/
#include	"util.h"

short rsdat1(short *m,short *g,const char *str)
{
class iceb_tu_str bros("");
int		i;
char stroka[strlen(str)+1];
strcpy(stroka,str);

for(i=0; stroka[i] != '\0'; i++)
  if(stroka[i] == ',')
    stroka[i]='.';

polen(stroka,m,1,'.');

polen(stroka,g,2,'.');


if(*m > 12 || *m <= 0)
 return(2);

if(*g < 1980 || *g > 3000)
 return(3);

return(0);
}
