/* $Id: provkol.c,v 5.8 2013-04-07 10:22:43 sasa Exp $ */
/*29.11.2009	20.06.2000	Белых А.И.	provkol.c
Поверка количества материалла во встречном документе
*/
#include        "buhg.h"


double provkol(short dd,short md,short gd,int skl,const char *nomd)
{
char		strsql[512];
long		kolstr;
SQL_str         row;
double		kolih;

sprintf(strsql,"select kolih from Dokummat1 where datd='%d-%02d-%02d' and \
sklad=%d and nomd='%s'",
gd,md,dd,skl,nomd);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(0.);
 }
if(kolstr == 0)
 {
  return(0.);
 }
kolih=0.;
while(cur.read_cursor(&row) != 0)
 {
  kolih+=atof(row[0]);
 }
return(kolih);

}
