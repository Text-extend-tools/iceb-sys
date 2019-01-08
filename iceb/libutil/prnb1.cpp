/*$Id: prnb1.c,v 5.8 2013/05/17 14:56:42 sasa Exp $*/
/*13.02.1997	13.02.1997	Белых А.И. prnb1.c
Вставка запятой в число
*/
#include	"util.h"

void    prnb1(double nb, //Число для преобразования
char *nb1)  //Символьная строка с числом
{
short           i,i1;

sprintf(nb1,"%.2f",nb);
i1=strlen(nb1);

for(i=i1;i >=0 ;i--)
 {
  if(nb1[i] == '.')
   {
    nb1[i] = ',';
    break;
   } 
 }
} 
