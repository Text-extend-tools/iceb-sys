/*$Id: readkey.c,v 5.7 2013/05/17 14:56:31 sasa Exp $*/
/*11.04.2012	20.03.2005	Белых А.И.	readkey.c
Чтение записи по ключу в базе данных
*/
#include "buhl.h"

int readkey(const char *zapros,SQL_str *row,SQLCURSOR *cur,WINDOW *win)
{

int voz=0;
if((voz=sql_readkey(&bd,zapros,row,cur)) < 0)
    msql_error(&bd,__FUNCTION__,zapros,win);

return(voz);
}

int readkey(char *zapros,WINDOW *win)
{
SQL_str row;
SQLCURSOR cur;
int voz=0;
if((voz=sql_readkey(&bd,zapros,&row,&cur)) < 0)
    msql_error(&bd,__FUNCTION__,zapros,win);

return(voz);
}
/**********************************/
int readkey(const char *zapros,SQL_str *row,SQLCURSOR *cur)
{
return(readkey(zapros,row,cur,stdscr));
}
/****************************/
int readkey(char *zapros)
{
return(readkey(zapros,stdscr));
}
