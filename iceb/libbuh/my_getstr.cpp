/*$Id: my_getstr.c,v 5.7 2013/05/17 14:56:31 sasa Exp $*/
/*05.09.2011    15.06.1993      Белых А.И.      my_getstr.c
Подпрограмма ввода и редактирования символьной строки длинна которой
больше видимой части строки
*/
#include        "buhl.h"

extern int metka_utf8; /*0-однобайтная кодировка 1-многобайтная кодировка utf8*/

int my_getstr(char *stroka,  //Строка для редактирования
int dls,  //длинна видимой части строки
int mdls, //максимальная длинна строки
int kolbait, /*длинна строки в байтах*/
WINDOW *okno)  //Указатель на окно
{
int             pozvid; /*Позиция курсора в видимой части строки*/
int             poznstr; /*Позиция начала строки*/
int             pozstr; //Позиция курсора в строке
int             K;
int             i;
int             kl;
int             xn,yn; /*Начальные значения*/
short           mu; /*Метка удаления символа*/
int		N=0;
char simvol[8];

noecho();            /* Отключить эхопечать */
getyx(okno,yn,xn);
pozstr=poznstr=pozvid=0;
mu=0;
//mvprintw(LINES-2,0,"dls=%d mdls=%d",dls,mdls);
refresh();
for(;;)
 {
//  mvprintw(LINES-4,0,"poz=%d pozstr=%d poznstr=%d       ",poz,pozstr,poznstr);

  refresh(); //Для бипа на этом месте, чтобы не сбить положение курсора

  /*Возвращаемся в начало строки*/
  wmove(okno,yn,xn);

  /*Выводим строку*/
  wprintw(okno,"%.*s",iceb_tu_kolbait(dls,iceb_tu_adrsimv(poznstr,stroka)),iceb_tu_adrsimv(poznstr,stroka));
//  wprintw(okno,"%.*s",dls,&stroka[poznstr]);
  if(mu>0)
   {
    wprintw(okno," ");
    mu=0;
   }

  /*Возвращаемся на свою позицию в строке*/
  wmove(okno,yn,xn+pozvid);


  wrefresh(okno);
  
//  mvprintw(LINES-3,0,"mygetstr K=%d",K);
//  refresh();
  K=wgetch(okno);

  if(K == KEY_MOUSE)
   {
    K=mouseklic(&N,yn+1,xn-1,dls,1,okno);
    break;
   }

  if( K == HOME)
   {
    poznstr=pozvid=pozstr=0;
    continue;
   }

  if( K == END)
   {
    pozvid=pozstr=iceb_tu_strlen(stroka);
    if(pozstr > dls)
     {
      poznstr=pozstr-dls;
      pozvid=dls;
     }
    continue;
   }

  if( K == SLEFT)
   {

    pozstr--;

    if(pozstr < 0)
      beep();

    if(pozstr <= 0)
     {
      pozvid=poznstr=pozstr=0;
      continue;
     }

    if(pozvid > 0)
      pozvid--;
    else
     {
      pozvid=0;
      poznstr--;
     }
    
    continue;
   }

  if( K == SRITH)
   {
    //Не двигаться дальше введенных символов строки
    if(pozstr >= (int)iceb_tu_strlen(stroka))
     {
      beep();
      continue;

     }

    pozstr++;

    if(pozstr > mdls-1)
     {
      pozstr=mdls-1;
      beep();
      continue;
     }

    pozvid++;

    if(pozvid > dls)
     {
      pozvid=dls;
      poznstr++;
     }

    continue;
   }

  if( K == INS)
   {

    kl=iceb_tu_strlen(stroka);
    if(kl >= mdls)
     {
      beep();
      continue;
     }    

    if(pozstr <= kl)
     {
      iceb_tu_vsimv(stroka,mdls,pozstr," ");
      continue;
     }
    continue;
   }

  if( K == DELETE)
   {
    iceb_tu_udsimv(stroka,pozstr);
    mu++;
    continue;
   }

  if( K == BACK)
   {
    if(pozstr <= 0)
     {
      beep();
      continue;
     }
    pozstr--;
    iceb_tu_udsimv(stroka,pozstr);
/*********
    kl=iceb_tu_strlen(stroka);
    for(i=pozstr; i<kl;i++)
	stroka[i]=stroka[i+1];
**********/
    pozvid--;
    if(pozvid < 0)
     {
      pozvid=0;
      poznstr--;
     }
    mu++;
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
    pozvid=pozstr=poznstr=0;
    memset(stroka,'\0',mdls);
    wmove(okno,yn,xn);
    wprintw(okno,"%*s",dls," ");
    continue;
   }

  if(K == ESC)
    break;

  if(K == FK1 || K == FK2 || K == FK3 || K == FK4 || K == FK5
   || K == FK6 || K == FK7 || K == FK8 || K == FK9 || K == FK10)
    break;

  if(K == KEY_RESIZE) //Изменении размера окна
   {
//    getch();
    break;
   }

  kl=iceb_tu_strlen(stroka);


  /*Если режим замены то надо включить эту проверку*/
  if(pozstr == mdls-1)
   {
    beep();
    continue;
   }

  if(kl == pozstr)
   {
    if(metka_utf8 == 0) /*Однобайтная кодировка*/
     {
      stroka[pozstr]=(char)K;
      pozstr++;
      stroka[pozstr]='\0';
     }
    else      /*многобайтная кодировка utf8*/
     {
      memset(simvol,'\0',sizeof(simvol));
      simvol[0]=(char)K;   
      if(K == 208 || K == 209 || K == 210)
       {
        simvol[1]=(char)getch();
       }
      iceb_tu_zsimv(stroka,kolbait,pozstr,simvol);
      pozstr++;
     }
   }
  else
   {
    if(metka_utf8 == 0) /*Однобайтная кодировка*/
     {
      stroka[pozstr]=(char)K;
      pozstr++;
     }
    else  /*многобайтная кодировка utf8*/
     {
    
      memset(simvol,'\0',sizeof(simvol));
      simvol[0]=(char)K;
      if(K == 208 || K == 209 || K == 210)
       {
        simvol[1]=(char)getch();
       }

      iceb_tu_zsimv(stroka,kolbait,pozstr,simvol);
      pozstr++;
     }
   }

  if(pozstr <= dls)
   {
    pozvid=pozstr;
    poznstr=0;
   }
  else
   {
    pozvid=dls;
    poznstr=pozstr-dls;
   }
 }
wmove(okno,yn,xn);
/*echo(); / *Включить эхопечать */

kl=strlen(stroka); /*Только так*/
/*Убираем пробелы в конце строки*/
for(i=kl-1; stroka[i] == ' '|| stroka[i] == '\t';i--)
  stroka[i]='\0';

return(K);
}
