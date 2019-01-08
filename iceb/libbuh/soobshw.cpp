/*$Id: soobshw.c,v 5.6 2013/05/17 14:56:32 sasa Exp $*/
/*30.07.2002	03.03.1999	Белих А.И.	soobshw.c
Выдача сообщения в окно 
*/
#include        <ctype.h>
#include        "buhl.h"

int soobshw(VVOD *VV,WINDOW *win1,
int PY0z,int PX0z,
int ost, //0-вывести реплику "Нажмите любую клавишу" 1-нет
int TEN) //0-без тени 1-с тенью
{
int             K;  /*Принимает код клавиши*/
int		N=0;
WINDOW		*win2;
int             Y,X;
int		PYstd=0,PXstd=0;

getyx(win1,Y,X);

/*Вытолкнем содержимое предыдущего экрана*/
wrefresh(win1);

VV->wvramka(PY0z,PX0z,&PYstd,&PXstd,TEN,0,0,win1,&win2);

//OSTANOVW(win2);

wrefresh(win2);
delwin(win2);
standend(); //Установка нормального режима отображения
refresh();


wmove(win1,0,0);

if(ost == 0)
  K=OSTANOVW(win1);
else
  K=wgetch(win1);

if(K == KEY_MOUSE)
  K=mouseklic(&N,PYstd,PXstd,VV->MDLS,VV->KLST,stdscr);

//if(K == KEY_RESIZE)
//  wgetch(win1);
  
touchwin(win1);

wmove(win1,Y,X);
wrefresh(win1);

standend(); //Установка нормального режима отображения
refresh();

return(K);
}
