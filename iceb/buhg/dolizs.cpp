/*$Id: dolizs.c,v 5.25 2013/09/26 09:43:32 sasa Exp $*/
/*14.07.2015	21.02.2005 	Белых А.И.	dolizs.c
Расчёт распределения по зарплатным счетам налогов
*/
#include <errno.h>
#include "buhg.h"


void dolizs_rs(class masiv_din_int *knv_nalog_661,
class masiv_din_double *knv_nalog_suma_661,
class masiv_din_double *knv_nalog_suma_663,
double itogo_suma_661,
double *itogo_suma_663,
double suma_nalog,
class SPISOK *dop_zar_sheta,
class SPISOK *dop_zar_sheta_spn,
class masiv_din_int *vse_kodi_nah,
FILE *ff);

extern short mmm,ggg;

extern char *shetb; /*Бюджетные счета начислений*/
extern char *shetbu; /*Бюджетные счета удержаний*/

extern short *knvr;/*Коды начислений не входящие в расчёт подоходного налога*/
extern class iceb_tu_str kodpn_all; /*Все коды подоходного налога*/

extern short kuprof; //Код удержания в профсоюзный фонд
extern short *knvrprof; //Коды не входящие в расчёт удержаний в профсоюзный фонд

extern class iceb_tu_str knvr_esv_r; /*Коды не входящие в расчёт единого социального взноса с работника*/
extern int kod_esv; /*Код удержания единого социального взноса*/
extern int kod_esv_bol; /*Код удержания единого социального взноса*/
extern int kod_esv_inv; /*Код удержания единого социального взноса*/
extern int kod_esv_dog; /*Код удержания единого социального взноса*/

extern class iceb_tu_str shrpz; /*Счет расчётов по зарплате*/


extern class SPISOK dop_zar_sheta; /*Список дополнительных зарплатных счетов*/
extern class SPISOK dop_zar_sheta_spn; 

int dolizs(class spis_oth *oth)
{
int  kodbzr=0;  /*Код отчисления на безработицу*/
int kodpen=0;  /*Код пенсионных отчислений*/
char strsql[512];
static class iceb_tu_str dat1("");
if(dat1.ravno()[0] == '\0')
 {
  sprintf(strsql,"%d.%d",mmm,ggg);
  dat1.new_plus(strsql);
 }
VVOD VVOD1(2);
VVOD1.VVOD_SPISOK_add_ZAG(gettext("Расчёт частей зарплатных счетов"));
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите дату (м.г)."));
 
naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"Enter",gettext("расчитать"),
"F10",gettext("выход"),NULL);

int kom1=vvod1(&dat1,8,&VVOD1,NULL,stdscr,-1,-1);

switch(kom1)
 {
  case FK1:
   GDITE();
//   prosf(strsql);
   goto naz;
  
  case FK10:
  case ESC:
   return(1);

  case FK2:
  case ENTER:
   break;

  default:
    goto naz;   
 }

short mr,gr;

if(rsdat1(&mr,&gr,dat1.ravno()) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz;
 }

GDITE();

iceb_t_poldan("Код пенсионного отчисления",&kodpen,"zarnast.alx");
iceb_t_poldan("Код отчисления на безработицу",&kodbzr,"zarnast.alx");


/*Чтение настроек для расчёта единого социального взноса*/
zarrnesv();

SQLCURSOR cur;
SQL_str row;
int kolstr=0;

class masiv_din_int sn661; //Список начислений для 661 счёта
sprintf(strsql,"select kod from Nash");

int kolih_nah=0;
if((kolih_nah=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolih_nah == 0)
 {
  iceb_t_soob(gettext("Не введены начисления !"));
  return(1);
 }

int kod;
class masiv_din_int vse_kodi_nah; /*Список всех кодов начисления*/
int metka=0;

while(cur.read_cursor(&row) != 0)
 {
  kod=atoi(row[0]);
  vse_kodi_nah.plus(kod,-1);
  metka=0;
  for(int ii=0; ii < dop_zar_sheta_spn.kolih(); ii++)
   if(proverka(dop_zar_sheta_spn.ravno(ii),row[0],0,1) == 0)
    {
     metka=1;
     break;
    }
  if(metka == 0)
   sn661.plus(kod,-1);
 }

sprintf(strsql,"select tabn,prn,knah,suma,shet from Zarp where datz >= '%04d-%02d-%02d' and \
datz <= '%04d-%02d-%02d' and suma <> 0. order by tabn asc",gr,mr,1,gr,mr,31);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }

int kodsocstr;  /*Код отчисления на соц-страх*/
//int *kodsocstrnv; //Коды не входящие в расчёт соцстраха
iceb_t_poldan("Код отчисления в соц-страх",&kodsocstr,"zarnast.alx");

int kolstr1=0;
short prn=0;
double suma_podoh=0.;
double suma_pens=0.;
double suma_socstr=0.;
double suma_bezrab=0.;
double suma_profs=0.;
double suma_esv=0.;

int knah=0;
double suma=0.;
int kolih_dop_shet=dop_zar_sheta.kolih();

class SPISOK knah_shet_663[kolih_dop_shet]; //Список начисление-счёт
class masiv_din_double suma_knah_shet_663[kolih_dop_shet]; //Суммы по начислениям-счетам


class masiv_din_double suma_nah_661;


class masiv_din_int knv_podoh_661; //коды не вошедшие в расчёт налога
class masiv_din_double knv_podoh_suma_661; //суммы по кодам не вошедшим в растчёт

class masiv_din_double knv_podoh_suma_663; //суммы по кодам не вошедшим в растчёт
knv_podoh_suma_663.make_class(kolih_dop_shet*kolih_nah);

class masiv_din_int knv_esv_661; //коды не вошедшие в расчёт налога
class masiv_din_double knv_esv_suma_661; //суммы по кодам не вошедшим в растчёт

class masiv_din_double knv_esv_suma_663; //суммы по кодам не вошедшим в растчёт
knv_esv_suma_663.make_class(kolih_dop_shet*kolih_nah);

class masiv_din_int knv_pens_661; //коды не вошедшие в расчёт налога
class masiv_din_double knv_pens_suma_661; //суммы по кодам не вошедшим в расчёт


class masiv_din_double knv_pens_suma_663; //суммы по кодам не вошедшим в растчёт
knv_pens_suma_663.make_class(kolih_dop_shet*kolih_nah);

class masiv_din_int knv_profs_661; //коды не вошедшие в расчёт налога
class masiv_din_double knv_profs_suma_661; //суммы по кодам не вошедшим в растчёт

class masiv_din_double knv_profs_suma_663; //суммы по кодам не вошедшим в растчёт
knv_profs_suma_663.make_class(kolih_dop_shet*kolih_nah);

class masiv_din_int knv_socstr_661; //коды не вошедшие в расчёт налога
class masiv_din_double knv_socstr_suma_661; //суммы по кодам не вошедшим в растчёт

class masiv_din_double knv_socstr_suma_663; //суммы по кодам не вошедшим в растчёт
knv_socstr_suma_663.make_class(kolih_dop_shet*kolih_nah);

class masiv_din_int knv_bezrab_661; //коды не вошедшие в расчёт налога
class masiv_din_double knv_bezrab_suma_661; //суммы по кодам не вошедшим в растчёт

class masiv_din_double knv_bezrab_suma_663; //суммы по кодам не вошедшим в растчёт
knv_bezrab_suma_663.make_class(kolih_dop_shet*kolih_nah);
suma_nah_661.make_class(sn661.kolih());
metka=0;
int nomer=0;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4]);
  
  strzag(LINES-1,0,kolstr,++kolstr1);
  
  prn=atoi(row[1]);
  knah=atoi(row[2]);  
  suma=atof(row[3]);
  
  if(prn == 1)  
   {
    if(proverka(shetb,row[4],0,1) == 0)
     continue; 
    
    if((nomer=sn661.find(knah)) >= 0)
      suma_nah_661.plus(suma,nomer);

    for(int ii=0 ; ii < kolih_dop_shet; ii++)
     if(proverka(dop_zar_sheta_spn.ravno(ii),row[2],0,1) == 0)
       {
        sprintf(strsql,"%d|%s",knah,row[4]);
        
        if((nomer=knah_shet_663[ii].find(strsql)) < 0)
         knah_shet_663[ii].plus(strsql);
        suma_knah_shet_663[ii].plus(suma,nomer);
       
       }

    if(provkod(knvr,knah) >= 0) //проверка на коды не входящие в расчёт подоходного налога
     {
      metka=0;
      for(int ii=0 ; ii < kolih_dop_shet; ii++)
       {            
        if(proverka(dop_zar_sheta_spn.ravno(ii),row[2],0,1) == 0)
         {
          nomer=vse_kodi_nah.find(row[2]);
          knv_podoh_suma_663.plus(suma,ii*kolih_nah+nomer);
          metka=1;
         }
        
       }      

      if(metka == 0)
       {
        if((nomer=knv_podoh_661.find(knah)) < 0)
          knv_podoh_661.plus(knah,nomer);
        knv_podoh_suma_661.plus(suma,nomer);
       }
     }

    if(proverka(knvr_esv_r.ravno(),knah,0,1) == 0) //проверка на коды не входящие в расчёт единого социального взноса
     {
      metka=0;
      for(int ii=0 ; ii < kolih_dop_shet; ii++)
       {            
        if(proverka(dop_zar_sheta_spn.ravno(ii),row[2],0,1) == 0)
         {
          nomer=vse_kodi_nah.find(row[2]);
          knv_esv_suma_663.plus(suma,ii*kolih_nah+nomer);
          metka=1;
         }
        
       }      

      if(metka == 0)
       {
        if((nomer=knv_esv_661.find(knah)) < 0)
          knv_esv_661.plus(knah,nomer);
        knv_esv_suma_661.plus(suma,nomer);
       }
     }
/**********************
    if(provkod(kodnvpen,knah) >= 0) //проверка на коды не входящие в расчёт пенсионного
     {
      metka=0;
      for(int ii=0 ; ii < kolih_dop_shet; ii++)
       {            
        if(proverka(dop_zar_sheta_spn.ravno(ii),row[2],0,1) == 0)
         {
          nomer=vse_kodi_nah.find(row[2]);
          knv_pens_suma_663.plus(suma,ii*kolih_nah+nomer);
          metka=1;
         }
       }
      if(metka == 0)
       {
        if((nomer=knv_pens_661.find(knah)) < 0)
          knv_pens_661.plus(knah,nomer);
        knv_pens_suma_661.plus(suma,nomer);
       }
     }
**********************/
    if(provkod(knvrprof,knah) >= 0) //проверка на коды не входящие в расчёт профсоюзного взноса
     {
      metka=0;
      for(int ii=0 ; ii < kolih_dop_shet; ii++)
       {            
        if(proverka(dop_zar_sheta_spn.ravno(ii),row[2],0,1) == 0)
         {
          nomer=vse_kodi_nah.find(row[2]);
          knv_profs_suma_663.plus(suma,ii*kolih_nah+nomer);
          metka=1;
         }
       }
      if(metka == 0)
       {
        if((nomer=knv_profs_661.find(knah)) < 0)
          knv_profs_661.plus(knah,nomer);
        knv_profs_suma_661.plus(suma,nomer);
       }
     }
   }

  if(prn == 2)
   {
    if(proverka(shetbu,row[4],0,1) == 0)
     continue; 
    if(proverka(kodpn_all.ravno(),knah,0,1) == 0)    
//    if(kodpn == knah)
     suma_podoh+=suma;
    
    if(kodpen == knah)
     suma_pens+=suma;
    
    if(kodsocstr == knah)
     suma_socstr+=suma;
    
    if(kodbzr == knah)
     suma_bezrab+=suma;
    
    if(kuprof == knah)
     suma_profs+=suma;
     
    if(knah == kod_esv ||  knah == kod_esv_bol || knah == kod_esv_inv || knah == kod_esv_dog)
     suma_esv+=suma;
   } 
 }

char imaf[64];
FILE *ff;

sprintf(imaf,"doli%d.tmp",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
startfil(ff);
iceb_t_zagolov(gettext("Расчёт частей зарплатных счетов."),0,0,0,0,mr,gr,ff);

class iceb_tu_str naim("");

double itogo_suma_661=0.;

fprintf(ff,"%s %s\n",gettext("Начисления по счёту"),shrpz.ravno());
fprintf(ff,"%s: %d\n",gettext("Количество начислений"),sn661.kolih());
fprintf(ff,"----------------------------------------------------------\n");

for(int i=0; i < sn661.kolih(); i++)
 {
  suma=suma_nah_661.ravno(i);
  if(suma == 0.)
   continue;
  naim.new_plus("");
  //читаем наименование начисления
  sprintf(strsql,"select naik from Nash where kod=%d",sn661.ravno(i));
  if(readkey(strsql,&row,&cur) == 1)
   naim.new_plus(row[0]);     
  fprintf(ff,"%-3d %-*.*s %10.2f\n",
  sn661.ravno(i),
  iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
  suma);
  itogo_suma_661+=suma;  
 }

fprintf(ff,"----------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f\n",iceb_tu_kolbait(34,gettext("Итого")),gettext("Итого"),itogo_suma_661);

double itogo_suma_663[kolih_dop_shet];
memset(itogo_suma_663,'\0',sizeof(itogo_suma_663));
double itogo=0.;
for(int ii=0; ii < kolih_dop_shet; ii++)
 {
  double suma_po_shetu=0.;
  fprintf(ff,"\n%s %s\n",gettext("Начисления по счёту"),dop_zar_sheta.ravno(ii));


  if(knah_shet_663[ii].kolih() > 0)
   {
    fprintf(ff,"%s: %d\n",gettext("Количество начислений"),knah_shet_663[ii].kolih());
    fprintf(ff,"----------------------------------------------------------\n");
    char shet_n[64];
    for(int i=0; i < knah_shet_663[ii].kolih(); i++)
     {
        
      suma=suma_knah_shet_663[ii].ravno(i);
      if(suma == 0.)
       continue;

      polen(knah_shet_663[ii].ravno(i),strsql,sizeof(strsql),1,'|');
      knah=atoi(strsql);

      polen(knah_shet_663[ii].ravno(i),shet_n,sizeof(shet_n),2,'|');
      
      naim.new_plus("");
      //читаем наименование начисления
      sprintf(strsql,"select naik from Nash where kod=%d",knah);
      if(readkey(strsql,&row,&cur) == 1)
       naim.new_plus(row[0]);     
      fprintf(ff,"%-3d %-*s %-*.*s %10.2f\n",
      knah,
      iceb_tu_kolbait(6,shet_n),shet_n,
      iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
      suma);
      
      itogo_suma_663[ii]+=suma;  
      itogo+=suma;
      suma_po_shetu+=suma;
     }
    fprintf(ff,"----------------------------------------------------------\n");

    fprintf(ff,"%*s:%10.2f\n",iceb_tu_kolbait(41,gettext("Итого")),gettext("Итого"),suma_po_shetu);

   }
 }

fprintf(ff,"\n\
%s:%10.2f\n",gettext("Общая сумма начислений по всем счетам"),itogo_suma_661+itogo);

suma_podoh*=-1;
suma_pens*=-1;
suma_profs*=-1;
suma_bezrab*=-1;
suma_socstr*=-1;
suma_esv*=-1;
fprintf(ff,"\n\
%s:%10.2f\n\
%s:%10.2f\n\
%s:%10.2f\n\
%s:%10.2f\n\
%s:%10.2f\n",
gettext("Общая сумма начисления подоходного налога"),suma_podoh,
gettext("Общая сумма начисления в пенсионный фонд"),suma_pens,
gettext("Общая сумма начисления профсоюзного отчисления"),suma_profs,
gettext("Общая сумма начисления в фонд безработицы"),suma_bezrab,
gettext("Общая сумма начисления в фонд соц-страха"),suma_socstr);

fprintf(ff,"\n***************%s**************\n",gettext("Подоходный налог"));
dolizs_rs(&knv_podoh_661,&knv_podoh_suma_661,&knv_podoh_suma_663,itogo_suma_661,itogo_suma_663,suma_podoh,&dop_zar_sheta,&dop_zar_sheta_spn,&vse_kodi_nah,ff);


fprintf(ff,"\n***************%s**************\n",gettext("Профсоюзный взнос"));
dolizs_rs(&knv_profs_661,&knv_profs_suma_661,&knv_profs_suma_663,itogo_suma_661,itogo_suma_663,suma_profs,&dop_zar_sheta,&dop_zar_sheta_spn,&vse_kodi_nah,ff);

if(sravmydat(1,mr,gr,ICEB_DNDESV,ICEB_MNDESV,ICEB_GNDESV) < 0)
 {
  fprintf(ff,"\n***************%s**************\n",gettext("Пенсионный фонд"));
  dolizs_rs(&knv_pens_661,&knv_pens_suma_661,&knv_pens_suma_663,itogo_suma_661,itogo_suma_663,suma_pens,&dop_zar_sheta,&dop_zar_sheta_spn,&vse_kodi_nah,ff);

  fprintf(ff,"\n***************%s**************\n",gettext("Соц. страхование (с работника)"));
  dolizs_rs(&knv_socstr_661,&knv_socstr_suma_661,&knv_socstr_suma_663,itogo_suma_661,itogo_suma_663,suma_socstr,&dop_zar_sheta,&dop_zar_sheta_spn,&vse_kodi_nah,ff);

  fprintf(ff,"\n***************%s**************\n",gettext("Фонд безработицы"));
  dolizs_rs(&knv_bezrab_661,&knv_bezrab_suma_661,&knv_bezrab_suma_663,itogo_suma_661,itogo_suma_663,suma_bezrab,&dop_zar_sheta,&dop_zar_sheta_spn,&vse_kodi_nah,ff);
 }
else
 {
  fprintf(ff,"\n***************%s**************\n",gettext("Единый социальный взнос"));
  dolizs_rs(&knv_esv_661,&knv_esv_suma_661,&knv_esv_suma_663,itogo_suma_661,itogo_suma_663,suma_esv,&dop_zar_sheta,&dop_zar_sheta_spn,&vse_kodi_nah,ff);
 }
podpis(ff);

fclose(ff);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт частей зарплатных счетов"));
return(0);
}

/*****************************/
/*Расчёт по долям*/
/**********************/
void dolizs_rs(class masiv_din_int *knv_nalog_661, /*Коды не входящие в расчёт для налога*/
class masiv_din_double *knv_nalog_suma_661, /*Суммы не входящие в расчёт для основного счёта*/
class masiv_din_double *knv_nalog_suma_663, /*Суммы не входящие в расчёт для дополнительных счетов*/
double itogo_suma_661, /*Общая сумма начисления для главного счёта 661 вместе с кодами не входящими в расчёт*/
double *itogo_suma_663, /*Общие суммы начислений для дополнительных счетов вместе с кодами не входящими в расчёт*/
double suma_nalog,      /*Общая сумма налога*/
class SPISOK *dop_zar_sheta, /*Список дополнительных зарплатных счетов*/
class SPISOK *dop_zar_sheta_spn, /*Список начислений для которых применяются дополнительные зарплатные счета*/
class masiv_din_int *vse_kodi_nah, /*Список всех кодов начислений*/
FILE *ff)
{
double suma=0.;
class iceb_tu_str naim("");
char strsql[512];
SQL_str row;
SQLCURSOR cur;

int kolih_dop_shet=dop_zar_sheta->kolih();

double suma_po_dop_sh=0;
for(int ii=0; ii < kolih_dop_shet; ii++)
 suma_po_dop_sh+=itogo_suma_663[ii];
double itogo_nah=itogo_suma_661+suma_po_dop_sh;

fprintf(ff,"%s:%.2f\n%s:%.2f\n",
gettext("Общая сумма всех начислений"),
itogo_nah,
gettext("Общая сумма налога"),
suma_nalog);

double itogo_knv_nalog_661=0.;
if(knv_nalog_661->kolih() > 0)
 {
  fprintf(ff,"%s %s\n",gettext("Счёт"),shrpz.ravno());
  fprintf(ff,"%s:\n",gettext("Коды начислений, с которых не начислялся налог"));
  for(int i=0; i < knv_nalog_661->kolih(); i++)
   {
    suma=knv_nalog_suma_661->ravno(i);
    naim.new_plus("");
    //читаем наименование начисления
    sprintf(strsql,"select naik from Nash where kod=%d",knv_nalog_661->ravno(i));
    if(readkey(strsql,&row,&cur) == 1)
     naim.new_plus(row[0]);     
    fprintf(ff,"%-3d %-*.*s %10.2f\n",
    knv_nalog_661->ravno(i),
    iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
    suma);

    itogo_knv_nalog_661+=suma;
   }
  fprintf(ff,"%34s:%10.2f\n",gettext("Итого"),itogo_knv_nalog_661);
 }
double suma_vr_661=itogo_suma_661-itogo_knv_nalog_661;

fprintf(ff,"\n%s %s -> %.2f-%.2f=%.2f\n",
gettext("Сумма с которой брался налог для счёта"),
shrpz.ravno(),itogo_suma_661,itogo_knv_nalog_661,suma_vr_661);




itogo_suma_661-=itogo_knv_nalog_661;

double itogo_knv_nalog_663=0.;
int kolih_nah=vse_kodi_nah->kolih();
double suma_vr_dopsh[kolih_dop_shet];
memset(suma_vr_dopsh,'\0',sizeof(suma_vr_dopsh));
double i_suma_vr_dopsh=0.;
for(int ii=0; ii < kolih_dop_shet; ii++)
 {
  fprintf(ff,"\n%s %s\n-------------------------------------\n",gettext("Счёт"),dop_zar_sheta->ravno(ii));
  fprintf(ff,"%s:\n",gettext("Коды начислений, с которых не начислялся налог"));

  itogo_knv_nalog_663=0.;

  for(int i=0; i < kolih_nah; i++)
   {
    /*Проверяем есть ли это начисление в списке для этого счёта, чтобы не отнимать сумму не входящюю в расчёт*/
    if(proverka(dop_zar_sheta_spn->ravno(ii),vse_kodi_nah->ravno(i),0,1) != 0)
     continue;

    if((suma=knv_nalog_suma_663->ravno(ii*kolih_nah+i)) == 0.)
     continue;
  
    naim.new_plus("");
    //читаем наименование начисления
    sprintf(strsql,"select naik from Nash where kod=%d",vse_kodi_nah->ravno(i));
    if(readkey(strsql,&row,&cur) == 1)
     naim.new_plus(row[0]);     
    fprintf(ff,"%-3d %-*.*s %10.2f\n",
    vse_kodi_nah->ravno(i),
    iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
    suma);
    
    itogo_knv_nalog_663+=suma;
   }
  fprintf(ff,"%*s:%10.2f\n",iceb_tu_kolbait(34,gettext("Итого")),gettext("Итого"),itogo_knv_nalog_663);

  suma_vr_dopsh[ii]+=itogo_suma_663[ii]-itogo_knv_nalog_663;
  i_suma_vr_dopsh+=suma_vr_dopsh[ii];
  fprintf(ff,"Сумма с которой брался налог для %s счёта -> %.2f-%.2f=%.2f\n",
  dop_zar_sheta->ravno(ii),itogo_suma_663[ii],itogo_knv_nalog_663,suma_vr_dopsh[ii]);
 }


fprintf(ff,"\n%s:\n\
---------------------------------------------------------\n\
%*s %10.2f\n",
gettext("Общая сумма взятая в расчёт по всем счетам"),
iceb_tu_kolbait(6,shrpz.ravno()),shrpz.ravno(),
suma_vr_661);

for(int ii=0; ii < kolih_dop_shet; ii++)
 fprintf(ff,"%*s %10.2f\n",iceb_tu_kolbait(6,dop_zar_sheta->ravno(ii)),dop_zar_sheta->ravno(ii),suma_vr_dopsh[ii]);

fprintf(ff,"\
---------------------------------------------------------\n");

double suma_vr=i_suma_vr_dopsh+suma_vr_661;
fprintf(ff,"%*s %10.2f\n",iceb_tu_kolbait(6,gettext("Итого")),gettext("Итого"),suma_vr);

fprintf(ff,"\n%s:\n",gettext("Расчитываем доли налога по счетам"));

fprintf(ff,"%s %*s",gettext("Счёт"),iceb_tu_kolbait(6,shrpz.ravno()),shrpz.ravno());
double nalog=suma_vr_661*suma_nalog/suma_vr;
nalog=okrug(nalog,0.01);

fprintf(ff," %.2f*%.2f/%.2f=%.2f\n",suma_vr_661,suma_nalog,suma_vr,nalog);
double inalog=nalog;
for(int ii=0; ii < kolih_dop_shet; ii++)
 {
  fprintf(ff,"%s %*s",gettext("Счёт"),iceb_tu_kolbait(6,dop_zar_sheta->ravno(ii)),dop_zar_sheta->ravno(ii));
  nalog=suma_vr_dopsh[ii]*suma_nalog/suma_vr;
  nalog=okrug(nalog,0.01);
  inalog+=nalog;
  fprintf(ff," %.2f*%.2f/%.2f=%.2f\n",suma_vr_dopsh[ii],suma_nalog,suma_vr,nalog);
 }
fprintf(ff,"%s:%.2f\n",gettext("Разом"),inalog);

}
