/*$Id: iceb_t_prov_vdb.c,v 5.1 2014/08/31 06:18:25 sasa Exp $*/
/*01.08.2014	31.07.2014	Белых А.И.	iceb_t_prov_vdb.c
Проверка возможности оператору выгружать дам базы
*/

#include "buhl.h"


int iceb_t_prov_vdb(const char *namebases)
{


char strsql[1024];
SQL_str row,row1;
class SQLCURSOR cur,cur1;

if(namebases[0] != '\0')
 {
  sprintf(strsql,"select vdb from %s.icebuser where login='%s'",namebases,iceb_tu_getlogin());

  if(readkey(strsql,&row,&cur) == 1)
   {
    if(atoi(row[0]) != 0)
     {
      sprintf(strsql,"%s %s",gettext("У оператора нет разрешения на выгрузку дамба для базы"),namebases);
      iceb_t_soob(strsql);
      return(1);
     }
   }
  else
   return(1);
 }
else
 {
  sprintf(strsql,"SHOW DATABASES");
  int kolstr=0;
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return(1);
   }

  if(kolstr == 0)
   {
    iceb_t_soob(gettext("Не найдено ни одной доступной базы!"));
    return(1);
   }
  /*так как база ещё не выбрана то она показывает все базы и те к которым у оператора нет доступа*/
  while(cur.read_cursor(&row) != 0)
   {
    sprintf(strsql,"select vdb from %s.icebuser where login='%s'",row[0],iceb_tu_getlogin());
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     {
      if(atoi(row1[0]) != 0)
       {
        sprintf(strsql,"%s %s",gettext("У оператора нет разрешения на выгрузку дамба для базы"),row[0]);
        iceb_t_soob(strsql);
        return(1);
       }
     }
    else
     {
      int kod=sql_nerror(&bd);
      if(kod  == ER_UNKNOWN_TABLE) /*нет в базе такой таблицы*/
       continue;
      if(kod  == ER_NO_SUCH_TABLE) /*в таблице нет этой колонки*/
       continue;
      printw("\n%s-%d\n",__FUNCTION__,sql_nerror(&bd));
      OSTANOV();
      return(1);
     }
   }
 }
return(0);
}
