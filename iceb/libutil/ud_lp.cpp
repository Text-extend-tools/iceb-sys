/*$Id: ud_lp.c,v 5.6 2013/08/19 06:44:41 sasa Exp $*/
/*18.08.2013	15.03.2005	Белых А.И.	ud_lp.c
Удаление в строке лидирующих пробелов
*/
#include <string.h>
void ud_lp(char *stroka)
{
if(stroka[0] != ' ')
 return;
int razmer=strlen(stroka)+1;
char str1[razmer];
memset(str1,'\0',razmer);
int sh=0;
for(int i=0; i < razmer;i++)
 {
  if(sh == 0)  
   if(stroka[i] == ' ')
    continue;
  str1[sh++]=stroka[i];
 }
strcpy(stroka,str1);
}
