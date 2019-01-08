/* $Id: gurorr.c,v 5.66 2013/09/26 09:43:33 sasa Exp $ */
/*10.06.2016    22.09.1993      Белых А.И.      gurorr.c
Получение оборотной ведомости по счетам с развернутым сальдо
Необходимо получить обороты по организациям.
Заряжаем организации в массив организаций
Определяем перечень счетов по дебету и кредиту
Определяем начальное сальдо
Просматроиваем период и записываем данные в массив
Распечатываем данные организации, с нулевыми результатами
не распечатываем
*/
#include        "buhg.h"

int gurorrp(class guror_rr *rekrs,int metkasort,class spis_oth*);
//int gosrs(class guror_rr *rekrs,int metkasort,class spis_oth *oth);
int gosrsn(class guror_rr *rekrs,int metkasort,class spis_oth *oth);

int gurorr(class guror_rr *rekrs,class spis_oth *oth)
{
int             kom=0;
int             sub;
int metkasort=0;

clear(); /*Очистить экран и закрасить фоновым цветом*/
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите форму распечатки"));

MENU.VVOD_SPISOK_add_MD(gettext("Распечатка журнала-ордера"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка проводок по контрагентам"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2
kom=0;

while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
 
if(kom == 4 || kom == -1)
 return(1);


MENU.VVOD_delete();
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите режим"));

MENU.VVOD_SPISOK_add_MD(gettext("Расчитать по субсчетам"));
MENU.VVOD_SPISOK_add_MD(gettext("Расчитать по счетам"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

sub=0;
while(menu3w(stdscr,&MENU,&sub,-1,-1,0) != 0);
 
if(sub == 2 || sub == -1)
 return(1);



MENU.VVOD_delete();
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите режим"));

MENU.VVOD_SPISOK_add_MD(gettext("Сортировка по коду контрагента в символьном виде"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Сортировка по коду контрагента в цифровом виде"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Сортировка по наименованию контрагента"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Сортировка по группе контрагента"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//4
metkasort=0;

while(menu3w(stdscr,&MENU,&metkasort,-1,-1,0) != 0);
 
if(metkasort == 4 || metkasort == -1)
  return(1);



clear();
GDITE();

if(kom == 1)
  return(gurorrp(rekrs,metkasort,oth));
else
 {
  return(gosrsn(rekrs,metkasort,oth));
 }
}
