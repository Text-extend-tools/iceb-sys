/*$Id: sql_zapis.c,v 5.12 2013/05/17 14:56:32 sasa Exp $*/
/*28.11.2011	22.03.2003	Белых А.И.	sql_zapisf.c
Запись в базу данных
Если вернули 0 - запиь сделана - иначе код ошибки
*/
#include	"bezcurses.h"


int sql_zapis(const char *strsql,
int metka, //0-выдать Ждите 1-нет
int metkasoob) //0-выдать сообщение о дублированной записи 1-нет
{

//if(metka == 0)
//  GDITE();

if(sql_zap(&bd,strsql) != 0)
 {
  int kod_oh=sql_nerror(&bd);
  if(kod_oh  == ER_DUP_ENTRY) //Запись уже есть
   {
    if(metkasoob == 0)
     {
      printf("\n%s-%s\n",__FUNCTION__,strsql);
      printf("%s\n",gettext("Такая запись уже есть !"));
     }
    return(ER_DUP_ENTRY);
   }

  if(kod_oh == ER_DBACCESS_DENIED_ERROR) //Только чтение
   {
    
    printf("%s\n",gettext("У вас нет полномочий для выполнения этой операции !"));
    printf("%s\n",strsql);

    return(ER_DBACCESS_DENIED_ERROR);
   }

//  if(kod_oh == ER_TABLE_NOT_LOCKED) //Таблица не заблокирована запросом LOCK TABLE - какая другая таблица заблокирована а эта нет - запись будет выполнена
//   return(0);
           
  msql_error(&bd,"",strsql);
  return(kod_oh);
 }
return(0);
}
