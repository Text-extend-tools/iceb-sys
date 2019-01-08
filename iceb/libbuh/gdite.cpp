/*$Id: gdite.c,v 5.3 2013/05/17 14:56:28 sasa Exp $*/
/*08.04.2011    15.11.1991      Белых А.И.      gdite.c
Подпрограмма выдачи реплики Ждите !!!
*/
#include        <nl_types.h>
#include        "buhl.h"

extern nl_catd	fils; /*Указатель на базу сообщений*/

void            GDITE()
{
int		Y,X;
char		bb[COLS];
short		dlst;
short		dlst1,dlst2;

getyx(stdscr,Y,X);
move(LINES-1,0);
clrtoeol();  /*Очистить стороку*/
attron(A_BLINK);
attron(A_REVERSE);
dlst=iceb_tu_strlen(gettext("Ждите !!!"));

dlst1=(COLS-1-dlst)/2;
dlst2=COLS-1-dlst-dlst1;
sprintf(bb,"%*s%s%*s",dlst1," ",gettext("Ждите !!!"),
dlst2," ");

printw("%s",bb);
attroff(A_BLINK);
attroff(A_REVERSE);
move(LINES-1,0);
move(Y,X);
refresh();
}
