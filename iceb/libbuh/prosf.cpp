/* $Id: prosf.c,v 5.10 2013/05/17 14:56:31 sasa Exp $ */
/*08.08.2010	01.03.1999	Белых А.И.	prosf.c
Просмотр файлов
*/
#include <errno.h>
#include "buhl.h"

/********************************/
void prosf(const char *imaf)
{
/*Проверяем возможность открытия файла*/
FILE *ff;

if((ff=fopen(imaf,"r")) == NULL)
 {
  error_op_nfil(imaf,errno,__FUNCTION__);
  return;
 }
fclose(ff);

class iceb_tu_str smotrfil;/*Программа для просмотра файлов*/
if(iceb_t_poldan("Просмотрщик файлов",&smotrfil,"nastsys.alx") != 0)
 return;

char		str[1024];

sprintf(str,"%s %s",smotrfil.ravno(),imaf);
runsystem(str);
}
