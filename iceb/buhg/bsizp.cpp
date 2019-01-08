/*$Id: bsizp.c,v 5.20 2013/09/26 09:43:29 sasa Exp $*/
/*04.07.2015	21.10.2005	Белых А.И.	bsizp.c
Получение балансовой стоимости и износа за период
Если вернули 0-нашли запись
	     1-не нашли
*/
#include        "buhg.h"


extern short	startgoduos; /*Стартовый год*/

int bsizp(int in, //Инвентарный номер
int ceh, //Цех/участок
int kodmo, /*код материально-ответственного*/
short dn,short mn,short gn,
short dk,short mk,short gk,
class bsiz_data *data,FILE *ff_prot)
{
long		kolstr;
SQL_str         row;
class SQLCURSOR curr;
char		strsql[1024];
double		bss=0.,izz=0.,bssby=0.,izzby=0.;
short		d,m,g;
short		tip;
class iceb_tu_str startos(gettext("ОСТ"));

if(ff_prot != NULL)
 fprintf(ff_prot,"\n%s-Инвентарный номер %d Подразделение %d Мат.отвественный %d\n-----------------------------\n",__FUNCTION__,in,ceh,kodmo);

 
data->clear_data();

if(ceh == 0 && kodmo == 0)
 return(0);



sprintf(strsql,"select datd,tipz,bs,iz,kodop,bsby,izby from Uosdok1 where \
datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' and innom=%d \
and podr=%d and kodol=%d and podt=1 order by datd asc",startgoduos,1,1,gk,mk,dk,in,ceh,kodmo);

if(kodmo == 0)
 sprintf(strsql,"select datd,tipz,bs,iz,kodop,bsby,izby from Uosdok1 where \
datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' and innom=%d \
and podr=%d and podt=1 order by datd asc",startgoduos,1,1,gk,mk,dk,in,ceh);

if(ceh == 0)
 sprintf(strsql,"select datd,tipz,bs,iz,kodop,bsby,izby from Uosdok1 where \
datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' and innom=%d \
and kodol=%d and podt=1 order by datd asc",startgoduos,1,1,gk,mk,dk,in,kodmo);

if(ff_prot != NULL)
 fprintf(ff_prot,"%s\n",strsql);
  
class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }



if(kolstr == 0)
  return(1);
double itogo[4];
memset(itogo,'\0',sizeof(itogo));
while(cur.read_cursor(&row) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %s %10s %10s %10s %10s %-*s\n",iceb_tu_datzap(row[0]),iceb_tu_tipz(row[1]),row[2],row[3],
   row[5],row[6],
   iceb_tu_kolbait(5,row[4]),
   row[4]);

  tip=atoi(row[1]);
  rsdat(&d,&m,&g,row[0],2);
  bss=atof(row[2]);
  bssby=atof(row[5]);

  /*Износ нужно брать только в последнем документе*/
  
  izz=atof(row[3]);  
  izzby=atof(row[6]);  
  
  if(tip == 2)
   {
    bss*=(-1.);
    izz*=(-1.);
    bssby*=(-1.);
    izzby*=(-1.);
   }

  itogo[0]+=bss;
  itogo[1]+=izz;
  itogo[2]+=bssby;
  itogo[3]+=izzby;

  if(sravmydat(d,m,g,dn,mn,gn) < 0) //до начала периода
   {
    data->nu.bs_nah_per+=bss;
    data->nu.izmen_iznosa_nah_per+=izz;

    data->bu.bs_nah_per+=bssby;
    data->bu.izmen_iznosa_nah_per+=izzby;
   }
  else
   {
    if(tip == 2)
     {
      data->nu.bs_rash_za_per+=bss;
      data->bu.bs_rash_za_per+=bssby;
      data->nu.iznos_rash_za_per+=izz;
      data->bu.iznos_rash_za_per+=izzby;
     }
    else
     {
      if(startgoduos == g && SRAV(row[4],startos.ravno(),0) == 0)
       {
        //это стартовый приход
       }
      else
       {
        data->nu.bs_prih_za_per+=bss;
        data->bu.bs_prih_za_per+=bssby;
        data->nu.iznos_prih_za_per+=izz;
        data->bu.iznos_prih_za_per+=izzby;
       }
     }
   } 
  data->nu.bs_kon_per+=bss;
  data->nu.izmen_iznosa_kon_per+=izz;

  data->bu.bs_kon_per+=bssby;
  data->bu.izmen_iznosa_kon_per+=izzby;
 }
if(ff_prot != NULL)
 fprintf(ff_prot,"%*s:%10.2f %10.2f %10.2f %10.2f\n",
 iceb_tu_kolbait(12,gettext("Итого")),
 gettext("Итого"),
 itogo[0],itogo[1],itogo[2],itogo[3]);


/*Узнаем амортизацию для налогового учёта*/
/*********************
if(startgoduos != gn)
  sprintf(strsql,"select god,mes,suma from Uosamor where \
((god = %d and mes >= %d) or \
(god > %d and god < %d) or \
(god = %d and mes <= %d)) and \
innom=%d and podr=%d and suma <> 0. order by god,mes asc",
startgoduos,1,startgoduos,gk,gk,mk,in,ceh);
else
  sprintf(strsql,"select god,mes,suma from Uosamor where \
god = %d and mes >= %d and  mes <= %d and \
innom=%d and podr=%d and suma <> 0. order by god,mes asc",
startgoduos,1,mk,in,ceh);
************************/
sprintf(strsql,"select da,suma from Uosamor where da >= '%04d-01-01' and da <= '%04d-%02d-31' and innom=%d and podr=%d and kmo=%d and suma <> 0. order by da asc",startgoduos,gk,mk,in,ceh,kodmo);

if(kodmo == 0)
 sprintf(strsql,"select da,suma from Uosamor where da >= '%04d-01-01' and da <= '%04d-%02d-31' and innom=%d and podr=%d and suma <> 0. order by da asc",startgoduos,gk,mk,in,ceh);
if(ceh == 0)
 sprintf(strsql,"select da,suma from Uosamor where da >= '%04d-01-01' and da <= '%04d-%02d-31' and innom=%d and kmo=%d and suma <> 0. order by da asc",startgoduos,gk,mk,in,kodmo);

if(ff_prot != NULL)
 fprintf(ff_prot,"\nАмортизация в налоговом учёте\n----------------------------\n%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
 }
double itogo_amort=0.;
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  if(ff_prot != NULL)
    fprintf(ff_prot,"%s %s\n",iceb_tu_datzap(row[0]),row[1]);
  rsdat(&d,&m,&g,row[0],2);
  
  //m=atoi(row[1]);
//  g=atoi(row[0]);

  bss=atof(row[1]);
  if(sravmydat(1,m,g,1,mn,gn) < 0)
    data->nu.amort_nah_per+=bss;

  data->nu.amort_kon_per+=bss;
  itogo_amort+=bss;
 }
if(ff_prot != NULL)
  fprintf(ff_prot,"На начало периода=%.2f\nНа конец периода=%.2f\nИтого=%.2f\n",
  data->nu.amort_nah_per,
  data->nu.amort_kon_per,
  itogo_amort);


/*Узнаем амортизацию для бухгалтерского учёта*/
/*******************
if(startgoduos != gn)
  sprintf(strsql,"select god,mes,suma from Uosamor1 where \
((god = %d and mes >= %d) or \
(god > %d and god < %d) or \
(god = %d and mes <= %d)) and \
innom=%d and podr=%d and suma <> 0. order by god,mes asc",
startgoduos,1,startgoduos,gk,gk,mk,in,ceh);
else
  sprintf(strsql,"select god,mes,suma from Uosamor1 where \
god = %d and mes >= %d and  mes <= %d and \
innom=%d and podr=%d and suma <> 0. order by god,mes asc",
startgoduos,1,mk,in,ceh);
*********************/
sprintf(strsql,"select da,suma from Uosamor1 where da >= '%04d-01-01' and da <= '%04d-%02d-31' and innom=%d and podr=%d and kmo=%d and suma <> 0. order by da asc",startgoduos,gk,mk,in,ceh,kodmo);
if(kodmo == 0)
 sprintf(strsql,"select da,suma from Uosamor1 where da >= '%04d-01-01' and da <= '%04d-%02d-31' and innom=%d and podr=%d and suma <> 0. order by da asc",startgoduos,gk,mk,in,ceh);
if(ceh == 0)
 sprintf(strsql,"select da,suma from Uosamor1 where da >= '%04d-01-01' and da <= '%04d-%02d-31' and innom=%d and kmo=%d and suma <> 0. order by da asc",startgoduos,gk,mk,in,kodmo);

if(ff_prot != NULL)
 fprintf(ff_prot,"\nАмортизация в бух.учёте\n----------------------------------------------\n%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
 }

itogo_amort=0.;
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  if(ff_prot != NULL)
    fprintf(ff_prot,"%s %s\n",iceb_tu_datzap(row[0]),row[1]);
  rsdat(&d,&m,&g,row[0],2);
  
//  m=atoi(row[1]);
//  g=atoi(row[0]);

  bss=atof(row[1]);
  if(sravmydat(1,m,g,1,mn,gn) < 0)
    data->bu.amort_nah_per+=bss;

  data->bu.amort_kon_per+=bss;
  itogo_amort+=bss;
 }
if(ff_prot != NULL)
  fprintf(ff_prot,"На начало периода=%.2f\nНа конец периода=%.2f\nИтого=%.2f\n",
  data->bu.amort_nah_per,
  data->bu.amort_kon_per,
  itogo_amort);


data->nu.bs_raznica=data->nu.bs_kon_per-data->nu.bs_nah_per;
data->nu.izmen_iznosa_raznica=data->nu.izmen_iznosa_kon_per-data->nu.izmen_iznosa_nah_per;
data->nu.amort_raznica=data->nu.amort_kon_per-data->nu.amort_nah_per;

data->bu.bs_raznica=data->bu.bs_kon_per-data->bu.bs_nah_per;
data->bu.izmen_iznosa_raznica=data->bu.izmen_iznosa_kon_per-data->bu.izmen_iznosa_nah_per;
data->bu.amort_raznica=data->bu.amort_kon_per-data->bu.amort_nah_per;

if(ff_prot != NULL)
 {
  fprintf(ff_prot,"Налоговый.учёт-Инвен.ном=%d Подр=%d\n\
bs_nah_per=%.2f\n\
izmen_iznosa_nah_per=%.2f\n\
amort_nah_per=%.2f\n\n\
bs_kon_per=%.2f\n\
izmen_iznosa_kon_per=%.2f\n\
amort_kon_per=%.2f\n\n\
bs_prih_za_per=%.2f\n\
bs_rash_za_per=%.2f\n\n\
iznos_prih_za_per=%.2f\n\
iznos_rash_za_per=%.2f\n\n\
bs_raznica=%.2f\n\
izmen_iznosa_raznica=%.2f\n\
amort_raznica=%.2f\n\n",
  in,ceh,
  data->nu.bs_nah_per,
  data->nu.izmen_iznosa_nah_per,
  data->nu.amort_nah_per,
  data->nu.bs_kon_per,
  data->nu.izmen_iznosa_kon_per,
  data->nu.amort_kon_per,
  data->nu.bs_prih_za_per,
  data->nu.bs_rash_za_per,
  data->nu.iznos_prih_za_per,
  data->nu.iznos_rash_za_per,
  data->nu.bs_raznica,
  data->nu.izmen_iznosa_raznica,
  data->nu.amort_raznica);

  fprintf(ff_prot,"Бух.учёт-Инвен.ном=%d Подр=%d\n\
bs_nah_per=%.2f\n\
izmen_iznosa_nah_per=%.2f\n\
amort_nah_per=%.2f\n\
bs_kon_per=%.2f\n\
izmen_iznosa_kon_per=%.2f\n\
amort_kon_per=%.2f\n\
bs_prih_za_per=%.2f\n\
bs_rash_za_per=%.2f\n\
iznos_prih_za_per=%.2f\n\
iznos_rash_za_per=%.2f\n\
bs_raznica=%.2f\n\
izmen_iznosa_raznica=%.2f\n\
amort_raznica=%.2f\n",
  in,ceh,
  data->bu.bs_nah_per,
  data->bu.izmen_iznosa_nah_per,
  data->bu.amort_nah_per,
  data->bu.bs_kon_per,
  data->bu.izmen_iznosa_kon_per,
  data->bu.amort_kon_per,
  data->bu.bs_prih_za_per,
  data->bu.bs_rash_za_per,
  data->bu.iznos_prih_za_per,
  data->bu.iznos_rash_za_per,
  data->bu.bs_raznica,
  data->bu.izmen_iznosa_raznica,
  data->bu.amort_raznica);
 }

return(0);

}
