/*$Id: iceb_t_poldan_vkl.c,v 5.4 2013/05/17 14:56:29 sasa Exp $*/
/*14.04.2011	14.04.2011	Белых А.И..	iceb_t_poldan_vkl.c
Определение включения настройки 0-включено
                                1-нет
*/
#include "buhl.h"

int iceb_t_poldan_vkl(const char *strpoi,const char *imaf)
{
class iceb_tu_str vkl("");
int voz=1;
if(iceb_t_poldan(strpoi,&vkl,imaf) == 0)
 {
  if(SRAV("Вкл",vkl.ravno(),1) == 0)
   voz=0;
 }
return(voz);

}
