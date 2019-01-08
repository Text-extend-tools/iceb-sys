/* $Id: sumkasor.c,v 5.8 2013/05/17 14:56:12 sasa Exp $ */
/*09.11.2008	05.09.2000	Белых А.И.	sumkasor.c
Вычисление суммы по кассовому оpдеpу
Если веpнули 0-записи в документе есть
             1-нет
*/
#include        "buhg.h"


int sumkasor(const char kassa[],short tipz,const char nomd[],short god,
double *sumd,double *sumpod)
{
char		strsql[512];
int		kolstr;
SQL_str		row;

*sumd=*sumpod=0.;

sprintf(strsql,"select datp,suma from Kasord1 where \
kassa=%s and nomd='%s' and tp=%d and god=%d",
kassa,nomd,tipz,god);
//printw("sumkasor=%s\n",strsql);
SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

if(kolstr <= 0)
  return(1);

while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s\n",row[0],row[1]);
  
  *sumd+=atof(row[1]);
  if(row[0][0] == '0')
    continue;
  *sumpod+=atof(row[1]);
 }
//OSTANOV();
return(0);

}
