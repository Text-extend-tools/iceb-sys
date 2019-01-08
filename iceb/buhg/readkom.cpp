/* $Id: readkom.c,v 5.14 2013-04-07 10:22:44 sasa Exp $ */
/*19.03.2013	12.05.2001	Белых А.И.	readkom.c
Чтение комментария к платежным документам
*/
#include        "buhg.h"


void readkom(const char *tabl,short dd,short md,short gd,const char *nomdk,class iceb_tu_str *koment)
{
long		kolstr;
SQL_str         row;
char		strsql[1024];
int i1;

koment->new_plus("");

if(SRAV(tabl,"Pltp",0) == 0)
  sprintf(strsql,"select zapis from Pltpz where datd='%04d-%02d-%02d' and \
nomd='%s' and mz=0 order by nz asc",gd,md,dd,nomdk);
if(SRAV(tabl,"Pltt",0) == 0)
  sprintf(strsql,"select zapis from Plttz where datd='%04d-%02d-%02d' and \
nomd='%s' and mz=0 order by nz asc",gd,md,dd,nomdk);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
 
memset(strsql,'\0',sizeof(strsql));
if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  i1 = strlen(row[0]);
  strncpy(strsql,row[0],sizeof(strsql)-1);
  if(strsql[i1-1] == '\n')
     strsql[i1-1]='\0';
  koment->plus(strsql);
  koment->plus(" ");

 }

if(iceb_tu_strlen(koment->ravno()) > 159)
 {  
  sprintf(strsql,gettext("Длинна комментария больше чем можно передать на %d символов"),
  iceb_tu_strlen(koment->ravno())-159);

  iceb_t_soob(strsql);
 }

if(iceb_tu_strlen(koment->ravno()) <= 3)
 {
  iceb_t_soob(gettext("Длинна комментария меньше трех символов"));
 }
}
