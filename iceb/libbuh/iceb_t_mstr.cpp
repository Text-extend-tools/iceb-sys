/*$Id: iceb_t_mstr.c,v 5.15 2012-10-30 09:24:32 sasa Exp $*/
/*09.11.2009	11.09.2007	Белых А.И.	iceb_t_mstr.c
Работа с меню из строк произвольной длинны
*/

#include "buhl.h"
extern int metka_utf8;
//Конструктор объекта

iceb_t_mstr::iceb_t_mstr(int nomr) //Номер раскраски меню
{
memset(LNU,'\0',sizeof(LNU));
memset(PNU,'\0',sizeof(PNU));
memset(LVU,'\0',sizeof(LVU));
memset(PVU,'\0',sizeof(PVU));
memset(VE,'\0',sizeof(VE));
memset(PO,'\0',sizeof(PO));

/*****************
Если кодировка utf8 то в линуксе всё работает првильно а в putty неправильно отображается рамка 
для того чтобы этого небыло подставляем псевдографику, которая всегда есть в этой кодировке
и которая правильно отображается везде
************************/

if(metka_utf8 == 0)  
 {
  LNU[0]=ACS_LLCORNER;
  PNU[0]=ACS_LRCORNER;
  LVU[0]=ACS_ULCORNER;
  PVU[0]=ACS_URCORNER;
  PO[0]=ACS_HLINE;
  VE[0]=ACS_VLINE;
  CFR = A_ALTCHARSET; /*Альтернативная таблица*/
 }
else 
 {
  strcpy(LNU,"└");
  strcpy(PNU,"┘");
  strcpy(LVU,"┌");
  strcpy(PVU,"┐");
  strcpy(PO,"─");
  strcpy(VE,"│");
  CFR = A_NORMAL; /*Не включаем алтернативную таблицу*/
  
 }
TENN=' ';

//CFR = A_ALTCHARSET; /*Альтернативная таблица*/
CFS = A_REVERSE; /*Инверсия*/
CFM = A_NORMAL;
CB =  A_NORMAL;

if(has_colors() != 0)  //Цветной
 {
  if(nomr == 0)
   {
    CFR |= COLOR_PAIR( COLOR_RED*8+COLOR_BLUE);
    CFS |= A_BOLD | COLOR_PAIR( COLOR_RED*8+COLOR_WHITE);
    CFM |= COLOR_PAIR( COLOR_WHITE*8+COLOR_BLACK);
    CB |= COLOR_PAIR( COLOR_WHITE*8+COLOR_BLACK);
   }

  if(nomr == 1)
   {
    CFR |= COLOR_PAIR(COLOR_RED*8+COLOR_BLUE);
    CFS |= A_BOLD | COLOR_PAIR( COLOR_RED*8+COLOR_WHITE);
    CFM |= COLOR_PAIR( COLOR_WHITE*8+COLOR_BLUE);
    CB |= COLOR_PAIR( COLOR_WHITE*8+COLOR_BLACK);
   }
 }

}

/*********************
Подпрограмма визуализации выбора нужного реквизита
ESC            -1
стрелка вверх если уперлись -2
стрелка вниз  если уперлись -3
страница вверх              -4
страница вниз               -5
прочитали сканерный ввод    -6
Изменение размеров экрана   -7

Если в начале программы N == -10 то нужно не дожидаясь опроса
клавиатуры выходить из подпрограммы.
*/



int iceb_t_mstr::menu(int N,  //Номер строки в массиве если -10 то выйти сразу после отображения меню
int	PY, //Позиция меню во вертикали
int	PX, //Позиция меню по горизонтали
int (*outvvod)(char*),
char *strk, //Строка содержит то что прочитали со сканера
int procp, //Процент просмотра
int *klav,  //Код клавиши
const char *nazvan, //Заголовок меню
int max_dlinna_stroki) /*Максимальная длинна строки 0-не задана*/
{
short           i,i1;
int             MDLS; /*Максимальная длинна строки*/
int             KLST; /*Количество заданных строк помещающихся на экран*/
int             K=0;  /*Принимает код клавиши*/
int             PR;
short		nsz; /*Номер строки со звездочкой*/
short		ss;  /*Счетчик строк*/
int		nomer=0;
int		kolstm=LINES-2-PY-1; //максимальное количество строк которое может поместиться на экране
int		PYY=PY;
short		metkasoob=0;
int zap_N=N;

if(N < 0)
 N=0;

if(nazvan[0] != '\0')
 metkasoob=2;
 
kolstm-=metkasoob;

//int lin=LINES-PY-2-1;
 
*klav=0;

nazad:;

attron(CFM);

KLST=spis_str.kolih();
MDLS=0;

for(i=nomer; i <  spis_str.kolih() ; i++)
  if((PR=iceb_tu_strlen(spis_str.ravno(i))) > MDLS)
     MDLS=PR;


if(nazvan[0] != '\0')
 {
  if((PR=iceb_tu_strlen(nazvan)) > MDLS)
     MDLS=PR;
  KLST+=2;
 }

if(max_dlinna_stroki > 0)
 if(MDLS > max_dlinna_stroki)
  MDLS=max_dlinna_stroki;

/*Первая проверка длинны строки*/
if(MDLS >  COLS-4)
  MDLS= COLS-4;

/*Вычисляем позицию по горизонтали*/
if(PX < 0)
  PX=( COLS-MDLS-4)/2;

/*Вычисляем позицию по вертикали*/
if(PY < 0)
 PY=(LINES-KLST-2)/2;

if(KLST > LINES-2-PY-1)
 KLST=LINES-2-PY-1;

/*Так как позиция по Х может быть задана, второй раз проверяем длинну строки*/
if(MDLS >  COLS-4-PX)
  MDLS= COLS-4-PX;

PYY=PY;

if(KLST == kolstm)
  nsz=procp*(KLST-1)/100;
else
  nsz=((nomer+KLST)*100/kolstm)*(KLST-1)/100;



/********************/
/*   Рисуем рамку   */
/********************/

attroff(CFM);
attron(CFR);

move(PY,PX);

printw("%s",LVU);      /*Левый уголок*/
for(i=PX;i<=PX+MDLS+1;i++)
  printw("%s",PO);      /*Горизонтальная полка*/
printw("%s",PVU);       /*Правый уголок*/
ss=0;
for(i=PY;i<PY+KLST && i < spis_str.kolih()+PY-nomer+metkasoob;i++)
 {
  ss++;
  move(i+1,PX);
  attron(CFR);

  printw("%s",VE);   /*Вертикальная стенка*/

  if(nazvan[0] != '\0' && ss == 2)
   {
    move(i+1,PX+1);
    for(i1=PX;i1<PX+MDLS+2;i1++)
     {
      printw("%s",PO);      /*Горизонтальная полка*/
     } 
   }
  else
   {
    attroff(CFR);
    attron(CFM);
    for(i1=PX;i1 < PX+MDLS+2;i1++)
     printw(" ");
    attroff(CFM);
    attron(CFR);
   }

  move(i+1,PX+MDLS+3);
  if(ss-1 != nsz)
    printw("%s",VE);         /*Вертикальная стенка*/
  else
   {
    attron(A_BOLD);
    attron(A_REVERSE);
    printw("%c",TENN);
    attroff(A_REVERSE);
    attroff(A_BOLD);
   }
 }

i1=i+2;
move(i+1,PX);
printw("%s",LNU);   /*Левый нижний уголок*/
for(i=PX;i<=PX+MDLS+1;i++)
  printw("%s",PO);    /*Горизонтальная полка*/
printw("%s",PNU);     /*Правый нижний уголок*/


refresh();

attroff( CFR);
attron( CFM);
if(nazvan[0] != '\0')
 {
  move(PY+1,PX+1);
  printw("%.*s",iceb_tu_kolbait(MDLS,nazvan),nazvan);
  PYY+=2;
  KLST-=2;
 } 

if(N > KLST-1)
  N=KLST-1;


move(PYY+2,PX+1);
i1=0;
for(i=nomer;i < KLST+nomer && i < spis_str.kolih();i++)
 {
  move(PYY+i1+1,PX+2);
  i1++;
  printw("%.*s",iceb_tu_kolbait(MDLS,spis_str.ravno(i)),spis_str.ravno(i));
 }
refresh();
if(zap_N == -10)
  {
    attron( CB);
    return(N);
  }

attron( CFS);

//mvprintw(0,COLS-20,"*KLST=%d N=%d",KLST,N);
//refresh();

move(PYY+N+1,PX+2);

printw("%.*s",iceb_tu_kolbait(MDLS,spis_str.ravno(N+nomer)),spis_str.ravno(N+nomer));
refresh();
PR=0;

/*Сброс сканера*/
if(outvvod != NULL)
 (*outvvod)(NULL);
int nzap=0;
for(;;)
 {
  if((outvvod != NULL))
   {
    if(((*outvvod)(strk) > 0)) 
     {
      /* получен ввод с внешнего устройства */
        attroff( CFS);
        attron( CFM);
        move(PYY+N+1,PX+2);
        printw("%.*s",iceb_tu_kolbait(MDLS,spis_str.ravno(N+nomer)),spis_str.ravno(N+nomer));
        refresh();

        N=(-6);
        goto konn;
     }
    else
      *klav=K=getch();
    
   }
  else 
   {
    /* получен ввод с клавиатуры или ошибка внешнего устройства */
    *klav=K=getch();
   }

//  printw("\n%s-%d\n",__FUNCTION__,K);
//  OSTANOV();  
  attroff(CFS);
  attron(CFM);
  move(PYY+N+1,PX+2);
  printw("%.*s",iceb_tu_kolbait(MDLS,spis_str.ravno(N+nomer)),spis_str.ravno(N+nomer));

  if(K == KEY_MOUSE)
     *klav=K=mouseklic(&N,PYY,PX,MDLS,KLST,stdscr);


  switch(K)
   {
    case SDN: /*Стрелка вниз*/
      
      N++;
      break;

    case SUP: /*Стрелка вверх*/
      N--;
      break;

    case HOME: /*На первую строчку меню*/
     N=0;
     break;
     
    case END: /*На последнюю строчку меню*/
      N=KLST-2;
      break;

    case ESC:
/*
#ifndef M_UNIX
      getch();
#endif
*/
      attroff( CFS);
      attron( CFM);
      move(PYY+N+1,PX+2);
      printw("%.*s",iceb_tu_kolbait(MDLS,spis_str.ravno(N+nomer)),spis_str.ravno(N+nomer));
      refresh();

      N=-1;
      goto konn;

    case FK10:
      attroff( CFS);
      attron( CFM);
      move(PYY+N+1,PX+2);
      printw("%.*s",iceb_tu_kolbait(MDLS,spis_str.ravno(N+nomer)),spis_str.ravno(N+nomer));
      refresh();

      N=-1;
      goto konn;

    case ENTER:
    case FK1:
    case FK2:
    case FK3:
    case FK4:
    case FK5:
    case FK6:
    case FK7:
    case FK8:
    case FK9:
    case SFK1:
    case SFK2:
    case SFK3:
    case SFK4:
    case SFK5:
    case SFK6:
    case SFK7:
    case SFK8:
    case SFK9:
    case SFK10:
    case PLU:
    case INS:
    case DELETE:
    case DEL:
    case NOL:
    case OTN:
    case UMN:
    case TAB:
      /*Для токо чтобы подсветка строки не сбрасывалась после выхода */
      attroff(CFM);
      attron(CFS);
      move(PYY+N+1,PX+2);
      printw("%.*s",iceb_tu_kolbait(MDLS,spis_str.ravno(N+nomer)),spis_str.ravno(N+nomer));
      goto konn;

    case PU: //Страница вверх
      N =(-4);
      goto konn;


    case PD: //Страница вниз
      N =(-5);
      goto konn;

    case KEY_RESIZE: //Изменение размеров экрана
//      getch();
      N=-7;
      goto konn;

   }
    
  if(N > KLST-1 ||  N >= spis_str.kolih()-nomer) //Стрелка вниз
   {
    nzap=N;
    N=-3;
    break;
   }

  if(N < 0) //Стрелка вверх
   {
    N=-2;
    break;
   }


  attroff(CFM);
  attron(CFS);
  move(PYY+N+1,PX+2);
  printw("%.*s",iceb_tu_kolbait(MDLS,spis_str.ravno(N+nomer)),spis_str.ravno(N+nomer));
  refresh();
 }

konn:;

if(spis_str.kolih() > kolstm && N <= -2 && N >= -5)
 {
  
  if(N == -2) /*стрелка вверх*/
   {
    nomer--;
    N=0;
   }

  if(N == -3) /*Стрелка вниз*/
   {
    if(nomer+kolstm < spis_str.kolih())
      nomer++;
    else
     beep();
    N=nzap-1;
   }

  if(N == -4) /*Страница вверх*/
   {
    nomer-=kolstm;
    N=0;
   }
  if(N == -5) /*Страница вниз*/
   {
    if(nomer+kolstm < spis_str.kolih())
     {
      nomer+=kolstm;
     }
    else
     beep();
    N=0;
   }  

  if(nomer < 0)
   {
    nomer=0;
    beep();
   }
  if(nomer > spis_str.kolih())
   {
    beep();
    nomer=spis_str.kolih()-1;    
   }
  goto nazad;
 }

//attroff(CFS);
//attron(CB);
standend(); //Установить нормальный режим отображения

refresh();

return(N+nomer);

}
