/*$Id: ostanov.c,v 5.8 2013/08/13 05:50:26 sasa Exp $*/
/*26.05.2013    24.05.1991      Белых А.И.      OSTANOV.C
Подпрограмма выдает сообщение и ждет нажатия "Enter"
*/
#include        <nl_types.h>
#include        "scr.h"

extern nl_catd	fils; /*Указатель на базу сообщений*/
extern int metka_utf8;

int OSTANOV()
{
int             K;
int             Y,X;

getyx(stdscr,Y,X);
beep();

move(LINES-1,0);

clrtoeol();
attron(A_BLINK);  /*Включить мигание*/
printw(gettext("Нажмите любую клавишу для продолжения"));
attroff(A_BLINK); /*Выключить мигание*/
refresh();
K=getch();

 
if(metka_utf8 == 1)
 if(K == 208 || K == 209 || K == 210)
  getch();


move(LINES-1,0);
clrtoeol();
move(Y,X);
refresh();
return(K);
}
