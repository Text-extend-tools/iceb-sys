/*$Id: zarlgot.c,v 5.18 2013/05/17 14:56:17 sasa Exp $*/
/*28.11.2011	26.04.2004	Белых А.И.	zarlgot.c
Чтение процента льгот по подоходному налогу
*/
#include "buhg.h"

extern int metka_psl; /*Метка применения социальной льготы 0-применяется 1-нет*/

float zarlgot(int tabn,short mr,short gr,int *kollgot,class iceb_tu_str *kod_lgot)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
float procent=0.;

*kollgot=0;
kod_lgot->new_plus("");

if(metka_psl == 1)
 {
  sprintf(strsql,"%s-Расчёт без применения социальных льгот!!!\n",__FUNCTION__);
  zar_pr_ins(strsql);  
  return(0.);
 }


sprintf(strsql,"select pl,koll,kodl from Zarlgot where tn=%d and dndl <= '%04d-%02d-01' order by dndl desc limit 1",tabn,gr,mr);
if(readkey(strsql,&row,&cur) == 1)
 {
  *kollgot=atoi(row[1]);
  procent=atof(row[0]);
  kod_lgot->new_plus(row[2]);
  sprintf(strsql,"%s-Льгота процент %.2f количество льгот %d\n",__FUNCTION__,procent,*kollgot);
  zar_pr_ins(strsql);  
 }
else
 {
  
  sprintf(strsql,"%s-Льгота не обнаружена!\n",__FUNCTION__);
  zar_pr_ins(strsql);  

 }  
return(procent);
}

