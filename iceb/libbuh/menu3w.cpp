/*$Id: menu3w.c,v 5.8 2013/05/17 14:56:31 sasa Exp $*/
/*21.10.2009    30.10.2000      Белых А.И.     menu3w.c
Подпрограмма получения нужного меню пользователя
работающая с псевдографикой.
работает в своем окне
Возвращаем номер строчки которую выбрали или -1 если нажали F10

*/

#include        "buhl.h"

int menu3w(WINDOW *win1, //Окно на котором создаем новое окно
VVOD *VV, //Структура данных меню
int *N, //Номер строки на которой должна быть подсветка
int Yz,int Xz, //Координаты вывода меню
int mt) //0-Тень не выводить 1-выводить
{
int             K;  /*Принимает код клавиши*/
WINDOW		*win2;
int kolbait=0;
int		PYstd=0,PXstd=0;
int		sob=0;

/*Вытолкнем содержимое предыдущего экрана*/
//wprintw(win1,"\nДо рамки\n");
//OSTANOVW(win1);

wrefresh(win1);

VV->wvramka(Yz,Xz,&PYstd,&PXstd,mt,0,0,win1,&win2);

//wprintw(win1,"\nПосле рамки\n");
//OSTANOVW(win1);

if(*N < 0 || *N >= VV->KLST)
 *N=0;
 
wattroff(win2,VV->VVOD_return_cs(iceb_CFM));
wattron(win2,VV->VVOD_return_cs(iceb_CFS)); 

wmove(win2,VV->PY+(*N)+1,VV->PX+1);
wprintw(win2,"%.*s",iceb_tu_kolbait(VV->MDLS,VV->VVOD_SPISOK_return_MD(*N)),VV->VVOD_SPISOK_return_MD(*N));

wrefresh(win2);
for(;;)
 {

  K=wgetch(win2);

  wattroff(win2,VV->VVOD_return_cs(iceb_CFS));
  wattron(win2,VV->VVOD_return_cs(iceb_CFM)); 

  if(K == KEY_RESIZE)
   {
    sob=K;
//    wgetch(win2);
  //  getch();
    break;
   }

  wmove(win2,VV->PY+*N+1,VV->PX+1);

  wprintw(win2,"%.*s",iceb_tu_kolbait(VV->MDLS,VV->VVOD_SPISOK_return_MD(*N)),VV->VVOD_SPISOK_return_MD(*N));
  wrefresh(win2);

  if(K == KEY_MOUSE) //Мышка
   K=mouseklic(N,PYstd,PXstd,VV->MDLS,VV->KLST,win2);

  if(K==SDN) /*Стрелка вниз*/
    *N+=1;

  if(K==SUP) /*Стрелка вверх*/
    *N-=1;

  if(*N > VV->KLST-1)
    *N=0;

  if(*N < 0)
    *N=VV->KLST-1;

  wattron(win2,VV->VVOD_return_cs(iceb_CFS)); 
  wmove(win2,VV->PY+*N+1,VV->PX+1);
  kolbait=iceb_tu_kolbait(VV->MDLS,VV->VVOD_SPISOK_return_MD(*N));
  wprintw(win2,"%.*s",kolbait,VV->VVOD_SPISOK_return_MD(*N));
  wrefresh(win2);

  if(K == ENTER)
   {
    wattroff(win2,VV->VVOD_return_cs(iceb_CFS));
    wattron(win2,VV->VVOD_return_cs(iceb_CFM));
    wmove(win2,VV->PY+*N+1,VV->PX+1);
    wprintw(win2,"%.*s",kolbait,VV->VVOD_SPISOK_return_MD(*N));
    break;
   }

  if(K == ESC)
   {
/*
#ifndef M_UNIX
      K=getch();
#endif
*/
    wattroff(win2,VV->VVOD_return_cs(iceb_CFS));
    wattron(win2,VV->VVOD_return_cs(iceb_CFM));
    wmove(win2,VV->PY+*N+1,VV->PX+1);
    wprintw(win2,"%.*s",kolbait,VV->VVOD_SPISOK_return_MD(*N));
    *N=-1;
    break;
   }

  if(K == FK10)
   {
    wattroff(win2,VV->VVOD_return_cs(iceb_CFS));
    wattron(win2,VV->VVOD_return_cs(iceb_CFM));
    wmove(win2,VV->PY+*N+1,VV->PX+1);
    wprintw(win2,"%.*s",kolbait,VV->VVOD_SPISOK_return_MD(*N));
    *N=-1;
    break;
   }

 }
//wattroff(win2,VV->VVOD_return_cs(iceb_CFS));
//wattron(win2,VV->VVOD_return_cs(iceb_CFM));

//wattron(win2,VV->VVOD_return_cs(iceb_CB));
wrefresh(win2);
delwin(win2);
touchwin(win1);
wrefresh(win1);

return(sob);
}
