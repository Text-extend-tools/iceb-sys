/*$Id: pole2.c,v 5.6 2013/05/17 14:56:42 sasa Exp $*/
/*09.11.2008    27.11.1996      Белых А.И.      pole2.c
Получение количества полей в строке
*/
#include	"util.h"

int pole2(const char *str,char raz)
{
int             i;
int             kp;

kp=0;
for(i=0; str[i] != '\0' ; i++)
 if(str[i] == raz)
  kp++;

if(kp != 0)
 kp++;
return(kp);
}
