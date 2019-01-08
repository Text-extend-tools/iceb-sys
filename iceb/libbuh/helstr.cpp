/*$Id: helstr.c,v 5.3 2013/05/17 14:56:28 sasa Exp $*/
/*06.04.2010   30.03.1999         Белых А.И.      helstr.c
*/
#include	"buhl.h"


void helstr(int py,int px,...)
{
extern long	cklav,cstr;
va_list		strh;
char		*kl;
char		*soob;
short		dl=0;
short		dlz=0;

move(py,px);
clrtoeol();  /*Очистить стороку*/
va_start(strh,px);
dl=0;
while((kl=va_arg(strh,char*)) != NULL)
 {
  dl+=iceb_tu_strlen(kl);

  if(dl >= COLS)
    break;

  dlz=dl;  

  attron(A_BOLD);
  attron(cklav);

  printw("%s",kl);

  attroff(A_BOLD);
  attroff(cklav);

  soob=va_arg(strh,char*);
  dl+=iceb_tu_strlen(soob);
  
  if(dl >= COLS)
   {
    if(COLS-1 > dlz)
     {    
//      mvprintw(LINES-2,0,"COLS=%d dlz=%d dl=%d",COLS,dlz,dl);
      attron(cstr);
      printw("%.*s",iceb_tu_kolbait(COLS-dlz-1,soob),soob);
      attroff(cstr);
     }    
    break;
   }
  attron(cstr);
  printw("%s",soob);
  attroff(cstr);

 }

va_end(strh);
}
