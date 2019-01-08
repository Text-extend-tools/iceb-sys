/*$Id: iceb_t_nalnak_str.c,v 5.5 2013/05/17 14:56:28 sasa Exp $*/
/*13.01.2011	13.01.2011	Белых А.И.	iceb_t_nalnak_str.c
Вывод строки в налоговую накладную
*/
#include "buhl.h"


void iceb_t_nalnak_str(int metka_str,
int tipnn,
const char *razdel,const char *datop,
const char *naim,
const char *ei,
double kolih,
double cena,
double suma,
FILE *ff)
{

if(metka_str == 1)
  fprintf(ff,"|%-4s|%-10s|%-*.*s|%-*s|%-*.*s|%10s",
  razdel,datop,
  iceb_tu_kolbait(40,naim),iceb_tu_kolbait(40,naim),naim,
  iceb_tu_kolbait(7,gettext("грн.")),gettext("грн."),
  iceb_tu_kolbait(9,gettext("Услуга")),iceb_tu_kolbait(9,gettext("Услуга")),gettext("Услуга"),
  prcn(cena));
else
  fprintf(ff,"|%-4s|%-10s|%-*.*s|%-*.*s|%9.9g|%10s",
  razdel,datop,
  iceb_tu_kolbait(40,naim),iceb_tu_kolbait(40,naim),naim,
  iceb_tu_kolbait(7,ei),iceb_tu_kolbait(7,ei),ei,
  kolih,prcn(cena));

if(tipnn == 0)
  fprintf(ff,"|%10s|%9s|%9s|%9s|%11s|\n",
   prcn(suma)," "," "," "," ");

if(tipnn == 1)
  fprintf(ff,"|%10s|%9s|%9s|%9s|%11s|\n",
  " ",prcn(suma)," "," "," ");

if(tipnn == 2)
  fprintf(ff,"|%10s|%9s|%9s|%9s|%11s|\n",
  " "," ",prcn(suma)," "," ");

if(tipnn == 3)
  fprintf(ff,"|%10s|%9s|%9s|%9s|%11s|\n",
  " "," "," ",prcn(suma)," ");

for(int nom=40; nom < iceb_tu_strlen(naim); nom+=40)
 fprintf(ff,"|%4s|%-10s|%-*.*s|%-7.7s|%9.9s|%10s|%10s|%9s|%9s|%9s|%11s|\n",
  " "," ",
  iceb_tu_kolbait(40,iceb_tu_adrsimv(nom,naim)),
  iceb_tu_kolbait(40,iceb_tu_adrsimv(nom,naim)),
  iceb_tu_adrsimv(nom,naim),
  " "," "," "," "," "," "," "," ");

}
 