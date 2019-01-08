/*$Id: uosprovar.c,v 5.5 2013/05/17 14:56:14 sasa Exp $*/
/*26.01.2010	13.04.2009	Белых А.И.	uosprvar.c
Получение алгоритма расчёта амортизации по группе
*/

#include "buhg.h"

int uosprovar(const char *kodgr,int metkau) /*0-налоговый учёт 1-бухучёт*/
{
int ar=-1;
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
if(metkau == 0)
  sprintf(strsql,"select ar from Uosgrup where kod='%s'",kodgr);
else
  sprintf(strsql,"select ar from Uosgrup1 where kod='%s'",kodgr);

if(readkey(strsql,&row,&cur) > 0)
  ar=atoi(row[0]);

return(ar);   

}
