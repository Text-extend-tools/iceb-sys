/*$Id:$*/
/*16.03.2017	16.03.2017	Белых А.И.	usl_pvku.cpp
проверка возможности корректировки или удаления в подсистеме "Учёт услуг"

может удалять и корректировать только тот кто сделал запись
или тот кто имеет полномочия на блокировку/разблокировку подсистемы "Учёт услуг"
Возвращаем 0-можно
           1-нельзя

*/
#include "buhg.h"

int usl_pvku(int kodop_v_prov)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select uub from icebuser where login='%s'",iceb_tu_getlogin());

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
    iceb_t_soob(gettext("Удалять и корректировать записи может тот кто их сделал\nили тот кому разрешена блокировка/разблокировка учёта услуг!"));
    return(1);
   }  
 }
return(0);
}
