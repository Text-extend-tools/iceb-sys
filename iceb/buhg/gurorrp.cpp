/*$Id: gurorrp.c,v 5.39 2013/09/26 09:43:33 sasa Exp $*/
/*07.02.2016	12.06.2002	Белых А.И.	gurorrp.c
Для счета с развернутым сальдо распечатка проводок по контрагентам
*/
#include        <errno.h>
#include        <math.h>
#include        "buhg.h"

void	prosmotrdo(short,short,short,const char*,const char*,short,double*,double*,short*,short*,short*,double*,double*,short,const char *kod_val);
void numlist(int *kolstrlist,int *kollist,FILE *ff);

extern short	startgodb; /*Стартовый год*/
extern short	vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/
extern int      kol_strok_na_liste;

int gurorrp(class guror_rr *rekrs,int metka_sort,  class spis_oth *oth)
{
SQLCURSOR cur;
SQLCURSOR cur1;
short dn=0,mn=0,gn=0;
short dk=0,mk=0,gk=0;
int		kolstr=0,kolstr1=0,kolstr2=0;
SQL_str         row,row1;
char		strsql[1024];
short		godn;
char		imaf[64],imafs[64];
FILE		*ff;
time_t          tmm;
int		kolstlst=0;
class iceb_tu_str koment("");
int             kollist=0;
class iceb_getkue_dat getval(rekrs->kod_val.ravno());
class iceb_tu_str data_saldo("");

time(&tmm);

if(rsdatp(&dn,&mn,&gn,rekrs->dat1.ravno(),&dk,&mk,&gk,rekrs->dat2.ravno()) != 0)
 return(1);

sprintf(strsql,"select nais,vids from Plansh where ns='%s'",rekrs->shet.ravno());

if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,gettext("Не найден счёт %s в плане счетов !"),rekrs->shet.ravno());
  iceb_t_soob(strsql);
  return(1);
 }
int vds=atoi(row[1]);
class iceb_tu_str naimshet(row[0]);

clear();

printw("%s %s\n",rekrs->shet.ravno(),naimshet.ravno());
printw("%d.%d.%d%s => %d.%d.%d%s\n",
dn,mn,gn,
gettext("г."),
dk,mk,gk,
gettext("г."));
GDITE();


godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;

sprintf(strsql,"%04d-01-01",godn);
data_saldo.new_plus(strsql);


class SPISOK skontr; /*Список контрагентов по которым были проводки от стартового года до конца периода*/
if(vds == 0 || vplsh == 1)
 sprintf(strsql,"select kodkon from Saldo where kkk='1' and gs=%d and ns like '%s%%'",godn,rekrs->shet.ravno());

if(vds == 1 && vplsh == 0)
 sprintf(strsql,"select kodkon from Saldo where kkk='1' and gs=%d and ns='%s'",godn,rekrs->shet.ravno());

//printw("%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  if(skontr.find_r(row[0]) < 0)
    skontr.plus(row[0]);   
 }

short d=1;
short m=1;
short g=godn;
int kolih_den=period(1,1,godn,dk,mk,gk,0);
kolstr1=0;
while(sravmydat(d,m,g,dk,mk,gk) <= 0)
 {
  
  strzag(LINES-1,0,kolih_den,++kolstr1);

  if(vds == 0 || vplsh == 1)
   sprintf(strsql,"select distinct kodkon from Prov \
 where val=0 and datp='%d-%02d-%02d' and sh like '%s%%'",g,m,d,rekrs->shet.ravno());

  if(vds == 1 && vplsh == 0)
   sprintf(strsql,"select distinct kodkon from Prov \
 where val=0 and datp='%d-%02d-%02d' and sh='%s'",g,m,d,rekrs->shet.ravno());

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return(1);
   }

  
  while(cur.read_cursor(&row) != 0)
   {
    if(row[0][0] == '\0')
     {
      iceb_t_soob(gettext("В проводке нет кода контрагента !"));

      continue;
     }

    if(skontr.find_r(row[0]) < 0)
      skontr.plus(row[0]);

   }

  dpm(&d,&m,&g,1);
 }

if(iceb_t_sortkk(metka_sort+1,&skontr,NULL) != 0)
 return(1);

sprintf(imaf,"gokp%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }


iceb_t_zagolov(gettext("Распечатка проводок по контрагентам"),dn,mn,gn,dk,mk,gk,ff);
fprintf(ff,"%s: %s %s",gettext("Счёт"),rekrs->shet.ravno(),naimshet.ravno());
if(rekrs->kod_val.ravno()[0] != '\0')
 fprintf(ff," %s:%s",gettext("Валюта"),rekrs->kod_val.ravno());
fprintf(ff,"\n");

kolstlst=8;
numlist(&kolstlst,&kollist,ff);

class SPISOK SHET(0);
class masiv_din_double DEB(0);
class masiv_din_double KRE(0);
double  debet=0.,kredit=0.;
double  stdebet=0.,stkredit=0.;
double  dodebet=0.,dokredit=0.;
double  prdebet=0.,prkredit=0.;
class iceb_tu_str naimkontr("");
short   dpp,mpp,gpp;
double  debpp,krepp;
int     nomer=0;
kolstr1=0;
int kolih_kontr=skontr.kolih();
//printw("%s-11111111111111111\n",__FUNCTION__);
//refresh();
for(int nomer_kontr=0; nomer_kontr < kolih_kontr; nomer_kontr++)
 {

  //Читаем наименование контрагента
//  sprintf(strsql,"select naikon,grup from Kontragent where kodkon='%s'",row[0]);   
  sprintf(strsql,"select naikon,grup from Kontragent where kodkon='%s'",skontr.ravno(nomer_kontr));   

  naimkontr.new_plus("");
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    naimkontr.new_plus(row1[0]);

  printw("%-*s %s\n",iceb_tu_kolbait(5,skontr.ravno(nomer_kontr)),skontr.ravno(nomer_kontr),naimkontr.ravno());  

  strzag(LINES-1,0,kolih_kontr,++kolstr1);  

  if(proverka(rekrs->mso.ravno(),row1[1],0,0) != 0)
    continue;
    
  stdebet=stkredit=0.;
  dodebet=dokredit=0.;
  prdebet=prkredit=0.;

  //Узнаем стартовое сальдо по контрагенту
  /*Отчет для многопорядкового плана счетов*/
  if(vplsh == 1)
   sprintf(strsql,"select deb,kre from Saldo where kkk='1' and ns like '%s%%' \
and gs=%d and kodkon='%s'",
   rekrs->shet.ravno(),godn,skontr.ravno(nomer_kontr));

  /*Отчет по субсчету двух-порядковый план счетов*/
  if(vds == 1 && vplsh == 0)
   sprintf(strsql,"select deb,kre from Saldo where kkk='1' and ns='%s' \
and gs=%d and kodkon='%s'",
   rekrs->shet.ravno(),godn,skontr.ravno(nomer_kontr));

  /*Отчет по счёту двух-порядковый план счетов*/
  if(vds == 0 && vplsh == 0)
   sprintf(strsql,"select deb,kre from Saldo where kkk='1' and ns like '%s%%' \
and gs=%d and kodkon='%s'",
   rekrs->shet.ravno(),godn,skontr.ravno(nomer_kontr));

  if(cur1.make_cursor(&bd,strsql) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }
  while(cur1.read_cursor(&row1) != 0)
   {
    stdebet+=getval.toue(row1[0],data_saldo.ravno());
    stkredit+=getval.toue(row1[1],data_saldo.ravno());
//    stdebet+=atof(row1[0]);
//    stkredit+=atof(row1[1]);
   }  

  prosmotrdo(dn,mn,gn,rekrs->shet.ravno(),skontr.ravno(nomer_kontr),vds,&dodebet,&dokredit,&dpp,&mpp,&gpp,&debpp,&krepp,godn,rekrs->kod_val.ravno());

  //Смотрим проводки за период
  /*Отчет для многопорядкового плана счетов*/
  if(vplsh == 1)
    sprintf(strsql,"select datp,shk,kto,nomd,oper,deb,kre,ktoi,komen from Prov where val=0 and \
datp >= '%04d-%d-%d' and datp <= '%04d-%d-%d' and sh like '%s%%' and kodkon='%s' order by datp asc",
    gn,mn,dn,gk,mk,dk,rekrs->shet.ravno(),skontr.ravno(nomer_kontr));

  /*Отчет по субсчету двух-порядковый план счетов*/
  if(vds == 1 && vplsh == 0)
    sprintf(strsql,"select datp,shk,kto,nomd,oper,deb,kre,ktoi,komen from Prov where val=0 and \
datp >= '%04d-%d-%d' and datp <= '%04d-%d-%d' and sh='%s' and kodkon='%s' order by datp asc",
    gn,mn,dn,gk,mk,dk,rekrs->shet.ravno(),skontr.ravno(nomer_kontr));

  /*Отчет по счёту двух-порядковый план счетов*/
  if(vds == 0 && vplsh == 0)
    sprintf(strsql,"select datp,shk,kto,nomd,oper,deb,kre,ktoi,komen from Prov where val=0 and \
datp >= '%04d-%d-%d' and datp <= '%04d-%d-%d' and sh like '%s%%' and kodkon='%s' order by datp asc",
    gn,mn,dn,gk,mk,dk,rekrs->shet.ravno(),skontr.ravno(nomer_kontr));

  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

  debet=stdebet+dodebet;
  kredit=stkredit+dokredit;

  if(kolstr2 == 0)
   if(fabs(debet - kredit) < 0.01)
     continue;

  kolstlst+=2;    
  if(kolstlst > kol_strok_na_liste-6)
   {
    numlist(&kolstlst,&kollist,ff);
    kolstlst++; //Единичка назначена в подпрограме
   }

  fprintf(ff,"\n%s %s %s %02d.%02d.%d => %02d.%02d.%d\n",
  skontr.ravno(nomer_kontr),naimkontr.ravno(),
  gettext("Период с"),
  dn,mn,gn,dk,mk,gk);

  kolstlst+=3;    
  fprintf(ff,"\
-----------------------------------------------------------------------------\n");
  fprintf(ff,gettext("\
  Дата    |Счёт кор|  Дебет   |  Кредит  |КТО ввел|Номер д.|Код оп.|Коментарий\n"));
  fprintf(ff,"\
-----------------------------------------------------------------------------\n");

  kolstlst++;    

  if(kolstlst > kol_strok_na_liste)
    numlist(&kolstlst,&kollist,ff);

  memset(strsql,'\0',sizeof(strsql));
  if(dpp != 0)
   {
    if(debpp > 0.009)
     sprintf(strsql,"%s:%02d.%02d.%d %s %.2f",
     gettext("Последняя проводка"),
     dpp,mpp,gpp,
     gettext("Дебет"),debpp);
    else
     sprintf(strsql,"%s:%02d.%02d.%d %s %.2f",
     gettext("Последняя проводка"),
     dpp,mpp,gpp,
     gettext("Кредит"),krepp);
    
   }   

  if(debet > kredit)
    fprintf(ff,"%*s:%10.2f %10s %s\n",
    iceb_tu_kolbait(19,gettext("Сальдо")),gettext("Сальдо"),debet-kredit," ",strsql);   
  else
    fprintf(ff,"%*s:%10s %10.2f %s\n",
    iceb_tu_kolbait(19,gettext("Сальдо")),gettext("Сальдо")," ",kredit-debet,strsql);

     
  while(cur1.read_cursor(&row1) != 0)
   {
    debet=getval.toue(row1[5],row1[0]);
    kredit=getval.toue(row1[6],row1[0]);

//    debet=atof(row1[5]);
//    kredit=atof(row1[6]);

    rsdat(&d,&m,&g,row1[0],2);

    if((nomer=SHET.find(row1[1])) != -1)
     {
      DEB.plus(debet,nomer);
      KRE.plus(kredit,nomer);
     }  
    else
     {
      SHET.plus(row1[1]);
      DEB.plus(debet,-1);
      KRE.plus(kredit,-1);
     }

    sozkom_prov(&koment,row1[8],row1[2],row1[4]);

    kolstlst++;    
    if(kolstlst > kol_strok_na_liste)
      numlist(&kolstlst,&kollist,ff);

    fprintf(ff,"%02d.%02d.%04d %-*s %10.2f %10.2f %-3s %-4s %-*s %-*s%.*s\n",
    d,m,g,
    iceb_tu_kolbait(8,row1[1]),row1[1],
    debet,kredit,row1[2],row1[7],
    iceb_tu_kolbait(8,row1[3]),row1[3],
    iceb_tu_kolbait(7,row1[4]),row1[4],
    iceb_tu_kolbait(23,koment.ravno()),koment.ravno());
 
    prdebet+=debet;
    prkredit+=kredit;

   }

  kolstlst++;    
  if(kolstlst > kol_strok_na_liste)
      numlist(&kolstlst,&kollist,ff);
  fprintf(ff,"\
-----------------------------------------------------------------------------\n");

  kolstlst++;    
  if(kolstlst > kol_strok_na_liste)
      numlist(&kolstlst,&kollist,ff);
  fprintf(ff,"%*s:%10.2f %10.2f\n",
  iceb_tu_kolbait(19,gettext("Оборот за период")),gettext("Оборот за период"),
  prdebet,prkredit);

  debet=stdebet+dodebet+prdebet;
  kredit=stkredit+dokredit+prkredit;


  kolstlst++;    
  if(kolstlst > kol_strok_na_liste)
      numlist(&kolstlst,&kollist,ff);
  fprintf(ff,"%*s:%10.2f %10.2f\n",
  iceb_tu_kolbait(19,gettext("Сальдо разв-тое")),gettext("Сальдо разв-тое"),
  debet,kredit);


  kolstlst++;    
  if(kolstlst > kol_strok_na_liste)
      numlist(&kolstlst,&kollist,ff);
  if(debet > kredit)
    fprintf(ff,"%*s:%10.2f\n",
    iceb_tu_kolbait(19,gettext("Сальдо свернутое")),gettext("Сальдо свернутое"),
    debet-kredit);
  else
    fprintf(ff,"%*s:%10s %10.2f\n",
    iceb_tu_kolbait(19,gettext("Сальдо свернутое")),gettext("Сальдо свернутое"),
    " ",
    kredit-debet);
 }
GDITE();
podpis(ff);

fclose(ff);


/*******************************/
//Распечатка свода по счетам

sprintf(imafs,"gokps%d.lst",getpid());

if((ff = fopen(imafs,"w")) == NULL)
 {
  error_op_nfil(imafs,errno,"");
  return(1);
 }


iceb_t_zagolov(gettext("Свод по счетам"),dn,mn,gn,dk,mk,gk,ff);
fprintf(ff,"%s: %s %s",gettext("Счёт"),rekrs->shet.ravno(),naimshet.ravno());
if(rekrs->kod_val.ravno()[0] != '\0')
 fprintf(ff," %s:%s",gettext("Валюта"),rekrs->kod_val.ravno());
fprintf(ff,"\n");

fprintf(ff,"\n");
fprintf(ff,"\
--------------------------------------------------------------\n");
fprintf(ff,gettext("\
  Счёт  |   Наименование счёта         |  Дебет   |  Кредит  |\n"));
fprintf(ff,"\
--------------------------------------------------------------\n");

int kolelem=SHET.kolih();
debet=kredit=0.;
for(nomer=0; nomer < kolelem; nomer++)
 {

  //Читаем наименование счета
  naimkontr.new_plus("");

  sprintf(strsql,"select nais from Plansh where ns='%s'",
  SHET.ravno(nomer));
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    naimkontr.new_plus(row1[0]);
    
  fprintf(ff,"%-*s %-*.*s %10.2f %10.2f\n",
  iceb_tu_kolbait(8,SHET.ravno(nomer)),SHET.ravno(nomer),
  iceb_tu_kolbait(30,naimkontr.ravno()),iceb_tu_kolbait(30,naimkontr.ravno()),naimkontr.ravno(),
  DEB.ravno(nomer),
  KRE.ravno(nomer));

  debet+=DEB.ravno(nomer),
  kredit+=KRE.ravno(nomer);
  
 }
fprintf(ff,"\
--------------------------------------------------------------\n");
fprintf(ff,"%*s %10.2f %10.2f\n",
iceb_tu_kolbait(39,gettext("Итого")),gettext("Итого"),debet,kredit);

podpis(ff);
fclose(ff);

printw_vr(tmm);

printw("%s:%d\n",gettext("Количество листов"),kollist);
OSTANOV();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Распечатка проводок по контрагентам"));
oth->spis_imaf.plus(imafs);
oth->spis_naim.plus(gettext("Свод по счетам"));

for(int nomer=0; nomer < oth->spis_imaf.kolih(); nomer++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nomer),1);
 
return(0);
}
/**********************************/
void prosmotrdo(short dn,short mn,short gn,const char *shet,const char *kontr,
short vds, //0-счет 1-субсчет
double *dodebet,double *dokredit,
short *dpp,short *mpp,short *gpp,//Дата последней проводки
double *debpp,double *krepp,short godn,
const char *kod_val)
{
short	d,m,g;
char	strsql[512];
SQL_str	row1;
class iceb_getkue_dat getval(kod_val);
*dpp=0;
*mpp=0;
*gpp=0;
*debpp=0.;
*krepp=0.;

d=1;
m=1;
g=godn;

SQLCURSOR cur1;

//Смотрим проводки до периода
for(;sravmydat(d,m,g,dn,mn,gn) < 0;)
 {
//  printw("%02d.%02d.%d\n",d,m,g);
//  refresh();
  /*Отчет для многопорядкового плана счетов*/
  if(vplsh == 1)
  sprintf(strsql,"select datp,deb,kre from Prov where val=0 and \
datp = '%04d-%d-%d' and sh like '%s%%' and kodkon='%s' order by datp asc",
  g,m,d,shet,kontr);

  /*Отчет по субсчету двух-порядковый план счетов*/
  if(vds == 1 && vplsh == 0)
    sprintf(strsql,"select datp,deb,kre from Prov where val=0 and \
datp = '%04d-%d-%d' and sh='%s' and kodkon='%s' order by datp asc",
    g,m,d,shet,kontr);

  /*Отчет по счёту двух-порядковый план счетов*/
  if(vds == 0 && vplsh == 0)
    sprintf(strsql,"select datp,deb,kre from Prov where val=0 and \
datp = '%04d-%d-%d' and sh like '%s%%' and kodkon='%s' order by datp asc",
    g,m,d,shet,kontr);

  if(cur1.make_cursor(&bd,strsql) < 0)
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

  while(cur1.read_cursor(&row1) != 0)
   {
    *dodebet+=getval.toue(row1[1],row1[0]);
    *dokredit+=getval.toue(row1[2],row1[0]);
/***********
    *dodebet+=atof(row1[0]);
    *dokredit+=atof(row1[1]);
***************/
    *dpp=d; *mpp=m; *gpp=g;

    *debpp=atof(row1[0]);
    *krepp=atof(row1[1]);
   }
  dpm(&d,&m,&g,1);  
 }

}

/**************************/
/*Печать нумерации листов */
/**************************/

void numlist(int *kolstrlist,int *kollist,FILE *ff)
{

if(*kollist > 0 )
 {
  fprintf(ff,"\f");
  *kolstrlist=0;
 }

*kollist+=1;
*kolstrlist+=2;
fprintf(ff,"%60s%s N%d\n","",gettext("Страница"),*kollist);

}
