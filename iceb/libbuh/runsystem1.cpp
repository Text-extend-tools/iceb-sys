/*$Id: runsystem1.c,v 5.1 2013/09/26 09:43:55 sasa Exp $*/
/*02.04.2010	14.01.2003	Белых А.И.	runsystem1.c
Запуск прогаммы с остановом после ее выполнения
для того чтобы увидеть вывод на экран запушенной программы
*/
#include        "buhl.h"

int runsystem1(const char *comanda,int metka) //0-c остановом 1-с остановом если не нулевой возврат
{
refresh();

savetty();
resetterm();
//def_prog_mode(); //записываем настройки терминала

int voz=system(comanda);

if(metka == 0 || (metka == 1 && voz != 0))
 {
  if(metka == 1 && voz != 0)
    printw("\n%s-Ошибка при выполнении команды:%s\n",__FUNCTION__,comanda);
  else
    printw("\n");
    
  cbreak(); /*Разрещение посимвольного ввода*/ 
  OSTANOV();
 }

endwin();
resetty();
//reset_prog_mode(); //Возвращаем терминальный режим

return(voz);

}
