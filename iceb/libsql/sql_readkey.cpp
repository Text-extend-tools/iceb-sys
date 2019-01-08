 /* $Id: sql_readkey.c,v 5.12 2013/05/17 14:56:38 sasa Exp $ */
/*28.05.2004	11.01.1999	Белых А.И.	sql_readkey.c
Получения записи по ключу
Возвращает количество записей в запросе
*/
#include	"libsql.h"

/*******************************************************/
/*******************************************************/
int sql_readkey(SQL_baza *bd,const char *zapros)
{
SQL_cursor      *cursor;
long		ll;
int      	kolstr;

if((ll=mysql_query(bd,zapros)) != 0)
 {
  if(mysql_errno(bd) != ER_NO_SUCH_TABLE)
   {
//    msql_error(bd,"sql_readkey",zapros);
   }
  return(-1);
 }

if((cursor=mysql_store_result(bd)) == NULL)
 {
//  msql_error(bd,"sql_readkey 2",zapros);
  return(-2);
 }
/*Определяем количество строк в курсоре*/
kolstr=mysql_num_rows(cursor);

mysql_free_result(cursor);

/*
printw("kolstr=%d\n",kolstr);
OSTANOV();
*/
return(kolstr);

}

/**********************************/
/***********************************/

int sql_readkey(SQL_baza *bd,const char *zapros,SQL_str *row,SQLCURSOR *cur)
{
int kolstr=0;

if((kolstr=cur->make_cursor(bd,zapros)) < 0)
 {
  return(-1);
 }

cur->read_cursor(row);

return(kolstr);

}
