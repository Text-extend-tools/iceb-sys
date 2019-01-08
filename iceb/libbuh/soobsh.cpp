/*$Id: soobsh.c,v 5.4 2013/05/17 14:56:32 sasa Exp $*/
/*20.11.2009	03.03.1999	Белих А.И.	soobsh.c
Выдача сообщения на экран
*/
#include        "buhl.h"

/***************/
/*Новая функция*/
/***************/

int soobsh(VVOD *VV,
int PY,int PX,
int TEN) //0-без тени 1-с тенью
{
int             K=0; 

VV->vramka(PY,PX,TEN);


standend(); //Установка нормального режима отображения
refresh();
return(K);

}
