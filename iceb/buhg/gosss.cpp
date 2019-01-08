/*$Id: gosss.c,v 5.21 2013/09/26 09:43:33 sasa Exp $*/
/*10.03.2016	02.09.2009	Белых А.И.	gosss.c
Расчёт журнала-ордера со свёрнутым сальдо
*/
#include        <math.h>
#include        <errno.h>
#include        "buhg.h"

void buhmesshet(class SPISOK *mes,class masiv_din_double *mes_shet,class SPISOK *sheta,FILE *ff);
void gosss_rspsk(const char *shetg,const char *naim_shet,short dn,short mn,short gn,short dk,short mk,short gk,class SPISOK *mes_shet,class masiv_din_double *mes_deb,class masiv_din_double *mes_kre,double debs,double kres,double debp,double krep,const char *kod_val,char *imaf);
void gosss_itogoprov(double debs,double kres,double debp,double krep,double dendebet,double denkredit,int metka,int *kolstrok,int *nomlist,FILE *ff);
void gosss_sap_prov(int *nomlist,int *kolstrok,FILE *ff);
void gosss_shet_prov(int *kolstrok,int *nomlist,FILE *ff);
int  gosss_dni(const char *imaf_dni,const char *shet,const char *naim_shet,class SPISOK *den_shet_period,class SPISOK *sheta_deb,class SPISOK *sheta_kre,class masiv_din_double *den_shet_deb_suma,class masiv_din_double *den_shet_kre_suma,double debs,double kres,double debp,double krep,int metka_a3_a4,const char *kod_val);

extern short	startgodb; /*Стартовый год*/
extern short	vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/
extern int kol_strok_na_liste;

int gosss(class guror_rr *rekrs,class spis_oth *oth)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
short dn=0,mn=0,gn=0;
short dk=0,mk=0,gk=0;

static class iceb_tu_str max_kz("");
struct  tm      *bf;
int sub=0;
time_t tmmn;
int kolih_znak=0;
class iceb_tu_str shet_s_mdkz(""); /*Cчёт с максимально-допустимым количеством знаков*/
class iceb_getkue_dat getval(rekrs->kod_val.ravno());

if(rsdatp(&dn,&mn,&gn,rekrs->dat1.ravno(),&dk,&mk,&gk,rekrs->dat2.ravno()) != 0)
 return(1);
 
short godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;

       

clear(); /*Очистить экран и закрасить фоновым цветом*/

VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите режим"));

MENU.VVOD_SPISOK_add_MD(gettext("Расчитать по субсчетам"));
MENU.VVOD_SPISOK_add_MD(gettext("Расчитать по счетам"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

while(menu3w(stdscr,&MENU,&sub,-1,-1,0) != 0);
  
if(sub == 2 || sub == -1)
 return(1);

if(sub == 1)
 {
  
  int kom1=0;
  VVOD VVOD1(2);
  VVOD1.VVOD_SPISOK_add_MD(gettext("Введите максимальное количество знаков в номере счёта"));

  if((kom1=vvod1(&max_kz,8,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
   return(1);
  if(kom1 == ESC)
   return(1);    
  kolih_znak=max_kz.ravno_atoi();
  if(kolih_znak <= 0)
   {
    iceb_t_soob(gettext("Количество знаков не может быть нулевой!"));
    return(1);    
   }
 }



time(&tmmn);
bf=localtime(&tmmn);

clear();
GDITE();
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

int kolstr=0;
double debs=0.;
double kres=0.;
double debp=0.;
double krep=0.;


/*Узнаем начальное сальдо по счёту*/

/*Отчет для многопорядкового плана счетов*/
if(vplsh == 1)
 sprintf(strsql,"select ns,deb,kre from Saldo where kkk='0' and gs=%d and \
ns like '%s%%'",godn,rekrs->shet.ravno());

/*Отчет по субсчету двух-порядковый план счетов*/
if(vds == 1 && vplsh == 0)
 sprintf(strsql,"select ns,deb,kre from Saldo where kkk='0' and gs=%d and \
ns='%s'",godn,rekrs->shet.ravno());

/*Отчет по счёту двух-порядковый план счетов*/
if(vds == 0 && vplsh == 0)
 sprintf(strsql,"select ns,deb,kre from Saldo where kkk='0' and gs=%d and \
ns like '%s%%'",godn,rekrs->shet.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
class iceb_tu_str start_god(godn);
start_god.plus("-01-01");

while(cur.read_cursor(&row) != 0)
 {
  debs+=getval.toue(row[1],start_god.ravno());
  kres+=getval.toue(row[2],start_god.ravno());
//  debs+=atof(row[1]);
//  kres+=atof(row[2]);
 }


printw("%s %.2f %.2f %d\n",gettext("Стартовое сальдо"),debs,kres,godn);

char imaf_prov[64];
sprintf(imaf_prov,"goprov%d.lst",getpid());
FILE *ff_prov;
if((ff_prov=fopen(imaf_prov,"w")) == NULL)
 {
  error_op_nfil(imaf_prov,errno,"");
  return(1);
 }

char imaf_prov_iden[64];
sprintf(imaf_prov_iden,"goprovi%d.lst",getpid());
FILE *ff_prov_iden;
if((ff_prov_iden=fopen(imaf_prov_iden,"w")) == NULL)
 {
  error_op_nfil(imaf_prov_iden,errno,"");
  return(1);
 }
iceb_t_zagolov(gettext("Расчёт журнала-ордера по счёту"),dn,mn,gn,dk,mk,gk,ff_prov);
iceb_t_zagolov(gettext("Расчёт журнала-ордера по счёту"),dn,mn,gn,dk,mk,gk,ff_prov_iden);

fprintf(ff_prov,"\n%s:%s %s",gettext("Счёт"),rekrs->shet.ravno(),naim_shet.ravno());
if(rekrs->kod_val.getdlinna() > 1)
 fprintf(ff_prov," %s:%s",gettext("Валюта"),rekrs->kod_val.ravno());
fprintf(ff_prov,"\n");

fprintf(ff_prov_iden,"\n%s:%s %s",gettext("Счёт"),rekrs->shet.ravno(),naim_shet.ravno());
if(rekrs->kod_val.getdlinna() > 1)
 fprintf(ff_prov_iden," %s:%s",gettext("Валюта"),rekrs->kod_val.ravno());
fprintf(ff_prov_iden,"\n");

int kolstr_prov_iden=5+2;
int kolstr_prov=5+2;
int kollist_prov=0;
int kollist_prov_iden=0;


gosss_sap_prov(&kollist_prov,&kolstr_prov,ff_prov);
gosss_sap_prov(&kollist_prov_iden,&kolstr_prov_iden,ff_prov_iden);

class SPISOK sheta_deb;
class SPISOK sheta_kre;

int kolih_mes_v_per=period(dn,mn,gn,dk,mk,gk,1);

class SPISOK den_shet[kolih_mes_v_per];
class masiv_din_double den_shet_deb[kolih_mes_v_per];
class masiv_din_double den_shet_kre[kolih_mes_v_per];


double deb=0.;
double kre=0.;
int kolstr1=0;
int kolih_den=period(1,1,godn,dk,mk,gk,0);
int nomer_den_shet=0;
int nomer_mes_shet=0;
int nomer_mes_v_per=-1;

class iceb_tu_str koment("");
class iceb_tu_str naikon("");

short d=1,m=1,g=godn;
int metka_perioda=0;
double ideb_den=0.;
double ikre_den=0.;
short den_zap=0;
short mes_zap=0;
class iceb_tu_str shet_zap("");

printw("%s %s\n",gettext("Реестр проводок по счёту"),rekrs->shet.ravno());
if(rekrs->kod_val.getdlinna() > 1)
 printw("%s:%s\n",gettext("Валюта"),rekrs->kod_val.ravno());
refresh();

while(sravmydat(d,m,g,dk,mk,gk) <= 0)
 {
  strzag(LINES-1,0,kolih_den,++kolstr1);
  
  
  //Для многопорядкового плана счетов
  if(vplsh == 1)
    sprintf(strsql,"select shk,kodkon,kto,nomd,oper,deb,kre,ktoi,komen,tz,datd,pod from Prov \
where val=0 and datp = '%04d-%02d-%02d' and sh like '%s%%' order by shk asc",g,m,d,rekrs->shet.ravno());

  /*Отчет по субсчету для двухпорядкового плана счетов*/
  if(vds == 1 && vplsh == 0)
    sprintf(strsql,"select shk,kodkon,kto,nomd,oper,deb,kre,ktoi,komen,tz,datd,pod from Prov \
where val=0 and datp = '%04d-%02d-%02d' and sh='%s' order by shk asc",g,m,d,rekrs->shet.ravno());

  /*Отчет по счёту для двухпорядкового плана счетов*/
  if(vds == 0 && vplsh == 0)
    sprintf(strsql,"select shk,kodkon,kto,nomd,oper,deb,kre,ktoi,komen,tz,datd,pod from Prov \
where val=0 and datp = '%04d-%02d-%02d' and sh like '%s%%' order by shk asc",g,m,d,rekrs->shet.ravno());


  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return(1);
   }

  if(sravmydat(d,m,g,dn,mn,gn) >= 0)
  if(mes_zap != m)
   {
    nomer_mes_v_per++;
    mes_zap=m;
   }            


  shet_zap.new_plus("");
  if(kolstr > 0)
  while(cur.read_cursor(&row) != 0)
   {
    deb=getval.toue(row[5],row[10]);
    kre=getval.toue(row[6],row[10]);
//    deb=atof(row[5]);
//    kre=atof(row[6]);
    
    if(sravmydat(d,m,g,dn,mn,gn) < 0) /*До периода*/
     {
      debs+=deb;
      kres+=kre;
     }
    else /*Период*/
     {
      if(metka_perioda == 0)
       {
        gosss_shet_prov(&kolstr_prov,&kollist_prov,ff_prov);
        gosss_shet_prov(&kolstr_prov_iden,&kollist_prov_iden,ff_prov_iden);
        if(debs > kres)
         {
          fprintf(ff_prov,"%*s:%12.2f\n",iceb_tu_kolbait(45,gettext("Сальдо начальное")),gettext("Сальдо начальное"),debs-kres);
          fprintf(ff_prov_iden,"%*s:%12.2f\n",iceb_tu_kolbait(45,gettext("Сальдо начальное")),gettext("Сальдо начальное"),debs-kres);
         }
        else
         {
          fprintf(ff_prov,"%*s:%12s %12.2f\n",iceb_tu_kolbait(45,gettext("Сальдо начальное")),gettext("Сальдо начальное")," ",kres-debs);
          fprintf(ff_prov_iden,"%*s:%12s %12.2f\n",iceb_tu_kolbait(45,gettext("Сальдо начальное")),gettext("Сальдо начальное")," ",kres-debs);
         }        
        metka_perioda=1;
       }

      if(den_zap != d)
       {
        if(den_zap != 0)
         gosss_itogoprov(debs,kres,debp,krep,ideb_den,ikre_den,1,&kolstr_prov_iden,&kollist_prov_iden,ff_prov_iden);
        ideb_den=ikre_den=0.;
        den_zap=d;
       }


      debp+=deb;
      krep+=kre;
      ideb_den+=deb;
      ikre_den+=kre;
      shet_s_mdkz.new_plus("");

      if(kolih_znak == 0)
       shet_s_mdkz.new_plus(row[0]);
      else
       {
        sprintf(strsql,"%.*s",
        iceb_tu_kolbait(kolih_znak,row[0]),row[0]);
        shet_s_mdkz.new_plus(strsql);
       }       
      sprintf(strsql,"%02d.%02d.%04d|%s",d,m,g,shet_s_mdkz.ravno());

      if(iceb_tu_srav_r(shet_zap.ravno(),shet_s_mdkz.ravno(),0) != 0) /*День не меняется*/
       {
        if((nomer_den_shet=den_shet[nomer_mes_v_per].find_r(strsql)) < 0)
         den_shet[nomer_mes_v_per].plus(strsql);

        den_shet_deb[nomer_mes_v_per].plus(deb,nomer_den_shet);      
        den_shet_kre[nomer_mes_v_per].plus(kre,nomer_den_shet);      
        if(nomer_den_shet < 0) /*Вообщето такого не может быть*/
         nomer_den_shet=den_shet[nomer_mes_v_per].kolih()-1; /*Нужен номер куда записало для дальнейшего использования*/
        shet_zap.new_plus(shet_s_mdkz.ravno());
       }
      else
       {
        den_shet_deb[nomer_mes_v_per].plus(deb,nomer_den_shet);      
        den_shet_kre[nomer_mes_v_per].plus(kre,nomer_den_shet);      
       }
       


      if(fabs(deb) > 0.009)
       {
        if(sheta_deb.find_r(shet_s_mdkz.ravno(),sub) < 0)
         sheta_deb.plus(shet_s_mdkz.ravno());
       }
      else
       if(sheta_kre.find_r(shet_s_mdkz.ravno(),sub) < 0)
        sheta_kre.plus(shet_s_mdkz.ravno());

      naikon.new_plus("");
      /*получение наименования контрагента*/
      buhpnk(&naikon,row[1],iceb_t_mpods(row[2]),atoi(row[10]),row[11],row[3],atoi(row[12]));

      sozkom_prov(&koment,row[8],row[2],row[4]);

      gosss_shet_prov(&kolstr_prov,&kollist_prov,ff_prov);

      fprintf(ff_prov,"%02d.%02d.%4d %-*s %-*s %-*s%*s %-*s %12s",
      d,m,g,
      iceb_tu_kolbait(6,row[0]),row[0],
      iceb_tu_kolbait(8,row[1]),row[1],
      iceb_tu_kolbait(3,row[2]),row[2],
      iceb_tu_kolbait(4,row[7]),row[7],
      iceb_tu_kolbait(10,row[3]),row[3],
      prnbr(deb));

      fprintf(ff_prov," %12s %-*s %-*.*s %.*s\n",
      prnbr(kre),
      iceb_tu_kolbait(4,row[4]),
      row[4],
      iceb_tu_kolbait(32,koment.ravno()),
      iceb_tu_kolbait(32,koment.ravno()),
      koment.ravno(),
      iceb_tu_kolbait(40,naikon.ravno()),
      naikon.ravno());

      for(int nom=32; nom < iceb_tu_strlen(koment.ravno()); nom+=32)
       {
        gosss_shet_prov(&kolstr_prov,&kollist_prov,ff_prov);

        fprintf(ff_prov,"%76s %-*.*s\n",
        "",
        iceb_tu_kolbait(32,iceb_tu_adrsimv(nom,koment.ravno())),
        iceb_tu_kolbait(32,iceb_tu_adrsimv(nom,koment.ravno())),
        iceb_tu_adrsimv(nom,koment.ravno()));
       }

      gosss_shet_prov(&kolstr_prov_iden,&kollist_prov_iden,ff_prov_iden);

      fprintf(ff_prov_iden,"%02d.%02d.%4d %-*s %-*s %-*s%*s %-*s %12s",
      d,m,g,
      iceb_tu_kolbait(6,row[0]),row[0],
      iceb_tu_kolbait(8,row[1]),row[1],
      iceb_tu_kolbait(3,row[2]),row[2],
      iceb_tu_kolbait(4,row[7]),row[7],
      iceb_tu_kolbait(10,row[3]),row[3],
      prnbr(deb));

      fprintf(ff_prov_iden," %12s %-*s %-*.*s %.*s\n",prnbr(kre),
      iceb_tu_kolbait(4,row[4]),row[4],
      iceb_tu_kolbait(32,koment.ravno()),
      iceb_tu_kolbait(32,koment.ravno()),
      koment.ravno(),
      iceb_tu_kolbait(40,naikon.ravno()),
      naikon.ravno());

      for(int nom=32; nom < iceb_tu_strlen(koment.ravno()); nom+=32)
       {

        gosss_shet_prov(&kolstr_prov_iden,&kollist_prov_iden,ff_prov_iden);

        fprintf(ff_prov_iden,"%76s %-*.*s\n",
        "",
        iceb_tu_kolbait(32,iceb_tu_adrsimv(nom,koment.ravno())),
        iceb_tu_kolbait(32,iceb_tu_adrsimv(nom,koment.ravno())),
        iceb_tu_adrsimv(nom,koment.ravno()));
       }       
     }
     
   }



  dpm(&d,&m,&g,1);
 }

GDITE();

gosss_itogoprov(debs,kres,debp,krep,0.,0.,0,&kolstr_prov,&kollist_prov,ff_prov);
gosss_itogoprov(debs,kres,debp,krep,ideb_den,ikre_den,1,&kolstr_prov_iden,&kollist_prov_iden,ff_prov_iden);

fprintf(ff_prov,"\n%s_______________________\n",gettext("Главный бухгалтер"));
podpis(ff_prov);
fclose(ff_prov);

fprintf(ff_prov_iden,"\n%s___________________\n",gettext("Главный бухгалтер"));
podpis(ff_prov_iden);
fclose(ff_prov_iden);



int kolih_deb=sheta_deb.kolih();
int kolih_kre=sheta_kre.kolih();


class SPISOK den_shet_period;
d=dn;
m=mn;
g=gn;
while(sravmydat(d,m,g,dk,mk,gk) <= 0)
 {
  sprintf(strsql,"%02d.%02d.%04d",d,m,g);
  den_shet_period.plus(strsql);
  dpm(&d,&m,&g,1);
 }  

kolih_den=den_shet_period.kolih();

class masiv_din_double den_shet_deb_suma;
class masiv_din_double den_shet_kre_suma;

den_shet_deb_suma.make_class(kolih_den*kolih_deb);
den_shet_kre_suma.make_class(kolih_den*kolih_kre);


int nomer_den_shet_period=0;
int nomer_sheta=0;
class iceb_tu_str data("");
class iceb_tu_str shet_d("");
class iceb_tu_str messhet("");
class SPISOK mes_shet;
class masiv_din_double mes_shet_deb;
class masiv_din_double mes_shet_kre;

for(nomer_mes_v_per=0; nomer_mes_v_per < kolih_mes_v_per; nomer_mes_v_per++)
for(int nomer_den_shet=0; nomer_den_shet < den_shet[nomer_mes_v_per].kolih(); nomer_den_shet++)
 {
  polen(den_shet[nomer_mes_v_per].ravno(nomer_den_shet),&data,1,'|');    
  polen(den_shet[nomer_mes_v_per].ravno(nomer_den_shet),&shet_d,2,'|');    

  nomer_den_shet_period=den_shet_period.find(data.ravno());

  deb=den_shet_deb[nomer_mes_v_per].ravno(nomer_den_shet);
  kre=den_shet_kre[nomer_mes_v_per].ravno(nomer_den_shet);

  if((nomer_sheta=sheta_deb.find_r(shet_d.ravno())) >= 0)  
    den_shet_deb_suma.plus(deb,nomer_den_shet_period*kolih_deb+nomer_sheta);
  if((nomer_sheta=sheta_kre.find_r(shet_d.ravno())) >= 0)  
    den_shet_kre_suma.plus(kre,nomer_den_shet_period*kolih_kre+nomer_sheta);

   messhet.new_plus(iceb_tu_adrsimv(3,data.ravno()));

   
   sprintf(strsql,"%s|%s",messhet.ravno(),shet_d.ravno());
   if((nomer_mes_shet=mes_shet.find_r(strsql)) < 0)
     mes_shet.plus(strsql);

   mes_shet_deb.plus(deb,nomer_mes_shet);      
   mes_shet_kre.plus(kre,nomer_mes_shet);      
 }


char imaf_dni[64];
sprintf(imaf_dni,"gpd%d.lst",getpid());

/*распечатка журнала на А4*/
gosss_dni(imaf_dni,rekrs->shet.ravno(),naim_shet.ravno(),&den_shet_period,&sheta_deb,&sheta_kre,&den_shet_deb_suma,&den_shet_kre_suma,debs,kres,debp,krep,0,rekrs->kod_val.ravno());

char imaf_dni_a3[64];
sprintf(imaf_dni_a3,"gpda%d.lst",getpid());

/*распечатка журнала на А3*/
gosss_dni(imaf_dni_a3,rekrs->shet.ravno(),naim_shet.ravno(),&den_shet_period,&sheta_deb,&sheta_kre,&den_shet_deb_suma,&den_shet_kre_suma,debs,kres,debp,krep,1,rekrs->kod_val.ravno());

/*Распечатка месячных итогов*/
/******************************************************************/


class SPISOK mes_shet_period;
d=dn;
m=mn;
g=gn;
while(sravmydat(d,m,g,dk,mk,gk) <= 0)
 {
  sprintf(strsql,"%02d.%04d",m,g);
  mes_shet_period.plus(strsql);
  dpm(&d,&m,&g,3);
 }  


class masiv_din_double mes_shet_deb_suma;
class masiv_din_double mes_shet_kre_suma;

int kolih_mes=mes_shet_period.kolih();
mes_shet_deb_suma.make_class(kolih_mes*kolih_deb);
mes_shet_kre_suma.make_class(kolih_mes*kolih_kre);
int nomer_mes_shet_period=0;

for(int nomer_mes_shet=0; nomer_mes_shet < mes_shet.kolih(); nomer_mes_shet++)
 {
  polen(mes_shet.ravno(nomer_mes_shet),&data,1,'|');    
  polen(mes_shet.ravno(nomer_mes_shet),&shet_d,2,'|');    

  nomer_mes_shet_period=mes_shet_period.find(data.ravno());
  
  if((nomer_sheta=sheta_deb.find_r(shet_d.ravno())) >= 0)  
    mes_shet_deb_suma.plus(mes_shet_deb.ravno(nomer_mes_shet),nomer_mes_shet_period*kolih_deb+nomer_sheta);
  if((nomer_sheta=sheta_kre.find_r(shet_d.ravno())) >= 0)  
    mes_shet_kre_suma.plus(mes_shet_kre.ravno(nomer_mes_shet),nomer_mes_shet_period*kolih_kre+nomer_sheta);
 }



FILE *ff_mes;
char imaf_mes[64];
sprintf(imaf_mes,"gpdm%d.lst",getpid());
if((ff_mes = fopen(imaf_mes,"w")) == NULL)
 {
  error_op_nfil(imaf_mes,errno,"");
  return(1);
 }
fprintf(ff_mes,"\x1B\x33%c",28); /*Изменение межстрочного интервала*/

printw("%s %s\n",gettext("Месячные итоги по счёту"),rekrs->shet.ravno());
refresh();


fprintf(ff_mes,"%s\n%s %s %s",iceb_t_get_pnk("00",0),
gettext("Месячные итоги по счёту"),rekrs->shet.ravno(),naim_shet.ravno());
if(rekrs->kod_val.getdlinna() > 1)
 fprintf(ff_mes," %s:%s",gettext("Валюта"),rekrs->kod_val.ravno());
fprintf(ff_mes,"\n");

fprintf(ff_mes,"%s %s ",gettext("Оборот по дебету счета"),rekrs->shet.ravno());

fprintf(ff_mes,"%s %d.%d.%d%s %s: %d:%d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);



buhmesshet(&mes_shet_period,&mes_shet_deb_suma,&sheta_deb,ff_mes);

fprintf(ff_mes,"\n%s %s \n",gettext("Оборот по кредиту счета"),rekrs->shet.ravno());

buhmesshet(&mes_shet_period,&mes_shet_kre_suma,&sheta_kre,ff_mes);


podpis(ff_mes);
fclose(ff_mes);

printw("%s %s\n",gettext("Итоги по счетам корреспондентам для счёта"),rekrs->shet.ravno());
refresh();
char imaf_shk[64];
gosss_rspsk(rekrs->shet.ravno(),naim_shet.ravno(),dn,mn,gn,dk,mk,gk,&mes_shet,&mes_shet_deb,&mes_shet_kre,debs,kres,debp,krep,rekrs->kod_val.ravno(),imaf_shk);




/*На экран тоже самое*/

printw("\
                               %s           %s\n",
gettext("Дебет"),
gettext("Кредит"));
sprintf(strsql,"%s %02d.%02d.%d",gettext("Сальдо на"),dn,mn,gn);

int kolznak=iceb_tu_strlen(strsql);

deb=kre=0.;
if(debs >= kres)
 {
  deb=debs-kres;
  printw("%s:%15s\n",strsql,prnbr(deb));
 }
else
 {
  kre=kres-debs;
  printw("%s:%15s  %15s\n",strsql," ",prnbr(kre));
 }
int kolb=iceb_tu_kolbait(kolznak,gettext("Оборот за период"));
printw("%*s:%15s",kolb,gettext("Оборот за период"),prnbr(debp));

printw("  %15s\n",prnbr(krep));

printw("\
%*s:%15s",iceb_tu_kolbait(kolznak,gettext("Сальдо роз-нутое")),gettext("Сальдо роз-нутое"),prnbr(deb+debp));

printw("  %15s\n",prnbr(kre+krep));

sprintf(strsql,"%s %02d.%02d.%d",gettext("Сальдо на"),dk,mk,gk);
if(debs+debp >=  kres+krep)
 {
  deb=(debs+debp)-(kres+krep);
  printw("%s:%15s\n",strsql,prnbr(deb));
 }
else
 {
  kre=(kres+krep)-(debs+debp);
  printw("%s:%15s  %15s\n",strsql," ",prnbr(kre));
 }


oth->spis_imaf.plus(imaf_dni);
sprintf(strsql,"%s %s A4",gettext("Журнал-ордер по счёту"),rekrs->shet.ravno());
oth->spis_naim.plus(strsql);

oth->spis_imaf.plus(imaf_dni_a3);
sprintf(strsql,"%s %s A3",gettext("Журнал-ордер по счёту"),rekrs->shet.ravno());
oth->spis_naim.plus(strsql);

oth->spis_imaf.plus(imaf_mes);
sprintf(strsql,"%s %s",gettext("Месячные итоги по счёту"),rekrs->shet.ravno());
oth->spis_naim.plus(strsql);

oth->spis_imaf.plus(imaf_shk);
sprintf(strsql,"%s %s",gettext("Итоги по счетам корреспондентам для счёта"),rekrs->shet.ravno());
oth->spis_naim.plus(strsql);

oth->spis_imaf.plus(imaf_prov);
sprintf(strsql,"%s %s",gettext("Реестр проводок по счёту"),rekrs->shet.ravno());
oth->spis_naim.plus(strsql);

oth->spis_imaf.plus(imaf_prov_iden);
sprintf(strsql,"%s %s",gettext("Реестр проводок с дневными итогами по счёту"),rekrs->shet.ravno());
oth->spis_naim.plus(strsql);

for(int nomer=0; nomer < oth->spis_imaf.kolih(); nomer++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nomer),1);
 
printw_vr(tmmn);
OSTANOV();

return(0);
}


/***********************************/
/*Распечатка свода по счетам корреспондентам*/
/*******************************************/

void gosss_rspsk(const char *shetg,
const char *naim_shet,
short dn,short mn,short gn,
short dk,short mk,short gk,
class SPISOK *mes_shet,
class masiv_din_double *mes_deb,
class masiv_din_double *mes_kre,
double debs,
double kres,
double debp,
double krep,
const char *kod_val,
char *imaf)
{
class SPISOK sheta;
class masiv_din_double deb;
class masiv_din_double kre;
class iceb_tu_str shet("");
int nomer_sheta=0;
for(int nomer=0; nomer < mes_shet->kolih(); nomer++)
 {
  polen(mes_shet->ravno(nomer),&shet,2,'|');
  if((nomer_sheta=sheta.find_r(shet.ravno())) < 0)
   sheta.plus(shet.ravno());
  deb.plus(mes_deb->ravno(nomer),nomer_sheta);
  kre.plus(mes_kre->ravno(nomer),nomer_sheta);
 }

sprintf(imaf,"spsk%d.lst",getpid());
FILE *ff;
if((ff=fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

iceb_t_zagolov(gettext("Расчёт журнала-ордера по счёту"),dn,mn,gn,dk,mk,gk,ff);
fprintf(ff,"\n%s:%s %s",gettext("Счёт"),shetg,naim_shet);
if(kod_val[0] != '\0')
 fprintf(ff," %s:%s",gettext("Валюта"),kod_val);
fprintf(ff,"\n");

fprintf(ff,"\
--------------------------------------------------------------\n");
fprintf(ff,gettext("\
  Счёт  |     Наименование счета       |  Дебет   | Кредит   |\n"));
fprintf(ff,"\
--------------------------------------------------------------\n");

if(debs > kres)
 {
  fprintf(ff,"%*s:%10.2f\n",iceb_tu_kolbait(39,gettext("Сальдо начальное")),gettext("Сальдо начальное"),debs-kres);
 }
else
 {
  fprintf(ff,"%*s:%10s %10.2f\n",iceb_tu_kolbait(39,gettext("Сальдо начальное")),gettext("Сальдо начальное"),"",kres-debs);
 }
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
class iceb_tu_str naimshet("");
for(int nomer=0; nomer < sheta.kolih(); nomer++)
 {
  sprintf(strsql,"select nais from Plansh where ns='%s'",sheta.ravno(nomer));
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    naimshet.new_plus(row[0]);
  else
    naimshet.new_plus("");  
  fprintf(ff,"%*s %-*.*s %10.2f %10.2f\n",
  iceb_tu_kolbait(8,sheta.ravno(nomer)),sheta.ravno(nomer),
  iceb_tu_kolbait(30,naimshet.ravno()),iceb_tu_kolbait(30,naimshet.ravno()),naimshet.ravno(),
  deb.ravno(nomer),
  kre.ravno(nomer));      
  
  if(iceb_tu_strlen(naimshet.ravno()) > 30)
   fprintf(ff,"%8s %s\n","",iceb_tu_adrsimv(30,naimshet.ravno()));
 }

fprintf(ff,"\
--------------------------------------------------------------\n");

fprintf(ff,"%*s:%10.2f %10.2f\n",
iceb_tu_kolbait(39,gettext("Оборот за период")),gettext("Оборот за период"),debp,krep);

fprintf(ff,"%*s:%10.2f %10.2f\n",
iceb_tu_kolbait(39,gettext("Сальдо развернутое")),
gettext("Сальдо развернутое"),
debp+debs,krep+kres);

if(debs+debp > kres+krep)
 {
  fprintf(ff,"%*s:%10.2f\n",
  iceb_tu_kolbait(39,gettext("Сальдо свернутое")),
  gettext("Сальдо свернутое"),
  debs+debp-kres-krep);
 }  
else
 {
  fprintf(ff,"%*s:%10s %10.2f\n",
  iceb_tu_kolbait(39,gettext("Сальдо свернутое")),
  gettext("Сальдо свернутое"),
  " ",
  kres+krep-debs-debp);
 }

fprintf(ff,"\n%s_______________________\n",gettext("Главный бухгалтер"));


podpis(ff);


fclose(ff);

}
/*****************/
/*Шапка рапечатки*/
/*****************/

void gosss_sap_prov(int *nomlist,int *kolstrok,FILE *ff)
{

*nomlist+=1;
fprintf(ff,"%90s %s%d\n"," ",gettext("Лист N"),*nomlist);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
  Дата    |Сч.к-т|Контраг.| Кто   |Номер док.|    Дебет   |   Кредит   |К.о.|      К о м е н т а р и й       |Наименование контрагента|\n"));

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------\n");
*kolstrok+=4;

}

/***************/
/*Счетчик строк*/
/***************/
void gosss_shet_prov(int *kolstrok,int *nomlist,FILE *ff)
{
*kolstrok+=1;
if(*kolstrok <= kol_strok_na_liste)
 return;

*kolstrok=1;
fprintf(ff,"\f");
gosss_sap_prov(nomlist,kolstrok,ff);

}

/***************/
/*Выдача итога*/
/**************/
void gosss_itogoprov(double debs,double kres,
double debp,double krep,
double dendebet,double denkredit,
int metka, //0-без дневного оборота
int *kolstrok,int *nomlist,
FILE *ff)
{

gosss_shet_prov(kolstrok,nomlist,ff);

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------------\n");
if(metka == 1)
 {
  gosss_shet_prov(kolstrok,nomlist,ff);
  fprintf(ff,"%*s:%12.2f %12.2f\n",iceb_tu_kolbait(45,gettext("Итого за день")),gettext("Итого за день"),
  dendebet,denkredit);
 }
gosss_shet_prov(kolstrok,nomlist,ff);

fprintf(ff,"%*s:%12.2f %12.2f\n",iceb_tu_kolbait(45,gettext("Оборот за период")),gettext("Оборот за период"),debp,krep);

double debet=debs+debp;
double kredit=kres+krep;

gosss_shet_prov(kolstrok,nomlist,ff);

fprintf(ff,"%*s:%12.2f %12.2f\n",
iceb_tu_kolbait(45,gettext("Сальдо развернутое")),gettext("Сальдо развернутое"),debet,kredit);

gosss_shet_prov(kolstrok,nomlist,ff);
if(debet > kredit)
 {
  fprintf(ff,"%*s:%12.2f\n",iceb_tu_kolbait(45,gettext("Сальдо свернутое")),gettext("Сальдо свернутое"),debet-kredit);
 }  
else
 {
  fprintf(ff,"%*s:%12s %12.2f\n",iceb_tu_kolbait(45,gettext("Сальдо свернутое")),gettext("Сальдо свернутое")," ",kredit-debet);
 }

gosss_shet_prov(kolstrok,nomlist,ff);
fprintf(ff,"\n");

}

/*******************************************/
/*Распечатка по дням*/
/******************************/
int  gosss_dni(const char *imaf_dni,
const char *shet,const char *naim_shet,
class SPISOK *den_shet_period,
class SPISOK *sheta_deb,
class SPISOK *sheta_kre,
class masiv_din_double *den_shet_deb_suma,
class masiv_din_double *den_shet_kre_suma,
double debs,
double kres,
double debp,
double krep,
int metka_a3_a4, /*0-a4 1-a3*/
const char *kod_val)
{
struct  tm      *bf;
char strsql[512];
time_t tmmn;
time(&tmmn);

bf=localtime(&tmmn);


FILE *ff_dni;
if((ff_dni = fopen(imaf_dni,"w")) == NULL)
 {
  error_op_nfil(imaf_dni,errno,"");
  return(1);
 }
fprintf(ff_dni,"\x1B%s\x1E\n","3"); /*Изменение межстрочного интервала*/


printw("%s\n",gettext("Распечатка по дням"));
refresh();


fprintf(ff_dni,"%s\n%s %s %s",iceb_t_get_pnk("00",0),
gettext("Журнал-ордер по счёту"),shet,naim_shet);
if(kod_val[0] != '\0')
 fprintf(ff_dni," %s:%s",gettext("Валюта"),kod_val);
fprintf(ff_dni,"\n");

sprintf(strsql,"%s %s %s %d.%d.%d%s %s: %d:%d",
gettext("Оборот по дебету счета"),shet,
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);



buhdenshet(strsql,den_shet_period,sheta_deb,den_shet_deb_suma,metka_a3_a4,ff_dni);
podpis(ff_dni);

fprintf(ff_dni,"\f");

fprintf(ff_dni,"%s\n%s %s %s\n",iceb_t_get_pnk("00",0),
gettext("Журнал-ордер по счёту"),shet,naim_shet);

sprintf(strsql,"%s %s %s %d.%d.%d%s %s: %d:%d",
gettext("Оборот по кредиту счета"),shet,
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

buhdenshet(strsql,den_shet_period,sheta_kre,den_shet_kre_suma,metka_a3_a4,ff_dni);


fprintf(ff_dni,"\
                           %s           %s\n",
gettext("Дебет"),
gettext("Кредит"));

if(debs >= kres)
 fprintf(ff_dni,"\
%*s:%15.2f\n",iceb_tu_kolbait(18,gettext("Сальдо начальное")),gettext("Сальдо начальное"),debs-kres);
else
  fprintf(ff_dni,"\
%*s:%15s  %15.2f\n",iceb_tu_kolbait(18,gettext("Сальдо начальное")),gettext("Сальдо начальное")," ",kres-debs);

fprintf(ff_dni,"\
%*s:%15.2f  %15.2f\n",iceb_tu_kolbait(18,gettext("Оборот за период")),gettext("Оборот за период"),debp,krep);

if(debs+debp >=  kres+krep)
  fprintf(ff_dni,"\
%*s:%15.2f\n",iceb_tu_kolbait(18,gettext("Сальдо конечное")),gettext("Сальдо конечное"),(debs+debp)-(kres+krep));
else
  fprintf(ff_dni,"\
%*s:%15s  %15.2f\n",iceb_tu_kolbait(18,gettext("Сальдо конечное")),gettext("Сальдо конечное")," ",(kres+krep)-(debs+debp));

fprintf(ff_dni,"\n%s________________  %s________________\n",
gettext("Главный бухгалтер"),
gettext("Исполнитель"));

fprintf(ff_dni,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/

fclose(ff_dni);
return(0);

}