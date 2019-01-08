/*$Id: iceb_t_getuid.c,v 5.4 2013/09/26 09:43:55 sasa Exp $*/
/*04.09.2013	04.09.2010	Белых А.И.	iceb_t_getuid.c
Получение номера оператора
*/
#include "buhl.h"

extern class iceb_tu_str login_op; /*логин оператора*/

int iceb_t_getuid()
{
static int nom_op=0;

if(nom_op == 0)
 {
  char strsql[512];
  SQL_str row;
  class SQLCURSOR cur;
//  sprintf(strsql,"select un from icebuser where login='%s'",iceb_tu_getlogin());
  sprintf(strsql,"select un from icebuser where login='%s'",login_op.ravno());
  if(readkey(strsql,&row,&cur) == 1)
   nom_op=atoi(row[0]);
 } 
return(nom_op);

}
