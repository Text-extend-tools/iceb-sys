/*$Id: init_screen.c,v 5.12 2013/05/17 14:56:35 sasa Exp $*/
/*20.06.2012	05.07.1998	Белых А.И.	init_screen.c
Инициализация работы экрана
0 COLOR_BLECK    Черный
1 COLOR_RED      Красный
2 COLOR_GREEN    Зеленый
3 COLOR_YELLOW   Коричневый/Желтый (включается с повышенной яркостью)
4 COLOR_BLUE     Голубой
5 COLOR_MAGENTA  Фиолетовый
6 COLOR_CYAN     Сиреневый
7 COLOR_WHITE    Белый
*/

#include        "scr.h"

long		cklav,cstr; /*Для helstr.c свет клавиши и строки*/
int		iscolor; /*0-черно-белый монитор*/

int init_screen() 
{
unsigned short		i,i1;

//printf("init_screen-----\n");

initscr();  /*Запуск экранного режима*/ 

/*--------------- Kлавиатура ---------------------------*/

meta(stdscr,TRUE); /*Разрешение ввода 8ми-битных символов в getch()*/ 
nl(); /*Разрешение ВК*/
cbreak(); /*Разрещение посимвольного ввода*/ 
noecho(); /* Отключить эхопечать */ 
scrollok(stdscr,TRUE); /*Разрешение ролинга на экране*/

putp(keypad_local);  /*Включение цифр на дополнительной клавиатуре*/
/*   putp(keypad_xmit); Выключение цифр на дополнительной клавиатуре*/
keypad(stdscr,TRUE); /*Разрешить распознавать функциональные клавиши*/
cklav=A_NORMAL;
cstr=A_REVERSE; /*Инверсия*/


start_color(); 

if((iscolor = has_colors()) != 0) 
 {
  /* на некоторых терминалах необходимо сначала включить 
     цветовую гамму по умолчанию */ 
  use_default_colors();
  
  /*Назначаем цветовые пары*/
  for(i=0 ; i < COLORS ;i++)
    for(i1=0 ; i1 < COLORS ;i1++)
      init_pair(i*COLORS+i1,i,i1); 

  cklav |= COLOR_PAIR(COLOR_WHITE*COLORS+COLOR_BLACK);
  cstr = COLOR_PAIR(COLOR_BLACK*COLORS+COLOR_CYAN);

 }

mousemask(ALL_MOUSE_EVENTS,NULL);
/*при запуске программы в гарфической оболочке на ввод иногда подбрасывает мусор, надо от него избавиться*/
timeout(0); //устанавливаем таймаут для считывания ввода 
for(int nom=0; nom < 3;nom++)
 {
  getch();
 }
timeout(-1); //*отменяем таймаут для считывания ввода тоесть ждать до нажатия клавиши

return(0);
}     
