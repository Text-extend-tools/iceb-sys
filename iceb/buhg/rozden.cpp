/* $Id: rozden.c,v 5.9 2013/09/26 09:43:39 sasa Exp $ */
/*15.07.2015    11.03.1993      Белых А.И.      rozden.c
Подпрограмма поиска дебета и кредита по счёту за определенный день
для нормального счета
*/
#include        "buhg.h"

extern short	vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/

void  rozden(const char *shh, //Счет
short d,short m,short g, //Дата
double *debet,
double *kredit,
short vds) //0-не полная проверка исходного счета 1-полная*/
{
long		kolstr;
SQL_str         row;
char		strsql[512];

/*Отчет по счёту*/
if(vds == 0 || vplsh == 1)
 sprintf(strsql,"select deb,kre from Prov \
where datp='%d-%02d-%02d' and sh like '%s%%' and val=0",
 g,m,d,shh);

/*отчёт по субсчету*/
//if(vds == 1 && vplsh == 0)
 sprintf(strsql,"select deb,kre from Prov USE INDEX (datp) \
where  datp='%d-%02d-%02d' and sh='%s' and val=0",
 g,m,d,shh);
/*
printw("%s\n",strsql);
refresh();
*/
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
  return;

while(cur.read_cursor(&row) != 0)
 {
  *debet+=atof(row[0]);
  *kredit+=atof(row[1]);
 }

}
