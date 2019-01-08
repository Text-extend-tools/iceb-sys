/*$Id: vibprint.c,v 5.6 2013-04-07 10:22:55 sasa Exp $*/
/*23.05.2008	23.09.1999	Белых А.И.	vibprint.c
Выбор принтера
Если вернули 0- не выбрали
	     1-выбрали системный
	     2-выбрали локальный
*/
#include "buhl.h"

//extern char     *printerloc; /*Локальная печать*/

int		vibprint()
{
int		i1;
char		*p;

class iceb_tu_str printerloc("prterm -r");
iceb_t_poldan("Приэкранный принтер",&printerloc,"nastsys.alx");

p=tigetstr("mc5");
if(p != NULL && p != (char*)-1)
if(printerloc.getdlinna() > 1)
  {

naz:;

   VVOD MENUW(3);
   MENUW.VVOD_SPISOK_add_ZAG(gettext("Выберите нужный принтер"));

   MENUW.VVOD_SPISOK_add_MD(gettext("Распечатка на системном принтере"));
   MENUW.VVOD_SPISOK_add_MD(gettext("Распечатка на локальном принтере"));
   MENUW.VVOD_SPISOK_add_MD(gettext("Выход"));
   
   clearstr(LINES-1,0);
   i1=0;
   if(menu3w(stdscr,&MENUW,&i1,-1,-1,0) != 0)
    goto naz;
    
   switch(i1)
    {
     case FK10:
     case -1:
     case 2:
       return(0);
     case 0:
       return(1);
     case 1:
       return(2);
    }   
  }

return(1);  
}
