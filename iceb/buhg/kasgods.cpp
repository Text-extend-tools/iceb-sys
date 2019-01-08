/*$Id: kasgods.c,v 5.1 2012-11-29 09:48:57 sasa Exp $*/
/*13.11.2012	13.11.2012	Белых А.И.	kasgods.c
получение списка годов за которые введена информация
*/
#include "buhg.h"
void kasgods()
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
int kolstr=0;

sprintf(strsql,"select distinct god from Kasord order by god asc");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;
 }
printw("%s\n",gettext("Список годов за которые введены кассовые ордера"));
int kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  printw("%s\n",row[0]);
  refresh();
 }
OSTANOV();

}
