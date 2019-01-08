/*$Id: provinnom.c,v 5.12 2013-04-07 10:22:54 sasa Exp $*/
/*05.12.2009	10.10.2002	Белых А.И.	provinnom.c
Проверить есть ли такой инвентарный номер
Если вернули 0 - нет
*/
#include        "buhl.h"


int provinnom(const char *innom)
{
char		strsql[512];
SQL_str		row;
SQLCURSOR       cur;
char		bros[512];

sprintf(strsql,"select innom from Uosin where innom=%s",innom);
if(sql_readkey(&bd,strsql,&row,&cur) == 0)
 {
  if(iceb_t_poldan("Проверка инвентарного номера в материальном учете",bros,"uosnast.alx") == 0)
   if(SRAV(bros,"Вкл",1) != 0)
      return(0);

  sprintf(strsql,"select sklad,nomk  from Kart where innom='%s' and rnd=''",innom);
  if(sql_readkey(&bd,strsql,&row,&cur) == 0)
   return(0);

  if(sql_readkey(&bd,strsql,&row,&cur) > 0)
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Такой инвентарный номер уже есть !"));
    sprintf(strsql,"%s %s %s %s",gettext("Склад"),row[0],
    gettext("карточка"),row[1]);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
   }   
 }
else
 {
   VVOD SOOB(1);
   SOOB.VVOD_SPISOK_add_MD(gettext("Такой инвентарный номер уже есть !"));
   SOOB.VVOD_SPISOK_add_MD(gettext("Учет основных средств"));
   soobshw(&SOOB,stdscr,-1,-1,0,1);
 }

return(1);
}
