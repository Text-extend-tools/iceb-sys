/*12.12.2013	03.10.2013	Белых А.И.	nastimp.c
Чтение настроек
*/
#include "bezcurses.h"

double		okrcn=.000001;  /*Округление цены*/
double		okrg1=.01;  /*Округление суммы*/
class iceb_tu_str spmatshets("");  /*Перечень материальных счетов*/
short		vtara=0; /*Код группы возвратная тара*/
class iceb_tu_str kodopsp(""); //Коды операций сторнирования для приходов
class iceb_tu_str kodopsr(""); //Коды операций сторнирования для расходов
short		startgod=0; /*Стартовый год просмотров в Материальном учёте*/

int nastimp()
{
int voz=0;

if(iceb_t_poldan("Стартовый год",&startgod,"matnast.alx") != 0)
  printf("%s-%s \"%s\"\n",__FUNCTION__,gettext("Не найдена настройка"),"Стартовый год");

if(iceb_t_poldan("Коды операций сторнирования для расходов",&kodopsr,"matnast.alx") != 0)
  printf("%s-%s \"%s\"\n",__FUNCTION__,gettext("Не найдена настройка"),"Коды операций сторнирования для расходов");

if(iceb_t_poldan("Коды операций сторнирования для приходов",&kodopsp,"matnast.alx") != 0)
  printf("%s-%s \"%s\"\n",__FUNCTION__,gettext("Не найдена настройка"),"Коды операций сторнирования для приходов");

if(iceb_t_poldan("Код группы возвратная тара",&vtara,"matnast.alx") != 0)
 {
  printf("%s-%s \"%s\"\n",__FUNCTION__,gettext("Не найдена настройка"),"Код группы возвратная тара");
 }
double bb=0.;

if(iceb_t_poldan("Округление цены",&bb,"matnast.alx") != 0)
 {
  printf("%s-%s \"%s\"\n",__FUNCTION__,gettext("Не найдена настройка"),"Округление цены");
 }
else
  okrcn=bb;
 
if(iceb_t_poldan("Округление суммы",&bb,"matnast.alx") != 0)
 {
  printf("%s-%s \"%s\"\n",__FUNCTION__,gettext("Не найдена настройка"),"Округление суммы");
 }
else
  okrg1=bb;

class iceb_tu_str bros("");
if(iceb_t_poldan("Перечень материальных счетов",&spmatshets,"matnast.alx") != 0)
 {
  printf("%s-%s \"%s\"\n",__FUNCTION__,gettext("Не найдена настройка"),"Перечень материальных счетов");
 }

if(okrcn == 0.)
 {
  printf("%s-Округление цены равно нолю!\n",__FUNCTION__);
  voz=1;  
 }
if(okrg1 == 0.)
 {
  printf("%s-Округление суммы равно нолю!\n",__FUNCTION__);
  voz=1;  
 }

return(voz);

}

