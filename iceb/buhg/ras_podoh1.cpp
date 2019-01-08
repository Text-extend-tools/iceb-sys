/*$Id: ras_podoh1.c,v 5.18 2014/02/28 05:13:47 sasa Exp $*/
/*02.09.2016	15.01.2011	Белых А.И.	ras_podoh1.c
Расчёт подоходного налога
Возвращает сумму подоходного налога
*/
#include <math.h>
#include        "buhg.h"

extern char	*shetb; /*Бюджетные счета*/
extern double   okrg; /*Округление*/
extern short    *kodmp;   /*Коды благотворит. помощи*/
extern short    *kn_bsl; //Коды начислений на которые не распространяется льгота по подоходному налогу
extern short    kodpn;   /*Код подоходного налога*/
extern short	metkarnb; //0-Расчёт налогов для бюджета пропорциональный 1-последовательный
extern short kodpn_sbol; /*Код подоходного с больничного*/
extern short   *kodbl; /*Код больничного*/

//double find_podoh(short mr,short gr,int tabn,double *suma_ndfv,double *sumanb,double *sumabu);
double prosmas(short*,short,short,double*);
double zarsbsl(int tabnom,short mp,short gp,double *suma_bsl_b,double *suma_bsl_start,double *suma_bsl_start_b);
double ras_podor1_rpnsb(int tbn,short mr,short gr,int podr,int metka_zap,double suma_nahisl,double lgota);

double ras_podoh1(int tn, //Табельный номер*/
short mp,short gp,
int podr,
int metka_zap, //0-делать запись 1-нет
double suma_boln,
const char *uder_only)
{
char strsql[2048];
SQL_str row;
class iceb_tu_str shet("");
class iceb_tu_str shetb("");
double nah=0.;
class ZARP     zp;
short		d;
class SQLCURSOR cur;
int metka_p_boln=0;
double bb=0.;

double suma_s_soc_lg=0.;
class zar_sumtn tnsum; 

if(suma_boln != 0.)
 metka_p_boln=1;
dpm(&d,&mp,&gp,5);

zp.dz=d;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;

sprintf(strsql,"\n%s-Рачёт подоходного налога после 1.1.2011\n\
---------------------------------------------------\n",__FUNCTION__);
zar_pr_ins(strsql);

if(proverka(uder_only,kodpn,0,0) != 0)
 {
  sprintf(strsql,"Код %d исключён из расчёта\n",kodpn);
  zar_pr_ins(strsql);
  return(0.);    
 }


double sumanb=0.;
double sumabu=0.;

zar_sumtn(mp,gp,tn,0,&tnsum); /*читаем всё что нужно для расчёта*/

nah=podohrs2016(tn,mp,gp,0,metka_p_boln,&tnsum);
double nah_zap=nah;

/*Проверяем есть ли код удержания подоходного налога в списке*/

sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='2' and knah=%d",tn,kodpn);
if(sql_readkey(&bd,strsql,&row,&cur) <= 0)
 {
  sprintf(strsql,"%s-Не введено удержание подоходного налога !\n",__FUNCTION__);
  zar_pr_ins(strsql);
  return(0.);
 }
shet.new_plus(row[0]);

sprintf(strsql,"select shet,shetb from Uder where kod=%d",kodpn);
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s-Не найден код удержания %d в справочнике удержаний!\n",__FUNCTION__,kodpn);
  zar_pr_ins(strsql);
  return(0.);
 }

shetb.new_plus(row[1]);
if(shetb.getdlinna() > 1)
  shet.new_plus(row[0]);

if(kodpn_sbol != 0 && suma_boln == 0.) /*нужно определить сколько подоходного налога приходится на больничные и записать их отдельно*/
 {
  nah_zap-=ras_podor1_rpnsb(tn,mp,gp,podr,metka_zap,suma_s_soc_lg,tnsum.suma_lgot);
 }


class iceb_tu_str koment("");
if(shetb.getdlinna() <= 1)
 {
  strcpy(zp.shet,shet.ravno());
  if(metka_zap == 0)
   zapzarp(d,mp,gp,tn,2,kodpn,nah_zap,shet.ravno(),mp,gp,0,0,"",podr,"",zp);
 }
else
 {

  
  double bb2=0.;
  if(metkarnb == 1) //Последовательный метод расчёта налогов
   {
    sprintf(strsql,"\n%s-Расчёт подоходного для бюджетных начислентий\n",__FUNCTION__);
    zar_pr_ins(strsql);
      
    class zar_sumtn tnsum_b; 

    zar_sumtn(mp,gp,tn,1,&tnsum_b); /*читаем всё что нужно для расчёта*/
    bb2=podohrs2016(tn,mp,gp,1,metka_p_boln,&tnsum_b);
    sprintf(strsql,"%s-Бюджет последовательный:%.2f Счёт:%s\n",__FUNCTION__,bb2,shetb.ravno());
    zar_pr_ins(strsql);
   } 

  if(metkarnb == 0) //Пропорциональный метод расчёта налогов
   {
    class zar_sumtn tnsum_b; 
    zar_sumtn(mp,gp,tn,1,&tnsum_b); /*Узнаём бюджетную сумму*/

    sprintf(strsql,"%s-Пропорционально:Разбивка суммы:%.2f не бюджет=%.2f бюджет:%.2f\n",__FUNCTION__,bb,sumanb,sumabu);
    zar_pr_ins(strsql);

    sumabu=tnsum_b.suma_nah_all_podoh;


    if(sumabu != 0.)
     {
      
      if(tnsum.suma_nah_all_podoh != 0.)
       {
        bb=tnsum.suma_nah_all_podoh-tnsum_b.suma_nah_all_podoh;
        //Если хозрасчёт не равен нолю то долю определять надо
        if(bb != 0.)
         bb2=nah_zap*sumabu/tnsum.suma_nah_all_podoh;
        else
         bb2=nah_zap;
       }
     }
   }

  bb2=okrug(bb2,okrg);

  sprintf(strsql,"%s-Бюджет:%.2f Счёт:%s %f\n",__FUNCTION__,bb2,shetb.ravno(),bb2);
  zar_pr_ins(strsql);
  if(shetb.getdlinna() > 1)
   {
    koment.new_plus(gettext("Общий фонд"));
    strcpy(zp.shet,shetb.ravno());
    if(metka_zap == 0)
      zapzarp(d,mp,gp,tn,2,kodpn,bb2,shetb.ravno(),mp,gp,0,0,koment.ravno(),podr,"",zp);
   }

//  polen(shpnb,&shet,1,',');
  bb2=nah_zap-bb2;
  bb2=okrug(bb2,okrg);

  sprintf(strsql,"%s-Хозрасчёт:%.2f(%.2f) Счёт:%s\n",__FUNCTION__,bb2,nah_zap,shet.ravno());
  zar_pr_ins(strsql);
  if(shet.getdlinna() > 1)
   {
    koment.new_plus(gettext("Специальный фонд"));
    strcpy(zp.shet,shet.ravno());
    if(metka_zap == 0)
      zapzarp(d,mp,gp,tn,2,kodpn,bb2,shet.ravno(),mp,gp,0,0,koment.ravno(),podr,"",zp);
   }
  
 }


return(nah);
}

/********************************/
/*Узнаём сумму больничных в счёт расчётного месяца и сумму единого социального взноса с больничных*/
/***********************************************/
double ras_podog1_sb(int tabn,
short mr,short gr,double *suma_esv_s_bol)
{
extern int kod_esv_bol; /*Код удержания единого социального взноса с больничного*/
extern int kod_esv_bol_inv; /*Код удержания единого социального взноса с больничных для инвалидов*/
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
int kolstr=0;
*suma_esv_s_bol=0.;
double boln=0.;
sprintf(strsql,"select prn,knah,suma,mesn from Zarp where tabn=%d and datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' \
and suma <> 0.",tabn,gr,mr,gr,mr);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(0.);
 }

if(kolstr == 0)
 {
  return(0.);
 }
double nah_suma_bol=0.;
while(cur.read_cursor(&row) != 0)
 {
  if(atoi(row[0]) == 1)
   {  
    
    if(provkod(kodbl,atoi(row[1])) >= 0)
     {
      if(mr == atoi(row[3]))
       nah_suma_bol+=atof(row[2]);
      boln+=atof(row[2]);
     }
   }
  else
   {
    int knah=atoi(row[1]);
    if(kod_esv_bol == knah || kod_esv_bol_inv == knah)
     *suma_esv_s_bol+=atof(row[2]);
   }
 }
if(nah_suma_bol > 0.)
 {
  double bb=*suma_esv_s_bol;
  *suma_esv_s_bol=*suma_esv_s_bol*nah_suma_bol/boln;
  sprintf(strsql,"%s-Вычисляем сумму единого социального взноса приходящююся на больничный=%.2f*%.2f/%.2f=%.2f\n",
  __FUNCTION__,bb,nah_suma_bol,boln,*suma_esv_s_bol);
  zar_pr_ins(strsql);
 }

return(nah_suma_bol);

}
/*******************************/
/*Расчёт доли подоходного налога с больничного*/
/*************************************************/
double ras_podor1_rpnsb(int tbn,short mr,short gr,int podr,int metka_zap,double suma_nahisl,double lgota)
{
extern float  ppn; //Процент подоходного налога
//extern double lgota; /*сумма льготы у работника расчитывается в podohrs2.c*/
float proc_pn=ppn;

char strsql[1024];
double suma_esv_s_bol=0.;
double suma_boln= ras_podog1_sb(tbn,mr,gr,&suma_esv_s_bol);
double suma_pn_s_bol=0.;
if(sravmydat(1,1,2016,1,mr,gr) <= 0)
 proc_pn=ICEB_PROCENT_PODOH_NAL_2016;
 
if(suma_boln != 0.)
 {  
  double suma_lgot_for_bol=0.;
  if(lgota > 0.)
   {
    if(suma_nahisl > 0.)
      suma_lgot_for_bol=lgota*suma_boln/suma_nahisl;
    else
      suma_lgot_for_bol=lgota;
   }  

  suma_pn_s_bol=(suma_boln-suma_lgot_for_bol+suma_esv_s_bol)*proc_pn/100.;

  if(suma_pn_s_bol < 0.)
   suma_pn_s_bol=0.;
  else
   suma_pn_s_bol*=-1;
   
  sprintf(strsql,"%s-Определяем сколько подоходного налога приходится на больничные\n",__FUNCTION__);
  zar_pr_ins(strsql);

  suma_pn_s_bol=okrug(suma_pn_s_bol,okrg);  

  sprintf(strsql,"%s-(%.2f-%.2f+%.2f)*%.2f=%.2f\n",__FUNCTION__,suma_boln,suma_lgot_for_bol,suma_esv_s_bol,proc_pn/100.,suma_pn_s_bol);
  zar_pr_ins(strsql);
 }

short d=1;

dpm(&d,&mr,&gr,5);

class ZARP     zp;
zp.dz=d;
zp.mesn=mr; zp.godn=gr;
zp.nomz=0;
zp.podr=podr;

class iceb_tu_str shet("");
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='2' and knah=%d",tbn,kodpn_sbol);
if(sql_readkey(&bd,strsql,&row,&cur) > 0)
  shet.new_plus(row[0]);
else
 {
  sprintf(strsql,"select shet from Uder where kod=%d",kodpn_sbol);
  if(readkey(strsql,&row,&cur) == 1)
   shet.new_plus(row[0]);
 }

if(shet.getdlinna() <= 1)
 {
  sprintf(strsql,"%s-Не нашли счёт для кода удержания подоходного налога с больничного %d\n",__FUNCTION__,kodpn_sbol);
  zar_pr_ins(strsql);
 }
else
 if(metka_zap == 0)
  {
   strcpy(zp.shet,shet.ravno());
   zapzarp(d,mr,gr,tbn,2,kodpn_sbol,suma_pn_s_bol,shet.ravno(),mr,gr,0,0,gettext("с больничного"),podr,"",zp);
  }

return(suma_pn_s_bol);

}
