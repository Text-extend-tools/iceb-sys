/*$Id: exit_iceb.c,v 5.4 2013-04-07 10:22:54 sasa Exp $*/
/*27.03.2013	07.05.2005	Белых А.И.	exit_iceb.c
Завершение работы программы
*/
#include "buhl.h"

//extern SQL_baza	bd;

int exit_iceb(int metka) //0-нормально 1-ошибка
{
sql_closebaz(&bd);

refresh();
endwin();

exit(metka);
}
