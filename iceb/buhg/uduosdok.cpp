/* $Id: uduosdok.c,v 5.11 2013/05/17 14:56:13 sasa Exp $ */
/*19.08.2010    04.07.1996      Белых А.И.      uduosdok.c
Проверяем сделана ли хоть одна запись в документе и если не сделана
удаляем шапку документа из файла
Если вернули 0 удалили
             1 не удалили
*/
#include        "buhg.h"


int uduosdok(short d,short m,short g,const char *ndk)
{
char		strsql[512];

sprintf(strsql,"select datd from Uosdok1 where datd='%d-%d-%d' and \
nomd='%s'",g,m,d,ndk);

if(readkey(strsql) > 0)
 return(1);

sprintf(strsql,"delete from Uosdok where datd='%d-%d-%d' and \
nomd='%s'",g,m,d,ndk);

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

/*Удаление оплат к документу*/
sprintf(strsql,"delete from Opldok where ps=3 and datd='%d-%d-%d' and \
nomd='%s'",g,m,d,ndk);

sql_zapis(strsql,1,0);

return(0);

}
