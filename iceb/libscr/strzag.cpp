/*$Id: strzag.c,v 5.12 2013/05/17 14:56:35 sasa Exp $*/
/*31.01.2008	30.04.1999	Белых А.И.	strzag.c
Строка отображения прогресса выполения программы
*/
#include        <math.h>
#include        "scr.h"

void		strzag(int YY,int XX,//Координаты вывода строки
long kolsto, //Количество всех строк
long kolstz) //Номер строки которая просматривается
{
extern int	iscolor;
int		kols=COLS-1; //на один символ короче чтобы не было первода строки
static int kc1z;
int		kc1;
int		kc2;
int		i;
int		Y,X;
double		proc;
char		kkk=' ';
static short	mkk;
short		metkam=0;

if(kolstz == 1)
 kc1z=0;

kc1=kolstz*kols/kolsto;
if(kc1 == kc1z ) /*Если ничено не изменилось то выходим*/
  return;
kc1z=kc1;


mkk++;
if(mkk > 3)
 mkk=0;
if(mkk < 0)
  mkk=0;
  
if(mkk == 0)
 kkk='|';

if(mkk == 1)
 kkk='/';
if(mkk == 2)
 kkk='-';
if(mkk == 3)
 kkk='\\';

getyx(stdscr,Y,X);




proc=kolstz*100./kolsto;
modf(proc,&proc);

kc2=kols-kc1;
//printw("kc1=%d kc2=%d\n",kc1,kc2);

if(iscolor != 0)
 attron(COLOR_PAIR(COLOR_WHITE*COLORS+COLOR_RED));
else
 attron(A_REVERSE);

move(YY,XX);
clrtoeol();
for(i=0; i < kc1; i++)
 {
  if(i == 0)
   {
    metkam++;
    printw("%c",kkk);
    continue;
   }
  printw(" ");
 }

if(iscolor != 0)
 {
 attroff(COLOR_PAIR(COLOR_WHITE*COLORS+COLOR_RED));

 attron(COLOR_PAIR(COLOR_WHITE*COLORS+COLOR_BLUE));
 }
else
  attroff(A_REVERSE);

for(i=0; i < kc2; i++)
 {
  if(i == 0 && metkam == 0)
   {
    metkam++;
    printw("%c",kkk);
    continue;
   }
  printw(" ");
 }
if(iscolor != 0)
 attroff(COLOR_PAIR(COLOR_WHITE*COLORS+COLOR_BLUE));

XX=kols/2-3;
if(kc1 > XX)
  attron(COLOR_PAIR(COLOR_WHITE*COLORS+COLOR_RED));
else
  attron(COLOR_PAIR(COLOR_WHITE*COLORS+COLOR_BLUE));


attron(A_BOLD);

mvprintw(YY,XX,"%.f%%",proc);
attroff(A_BOLD);

if(kc1 > XX)
  attroff(COLOR_PAIR(COLOR_WHITE*COLORS+COLOR_RED));
else
  attroff(COLOR_PAIR(COLOR_WHITE*COLORS+COLOR_BLUE));

move(Y,X);
refresh();

}
