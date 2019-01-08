/*$Id: zaravpt_os.c,v 5.3 2013/05/17 14:56:17 sasa Exp $*/
/*03.04.2015	10.03.2011	Белых А.И.	zaravpt_os.c
Определение сумм соц. отчислений
Программа формирования масивов объединенных одинаковых строк
Если строк несколько то при выполнении проводок по каждой строке
возникает погрешность при округлении до копеек . Если одинаковые строки объединить
то погрешность будет меньше.
*/

#include "buhg.h"

int zaravpt_os(int tbn,short mp,short gp,SPISOK *data,masiv_din_double *suma)
{
SQL_str row1;
int kolstr2;
char strsql[1024];

SQLCURSOR cur1;

sprintf(strsql,"select datz,suma,shet,knah,kom from Zarp where \
datz >='%04d-%02d-01' and datz <= '%04d-%02d-31' and tabn=%d and \
suma <> 0. and prn='1' order by suma asc",
gp,mp,gp,mp,tbn);

if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
if(kolstr2 == 0)
  return(1);
int nomer=0;
while(cur1.read_cursor(&row1) != 0)
 {
  sprintf(strsql,"%s|%s|%s|%s|",row1[0],row1[2],row1[3],row1[4]);
  if((nomer=data->find(strsql)) < 0)
     data->plus(strsql);
  
  suma->plus(atof(row1[1]),nomer);
 }
 
return(0);
}
