/*$Id: uplztf.c,v 5.2 2013/05/17 14:56:15 sasa Exp $*/
/*04.05.2012	13.04.2012	Белых А.И.	uplztf.c
Получение фактических затрат и по норме топлива по путевому листу
*/

#include "buhg.h"

double uplztf(short dd,short md,short gd,int podr,const char *nom_dok,double *ztpn)
{
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
char strsql[1024];
double ztf=0.;
*ztpn=0.;
sprintf(strsql,"select zn,zf from Upldok3 where datd='%04d-%02d-%02d' and kp=%d and nomd='%s'",gd,md,dd,podr,nom_dok);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(0.);
 }
if(kolstr == 0)
 return(0.);
while(cur.read_cursor(&row) != 0)
 {
  *ztpn+=atof(row[0]);
  ztf+=atof(row[1]);
 }

return(ztf);
}
/*****************************************/

double uplztf(short dd,short md,short gd,int podr,const char *nom_dok)
{
double ztpn=0.;
return(uplztf(dd,md,gd,podr,nom_dok,&ztpn));
}
