/*$Id: xdkpg.c,v 5.1 2012-11-29 09:48:57 sasa Exp $*/
/*15.11.2012	15.11.2012	Белых А.И.	xdkpg.c
Получение списка годов за которые введены данные 
*/
#include "buhg.h"

void xdkpg()
{
char strsql[512];
int kolstr=0;
int kolstr1=0;
short d,m,g;
SQL_str row;
class SQLCURSOR cur;
short god;

clear();
GDITE();

sprintf(strsql,"select distinct datd from Pltp order by datd asc");
/*
printw("\n%s\n",strsql);
refresh();
*/
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,"",strsql);
 }
printw("%s\n",gettext("База данных платежных поручений"));

if(kolstr == 0)
 {
  printw("%s\n",gettext("Не найдено ни одной записи !"));
 }
else
 {
  kolstr1=god=0;
  while(cur.read_cursor(&row) != 0)
   {
    kolstr1++;
    strzag(LINES-1,0,kolstr,kolstr1);
    rsdat(&d,&m,&g,row[0],2);
    if(god != g)
     {
      printw("%d\n",g);
      god=g;
     }
   }
 }

printw("%s\n",gettext("База данных платежных требований"));
refresh();
sprintf(strsql,"select distinct datd from Pltt order by datd asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,"",strsql);
 }

if(kolstr == 0)
 {
  printw("%s\n",gettext("Не найдено ни одной записи !"));
 }
else
 {
  kolstr1=god=0;
  while(cur.read_cursor(&row) != 0)
   {
    kolstr1++;
    strzag(LINES-1,0,kolstr,kolstr1);
    rsdat(&d,&m,&g,row[0],2);
    if(god != g)
     {
      printw("%d\n",g);
      god=g;
     }
   }
 }

OSTANOV();

}
