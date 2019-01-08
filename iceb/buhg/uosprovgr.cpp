/*$Id: uosprovgr.c,v 5.5 2013/05/17 14:56:14 sasa Exp $*/
/*12.04.2009	12.04.2009	Белых А.И.	uosprovgr.c
Проверка группы на тип амортизации
Если вернули 0 - амортизация пообъектная
             1 - в целом по группе

*/
#include "buhg.h"

int uosprovgr(const char *kodgr)
{
int ta=-1;
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select ta from Uosgrup where kod='%s'",kodgr);
if(readkey(strsql,&row,&cur) > 0)
  ta=atoi(row[0]);

return(ta);   

}
