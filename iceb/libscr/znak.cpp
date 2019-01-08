/*$Id: znak.c,v 5.9 2013/05/17 14:56:35 sasa Exp $*/
/*03.05.2016	11.11.1999	Белых А.И.	znak.c
Вывод на экран фирменного знака
*/
#include        "scr.h"
extern int metka_utf8;

void		znak(int Y,int X) //координаты вывода знака
{
int		y,x,i;
extern int	iscolor;
char		*tipterm;

tipterm=getenv("TERM");

attron(COLOR_PAIR(COLOR_WHITE*COLORS+COLOR_BLUE) | A_BOLD);

if(iscolor == 0) /*Черно-белый экран*/
  attron(A_REVERSE);

y=Y; x=X;
if(strncmp(tipterm,"linux",5) == 0)
 {
  mvprintw(y++,x,"                  ");
  mvprintw(y++,x,"        ▐   iceB  ");
  mvprintw(y++,x,"       ▐▐▐        ");
  mvprintw(y++,x,"      ▐▐▐▐▐       ");
  mvprintw(y++,x,"   ~~~▐▐ ▐▐ ~~~   ");
  mvprintw(y++,x,"    ~ ▐   ▐  ~    ");
  mvprintw(y++,x,"     ~ ~ ~ ~      ");
  mvprintw(y++,x,"                  ");

  if(iscolor == 0)  /*Черно-белый экран*/
    attroff(A_REVERSE);

  attroff(COLOR_PAIR(COLOR_WHITE*COLORS+COLOR_BLUE) | A_BOLD);

  if(iscolor == 1) /*Экран цветной*/
   {
    y++; x+=3;
    move(y++,x);
    attron(A_ALTCHARSET); /*Альтернативная таблица*/
    attron(COLOR_PAIR(COLOR_BLUE*COLORS+COLOR_BLACK) | A_BOLD);
    for(i=0; i<12;i++)
      printw("%c",ACS_BLOCK);
    attroff(COLOR_PAIR(COLOR_BLUE*COLORS+COLOR_BLACK) | A_BOLD);

    move(y++,x);
    attron(A_BOLD);
    attron(COLOR_PAIR(COLOR_YELLOW*COLORS+COLOR_BLACK));
    for(i=0; i<12;i++)
      printw("%c",ACS_BLOCK);

    attroff(A_BOLD);
    attroff(COLOR_PAIR(COLOR_YELLOW*COLORS+COLOR_BLACK));
    attroff(A_ALTCHARSET); /*Альтернативная таблица*/
    
   }
  attroff(A_ALTCHARSET); /*Альтернативная таблица*/
 }
else
 {
  long    alter_table=0;        /*Включение альтернативной таблицы*/
  char    vert[8];      /*Вертикальная стенка рамки меню*/
  memset(vert,'\0',sizeof(vert));
  if(metka_utf8 == 0) /*односимвольная кодировка*/
   {
    vert[0]=ACS_VLINE;
    alter_table = A_ALTCHARSET; /*Альтернативная таблица*/
   }
  else /*utf8 кодировка*/
   {

//    strcpy(vert,"│");
    strcpy(vert,"▐");
    alter_table = A_NORMAL; /*Не включаем алтернативную таблицу*/
   }

  attron(alter_table); /*Альтернативная таблица*/

  mvprintw(y,x,"."); /*иногда изза того что в следующей строке нет символов засвечивается вся строка а не выводимый отрезок */
  refresh();         /*обязательно если убрать то строка засвечивается*/

  mvprintw(y++,x,"                 ");
  mvprintw(y++,x,"        %s  ",vert);
  attroff(alter_table); /*Альтернативная таблица*/
  printw("iceB  ");
  attron(alter_table); /*Альтернативная таблица*/
//mvprintw(y++,x,"        %s  ICEB  ",vert);
  mvprintw(y++,x,"       %s%s%s       ",vert,vert,vert);
  mvprintw(y++,x,"      %s%s%s%s%s      ",vert,vert,vert,vert,vert);

  attroff(alter_table); /*Альтернативная таблица*/
  mvprintw(y++,x,"   ~~~");

  attron(alter_table); /*Альтернативная таблица*/
  printw("%s%s %s%s",vert,vert,vert,vert);

  attroff(alter_table); /*Альтернативная таблица*/
  printw("~~~   ");


  mvprintw(y++,x,"    ~ ");

  attron(alter_table); /*Альтернативная таблица*/
  printw("%s   %s",vert,vert);

  attroff(alter_table); /*Альтернативная таблица*/
  printw(" ~    ");


  mvprintw(y++,x,"     ~ ~ ~ ~     ");

  mvprintw(y,x,"."); /*иногда изза того что в следующей строке нет символов засвечивается вся строка а не выводимый отрезок */
  refresh();         /*обязательно если убрать то строка засвечивается*/

  mvprintw(y++,x,"                 ");

  y++; x+=3;
  move(y++,x);
  printw("            ");
  attroff(COLOR_PAIR(COLOR_WHITE*COLORS+COLOR_BLUE) | A_BOLD);

  move(y++,x);
  attron(COLOR_PAIR(COLOR_WHITE*COLORS+COLOR_YELLOW) | A_BOLD);
  printw("            ");
  attroff(COLOR_PAIR((COLOR_WHITE*COLORS)+COLOR_YELLOW) | A_BOLD);
  attron(COLOR_PAIR(COLOR_WHITE*COLORS+COLOR_BLACK));
 }

if(iscolor == 0) /*Черно-белый экран*/
  attroff(A_REVERSE);


}
