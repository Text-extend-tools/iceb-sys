/* $Id: findnah.c,v 5.11 2013/09/26 09:43:32 sasa Exp $ */
/*04.09.2006    10.12.1993      Белых А.И.      findnah.c
Поиск всех начислений на нужного человека за указанный
месяц и год
*/
#include        "buhg.h"

extern short    *knvr;/*Коды начислений не входящие в расчёт налогов*/
extern  short   *kodbl; /*Код больничного*/

double          findnah(long tnn, //Табельный номер
short m,short g) //Дата
{
double          nah;
long		kolstr;
SQL_str         row;
char		strsql[512];
short		knah;

//printw("findhah-%ld %d.%d\n",tnn,m,g);

sprintf(strsql,"select knah,suma,mesn from Zarp where \
datz >= '%d-%02d-01' and datz <= '%d-%02d-31' and tabn=%ld and prn='1'",
g,m,g,m,tnn);
class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(0);
 }

if(kolstr <= 0)
  return(0.);

nah=0.;

while(cur.read_cursor(&row) != 0)
 {

//  printw("%s %s %s\n",row[0],row[1],row[2]);
  
  /*Если больничный не за текущий месяц то в общюю сумму не включ.*/
  /*Для расчёта пенсионого и профсоюзного взносов включается*/
  knah=atoi(row[0]);
  if(provkod(kodbl,knah) >= 0 && atoi(row[2]) != m)
     continue;

  nah+=atof(row[1]);
 }
return(nah);
}
