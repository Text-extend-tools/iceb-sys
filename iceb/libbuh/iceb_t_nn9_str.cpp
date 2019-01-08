/*$Id: iceb_t_nn6_str.c,v 5.6 2014/07/31 07:09:27 sasa Exp $*/
/*22.03.2017	22.03.2017	Белых А.И.	iceb_t_nn9_str.c
Вывод строки в налоговую накладную
*/
#include "buhl.h"


void iceb_t_nn9_str(int nomer_str,int metka_str,
const char *naim,
const char *ei,
double kolih,
double cena,
double suma,
const char *kodtov,
int metka_imp, /*метка импортного товара 0-не импортный 1-импортный*/
const char *ku, /*код послуги згідно з ДКПП*/
const char *kdstv, /*код діяльності сільклгосподарського товаровиробника*/
FILE *ff)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
class iceb_tu_str kodei("");

/*Узнаём код единицы измерения*/
sprintf(strsql,"select kei from Edizmer where kod='%s'",sql_escape_string(ei,0));
if(readkey(strsql,&row,&cur) == 1)
  kodei.new_plus(row[0]); 

char metka_importa[8];

if(metka_imp == 1)
 sprintf(metka_importa,"%s","  X");
else
 sprintf(metka_importa,"%s","");

if(metka_str == 1) /*услуги*/
 {
  fprintf(ff,"|%5d|%-*.*s|%-*.*s|%-6s|%-*.*s|%-*.*s|%-*.*s|%11.11g|%10s|%-6s|%-6s|%22.2f|%-*.*s|\n",
  nomer_str,
  iceb_tu_kolbait(59,naim),iceb_tu_kolbait(59,naim),naim,
  iceb_tu_kolbait(10,kodtov),iceb_tu_kolbait(10,kodtov),kodtov,
  metka_importa,
  iceb_tu_kolbait(7,ku),
  iceb_tu_kolbait(7,ku),
  ku,
  iceb_tu_kolbait(8,gettext("Услуга")),iceb_tu_kolbait(8,gettext("Услуга")),gettext("Услуга"),
  iceb_tu_kolbait(5,kodei.ravno()),
  iceb_tu_kolbait(5,kodei.ravno()),
  kodei.ravno(),
  kolih,
  prcn(cena),
  "",
  "",
  suma,
  iceb_tu_kolbait(12,kdstv),iceb_tu_kolbait(12,kdstv),kdstv);  
 }
else
  fprintf(ff,"|%5d|%-*.*s|%-*.*s|%-6s|%-*.*s|%-*.*s|%-*.*s|%11.11g|%10s|%-6s|%-6s|%22.2f|%-*.*s|\n",
  nomer_str,
  iceb_tu_kolbait(59,naim),iceb_tu_kolbait(59,naim),naim,
  iceb_tu_kolbait(10,kodtov),iceb_tu_kolbait(10,kodtov),kodtov,
  metka_importa,
  iceb_tu_kolbait(7,ku),
  iceb_tu_kolbait(7,ku),
  ku,
  iceb_tu_kolbait(8,ei),iceb_tu_kolbait(8,ei),ei,
  iceb_tu_kolbait(5,kodei.ravno()),
  iceb_tu_kolbait(5,kodei.ravno()),
  kodei.ravno(),
  kolih,
  prcn(cena),
  "",
  "",
  suma,
  iceb_tu_kolbait(12,kdstv),iceb_tu_kolbait(12,kdstv),kdstv);  

for(int nom=59; nom < iceb_tu_strlen(naim); nom+=59)
  fprintf(ff,"|%5s|%-*.*s|%10s|%6s|%7s|%8s|%5s|%11s|%10s|%-6s|%-6s|%22s|%12s|\n",
  " ",
  iceb_tu_kolbait(40,iceb_tu_adrsimv(nom,naim)),
  iceb_tu_kolbait(40,iceb_tu_adrsimv(nom,naim)),
  iceb_tu_adrsimv(nom,naim),
  "","","","","","","","","","","");

}
 