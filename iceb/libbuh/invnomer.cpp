/*$Id: invnomer.c,v 5.14 2013/08/13 05:50:23 sasa Exp $*/
/*22.02.2010	10.10.2002	Белых А.И.	invnomer.c
Получение Свободного инвентарного номера
*/
#include        "buhl.h"

/*************************/
int invnomer(int metka)
{
int		i;
char		strsql[512];
SQL_str         row,rowtmp;
long		kolstr;

if(metka == 0)
  GDITE();

class iceb_t_tmptab tabtmp;
const char *imatmptab={"icebinvnomer"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (kod int not null primary key) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(0);
 }  



sprintf(strsql,"select innom from Uosin order by innom asc");
SQLCURSOR cur,curtmp;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
 }
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"insert into %s values (%d)",imatmptab,atoi(row[0]));

  sql_zapis(strsql,1,1);    
 }

sprintf(strsql,"select innom from Kart where innom != '' and rnd=''");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
 }

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"insert into %s values (%d)",imatmptab,atoi(row[0]));

  sql_zapis(strsql,1,1);    
 }

sprintf(strsql,"select * from %s order by kod asc",imatmptab);

int kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return(0);
 }

i=0;
while(curtmp.read_cursor(&rowtmp) != 0)
 {
  //Регістационные номера мед.препаратов начинаются на символ
  if(atoi(rowtmp[0]) == 0)  /*Значит символ не число*/
   continue;
  i++;
  if(i != atoi(rowtmp[0]))
   {
    return(i);
   }
 }
i++;

return(i);


}
