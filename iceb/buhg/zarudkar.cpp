/* $Id: zarudkar.c,v 5.17 2013/09/26 09:43:46 sasa Exp $ */
/*16.04.2017	25.05.1999	Белых А.И.	zarudkar.c
Удаление карточки в зарплате и всех сопутствующих записей
Вернули 0 если удалили
        1 нет
*/
#include        "buhg.h"

extern short 	ddd,mmm,ggg;

int zarudkar(int tabn)
{
char		strsql[512];

VVOD DANET(1);
DANET.VVOD_SPISOK_add_MD(gettext("Кроме карточки по этому табельному номеру будут удалены"));
DANET.VVOD_SPISOK_add_MD(gettext("все начисления, удержания, проводки."));

DANET.VVOD_SPISOK_add_MD(gettext("Удалить карточку ? Вы уверены ?"));
if(danet(&DANET,2,stdscr) == 2)
  return(1);

helstr(LINES-1,0,"F10",gettext("выход"),NULL);

strsql[0]='\0';
VVOD PAROL(2);
PAROL.VVOD_SPISOK_add_MD(gettext("Введите пароль"));
if(vparol(&PAROL,strsql,20,stdscr,-1,-1,1) == FK10)
  return(1);


if(ATOFM(strsql) != ddd+mmm+ggg)
 {
  iceb_t_soob(gettext("Не верно введён пароль !"));
  return(1);
 }
GDITE();

sprintf(strsql,"delete from Zarp where tabn=%d",tabn);
if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
  {
   iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
   return(1); 
  }
 else
   msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
 }

sprintf(strsql,"delete from Zarn where tabn=%d",tabn);
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Zarn1 where tabn=%d",tabn);
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Zsal where tabn=%d",tabn);
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Ztab where tabn=%d",tabn);
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Zarsocz where tabn=%d",tabn);
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Kartb where tabn=%d",tabn);
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Kartb1 where tabn=%d",tabn);
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Zarkh where tn=%d",tabn);
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Zarrud where tn=%d",tabn);
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Zargr where tn=%d",tabn);
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Zarpr where tn=%d",tabn);
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Zarsdo where tn=%d",tabn);
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Zarlgot where tn=%d",tabn);
sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Prov where kto='%s' and nomd like '%%-%d'",ICEB_MP_ZARP,tabn);
sql_zapis(strsql,1,0);

return(0);
}



