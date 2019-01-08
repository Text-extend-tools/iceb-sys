/*$Id: runsystem.c,v 5.1 2013/09/26 09:43:55 sasa Exp $*/
/*22.03.2010	06.10.1999	Белых А.И.	runsystem.c
Запуск программы без останова после выполнения программы
*/
#include        "buhl.h"

int runsystem(const char *comanda)
{
int voz=0;

refresh();
def_prog_mode(); //записываем настройки терминала
endwin(); //выключаем терминальный режим работы

voz=system(comanda);

reset_prog_mode(); //Возвращаем терминальный режим
refresh();
return(voz);
}
