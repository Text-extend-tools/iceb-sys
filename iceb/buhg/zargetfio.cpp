/*$Id: zargetfio.c,v 5.2 2013/05/17 14:56:17 sasa Exp $*/
/*28.04.2013	09.04.2013	Белых А.И.	zargetfio.c
Получение фамилии по табельному номеру
*/
#include "buhg.h"

const char *zargetfio(int tabnom)
{
char bros[128];
sprintf(bros,"%d",tabnom);
return(zargetfio(bros));
}
/***************************/
const char *zargetfio(const char *tabnom)
{
char strsql[512];
static class iceb_tu_str fio;
SQL_str row;
class SQLCURSOR cur;
fio.new_plus("");

sprintf(strsql,"select fio from Kartb where tabn=%s",tabnom);
if(readkey(strsql,&row,&cur) == 1)
 fio.new_plus(row[0]);


return(fio.ravno());



}
