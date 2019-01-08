/*$Id: okladtr.c,v 5.6 2013/05/17 14:56:06 sasa Exp $*/
/*12.04.2011	28.11.2008	Белых А.И.	okladtr.c
Расчёт оклада по тарифу
*/

#include "buhg.h"

double okladtr(short mr,short gr,int roz)
{
char strsql[1024];
double oklad=0.;

class zar_read_tn1h nastr;
zar_read_tn1(1,mr,gr,&nastr);

sprintf(strsql,"%s-Расчёт оклада по тарифному разряду:\n",__FUNCTION__);
zar_pr_ins(strsql);

sprintf(strsql,"Разряд:%d\n",roz);
zar_pr_ins(strsql);
    
float kof=zartarroz(roz);

sprintf(strsql,"Коэффициент:%f\n",kof);
zar_pr_ins(strsql);
  
double okk=0.;
if(kof > 0.)
 {
  okk=nastr.minzar*kof;
  oklad=okrug(okk,1.);              
 }
else
 {
  sprintf(strsql,"Не нашли коэффициент!\n");
  zar_pr_ins(strsql);
  oklad=0.;
  kof=0.;
 }   

sprintf(strsql,"Оклад:%.2f*%f=%f округляем=%.2f\n",nastr.minzar,kof,okk,oklad);
zar_pr_ins(strsql);

return(oklad);

}
