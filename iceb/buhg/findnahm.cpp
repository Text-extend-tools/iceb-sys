/* $Id: findnahm.c,v 5.35 2013/09/26 09:43:32 sasa Exp $ */
/*04.05.2013    10.12.1993      Белых А.И.      findnahm.c
Поиск всех начислений на нужного человека за указанный
месяц и год
*/
#include        <math.h>
#include        "buhg.h"
//#include        "zar11.h"


int findnagm_prov(short mt,short mr,int prnu,short mes,int knu,double suma,char *sheta,
short *kodi,double *suma_nfnz,double *suma_nfnz_b,double *suma_zarp_o,double *sumabu,
double *sumanb,double *suma_zarp_b,double *nah,double*);

extern short    *knvr;/*Коды начислений не входящие в расчёт подоходного налога*/
extern short    *kuvr;/*Коды удержаний входящие в расчёт подоходного нолога*/
extern char	*shetb; /*Бюджетные счета*/
//extern struct ZAR zar;
extern  short   *kodbl; /*Код больничного*/
extern short    *kuvvr_pens_sr; //Коды удержаний входящие в расчёт при отчислении с работника
extern short    *kn_bsl; //Коды начислений на которые не распространяется льгота по подоходному налогу

/**************/
/*Чтение в базе*/
/***************/

double findnahm(short mr,short gr, //Дата расчёта
long tabn,
short mt, //1- подоходный налог
          //2 - пенсионные отчисления
          //3 - профсоюз
          //4 - отчисления в фонд безработицы
          //5 - соц-страх
short *kodi, //Дополнительные коды не входящие в расчёт*/
FILE *ff_prot,
double *sumanb, //Сумма не бюджетная
double *sumabu, //Сумма бюджетная
double *suma_nfnz, //Общая сумма начисленная не денежными формами оплаты
double *suma_nfnz_b, //Бюджетная сумма начисленная не денежными формами оплаты
double *suma_zarp_o, //Сумма зарплатных начислений (на которые распространяется социальная льгота) общая
double *suma_zarp_b, //Сумма зарплатных начислений (на которые распространяется социальная льгота) бюджета
double *suma_boln_rm) /*Сумма больничных за расчётный месяц*/
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;
int kolstr;
double nah;

*suma_zarp_o=*suma_zarp_b=0.;
*sumanb=*sumabu=nah=0.;
*suma_nfnz=*suma_nfnz_b=0.;
*suma_boln_rm=0.;

sprintf(strsql,"select prn,knah,suma,shet,mesn from Zarp where datz >= '%04d-%02d-%02d' \
and datz <= '%04d-%02d-%02d' and tabn=%ld",gr,mr,1,gr,mr,31,tabn);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(0);
 }
while(cur.read_cursor(&row) != 0)
 {
  findnagm_prov(mt,mr,atoi(row[0]),atoi(row[4]),atoi(row[1]),atof(row[2]),row[3],
  kodi,suma_nfnz,suma_nfnz_b,suma_zarp_o,
  sumabu,sumanb,suma_zarp_b,&nah,suma_boln_rm);

 }

return(nah);

}
/*******************************************/
/*Проверка общая для двух подпрограмм*/
/*****************************************/
//Если вернули 0-всё впорядке 1-прервать цикл
int findnagm_prov(short mt,short mr,int prnu,short mes,int knu,double suma,char *sheta,
short *kodi,
double *suma_nfnz,
double *suma_nfnz_b,
double *suma_zarp_o,
double *sumabu,
double *sumanb,
double *suma_zarp_b,
double *nah,
double *suma_boln_rm) /*Сумма больничных за расчётный месяц*/
{
char strsql[512];
if(prnu == 0)
 return(1);
 
//  printw("%.2f\n",suma);

if(prnu == 1)
 {
  /*Узнаём сумму больничных за расчётный месяц*/
   if(mes == mr)
     if(provkod(kodbl,knu) >= 0)
      {
       sprintf(strsql,"%s-Больничный %d %.2f\n",__FUNCTION__,knu,suma);
       zar_pr_ins(strsql);
       *suma_boln_rm+=suma;
      }


  /*Если больничный не за текущий месяц то в общюю сумму не включ.*/
  /*Для расчёта профсоюзного взносов включается*/
  /*Для этих удержаний делается перерасчёт по этим больничным отдельно*/
  
  if(mt == 1 || mt == 2 || mt == 5)
   if(mes != mr)
     if( provkod(kodbl,knu) >= 0)
      {
       sprintf(strsql,"%s-Не вошло в расчёт %d %.2f\n",__FUNCTION__,knu,suma);
       zar_pr_ins(strsql);
       return(0);
      }
  /*Коды начислений не входящие в расчёт подоходного налога*/
  if(mt == 1)
  if(provkod(knvr,knu) >= 0)
   {
    sprintf(strsql,"%s-Не вошло в расчёт %d %.2f\n",__FUNCTION__,knu,suma);
    zar_pr_ins(strsql);
    return(0);
   }
   
  if(provkod(kodi,knu) >= 0)
   {
    sprintf(strsql,"%s-Не вошло в расчёт %d %.2f\n",__FUNCTION__,knu,suma);
    zar_pr_ins(strsql);
    return(0);
   }

  *suma_nfnz+=knnfnds(1,knu,sheta,suma,suma_nfnz_b,0);

  if(provkod(kn_bsl,knu) < 0)
   {
    *suma_zarp_o+=suma;
   }

  if(shetb != NULL ) 
   {
    /*Узнаем какая часть суммы бюджетных счетов и всех остальных*/
    if(proverka(shetb,sheta,0,0) == 0)
     {
      *sumabu+=suma;
      if(provkod(kn_bsl,knu) < 0)
        *suma_zarp_b+=suma;
     }
    else
        *sumanb+=suma;
    
/*************    
    int i1=0;
    if(pole1(shetb,sheta,',',0,&i1) == 0)
        *sumabu+=suma;
    else
        *sumanb+=suma;
**************/


   }

  *nah+=suma;
 
 }

if(prnu == 2)
 {
  /*Коды удержаний входящие в расчёт подоходного налога*/

  if(mt == 1)  /*Для пенсии и профсоюза входят*/
  if(provkod(kuvr,knu) >= 0)
   {
    *nah+=suma; //Удержание отрицательное поэтому для того, чтобы уменьшить сумму мы его просто прибавляем
    sprintf(strsql,"%s-Вошло в расчёт удержание %d %.2f\n",__FUNCTION__,knu,suma);
    zar_pr_ins(strsql);
    return(0);
   }

  if(mt == 2)  //Коды удержаний входящие в расчёт для пенсионного отчисления с работника
  if(provkod(kuvvr_pens_sr,knu) >= 0)
   {
    *nah+=suma; //Удержание отрицательное поэтому для того, чтобы уменьшить сумму мы его просто прибавляем
    sprintf(strsql,"%s-Вошло в расчёт удержание %d %.2f\n",__FUNCTION__,knu,suma);
    zar_pr_ins(strsql);
    return(0);
   }

 }
return(0);
}

