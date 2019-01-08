/*$Id: readkey.c,v 5.7 2013/05/17 14:56:31 sasa Exp $*/
/*11.04.2012	20.03.2005	Белых А.И.	readkeyf.c
Чтение записи по ключу в базе данных
*/
#include "bezcurses.h"

int readkey(const char *zapros,SQL_str *row,SQLCURSOR *cur)
{
int voz=0;
if((voz=sql_readkey(&bd,zapros,row,cur)) < 0)
 {
  printf("%d %s\n",sql_nerror(&bd),sql_error(&bd));

  printf("%s\n",zapros);
 }
return(voz);
}
/****************************/
int readkey(char *zapros)
{
SQL_str row;
class SQLCURSOR cur;
return(readkey(zapros,&row,&cur));
}
