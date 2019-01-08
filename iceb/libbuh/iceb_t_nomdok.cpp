/* $Id: iceb_t_nomdok.c,v 5.9 2013/05/17 14:56:06 sasa Exp $ */
/*19.06.2015	22.01.1994      Белых А.И.      iceb_t_nomdok.c
Определение последнего номера документа
*/
#include        "buhl.h"

int iceb_t_nomdok(short god,const char *tabl)
{
int             mpn;  /*Максимальный порядковый номер*/
int		i;
long		kolstr;
SQL_str         row;
char		strsql[512];

mpn=0;

sprintf(strsql,"select nomd from %s where datd >= '%d-01-01' and \
datd <= '%d-12-31'",tabl,god,god);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(0);
 }

if(kolstr == 0)
 {
  return(1);
 }  


while(cur.read_cursor(&row) != 0)
 {
  i=atoi(row[0]);
  if(i > mpn)
   mpn=i;

 }

mpn++;

return(mpn);

}
