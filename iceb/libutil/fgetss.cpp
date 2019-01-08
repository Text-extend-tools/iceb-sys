/*$Id: fgetss.c,v 5.5 2013/05/17 14:56:40 sasa Exp $*/
/*10.04.1991    10.04.1991      Белых А.И.      FGETSS.C
Программа получения строки из файла и замены
перевода строки на нулевой байт
*/
#include	"util.h"

int     FGETSS(char *STROKA,int DLI,FILE *FILL)
{
int             I;

if(fgets(STROKA,DLI,FILL) == NULL)
   return(0);
for(I=0;;I++)
 {
  if(STROKA[I] == '\n')
   {
    STROKA[I] = '\0';
    break;
   }
 }
return(1);

}
