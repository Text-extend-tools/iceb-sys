/*$Id: dirmasiv.c,v 5.13 2013-04-07 10:22:54 sasa Exp $*/
/*16.02.2009    28.01.2000      Белых А.И.   dirmasiv.c
Подпрограмма просмотра на экране массива с информацией
Возвращаем номер выбранной строки в массиве
Если вернули  -1 вышли по F10 или ESC
*/

#include	"buhl.h"


int dirmasiv(class SPISOK *masivmenu,//Массив для просмотра
int y,int x, // Координаты вывода меню
int kk, //Номер строки -10 выход
const char *soob, //Строка сообщения
int nomras, //Номер раскраски меню
int *kod_kl) /*Возвращается код клавиши*/
{
int		prc;
int             nomstr; /*Номер строки в массиве*/
int             ns; /*Номер строки меню*/
int             kolstr; /*Количество строл в меню*/
int		ii;
int		K;
short		korks;
int		ks=0;
class iceb_t_mstr spis_menu(0);

korks=kolstr=0;
if(soob[0] != '\0')
 korks=2;
 

kolstr=masivmenu->kolih();



if(kolstr == 0)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  *kod_kl=soobshw(&SOOB,stdscr,-1,-1,1,1);
  return(-1);
 }

nomstr=0;

naz:;

/*Вычисляем позицию по вертикали*/
if(y < 0)
 {
  if(kolstr > LINES-3-korks)
   {
    y=0;
    ks=LINES-3-korks;
   }
  else
   {
    y=(LINES-3-kolstr-korks)/2;
    ks=kolstr;
   }
 }
else
 {
  ks=LINES-y-3-korks;
 }


spis_menu.spis_str.free_class();

if(kolstr > 0 && nomstr >= kolstr)
 nomstr=kolstr;
if(nomstr < 0)
 nomstr=0; 

//printw("nomstr=%d ks=%d\n",nomstr,ks);

ns=0;
char bros[512];
//printw("\nnomstr=%d\n",nomstr);
for(ii=nomstr ; ns < ks ; ii++)
 {
  if(ii >= kolstr)
   break;

  sprintf(bros,"%.*s",COLS-4,masivmenu->ravno(ii));
  ns++;
  spis_menu.spis_str.plus(bros);
 }
//OSTANOV();
if(kk >= ns)
  kk=ns-1;

if(kk != -10 && kk < 0)
 kk=0;
/*
if(ns < ks)
   sprintf(MM5.MD[ns],gettext("Конец списка"));
*/


prc=(nomstr+ns)*100/kolstr;
memset(bros,'\0',sizeof(bros));
kk=spis_menu.menu(kk,y,x,NULL,bros,prc,&K,soob,0);


if(kk == -7) //Изменение размеров экрана
 {
  goto naz;  
 }

if(kk == -5)  /*Экран вниз*/
 {
  if(nomstr+ks >= kolstr)
   {
    beep();
    goto naz;
   }
  nomstr+=ks;
  goto naz;
 }

if(kk == -3 )  /*Стрелка вниз*/
 {
  kk=ks-1;
  if(nomstr+ks >= kolstr)
   {
    beep();
    goto naz;    
   }
  nomstr++;
/*
  printw("\nkk= %d ns=%d ks=%d nomstr=%d kolstr=%d\n",kk,ns,ks,nomstr,kolstr);
  OSTANOV();
*/
  goto naz;
 }
if(kk == -2 ) /*Стрелка вверх*/
 {

  nomstr--;
  if(nomstr <0)
    nomstr=0;

  if(nomstr == 0)
   beep();
  goto naz;
 }

if(kk == -4) /*Экран в верх*/
 {
  nomstr-=ks;
  if(nomstr < 0)
   {
    beep();
    nomstr=0;
   }
  goto naz;
 } 
*kod_kl=K;

switch (K)
 {
  case FK10: 
  case ESC :
     return(-1);

  case ENTER:
    return(kk+nomstr);

  default :
   return(kk+nomstr);

/*********
   goto naz;
************/
 }

}
