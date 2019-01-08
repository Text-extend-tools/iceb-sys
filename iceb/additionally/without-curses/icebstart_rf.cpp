/*$Id: icebstart_rf.c,v 5.8 2013/05/17 14:56:29 sasa Exp $*/
/*31.05.2012	07.09.2007	Белых А.И.	icebstart_rf.c
Чтение и выполнение запросов к базе данных сразу после открытия базы
*/
//#include <glib.h>
#include "bezcurses.h"
extern class iceb_tu_str kodirovka_iceb;

void icebstart_rf()
{
char stroka[1024];

if(kodirovka_iceb.getdlinna() <= 1)
    return;
SQL_str row;
class SQLCURSOR cur;
readkey("select VERSION()",&row,&cur);
if(atof(row[0]) < 4.1)
  return;   
sprintf(stroka,"SET NAMES %s",kodirovka_iceb.ravno_udsimv("-"));
sql_zapis(stroka,1,0);
}