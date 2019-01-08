/* $Id: sumprpd.c,v 5.7 2013/05/17 14:56:12 sasa Exp $ */
/*17.02.2009	29.05.1998	Белых А.И.	sumprpd.c
Определение суммы выполненных проводок для определенного
платежного документа.
*/
#include        <math.h>
#include        "buhg.h"


double sumprpd(short d,short m,short g,const char *nn,int pod,
const char *kto,const char *shet)
{
double		sum;
long		kolstr;
SQL_str         row;
char		strsql[512];

/*Суммиреум выполненные проводки*/
sprintf(strsql,"select deb from Prov \
where kto='%s' and pod=%d and nomd='%s' and datd='%d-%02d-%02d' and \
shk='%s'",kto,pod,nn,g,m,d,shet);

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(0.);
 }

if(kolstr == 0)
  return(0.);

sum=0.;

while(cur.read_cursor(&row) != 0)
  sum+=atof(row[0]);


return(sum);

}
