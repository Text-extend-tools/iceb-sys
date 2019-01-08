/*$Id: iceb_t_00_skl.c,v 5.3 2013-04-07 10:22:54 sasa Exp $*/
/*27.03.2013	29.07.2004	Белых А.И.	iceb_t_00_skl.c
Получениe кода склада из кода контрагента
Возвращает код склада
*/
#include "buhl.h"

int iceb_t_00_skl(const char *kontr)
{

if(kontr[0] == '0' && kontr[1] == '0' && kontr[2] == '-')
 {
  class iceb_tu_str sklad("");
  polen(kontr,&sklad,2,'-');
  return(sklad.ravno_atoi());  
 }

return(0);

}
