/*$Id: uplnomdok.c,v 5.8 2013/05/17 14:56:15 sasa Exp $*/
/*14.04.2003	14.04.2003	Белых А.И.	uplnomdok.c
Получение номер путевого листа
*/
#include        "buhg.h"


void uplnomdok(short god,const char *podr,char *nomdok)
{
int		kolstr;
SQL_str         row;
char		strsql[512];
int		nomer=0;
int		nomerv=0;
SQLCURSOR cur;

sprintf(strsql,"select nomd from Upldok where god=%d and kp=%s",
god,podr);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  if(POLE(row[0],strsql,2,'/') != 0)
   continue;

  nomer=atoi(strsql);

  if(nomerv < nomer)
    nomerv=nomer;
 }
nomerv++;
sprintf(nomdok,"%s/%d",podr,nomerv);
}
