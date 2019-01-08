/*$Id: menu3.c,v 5.6 2013/05/17 14:56:31 sasa Exp $*/
/*21.10.2009    17.05.1991      Белых А.И.     menu3.c
Подпрограмма получения нужного меню пользователя
работающая с псевдографикой.
*/

#include        "buhl.h"

int menu3(VVOD *VV,
int *N,  //Возвращаемое значение
int mt, //0-без тени 1-с тенью
int PYz,int PXz) //Координаты вывода меню
{
int             K=0;  /*Принимает код клавиши*/
int             sob=0;

VV->vramka(PYz,PXz,mt);

if( *N < 0 || *N >= VV->KLST)
 *N=0;
 
attroff(VV->VVOD_return_cs(iceb_CFM));
attron(VV->VVOD_return_cs(iceb_CFS)); 
int kolbait=0;
move(VV->PY+*N+1,VV->PX+1);
printw("%.*s",iceb_tu_kolbait(VV->MDLS,VV->VVOD_SPISOK_return_MD(*N)),VV->VVOD_SPISOK_return_MD(*N));
for(;;)
 {

  K=getch();

  attroff(VV->VVOD_return_cs(iceb_CFS));
  attron(VV->VVOD_return_cs(iceb_CFM));

  if(K == KEY_RESIZE) //Изменен размер экрана
   {
    sob=K;
//    getch();
    break;
   }
  kolbait=iceb_tu_kolbait(VV->MDLS,VV->VVOD_SPISOK_return_MD(*N));
  move(VV->PY+*N+1,VV->PX+1);
  printw("%.*s",kolbait,VV->VVOD_SPISOK_return_MD(*N));
  refresh();

  if(K == KEY_MOUSE)
   K=mouseklic(N,VV->PY,VV->PX,VV->MDLS,VV->KLST,stdscr);
   

  if(K==SDN) /*Стрелка вниз*/
   *N+=1;

  if(K==SUP) /*Стрелка вверх*/
   *N-=1;

  if(*N > VV->KLST-1)
   *N=0;

  if(*N < 0)
    *N=VV->KLST-1;

  attron(VV->VVOD_return_cs(iceb_CFS)); 
  move(VV->PY+*N+1,VV->PX+1);
  kolbait=iceb_tu_kolbait(VV->MDLS,VV->VVOD_SPISOK_return_MD(*N));
  printw("%.*s",kolbait,VV->VVOD_SPISOK_return_MD(*N));
  refresh();

  if(K == ENTER)
   {
    attroff(VV->VVOD_return_cs(iceb_CFS));
    attron(VV->VVOD_return_cs(iceb_CFM));
    move(VV->PY+*N+1,VV->PX+1);
    printw("%.*s",kolbait,VV->VVOD_SPISOK_return_MD(*N));
    break;
   }
  if(K == ESC)
   {
/*
#ifndef M_UNIX
     getch();
#endif
*/
    attroff(VV->VVOD_return_cs(iceb_CFS));
    attron(VV->VVOD_return_cs(iceb_CFM));
    move(VV->PY+*N+1,VV->PX+1);
    printw("%.*s",kolbait,VV->VVOD_SPISOK_return_MD(*N));
    *N=-1;
    break;
   }
  if(K == FK10)
   {
    attroff(VV->VVOD_return_cs(iceb_CFS));
    attron(VV->VVOD_return_cs(iceb_CFM));
    move(VV->PY+*N+1,VV->PX+1);
    printw("%.*s",kolbait,VV->VVOD_SPISOK_return_MD(*N));
    *N=-1;
    break;
   }


 }
//attroff(VV->VVOD_return_cs(iceb_CFS));
//attron(VV->VVOD_return_cs(iceb_CB));
standend(); //Установка нормального режима
refresh();

return(sob);
}
