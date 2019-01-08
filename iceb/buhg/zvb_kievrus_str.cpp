/*$Id: zvb_kievrus_str.c,v 5.3 2013-02-13 11:34:18 sasa Exp $*/
/*18.01.2013	11.10.2011	Белых А.И.	zvb_kievrus_str.c
Вывод строк в файлы
*/
#include "buhg.h"

void zvb_kievrus_str(int nomerpp,const char *fio,
const char *nomersh,
double suma,
const char *inn,
const char *tabnom,
FILE *ff_dbf_tmp,FILE *ffras)
{

fprintf(ff_dbf_tmp," %-14.14s%-12.2f%-*.*s%-*.*s",
nomersh,
suma,
iceb_tu_kolbait(10,inn),
iceb_tu_kolbait(10,inn),
inn,
iceb_tu_kolbait(50,fio),
iceb_tu_kolbait(50,fio),
fio);

fprintf(ffras,"\
%6d %-*s %-*s %-*s %-*s %10.2f\n",
nomerpp,
iceb_tu_kolbait(6,tabnom),tabnom,
iceb_tu_kolbait(40,fio),fio,
iceb_tu_kolbait(10,inn),inn,
iceb_tu_kolbait(14,nomersh),nomersh,
suma);


}
