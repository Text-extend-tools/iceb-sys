/*$Id: mygetstr.c,v 5.14 2013/05/17 14:56:35 sasa Exp $*/
/*05.11.2007    15.06.1993      Белых А.И.      mygetstr.c
Подпрограмма ввода и редактирования символьной строки
*/
#include        "scr.h"

int             mygetstr(char str[],  //Строка для редактирования
int dls,  //Максимально допустимая длинна строки
WINDOW *okno)  //Указатель на окно
{
int             poz; /*Позиция маркера в строке*/
int             K;
int             i;
short           kl;
int             xn,yn; /*Начальные значения*/
short           mu; /*Метка удаления символа*/
//MEVENT	        event;
int		N=0;

noecho();            /* Отключить эхопечать */
getyx(okno,yn,xn);
wprintw(okno,"%.*s",dls-1,str);
//poz=strlen(str);
wmove(okno,yn,xn);
poz=0;
mu=0;

for(;;)
 {
  wrefresh(okno);
//  mvprintw(LINES-3,0,"mygetstr K=%d",K);
//  refresh();
  K=wgetch(okno);
//  K=getch();

  if(K == KEY_MOUSE)
   {
    K=mouseklic(&N,yn+1,xn-1,dls,1,okno);
    break;
   }
/********
   if(getmouse(&event) == OK)
    {
     break;
    }
********/
  if( K == HOME)
   {
    wmove(okno,yn,xn);
    poz=0;
    continue;
   }
  if( K == END)
   {
    poz=strlen(str);
    wmove(okno,yn,xn+poz);
    continue;
   }
  if( K == SLEFT)
   {
    if(poz > 0)
     {
      poz--;
      wmove(okno,yn,xn+poz);
   /*   wprintw(okno,"\b");*/

     }
    continue;
   }
  if( K == SRITH)
   {
    kl=strlen(str);
  /*if(kl == dls-1)*/
    if(poz == dls-1)
     {
      beep();
      refresh();
      continue;
     }
    poz++;
    /*Если стоим в конце строки*/
    if(kl == poz-1)
     {
      str[poz-1]=' ';
      str[poz]='\0';
      goto kkk;
     }
    wmove(okno,yn,xn+poz);
    continue;
   }
  if( K == INS)
   {

    kl=strlen(str);
    if(kl+1 >= dls)
     {
      beep();
      continue;
     }    

    if(poz <= kl)
     {
      for(i=kl+1; i>=poz;i--)
	str[i+1]=str[i];

      str[poz]=' ';

      goto kkk;
     }
    continue;
   }
  if( K == DELETE)
   {
    kl=strlen(str);
    for(i=poz; i<kl;i++)
      str[i]=str[i+1];
    mu++;
    goto kkk;
   }
  if( K == BACK)
   {
    if(poz > 0)
     {
      poz--;
      kl=strlen(str);
      for(i=poz; i<kl;i++)
	str[i]=str[i+1];
      mu++;
      goto kkk;
     }
    continue;
   }

  if( K == ENTER)
    break;
  if( K == SDN)
    break;
  if( K == SUP)
    break;
  if( K == TAB) //Очистка строки
   {
    poz=0;
    memset(str,'\0',dls);
    wmove(okno,yn,xn);
    wprintw(okno,"%*s",dls-1," ");
    wmove(okno,yn,xn);
    wrefresh(okno);
    continue;
   }
/*  if( K == ESC)*/
  if(K == ESC)
   {
/*
#ifndef M_UNIX
     getch();
#endif
*/
    break;
   }

  if(K == FK1 || K == FK2 || K == FK3 || K == FK4 || K == FK5
   || K == FK6 || K == FK7 || K == FK8 || K == FK9 || K == FK10)
    break;

  if(K == KEY_RESIZE) //Изменении размера окна
   {
    getch();
    break;
   }

  kl=strlen(str);
  /*Если режим вставки то надо включит эту проверку*/
  /*
  if(kl == dls-1)
   {
     beep();
      refresh();
     continue;
   }
   */
  /*Если режим замены то надо включит эту проверку*/
  if(poz == dls-1)
   {
    beep();
    refresh();
    continue;
   }

  if(kl == poz)
   {
    str[poz]=(char)K;
    poz++;
    str[poz]='\0';
   }
  else
   {
   /*Режим вставки символа*/
   /*
    for(i=kl; i>=poz; i--)
      str[i+1]=str[i];
     */
    str[poz]=(char)K;
    poz++;
   }

kkk:;

  /*Возвращаемся в начало строки*/
  wmove(okno,yn,xn);

  /*Выводим строку*/
  wprintw(okno,"%.*s",dls-1,str);
  if(mu>0)
   {
    wprintw(okno," ");
    mu=0;
   }
  kl=strlen(str);

  /*Возвращаемся на свою позицию в строке*/
  wmove(okno,yn,xn+poz);

 }
wmove(okno,yn,xn);
/*echo(); / *Включить эхопечать */

kl=strlen(str);
/*Убираем пробелы в конце строки*/
for(i=kl-1; str[i] == ' '|| str[i] == '\t';i--)
 {
  str[i]='\0';
 }

return(K);
}
