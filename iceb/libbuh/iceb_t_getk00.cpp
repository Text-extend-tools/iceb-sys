/*$Id: iceb_t_getk00.c,v 5.3 2013/05/17 14:56:28 sasa Exp $*/
/*28.12.2011	28.12.2011	Белых А.И.	iceb_t_getk00.c
Получение кода контрагента для операции
*/
#include "buhl.h"

const char *iceb_t_getk00(int metka, /*0-мат-учёт 1-учёт услуг 2-учёт основных средств*/
const char *kodop)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
static iceb_tu_str kod_kontr("");

kod_kontr.new_plus("00");

if(metka == 0)
 sprintf(strsql,"select kk from Rashod where kod='%s'",kodop);
if(metka == 1)
 sprintf(strsql,"select kk from Usloper2 where kod='%s'",kodop);
if(metka == 2)
 sprintf(strsql,"select kk from Uosras where kod='%s'",kodop);

if(readkey(strsql,&row,&cur) == 1)
 kod_kontr.new_plus(row[0]);

return(kod_kontr.ravno());
}