/*$Id: zvb_privatbank_str.c,v 5.1 2014/08/31 06:18:19 sasa Exp $*/
/*22.09.2016	17.10.2014	Белых А.И.	zvb_privatbank_str.c
Вывод строк в файлы
BRANCH     CHAR     3     карточный бранч             обязательно      (в банке должны вам его сообщить)
ZPКOD      CHAR     3     код зарплатного проэкта     обязательно      (в банке дожны вам его сообщить)
CARD_NO    CHAR     16    номер карты                 не обязательно
LSTBL      CHAR     10    Табельный номер сотрудника  обязательно
FAM        CHAR     50    фамилия                     не обязательно
NAME       CHAR     50    имя                         не обязательно
OT         CHAR     50    отчество                    не обязательно
RLSUM      NUM      15.2  сумма                       обязательно
INN        CHAR     20    инн сотрудника              не обязательно
RLKOD      CHAR     30    назначение зачисления       не обязательно
*/
#include "buhg.h"



void zvb_privatbank_str(int nomerpp,const char *fio,
const char *nomersh,
double suma,
const char *inn,
const char *tabnom,
const char *branch,
const char *zpkod,
const char *rlkod,
FILE *ff_dbf_tmp,FILE *ffras)
{
class iceb_tu_str fam("");
class iceb_tu_str ima("");
class iceb_tu_str otch("");
polen(fio,&fam,1,' ');
polen(fio,&ima,2,' ');
polen(fio,&otch,3,' ');

fprintf(ff_dbf_tmp," %-*.*s%-*.*s%-*.*s%-*.*s%-*.*s%-*.*s%-*.*s%15.2f%-*.*s%-*.*s",
iceb_tu_kolbait(3,branch),
iceb_tu_kolbait(3,branch),
branch,
iceb_tu_kolbait(3,zpkod),
iceb_tu_kolbait(3,zpkod),
zpkod,
iceb_tu_kolbait(16,nomersh),
iceb_tu_kolbait(16,nomersh),
nomersh,
iceb_tu_kolbait(10,tabnom),
iceb_tu_kolbait(10,tabnom),
tabnom,
iceb_tu_kolbait(50,fam.ravno()),
iceb_tu_kolbait(50,fam.ravno()),
fam.ravno(),
iceb_tu_kolbait(50,ima.ravno()),
iceb_tu_kolbait(50,ima.ravno()),
ima.ravno(),
iceb_tu_kolbait(50,otch.ravno()),
iceb_tu_kolbait(50,otch.ravno()),
otch.ravno(),
suma,
iceb_tu_kolbait(20,inn),
iceb_tu_kolbait(20,inn),
inn,
iceb_tu_kolbait(30,rlkod),
iceb_tu_kolbait(30,rlkod),
rlkod);


fprintf(ffras,"\
%6d %-*s %-*s %*s %-*s %10.2f\n",
nomerpp,
iceb_tu_kolbait(6,tabnom),tabnom,
iceb_tu_kolbait(40,fio),fio,
iceb_tu_kolbait(14,nomersh),nomersh,
iceb_tu_kolbait(10,inn),inn,
suma);



}
