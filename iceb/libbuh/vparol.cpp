/*$Id: vparol.c,v 5.8 2013/05/17 14:56:32 sasa Exp $*/
/*28.08.2012    22.11.1999              Белых А.И.      vparol.c
Подпрограмма выдает запрос на ввод пароля
Вводимый пароль не отображается на экране
Если вернули 0 нажата клавиша ввод
             1 нажана клавиша FK10
*/
#include        "buhl.h"

int vparol(VVOD *VV,char parol[],int dlstrk,
WINDOW *win1,   //Окно на которое выведено должно быть меню
int Yz,int Xz, //Координаты вывода меню
int TEN) //0-без тени 1-с тенью
{
int             i;
int             K=0;  /*Принимает код клавиши*/
short		dls;
WINDOW		*win2;
int		PYstd=0,PXstd=0;
MEVENT	        event;
int		KL=0;


/*Вытолкнем содержимое предыдущего экрана*/
wrefresh(win1);

int kolelem=VV->VVOD_SPISOK_return_kolstr();

/********************/
/*   Рисуем рамку   */
/********************/

VV->wvramka(Yz,Xz,&PYstd,&PXstd,TEN,1,0,win1,&win2);

int poz=VV->PY+kolelem+1;
 
wmove(win2,poz,VV->PX+1);
wattroff(win2,VV->VVOD_return_cs(iceb_CFM));

wattron(win2,VV->VVOD_return_cs(iceb_CFV)); 

wprintw(win2,"%*s",VV->MDLS," ");
wmove(win2,poz,VV->PX+1);
wprintw(win2,"%s",parol);
wmove(win2,poz,VV->PX+1);

dls=VV->MDLS;
if(dlstrk-1 < VV->MDLS)
  dls=dlstrk-1;



for(i=0;;i++)
 {
  wrefresh(win2);  


  KL=parol[i]=wgetch(win2);
  parol[i+1]='\0';

  if(KL == (char) KEY_MOUSE)
   {
    if(getmouse(&event) == OK)
     {

      K=1;
      parol[i]='\0';
      break;
     }
   }
  if(parol[i] == (char)FK10)
   {
    K=1;
    parol[i]='\0';
    break;
   }   

  if(parol[i] == ENTER)
   {
    parol[i]='\0';
    break;
   }
  if(parol[i] == (char)BACK)
   {
    parol[i]='\0';
    wmove(win2,poz,VV->PX+1);
    wprintw(win2,"%*s",i," ");

    i--;
    if(i < 0)
      i=0;
    parol[i]='\0';
    wmove(win2,poz,VV->PX+1);
    for(int iii=0; iii < i; iii++)
       wprintw(win2,"*");
    wmove(win2,poz,VV->PX+1+i);
    
    i--;
    if(i < -1)
      i=0;
    wrefresh(win2);
    continue;
   }
  if(i >= dls)
   {
    parol[i]='\0';
    parol[i-1]='\0';
    i--;
    beep();
    wmove(win2,poz,VV->PX+1+i);
    continue;
   }
  wprintw(win2,"*");

 }


wattroff(win2,VV->VVOD_return_cs(iceb_CFV)); 

wrefresh(win2);
delwin(win2);

touchwin(win1);
wrefresh(win1);
attron(VV->VVOD_return_cs(iceb_CB));
return(K);
}
