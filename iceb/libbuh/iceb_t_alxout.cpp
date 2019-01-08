/*$Id: iceb_t_alxout.c,v 5.5 2014/05/07 10:57:29 sasa Exp $*/
/*30.04.2014	07.09.2010	Белых А.И.	iceb_t_alxout.c
Выгрузка настроек в файл
*/
#include <errno.h>
#include "buhl.h"

/************************************/
int iceb_t_alxout(const char *imaf,const char *imafin)
{
FILE *ff;
char strsql[512];
int kolstr=0;
class iceb_tu_str fil(imafin);
SQL_str row;
class SQLCURSOR cur;

if(imafin == NULL || imafin[0] == '\0')
 fil.new_plus(imaf);

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,"",strsql);
  return(1);
 }  

if(kolstr == 0)
 {
//  sprintf(strsql,"%s-%s\n%s",__FUNCTION__,gettext("Не найдено ни одной записи !"),imaf);
//  iceb_t_soob(strsql);
  return(1);
 }

if((ff=fopen(fil.ravno(),"w")) == NULL)
  {
   error_op_nfil(fil.ravno(),errno,"");
   return(1);
  }
while(cur.read_cursor(&row) != 0)
 {
  fprintf(ff,"%s",row[0]);
 }

fclose(ff);

return(0);
}
/******************************************/
int iceb_t_alxout(const char *imaf)
 {
  return(iceb_t_alxout(imaf,NULL));
 }

