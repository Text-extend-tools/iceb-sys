/*$Id: zapf8dr.c,v 5.4 2013/08/13 05:50:01 sasa Exp $*/
/*17.04.2015	02.08.2011	Белых А.И.	zapf8dr.c
Запись в dbf файл  формы 1ДФ
**/
#include <stdio.h>

void  zapf8dr(long npp1,short kvrt,short gn,const char *koded,short TYP,const char *inn,
double suman,double sumav,double podohn,double podohv,const char *priz,const char *datn,const char *datk,
short kodlg,short vidz,FILE *ff4)
{

char kodlgot[64];
sprintf(kodlgot,"%2d",kodlg); /*код льготы должен быть 2 знака*/
fprintf(ff4, " %5.5ld%1d%4d%-10.10s%d%-10.10s%12.2f%12.2f%12.2f%12.2f%3.3s%8.8s%8.8s%2.2s%1.1d",
npp1,kvrt,gn,koded,TYP,inn,suman,sumav,podohn,podohv,priz,datn,datk,kodlgot,vidz);

}
