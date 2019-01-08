/*$Id: ostanov1.c,v 5.5 2013/05/17 14:56:35 sasa Exp $*/
/*27.10.2010    24.05.1991      Белых А.И.      OSTANOV1.C
Подпрограмма выдает сообщение и ждет нажатия любой клавиши
Код нажатия клавиши возвращаем в вызывающюю программу
*/

#include        "scr.h"

extern int metka_utf8;

int    ostanov1(char str[])
{
int             voz;
int             Y,X;

getyx(stdscr,Y,X);
move(LINES-1,0);
clrtoeol();
printw(str);
refresh();

voz=getch();

if(metka_utf8 == 1)
 if(voz == 208 || voz == 209 || voz == 210)
  getch();

if( voz == ESC)
 {
/*
#ifndef M_UNIX
  voz=getch();
#endif
  voz = ESC;
*/
 }
move(LINES-1,0);
clrtoeol();
move(Y,X);
refresh();
return(voz);
}
