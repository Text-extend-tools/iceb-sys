/*$Id: klaviat.c,v 5.15 2013/05/17 14:56:48 sasa Exp $*/
/*15.11.2009    29.05.1991      KLAVIAT.C
Программа получения кодов клавиш
*/
#include <glib.h>
#ifdef	HAVE_CONFIG_H
#include 				<iceb.h>
#endif
#include        <stdlib.h>
#include <ctype.h>
#include <stdio.h>
//#ifdef USE_NCURSES
#include <ncurses.h>
//#else
//#ifdef USE_STDCURSES
//#include <curses.h>
//#endif
//#endif
#include        <locale.h>

#include <term.h>
#include <string.h>
int  main()
{
int key;
int     i;
MEVENT	event;
WINDOW *win1;   //Окно на котором будет расположено новое окно
WINDOW *okno=NULL;   //Окно на котором будет расположено новое окно

(void)setlocale(LC_ALL,"");
(void)setlocale(LC_NUMERIC,"C"); //Переменная среды определяющая чем 
                                 //разделяется целая и дробная часть числа

initscr();
meta(stdscr,TRUE); /*Разрешение ввода 8ми-битных символов в getch()*/ 

nl();
keypad(stdscr,TRUE);  /*Разрешить распознавать функциональные клавиши*/
noecho();
cbreak();  /*Разрещение посимвольного ввода*/

scrollok(stdscr,TRUE); //Разрешена прокрутка в окне
putp(keypad_local);  /*Включение цифр на дополнительной клавиатуре*/
/*   putp(keypad_xmit); Выключение цифр на дополнительной клавиатуре*/
keypad(stdscr,TRUE); /*Разрешить распознавать функциональные клавиши*/

int PY=0,PX=0;
if((win1=newwin(LINES,COLS,PY,PX)) == NULL)
 {
  beep();
  wprintw(okno,"\nОшибка создания окна !\n");
  wrefresh(okno);
  endwin();
  exit(1);
 }
scrollok(win1,TRUE); //Разрешена прокрутка в окне
keypad(win1,TRUE);  /*Разрешить распознавать функциональные клавиши*/

//okno=stdscr;
okno=win1;

wprintw(okno,"mousmask=%d\n",mousemask(ALL_MOUSE_EVENTS,NULL));
//printw("mousmask=%d\n",mousemask(BUTTON1_CLICKED,NULL));
wprintw(okno,"Программа проверки кодов клавиш. Выход \"0\"\n");
wrefresh(okno);
    /* Read and display keys until ESC is pressed. */
//curs_set(0);
for(i=0; ; i++)
 {
	/* If first key is 0, then get second extended. */
//  key = getch();
//  key = wgetch(stdscr);
  key = wgetch(okno);
  if(key == KEY_MOUSE)
   {
    wprintw(okno,"Мышка\n");
    if(getmouse(&event) == OK)
     {
      if(event.bstate & BUTTON1_PRESSED)
       wprintw(okno,"Первая кнопка нажата. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      int metka=0;
      if(event.bstate & BUTTON1_RELEASED)
       {
        metka=1;
        wprintw(okno,"Первая кнопка отпущена. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
       }
      if(event.bstate & BUTTON1_CLICKED)
       wprintw(okno,"Первая кнопка нажата и отпущена. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      if(event.bstate & BUTTON1_DOUBLE_CLICKED)
       wprintw(okno,"Первая кнопка нажата и отпущена дважды. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      if(event.bstate & BUTTON1_TRIPLE_CLICKED)
       wprintw(okno,"Первая кнопка нажата и отпущена трижды. x=%d y=%d z=%d.\n",event.x,event.y,event.z);

      if(event.bstate & BUTTON2_PRESSED)
       wprintw(okno,"Вторая кнопка нажата. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      if(event.bstate & BUTTON2_RELEASED)
       wprintw(okno,"Вторая кнопка отпущена. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      if(event.bstate & BUTTON2_CLICKED)
       wprintw(okno,"Вторая кнопка нажата и отпущена. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      if(event.bstate & BUTTON2_DOUBLE_CLICKED)
       wprintw(okno,"Вторая кнопка нажата и отпущена дважды. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      if(event.bstate & BUTTON2_TRIPLE_CLICKED)
       wprintw(okno,"Вторая кнопка нажата и отпущена трижды. x=%d y=%d z=%d.\n",event.x,event.y,event.z);

      if(event.bstate & BUTTON3_PRESSED)
       wprintw(okno,"Третья кнопка нажата. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      if(event.bstate & BUTTON3_RELEASED)
       wprintw(okno,"Третья кнопка отпущена. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      if(event.bstate & BUTTON3_CLICKED)
       wprintw(okno,"Третья кнопка нажата и отпущена. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      if(event.bstate & BUTTON3_DOUBLE_CLICKED)
       wprintw(okno,"Третья кнопка нажата и отпущена дважды. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      if(event.bstate & BUTTON3_TRIPLE_CLICKED)
       wprintw(okno,"Третья кнопка нажата и отпущена трижды. x=%d y=%d z=%d.\n",event.x,event.y,event.z);

      if(event.bstate & BUTTON4_PRESSED)
       wprintw(okno,"Четвертая кнопка нажата. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      if(event.bstate & BUTTON4_RELEASED)
       wprintw(okno,"Четвертая кнопка отпущена. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      if(event.bstate & BUTTON4_CLICKED)
       wprintw(okno,"Четвертая кнопка нажата и отпущена. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      if(event.bstate & BUTTON4_DOUBLE_CLICKED)
       wprintw(okno,"Четвертая кнопка нажата и отпущена дважды. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
      if(event.bstate & BUTTON4_TRIPLE_CLICKED)
       wprintw(okno,"Четвертая кнопка нажата и отпущена трижды. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
/****************************
      wprintw(okno,"event.bstate=%0x BUTTON1_PRESSED=%0x BUTTON_SHIFT=%0x\n",
      event.bstate, BUTTON1_PRESSED, BUTTON_SHIFT);
            
      if(event.bstate & (BUTTON1_PRESSED | BUTTON_SHIFT))
       wprintw(okno,"Первая кнопка нажата + Shift. x=%d y=%d z=%d.\n",event.x,event.y,event.z);

      if(event.bstate & BUTTON1_PRESSED & BUTTON_CTRL)
       wprintw(okno,"Первая кнопка нажата + Ctrl. x=%d y=%d z=%d.\n",event.x,event.y,event.z);


      if(event.bstate & BUTTON1_PRESSED & BUTTON_ALT)
       wprintw(okno,"Первая кнопка нажата + Alt. x=%d y=%d z=%d.\n",event.x,event.y,event.z);
**********************/
      wrefresh(okno);
      int Y=0,X=0;
      getyx(okno,Y,X);
 
      chtype simv;
      char stroka[1024];
      simv=mvwinch(okno,event.y,event.x);
      mvwinstr(okno,event.y,event.x,stroka);           
      wmove(okno,Y,X);
      wprintw(okno,"Прочитали в позиции клика символ=%c число=%d metka=%d\n",simv,simv,metka);
      wprintw(okno,"прочитали строку %s\n",stroka);
     }
         
    wrefresh(okno);
   }
  else
   {   
    wprintw(okno, " %d Char: %c ",i, key );
//    wprintw(okno, " %d Char: %c ",i,isgraph( key ) ? key : ' ' );
    wprintw(okno, "Dec:%d Hex:%X OC:%o intchar=%d", key, key, key,(char)key );

    gunichar kod=(char)key;
    if(g_unichar_validate(kod) == TRUE) /*символ в utf8 кодировке*/
//    if(g_unichar_isdefined(kod) == TRUE) /*символ в utf8 кодировке*/
     {
      wprintw(okno," utf8");
     }    
    wprintw(okno,"\n");
    wrefresh(okno);
   }
   /* Echo character response to prompt. */
  if( key == '0')
   {
    wprintw(okno, "Do you really want to quit? (Y/n) " );
    wrefresh(okno);
    key = getchar();
    wprintw(okno, "\n" );
    wrefresh(okno);

    if( (toupper( key ) == 'Y') || (key == 13) )
	break;
   }
 }
endwin();
}
