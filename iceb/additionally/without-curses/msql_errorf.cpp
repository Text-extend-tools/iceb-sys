/* $Id: msql_error.c,v 5.12 2013/05/17 14:56:31 sasa Exp $ */
/*18.05.2012	06.03.2001	Белых А.И.	msql_errorf.c
Меню вывода сообщения об ощибке базы данных
Две подпрограммы одна выводит на экран другая в файл
*/
#include        <sys/types.h>
#include        "bezcurses.h"


void	msql_error(SQL_baza *bd,const char *soobsh,const char *zapros)
{

if(soobsh[0] != '\0') 
 printf("%s\n",soobsh);

printf("%d %s\n",sql_nerror(bd),sql_error(bd));

printf("%s\n",zapros);

}

