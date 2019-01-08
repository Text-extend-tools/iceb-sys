/*$Id:$*/
/*14.05.2016	14.05.2016	Белых А.И.	admin_klaviat.c
определение кодов клавиш
*/
#include <ctype.h>

#include "buhg.h"

void admin_klaviat()
{
int key=0;
MEVENT	event;

clear();

printw("mousmask=%d\n",mousemask(ALL_MOUSE_EVENTS,NULL));
//printw("mousmask=%d\n",mousemask(BUTTON1_CLICKED,NULL));
printw("%s\n",gettext("Нажмите любую клавишу на клавиатуре для колучения её кода. Выход \"0\""));
refresh();
    /* Read and display keys until ESC is pressed. */
//curs_set(0);
for(int i=0; ; i++)
 {
	/* If first key is 0, then get second extended. */
//  key = getch();
  key = wgetch(stdscr);

  if(key == KEY_MOUSE)
   {
    printw("Мышка\n");
    if(getmouse(&event) == OK)
     {
      if(event.bstate & BUTTON1_PRESSED)
       printw("Первая кнопка нажата. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      int metka=0;
      if(event.bstate & BUTTON1_RELEASED)
       {
        metka=1;
        printw("Первая кнопка отпущена. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
       }
      if(event.bstate & BUTTON1_CLICKED)
       printw("Первая кнопка нажата и отпущена. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      if(event.bstate & BUTTON1_DOUBLE_CLICKED)
       printw("Первая кнопка нажата и отпущена дважды. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      if(event.bstate & BUTTON1_TRIPLE_CLICKED)
       printw("Первая кнопка нажата и отпущена трижды. x=%d y=%d z=%d.\n",event.x,event.y,event.z);

      if(event.bstate & BUTTON2_PRESSED)
       printw("Вторая кнопка нажата. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      if(event.bstate & BUTTON2_RELEASED)
       printw("Вторая кнопка отпущена. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      if(event.bstate & BUTTON2_CLICKED)
       printw("Вторая кнопка нажата и отпущена. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      if(event.bstate & BUTTON2_DOUBLE_CLICKED)
       printw("Вторая кнопка нажата и отпущена дважды. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      if(event.bstate & BUTTON2_TRIPLE_CLICKED)
       printw("Вторая кнопка нажата и отпущена трижды. x=%d y=%d z=%d.\n",event.x,event.y,event.z);

      if(event.bstate & BUTTON3_PRESSED)
       printw("Третья кнопка нажата. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      if(event.bstate & BUTTON3_RELEASED)
       printw("Третья кнопка отпущена. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      if(event.bstate & BUTTON3_CLICKED)
       printw("Третья кнопка нажата и отпущена. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      if(event.bstate & BUTTON3_DOUBLE_CLICKED)
       printw("Третья кнопка нажата и отпущена дважды. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      if(event.bstate & BUTTON3_TRIPLE_CLICKED)
       printw("Третья кнопка нажата и отпущена трижды. x=%d y=%d z=%d.\n",event.x,event.y,event.z);

      if(event.bstate & BUTTON4_PRESSED)
       printw("Четвертая кнопка нажата. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      if(event.bstate & BUTTON4_RELEASED)
       printw("Четвертая кнопка отпущена. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      if(event.bstate & BUTTON4_CLICKED)
       printw("Четвертая кнопка нажата и отпущена. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      if(event.bstate & BUTTON4_DOUBLE_CLICKED)
       printw("Четвертая кнопка нажата и отпущена дважды. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      if(event.bstate & BUTTON4_TRIPLE_CLICKED)
       printw("Четвертая кнопка нажата и отпущена трижды. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
/****************************
      printw("event.bstate=%0x BUTTON1_PRESSED=%0x BUTTON_SHIFT=%0x\n",
      event.bstate, BUTTON1_PRESSED, BUTTON_SHIFT);
            
      if(event.bstate & (BUTTON1_PRESSED | BUTTON_SHIFT))
       printw("Первая кнопка нажата + Shift. x=%d y=%d z=%d.\n",event.x,event.y,event.z);

      if(event.bstate & BUTTON1_PRESSED & BUTTON_CTRL)
       printw("Первая кнопка нажата + Ctrl. x=%d y=%d z=%d.\n",event.x,event.y,event.z);


      if(event.bstate & BUTTON1_PRESSED & BUTTON_ALT)
       printw("Первая кнопка нажата + Alt. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
**********************/
      refresh();
      int Y=0,X=0;
      getyx(stdscr,Y,X);
 
      chtype simv;
      char stroka[1024];
      simv=mvwinch(stdscr,event.y,event.x);
      mvwinstr(stdscr,event.y,event.x,stroka);           
      wmove(stdscr,Y,X);
      printw("Прочитали в позиции клика символ=%c число=%d metka=%d\n",simv,simv,metka);
      printw("прочитали строку %s\n",stroka);
     }
         
    refresh();
   }
  else
   {   
    printw(" %d Char: %c ",i, key );
//    printw " %d Char: %c ",i,isgraph( key ) ? key : ' ' );
    printw("Dec:%d Hex:%X OC:%o intchar=%d", key, key, key,(char)key );

    gunichar kod=(char)key;
    if(g_unichar_validate(kod) == TRUE) /*символ в utf8 кодировке*/
//    if(g_unichar_isdefined(kod) == TRUE) /*символ в utf8 кодировке*/
     {
      printw(" utf8");
     }    
    printw("\n");
    refresh();
   }
   /* Echo character response to prompt. */
  if( key == '0')
   {
    printw("Do you really want to quit? (Y/n) " );
    refresh();
//    key = getchar();
    key = wgetch(stdscr);
    printw("\n" );
    refresh();

    if( (toupper( key ) == 'Y') || (key == 13) )
	break;
   }
 }


}
