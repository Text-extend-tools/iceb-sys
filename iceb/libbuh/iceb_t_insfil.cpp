/*$Id: iceb_t_insfil.c,v 5.4 2013/05/17 14:56:28 sasa Exp $*/
/*13.10.2010	12.10.2010	Белых А.И.	iceb_t_insfil.c
Вставка содержимого настроечного файла в файл
*/
#include "buhl.h"

int iceb_t_insfil(const char *imaf,FILE *ff)
{
int kolih_str_ins=0;
char strsql[512];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(0);
 }

if(kolstr == 0)
  return(0);
while(cur.read_cursor(&row) != 0)
 {
  if(row[0][0] == '#')
   continue;
  fprintf(ff,"%s",row[0]);
  kolih_str_ins++;
 }

return(kolih_str_ins);
}