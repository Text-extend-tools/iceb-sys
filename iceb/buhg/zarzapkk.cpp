/*$Id:$*/
/*08.06.2016	08.06.2016	Белых А.И.	zarzapkk.c
Запись кода контрагента в общий список контрагентов из подсистемы "Заработная плата"
если вернули 0 то есть
             1 нет
*/

#include "buhg.h"

int zarzapkk(const char *kod_kontr)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

//Проверяем есть ли в общем списке
sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",iceb_tu_sqlfiltr(kod_kontr));
if(readkey(strsql) >= 1)
 return(0);

//Узнаём фамилию и другие реквизиты


class iceb_tu_str fio("");

class iceb_tu_str tabn;
tabn.plus(iceb_tu_adrsimv(iceb_tu_strlen(ICEB_ZAR_PKTN),kod_kontr));

class iceb_tu_str adres("");

class iceb_tu_str telef("");

class iceb_tu_str inn("");

sprintf(strsql,"select fio,adres,telef,inn from Kartb where tabn=%s",tabn.ravno());
if(readkey(strsql,&row,&cur) == 1)
 {
  fio.new_plus(row[0]);

  adres.new_plus(row[1]);

  telef.new_plus(row[2]);

  inn.new_plus(row[3]);
 }

sprintf(strsql,"replace into Kontragent \
(kodkon,naikon,adres,innn,telef,ktoz,vrem) \
values ('%s','%s','%s','%s','%s',%d,%ld)",iceb_tu_sqlfiltr(kod_kontr),fio.ravno_filtr(),adres.ravno_filtr(),inn.ravno(),telef.ravno_filtr(),iceb_t_getuid(),time(NULL));

if(sql_zapis(strsql,0,0) != 0)
  return(1);
 

return(0);
}
