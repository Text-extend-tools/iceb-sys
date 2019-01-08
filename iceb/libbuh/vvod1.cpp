/*$Id: vvod1.c,v 5.9 2013/05/17 14:56:32 sasa Exp $*/
/*27.10.2010    12.11.1998              Белых А.И.      vvod1.c
Две подпрограммы.Первая работает на стандартоном экране, вторая
открывает для себя окно и исчезает с сохранением изображение
которое было под окном
*/
#include        "buhl.h"

/*******************************************/

int vvod1(class iceb_tu_str *strk, //Строка для воода и редактирования
short dlstrk, //Максимальная длинна строки для ввода в символах
VVOD *VV,
int (*outvvod)(char*), //Функция чтения сканерного ввода
WINDOW *win1, //Указатель на окно на котором будет создано новое окно
int PY,int PX) //Координаты вывода окна
{
int kolbait=dlstrk*sizeof(wchar_t);
char stroka[kolbait];
strncpy(stroka,strk->ravno(),sizeof(stroka)-1);
int K;
if((K=vvod1(stroka,dlstrk,kolbait,VV,(*outvvod),win1,PY,PX)) == FK10)
 return(K);
if(K == ESC)
 return(K);
  
strk->new_plus(stroka);
return(K);


}
/*********************************************/

int vvod1(char *strk, //Строка для воода и редактирования
int dlstrk, //Максимальная длинна строки для ввода в символах
int kolbait, //Длинна строки в байтах
VVOD *VV,
int (*outvvod)(char*), //Функция чтения сканерного ввода
WINDOW *win1, //Указатель на окно на котором будет создано новое окно
int PYz,int PXz) //Координаты вывода окна
{
WINDOW		*win2;
int             K;  /*Принимает код клавиши*/
short		dls;
int		PYstd=0,PXstd=0;

/*Вытолкнем содержимое предыдущего экрана*/
wrefresh(win1);
VV->dlr=dlstrk;

VV->wvramka(PYz,PXz,&PYstd,&PXstd,1,1,dlstrk,win1,&win2);
int kolelem=VV->VVOD_SPISOK_return_kolstr();
int poz=VV->PY+kolelem+1;

wmove(win2,poz,VV->PX+1);
wattroff(win2,VV->VVOD_return_cs(iceb_CFM));

wattron(win2,VV->VVOD_return_cs(iceb_CFV)); 

wprintw(win2,"%*s",VV->MDLS," ");
wmove(win2,poz,VV->PX+1);
wprintw(win2,"%s",strk);
wmove(win2,poz,VV->PX+1);

dls=VV->MDLS;
if(dlstrk < VV->MDLS)
  dls=dlstrk;
wrefresh(win2);  

if ((*outvvod != (int)NULL) && ((*outvvod)(strk) > 0)) 
 {
  /* получен ввод с внешнего устройства */
  K=-1;
 }
else 
 {
 /* получен ввод с клавиатуры или ошибка внешнего устройства */
  K=my_getstr(strk,dls,dlstrk,kolbait,win2); /*Только так - чтенив не строку сласса VVOD*/

 }


wattroff(win2,VV->VVOD_return_cs(iceb_CFV)); 

wrefresh(win2);
delwin(win2);

touchwin(win1);
wrefresh(win1);
attron(VV->VVOD_return_cs(iceb_CB));

return(K);
}
