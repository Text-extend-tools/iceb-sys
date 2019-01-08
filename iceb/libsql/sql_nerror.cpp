/* $Id: sql_nerror.c,v 5.4 2013/05/17 14:56:37 sasa Exp $ */
/*12.01.2000	12.01.2000	Белых А.И.	sql_nerror.c
Выдача номера ошибки
*/
#include	"libsql.h"

unsigned int	sql_nerror(SQL_baza *bd)
{
return(mysql_errno(bd));
}
