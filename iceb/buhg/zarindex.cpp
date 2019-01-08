/* $Id: zarindex.c,v 5.39 2014/06/30 06:36:35 sasa Exp $ */
/*20.04.2016	19.05.2000	Белых А.И.	zarindex.c
Расчёт индексации зарплаты
*/
#include        <errno.h>
#include        "buhg.h"
#include "read_nast_ind.h"

extern class index_ua tabl_ind; /*Таблица для индексации зарплаты*/

extern short	kodindex;  /*Код индексации*/
extern short    *kodnvrindex; //Коды не входящие в расчёт индексации
extern double   okrg; /*Округление*/
extern short    ggg,mmm;
double zarindex_tb(int tn,short mp,short gp,double suma_ind);

void zarindex(int tn, //Табельный номер*/
short mp,short gp,int podr,const char *nah_only)
{
int		i;
char            strsql[512];
short		mi,gi; /*месяц индексации*/
double		procind; /*процент индексации*/
double		sumaog=0;  /*Сумма ограничения индексации*/
double		suman;   /*Сумма начисленная в месяце индексации*/
double		sumain;  /*Сумма индексации*/
class iceb_tu_str shet("");
short		d;
long		kolstr;
SQL_str         row;
SQLCURSOR cur;
class ZARP     zp;
double		insum=0.; //Индексируемая сумма
double porog_ind=1.009999; /*1.01*/
if(sravmydat(1,1,2016,1,mp,gp) <= 0)
 porog_ind=1.029999; /*1.03*/
 
sprintf(strsql,"\n%s-Расчёт индексации начислений\n\
-----------------------------------------------\n",__FUNCTION__);
zar_pr_ins(strsql);

if(kodindex <= 0)
 {
  sprintf(strsql,"%s-В файле настройки zarnast.alx не введён код индексации!\n",__FUNCTION__);
  zar_pr_ins(strsql);
  return;
 }
if(proverka(nah_only,kodindex,0,0) != 0)
 {
  sprintf(strsql,"%s-Код индексации %d исключён из расчёта\n",__FUNCTION__,kodindex);
  zar_pr_ins(strsql);
  return;
 }

/*Проверяем есть ли индексация в списке начислений*/
sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='1' and \
knah=%d",tn,kodindex);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  
  sprintf(strsql,"%s-Не введён код индексации %d в список начислений!\n",__FUNCTION__,kodindex);
  zar_pr_ins(strsql);
  return;

 }

shet.new_plus(row[0]);

sprintf(strsql,"%s-Коды не входящие в расчёт индексации:",__FUNCTION__);
zar_pr_ins(strsql);

if(kodnvrindex != NULL)
 for(i=1; i <= kodnvrindex[0]; i++)
  {
   sprintf(strsql,"%d ",kodnvrindex[i]);
   zar_pr_ins(strsql);
  }
sprintf(strsql,"\n");
zar_pr_ins(strsql);

//Читаем дату приёма на работу и дату последнего повышения зарплаты
sprintf(strsql,"select datn,datk,dppz from Kartb where tabn=%d",tn);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s-Не найдена карточка для табельного номера %d\n",__FUNCTION__,tn);
  zar_pr_ins(strsql);
  return;
 }


short dppz=0,mppz=0,gppz=0; //Дата последнего повышения зарплаты
short dnr=0,mnr=0,gnr=0;    //Дата начала работы
short mb=0,gb=0;       //базовый месяц для расчёта индексации

short du=0,mu=0,gu=0;
rsdat(&du,&mu,&gu,row[1],2);

if(rsdat(&dppz,&mppz,&gppz,row[2],2) == 0)
 {
  sprintf(strsql,"%s-Дата последнего повышения зарплаты %d.%d\n",__FUNCTION__,mppz,gppz);
  zar_pr_ins(strsql);
 }
if(rsdat(&dnr,&mnr,&gnr,row[0],2) == 0)
 {
  sprintf(strsql,"%s-Дата принятия на работу %d.%d.%d\n",__FUNCTION__,dnr,mnr,gnr);
  zar_pr_ins(strsql);
 } 

d=1;
dpm(&d,&mp,&gp,5);
zp.dz=d;
zp.nomz=0;
zp.podr=podr;

int kolih_mr=tabl_ind.mr.kolih();
if(kolih_mr == 0)
 {
  sprintf(strsql,"%s-Не найдено ни одного месяца расчёта индексации !\n",__FUNCTION__);
  zar_pr_ins(strsql);
  return;
 }
class zar_read_tn1h nast_mz;

for(int kol=0; kol < kolih_mr ; kol++)
 {
  mi=tabl_ind.mr.ravno(kol);
  gi=tabl_ind.gr.ravno(kol);

  


  if(gppz != 0)
   {
    if(sravmydat(1,mppz,gppz,1,mi,gi) > 0)
     {
      mb=mnr; gb=gnr;
     }
    else
     {
      mb=mppz; gb=gppz;
     }
   }
  else
   {
    mb=mnr; gb=gnr;
   }  

  zar_read_tn1(1,mi,gi,&nast_mz); /*читаем какая минимальная зарплата действует в этом месяце*/
  sumaog=nast_mz.prog_min_tek;
  sprintf(strsql,"\n%s-Дата индексации:%d.%d Максимальная сумма индексации:%.2f\n",__FUNCTION__,mi,gi,sumaog);
  zar_pr_ins(strsql);

  if(sumaog == 0.)
   {
    sprintf(strsql,"%s-Прожиточный минимум равен нолю!\n",__FUNCTION__);
    zar_pr_ins(strsql);
    continue;

   }
  int kol_mes_tabl=tabl_ind.mi.kolih();
  sprintf(strsql,"%s-Количество месяцев в таблице=%d\n",__FUNCTION__,kol_mes_tabl);
  zar_pr_ins(strsql);

  procind=1.;

  short dpr,mpr,gpr;

  dpr=1; mpr=mi; gpr=gi;
  dpm(&dpr,&mpr,&gpr,4);    
  float index=0;
  float pred_index=1;

  if(sravmydat(1,mu,gu,1,mi,gi) == 0)
   {
    sprintf(strsql,"%s-Уволен %d.%d.%d \n",__FUNCTION__,du,mu,gu);
    zar_pr_ins(strsql);
   }
  else  
  for(int kol_tabl=0; kol_tabl < kol_mes_tabl ; kol_tabl++)
   {
    if(sravmydat(1,mb,gb,1,tabl_ind.mi.ravno(kol_tabl),tabl_ind.gi.ravno(kol_tabl)) < 0)
    if(sravmydat(1,mpr,gpr,1,tabl_ind.mi.ravno(kol_tabl),tabl_ind.gi.ravno(kol_tabl)) > 0)
     {

      index=tabl_ind.koef.ravno(kol_tabl)/100.;
//      index=okrug(index,0.001);

      sprintf(strsql,"%s-%02d.%04d %8.2f %f * %f = %f ",
      __FUNCTION__,
      tabl_ind.mi.ravno(kol_tabl),tabl_ind.gi.ravno(kol_tabl),
      tabl_ind.koef.ravno(kol_tabl),index,pred_index,index*pred_index);
      zar_pr_ins(strsql);

      if(index*pred_index >= porog_ind)
       {

        sprintf(strsql,"%s-Больше или равно %.2f\n%f*%f*%f=",__FUNCTION__,porog_ind,procind,index,pred_index);
        zar_pr_ins(strsql);
        procind*=index*pred_index;

        sprintf(strsql,"%f",procind);
        zar_pr_ins(strsql);

        procind=okrug(procind,0.001);

        sprintf(strsql," Округляем=%f",procind);
        zar_pr_ins(strsql);
        
        pred_index=1.;
       }
      else
       {
        pred_index*=index;
//        pred_index=okrug(pred_index,0.001);
       }      
      sprintf(strsql,"\n\n");
      zar_pr_ins(strsql);
    
     }
   }
  procind=okrug(procind,0.001);
  sprintf(strsql,"%s-Округляем %.4f\n",__FUNCTION__,procind);
  zar_pr_ins(strsql);
   
  
   if(procind != 0.)
     sprintf(strsql,"%s-Величина прироста индекса потребительских цен:%.4f*100-100=%.4f\n",
     __FUNCTION__,
     procind,(procind*100.)-100.);
   else
     sprintf(strsql,"%s-Величина прироста индекса потребительских цен:0.\n",__FUNCTION__);
   zar_pr_ins(strsql);

  if(procind != 0.)
    procind=(procind*100.)-100.;


  /*Читаем все начисления за месяц индексации*/
  sprintf(strsql,"select knah,suma from Zarp where datz >= '%d-%d-01' \
and datz <= '%d-%d-31' and tabn=%d and prn='1' and suma <> 0.",
  gi,mi,gi,mi,tn);
  SQLCURSOR cur;
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

  if(kolstr == 0)
   {
    sprintf(strsql,"%s-Не найдены начисления за %d.%d\n",__FUNCTION__,mi,gi);
    zar_pr_ins(strsql);
    continue;
   }

  suman=0.;
  while(cur.read_cursor(&row) != 0)
   {
    //Если за текущий месяц не должно входить в расчёт чтобы при пересчёте сумма не менялась
    if(atoi(row[0]) == kodindex && mmm == mi && ggg == gi)
      continue;
      
    if(provkod(kodnvrindex,atoi(row[0])) >= 0)
      continue;
    
    suman+=atof(row[1]);
   }

  sprintf(strsql,"%s-Сумма начислений которая индексируется-%.2f\n",__FUNCTION__,suman);
  zar_pr_ins(strsql);
  insum=suman;
  if(suman > sumaog || suman < sumaog)
    insum=sumaog;

  sprintf(strsql,"%s-Максимальная сумма индексации:%.2f Процент индексации:%f\n\
Сумма начисленная:%.2f Сумма взятая для индексации:%.2f \n",__FUNCTION__,sumaog,procind,suman,insum);
  zar_pr_ins(strsql);
      
  sumain=insum*procind/100.;
  sumain=okrug(sumain,okrg);

  sprintf(strsql,"%s-Код индексации:%d Сумма индексации:%.2f\n",__FUNCTION__,kodindex,sumain);
  zar_pr_ins(strsql);

  sumain=zarindex_tb(tn,mp,gp,sumain);/*перерасчёт в соотвецтвии с реально отработанным временем*/

  zp.mesn=mi; zp.godn=gi;

  strcpy(zp.shet,shet.ravno());
  zapzarp(zp.dz,mp,gp,tn,1,kodindex,sumain,shet.ravno(),mi,gi,0,0,"",podr,"",zp);

 }

//OSTANOV();
}
/******************************/

double zarindex_tb(int tn, //Табельный номер*/
short mp,short gp,double suma_ind)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
int kolstr=0;
float dnei=0.;
float has=0.;
class iceb_tu_str ktvr("");

if(suma_ind == 0.)
 return(0.);
/*узнаём количество рабочих часов и дней в месяце*/
redkal(mp,gp,&dnei,&has);

sprintf(strsql,"%s-Количество рабочих дней %.2f Количество рабочих часов%.2f Сумма индексации:%.2f\n",
__FUNCTION__,dnei,has,suma_ind);

zar_pr_ins(strsql);

if(dnei == 0. && has == 0.)
 return(suma_ind);

if(iceb_t_poldan("Коды табеля входящие в расчёт индексации",&ktvr,"zarnast.alx") != 0)
 return(suma_ind);

sprintf(strsql,"%s-Коды табеля входящие в расчёт:%s\n",__FUNCTION__,ktvr.ravno());
zar_pr_ins(strsql);

/*ОПРЕДЕЛЯЕМ ОТРАБОТАННОЕ ВРЕМЯ*/
sprintf(strsql,"select kodt,dnei,has from Ztab where god=%d and mes=%d and tabn=%d",
gp,mp,tn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
float odnei=0.;
float ohas=0.;
while(cur.read_cursor(&row) != 0)
 {
  if(proverka(ktvr.ravno(),row[0],0,0) != 0)
   continue;
  odnei+=atof(row[1]);
  ohas+=atof(row[2]);
 }

sprintf(strsql,"%s-Количество отработанных дней %.2f Количество отработанных часов %.2f\n",__FUNCTION__,odnei,ohas);
zar_pr_ins(strsql);

double nsum_ind=0.;
if(ohas == 0. && odnei == 0.)
 return(suma_ind);

if(ohas != 0. && has != 0.)
 {
  nsum_ind=suma_ind/has*ohas;
  nsum_ind=okrug(nsum_ind,0.01);

  sprintf(strsql,"%s-Пересчитываем индексацию %.2f/%.2f*%.2f=%.2f\n",
  __FUNCTION__,suma_ind,has,ohas,nsum_ind);
  zar_pr_ins(strsql);
 } 
else
 {
  if(dnei == 0.)
   return(suma_ind);
  nsum_ind=suma_ind/dnei*odnei;
  nsum_ind=okrug(nsum_ind,0.01);
  sprintf(strsql,"%s-Пересчитываем индексацию %.2f/%.2f*%.2f=%.2f\n",
  __FUNCTION__,suma_ind,dnei,odnei,nsum_ind);
  zar_pr_ins(strsql);
 }
return(nsum_ind); 
}

