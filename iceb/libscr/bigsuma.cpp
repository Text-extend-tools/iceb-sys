/*$Id: bigsuma.c,v 5.7 2013/08/13 05:50:26 sasa Exp $*/
/*06.05.2003	09.04.2003	Белых А.И.	bigsuma.c
Вывод на экран числа с увеличенным размером 
*/
#include	<stdio.h>
#include	<scr.h>

void    minus(int,int);
void    odin(int,int);
void    dva(int,int);
void    tri(int,int);
void	hetire(int,int);
void	pqt(int,int);
void	hest(int,int);
void    sem(int,int);
void    vosem(int,int);
void    devqt(int,int);
void    nolik(int,int);
void    zapqtaq(int,int);
void    bigramka(int,int,int,const char*);

/*
           ■■■■■  ■■■■■ ■■■■■ ■■■■■ ■■■■■ ■■■■■ ■   ■ ■■■■■ ■■■■■    ■■
           ■   ■  ■   ■ ■   ■     ■ ■     ■     ■   ■     ■     ■   ■■■
           ■   ■  ■   ■ ■   ■     ■ ■     ■     ■   ■     ■     ■     ■
     ■■■■■ ■   ■  ■■■■■ ■■■■■     ■ ■■■■■ ■■■■■ ■■■■■   ■■■ ■■■■■     ■
           ■   ■      ■ ■   ■     ■ ■   ■     ■     ■     ■ ■         ■
           ■   ■      ■ ■   ■     ■ ■   ■     ■     ■     ■ ■         ■ ■■ 
           ■■■■■  ■■■■■ ■■■■■     ■ ■■■■■ ■■■■■     ■ ■■■■■ ■■■■■     ■ ■■
                                                                         ■
*/



void	bigsuma(double suma,const char *zagol)
{
extern int	iscolor;
char		stroka[50];
int		kolsimv=0;
int		kolstrzag=0;

memset(stroka,'\0',sizeof(stroka));
sprintf(stroka,"%.2f",suma);
kolsimv=strlen(stroka);

int dlinastr=(kolsimv-1)*6-1+3;
int X=(COLS-dlinastr)/2;
int dlinamenu=strlen(zagol);
if(dlinamenu > 0)
 kolstrzag=1;
 
if(dlinastr > dlinamenu)
  dlinamenu=dlinastr;

int kols=0;
if(kolstrzag > 0)
 kols=kolstrzag+1;  

int Y=(LINES-7-kols)/2;

//printw("\n bigsuma-COLS=%d dlinastr=%d X=%d stroka=%s\n",COLS,dlinastr,X,stroka);
//OSTANOV();

//clear();
bigramka(Y,X,dlinastr,zagol);
Y+=kols;

if(iscolor != 0)
 attron(COLOR_PAIR(COLOR_WHITE*COLORS+COLOR_RED));
else
 attron(A_REVERSE);

for(int i=0 ; i < dlinastr; i++)
 {
  switch(stroka[i])
   {
    case '0':
     nolik(Y,X);
     X+=6;
     break;

    case '1':
     odin(Y,X);
     X+=6;
     break;

    case '2':
     dva(Y,X);
     X+=6;
     break;

    case '3':
     tri(Y,X);
     X+=6;
     break;

    case '4':
     hetire(Y,X);
     X+=6;
     break;

    case '5':
     pqt(Y,X);
     X+=6;
     break;

    case '6':
     hest(Y,X);
     X+=6;
     break;

    case '7':
     sem(Y,X);
     X+=6;
     break;
              

    case '8':
     vosem(Y,X);
     X+=6;
     break;

    case '9':
     devqt(Y,X);
     X+=6;
     break;

    case '.':
     zapqtaq(Y+5,X);
     X+=3;
     break;

    case '-':
     minus(Y+3,X);
     X+=6;
     break;
   }
 }


if(iscolor != 0)
 attroff(COLOR_PAIR(COLOR_WHITE*COLORS+COLOR_RED));
else
 attroff(A_REVERSE);

OSTANOV();

}

void   odin(int Y,int X)
{

/*
   ■■
  ■■■
    ■
    ■
    ■
    ■
    ■
*/

mvprintw(Y,X+3,"  ");
mvprintw(++Y,X+2,"   ");
mvprintw(++Y,X+4," ");
mvprintw(++Y,X+4," ");
mvprintw(++Y,X+4," ");
mvprintw(++Y,X+4," ");
mvprintw(++Y,X+4," ");

}

void dva(int Y,int X)
{

/*
■■■■■
    ■
    ■
■■■■■
■
■
■■■■■
*/

mvprintw(Y,X,"     ");
mvprintw(++Y,X+4," ");
mvprintw(++Y,X+4," ");
mvprintw(++Y,X,"     ");
mvprintw(++Y,X," ");
mvprintw(++Y,X," ");
mvprintw(++Y,X,"     ");




}

void tri(int Y,int X)
{

/*
■■■■■
    ■
    ■
  ■■■
    ■
    ■
■■■■■
*/

mvprintw(Y,X,"     ");
mvprintw(++Y,X+4," ");
mvprintw(++Y,X+4," ");
mvprintw(++Y,X+2,"   ");
mvprintw(++Y,X+4," ");
mvprintw(++Y,X+4," ");
mvprintw(++Y,X,"     ");

}

void	hetire(int Y,int X)
{
/*
■   ■
■   ■
■   ■
■■■■■
    ■
    ■
    ■
*/
mvprintw(Y,X," ");
mvprintw(Y,X+4," ");

mvprintw(++Y,X," ");
mvprintw(Y,X+4," ");

mvprintw(++Y,X," ");
mvprintw(Y,X+4," ");


mvprintw(++Y,X,"     ");
mvprintw(++Y,X+4," ");
mvprintw(++Y,X+4," ");
mvprintw(++Y,X+4," ");
}

void pqt(int Y,int X)
{

/*
■■■■■
■
■
■■■■■
    ■
    ■
■■■■■
*/
mvprintw(Y,X,"     ");
mvprintw(++Y,X," ");
mvprintw(++Y,X," ");
mvprintw(++Y,X,"     ");
mvprintw(++Y,X+4," ");
mvprintw(++Y,X+4," ");
mvprintw(++Y,X,"     ");

}

void hest(int Y,int X)
{

/*
■■■■■
■
■
■■■■■
■   ■
■   ■
■■■■■
*/
mvprintw(Y,X,"     ");
mvprintw(++Y,X," ");
mvprintw(++Y,X," ");
mvprintw(++Y,X,"     ");

mvprintw(++Y,X," ");
mvprintw(Y,X+4," ");

mvprintw(++Y,X," ");
mvprintw(Y,X+4," ");

mvprintw(++Y,X,"     ");


}

void	sem(int Y,int X)
{
/*
■■■■■
    ■
    ■
    ■
    ■
    ■
    ■
*/
mvprintw(Y,X,"     ");
mvprintw(++Y,X+4," ");
mvprintw(++Y,X+4," ");
mvprintw(++Y,X+4," ");
mvprintw(++Y,X+4," ");
mvprintw(++Y,X+4," ");
mvprintw(++Y,X+4," ");



}

void vosem(int Y,int X)
{

/*
■■■■■
■   ■
■   ■
■■■■■
■   ■
■   ■
■■■■■
*/
mvprintw(Y,X,"     ");

mvprintw(++Y,X," ");
mvprintw(Y,X+4," ");

mvprintw(++Y,X," ");
mvprintw(Y,X+4," ");

mvprintw(++Y,X,"     ");

mvprintw(++Y,X," ");
mvprintw(Y,X+4," ");

mvprintw(++Y,X," ");
mvprintw(Y,X+4," ");

mvprintw(++Y,X,"     ");

}


void devqt(int Y,int X)
{
/*
■■■■■
■   ■
■   ■
■■■■■
    ■
    ■
■■■■■
*/
mvprintw(Y,X,"     ");

mvprintw(++Y,X," ");
mvprintw(Y,X+4," ");

mvprintw(++Y,X," ");
mvprintw(Y,X+4," ");

mvprintw(++Y,X,"     ");

mvprintw(++Y,X+4," ");
mvprintw(++Y,X+4," ");

mvprintw(++Y,X,"     ");

}

void nolik(int Y,int X)
{
/*
■■■■■
■   ■
■   ■
■   ■
■   ■
■   ■
■■■■■
*/
mvprintw(Y,X,"     ");

mvprintw(++Y,X," ");
mvprintw(Y,X+4," ");

mvprintw(++Y,X," ");
mvprintw(Y,X+4," ");

mvprintw(++Y,X," ");
mvprintw(Y,X+4," ");

mvprintw(++Y,X," ");
mvprintw(Y,X+4," ");

mvprintw(++Y,X," ");
mvprintw(Y,X+4," ");

mvprintw(++Y,X,"     ");

}

void zapqtaq(int Y,int X)
{
/*
■■
■■
 ■
*/
mvprintw(Y,X,"  ");
mvprintw(++Y,X,"  ");
mvprintw(++Y,X+1," ");


}

void minus(int Y,int X)
{

//■■■■■
mvprintw(Y,X,"     ");

}

/********************/
/*   Рисуем рамку   */
/********************/

void  bigramka(int Y,int X,int dlinastr,const char *zagol)
{

attron(A_ALTCHARSET); /*Альтернативная таблица*/

int PY=Y-1;
int PX=X-1;
int KLST=7;
int i=0;
int kolelemzag=0;
int		shethik=0;

if(zagol[0] != '\0')
 {
  KLST+=2;
  kolelemzag=1;
 } 
move(PY,PX);
int MDLS=dlinastr;

printw("%c",ACS_ULCORNER);      /*Левый уголок*/
for(i= PX; i< PX+(MDLS);i++)
  printw("%c",ACS_HLINE);      /*Горизонтальная полка*/

printw("%c",ACS_URCORNER);       /*Правый уголок*/
int iii=0;
for(i= PY;i< PY+(KLST)+iii;i++)
 {
  shethik++;
  attron( A_ALTCHARSET );
  move(i+1, PX);
  printw("%c",ACS_VLINE);   /*Вертикальная стенка*/
  if(kolelemzag != 0 && shethik == kolelemzag+1)
   {
    move(i+1,PX+1);
    for(int i1=PX;i1<PX+MDLS;i1++)
      printw("%c",ACS_HLINE);      /*Горизонтальная полка*/
   }
  else
   {

    attroff( A_ALTCHARSET );
    attron(A_NORMAL );
    for(int i1= PX;i1< PX+(MDLS);i1++)
      printw(" ");
   
    attroff( A_NORMAL );
    attron( A_ALTCHARSET );
   }
  move(i+1,PX+(MDLS)+1);
  printw("%c",ACS_VLINE);         /*Вертикальная стенка*/
  attron(A_NORMAL);
/****************
  if( TEN == 1)
   {
    attron(A_REVERSE);
    printw("%c%c",TENN,TENN);
    attroff(A_REVERSE);
   }
************/ 
 }
attron( A_ALTCHARSET );


move(i+1, PX);
printw("%c",ACS_LLCORNER);   /*Левый нижний уголок*/
for(i= PX;i< PX+(MDLS);i++)
  printw("%c",ACS_HLINE);    /*Горизонтальная полка*/
printw("%c",ACS_LRCORNER);     /*Правый нижний уголок*/

attroff( A_ALTCHARSET );


//attron( CFM);
if(zagol[0] != '\0')
 {
  move(PY+1,PX+1);
  printw("%.*s",MDLS,zagol);
 } 



}
