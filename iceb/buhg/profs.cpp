/* $Id: profs.c,v 5.38 2013/09/26 09:43:37 sasa Exp $ */
/*20.04.2016    02.12.1992      Белых А.И.      profs.c
Подпрограмма расчёта отчисления в профсоюзный фонд
*/
#include        "buhg.h"

extern short	metkarnb; //0-расчёт налогов для бюджета пропорциональный 1-последовательный
extern double   okrg;
extern short	kuprof; //Код удержания в профсоюзный фонд
extern float	procprof; //Процент отчисления в профсоюзный фонд
extern short	*knvrprof; //Коды не входящие в расчёт удержаний в профсоюзный фонд
extern short    *kodmp;   /*Коды материальной помощи*/

void profs(int tn,short mp,short gp,int podr,const char *uder_only)
{
double          nah,nah1,bb2;
short           d;
class iceb_tu_str shet("");
class iceb_tu_str shetb("");
SQL_str         row;
char		strsql[1024];
double		sumanb,sumabu;
class ZARP     zp;
double		matpomb=0.;
double          suma_zarp_o; //Сумма чистой зарплаты общая
double          suma_zarp_b; //Сумма чистой зарплаты бюджетная
double suma_boln_rm=0.;
SQLCURSOR cur;
/*
printw("profs---------------\n");
refresh();
*/
if(kuprof == 0)
  return;

class zar_read_tn1h nastr;
zar_read_tn1(1,mp,gp,&nastr);

dpm(&d,&mp,&gp,5);
zp.dz=d;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;
  
/*Проверяем есть ли код удержания в списке*/
sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='2' and knah=%d",tn,kuprof); 
//printw("%s\n",strsql);
if(readkey(strsql,&row,&cur) > 0)
 {
  
  sprintf(strsql,"\n%s-Расчёт профсоюзного отчисления\n\
-----------------------------------------------\n",__FUNCTION__);
  zar_pr_ins(strsql);


  if(knvrprof != NULL)
   {
    sprintf(strsql,"Коды невходящие в расчёт:");
    zar_pr_ins(strsql);
   
    for(int kkk=1; kkk < knvrprof[0] ; kkk ++)
     {
       sprintf(strsql,"%d ",knvrprof[kkk]);
       zar_pr_ins(strsql);
     }
    sprintf(strsql,"\n");
    zar_pr_ins(strsql);
   }


  if(proverka(uder_only,kuprof,0,0) != 0)
   {
    sprintf(strsql,"%s-Код %d исключён из расчёта\n",__FUNCTION__,kuprof);
    zar_pr_ins(strsql);
    return;   
   }

  shet.new_plus(row[0]);

  sprintf(strsql,"select shet,shetb from Uder where kod=%d",kuprof);
  if(readkey(strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s-Не найден код удержания %d в справочнике удержаний!\n",__FUNCTION__,kuprof );
    zar_pr_ins(strsql);
    return;
   }

  shetb.new_plus(row[1]);
  if(shetb.getdlinna() > 1)
    shet.new_plus(row[0]);
  
  double suma_nfnz,suma_nfnz_b;
  nah=findnahm(mp,gp,tn,3,knvrprof,NULL,&sumanb,&sumabu,&suma_nfnz,&suma_nfnz_b,&suma_zarp_o,&suma_zarp_b,&suma_boln_rm);

  sprintf(strsql,"%s-Берем в расчёт %.2f\n",__FUNCTION__,nah);
  zar_pr_ins(strsql);

  if(nah > nastr.max_sum_for_soc)
   {
    sprintf(strsql,"%s-Превышение максимума %.2f > %.2f\n",__FUNCTION__,nah,nastr.max_sum_for_soc);
    zar_pr_ins(strsql);
    nah=nastr.max_sum_for_soc;      
   }
  

  nah1=nah*procprof/100.*(-1);
  nah1=okrug(nah1,okrg);
  sprintf(strsql,"%s-%.2f*%.2f/100=%.2f\n",__FUNCTION__,nah,procprof,nah*procprof/100.);
  zar_pr_ins(strsql);

  if(shetb.getdlinna() <= 1)
   {
    strcpy(zp.shet,shet.ravno());
    sprintf(strsql,"%.2f%%",procprof);
   
    zapzarp(d,mp,gp,tn,2,kuprof,nah1,shet.ravno(),mp,gp,0,0,strsql,podr,"",zp); 
   }
  else
   {
    bb2=0.;
    if(metkarnb == 1)
     {
      if(sumabu > nastr.max_sum_for_soc)
       {
        sprintf(strsql,"%s-Превышение максимума суммы для бюджета %.2f > %.2f\n",__FUNCTION__,sumabu,nastr.max_sum_for_soc);
        zar_pr_ins(strsql);
        sumabu=nastr.max_sum_for_soc;
       }

      bb2=(sumabu-matpomb)*procprof/100.*(-1);
      sprintf(strsql,"%s-Бюджетный профсоюз (%.2f-%.2f)*%f/100.*-1=%.2f",__FUNCTION__,sumabu,matpomb,procprof,bb2);
      zar_pr_ins(strsql);
     }
    if(metkarnb == 0)
     {
      sprintf(strsql,"%s-Разбивка суммы:%.2f не бюджет=%.2f бюджет:%.2f\n",__FUNCTION__,nah,sumanb,sumabu);
      zar_pr_ins(strsql);
      if(sumabu > 0.009)
       {
        //Если хозрасчёт не равен нолю то долю определять надо
        if(sumanb != 0.)
         bb2=nah1*sumabu/nah;
        else
         bb2=nah1;
       }
     }

    bb2=okrug(bb2,okrg);
    sprintf(strsql,"%s-Налог бюджет=%.2f\n",__FUNCTION__,bb2);
    zar_pr_ins(strsql);
    if(shetb.ravno()[0] != '\0')
     {
      strcpy(zp.shet,shetb.ravno());
      sprintf(strsql,"%s %.2f%%",gettext("Общий фонд"),procprof);
      zapzarp(d,mp,gp,tn,2,kuprof,bb2,shetb.ravno(),mp,gp,0,0,strsql,podr,"",zp);
     }

    bb2=nah1-bb2;
    bb2=okrug(bb2,okrg);
    if(shet.ravno()[0] != '\0')
     {
      strcpy(zp.shet,shet.ravno());
      sprintf(strsql,"%s %.2f%%",gettext("Специальный фонд"),procprof);
      zapzarp(d,mp,gp,tn,2,kuprof,bb2,shet.ravno(),mp,gp,0,0,strsql,podr,"",zp);
     }
   }
 }

}
