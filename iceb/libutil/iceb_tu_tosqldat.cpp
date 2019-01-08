/*$Id: iceb_tu_tosqldat.c,v 5.3 2013/05/17 14:56:41 sasa Exp $*/
/*10.11.2011	10.11.2011	Белых А.И.	iceb_tu_tosqldat.c
Преобразование даты в sql формат
*/
#include "util.h"

const char *iceb_tu_tosqldat(const char *data)
{
static char stroka[64];
memset(stroka,'\0',sizeof(stroka));

short d=0,m=0,g=0;


if(rsdat(&d,&m,&g,data,1) != 0)
 {
  if(rsdat1(&m,&g,data) != 0)
   strcpy(stroka,"0000-00-00"); 
  else
   sprintf(stroka,"%04d.%02d.01",g,m);
 }
else
 sprintf(stroka,"%04d.%02d.%02d",g,m,d);

return(stroka);

}
