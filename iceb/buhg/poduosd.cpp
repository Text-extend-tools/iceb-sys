/* $Id: poduosd.c,v 5.9 2013-04-07 10:22:43 sasa Exp $ */
/*16.12.2009    04.07.1996      Белых А.И.      poduosd.c
Проверка подтверждения документа
*/
#include        "buhg.h"


void poduosd(short d,short m,short g,const char *ndk)
{
long		kolstr;
SQL_str         row;
char		strsql[512];
int		mp;
short		podt;

sprintf(strsql,"select podt from Uosdok1 where datd='%d-%d-%d' and \
nomd='%s'",g,m,d,ndk);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  return;
 }
mp=0;
while(cur.read_cursor(&row) != 0)
 {
  if(atoi(row[0]) == 1)
   mp++;  
 }


podt=0;
if(kolstr == mp)
 podt=1;

/*
printw("podt=%d\n",podt);
OSTANOV();
*/

sprintf(strsql,"update Uosdok \
set \
podt=%d \
where datd='%04d-%d-%d' and nomd='%s'",podt,g,m,d,ndk);

if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_DBACCESS_DENIED_ERROR)
  {
   char bros[512];
   sprintf(bros,"%s-%s",__FUNCTION__,gettext("Ошибка корректировки записи !"));
   msql_error(&bd,bros,strsql);
  }

}
