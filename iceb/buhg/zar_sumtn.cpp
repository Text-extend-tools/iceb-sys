/*$Id: zar_sumtn.c,v 5.4 2014/02/28 05:13:48 sasa Exp $*/
/*15.02.2016	03.09.2012	Белых А.И.	zar_sumtn.c
Определение нужных для расчётов сумм по табельному номеру
*/
#include "buhg.h"

extern short *knnf; /*Коды неденежных форм начисления зарплаты*/
extern short *knnf_nds; /*Коды начислений неденежными формами на которые начисляется НДС*/
extern short *kn_bsl; //Коды начислений на которые не распространяется льгота по подоходному налогу
extern short *knvr;/*Коды начислений не входящие в расчёт подоходного налога*/
extern short *kodbl;  /*Коды начисления больничного*/
extern class iceb_tu_str kods_esv_all;  /*Все коды удержания единого социального взноса*/
extern char *shetb; /*Бюджетные счета начислений*/
extern float pnds; 
extern double okrg;

void zar_sumtn(short mes,short god,int tabnom,
int metka_bn, /*0-смотрим все начисления 1-только бюджетные*/
class zar_sumtn *tnsum)
{
char strsql[2048];
int kolstr;
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select prn,knah,suma,godn,mesn,shet from Zarp where datz >= '%04d-%02d-%02d' and \
datz <= '%04d-%02d-%02d' and tabn=%d and suma <> 0.",
god,mes,1,god,mes,31,tabnom);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return;
 }

if(kolstr == 0)
 {
  return;
 }
/******************
sprintf(strsql,"%s-%d.%d %d\n",__FUNCTION__,mes,god,tabnom);
zar_pr_ins(strsql);

sprintf(strsql,"%s-Коды неденежных форм выплаты:",__FUNCTION__);
zar_pr_ins(strsql);
if(knnf != NULL)
 for(int nom=1; nom <= knnf[0]; nom++)
  {
   
   sprintf(strsql,"%d ",knnf[nom]);
   zar_pr_ins(strsql);
  }
zar_pr_ins("\n");
*****************************/
double suma=0.;

while(cur.read_cursor(&row) != 0)
 {
  if(metka_bn == 1)
   if(proverka(shetb,row[5],1,0) != 0)
    continue;

  suma=atof(row[2]);
  if(atoi(row[0]) == 1)
   {

    /****************************
    Если больничный не всчёт текущего месяца то в расчёте подоходного он участия не берёт 
    подоходный с него начисляется в boln.c
    *********************/
    if(provkod(kodbl,row[1]) >= 0)     
     {
      if(atoi(row[3]) != god || atoi(row[4]) != mes)
       continue;
     }    
    if(provkod(knvr,row[1]) >= 0)     
      continue;

    tnsum->suma_nah_all_podoh+=suma;
    if(provkod(knnf,row[1]) >= 0)
     {
      if(provkod(knnf_nds,row[1]) >= 0)
       {
        double suma_start=suma;
        double suma_nds=suma*pnds/100.;
        suma_nds=okrug(suma_nds,okrg);
        suma+=suma_nds;

        sprintf(strsql,"%s-Начисляем НДС %.2f+%.2f=%.2f\n",__FUNCTION__,suma_start,suma_nds,suma);
        zar_pr_ins(strsql);

       }                  

      if(provkod(kn_bsl,row[1]) >= 0)
       tnsum->suma_ndfv_blg+=suma;
      else
       tnsum->suma_ndfv_slg+=suma;
           
     }

    if(provkod(kn_bsl,row[1]) >= 0)
      tnsum->suma_blg+=suma;
    else
      tnsum->suma_slg+=suma;
   }
  else
   {
//    if(proverka(kods_esv_all.ravno(),row[1],0,1) == 0)
//     tnsum->suma_esv_all+=suma;
   }
       
 }

/*определяем сумму больничных начисленных в счёт расчётного месяца в будущих периодах*/
sprintf(strsql,"select knah,suma from Zarp where datz > '%04d-%02d-31' \
and godn=%d and mesn=%d and tabn=%d and suma <> 0.",god,mes,god,mes,tabnom);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  if(provkod(kodbl,row[0]) >= 0)     
   tnsum->suma_boln_bp+=atof(row[1]);
 }
if(metka_bn == 1)
sprintf(strsql,"%s-Смотрели только бюджетные начисления !!!\n",__FUNCTION__);
zar_pr_ins(strsql);
sprintf(strsql,"%s-Общая сумма начислений с которых берётся подоходный:%.2f\n",__FUNCTION__,tnsum->suma_nah_all_podoh);
zar_pr_ins(strsql);
sprintf(strsql,"%s-Cумма начисления на которую распространяется соц. льгота:%.2f\n",__FUNCTION__,tnsum->suma_slg);
zar_pr_ins(strsql);
sprintf(strsql,"%s-Cумма начисления на которую не распространяется соц. льгота:%.2f\n",__FUNCTION__,tnsum->suma_blg);
zar_pr_ins(strsql);
sprintf(strsql,"%s-Cумма неденежных форм начисления с соц. льготой:%.2f\n",__FUNCTION__,tnsum->suma_ndfv_slg);
zar_pr_ins(strsql);
sprintf(strsql,"%s-Cумма неденежных форм начисления без соц. льготы:%.2f\n",__FUNCTION__,tnsum->suma_ndfv_blg);
zar_pr_ins(strsql);
//sprintf(strsql,"%s-Cумма единого социального взноса общая:%.2f\n",__FUNCTION__,tnsum->suma_esv_all);
//zar_pr_ins(strsql);
sprintf(strsql,"%s-Cумма больничных будущих периодов:%.2f\n",__FUNCTION__,tnsum->suma_boln_bp);
zar_pr_ins(strsql);


}
