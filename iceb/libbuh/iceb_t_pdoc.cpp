/*$Id: iceb_t_pdoc.c,v 5.4 2013/05/17 14:56:29 sasa Exp $*/
/*22.09.2010	22.09.2010	Белых А.И.	iceb_t_pdoc.c
Просмотр документации
*/
#include <errno.h>
#include "buhl.h"


int iceb_t_pdoc(const char *imafil)
{
FILE *ff;
char strsql[512];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select str from Doc where fil='%s' order by ns asc",imafil);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,"",strsql);
  return(1);
 }  

if(kolstr == 0)
 {
  sprintf(strsql,"%s-%s\n%s",__FUNCTION__,gettext("Не найдено ни одной записи !"),imafil);
  iceb_t_soob(strsql);
  return(1);
 }

if((ff=fopen(imafil,"w")) == NULL)
  {
   error_op_nfil(imafil,errno,"");
   return(1);
  }
while(cur.read_cursor(&row) != 0)
 {
  fprintf(ff,"%s",row[0]);
 }

fclose(ff);

prosf(imafil);
unlink(imafil);
return(0);
}
