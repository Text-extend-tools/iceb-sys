/* $Id: pbkas.c,v 5.9 2013/05/17 14:56:06 sasa Exp $ */
/*17.1.2008	28.09.2000	Белых А.И.	pbkas.c
Проверка блокировки даты в Учете кассовых ордеров
Если вернули 0 дата не заблокирована
             1 дата заблокирована
*/
#include        "buhg.h"


int pbkas(short md,short gd)
{
if(iceb_t_pbpds(md,gd) != 0)
 return(1);
return(0);
}
