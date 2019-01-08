/*$Id: iceb_t_getfioop.c,v 5.4 2013/09/26 09:43:54 sasa Exp $*/
/*18.09.2013	07.09.2010	Белых А.И.	iceb_t_getfioop.c
Получение фамилии оператора
*/

#include "buhl.h"
extern class iceb_tu_str login_op; /*логин оператора*/

const char *iceb_t_getfioop()
{
static iceb_tu_str fio("");

if(fio.getdlinna() <= 1)
 {
  SQL_str row;
  class SQLCURSOR cur;
  char strsql[512];
//  sprintf(strsql,"select fio from icebuser where login='%s'",iceb_tu_getlogin());
  sprintf(strsql,"select fio from icebuser where login='%s'",login_op.ravno());

  if(readkey(strsql,&row,&cur) == 1)
   fio.new_plus(row[0]);
//  printw("\n%s-%s\n%s\n",__FUNCTION__,strsql,fio.ravno());
//  OSTANOV();
 } 
return(fio.ravno());

}
