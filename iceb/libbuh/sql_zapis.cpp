/*$Id: sql_zapis.c,v 5.14 2014/04/30 06:12:32 sasa Exp $*/
/*29.11.2013	22.03.2003	Белых А.И.	sql_zapis.c
Запись в базу данных
Если вернули 0 - запиь сделана - иначе код ошибки
*/
#include	"buhl.h"


int sql_zapis(const char *strsql,
int metka, //0-выдать Ждите 1-нет
int metkasoob,WINDOW *win) //0-выдать сообщение о дублированной записи 1-нет
{

if(metka == 0)
  GDITE();

if(sql_zap(&bd,strsql) != 0)
 {
  int kod_oh=sql_nerror(&bd);
  if(kod_oh  == ER_DUP_ENTRY) //Запись уже есть
   {
    if(metkasoob == 0)
     {
//      wprintw(win,"\n%s-%s\n",__FUNCTION__,strsql);
      iceb_t_soob(gettext("Такая запись уже есть !"));
     }
    return(ER_DUP_ENTRY);
   }

  if(kod_oh == ER_DBACCESS_DENIED_ERROR) //Только чтение
   {
    class iceb_tu_str repl;
    class iceb_tu_str npole;
    
    repl.plus_ps(gettext("У вас нет полномочий для выполнения этой операции !"));
    for(int nom=1; nom <= 3; nom++)
     {
      if(polen(strsql,&npole,nom,' ') == 0)
        repl.plus(" ",npole.ravno());
     }
    iceb_t_soob(repl.ravno(),win);
    return(ER_DBACCESS_DENIED_ERROR);
   }

//  if(kod_oh == ER_TABLE_NOT_LOCKED) //Таблица не заблокирована запросом LOCK TABLE - какая другая таблица заблокирована а эта нет - запись будет выполнена
//   return(0);
           
  msql_error(&bd,__FUNCTION__,strsql,win);
  return(kod_oh);
 }
return(0);
}
/*****************************************/

int sql_zapis(const char *strsql,
int metka, //0-выдать Ждите 1-нет
int metkasoob) //0-выдать сообщение о дублированной записи 1-нет
{

return(sql_zapis(strsql,metka,metkasoob,stdscr));
}