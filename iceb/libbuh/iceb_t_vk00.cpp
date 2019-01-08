/*$Id: iceb_t_vk00.c,v 5.2 2013/05/17 14:56:29 sasa Exp $*/
/*31.05.2012	31.05.2012	Белых А.И.	iceb_t_vk00.c
Ввод кода контрагента реквизиты которого должны братся в распечатки документов
*/

#include "buhl.h"

void iceb_t_vk00(class iceb_tu_str *k00)
{
class VVOD VVOD1(2);

VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код контрагента с вашими реквизитами"));

vvod1(k00,16,&VVOD1,NULL,stdscr,-1,-1);

char strsql[512];
sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",k00->ravno());
if(readkey(strsql) != 1)
 {
  sprintf(strsql,"%s %s %s!",gettext("Нет контрагента"),k00->ravno(),gettext("в справочнике контрагентов"));
  iceb_t_soob(strsql);
  k00->new_plus("00");
 }


}
