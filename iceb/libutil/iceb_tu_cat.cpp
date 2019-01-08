/*$Id: iceb_tu_cat.c,v 5.3 2013/05/17 14:56:40 sasa Exp $*/
/*23.09.2009	23.09.2009	Белых А.И.	iceb_tu_cat.c
Объединение двух файлов в один
*/

#include <errno.h>
#include <stdio.h>
int iceb_tu_cat(const char *imaf1,const char *imaf2)
{
FILE *ff_1;
FILE *ff_2;
if((ff_1 = fopen(imaf1,"a")) == NULL)
 {
//  printf("Ошибка открытия файла %s: %d!\n",imaf1,errno);
  return(errno);
 }

if((ff_2 = fopen(imaf2,"r")) == NULL)
 {
//  printf("Ошибка открытия файла %s: %d!\n",imaf2,errno);
  return(errno);
 }

int i=0;
while((i=fgetc(ff_2)) != EOF)
  fputc(i, ff_1);  

fclose(ff_2);

fclose(ff_1);
return(0);

}
