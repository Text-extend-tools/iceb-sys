/* $Id: sql_zap.c,v 5.6 2013/05/17 14:56:38 sasa Exp $ */
/*09.11.2008	26.01.1999	Белых А.И.	sql_zap.c
Запрос к базе данных
*/

#include	"libsql.h"

int sql_zap(SQL_baza *bd,const char *zapros)
{
int		ll;

if((ll=mysql_query(bd,zapros)) != 0)
 {
/*
  printw("ll=%d 111111\n",ll);
  refresh();
*/
  return(ll);
 }
return(0);
}
