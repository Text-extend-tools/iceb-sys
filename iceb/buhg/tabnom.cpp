/* $Id: tabnom.c,v 5.5 2013/05/17 14:56:13 sasa Exp $ */
/*23.06.2001	07.06.1999	Белых А.И.	tabnom.c
Определить свободный табельный номер
*/
#include        "buhg.h"

int		tabnom()
{
int             i;
char		strsql[100];
SQL_str         row;
long		kolstr;

GDITE();

sprintf(strsql,"select tabn from Kartb order by tabn asc");
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  printw("%s\n",sql_error(&bd));
  OSTANOV();
 }

i=0;
while(cur.read_cursor(&row) != 0)
 {
  i++;
  if(i != atoi(row[0]))
    return(i);
 }

i++;

return(i);

}
