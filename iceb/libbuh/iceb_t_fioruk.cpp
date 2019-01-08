/*$Id: iceb_t_fioruk.c,v 5.9 2013-01-15 10:18:02 sasa Exp $*/
/*18.03.2016	04.04.2008	Белых А.И.	iceb_t_fioruk.c
Получение фамилий главного бухгалтера и руководителя предприятия
Если вернули 0-нашли
             1-нет
*/
#include "buhl.h"



int iceb_t_fioruk(int metka, /*1-ищем фамилию руководителя 2-главного бухгалтера 3-выплачивающего зарплату 4-проверяющего ведомости на зарплату*/
class iceb_t_fioruk_rk *rek)
{

rek->clear();

class iceb_tu_str tabnom("");

if(metka == 1)
  iceb_t_poldan("Табельный номер руководителя",&tabnom,"zarnast.alx");
if(metka == 2)
  iceb_t_poldan("Табельный номер бухгалтера",&tabnom,"zarnast.alx");

if(metka == 3)
  iceb_t_poldan("Табельный номер выплачивающего зарплату",&tabnom,"zarnast.alx");
if(metka == 4)
  iceb_t_poldan("Табельный номер проверяющего ведомости на зарплату",&tabnom,"zarnast.alx");

return(iceb_t_get_rtn(tabnom.ravno_atoi(),rek));


}
