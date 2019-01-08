/*$Id: mouseklic.c,v 5.10 2013/05/17 14:56:35 sasa Exp $*/
/*12.08.2015    23.07.2002      Белых А.И.     mouseklic.c
Подпрограмма получения нужного меню пользователя
работающая с псевдографикой.
*/
#include <glib.h>
#include        <time.h>
#include        <ctype.h>
#include        "scr.h"

int mousfk(int X,int metka,WINDOW *win2);

int  mouseklic(int *N,int PY,int PX,int MDLS,int KLST,
WINDOW *win2)
{
MEVENT	        event;

if(getmouse(&event) == OK)
 {
//  if(event.bstate & BUTTON1_CLICKED || event.bstate & BUTTON1_RELEASED) //Первая кнопка нажата и отпущена
  if(event.bstate & BUTTON1_CLICKED) //Первая кнопка нажата и отпущена
   {
    if(event.y == LINES-1)
      return(mousfk(event.x,0,win2));
    if(event.x >= PX+1 && event.y >= PY+1 && \
    event.x <= PX+1+MDLS && event.y <= PY+KLST)
       *N=event.y - (PY+1);

    if(event.y == PY+KLST+1)
      return(SDN); //Стрелка вниз

    if(event.y == PY)
      return(SUP); //Стрелка вверх

   }
  if(event.bstate & BUTTON3_CLICKED) //Третья кнопка нажата и отпущена.
   {
    if(event.y == LINES-1)
      return(mousfk(event.x,1,win2));
    if(event.x >= PX+1 && event.y >= PY+1 && \
    event.x <= PX+1+MDLS && event.y <= PY+KLST)
     {
       *N=event.y - (PY+1);
      return(ENTER);
     }
   }

  if(event.bstate & BUTTON1_DOUBLE_CLICKED) //Первая кнопка нажата и отпущена дважды.
   {
    if(event.y == PY+KLST+1)
      return(PD); //Страница вниз

    if(event.y == PY)
      return(PU); //Страница вверх

    if(event.y == LINES-1)
      return(mousfk(event.x,0,win2));
    if(event.x >= PX+1 && event.y >= PY+1 && \
    event.x <= PX+1+MDLS && event.y <= PY+KLST)
     {
      *N=event.y - (PY+1);
      return(ENTER);
     }
   }
  if(event.bstate & BUTTON3_DOUBLE_CLICKED) //Третья кнопка нажата и отпущена дважды.
    return(FK10);


 }
return(0);
}

/****************************************************************/
/*Программа определения выбранной мышкой функциональной клавиши*/
/***************************************************************/

int mousfk(int X,int metka,WINDOW *win2)
{
chtype	simv;
int	K=FK1;
int     SFK=0;


if(metka == 1)
 SFK=MAXFKEYS;

char stroka[sizeof(wchar_t)*COLS+1];
mvwinstr(win2,LINES-1,0,stroka);

if(g_utf8_validate(stroka,-1,NULL) == FALSE) /*однобайтная кодировка*/
 {
  for(int i=0; i <= X; i++)
   {
    simv=mvinch(LINES-1,i);
  //  mvprintw(LINES-3,0,"simv=%c/%d i=%d X=%d ***=%c/%d",simv,simv,i,X,'4','4');  
    if((char)simv == '1')
     K=FK1+SFK;
    if((char)simv == '2')
     K=FK2+SFK;
    if((char)simv == '3')
     K=FK3+SFK;
    if((char)simv == '4')
      K=FK4+SFK;
    if((char)simv == '5')
      K=FK5+SFK;
    if((char)simv == '6')
      K=FK6+SFK;
    if((char)simv == '7')
      K=FK7+SFK;
    if((char)simv == '8')
      K=FK8+SFK;
    if((char)simv == '9')
      K=FK9+SFK;
    if((char)simv == '0')
      K=FK10+SFK;
   }
 }
else /*utf8 кодировка*/
 {
  gchar *simvu;
  for(int i=0; i <= X; i++)
   {
     simvu=g_utf8_offset_to_pointer(stroka,i);

    if(simvu[0] == '1')
     K=FK1+SFK;
    if(simvu[0] == '2')
     K=FK2+SFK;
    if(simvu[0] == '3')
     K=FK3+SFK;
    if(simvu[0] == '4')
      K=FK4+SFK;
    if(simvu[0] == '5')
      K=FK5+SFK;
    if(simvu[0] == '6')
      K=FK6+SFK;
    if(simvu[0] == '7')
      K=FK7+SFK;
    if(simvu[0] == '8')
      K=FK8+SFK;
    if(simvu[0] == '9')
      K=FK9+SFK;
    if(simvu[0] == '0')
      K=FK10+SFK;
   }
 }
return(K);
}

