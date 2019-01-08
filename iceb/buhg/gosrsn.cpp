/*$Id: gosrs.c,v 5.17 2013/09/26 09:43:33 sasa Exp $*/
/*31.07.2017	28.08.2009	Белых А.И.	gosrsn.c
Расчёт журнала-ордера для счёта с развёрнутым сальдо
Новый алгоритм
*/
#include <math.h>
#include <errno.h>
#include "buhg.h"

void gosrsn_hapgo(short dn,short mn,short gn,short dk,short mk,short gk,int *kollist,int *kolstr,FILE *ff);
void gosrsn_ss(int metka,short dn,short mn,short gn,short dk,short mk,short gk,int *kollist,int *kolstr,FILE *ff);
void gosrsn_itoggo(const char *shet,short dn,short mn,short gn,short dk,short mk,short gk,const char *naim_shet,class SPISOK *oss,const char *imatmptab_hax,FILE *ff);
void gossrsn_sapsk(short dk,short gk,short mk,int *kollist_sk,int *kolstr_sk,FILE *ff);
int gorsrs_provk(const char *kod_kontr,double debs,double kres,double debp,double krep,const char *imatabl);


void gorsrsn_podn(const char *shet,const char *naim_shet,short dn,short mn,short gn,short dk,short mk,short gk,class SPISOK *ssd,class SPISOK *ssk,double debs,double kres,double debp,double krep,double debk,double krek,int metka_f,const char *kod_val,const char *imatab,char *imaf);

int gosrs_zvt(int per,const char *kod_kontr,double deb,double kre,const char *imatabl);
int gosrs_zvt_den(short d,short m,short g,const char *shet,double deb,double kre,const char *imatabl);
int gosrs_zvt_hax(const char *kod_kontr,const char *shet,double deb,double kre,const char *imatabl);
int gosrsn_ps(const char *imatmptab,const char *str_sort);

extern short	startgodb; /*Стартовый год*/
extern short	vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/
extern int kol_strok_na_liste;


int gosrsn(class guror_rr *rekrs,
int metkasort,
class spis_oth *oth)
{
char str_sort[1024];
short dn=0,mn=0,gn=0;
short dk=0,mk=0,gk=0;
char strsql[1024];
int kolstr=0;
double deb=0.,kre=0.;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
class SPISOK ssd; /*Список счетов дебета*/
class SPISOK ssk; /*Список счетов кредита*/
class SPISOK oss; /*Общий список счетов*/



class masiv_din_double hach_kontr_shet_deb; /*шахматка контрагент-счёт по дебету*/
class masiv_din_double hach_kontr_shet_kre; /*шахматка контрагент-счёт по кредиту*/


class iceb_getkue_dat getval(rekrs->kod_val.ravno());
time_t tmmn=time(NULL);
short godn=startgodb;

if(rsdatp(&dn,&mn,&gn,rekrs->dat1.ravno(),&dk,&mk,&gk,rekrs->dat2.ravno()) != 0)
  return(1);

if(startgodb == 0 || startgodb > gn)
 godn=gn;

clear();
int vds=0;
/*Узнаём наименование счёта*/
class iceb_tu_str naim_shet("");
sprintf(strsql,"select nais,vids from Plansh where ns='%s'",rekrs->shet.ravno());
if(readkey(strsql,&row,&cur) == 1)
 {
  naim_shet.new_plus(row[0]);
  vds=atoi(row[1]);
 }
printw("%s %s\n",rekrs->shet.ravno(),naim_shet.ravno());
printw("%02d.%02d.%d -> %02d.%02d.%d\n",dn,mn,gn,dk,mk,gk);
if(rekrs->kod_val.ravno()[0] != '\0')
 printw("%s:%s\n",gettext("Код валюты"),rekrs->kod_val.ravno());
 



class iceb_t_tmptab tabtmp;
const char *imatmptab={"gosrs"};
char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
kod_kontr char(32) not null DEFAULT '',\
debs double(16,2) not null DEFAULT 0.,\
kres double(16,2) not null DEFAULT 0.,\
debp double(16,2) not null DEFAULT 0.,\
krep double(16,2) not null DEFAULT 0.,\
kod_kontr_int INT NOT NULL DEFAULT 0,\
nm CHAR(255) NOT NULL DEFAULT '',\
gr INT NOT NULL DEFAULT 0,\
ns INT NOT NULL DEFAULT -1,\
index(nm),\
index(gr),\
unique(kod_kontr)) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  





/*Узнаем начальное сальдо по каждой организации*/
printw("%s\n",gettext("Вычисляем стартовое сальдо по всем контрагентам"));
refresh();

if(vds == 0 || vplsh == 1)
 sprintf(strsql,"select kodkon,deb,kre from Saldo where kkk='1' and gs=%d \
and ns like '%s%%'",godn,rekrs->shet.ravno());

if(vds == 1 && vplsh == 0)
 sprintf(strsql,"select kodkon,deb,kre from Saldo where kkk='1' and gs=%d and ns='%s'",godn,rekrs->shet.ravno());

//printw("%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
sprintf(strsql,"%04d-01-01",godn);

class iceb_tu_str data_saldo(strsql);

int nomer_kontr=0;

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  if(rekrs->mso.ravno()[0] != '\0')
   {
    sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[0]);
    if(readkey(strsql,&row1,&cur1) != 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),row[0]);
      iceb_t_soob(strsql);
      continue;
     }
    if(proverka(rekrs->mso.ravno(),row1[0],0,0) != 0)
      continue;  
   }

  deb=getval.toue(row[1],data_saldo.ravno());
  kre=getval.toue(row[2],data_saldo.ravno());
  

 
  gosrs_zvt(0,row[0],deb,kre,imatmptab); /*запись в таблицу*/

 }


class iceb_t_tmptab tabtmp_den;
const char *imatmptab_den={"gosrsden"};
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
data DATE not null DEFAULT '0000-00-00',\
shet char(64) NOT NULL DEFAULT '',\
deb double(16,2) not null DEFAULT 0.,\
kre double(16,2) not null DEFAULT 0.,\
unique(data,shet)) ENGINE = MYISAM",imatmptab_den);


if(tabtmp_den.create_tab(imatmptab_den,zaprostmp) != 0)
 {
  return(1);
 }  

class iceb_t_tmptab tabtmphax;
const char *imatmptab_hax={"gosrshax"};
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
kod_kontr char(64) not null DEFAULT '',\
shet char(64) NOT NULL DEFAULT '',\
deb double(16,2) not null DEFAULT 0.,\
kre double(16,2) not null DEFAULT 0.,\
unique(kod_kontr,shet)) ENGINE = MYISAM",imatmptab_hax);


if(tabtmphax.create_tab(imatmptab_hax,zaprostmp) != 0)
 {
  return(1);
 }  


/*смотрим проводки заполняем массивы*/
int nomer_mes_v_per=-1;
//int nomer_den_shet=0;
//int nomer_kontr_shet=0;
short d=1,m=1,g=godn;
int kolih_den=period(1,1,godn,dk,mk,gk,0);
int kolstr1=0;
short mes_zap=0;

while(sravmydat(d,m,g,dk,mk,gk) <= 0)
 {
  
  strzag(LINES-1,0,kolih_den,++kolstr1);

  if(vds == 0 || vplsh == 1)
   sprintf(strsql,"select sh,shk,kodkon,deb,kre,datp from Prov \
 where val=0 and datp='%d-%02d-%02d' and sh like '%s%%'",g,m,d,rekrs->shet.ravno());

  if(vds == 1 && vplsh == 0)
   sprintf(strsql,"select sh,shk,kodkon,deb,kre,datp from Prov \
 where val=0 and datp='%d-%02d-%02d' and sh='%s'",g,m,d,rekrs->shet.ravno());

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return(1);
   }

  if(sravmydat(d,m,g,dn,mn,gn) >= 0) /*период*/
  if(mes_zap != m)
   {
    nomer_mes_v_per++;
    mes_zap=m;
   }            
  
  while(cur.read_cursor(&row) != 0)
   {
//    printw("%02d.%02d.%04d|%6s|%6s|%6s|%10s|%10s|\n",d,m,g,row[0],row[1],row[2],row[3],row[4]);
//    refresh();
    
    if(row[2] == NULL || row[2][0] == '\0')
     {
      VVOD SOOB(1);
      sprintf(strsql,"%d.%d.%d|%6s|%6s|%6s|%10s|%10s|",d,m,g,row[0],row[1],row[2],row[3],row[4]);
      SOOB.VVOD_SPISOK_add_MD(strsql);
      SOOB.VVOD_SPISOK_add_MD(gettext("В проводке нет кода контрагента !"));
      soobshw(&SOOB,stdscr,-1,-1,0,1);

      continue;
     }

    if(rekrs->mso.ravno()[0] != '\0')
     {
      sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[2]);
      if(readkey(strsql,&row1,&cur1) != 1)
       {
        sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),row[2]);
        iceb_t_soob(strsql);
        continue;
       }
      if(proverka(rekrs->mso.ravno(),row1[0],0,0) != 0)
        continue;  
     }



    deb=getval.toue(row[3],row[5]);
    kre=getval.toue(row[4],row[5]);

    if(sravmydat(d,m,g,dn,mn,gn) < 0) /*До периода*/
     {
      gosrs_zvt(0,row[2],deb,kre,imatmptab); /*запись в таблицу*/
     }
    else /*Период*/
     {
      if(oss.find_r(row[1]) < 0)
         oss.plus(row[1]);

      gosrs_zvt(1,row[2],deb,kre,imatmptab); /*запись в таблицу*/
/***********
      sprintf(strsql,"%s|%s",row[2],row[1]);
      if((nomer_kontr_shet=kontr_shet.find_r(strsql)) < 0)
       kontr_shet.plus(strsql);

      //Всегда дебет и кредит независимо от их значений*
      kontr_shet_deb.plus(deb,nomer_kontr_shet);
      kontr_shet_kre.plus(kre,nomer_kontr_shet);
*******************/
      gosrs_zvt_hax(row[2],row[1],deb,kre,imatmptab_hax);

      if(fabs(deb) > 0.009)
       {
        if(ssd.find_r(row[1]) < 0)
         ssd.plus(row[1]);
       }
      else
       {
        if(ssk.find_r(row[1]) < 0)
          ssk.plus(row[1]);
       }
      gosrs_zvt_den(d,m,g,row[1],deb,kre,imatmptab_den);
     }
   }

  dpm(&d,&m,&g,1);
 }

GDITE();

printw("%s\n",gettext("Определяем обороты по счетам корреспондентам"));
refresh();



if(metkasort == 0) /*по коду контрагента в символьном виде*/
 sprintf(str_sort,"order by kod_kontr asc");

if(metkasort == 1) /*по коду контрагента в цифровом виде*/
 sprintf(str_sort,"order by kod_kontr_int asc");

if(metkasort == 2) /*по наименованию*/
 sprintf(str_sort,"order by nm asc");

if(metkasort == 3) /*по группе контрагента*/
 sprintf(str_sort,"order by gr asc,nm asc");

sprintf(strsql,"select kod_kontr from %s where debp <> 0. or krep <> 0.%s",imatmptab,str_sort);
int kolih_kontr=readkey(strsql);

int kolih_shet=oss.kolih();
int nomer_sheta=0;
//class iceb_tu_str kod_kontr0("");
//class iceb_tu_str shet_k("");
/*заряжаем шахматки*/
hach_kontr_shet_deb.make_class(kolih_kontr*kolih_shet);
hach_kontr_shet_kre.make_class(kolih_kontr*kolih_shet);
/*******************
for(int nomer_kontr_shet=0; nomer_kontr_shet < kontr_shet.kolih(); nomer_kontr_shet++)
 {
  polen(kontr_shet.ravno(nomer_kontr_shet),&kod_kontr0,1,'|');  
  polen(kontr_shet.ravno(nomer_kontr_shet),&shet_k,2,'|');  

  nomer_kontr=gosrsn_pns(kod_kontr0.ravno(),str_sort);

  nomer_sheta=oss.find_r(shet_k.ravno());

  hach_kontr_shet_deb.plus(kontr_shet_deb.ravno(nomer_kontr_shet),nomer_kontr*kolih_shet+nomer_sheta);

  hach_kontr_shet_kre.plus(kontr_shet_kre.ravno(nomer_kontr_shet),nomer_kontr*kolih_shet+nomer_sheta);
 }
******************/

gosrsn_ps(imatmptab,str_sort); /*пронумеровываем строки*/

sprintf(strsql,"select * from %s order by kod_kontr asc",imatmptab_hax);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  deb=atof(row[2]);
  kre=atof(row[3]);  
  
  sprintf(strsql,"select ns from %s where kod_kontr='%s'",imatmptab,row[0]);
  if(readkey(strsql,&row1,&cur1) != 1)
   {

    printw("%s\n",strsql);   
    iceb_t_soob(gettext("Не найден номер строки"));

    continue;
   }
  nomer_kontr=atoi(row1[0]);

  nomer_sheta=oss.find_r(row[1]);

  hach_kontr_shet_deb.plus(deb,nomer_kontr*kolih_shet+nomer_sheta);

  hach_kontr_shet_kre.plus(kre,nomer_kontr*kolih_shet+nomer_sheta);
 }
 
printw("%s\n",gettext("Вертикальна форма распечатки по контрагентам"));
refresh();


int metka_naim_shet=0;
memset(strsql,'\0',sizeof(strsql));
if(iceb_t_poldan("Печатать наименование счёта корреспондента в журналах-ордерах для счетов с развёрнутым сальдо",strsql,"nastrb.alx") == 0)
 if(SRAV("Вкл",strsql,1) == 0)
  metka_naim_shet=1;

if(metka_naim_shet == 1)
 printw("%s\n",gettext("Печатаем с наименованиями счетов кореспондентов"));
else 
 printw("%s\n",gettext("Печатаем без наименований счетов кореспондентов"));

char imaf_obor[64];
char imaf_gur[64];
char imaf_sk[64];

sprintf(imaf_obor,"obor%d.lst",getpid());
sprintf(imaf_gur,"gurrs%d.lst",getpid());
sprintf(imaf_sk,"salk%d.lst",getpid());

class iceb_fopen ff_obor;
if(ff_obor.start(imaf_obor,"w") != 0)
 return(1);

class iceb_fopen ff_gur;
if(ff_gur.start(imaf_gur,"w") != 0)
 return(1);
 
class iceb_fopen ff_sk;
if(ff_sk.start(imaf_sk,"w") != 0)
 return(1);
 
struct  tm      *bf;
time_t tmm=time(NULL);

bf=localtime(&tmm);
fprintf(ff_obor.ff,"%s\n\n\
			      %s %s %s\n\
				  %s %d.%d.%d%s %s %d.%d.%d%s",
iceb_t_get_pnk("00",0),
gettext("Оборот по счёту"),
rekrs->shet.ravno(),naim_shet.ravno(),
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."));

if(rekrs->kod_val.ravno()[0] != '\0')
 fprintf(ff_obor.ff," %s:%s",gettext("Валюта"),rekrs->kod_val.ravno());

fprintf(ff_obor.ff,"\n\
			      %s %d.%d.%d%s  %s: %d.%d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

int kolstr_obor=5; 

fprintf(ff_gur.ff,"%s\n\n\
			       %s %s %s\n\
				  %s %d.%d.%d%s %s %d.%d.%d%s",
iceb_t_get_pnk("00",0),
gettext("Журнал ордер по счёту"),
rekrs->shet.ravno(),naim_shet.ravno(),
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."));


if(rekrs->kod_val.ravno()[0] != '\0')
 fprintf(ff_gur.ff," %s:%s",gettext("Валюта"),rekrs->kod_val.ravno());
 
fprintf(ff_gur.ff,"\n\
			       %s %d.%d.%d  %s - %d:%d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("Время"),
bf->tm_hour,bf->tm_min);

int kolstr_gur=5;

fprintf(ff_sk.ff,"%s\n\n\
%s %d.%d.%d%s %s %s %s",
iceb_t_get_pnk("00",0),
gettext("Конечное сальдо на"),
dk,mk,gk,
gettext("г."),
gettext("по счету"),
rekrs->shet.ravno(),naim_shet.ravno());

if(rekrs->kod_val.ravno()[0] != '\0')
 fprintf(ff_sk.ff," %s:%s",gettext("Валюта"),rekrs->kod_val.ravno());
 
fprintf(ff_sk.ff,"\n\
%s %d.%d.%d%s  %s: %d.%d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

int kolstr_sk=4;

int klst=1;
if(rekrs->mso.ravno()[0] != '\0')
 {
  fprintf(ff_obor.ff,"%s: %s\n",gettext("Список контрагентов"),rekrs->mso.ravno());
  printcod(ff_obor.ff,"Gkont","kod","naik",0,rekrs->mso.ravno(),&klst);

  klst=0;
  fprintf(ff_gur.ff,"%s: %s\n",gettext("Список контрагентов"),rekrs->mso.ravno());
  printcod(ff_gur.ff,"Gkont","kod","naik",0,rekrs->mso.ravno(),&klst);

  klst=0;
  fprintf(ff_sk.ff,"%s: %s\n",gettext("Список контрагентов"),rekrs->mso.ravno());
  printcod(ff_sk.ff,"Gkont","kod","naik",0,rekrs->mso.ravno(),&klst);
 }

kolstr_gur+=klst;
kolstr_obor+=klst;
kolstr_sk+=klst;
int kollist_gur=0;
int kollist_obor=0;
int kollist_sk=0;
/*Шапка оборотного баланса*/
sapgors(dn,mn,gn,dk,mk,gk,&kollist_obor,&kolstr_obor,ff_obor.ff);
/*Шапка журнал-ордера*/
gosrsn_hapgo(dn,mn,gn,dk,mk,gk,&kollist_gur,&kolstr_gur,ff_gur.ff);
/*Шапка конечного сальдо*/
gossrsn_sapsk(dk,gk,mk,&kollist_sk,&kolstr_sk,ff_sk.ff);

//int metka_prop=0;
class iceb_tu_str kod_kontr;
class iceb_tu_str naim_kontr;
class iceb_tu_str telef;
int grupa=0;
int grupaz=0;
double   sdgr=0.,skgr=0.,sdngr=0.,skngr=0.,dnsgr=0.,knsgr=0.;
double idebs=0.,ikres=0.;
double idebp=0.,ikrep=0.;
double idebk=0.,ikrek=0.;

printw("%s=%d\n",gettext("Количество контрагентов за период"),kolih_kontr);
refresh();

sprintf(strsql,"select * from %s %s",imatmptab,str_sort);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

printw("%s=%d\n",gettext("Общее количество контрагентов"),kolstr);
refresh();

double debs=0.,kres=0.,debp=0.,krep=0.;
nomer_kontr=0;
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%6s %10s %10s %10s %10s\n",row[0],row[1],row[2],row[3],row[4]);  
//  OSTANOV();
  nomer_kontr++;

    
  strzag(LINES-1,0,kolstr,++kolstr1);
  
  debs=atof(row[1]);
  kres=atof(row[2]);
  debp=atof(row[3]);
  krep=atof(row[4]);
  
  if(gorsrs_provk(row[0],debs,kres,debp,krep,imatmptab_hax) != 0)
   continue;

  sprintf(strsql,"select kodkon,naikon,telef,grup from Kontragent where kodkon='%s'",row[0]);

  if(readkey(strsql,&row1,&cur1) != 1)
   {
    fprintf(ff_obor.ff,"\n\n%s %s!!!\n\n",gettext("Не найден код контрагента"),row[0]);
    fprintf(ff_gur.ff,"\n\n%s %s!!!\n\n",gettext("Не найден код контрагента"),row[0]);
    sprintf(strsql,"%s %s",gettext("Не найден код контрагента"),row[0]);
    iceb_t_soob(strsql);
    continue;
   }
  naim_kontr.new_plus(row1[1]);
  telef.new_plus(row1[2]);
  grupa=atoi(row1[3]); 

  if(metkasort == 3)
   if(grupaz != grupa)
    {
     if(grupaz != 0)
      {
     gosrsn_ss(0,dn,mn,gn,dk,mk,gk,&kollist_obor,&kolstr_obor,ff_obor.ff);
     fprintf(ff_obor.ff,"\
 . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .\n");
     class iceb_tu_str naimgrup("");       
     sprintf(strsql,"select naik from Gkont where kod=%d",grupaz);
     if(readkey(strsql,&row1,&cur1) == 1)
         naimgrup.new_plus(row1[0]);

     gosrsn_ss(0,dn,mn,gn,dk,mk,gk,&kollist_obor,&kolstr_obor,ff_obor.ff);

     sprintf(strsql,"%s %d %s",gettext(" Итого по группе"),grupaz,naimgrup.ravno());
     fprintf(ff_obor.ff,"\
%-*.*s %14.2f %14.2f %14.2f %14.2f %14.2f %14.2f\n",
     iceb_tu_kolbait(31,strsql),iceb_tu_kolbait(31,strsql),strsql,dnsgr,knsgr,sdngr,skngr,sdgr,skgr);

     /*****************************************/

     gosrsn_ss(1,dn,mn,gn,dk,mk,gk,&kollist_gur,&kolstr_gur,ff_gur.ff);
     fprintf(ff_gur.ff,"\
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");


     sprintf(strsql,"%s %d %s",gettext(" Итого по группе"),grupaz,naimgrup.ravno());

     gosrsn_ss(1,dn,mn,gn,dk,mk,gk,&kollist_gur,&kolstr_gur,ff_gur.ff);
     fprintf(ff_gur.ff,"\
%-*.*s %14.2f %14.2f %10s %14.2f %14.2f %14.2f %14.2f\n",
     iceb_tu_kolbait(31,strsql),iceb_tu_kolbait(31,strsql),strsql,dnsgr,knsgr," ",sdngr,skngr,sdgr,skgr);
      }

     sdgr=skgr=sdngr=skngr=dnsgr=knsgr=0.;
     grupaz=grupa;
    }

  deb=kre=0.;
/*************
  if(debs.ravno(nomer_kontr) > kres.ravno(nomer_kontr))
   deb=debs.ravno(nomer_kontr)-kres.ravno(nomer_kontr);
  else
   kre=kres.ravno(nomer_kontr)-debs.ravno(nomer_kontr);
****************/
  if(debs > kres)
   deb=debs-kres;
  else
   kre=kres-debs;

  gosrsn_ss(0,dn,mn,gn,dk,mk,gk,&kollist_obor,&kolstr_obor,ff_obor.ff);
  gosrsn_ss(1,dn,mn,gn,dk,mk,gk,&kollist_gur,&kolstr_gur,ff_gur.ff);
  gosrsn_ss(2,dn,mn,gn,dk,mk,gk,&kollist_sk,&kolstr_sk,ff_sk.ff);
  if(deb > 0.)
   {
    idebs+=deb;
    dnsgr+=deb;
    fprintf(ff_obor.ff,"%*s %-*.*s %14.2f %14s %14.2f %14.2f ",
    iceb_tu_kolbait(10,row[0]),row[0],
    iceb_tu_kolbait(20,naim_kontr.ravno()),iceb_tu_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
    deb," ",debp,krep);

    fprintf(ff_gur.ff,"%*s %-*.*s %14.2f %14s %10s %14.2f %14.2f ",
    iceb_tu_kolbait(10,row[0]),row[0],
    iceb_tu_kolbait(20,naim_kontr.ravno()),iceb_tu_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
    deb," "," ",debp,krep);
   }
  else
   {
    ikres+=kre;
    knsgr+=kre;
    fprintf(ff_obor.ff,"%*s %-*.*s %14s %14.2f %14.2f %14.2f ",
    iceb_tu_kolbait(10,row[0]),row[0],
    iceb_tu_kolbait(20,naim_kontr.ravno()),iceb_tu_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
    " ",kre,debp,krep);

    fprintf(ff_gur.ff,"%*s %-*.*s %14s %14.2f %10s %14.2f %14.2f ",
    iceb_tu_kolbait(10,row[0]),row[0],
    iceb_tu_kolbait(20,naim_kontr.ravno()),iceb_tu_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
    " ",kre," ",debp,krep);
   }


  idebp+=debp;
  ikrep+=krep;

  deb=kre=0.;
  if(debs+debp > kres+krep)
   deb=debs+debp - (kres+krep);
  else
   kre=kres+krep-(debs+debp);

  if(deb > 0.)
   {
    idebk+=deb;
    sdgr+=deb;
    fprintf(ff_obor.ff,"%14.2f\n",deb);
    fprintf(ff_gur.ff,"%14.2f\n",deb);

    fprintf(ff_sk.ff,"%*s %-*.*s %14.2f %14s %s\n",
    iceb_tu_kolbait(10,row[0]),row[0],
    iceb_tu_kolbait(20,naim_kontr.ravno()),iceb_tu_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
    deb," ",telef.ravno());

   }
  else
   {
    fprintf(ff_obor.ff,"%14s %14.2f\n"," ",kre);
    fprintf(ff_gur.ff,"%14s %14.2f\n"," ",kre);
    fprintf(ff_sk.ff,"%*s %-*.*s %14s %14.2f %s\n",
    iceb_tu_kolbait(10,row[0]),row[0],
    iceb_tu_kolbait(20,naim_kontr.ravno()),iceb_tu_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
    " ",kre,telef.ravno());
    ikrek+=kre;
    skgr+=kre;
   }

  if(iceb_tu_strlen(naim_kontr.ravno()) > 20)
   {
    gosrsn_ss(0,dn,mn,gn,dk,mk,gk,&kollist_obor,&kolstr_obor,ff_obor.ff);
    fprintf(ff_obor.ff,"%-10s %.*s\n",
    " ",
    iceb_tu_kolbait(110,iceb_tu_adrsimv(20,naim_kontr.ravno())),
    iceb_tu_adrsimv(20,naim_kontr.ravno()));

    gosrsn_ss(1,dn,mn,gn,dk,mk,gk,&kollist_gur,&kolstr_gur,ff_gur.ff);
    fprintf(ff_gur.ff,"%-10s %.*s\n",
    " ",
    iceb_tu_kolbait(120,iceb_tu_adrsimv(20,naim_kontr.ravno())),
    iceb_tu_adrsimv(20,naim_kontr.ravno()));

    gosrsn_ss(2,dn,mn,gn,dk,mk,gk,&kollist_sk,&kolstr_sk,ff_sk.ff);
    fprintf(ff_sk.ff,"%-10s %.*s\n",
    " ",
    iceb_tu_kolbait(70,iceb_tu_adrsimv(20,naim_kontr.ravno())),
    iceb_tu_adrsimv(20,naim_kontr.ravno()));
   }

 for(int nomer_shet=0; nomer_shet < kolih_shet; nomer_shet++)
  {
//   deb=hach_kontr_shet_deb.ravno((nomer_kontr-1)*kolih_shet+nomer_shet);
//   kre=hach_kontr_shet_kre.ravno((nomer_kontr-1)*kolih_shet+nomer_shet);
   int nom_kontr_zp=atoi(row[8]);
   if(nom_kontr_zp == -1)
     break;
   deb=hach_kontr_shet_deb.ravno((nom_kontr_zp)*kolih_shet+nomer_shet);
   kre=hach_kontr_shet_kre.ravno((nom_kontr_zp)*kolih_shet+nomer_shet);

   if(fabs(deb) < 0.009 && fabs(kre) < 0.009)
    continue;
   if(metka_naim_shet == 1)
     {
      class iceb_tu_str naim_shet_kor("");
      sprintf(strsql,"select nais from Plansh where ns='%s'",oss.ravno(nomer_shet));
      if(readkey(strsql,&row1,&cur1) == 1)
         naim_shet_kor.new_plus(row1[0]);
       
      int razmer_naim=71-iceb_tu_strlen(oss.ravno(nomer_shet));
      sprintf(strsql,"%.*s %s",
      iceb_tu_kolbait(razmer_naim,naim_shet_kor.ravno()),
      naim_shet_kor.ravno(),
      oss.ravno(nomer_shet));
     }
   else
      sprintf(strsql,"%s",oss.ravno(nomer_shet));
      
    gosrsn_ss(1,dn,mn,gn,dk,mk,gk,&kollist_gur,&kolstr_gur,ff_gur.ff);

    fprintf(ff_gur.ff,"%*.*s %14.2f %14.2f\n",
    iceb_tu_kolbait(72,strsql),iceb_tu_kolbait(72,strsql),strsql,deb,kre);


  }

  gosrsn_ss(1,dn,mn,gn,dk,mk,gk,&kollist_gur,&kolstr_gur,ff_gur.ff);
  fprintf(ff_gur.ff,"\
.....................................................................................................................................\n");

  sdngr+=debp;
  skngr+=krep;
 }

if(metkasort == 3)
  {
     gosrsn_ss(0,dn,mn,gn,dk,mk,gk,&kollist_obor,&kolstr_obor,ff_obor.ff);
     fprintf(ff_obor.ff,"\
 . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .\n");
     class iceb_tu_str naimgrup("");       
     sprintf(strsql,"select naik from Gkont where kod=%d",grupa);
     if(readkey(strsql,&row1,&cur1) == 1)
         naimgrup.new_plus(row1[0]);

     gosrsn_ss(0,dn,mn,gn,dk,mk,gk,&kollist_obor,&kolstr_obor,ff_obor.ff);

     sprintf(strsql,"%s %d %s",gettext(" Итого по группе"),grupa,naimgrup.ravno());
     fprintf(ff_obor.ff,"\
%-*.*s %14.2f %14.2f %14.2f %14.2f %14.2f %14.2f\n",
     iceb_tu_kolbait(31,strsql),iceb_tu_kolbait(31,strsql),strsql,
     dnsgr,knsgr,sdngr,skngr,sdgr,skgr);

     /*****************************************/

     gosrsn_ss(1,dn,mn,gn,dk,mk,gk,&kollist_gur,&kolstr_gur,ff_gur.ff);
     fprintf(ff_gur.ff,"\
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");


     sprintf(strsql,"%s %d %s",gettext(" Итого по группе"),grupa,naimgrup.ravno());

     gosrsn_ss(1,dn,mn,gn,dk,mk,gk,&kollist_gur,&kolstr_gur,ff_gur.ff);
     fprintf(ff_gur.ff,"\
%-*.*s %14.2f %14.2f %10s %14.2f %14.2f %14.2f %14.2f\n",
     iceb_tu_kolbait(31,strsql),iceb_tu_kolbait(31,strsql),strsql,
     dnsgr,knsgr," ",sdngr,skngr,sdgr,skgr);

  }

fprintf(ff_obor.ff,"\
--------------------------------------------------------------------------------------------------------------------------\n\
%-*.*s %14.2f %14.2f %14.2f %14.2f %14.2f %14.2f\n",
iceb_tu_kolbait(31,gettext(" И т о г о")),iceb_tu_kolbait(31,gettext(" И т о г о")),gettext(" И т о г о"),idebs,ikres,idebp,ikrep,idebk,ikrek);

fprintf(ff_obor.ff,"\n%s__________________________\n",
gettext("Главный бухгалтер"));

podpis(ff_obor.ff);
ff_obor.end();


fprintf(ff_gur.ff,"\
-------------------------------------------------------------------------------------------------------------------------------------\n\
%-*.*s %14.2f %14.2f %10s %14.2f %14.2f %14.2f %14.2f\n",
iceb_tu_kolbait(31,gettext(" И т о г о")),iceb_tu_kolbait(31,gettext(" И т о г о")),gettext(" И т о г о"),idebs,ikres," ",idebp,ikrep,idebk,ikrek);

gosrsn_itoggo(rekrs->shet.ravno(),dn,mn,gn,dk,mk,gk,naim_shet.ravno(),&oss,imatmptab_hax,ff_gur.ff);

fprintf(ff_gur.ff,"\n%s__________________________\n",
gettext("Главный бухгалтер"));
podpis(ff_gur.ff);

ff_gur.end();

fprintf(ff_sk.ff,"\
----------------------------------------------------------------------------------\n");
fprintf(ff_sk.ff,"%-*.*s %14.2f %14.2f\n",
iceb_tu_kolbait(31,gettext(" И т о г о")),iceb_tu_kolbait(31,gettext(" И т о г о")),gettext(" И т о г о"),idebk,ikrek);

fprintf(ff_sk.ff,"\n%s__________________________\n",
gettext("Главный бухгалтер"));
podpis(ff_sk.ff);
ff_sk.end();



/*Распечатка по дням*/
printw("%s\n",gettext("Распечатка по дням в формате А4"));
refresh();

/*распечатка в формате А4*/
char imaf_dn[64];
sprintf(imaf_dn,"gopd%d.lst",getpid());
gorsrsn_podn(rekrs->shet.ravno(),naim_shet.ravno(),dn,mn,gn,dk,mk,gk,&ssd,&ssk,idebs,ikres,idebp,ikrep,idebk,ikrek,0,rekrs->kod_val.ravno(),imatmptab_den,imaf_dn);

printw("%s\n",gettext("Распечатка по дням в формате А3"));
refresh();
/*распечатка в формате А3*/
char imaf_dn_a3[64];
sprintf(imaf_dn_a3,"gopda%d.lst",getpid());
gorsrsn_podn(rekrs->shet.ravno(),naim_shet.ravno(),dn,mn,gn,dk,mk,gk,&ssd,&ssk,idebs,ikres,idebp,ikrep,idebk,ikrek,1,rekrs->kod_val.ravno(),imatmptab_den,imaf_dn_a3);

printw("%29s %*s %*s\n","",
iceb_tu_kolbait(20,gettext("Дебет")),
gettext("Дебет"),
iceb_tu_kolbait(20,gettext("Кредит")),
gettext("Кредит"));

printw("%*s %02d.%02d.%04d:%20s",iceb_tu_kolbait(18,gettext("Сальдо на")),gettext("Сальдо на"),dn,mn,gn,prnbr(idebs));

printw(" %20s\n",prnbr(ikres));

printw("%*s:%20s",iceb_tu_kolbait(29,gettext("Оборот за период")),gettext("Оборот за период"),prnbr(idebp));

printw(" %20s\n",prnbr(ikrep));

printw("%*s %02d.%02d.%04d:%20s",iceb_tu_kolbait(18,gettext("Сальдо на")),gettext("Сальдо на"),dk,mk,gk,prnbr(idebk));

printw(" %20s\n",prnbr(ikrek));

if(idebk > ikrek)
  printw("%*s:%20s\n",iceb_tu_kolbait(29,gettext("Сальдо свёрнутое")),gettext("Сальдо свёрнутое"),prnbr(idebk-ikrek));
else
  printw("%*s:%20s %20s\n",iceb_tu_kolbait(29,gettext("Сальдо свёрнутое")),gettext("Сальдо свёрнутое"),"",prnbr(ikrek-idebk));

printw_vr(tmmn);
OSTANOV();

oth->spis_imaf.plus(imaf_obor);

sprintf(strsql,"%s %s",gettext("Оборотный баланс по счёту"),rekrs->shet.ravno());
oth->spis_naim.plus(strsql);

oth->spis_imaf.plus(imaf_gur);
sprintf(strsql,"%s %s",gettext("Журнал-ордер по счёту"),rekrs->shet.ravno());
oth->spis_naim.plus(strsql);

oth->spis_imaf.plus(imaf_sk);
sprintf(strsql,"%s %s",gettext("Конечное сальдо по счёту"),rekrs->shet.ravno());
oth->spis_naim.plus(strsql);

oth->spis_imaf.plus(imaf_dn);
oth->spis_naim.plus(gettext("Распечатка по дням A4"));

oth->spis_imaf.plus(imaf_dn_a3);
oth->spis_naim.plus(gettext("Распечатка по дням A3"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),1);

return(0);
}
/****************************/
/*Шапка журнала ордера*/
/*****************************/
void gosrsn_hapgo(short dn,short mn,short gn,
short dk,short mk,short gk,
int *kollist,
int *kolstr,
FILE *ff)
{
*kollist+=1;
*kolstr+=6;
fprintf(ff,"%*s%d\n",iceb_tu_kolbait(100,gettext("Лист N")),gettext("Лист N"),*kollist);
fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("  Код     |    Наименование    | C а л ь д о  на  %02d.%02d.%d |   Счёт   |      Оборот за период       | С а л ь д о  на  %02d.%02d.%d |\n"),
dn,mn,gn,dk,mk,gk);
fprintf(ff,gettext("контраген-|    контрагента     ------------------------------|кореспон- |------------------------------------------------------------\n"));
fprintf(ff,gettext("   та     |                    |   Дебет      |   Кредит     | дент     |   Дебет      |    Кредит    |   Дебет      |   Кредит     |\n"));

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------\n");

}
/************************************/
/*Шапка конечного сальдо*/
/**********************************/
void gossrsn_sapsk(short dk,short gk,short mk,int *kollist_sk,int *kolstr_sk,FILE *ff)
{
*kollist_sk+=1;
*kolstr_sk+=6;
fprintf(ff,"%*s%d\n",iceb_tu_kolbait(68,gettext("Лист N")),gettext("Лист N"),*kollist_sk);

fprintf(ff,"\
----------------------------------------------------------------------------------\n");

fprintf(ff,gettext("  Код     |    Наименование    | C а л ь д о  на  %02d.%02d.%d |                    \n"),
dk,mk,gk);
fprintf(ff,gettext("контраген-|    контрагента     -------------------------------     Т е л е ф о н  \n"));
fprintf(ff,gettext("   та     |                    |   Дебет      |   Кредит     |                    \n"));

fprintf(ff,"\
----------------------------------------------------------------------------------\n");
}
/******************/
/*Счётчик строк*/
/*****************/
void gosrsn_ss(int metka, /*0-оборотный баланс  1-журнал ордер*/
short dn,short mn,short gn,
short dk,short mk,short gk,
int *kollist,int *kolstr,FILE *ff)
{
*kolstr+=1;

if(*kolstr <= kol_strok_na_liste)
  return;

fprintf(ff,"\f");
*kolstr=1;

if(metka == 0)
  sapgors(dn,mn,gn,dk,mk,gk,kollist,kolstr,ff);
if(metka == 1)
  gosrsn_hapgo(dn,mn,gn,dk,mk,gk,kollist,kolstr,ff);
if(metka == 2)
  gossrsn_sapsk(dk,gk,mk,kollist,kolstr,ff);
 
}

/**************************************/
/*Распечатка свода по счетам корреспондентам для журнала ордера*/
/**************************************************************/

void gosrsn_itoggo(const char *shet,
short dn,short mn,short gn, //Дата начала периода
short dk,short mk,short gk, //Дата конца периода
const char *naim_shet,
class SPISOK *oss,
const char *imatmptab_hax,
FILE *ff)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[1024];
struct  tm      *bf;
time_t tmm=time(NULL);
bf=localtime(&tmm);

fprintf(ff,"\f\%s\n\n\
%s %s %s\n\
%s\n\
%s %d.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d%s %s - %d:%d\n",
iceb_t_get_pnk("00",0),
gettext("Счёт"),
shet,naim_shet,
gettext("Всего по счетам корреспондентам."),
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ff,"\
--------------------------------------------------------------------------\n");
fprintf(ff,gettext("  Счёт    |   Наименование счета         |   Дебет       |   Кредит      |\n"));
fprintf(ff,"\
--------------------------------------------------------------------------\n");
double ideb=0.;
double ikre=0.;
double deb=0.;
double kre=0.;
class iceb_tu_str shet_kor("");
int kolstr=0;
for(int nomer_sheta=0; nomer_sheta < oss->kolih(); nomer_sheta++)
 {
  deb=kre=0.;
  sprintf(strsql,"select * from %s where shet='%s'",imatmptab_hax,oss->ravno(nomer_sheta));

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
   }

  if(kolstr > 0)
  while(cur.read_cursor(&row) != 0)
   {
    deb+=atof(row[2]);
    kre+=atof(row[3]);
   }

/********************
  for(int nomer_kontr_shet=0; nomer_kontr_shet < kontr_shet->kolih(); nomer_kontr_shet++)
   {
     
    polen(kontr_shet->ravno(nomer_kontr_shet),&shet_kor,2,'|');
    if(iceb_tu_srav_r(shet_kor.ravno(),oss->ravno(nomer_sheta),0) != 0)
      continue;
    deb+=kontr_shet_deb->ravno(nomer_kontr_shet);
    kre+=kontr_shet_kre->ravno(nomer_kontr_shet);

   }
*********************/
  sprintf(strsql,"select nais from Plansh where ns='%s'",oss->ravno(nomer_sheta));

  if(readkey(strsql,&row,&cur) != 1)
   {
    beep();
    printw(gettext("Не найден счёт %s в плане счетов !\n"),oss->ravno(nomer_sheta));
    OSTANOV();
    continue;
   }
  strncpy(strsql,row[0],sizeof(strsql)-1);

  fprintf(ff,"%-*s %-*.*s %15s",
  iceb_tu_kolbait(10,oss->ravno(nomer_sheta)),oss->ravno(nomer_sheta),
  iceb_tu_kolbait(30,strsql),iceb_tu_kolbait(30,strsql),strsql,
  prnbr(deb));
  
  fprintf(ff," %15s\n",prnbr(kre));
  ideb+=deb;
  ikre+=kre;

 }
fprintf(ff,"\
--------------------------------------------------------------------------\n");
fprintf(ff,"%*s %15s",iceb_tu_kolbait(41,gettext("Итого")),gettext("Итого"),prnbr(ideb));

fprintf(ff," %15s\n",prnbr(ikre));

}
/************************/
/*проверка контрагента*/
/************************/
int gorsrs_provk(const char *kod_k,
double debs,
double kres,
double debp,
double krep,
const char *imatmptab_hax)
//class SPISOK *kontr_shet)
{
char strsql[512];
int   metka_prop=0;  
//int kolih_kontrk_shet=kontr_shet->kolih();
class iceb_tu_str kod_kontr("");

if(fabs(debs - kres) < 0.009)
 if(fabs(debp) < 0.01 && fabs(krep) < 0.009)
  {

   sprintf(strsql,"select * from %s where kod_kontr='%s'",imatmptab_hax,kod_k);
   if(readkey(strsql) != 1)
     metka_prop=1;
/************
   int nomer_kontr_shet=0;
   for(nomer_kontr_shet=0 ; nomer_kontr_shet < kolih_kontr_shet; nomer_kontr_shet++)
    {
      polen(kontr_shet->ravno(nomer_kontr_shet),&kod_kontr,1,'|');

      if(iceb_tu_srav_r(kod_kontr.ravno(),kod_k,0) == 0)
       {
        break;
       }
    }
   if(nomer_kontr_shet == kolih_kontr_shet)
    metka_prop=1;
*********************/
  }
return(metka_prop);
}

/*********************/
/*Распечатка по дням*/
/***********************/
void gorsrsn_podn(const char *shet,
const char *naim_shet,
short dn,short mn,short gn, //Дата начала периода
short dk,short mk,short gk, //Дата конца периода
class SPISOK *ssd, /*Список счетов дебета*/
class SPISOK *ssk, /*Список счетов кредита*/
double debs,double kres,
double debp,double krep,
double debk,double krek,
int metka_f, /*0-A4 1-A3*/
const char *kod_val,
const char *imatab,
char *imaf)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
//FILE *ff;
class iceb_fopen ff;
if(ff.start(imaf,"w") != 0)
 return;
/*****************
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }
*******************/
int kolih_deb=ssd->kolih();
int kolih_kre=ssk->kolih();


class iceb_tu_str liniq;
for(int nomer=0; nomer < kolih_deb+2; nomer++)
 liniq.plus("-----------");


class SPISOK dni;
int kolih_dni=period(dn,mn,gn,dk,mk,gk,0);
//int kolih_mes_v_per=period(dn,mn,gn,dk,mk,gk,1);
short d=dn,m=mn,g=gn;

for(int nomer=0; nomer < kolih_dni; nomer++)
 {
  sprintf(strsql,"%02d.%02d.%04d",d,m,g);
  dni.plus(strsql);
  dpm(&d,&m,&g,1);
 }

class masiv_din_double den_shet_suma;
den_shet_suma.make_class(kolih_dni*kolih_deb);
class iceb_tu_str den_mes_god("");
class iceb_tu_str shet_d("");
int nomer_sheta=0;
//int nomer_data=0;
/****************
for(int nomer_mes_v_per=0; nomer_mes_v_per < kolih_mes_v_per; nomer_mes_v_per++)

for(int nomer_den_shet=0; nomer_den_shet < den_shet_deb[nomer_mes_v_per].kolih(); nomer_den_shet++)
 {
  polen(den_shet_deb[nomer_mes_v_per].ravno(nomer_den_shet),&den_mes_god,1,'|');
  polen(den_shet_deb[nomer_mes_v_per].ravno(nomer_den_shet),&shet_d,2,'|');
  nomer_data=dni.find(den_mes_god.ravno());  
  nomer_sheta=ssd->find_r(shet_d.ravno());
  den_shet_suma.plus(den_shet_deb_suma[nomer_mes_v_per].ravno(nomer_den_shet),nomer_data*kolih_deb+nomer_sheta);
 }
***************/

double deb=0.,kre=0.;

d=dn;
m=mn;
g=gn;
int kolstr=0;
for(int nomer=0; nomer < kolih_dni; nomer++)
 {
//  sprintf(strsql,"select * from %s where data='%04d-%02d-%02d' and deb <> 0.",imatab,g,m,d);
  sprintf(strsql,"select * from %s where data='%04d-%02d-%02d'",imatab,g,m,d);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
   }
  if(kolstr > 0)
  while(cur.read_cursor(&row) != 0)
   {

    deb=atof(row[2]);
    nomer_sheta=ssd->find_r(row[1]);
    den_shet_suma.plus(deb,nomer*kolih_deb+nomer_sheta);
   }

  dpm(&d,&m,&g,1);
 }


struct  tm      *bf;
time_t tmmn=time(NULL);
bf=localtime(&tmmn);


fprintf(ff.ff,"\x1B\x33%c",30); /*Изменение межстрочного интервала*/

fprintf(ff.ff,"%s\n%s %s %s",iceb_t_get_pnk("00",0),gettext("Журнал-ордер по счёту"),shet,naim_shet);
if(kod_val[0] != '\0')
 fprintf(ff.ff," %s:%s",gettext("Валюта"),kod_val);
 
sprintf(strsql,"\n%s %s %s %d.%d.%d%s %s: %d:%d",
gettext("Оборот по дебету счета"),shet,
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

buhdenshet(strsql,&dni,ssd,&den_shet_suma,metka_f,ff.ff);
podpis(ff.ff);


/************************************************************/




den_shet_suma.free_class();
den_shet_suma.make_class(kolih_dni*kolih_kre);
/**************
for(int nomer_mes_v_per=0; nomer_mes_v_per < kolih_mes_v_per; nomer_mes_v_per++)
for(int nomer_den_shet=0; nomer_den_shet < den_shet_kre[nomer_mes_v_per].kolih(); nomer_den_shet++)
 {
  polen(den_shet_kre[nomer_mes_v_per].ravno(nomer_den_shet),&den_mes_god,1,'|');
  polen(den_shet_kre[nomer_mes_v_per].ravno(nomer_den_shet),&shet_d,2,'|');
  nomer_data=dni.find(den_mes_god.ravno());  
  nomer_sheta=ssk->find_r(shet_d.ravno());
  den_shet_suma.plus(den_shet_kre_suma[nomer_mes_v_per].ravno(nomer_den_shet),nomer_data*kolih_kre+nomer_sheta);
 }
******************/
d=dn;
m=mn;
g=gn;
for(int nomer=0; nomer < kolih_dni; nomer++)
 {
  sprintf(strsql,"select * from %s where data='%04d-%02d-%02d'",imatab,g,m,d);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
   }
  if(kolstr > 0)
  while(cur.read_cursor(&row) != 0)
   {
    
    kre=atof(row[3]);
    nomer_sheta=ssk->find_r(row[1]);
    den_shet_suma.plus(kre,nomer*kolih_kre+nomer_sheta);
   }

  dpm(&d,&m,&g,1);
 }

fprintf(ff.ff,"\f");
fprintf(ff.ff,"%s\n%s %s %s",iceb_t_get_pnk("00",0),gettext("Журнал-ордер по счёту"),shet,naim_shet);

if(kod_val[0] != '\0')
 fprintf(ff.ff," %s:%s",gettext("Валюта"),kod_val);

sprintf(strsql,"\n%s %s %s %d.%d.%d%s %s: %d:%d",
gettext("Оборот по кредиту счета"),shet,
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);


buhdenshet(strsql,&dni,ssk,&den_shet_suma,metka_f,ff.ff);


fprintf(ff.ff,"\
        		   %s           %s\n",
gettext("Дебет"),
gettext("Кредит"));

fprintf(ff.ff,"\
%*s:%15.2f %15.2f\n",
iceb_tu_kolbait(18,gettext("Сальдо начальное")),gettext("Сальдо начальное"),debs,kres);

fprintf(ff.ff,"\
%*s:%15.2f %15.2f\n",
iceb_tu_kolbait(18,gettext("Оборот за период")),gettext("Оборот за период"),
debp,krep);

fprintf(ff.ff,"\
%*s:%15.2f %15.2f\n",
iceb_tu_kolbait(18,gettext("Сальдо конечное")),gettext("Сальдо конечное"),
debk,krek);

fprintf(ff.ff,"\n%s__________________________\n",
gettext("Главный бухгалтер"));

ff.end();

}
/***********************************************/
/* запись в промежуточную таблицу*/
/********************************************/
int gosrs_zvt(int per,const char *kod_kontr,double deb,double kre,const char *imatabl)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;



sprintf(strsql,"select debs,kres,debp,krep from %s where kod_kontr='%s'",imatabl,kod_kontr);

if(readkey(strsql,&row,&cur) == 0)
 {
  class iceb_tu_str naim("");
  class iceb_tu_str grup("");
  
  int kod_kontr_int=atoi(kod_kontr);
  sprintf(strsql,"select naikon,grup from Kontragent where kodkon='%s'",kod_kontr);
  if(readkey(strsql,&row,&cur) == 1)
   {
    naim.new_plus(row[0]);
    grup.new_plus(row[1]);
   }

  if(per == 0)
   sprintf(strsql,"insert into %s values ('%s',%.2f,%.2f,0.,0.,%d,'%s','%s',-1)",imatabl,kod_kontr,deb,kre,kod_kontr_int,naim.ravno_filtr(),grup.ravno_filtr());
  if(per == 1)
   sprintf(strsql,"insert into %s values ('%s',0.,0.,%.2f,%.2f,%d,'%s','%s',-1)",imatabl,kod_kontr,deb,kre,kod_kontr_int,naim.ravno_filtr(),grup.ravno_filtr());
 }
else
 {

  double debz=0.;
  double krez=0.;

  if(per == 0)
   {
  
    debz=deb+atof(row[0]);
    krez=kre+atof(row[1]);

    sprintf(strsql,"update %s \
set \
debs=%.2f,\
kres=%.2f \
where kod_kontr='%s'",imatabl,debz,krez,kod_kontr);
   }

  if(per == 1)
   {
    debz=deb+atof(row[2]);
    krez=kre+atof(row[3]);
   sprintf(strsql,"update %s \
set \
debp=%.2f,\
krep=%.2f \
where kod_kontr='%s'",imatabl,debz,krez,kod_kontr);
   }
 }
//printw("%s-%s\n",__FUNCTION__,strsql);
//OSTANOV();
if(sql_zapis(strsql,1,0) != 0)
 return(1);

return(0);
}
/********************************************/
/*запись в в таблицу по дням*/
/*********************************************/
int gosrs_zvt_den(short d,short m,short g,const char *shet,double deb,double kre,const char *imatabl)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;



sprintf(strsql,"select deb,kre from %s where data='%04d-%02d-%02d' and shet='%s'",imatabl,g,m,d,shet);

if(readkey(strsql,&row,&cur) == 0)
 {
  sprintf(strsql,"insert into %s values ('%04d-%02d-%02d','%s',%.2f,%.2f)",imatabl,g,m,d,shet,deb,kre);
 }
else
 {

  double debz=deb+atof(row[0]);
  double krez=kre+atof(row[1]);


  sprintf(strsql,"update %s \
set \
deb=%.2f,\
kre=%.2f \
where data='%04d-%02d-%02d' and shet='%s'",imatabl,debz,krez,g,m,d,shet);

 }

if(sql_zapis(strsql,1,0) != 0)
 return(1);

return(0);
}

/********************************************/
/*запись в в таблицу для шахматки*/
/*********************************************/
int gosrs_zvt_hax(const char *kod_kontr,const char *shet,double deb,double kre,const char *imatabl)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;



sprintf(strsql,"select deb,kre from %s where kod_kontr='%s' and shet='%s'",imatabl,kod_kontr,shet);

if(readkey(strsql,&row,&cur) == 0)
 {
  sprintf(strsql,"insert into %s values ('%s','%s',%.2f,%.2f)",imatabl,kod_kontr,shet,deb,kre);
 }
else
 {

  double debz=deb+atof(row[0]);
  double krez=kre+atof(row[1]);


  sprintf(strsql,"update %s \
set \
deb=%.2f,\
kre=%.2f \
where kod_kontr='%s' and shet='%s'",imatabl,debz,krez,kod_kontr,shet);

 }

if(sql_zapis(strsql,1,0) != 0)
 return(1);

return(0);
}
/****************************************/
/*пронумерование строк */
/**********************************/
int gosrsn_ps(const char *imatmptab,const char *str_sort)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
int nomer_str=0;
sprintf(strsql,"select kod_kontr from %s where debp <> 0. or krep <> 0. %s",imatmptab,str_sort);

if(cur.make_cursor(&bd,strsql) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return(1);
 }

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"update %s \
set \
ns=%d \
where kod_kontr='%s'",imatmptab,nomer_str++,row[0]);
  sql_zapis(strsql,1,0);
  
 }
 
return(0);
}
