/*$Id: zarreadn.c,v 5.19 2013-04-07 10:22:45 sasa Exp $*/
/*16.02.2016	23.01.2009	Белых А.И.	zarreadn.c
Чтение настроек перед автоматическим расчётом начислений и удерданий*/
#include "buhg.h"
#include "read_nast_ind.h"

int read_nast_ind(short mr,short gr,class index_ua *data);
void zar_mpsl();

class index_ua tabl_ind; /*Таблица для индексации зарплаты*/
float pnds=20.;




void zarreadn(short dr,short mr,short gr,
int metka_r)  //1-начисления 2-удержания 3-начисления и удержания
            //4-соц-отчисления
            //5-проводки
{
/*Чтение процента налога на добавленную стоимость*/
pnds=iceb_t_pnds(dr,mr,gr);


/*Читаем таблицу разрядов-коэффициентов*/
zartarroz();
zartarroz1();

/*Чтение настроек для расчёта индексации зарплаты*/
read_nast_ind(mr,gr,&tabl_ind);

/*Чтение кодов единого социального взноса*/
zarrnesv();

if(metka_r == 2 || metka_r == 3)
  zar_mpsl(); /*Определяем применять социальную льготу при расчёте или нет*/


}
