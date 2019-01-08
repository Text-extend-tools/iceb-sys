/*$Id: iceb_t_fopen.c,v 5.3 2013/05/17 14:56:28 sasa Exp $*/
/*12.03.2013	12.03.2013	Белых А.И.	iceb_t_fopen.c
Отрыть файл 
*/
#include <errno.h>
#include "buhl.h"
int iceb_fopen::start(const char *imaf,const char *wr)
{
 if((ff =fopen(imaf,wr)) == NULL)
  {
   error_op_nfil(imaf,errno,"");
   return(1);
  }
return(0);
}
