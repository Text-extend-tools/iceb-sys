/* $Id: rasshetz.c,v 5.34 2014/08/31 06:18:18 sasa Exp $ */
/*30.11.2016	15.09.2000	Белых А.И.	rasshetz.c
Расчёт начислений и удержаний заработной платы
*/
#include        <errno.h>
#include        "buhg.h"

double dolhnn(int tn,short mp,short gp,int kp,const char *nah_only);
void     zvanie(int,short,short,int,const char *nah_only);
void	zarindex(int,short,short,int,const char *nah_only);
void	zarindexv(int,short,short,int,const char *nah_only);
void     zarkmd(int tn,short mp,short gp,int podr,const char *nah_only);
double   podoh1(int,short,short,int,const char *uder_only);
void     procent(int,short,short,int,const char *nah_only);
double   boln(long,short,short,int,const char *uder_only,FILE*);
double boln1(int tn,short mp,short gp,int podr,const char *uder_only);
void     profs(int,short,short,int,const char *uder_only);
void     kredit(int,short,short,int,const char *uder_only);
void     alim(int,short,short,int,const char *uder_only);
void     zaruoz(int,short,short,int,const char *uder_only);
void     zarvpn(int,short,short,int,const char *uder_only);
void zardzn(int tn,short mp,short gp,int podr,const char *nah_only);
void dopdomin(int tabn,short mp,short gp,int podr,double koefpv,const char *nah_only);
void zarvn(int tabnom,short mes,short god,int podr);

extern short	kodindex;  /*Код индексации*/
extern int zar_pr_metka_r; /*0-потоковый расчёт по всем табельным номерам 1-расчёт по конкретному табельному номеру*/

void rasshetz(int tabn,short mp,short gp,int kp,
int prn,  //1-только начисления 2-только удержания 3-все
const char *nah_only,
const char *uder_only)
{
char strsql[1024];
double		koefpv;



 

if(prn == 1 || prn == 3)
 {
  zar_pr_start(1,tabn,mp,gp);

  sprintf(strsql,"%s-Код начисления:%s\nКод удержания:%s\n",__FUNCTION__,nah_only,uder_only);
  zar_pr_ins(strsql);

  koefpv=dolhnn(tabn,mp,gp,kp,nah_only); /*Должносной оклад*/

  zvanie(tabn,mp,gp,kp,nah_only); /*За звание*/

  procent(tabn,mp,gp,kp,nah_only); /*Процент*/

  zarindex(tabn,mp,gp,kp,nah_only); /*Расчёт индексации начислений*/
  zarindexv(tabn,mp,gp,kp,nah_only); /*Расчёт индексации на не вовремя выплаченную зарплату*/

  dopdomin(tabn,mp,gp,kp,koefpv,nah_only); /*Расчёт начисления до минимальной зарплаты*/
  zarkmd(tabn,mp,gp,kp,nah_only); //Расчёт командировочных
  zardzn(tabn,mp,gp,kp,nah_only); /*Расчёт доплаты до средней зарплаты в командировке с учётом надбавки за награду*/
  zar_pr_end();    
 }


if(prn == 2 || prn == 3)
 {
  zar_pr_start(2,tabn,mp,gp);

  sprintf(strsql,"%s-Код начисления:%s\nКод удержания:%s\n",__FUNCTION__,nah_only,uder_only);
  zar_pr_ins(strsql);


  podoh1(tabn,mp,gp,kp,uder_only); /*Расчёт подоходного налога*/
    
  zarvpn(tabn,mp,gp,kp,uder_only); /*Возврат подоходного налога*/
  profs(tabn,mp,gp,kp,uder_only); /*Профсоюзный взнос*/

  kredit(tabn,mp,gp,kp,uder_only);  /*кредит */

  alim(tabn,mp,gp,kp,uder_only);    /*алименты*/
 
  zaruoz(tabn,mp,gp,kp,uder_only);    /*расчёт удержания дневного заработка*/

  zarvn(tabn,mp,gp,kp); /*расчёт военного сбора*/
   
  /* перерасчёт больничного делаем здесь так как будут перечитаны настройки по месяцам расчёта*/
  if(sravmydat(1,mp,gp,ICEB_DNDESV,ICEB_MNDESV,ICEB_GNDESV) >= 0)
      boln1(tabn,mp,gp,kp,uder_only); /*Перерасчёт подоходного из за больничных*/

  zar_pr_end();
 }
}
