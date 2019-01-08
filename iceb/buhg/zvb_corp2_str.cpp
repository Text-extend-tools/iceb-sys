/*$Id: zvb_corp2_str.c,v 5.1 2014/08/31 06:18:19 sasa Exp $*/
/*20.08.2014	18.08.2014	Белых А.И.	zvb_corp2_str.c
Вывод строк в файлы
Ім’я поля 	Тип		Довжина	Найменування реквізиту
NSC		Numeric		15	рахунок отримувача
SUMMA		Numeric		20.2	сума платежу (в грн)
FIO		Character	38	найменування отримувача
ID_KOD		Numeric		14	ідентифікатор код отримувача
*/
#include "buhg.h"

void zvb_corp2_str(int nomerpp,const char *fio,
const char *nomersh,
double suma,
const char *inn,
const char *tabnom,
FILE *ff_dbf_tmp,FILE *ffras)
{


fprintf(ff_dbf_tmp," %-*.*s%20.2f%-*.*s%-*.*s",
iceb_tu_kolbait(15,nomersh),
iceb_tu_kolbait(15,nomersh),
nomersh,
suma,
iceb_tu_kolbait(38,fio),
iceb_tu_kolbait(38,fio),
fio,
iceb_tu_kolbait(14,inn),
iceb_tu_kolbait(14,inn),
inn);


fprintf(ffras,"\
%6d %-*s %-*s %*s %-*s %10.2f\n",
nomerpp,
iceb_tu_kolbait(6,tabnom),tabnom,
iceb_tu_kolbait(40,fio),fio,
iceb_tu_kolbait(14,nomersh),nomersh,
iceb_tu_kolbait(10,inn),inn,
suma);



}
