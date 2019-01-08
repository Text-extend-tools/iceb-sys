/* $Id: rassocp.c,v 5.10 2013/08/13 05:49:53 sasa Exp $ */
/*20.06.2002	04.04.2000	Белых А.И.	rassocp.c
Распечатка на экран сумм социальных перечислений по конкретному
табельному номеру за конкретный месяц
*/
#include        <math.h>
#include        "buhg.h"


void rassocp(short mp,short gp,int tabn,const char *fio)
{
long		kolstr;
SQL_str         row,row1;
char		strsql[512];
class iceb_tu_str naim("");
double		suma,sumab;
clear();
printw("%d.%d %d %s\n",mp,gp,tabn,fio);
GDITE();

sprintf(strsql,"select kodz,shet,sumas,sumap,proc,ktoz,sumapb from Zarsocz \
where datz='%04d-%02d-01' and tabn=%d",gp,mp,tabn);
SQLCURSOR cur;
SQLCURSOR curr;
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
sumab=suma=0.;
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"select naik from Zarsoc where kod=%s",row[0]);
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
   naim.new_plus(row1[0]);
  else
   naim.new_plus("");
   
  printw("%s %-*.*s %-*s %7s %6s %5s%% %6s %s\n",
  row[0],
  iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
  iceb_tu_kolbait(10,row[1]),row[1],
  row[2],row[3],row[4],row[6],row[5]);
  suma+=atof(row[3]);
  sumab+=atof(row[6]);
 }
printw("%*s:%6.2f%8s%6.2f\n",iceb_tu_kolbait(51,gettext("Итого")),gettext("Итого"),suma," ",sumab);

OSTANOV();
}
