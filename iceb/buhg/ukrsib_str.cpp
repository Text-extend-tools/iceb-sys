/*$Id: ukrsib_str.c,v 5.6 2013/05/17 14:56:13 sasa Exp $*/
/*27.10.2010	29.01.2006	Белых А.И.	ukrsib_str.c
Вывод строки данных для зачисления средств на картстчета Укрсиббанка
*/
#include <stdio.h>
#include <stdlib.h>

void ukrsib_str(short dt,short mt,short gt,double suma,
const char *kartshet,
const char *fio,
int *nomer,
const char *tabnom,
const char *ope_tor,
FILE *ffr,FILE *fftmp)
{
*nomer+=1;
/********
fprintf(fftmp," %d%02d%02d%-7d20%-14s%15.2fUAH%-10s%8s%8s",
gt,mt,dt,
*nomer,
kartshet,
suma,
" "," "," ");      
***********/
fprintf(fftmp," %d%02d%02d%-7d20%-14s%15.2fUAH%-10.10s",
gt,mt,dt,
*nomer,
kartshet,
suma,
ope_tor);      

fprintf(ffr,"%4d %-9s %-14s %10.2f %s\n",
*nomer,tabnom,kartshet,suma,fio);
}
