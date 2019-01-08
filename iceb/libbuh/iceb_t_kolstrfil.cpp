/*$Id: iceb_t_kolstrfil.c,v 5.6 2013/05/17 14:56:28 sasa Exp $*/
/*09.11.2008	20.03.2007	Белых А.И.	iceb_tu_kolstrfil.c
Подпрограмма возвращает количество строк в файле 
Если файл не обнаружен возвращает -1
        
*/
#include <errno.h>
#include "buhl.h"

int iceb_t_kolstrfil(const char *imaf)
{
char stroka[1024];
FILE *ff;
int kolstr=0;
if((ff=fopen(imaf,"r")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(-1);
 }
while(fgets(stroka,sizeof(stroka),ff) != NULL)
 kolstr++; 
fclose(ff);

return(kolstr);
}
