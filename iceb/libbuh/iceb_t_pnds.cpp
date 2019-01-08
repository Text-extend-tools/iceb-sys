/*$Id: iceb_t_pnds.c,v 5.4 2013-01-02 10:38:43 sasa Exp $*/
/*29.12.2012	14.10.2010	Белых А.И.	iceb_t_pnds.c
Получение действующего процента НДС по дате
*/
#include "buhl.h"
double iceb_t_pnds(short d,short m,short g)
{
SQL_str row;
class SQLCURSOR cur;
int kolstr=0;

char strsql[512];
if(d != 0)
 sprintf(strsql,"select pr from Nalog where vn=0 and dndn <= '%04d-%02d-%02d' order by dndn desc limit 1",g,m,d);
else
 sprintf(strsql,"select pr from Nalog where vn=0 order by dndn desc limit 1");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,"",strsql);
  return(1);
 }

if(kolstr == 0)
 {
//  printw("\n%s\n",strsql);
  sprintf(strsql,"%s-%s",__FUNCTION__,gettext("Не нашли процент НДС!"));
  iceb_t_soob(strsql);
  return(0.);
 }
cur.read_cursor(&row);
return(atof(row[0]));
}
/**********************************/
double iceb_t_pnds(const char *data)
{
short d=0,m=0,g=0;
rsdat(&d,&m,&g,data,0);
return( iceb_t_pnds(d,m,g));

}
/**********************************/
double iceb_t_pnds()
{
short d=0,m=0,g=0;
poltekdat(&d,&m,&g);
return( iceb_t_pnds(d,m,g));

}
