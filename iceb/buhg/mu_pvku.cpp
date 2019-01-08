/*$Id:$*/
/*06.02.2017	06.02.2017	Белых А.И.	mu_pvku.cpp
проверка возможности корректировки или удаления в подсистеме "Материальный учёт"

может удалять и корректировать только тот кто сделал запись
или тот кто имеет полномочия на блокировку/разблокировку подсистемы "Материальный учёт"
Возвращаем 0-можно
           1-нельзя

*/
#include "buhg.h"

int mu_pvku(int kodop_v_prov)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select mub from icebuser where login='%s'",iceb_tu_getlogin());

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
    iceb_t_soob(gettext("Удалять и корректировать записи может тот кто их сделал\nили тот кому разрешена блокировка/разблокировка мат. учёта!"));
    return(1);
   }  
 }
return(0);
}
