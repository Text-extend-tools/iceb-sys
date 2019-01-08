/*$Id: rudnfv.c,v 5.10 2013/05/17 14:56:11 sasa Exp $*/
/*11.01.2016	01.08.2004	Белых А.И.	rudnfv.c
Расчёт увеличения дохода при неденежных формах начисления зарплаты
*/
#include        "buhg.h"

extern double   okrg;

void rudnfv(double *suma_zar,double suma_ndfnz,double proc_podoh)
{
if(suma_ndfnz == 0.)
 return;

float suma_ud=0.;
 
double suma_snn=0; //Сумма с которой должен начисляться налог
suma_snn=suma_ndfnz*(100/(100-proc_podoh));
suma_snn=okrug(suma_snn,okrg);
//вычисляем сумму увеличения дохода
suma_ud=suma_snn-suma_ndfnz;
char strsql[1024];
sprintf(strsql,"%s-Cумма неденежных форм начисления зарплаты:%.2f!\n",__FUNCTION__,suma_ndfnz);
zar_pr_ins(strsql);

sprintf(strsql,"%s-Сумма с которой нужно начислять налог:%.2f*(100/(100-%f))=%.2f\n",__FUNCTION__,suma_ndfnz,proc_podoh, suma_snn);
zar_pr_ins(strsql);

sprintf(strsql,"%s-Увеличение дохода на %.2f-%.2f=%.2f\n",__FUNCTION__,suma_snn, suma_ndfnz, suma_ud);    
zar_pr_ins(strsql);

sprintf(strsql,"%s-Увеличиваем сумму облагаемого налогом дохода:%.2f+%.2f=%.2f\n",__FUNCTION__,*suma_zar,suma_ud,*suma_zar+suma_ud);
zar_pr_ins(strsql);

*suma_zar+=suma_ud;

}
