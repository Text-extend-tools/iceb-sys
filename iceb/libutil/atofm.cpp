/*$Id: atofm.c,v 5.9 2013/05/17 14:56:40 sasa Exp $*/
/*21.10.2009  22.04.1987          Белых А.И.    ATOFM.C
Функция преобразования символьной строки в число
*/
#include	"util.h"

double ATOFM(const char *S)
{
int		l,I;
double		sum;

l = strlen(S);
char S1[l+1];

strcpy(S1,S);

for (I=0; I<l; I++) 
 {
  switch (S1[I]) 
    {
#if 0
#################333
//    case 'о':  /*Русские*/
    case 'О':
//    case 'o': /*Английские*/
    case 'O':
      S1[I]='0';
      break;
//    case 'з':  /*Русские*/
    case 'З':
      S1[I]='3';
      break;
##################333
#endif
    case ',':
      S1[I]='.';
      break;
    }
  
               
 }

sum=atof(S1);

return(sum);
}
