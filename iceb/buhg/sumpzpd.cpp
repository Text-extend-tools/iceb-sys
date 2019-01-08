/* $Id: sumpzpd.c,v 5.7 2013-01-02 10:38:36 sasa Exp $ */
/*13.04.2006	28.05.1998	Белых А.И.	sumpzpd.c
Определение общей суммы в подтверждающих записях
к платежным документам
*/
#include        <math.h>
#include        "buhg.h"
#include        "dok4.h"

extern class REC rec;

double sumpzpd(const char *tabl,short *dp,short *mp,short *gp,
double *sumpp) //Сумма последнего подтверждения
{
double		sump;
long		kolstr;
SQL_str         row;
char		strsql[512];
short		d,m,g;

*dp=*mp=*gp=0;
*sumpp=0.;

/*Определяем сумму подтверждения документа*/
if(SRAV(tabl,"Pltp",0) == 0)
 sprintf(strsql,"select datp,suma from Pzpd where tp=0 and \
datd='%d-%02d-%02d' and nomd='%s' order by datp asc",rec.gd,rec.md,rec.dd,rec.nomdk.ravno());

if(SRAV(tabl,"Pltt",0) == 0)
 sprintf(strsql,"select datp,suma from Pzpd where tp=1 and \
datd='%d-%02d-%02d' and nomd='%s' order by datp asc",rec.gd,rec.md,rec.dd,rec.nomdk.ravno());
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(0.);
 }

if(kolstr == 0)
  return(0.);


sump=0.;

while(cur.read_cursor(&row) != 0)
 {
  rsdat(&d,&m,&g,row[0],2);
  *dp=d; *mp=m; *gp=g;
  *sumpp=atof(row[1]);
  sump+=atof(row[1]);
 }
return(sump);
}
