/*$Id: zvb_alfabank_str.c,v 5.7 2014/08/31 06:18:19 sasa Exp $*/
/*29.06.2013	15.10.2009	Белых А.И.	zvb_alfabank_str.c
Вывод строк в файлы
*/
#include "buhg.h"

void zvb_alfabank_str(int nomerpp,const char *fio,
const char *nomersh,
double suma,
const char *inn,
const char *kedrpou,
FILE *ff_dbf_tmp,FILE *ffras)
{
class iceb_tu_str famil("");
class iceb_tu_str ima("");
class iceb_tu_str oth("");

polen(fio,&famil,1,' ');
polen(fio,&ima,2,' ');
polen(fio,&oth,3,' ');



fprintf(ff_dbf_tmp," %6d%-*.*s%-*.*s%-*.*s%-14.14s%10.2f%-14.14s%-14.14s",
nomerpp,
iceb_tu_kolbait(30,famil.ravno()),iceb_tu_kolbait(30,famil.ravno()),famil.ravno(),
iceb_tu_kolbait(30,ima.ravno()),iceb_tu_kolbait(30,ima.ravno()),ima.ravno(),
iceb_tu_kolbait(30,oth.ravno()),iceb_tu_kolbait(30,oth.ravno()),oth.ravno(),
nomersh,suma,inn,kedrpou);

fprintf(ffras,"\
%6d %-*s %*s %10.2f %*s %*s\n",
nomerpp,
iceb_tu_kolbait(40,fio),fio,
iceb_tu_kolbait(14,nomersh),nomersh,
suma,
iceb_tu_kolbait(10,inn),inn,
iceb_tu_kolbait(10,kedrpou),kedrpou);



}
