/* $Id: uduoszd.c,v 5.12 2013/08/13 05:49:56 sasa Exp $ */
/*11.06.2013    04.07.1996      Белых А.И.      uduoszd.c
Удаление записей в документе
Если вернули 0 удалили
             1 нет
*/
#include        "buhg.h"

int provuin(int in);
void udinn(int in);
void udamort(int in);


int uduoszd(int in,short d,short m,short g,const char *ndk)
{
int            inn=0;
long		kolstr;
SQL_str         row;
char		strsql[512];

//printw("uduoszd-----------\n");

if(in != 0)
  sprintf(strsql,"select innom,podr from Uosdok1 where datd='%d-%d-%d' and \
nomd='%s' and innom=%d",g,m,d,ndk,in);
else
  sprintf(strsql,"select innom,podr from Uosdok1 where datd='%d-%d-%d' and \
nomd='%s'",g,m,d,ndk);
/*
printw("\n%s\n",strsql);
OSTANOV();
*/
class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
  return(0);

while(cur.read_cursor(&row) != 0)
 {
  inn=atol(row[0]);

  sprintf(strsql,"delete from Uosdok1 where datd='%d.%02d.%02d' and \
nomd='%s' and innom=%d",g,m,d,ndk,inn);
  if(sql_zap(&bd,strsql) != 0)
   {
   if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
    {
     iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
     return(1);
    }
   else
    msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
   }

  if(provuin(inn) == 0)
   {
    udamort(inn);
    udinn(inn);
   }

 }
return(0);
}
