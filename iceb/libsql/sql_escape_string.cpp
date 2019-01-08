/*$Id: sql_escape_string.c,v 5.3 2013/05/17 14:56:37 sasa Exp $*/
/*26.11.2011	26.11.2011	Белых А.И.	sql_escape_string.c
Получение отфильтрованного запроса
*/
#include <string.h>
#include "libsql.h"

const char *sql_escape_string(const char *str_from,int razmer) /*размер str_from*/
{
static char *str_out=NULL;

if(str_out != NULL)
 delete [] str_out;
 
if(razmer <= 0)
 razmer=strlen(str_from);

int razmer_str_out=razmer*2+1;

str_out=new char[razmer_str_out];

memset(str_out,'\0',razmer_str_out);

mysql_escape_string(str_out,str_from,razmer);
/*Есть ещё функция mysql_real_escape_string в качестве первого параметра индетефикатор базы*/
return(str_out);

}