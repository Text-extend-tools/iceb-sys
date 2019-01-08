/*$Id: rnngods.c,v 5.1 2012-11-29 09:48:57 sasa Exp $*/
/*17.11.2012	17.11.2012	Белых А.И.	rnngods.c
Получение списка годов за которые введена информация
*/

#include "buhg.h"
void rnngods()
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
int kolstr=0;
int kolstr1=0;
short d,m,g;
short god=0;

clear();
printw("%s\n%s\n",gettext("Список годов за которые введены записи"),gettext("Реестр выданных налоговых накладных"));
GDITE();

sprintf(strsql,"select distinct datd from Reenn");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  printw("%s\n",gettext("Не найдено ни одной записи !"));
 }

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  rsdat(&d,&m,&g,row[0],2);
  if(god != g)
   {
    printw("%d\n",g);
    god=g;
   }
 }


printw("\n%s\n",gettext("Реестр полученных налоговых накладных"));
GDITE();
sprintf(strsql,"select distinct datd from Reenn1");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  printw("%s\n",gettext("Не найдено ни одной записи !"));
 }
kolstr1=0;

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  rsdat(&d,&m,&g,row[0],2);
  if(god != g)
   {
    printw("%d\n",g);
    god=g;
   }
 }


OSTANOV();
}
