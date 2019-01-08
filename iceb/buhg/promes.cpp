/* $Id: promes.c,v 5.10 2013/08/13 05:49:51 sasa Exp $ */
/*24.05.2013    14.03.1993      Белых А.И.      promes.c
Получение распечатки начислений или удержаний за прошлый месяц
*/
#include        "buhg.h"


void            promes(short m,short g,short kd,long tn)
{
class iceb_tu_str naim("");
long		kolstr;
SQL_str         row,row1;
char		strsql[512];

clear();

m--;
if(m == 0)
 {
  m=12;
  g--;
 }

sprintf(strsql,"select knah,mesn,godn,suma from Zarp where \
datz >='%d-%02d-01' and \
datz <= '%d-%02d-31' and tabn=%ld and prn=%d and suma <> 0.",
g,m,g,m,tn,kd);

SQLCURSOR cur;
SQLCURSOR cur1;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,gettext("Не найдено ни одной записи за %d.%dг. !"),m,g);
  iceb_t_soob(strsql);
  return;
 }

move(LINES-1,0);
printw("%s %d.%d%s %s %ld\n\
-----------------------------------------------------\n",
gettext("Дата"),m,g,
gettext("г."),
gettext("Табельный номер"),tn);

while(cur.read_cursor(&row) != 0)
 {
  if(kd == 1)
    sprintf(strsql,"select naik from Nash where kod=%s",row[0]);
  if(kd == 2)
    sprintf(strsql,"select naik from Uder where kod=%s",row[0]);
     
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    beep();
    if(kd == 1)
     printw("%s %s !\n",gettext("Не найден код начисления"),row[0]);
    if(kd == 2)
     printw("%s %s !\n",gettext("Не найден код удержания"),row[0]);
    OSTANOV();
    naim.new_plus("");
   }
  else
    naim.new_plus(row1[0]);

  printw("%2s %-*.*s %2s.%4s %.2f\n",
  row[0],
  iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
  row[1],
  row[2],
  atof(row[3]));
 }
printw("\n\n");
OSTANOV();
}
