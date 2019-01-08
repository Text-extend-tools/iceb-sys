/*$Id: cennik.c,v 5.11 2013/11/05 10:50:48 sasa Exp $*/
/*13.07.2015	07.05.2003	Белых А.И.	cennik.c
Программа печати ценника на товар
*/
#include "buhg.h"


void cennik(short d,short m,short g, //Дата распечатки ценника
const char *kodtv, //Код товара
const char *naim,
double cena,
const char *shrihkod,
const char *organ,
FILE *ff)
{
char  stroka[1024];

fprintf(ff,"\
┌────────────────────────────────────────┐\n");

sprintf(stroka,"%s:%s",gettext("Код товара"),kodtv);
fprintf(ff,"│%-*.*s│\n",iceb_tu_kolbait(40,stroka),iceb_tu_kolbait(40,stroka),stroka);

fprintf(ff,"│%-*.*s│\n",iceb_tu_kolbait(40,naim),iceb_tu_kolbait(40,naim),naim);

sprintf(stroka,"%s:%.2f",gettext("Цена"),cena);

fprintf(ff,"│");
fprintf(ff,"\x0E"); //Двойная ширина

fprintf(ff,"\x1B\x45"); //Включить выделенный режим
fprintf(ff,"%-*.*s",iceb_tu_kolbait(20,stroka),iceb_tu_kolbait(20,stroka),stroka);
fprintf(ff,"\x1B\x46"); //Выключить выделенный режим
fprintf(ff,"\x14"); //отмена двойной ширины
fprintf(ff,"│\n");

sprintf(stroka,"%s:%s",gettext("Щтрих код"),shrihkod);
//fprintf(ff,"│%-40.40s│\n",stroka);
fprintf(ff,"│%-*.*s│\n",iceb_tu_kolbait(40,stroka),iceb_tu_kolbait(40,stroka),stroka);

sprintf(stroka,"%d.%d.%d %s",d,m,g,organ);
//fprintf(ff,"│%-40.40s│\n",stroka);
fprintf(ff,"│%-*.*s│\n",iceb_tu_kolbait(40,stroka),iceb_tu_kolbait(40,stroka),stroka);

fprintf(ff,"\
└────────────────────────────────────────┘\n");

}
