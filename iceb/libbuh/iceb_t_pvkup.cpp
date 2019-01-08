/*$Id:$*/
/*12.02.2016	12.02.2016	Белых А.И.	iceb_t_pvkup.c

проверка возможности корректировки или удаления проводки

Проводку может удалять и корректировать только тот кто её сделал
или тот кто имеет полномочия на блокировку/разблокировку подсистемы
"Главная книга"
Возвращаем 0-можно
           1-нельзя

*/
#include "buhl.h"

int iceb_t_pvkup(int kodop_v_prov)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select gkb from icebuser where login='%s'",iceb_tu_getlogin());

if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s-%s\n%s %s",
  __FUNCTION__,
  gettext("У Вас нет полномочий для выполнения этой операции!"),
  gettext("Ненайден логин"),
  iceb_tu_getlogin());

  iceb_t_soob(strsql);

  return(1);
  
 }

if(atoi(row[0]) == 1)
 {
  if(kodop_v_prov != iceb_t_getuid())
   {
    iceb_t_soob(gettext("Удалять и корректировать проводки может тот кто её сделал\nили тот кому разрешена блокировка/разблокировка проводок!"));
    return(1);
   }  
 }
return(0);
}