/*$Id: iceb_t_spk.c,v 5.8 2013/05/17 14:56:29 sasa Exp $*/
/*13.03.2010	30.05.2007	Белых А.И.	iceb_t_spk.c
Вывод в файл справки по конечному сальдо по контрагенту
*/
#include "buhl.h"


void iceb_t_spk(short dk,short mk,short gk,
const char *shet,
const char *naimsh,
const char *kontr,
const char *naikontr,
double suma,
FILE *ff_spr)
{
fprintf(ff_spr,"%s\n\n",iceb_t_get_pnk("00",0));

fprintf(ff_spr,"%20s %s\n\n\
%s %s %s\n\
%s %s %s\n\
%s %02d.%02d.%d %s %s %.2f %s\n",
"",
gettext("Справка"),
gettext("На счёте"),
shet,
naimsh,
gettext("по контрагенту"),
kontr,naikontr,
gettext("по состоянию на"),
dk,mk,gk,
gettext("г."),
gettext("числится сумма в размере"),
suma,
gettext("грн."));

class iceb_tu_str suma_prop("");
preobr(suma,&suma_prop,0);

fprintf(ff_spr,"%s: %s\n",gettext("Сумма прописью"),suma_prop.ravno());

fprintf(ff_spr,"\n\
%-*s____________________\n\n\
%-*s____________________\n",
iceb_tu_kolbait(20,gettext("Руководитель")),
gettext("Руководитель"),
iceb_tu_kolbait(20,gettext("Главный бухгалтер")),
gettext("Главный бухгалтер"));

podpis(ff_spr);
}
