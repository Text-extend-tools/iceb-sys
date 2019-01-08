/*$Id: knnfnds.c,v 5.7 2013-01-15 10:17:56 sasa Exp $*/
/*12.01.2013	12.08.2009	Белых А.И.	knnfnds.c
Начисление НДС на неденежные формы оплаты
Возвращает либо увеличенную сумму либо туже самую если НДС на этот код не начисляется
*/
#include "buhg.h"
extern short *kn_bsl; //Коды начислений на которые не распространяется льгота по подоходному налогу
extern short *knnf; /*Коды начислений неденежными формами*/
extern short *knnf_nds; /*Коды начислений неденежными формами на которые начисляется НДС*/
extern float pnds; 
extern double okrg;
extern char *shetb; /*Бюджетные счета*/

double knnfnds(int metka,/*0-для расчёта подоходного налога 1-все остальные случаи*/
int knu,const char *shet,double suma,double *sumab,int metka_repl) /* 0- без реплик 1-с репликами*/
{
/***************
Коды на которые не распространяется социальная льгота, могут также быть
в списке кодов не денежных форм начисления зарплаты. Тоесть на них может быть 
начислен НДС и применён коэффициент увеличения. При расчёте подоходного налога это
делается в другом месте
***********/
char strsql[1024];
if(metka_repl == 1)
 {
  sprintf(strsql,"%s-Начисление НДС на неденежные формы выплаты\n",__FUNCTION__);
  zar_pr_ins(strsql);

  sprintf(strsql,"%s-metka=%d knu=%d shet=%s\n",__FUNCTION__,metka,knu,shet);
  zar_pr_ins(strsql);
 }
 
if(metka == 0)
 if(provkod(kn_bsl,knu) >= 0) /*Для них отдельно начисляется если это нужно*/
  {
   if(metka_repl == 1)
    {
     sprintf(strsql,"%s-Код начисления без социальных льгот- не подходит\n",__FUNCTION__);
     zar_pr_ins(strsql);
    }
   return(0.);
  }

if(provkod(knnf,knu) < 0)
 {
  if(metka_repl == 1)
   {
    sprintf(strsql,"%s-Это денежная выплата-не подходит\n",__FUNCTION__);
    zar_pr_ins(strsql);
   }
  return(0.);
 }

if(metka_repl == 1)
 {
  sprintf(strsql,"%s-Не денежная форма оплаты %d %.2f\n",__FUNCTION__,knu,suma);
  zar_pr_ins(strsql);
 }

double suma_s_nds=suma;
if(provkod(knnf_nds,knu) >= 0)
 {
  double suma_nds=suma*pnds/100.;
  suma_nds=okrug(suma_nds,okrg);

  suma_s_nds+=suma_nds;
  sprintf(strsql,"%s-На начисление начисляется НДС = %f+%f*%f/100.=%f\n",__FUNCTION__,suma,suma,pnds,suma_s_nds);
  zar_pr_ins(strsql);

  if(proverka(shetb,shet,0,1) == 0)
   {
    sprintf(strsql,"%s-Не денежная форма оплаты (бюджет) %d %.2f\n",__FUNCTION__,knu,suma);
    zar_pr_ins(strsql);
    *sumab+=suma+suma_nds;  
   }

 }
 
return(suma_s_nds);

}

