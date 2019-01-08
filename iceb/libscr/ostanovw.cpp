/*$Id: ostanovw.c,v 5.7 2013/08/13 05:50:26 sasa Exp $*/
/*26.05.2013    24.05.1991      Белых А.И.      OSTANOV.C
Подпрограмма выдает сообщение и ждет нажатия "Enter"
*/
#include        <nl_types.h>
#include        "scr.h"

extern nl_catd	fils; /*Указатель на базу сообщений*/

int    OSTANOVW(WINDOW	*win) //указатель на окно
{
short           K;
int             Y,X;

getyx(win,Y,X);
beep();
wmove(win,LINES-1,0);
wclrtoeol(win);
wattron(win,A_BLINK);  /*Включить мигание*/
wprintw(win,gettext("Нажмите любую клавишу для продолжения"));
wattroff(win,A_BLINK); /*Выключить мигание*/
wrefresh(win);
K=wgetch(win);
if(K == ESC)
 {
/*
#ifndef M_UNIX
  K=wgetch(win);
#endif
  K=ESC;
*/
 }

wmove(win,LINES-1,0);
wclrtoeol(win);
wmove(win,Y,X);
wrefresh(win);
return(K);
}
