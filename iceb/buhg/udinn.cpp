/* $Id: udinn.c,v 5.8 2013/05/17 14:56:13 sasa Exp $ */
/*19.03.2011    04.07.1996      Белых А.И.      udinn.c
Удаление инвентарного номера
*/
#include        "buhg.h"


void udinn(int in)
{
char strsql[512];

/*Проверяем если больше нет записей в документах с этим инвентарным
номером то все по нему удалить*/

sprintf(strsql,"select innom from Uosdok1 where innom=%d",in);
if(readkey(strsql) > 0)
 return;

sprintf(strsql,"delete from Uosinp where innom=%d",in);

sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Uosin1 where innom=%d",in);

sql_zapis(strsql,1,0);


sprintf(strsql,"delete from Uosin where innom=%d",in);

sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Uosls where inn=%d",in);

sql_zapis(strsql,1,0);

}
