/*$Id: iceb_t_kszap.c,v 5.4 2013/05/17 14:56:28 sasa Exp $*/
/*11.10.2010	27.09.2010	Белых А.И.	iceb_t_kszap.c
Формирование символной строки с номером и фамилией оператора
*/

#include "buhl.h"

const char *iceb_t_kszap(int kodop)
{
static char stroka[512];
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
sprintf(strsql,"select fio from icebuser where un=%d",kodop);

if(readkey(strsql,&row,&cur) == 1)
   sprintf(stroka,"%d %.*s",kodop,iceb_tu_kolbait(20,row[0]),row[0]);
else
  sprintf(stroka,"%d %s",kodop,gettext("Неизвестный логин"));

return(stroka);
}
/**********************************/
const char *iceb_t_kszap(const char *kodop)
{
if(kodop != NULL)
  return(iceb_t_kszap(atoi(kodop)));
else 
  return("NULL");
}
