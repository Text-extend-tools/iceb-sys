/*$Id: iceb_tu_cp.c,v 5.3 2013/05/17 14:56:40 sasa Exp $*/
/*24.09.2009	23.09.2009	Белых А.И.	iceb_tu_cp.c
Копирование файла
*/
#include <errno.h>
#include <stdio.h>
int iceb_tu_cp(const char *imaf_out,const char *imaf_in)
{
FILE *ff_out;
FILE *ff_in;
if((ff_in = fopen(imaf_in,"w")) == NULL)
 {
//  printf("Ошибка открытия файла %s: %d!\n",imaf_in,errno);
  return(errno);
 }

if((ff_out = fopen(imaf_out,"r")) == NULL)
 {
//  printf("Ошибка открытия файла %s: %d!\n",imaf_out,errno);
  return(errno);
 }
int i=0;
while((i=fgetc(ff_out)) != EOF)
  fputc(i, ff_in);  

fclose(ff_in);

fclose(ff_out);
return(0);
}
