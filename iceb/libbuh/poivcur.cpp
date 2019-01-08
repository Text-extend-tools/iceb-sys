/* $Id: poivcur.c,v 5.8 2013/05/17 14:56:31 sasa Exp $ */
/*27.11.2009	17.03.1999	Белых А.И.	poivcur.c
Поиск в курсоре нужной строки
Возвращяет номер найденной строки
*/
#include        "buhl.h"


int poivcur(const char *kod,int nb,SQLCURSOR *cur)
{
SQL_str         row;
int		pozz;

cur->poz_cursor(0);
pozz=0;
while(cur->read_cursor(&row) != 0)
 {
//  printw("poivcur-%s\n",row[nb]);
//  refresh();
  if(SRAV(row[nb],kod,0) == 0)
   {
//  printw("poivcur-подошел-%s\n",row[nb]);
//  refresh();
    return(pozz);        
   }
  pozz++;
 }
return(pozz);
}
