/*$Id: upl_obv.c,v 5.23 2013/08/13 05:49:58 sasa Exp $*/
/*03.11.2009	27.06.2005	Белых А.И.	upl_obv.c
Оборотнаые ведомости движения топлива по автомобилям и водителям
*/

#include        <errno.h>
#include        "buhg.h"
#define KOLIH_KOLONOK 5 //Количество вычисляемых колонок в отчёте. конечный остаток вычисляется по предыдущим колонкам
#define KOLIH_KOLONOK_MT 3 //Количество вычисляемых колонок в отчёте для марок топлива

void  upl_obv_rs(int metka,
short dn,short mn,short gn,
short dk,short mk,short gk,
masiv_din_int *kod,double suma[][KOLIH_KOLONOK],
const char*,const char*,
const char *imaf);

void  upl_obv_rs_mt(short dn,short mn,short gn,
short dk,short mk,short gk,
SPISOK *kod,
double suma[][KOLIH_KOLONOK_MT],
char *imaf);

extern short	startgodupl; //Стартовый год для "Учета путевых листов"

int upl_obv(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *kodavt,
const char *kodvod,
class spis_oth *oth)
{
GDITE();
char strsql[512];
int kolstr;
SQL_str row;
SQLCURSOR cur;
masiv_din_int m_kod_avt;
masiv_din_int m_kod_vod;
SPISOK m_kod_mt;

short godn=startgodupl;
if(godn > gn || godn == 0)
 godn=gn;
 
sprintf(strsql,"select kod from Uplavt");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

double m_suma_avt[kolstr][KOLIH_KOLONOK];
memset(m_suma_avt,'\0',sizeof(m_suma_avt));

while(cur.read_cursor(&row) != 0)
 m_kod_avt.plus(atoi(row[0]),-1);

sprintf(strsql,"select kod from Uplouot where mt=1");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

double m_suma_vod[kolstr][KOLIH_KOLONOK];
memset(m_suma_vod,'\0',sizeof(m_suma_vod));
while(cur.read_cursor(&row) != 0)
 m_kod_vod.plus(atoi(row[0]),-1);

sprintf(strsql,"select kodt from Uplmt");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

double m_suma_mt[kolstr][KOLIH_KOLONOK_MT];
memset(m_suma_mt,'\0',sizeof(m_suma_mt));
while(cur.read_cursor(&row) != 0)
 m_kod_mt.plus(row[0]);


//смотрим сальдо стартовое по машинам
sprintf(strsql,"select ka,kv,sal,kmt from Uplsal where god=%d",godn);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
double kolih;
while(cur.read_cursor(&row) != 0)
 {
//  if(atoi(row[0]) == 1)
//   printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
  kolih=atof(row[2]);
  
  //вообщето он однозначно должен быть в списке
  if(m_kod_avt.find(row[0]) < 0)
    m_kod_avt.plus(row[0],-1);    
  
  m_suma_avt[m_kod_avt.find(row[0])][0]+=kolih;

  //вообщето он однозначно должен быть в списке
  if(m_kod_vod.find(row[1]) < 0)
    m_kod_vod.plus(row[1],-1);    
  
  m_suma_vod[m_kod_vod.find(row[1])][0]+=kolih;
  
  m_suma_mt[m_kod_mt.find_r(row[3])][0]+=kolih;
 }

sprintf(strsql,"select datd,ka,kv,kodsp,tz,kolih,kap,kodtp from Upldok1 where \
datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' order by datd asc",
godn,1,1,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
short d,m,g;
int tipz;
int kap;
int nomer_avt;
int nomer_vod;
int kolstr1=0;
int nomer_mt=0;

while(cur.read_cursor(&row) != 0)
 {
//  if(atoi(row[1]) == 1)
//   printw("%s %s %s %s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7]);
  
  strzag(LINES-1,0,kolstr,++kolstr1);
  
  tipz=atoi(row[4]);
  
  kolih=atof(row[5]);
  
  kap=atoi(row[6]);
  
  rsdat(&d,&m,&g,row[0],2);  

  nomer_avt=m_kod_avt.find(row[1]);
  nomer_vod=m_kod_vod.find(row[2]);
  nomer_mt=m_kod_mt.find_r(row[7]);

  if(sravmydat(d,m,g,dn,mn,gn) < 0)//До периода
   {
    if(tipz == 1)
     {
      m_suma_vod[nomer_vod][0]+=kolih;
      m_suma_avt[nomer_avt][0]+=kolih;
      if(kap == 0)
       m_suma_mt[nomer_mt][0]+=kolih;
     }
    if(tipz == 2)
     {
      m_suma_vod[nomer_vod][0]-=kolih;
      m_suma_avt[nomer_avt][0]-=kolih;
      m_suma_mt[nomer_mt][0]-=kolih;
     }

    if(kap != 0) //Передача топлива
     {
      nomer_avt=m_kod_avt.find(kap);
      nomer_vod=m_kod_vod.find(row[3]);

      m_suma_vod[nomer_vod][0]-=kolih;
      m_suma_avt[nomer_avt][0]-=kolih;
      
     }       
    continue;
   }   

  //Период
  if(tipz == 1) //Приход
   {
    if(kap == 0) //Чистый приход
     {
      m_suma_vod[nomer_vod][2]+=kolih;
      m_suma_avt[nomer_avt][2]+=kolih;
      m_suma_mt[nomer_mt][1]+=kolih;
     }  
    else //Передача топлива
     {
      m_suma_vod[nomer_vod][1]+=kolih;
      m_suma_avt[nomer_avt][1]+=kolih;

      //Записываем от какого автомобиля и какого водителя ушло
       
      nomer_avt=m_kod_avt.find(kap);
      nomer_vod=m_kod_vod.find(row[3]);

      m_suma_vod[nomer_vod][4]+=kolih;
      m_suma_avt[nomer_avt][4]+=kolih;
  
     }
     
   }

  if(tipz == 2) //Расход
   {
    m_suma_vod[nomer_vod][3]+=kolih;
    m_suma_avt[nomer_avt][3]+=kolih;
    m_suma_mt[nomer_mt][2]+=kolih;
   }
    
 }



//Распечатываем по автомобилям

char imaf_avt[64];
sprintf(imaf_avt,"ovdt_a%d.lst",getpid());

upl_obv_rs(0,dn,mn,gn,dk,mk,gk,&m_kod_avt,m_suma_avt,kodvod,kodavt,imaf_avt);

char imaf_vod[64];
sprintf(imaf_vod,"ovdt_v%d.lst",getpid());

upl_obv_rs(1,dn,mn,gn,dk,mk,gk,&m_kod_vod,m_suma_vod,kodvod,kodavt,imaf_vod);


char imaf_mt[64];
sprintf(imaf_mt,"ovdt_m%d.lst",getpid());
upl_obv_rs_mt(dn,mn,gn,dk,mk,gk,&m_kod_mt,m_suma_mt,imaf_mt);

oth->spis_imaf.plus(imaf_vod);
oth->spis_naim.plus(gettext("Оборотная ведомость по водителям"));
oth->spis_imaf.plus(imaf_avt);
oth->spis_naim.plus(gettext("Оборотная ведомость по автомобилям"));
oth->spis_imaf.plus(imaf_mt);
oth->spis_naim.plus(gettext("Оборотная ведомость по маркам топлива"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),0);
 
return(0);

}
/********************************************/
/*Распечатка массивов*/
/**********************************/
void  upl_obv_rs(int metka, //0- по автомобилям 1-по водителям
short dn,short mn,short gn,
short dk,short mk,short gk,
masiv_din_int *kod,
double suma[][KOLIH_KOLONOK],
const char *kodvod,
const char *kodavt,
const char *imaf)
{

FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }
 

char strsql[1024];
if(metka == 0)
 strcpy(strsql,gettext("Оборотная ведомость движения топлива по автомобилям"));
if(metka == 1)
 strcpy(strsql,gettext("Оборотная ведомость движения топлива по водителям"));

iceb_t_zagolov(strsql,dn,mn,gn,dk,mk,gk,ff);

if(metka == 0)
 {
  if(kodavt[0] != '\0')
   fprintf(ff,"%s:%s\n",gettext("Код автомобиля"),kodavt);
  fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");
  fprintf(ff,gettext("\
Код |           Автомобиль          | Остаток  |    Движение топлива за период             | Остаток  |\n\
авт.|      Марка    |Государс. номер|%02d.%02d.%04d| Принято  | Получено |Потрачено | Передано |%02d.%02d.%04d|\n"),
  dn,mn,gn,dk,mk,gk);
/*********
1234 123456789012345 123456789012345 1234567890     1234567890
*************/
  fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");
 }

if(metka == 1)
 {
  if(kodvod[0] != '\0')
   fprintf(ff,"%s:%s\n",gettext("Код водителя"),kodvod);
  fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Код |       Фамилия водителя        | Остаток  |           Движение топлива за период      | Остаток  |\n\
вод.|                               |%02d.%02d.%04d| Принято  | Получено |Потрачено | Передано |%02d.%02d.%04d|\n"),
  dn,mn,gn,dk,mk,gk);
/******
1234 123456789012345678901234567890
*************/
fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");
 }



double ostatok;
int metka_zero;
class iceb_tu_str naim("");
SQL_str row;
SQLCURSOR cur;
double itogo[6];
memset(&itogo,'\0',sizeof(itogo));

for(int i=0; i < kod->kolih(); i++)
 {
  metka_zero=0;
  for(int ii=0; ii < KOLIH_KOLONOK ; ii++)
    if(suma[i][ii] != 0)
     {
      metka_zero=1;
      break;
     }    
  if(metka_zero == 0)
    continue;   

  
  if(metka == 0)
   {
    if(proverka(kodavt,kod->ravno_char(i),0,0) != 0)
     continue;
    sprintf(strsql,"select nomz,naik from Uplavt where kod=%d",kod->ravno(i)); 
    if(readkey(strsql,&row,&cur) == 1)
     {
//      sprintf(naim,"%-15.15s %-15.15s",row[1],row[0]);
      naim.new_plus(row[1]);
      naim.plus(" ",row[0]);
     }
    else
     naim.new_plus("");
   }
  if(metka == 1)
   {
    if(proverka(kodvod,kod->ravno_char(i),0,0) != 0)
     continue;
    sprintf(strsql,"select naik from Uplouot where kod=%d",kod->ravno(i)); 
    if(readkey(strsql,&row,&cur) == 1)
      naim.new_plus(row[0]);
    else
      naim.new_plus("");
   }

  suma[i][0]=okrug(suma[i][0],0.001);
  suma[i][1]=okrug(suma[i][1],0.001);
  suma[i][2]=okrug(suma[i][2],0.001);
  suma[i][3]=okrug(suma[i][3],0.001);
  suma[i][4]=okrug(suma[i][4],0.001);
  
  ostatok=suma[i][0]+suma[i][1]+suma[i][2]-suma[i][3]-suma[i][4];
  ostatok=okrug(ostatok,0.001);
  
  fprintf(ff,"%4d %-*.*s %10.6g %10.6g %10.6g %10.6g %10.6g %10.6g\n",
  kod->ravno(i),
  iceb_tu_kolbait(31,naim.ravno()),iceb_tu_kolbait(31,naim.ravno()),naim.ravno(),
  suma[i][0],suma[i][1],suma[i][2],suma[i][3],suma[i][4],ostatok);

  itogo[0]+=suma[i][0];
  itogo[1]+=suma[i][1];
  itogo[2]+=suma[i][2];
  itogo[3]+=suma[i][3];
  itogo[4]+=suma[i][4];
  itogo[5]+=ostatok;

 }

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"%4s %*.*s %10.6g %10.6g %10.6g %10.6g %10.6g %10.6g\n",
"",
iceb_tu_kolbait(31,gettext("Итого")),
iceb_tu_kolbait(31,gettext("Итого")),
gettext("Итого"),itogo[0],itogo[1],itogo[2],itogo[3],itogo[4],itogo[5]);

podpis(ff);


fclose(ff);


}
/******************************/
/*Распечатка по маркам топлива*/
/*******************************/
void  upl_obv_rs_mt(short dn,short mn,short gn,
short dk,short mk,short gk,
SPISOK *kod,
double suma[][KOLIH_KOLONOK_MT],
char *imaf)
{

FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }
 

char strsql[1024];

iceb_t_zagolov(gettext("Оборотная ведомость движения по маркам топлива"),dn,mn,gn,dk,mk,gk,ff);

fprintf(ff,"\
------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
   Код  |    Наименование топлива      | Остаток  |Движение за период   | Остаток  |\n\
топлива |                              |%02d.%02d.%04d| Получено |Потрачено |%02d.%02d.%04d|\n"),
dn,mn,gn,dk,mk,gk);

/******
12345678 123456789012345678901234567890
*************/
fprintf(ff,"\
------------------------------------------------------------------------------------\n");



double ostatok;
int metka_zero;
class iceb_tu_str naim("");
SQL_str row;
SQLCURSOR cur;
double itogo[4];
memset(&itogo,'\0',sizeof(itogo));

for(int i=0; i < kod->kolih(); i++)
 {
  metka_zero=0;
  for(int ii=0; ii < KOLIH_KOLONOK_MT ; ii++)
    if(suma[i][ii] != 0)
     {
      metka_zero=1;
      break;
     }    
  if(metka_zero == 0)
    continue;   

  
  naim.new_plus("");
  sprintf(strsql,"select kodm from Uplmt where kodt='%s'",kod->ravno(i)); 
  if(readkey(strsql,&row,&cur) == 1)
   {
    sprintf(strsql,"select naimat from Material where kodm=%s",row[0]); 
    if(readkey(strsql,&row,&cur) == 1)
      naim.new_plus(row[0]);
   }
  ostatok=suma[i][0]+suma[i][1]-suma[i][2];

  fprintf(ff,"%-8s %-*.*s %10.6g %10.6g %10.6g %10.6g\n",
  kod->ravno(i),
  iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
  suma[i][0],suma[i][1],suma[i][2],ostatok);

  itogo[0]+=suma[i][0];
  itogo[1]+=suma[i][1];
  itogo[2]+=suma[i][2];
  itogo[3]+=ostatok;

 }

fprintf(ff,"\
------------------------------------------------------------------------------------\n");

fprintf(ff,"%8s %*.*s %10.6g %10.6g %10.6g %10.6g\n",
"",
iceb_tu_kolbait(30,gettext("Итого")),
iceb_tu_kolbait(30,gettext("Итого")),
gettext("Итого"),itogo[0],itogo[1],itogo[2],itogo[3]);

podpis(ff);


fclose(ff);


}
