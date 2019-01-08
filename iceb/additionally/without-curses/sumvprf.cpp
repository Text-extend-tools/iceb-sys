/*$Id: sumvpr.c,v 5.14 2013-04-07 10:22:55 sasa Exp $*/
/*24.11.2008	32.09.1998	Белых А.И.	sumvpr.c
Узнать сумму уже выполненых проводок
*/
#include        "bezcurses.h"


double sumvpr(const char *sh,const char *shk,
short d,short m,short g,const char *nn,int skk,const char *kom,time_t vrem,
int metkaprov,const char *kto)
{
double		suma=0.;
char		strsql[512];
SQL_str         row;
long		kolstr;

sprintf(strsql,"select deb from Prov where val=%d and kto='%s' and \
pod=%d and nomd='%s' and datd='%d-%02d-%02d' and sh='%s' and shk='%s' \
and vrem <> %ld and komen='%s' and deb <> 0.",metkaprov,
kto,
skk,nn,g,m,d,sh,shk,vrem,kom);

SQLCURSOR cur;  
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,"",strsql);
  return(0.);
 }

if(kolstr == 0)
  return(0.);

while(cur.read_cursor(&row) != 0)
 {
  suma+=atof(row[0]);

 }
suma=okrug(suma,0.01);
return(suma);

}
