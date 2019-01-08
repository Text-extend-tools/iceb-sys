/*$Id: iceb_t_cp.c,v 5.6 2013/05/17 14:56:28 sasa Exp $*/
/*15.02.2010	23.09.2009	Белых А.И.	iceb_t_cp.c
Копирование файла
Возвращаем 0- если файл скопирова
           1- если входной файл не смогли открыть
           2- если выходной файл не смогли открыть
           3- если коприрование с проверкой и такой файл там уже есть
*/
#include <sys/stat.h>
#include <errno.h>
#include "buhl.h"


int iceb_t_cp(const char *imaf_out,const char *imaf_in,int metka) /*0-всегда копировать 1-только если там его нет 2-тоже что и 1 только без сообщения*/
{
/*проверяем может файл там уже есть*/
if(metka > 0)
 {
  struct stat buff;
  if(stat(imaf_in,&buff) == 0) /*файл там уже есть*/
   {
    if(metka == 1)
     {
      class iceb_tu_str repl(gettext("Такой файл уже есть!"));
      repl.ps_plus(imaf_in);
      iceb_t_soob(repl.ravno());
     }
    return(3);
   }
 }

FILE *ff_out;
FILE *ff_in;
if((ff_in = fopen(imaf_in,"w")) == NULL)
 {
  error_op_nfil(imaf_in,errno,__FUNCTION__);
  return(2);
 }

if((ff_out = fopen(imaf_out,"r")) == NULL)
 {
  error_op_nfil(imaf_out,errno,__FUNCTION__);
  return(1);
 }
int i=0;
while((i=fgetc(ff_out)) != EOF)
  fputc(i, ff_in);  

fclose(ff_in);

fclose(ff_out);
return(0);
}
