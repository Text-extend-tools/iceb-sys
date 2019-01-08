/*$Id: zarsbsl.c,v 5.10 2013/05/17 14:56:18 sasa Exp $*/
/*16.04.2017	13.08.2009	Белых А.И.	zarsbsl.c
Определение суммы начислений на которые не распространяется социальная льгота
*/
#include "buhg.h"

extern short *knnf; //Коды начислений недежными формами 
extern short *knnf_nds; /*Коды начислений неденежными формами на которые начисляется НДС*/
extern short *kn_bsl; //Коды начислений на которые не распространяется льгота по подоходному налогу
extern  short   *kodbl; /*Код больничного*/
extern char	*shetb; /*Бюджетные счета начислений*/
extern float pnds; 
extern double okrg;
extern float  ppn; //Процент подоходного налога


double zarsbsl1(int tabnom,short mp,short gp,
double *suma_bsl_b,/*сумма с увеличением для бюджета*/
double *suma_bsl_start, /*Сумма без социальной льготы без начисления в чистом виде общая*/
double *suma_bsl_start_b);  /*Сумма без социальной льготы без начисдения в чистом виде бюджет*/


double zarsbsl(int tabnom,short mp,short gp,
double *suma_bsl_b,/*сумма с увеличением для бюджета*/
double *suma_bsl_start, /*Сумма без социальной льготы без начисления в чистом виде общая*/
double *suma_bsl_start_b)  /*Сумма без социальной льготы без начисдения в чистом виде бюджет*/
{

return(zarsbsl1(tabnom,mp,gp,suma_bsl_b,suma_bsl_start,suma_bsl_start_b));

return(0.);

}
/*******************/
/*поиск в базе*/
/*******************/
double zarsbsl1(int tabnom,short mp,short gp,
double *suma_bsl_b,/*сумма с увеличением для бюджета*/
double *suma_bsl_start, /*Сумма без социальной льготы без начисления в чистом виде общая*/
double *suma_bsl_start_b)  /*Сумма без социальной льготы без начисдения в чистом виде бюджет*/
{
char strsql[1024];
float procent_pn=ppn;
if(sravmydat(1,1,2016,1,mp,gp) <= 0)
 procent_pn=ICEB_PROCENT_PODOH_NAL_2016;
 
sprintf(strsql,"%s-%d.%d Определяем сумму начислений на которые не распространяется социальная льгота\n",__FUNCTION__,mp,gp);
zar_pr_ins(strsql);

double	sum=0.;
*suma_bsl_b=0.;
int kolstr=0;
SQL_str row;

if(kn_bsl == NULL)
 {
  zar_pr_ins("Не введены коды начислений на которые распространяестя соц. льгота\n");
  return(0.);
 }
sprintf(strsql,"select knah,suma,mesn,shet from Zarp where \
datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' and tabn=%d and prn='1' and \
suma <> 0. order by prn,knah asc",gp,mp,gp,mp,tabnom);
//zar_pr_ins(strsql);
//zar_pr_ins("\n");

class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(0.);
 }

if(kolstr == 0)
  return(0.);
short mes=0;
int kodnah=0;
double suma_nah=0.;

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s-%s %s %s %s\n",__FUNCTION__,row[0],row[1],row[2],row[3]);
  zar_pr_ins(strsql);

  kodnah=atoi(row[0]);
  if(provkod(kn_bsl,kodnah) >= 0)
   {
     sprintf(strsql,"%s %s\n",row[0],row[1]);
     zar_pr_ins(strsql);

     sprintf(strsql,"%s-Подошло\n",__FUNCTION__);
     zar_pr_ins(strsql);
     
     mes=atoi(row[2]);
     suma_nah=atof(row[1]);
     //Если больничный за предыдущие периоды, то игнорируем так как 
     //он уже учтен в findnahm
     if(mes != mp && provkod(kodbl,kodnah) >= 0)
      {
       sprintf(strsql,"%s-Больничный за предыдущий период\n",__FUNCTION__);
       zar_pr_ins(strsql);
      }
     else
      {
       if(provkod(knnf,kodnah) >= 0)
        {
         double suma=suma_nah;
         *suma_bsl_start+=suma_nah;   
         sprintf(strsql,"%s-Код начисления %d сумма=%.2f\n",__FUNCTION__,kodnah,suma);
         zar_pr_ins(strsql);
         if(provkod(knnf_nds,kodnah) >= 0)
          {
           double suma_nds=suma*pnds/100.;
           suma_nds=okrug(suma_nds,okrg);
           suma+=suma_nds;
           sprintf(strsql,"%s-Начисляем НДС %.2f+%.2f=%.2f\n",__FUNCTION__,suma_nah,suma_nds,suma);
           zar_pr_ins(strsql);
          }                  

         rudnfv(&suma,suma,procent_pn); 

         sum+=suma;
         if(proverka(shetb,row[3],0,1) == 0)
          {
           *suma_bsl_start_b+=suma_nah;   
           *suma_bsl_b+=suma;
          }
        }        
       else
        {
         sum+=suma_nah;
         *suma_bsl_start+=suma_nah;   
         if(proverka(shetb,row[3],0,1) == 0)
          {
           *suma_bsl_b+=suma_nah;
           *suma_bsl_start_b+=suma_nah;   
          }
        }

      }
   }
 }
return(sum);
}
