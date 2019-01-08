/*$Id: zarstved.c,v 5.4 2013/05/17 14:56:18 sasa Exp $*/
/*10.01.2018	22.06.2009	Белых А.И.	zarstved.c
Вывод строки ведомости на выдачу зарплаты
*/
#include "buhg.h"

void zarstved(int *nomer_vedom,const char *tabb,const char *fio,double suma,FILE *ff)
{
*nomer_vedom+=1;
fprintf(ff,"%-3d %-*s %-*.*s",*nomer_vedom,
iceb_tu_kolbait(9,tabb),
tabb,
iceb_tu_kolbait(35,fio),iceb_tu_kolbait(35,fio),fio);
fprintf(ff,"%10s\n",prnbr(suma));
fprintf(ff,"\
..............................................................................\n");
}
/************************************/
/************************************/

void zarstved(int *nomer_vedom,int tabb,const char *fio,double suma,FILE *ff)
{
*nomer_vedom+=1;
fprintf(ff,"%-3d %-9d %-*.*s",*nomer_vedom,
tabb,iceb_tu_kolbait(35,fio),iceb_tu_kolbait(35,fio),fio);
fprintf(ff,"%10s\n",prnbr(suma));
fprintf(ff,"\
..............................................................................\n");
}
