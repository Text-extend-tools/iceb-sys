/*$Id: nalndoh.c,v 5.8 2013/08/13 05:49:49 sasa Exp $*/
/*19.05.2013	12.03.2003	Белых А.И.	nalndoh.c
Чтение настройки
Возвращаем 0-если параметр включен
           1-выключен
*/

#include        "buhg.h"


int	nalndoh(void)
{
class iceb_tu_str bros;

if(iceb_t_poldan("Предприятие является плательщиком налога на доход на общих основаниях",&bros,"matnast.alx") == 0)
 if(SRAV(bros.ravno(),"Вкл",1) != 0)
   return(1);  //Не является

return(0);

}
