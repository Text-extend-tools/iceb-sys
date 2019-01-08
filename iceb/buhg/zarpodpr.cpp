/* $Id: zarpodpr.c,v 5.13 2013/08/13 05:50:02 sasa Exp $ */
/*28.06.2013	18.03.2000	Белых А.И.	zarpodpr.c
Поиск по коду подразделения процента выполнения плана
*/
#include        <errno.h>
#include        "buhg.h"


double		zarpodpr(int kodpr,//Код подразделения
double *procb)  //Процент выполнения плана бюджетной части
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
class iceb_tu_str bros("");
double		procent;

*procb=0.;
procent=100.;

sprintf(strsql,"select str from Alx where fil='zarpodpr.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarpodpr.alx");
  iceb_t_soob(strsql);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(polen(row_alx[0],&bros,1,'|') == 0)
   if(bros.ravno_atoi() == kodpr)
    {
     polen(row_alx[0],&procent,2,'|');
//     procent=ATOFM(bros);
     polen(bros.ravno(),procb,2,'/');
//     *procb=ATOFM(str);
//     printw("\nprocent=%f bros=%s\n",procent,bros);
     break;
    }
 }

return(procent);

}
