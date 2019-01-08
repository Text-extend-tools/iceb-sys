/* $Id: udkasdok.c,v 5.11 2012-11-29 09:48:57 sasa Exp $ */
/*11.11.2012   29.09.2000      Белых А.И.      udkasdok.c
Удаление документа в подсистеме "Учет кассовых ордеров"
Если номера документа нет значит удалить за год
Если вернули 0 удалили
             1 нет

*/
#include        "buhg.h"


int udkasdok(const char *kassa,const char *nomd,short dd,short md,short gd,short tipz)
{
char		strsql[512];

if(nomd[0] != '\0')
 sprintf(strsql,"DELETE FROM Kasord1 where kassa=%s and tp=%d and \
 nomd='%s' and god=%d",kassa,tipz,nomd,gd);
else
 sprintf(strsql,"DELETE FROM Kasord1 where god=%d",gd);

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
if(nomd[0] != '\0')
 sprintf(strsql,"DELETE FROM Kasord where kassa=%s and tp=%d and \
 nomd='%s' and god=%d",kassa,tipz,nomd,gd);
else
 sprintf(strsql,"DELETE FROM Kasord where god=%d",gd);

sql_zapis(strsql,1,0);

if(nomd[0] != '\0')
 sprintf(strsql,"DELETE FROM Kasnomved where god=%d and nomd='%s'",gd,nomd);
else
 sprintf(strsql,"DELETE FROM Kasnomved where god=%d and nomd <> ''",gd);
sql_zapis(strsql,1,0);

return(0);
}



