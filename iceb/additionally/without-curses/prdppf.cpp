/* $Id: prdpp.c,v 5.6 2013-04-07 10:22:43 sasa Exp $ */
/*23.06.2001	09.02.1999	Белых А.И.	prdppf.c

Получение даты первого прихода в карточку
Если вернули 0 дату первого прихода нашли
             1-нет
*/

//#include        <pwd.h>
//#include        <time.h>
#include "bezcurses.h"

int prdpp(int skl,int nk,short *dp,short *mp,short *gp)
{
SQL_str         row;
char		strsql[512];
int		kolstr;
short		d,m,g;

*dp=0;
*mp=0;
*gp=0;

sprintf(strsql,"select datdp from Zkart where sklad=%d and nomk=%d and \
tipz=1 order by datdp asc",skl,nk);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return(1);
 }
if(kolstr == 0)
  return(1);
if(cur.read_cursor(&row) != 0)
 {
  rsdat(&d,&m,&g,row[0],2);
  *gp=g;
  *mp=m;
  *dp=d;
 }

return(0);

}
