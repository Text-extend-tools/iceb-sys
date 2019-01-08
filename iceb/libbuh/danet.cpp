/*$Id: danet.c,v 5.7 2013/08/13 05:50:22 sasa Exp $*/
/*21.11.2009    28.04.1993              Белых А.И.      danet.c
Подпрограмма выдает запрос на который нужно выбрать
да или нет
Если вернули 1-выбрали да 2-выбрали нет
*/
#include        "buhl.h"


int danet(VVOD *VV,
int N, //1 стать на да, 2 стать на нет
WINDOW *win1)   //Окно на которое выведено должно быть меню
{
int             i;
//char            ST[COLS+10]; /*Строка меню*/
class iceb_tu_str ST;
int             K;  /*Принимает код клавиши*/
//int             PR;
int             poz;
WINDOW		*win2;
int		kolelem=0;
int		PYstd=0,PXstd=0;
MEVENT	        event;

//wprintw(win1,"\ndanet-----------\n");
//wrefresh(win1);

/*Вытолкнем содержимое предыдущего экрана*/
wrefresh(win1);

kolelem=VV->VVOD_SPISOK_return_kolstr();

/********************/
/*   Рисуем рамку   */
/********************/
VV->wvramka(-1,-1,&PYstd,&PXstd,1,1,0,win1,&win2);

wattroff(win2,VV->VVOD_return_cs(iceb_CFR));
wattron(win2,VV->VVOD_return_cs(iceb_CFM));

poz=VV->PY+kolelem+1;

wmove(win2,poz,VV->PX+1);
wrefresh(win2);
/**************
strcpy(ST," ");
for(i=0 ; i < (VV->MDLS-9)/2 ; i++)
  strcat(ST," ");
strcat(ST,gettext(" Да  "));
strcat(ST,gettext(" Нет "));

wprintw(win2,"%s",ST);
***************************/
ST.plus(" ");
for(i=0 ; i < (VV->MDLS-9)/2 ; i++)
  ST.plus(" ");
ST.plus(gettext(" Да  "));
ST.plus(gettext(" Нет "));
wprintw(win2,"%s",ST.ravno());

wattroff(win2,VV->VVOD_return_cs(iceb_CFM));
wattron(win2,VV->VVOD_return_cs(iceb_CFS)); 

int point1=VV->PX+3+(VV->MDLS-9)/2-1;
int point2=VV->PX+7+(VV->MDLS-9)/2;

if(N == 1)
 {
  wmove(win2,poz,point1);
  wprintw(win2,"%s",gettext(" Да  "));
 }

if(N == 2)
 {
  wmove(win2,poz,point2);
  wprintw(win2,"%s",gettext(" Нет "));
 }
//PR=0;

wrefresh(win2);
for(;;)
 {
  K=wgetch(win2);

  wattroff(win2,VV->VVOD_return_cs(iceb_CFS));
  wattron(win2,VV->VVOD_return_cs(iceb_CFM));

  if(N == 1)
   {
    wmove(win2,poz,point1);
    wprintw(win2,"%s",gettext(" Да  "));
    wrefresh(win2);
   }
  if(N == 2)
   {
    wmove(win2,poz,point2);
    wprintw(win2,"%s",gettext(" Нет "));
    wrefresh(win2);
   }

  if(K == KEY_MOUSE)
   if(getmouse(&event) == OK)
    {
     if(event.y == PYstd+kolelem+1)
      {
       if(event.x >= PXstd+point1 && event.x < PXstd+point2)
        N=1;
       if(event.x >= PXstd+point2 && event.x < PXstd+point2+4)
        N=2;
       if(event.bstate & BUTTON1_DOUBLE_CLICKED) //Первая кнопка нажата и отпущена дважды.
         K=ENTER;

       if(event.bstate & BUTTON3_CLICKED) //Третья кнопка нажата и отпущена.
         K=ENTER;
      }
    }

  if(K==SRITH) /*Стрелка вправо*/
    N++;
  if(K==SLEFT) /*Стрелка влево*/
    N--;
  if(N > 2)
    N=1;
  if(N < 1)
    N=2;

  wattron(win2,VV->VVOD_return_cs(iceb_CFS)); 
  if(N == 1)
   {
    wmove(win2,poz,point1);
    wprintw(win2,"%s",gettext(" Да  "));
    wrefresh(win2);
  
   }
  if(N == 2)
   {
    wmove(win2,poz,point2);
    wprintw(win2,"%s",gettext(" Нет "));
    wrefresh(win2);
   }
  
  if(K == ENTER)
      break;

 }
wattroff(win2,VV->VVOD_return_cs(iceb_CFS));
wattron(win2,VV->VVOD_return_cs(iceb_CFM));

wrefresh(win2);
delwin(win2);

touchwin(win1);
wrefresh(win1);
return(N);
}
