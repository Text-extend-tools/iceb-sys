/* $Id: redkal.c,v 5.15 2013/05/17 14:56:10 sasa Exp $ */
/*24.05.2016    06.01.1994      Белых А.И.      redkal.c
Чтение количества рабочих дней и часов за нужный месяц
*/
#include        "buhg.h"

void redkal(short m,short g, //Дата
float *dnei,float *has)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[1024];
*dnei=0.;
*has=0.;



sprintf(strsql,"select krd,krh from Zarskrdh where data='%04d-%02d-01'",g,m);
if(readkey(strsql,&row,&cur) == 1)
 {
  *dnei=atof(row[0]);
  *has=atof(row[1]);
  sprintf(strsql,"%s-Дата %d.%d г. Рабочих денй %.6g Часов %.6g\n",__FUNCTION__,m,g,*dnei,*has);
  zar_pr_ins(strsql);
 }
else
 {
  sprintf(strsql,"%s-Нет записи с количеством рабочих дней и часов по дате %d.%d г.\n",__FUNCTION__,m,g);
  zar_pr_ins(strsql);
 }
}
