/*$Id: iceb_t_ndelkod.c,v 5.5 2013/05/17 14:56:29 sasa Exp $*/
/*12.04.2009	12.04.2009	Белых А.И.	iceb_t_ndelkod.c
Сообщени о невозможности удаления кода
*/

#include "buhl.h"


void iceb_t_ndelkod(const char *tablica,const char *kod)
{
char strsql[512];
VVOD SOOB(1);
SOOB.VVOD_SPISOK_add_MD(gettext("Удалить невозможно!"));

sprintf(strsql,gettext("В таблице %s есть запись с кодом %s"),tablica,kod);
SOOB.VVOD_SPISOK_add_MD(strsql);

soobshw(&SOOB,stdscr,-1,-1,0,1);


}
