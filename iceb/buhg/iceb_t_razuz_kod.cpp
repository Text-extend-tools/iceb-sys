/*$Id: iceb_t_razuz_kod.c,v 5.4 2013/05/17 14:56:02 sasa Exp $*/
/*06.11.2007	03.09.2004	Белых А.И.	iceb_t_razuz_kod.c
Получение на заданный код списка входящих деталей
Если вернули 0-нет входящих деталей
*/
#include "buhg.h"

extern SQL_baza	bd;
int iceb_t_razuz_kod(class iceb_t_razuz_data *data)
{
char strsql[512];
SQL_str  row;
SQLCURSOR cur;

sprintf(strsql,"select kodi from Specif where kodi=%d limit 1",data->kod_izd);

if(readkey(strsql,&row,&cur) <= 0)
 return(0);

 
/*Определяем входящие узлы*/
razuz(data);

/*Определяем входящие детали*/
int kolndet=razuz1(data);

return(kolndet);
}
