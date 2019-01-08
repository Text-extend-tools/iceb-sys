/*$Id: iceb_t_cat.c,v 5.3 2013/05/17 14:56:28 sasa Exp $*/
/*23.09.2009	23.09.2009	Белых А.И.	iceb_t_cat.c
Объединение двух файлов
*/
#include <errno.h>
#include "buhl.h"

int iceb_t_cat(const char *imaf1,const char *imaf2)
{
FILE *ff_1;
FILE *ff_2;
if((ff_1 = fopen(imaf1,"a")) == NULL)
 {
  error_op_nfil(imaf1,errno,"iceb_t_cat");
  return(1);
 }

if((ff_2 = fopen(imaf2,"r")) == NULL)
 {
  error_op_nfil(imaf2,errno,"iceb_t_cat");
  return(1);
 }

int i=0;
while((i=fgetc(ff_2)) != EOF)
  fputc(i, ff_1);  

fclose(ff_2);

fclose(ff_1);
return(0);
}