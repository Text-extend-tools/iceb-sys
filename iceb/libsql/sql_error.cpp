/* $Id: sql_error.c,v 5.4 2013/05/17 14:56:37 sasa Exp $ */
/*29.09.2004	26.01.1999	Белых А.И.	sql_error.c
Выдача сообщения об ошибке
*/
#include	"libsql.h"

const char *sql_error(SQL_baza *bd)
{
return(mysql_error(bd));
}
