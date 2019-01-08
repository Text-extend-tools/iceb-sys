/*$Id: zvb_promekonom_str.c,v 5.3 2013/05/17 14:56:19 sasa Exp $*/
/*10.12.2009	10.12.2009	Белых А.И.	zvb_promekonom_str.c
Вывод строк в файлы
*/
#include "buhg.h"

void zvb_promekonom_str(int nomerpp,const char *fio,
const char *nomersh,
double suma,
const char *inn,
const char *tabnom,
FILE *ff_dbf_tmp,FILE *ffras)
{

fprintf(ff_dbf_tmp," %-*.*s%-*.*s%10.2f",
iceb_tu_kolbait(40,nomersh),iceb_tu_kolbait(40,nomersh),nomersh,
iceb_tu_kolbait(60,fio),iceb_tu_kolbait(60,fio),fio,
suma);

fprintf(ffras,"\
%6d %-*s %-*s %-*s %-*s %10.2f\n",
nomerpp,
iceb_tu_kolbait(6,tabnom),tabnom,
iceb_tu_kolbait(40,fio),fio,
iceb_tu_kolbait(10,inn),inn,
iceb_tu_kolbait(14,nomersh),nomersh,
suma);


}
