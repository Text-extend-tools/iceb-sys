/* $Id: provpodko.c,v 5.10 2013/05/17 14:56:08 sasa Exp $ */
/*28.04.2013	29.09.2000	Белых А.И.	provpodkof.c
Проверка подтверждения всех записей в кассовом ордере
*/
#include "bezcurses.h"


void	provpodko(const char *kassa,const char *nomd,short dd,short md,short gd,short tipz)
{
char		strsql[512];
int		kolstr;

sprintf(strsql,"select datp from Kasord1 where datp='0000-00-00' \
and kassa=%s and nomd='%s' and tp=%d and datd='%d-%d-%d'",
kassa,nomd,tipz,gd,md,dd);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return;
 }

if(kolstr != 0)
  sprintf(strsql,"update Kasord set podt=0 where kassa=%s and tp=%d \
and nomd='%s' and god=%d",kassa,tipz,nomd,gd);
else 
  sprintf(strsql,"update Kasord set podt=1 where kassa=%s and tp=%d \
and nomd='%s' and god=%d",kassa,tipz,nomd,gd);

if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_DBACCESS_DENIED_ERROR)
  {
   msql_error(&bd,__FUNCTION__,strsql);
  }
}
