/*$Id: uplovdt.c,v 5.3 2013/08/13 05:49:58 sasa Exp $*/
/*04.05.2012	27.04.2003	Белых А.И.	uplovdt.c
Распечатать оборотную ведомость движения топлива по автомобилям
*/
#include        <errno.h>
#include        "buhg.h"

void hpov(short,short,short,short,short,short,FILE*);
void uplovdt_n(short dn,short mn,short gn,short dk,short mk,short gk,const char *kodvod,const char *kodavt,const char *podr,const char *nadp,FILE *ff);


int uplovdt(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *kodavt,
const char *kodvod,
const char *podr,
class spis_oth *oth)
{
char	strsql[1024];
int	kolstr=0;
SQL_str row;
char	imaf[64];
FILE    *ff;
time_t  tmmn;
class SQLCURSOR curr;

time(&tmmn);

clear();

printw("%s\n",gettext("Оборотная ведомость движения топлива по автомобилям"));
printw("%d.%d.%d => %d.%d.%d\n",dn,mn,gn,dk,mk,gk);
GDITE();

//Определяем список автомобилей

sprintf(strsql,"select distinct kv,ka from Upldok1 where datd >= '%d-%d-%d' and \
datd <= '%d-%d-%d' order by ka,kv asc",gn,mn,dn,gk,mk,dk);

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
if(kolstr == 0)
 {
  iceb_t_soob(gettext("Нет записей по заданным реквизитам поиска !"));
  return(1);
 }  
class SPISOK KVKA;  //код водителя|код автомобиля
class masiv_din_int kod_avt;  //коды автомобилей
#define OST_AVTOMOBIL 5
class masiv_din_double ost_avt[OST_AVTOMOBIL]; //остатки по автомобилям

class masiv_din_int kod_vod;  //коды водителей
#define OST_VODITEL 5
class masiv_din_double ost_vod[OST_VODITEL]; //остатки по водителям


int kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s\n",row[0],row[1]);
  
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(kodvod,row[0],0,0) != 0)
   continue;

  if(proverka(kodavt,row[1],0,0) != 0)
   continue;

  sprintf(strsql,"%s|%s",row[0],row[1]);
  KVKA.plus(strsql);
  
  if(kod_avt.find(atoi(row[1])) < 0)
   kod_avt.plus(atoi(row[1]),-1);

  if(kod_vod.find(atoi(row[0])) < 0)
   kod_vod.plus(atoi(row[0]),-1);
 }

kolstr1=kod_avt.kolih();

for(int i=0 ; i < OST_AVTOMOBIL; i++)
 ost_avt[i].make_class(kolstr1);

kolstr1=kod_vod.kolih();

for(int i=0 ; i < OST_VODITEL; i++)
 ost_vod[i].make_class(kolstr1);

sprintf(imaf,"rpl%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }


uplovdt_n(dn,mn,gn,dk,mk,gk,kodvod,kodavt,podr,gettext("Оботротная ведомость движения топлива по автомобилям"),ff);



hpov(dn,mn,gn,dk,mk,gk,ff);

int kol=KVKA.kolih();
class iceb_tu_str kodvd("");
class iceb_tu_str kodav("");
class iceb_tu_str naimvod("");
class iceb_tu_str naimavt("");
class iceb_tu_str rnavt("");
double kolih=0.;


double dvig[4];
memset(&dvig,'\0',sizeof(dvig));
double idvig[4];
memset(&idvig,'\0',sizeof(idvig));
int nomer_avt=0;
class iceb_tu_str nomdok("");
double peredano=0.;
double iperedano=0.;
class iceb_tu_str otmet("");
int kolstr2=0;
for(int nomer=0; nomer < kol; nomer++)
 {

  polen(KVKA.ravno(nomer),&kodvd,1,'|');
  polen(KVKA.ravno(nomer),&kodav,2,'|');

  printw("\n%s=%s %s=%s\n",
  gettext("Код водителя"),
  kodvd.ravno(),
  gettext("Код автомобиля"),
  kodav.ravno());

  strzag(LINES-1,0,kol,++kolstr2);
  
//  printw("%s %s\n",kodav.ravno(),kodvd.ravno());
  //Читаем водителя
  naimvod.new_plus(kodvd.ravno());
  sprintf(strsql,"select naik from Uplouot where kod=%s",kodvd.ravno());
//  printw("%s\n",strsql);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)  
   {
    naimvod.plus(" ",row[0]);
   }
  //Читаем автомобиль
  naimavt.new_plus(kodav.ravno());
  sprintf(strsql,"select nomz,naik from Uplavt where kod=%s",kodav.ravno());
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)  
   {
    rnavt.new_plus(row[0]);
    naimavt.plus(" ",row[1]);
   }
  else
   rnavt.new_plus("");
  sprintf(strsql,"select kp,tz,kolih,datd,nomd,nz,kap from Upldok1 where datd >= '%d-%d-%d' \
and datd <= '%d-%d-%d' and kv=%s and ka=%s order by datd,nomd asc",
  gn,mn,dn,gk,mk,dk,kodvd.ravno(),kodav.ravno());

  if((kolstr1=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }
  memset(&dvig,'\0',sizeof(dvig));



  nomdok.new_plus("");
  int sh_str=0;
  while(cur.read_cursor(&row) != 0)
   {
    if(sh_str == 0)
     {
      dvig[0]=ostva(dn,mn,gn,kodvd.ravno(),kodav.ravno(),atoi(row[5]),NULL,NULL,NULL);
      sh_str=1;
     }
//    printw("%s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4]);
    if(proverka(podr,row[0],0,0) != 0)
     continue;
    kolih=atof(row[2]);               
    if(row[1][0] == '1')
     {
      if(atoi(row[6]) != 0)
        dvig[1]+=kolih;      
      else
        dvig[2]+=kolih;      
     }
    else
     {
      dvig[3]+=kolih;      
     }
    if(SRAV(nomdok.ravno(),row[4],0) != 0)
     {
      otmet.new_plus("");
      nomdok.new_plus(row[4]); 
     }    
   }

  peredano=upl_pt(dn,mn,gn,dk,mk,gk,kodav.ravno(),kodvd.ravno());
  iperedano+=peredano;
  
  kolih=dvig[0]+dvig[1]+dvig[2]-dvig[3]-peredano;

  nomer_avt=kod_avt.find(atoi(kodav.ravno()));
  
  ost_avt[0].plus(dvig[0],nomer_avt);
  ost_avt[1].plus(dvig[1],nomer_avt);
  ost_avt[2].plus(dvig[2],nomer_avt);
  ost_avt[3].plus(dvig[3],nomer_avt);
  ost_avt[4].plus(peredano,nomer_avt);

  nomer_avt=kod_vod.find(kodvd.ravno_atoi());

  ost_vod[0].plus(dvig[0],nomer_avt);
  ost_vod[1].plus(dvig[1],nomer_avt);
  ost_vod[2].plus(dvig[2],nomer_avt);
  ost_vod[3].plus(dvig[3],nomer_avt);
  ost_vod[4].plus(peredano,nomer_avt);

  
  fprintf(ff,"%-*.*s %-*.*s %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %s\n",
  iceb_tu_kolbait(15,naimavt.ravno()),iceb_tu_kolbait(15,naimavt.ravno()),naimavt.ravno(),
  iceb_tu_kolbait(15,rnavt.ravno()),iceb_tu_kolbait(15,rnavt.ravno()),rnavt.ravno(),
  iceb_tu_kolbait(20,naimvod.ravno()),iceb_tu_kolbait(20,naimvod.ravno()),naimvod.ravno(),
  dvig[0],dvig[1],dvig[2],dvig[3],peredano,kolih,otmet.ravno());

  
  idvig[0]+=dvig[0];
  idvig[1]+=dvig[1];
  idvig[2]+=dvig[2];
  idvig[3]+=dvig[3];


 }
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------\n");

kolih=idvig[0]+idvig[1]+idvig[2]-idvig[3]-iperedano;

fprintf(ff,"\
%*s:%10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
iceb_tu_kolbait(52,gettext("Итого")),gettext("Итого"),
idvig[0],idvig[1],idvig[2],idvig[3],iperedano,kolih);


podpis(ff);
fclose(ff);



//Распечатка только по автомобилям
char imaf_avt[64];

sprintf(imaf_avt,"ostavt%d.lst",getpid());

if((ff = fopen(imaf_avt,"w")) == NULL)
 {
  error_op_nfil(imaf_avt,errno,"");
  return(1);
 }

uplovdt_n(dn,mn,gn,dk,mk,gk,kodvod,kodavt,podr,gettext("Оботротная ведомость движения топлива по автомобилям"),ff);




fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Код |           Автомобиль          | Остаток  |    Движение топлива за период             | Остаток  |\n\
авт.|      Марка    |Государс. номер|%02d.%02d.%04d| Принято  | Получено |Потрачено | Передано |%02d.%02d.%04d|\n"),
dn,mn,gn,dk,mk,gk);

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");




kol=kod_avt.kolih();
int kavt;

for(int nomer=0; nomer < kol; nomer++)
 {
  kavt=kod_avt.ravno(nomer);
  //Читаем автомобиль
  
  sprintf(strsql,"select nomz,naik from Uplavt where kod=%d",kavt);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)  
   {
    rnavt.new_plus(row[0]);
    naimavt.new_plus(row[1]);
   }
  else
   {
    rnavt.new_plus("");
    naimavt.new_plus("");
   }
  kolih=ost_avt[0].ravno(nomer)+ost_avt[1].ravno(nomer)+ost_avt[2].ravno(nomer)-\
  ost_avt[3].ravno(nomer)-ost_avt[4].ravno(nomer);

  fprintf(ff,"%4d %-*s %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  kavt,
  iceb_tu_kolbait(15,naimavt.ravno()),naimavt.ravno(),
  iceb_tu_kolbait(15,rnavt.ravno()),iceb_tu_kolbait(15,rnavt.ravno()),rnavt.ravno(),
  ost_avt[0].ravno(nomer),
  ost_avt[1].ravno(nomer),
  ost_avt[2].ravno(nomer),
  ost_avt[3].ravno(nomer),
  ost_avt[4].ravno(nomer),
  kolih);


 }


podpis(ff);
fclose(ff);


//Распечатка только по водителям
char imaf_vod[64];

sprintf(imaf_vod,"ostvod%d.lst",getpid());

if((ff = fopen(imaf_vod,"w")) == NULL)
 {
  error_op_nfil(imaf_vod,errno,"");
  return(1);
 }

uplovdt_n(dn,mn,gn,dk,mk,gk,kodvod,kodavt,podr,gettext("Оботротная ведомость движения топлива по водителям"),ff);


fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Код |       Фамилия водителя        | Остаток  |           Движение топлива за период      | Остаток  |\n\
вод.|                               |%02d.%02d.%04d| Принято  | Получено |Потрачено | Передано |%02d.%02d.%04d|\n"),
dn,mn,gn,dk,mk,gk);
fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");



kol=kod_vod.kolih();

for(int nomer=0; nomer < kol; nomer++)
 {
  kavt=kod_vod.ravno(nomer);

  //Читаем водителя
  sprintf(strsql,"select naik from Uplouot where kod=%d",kavt);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)  
   {
    naimvod.new_plus(row[0]);
   }
  else
   naimvod.new_plus("");
  kolih=ost_vod[0].ravno(nomer)+ost_vod[1].ravno(nomer)+ost_vod[2].ravno(nomer)-ost_vod[3].ravno(nomer)-ost_vod[4].ravno(nomer);

  fprintf(ff,"%4d %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  kavt,
  iceb_tu_kolbait(31,naimvod.ravno()),iceb_tu_kolbait(31,naimvod.ravno()),naimvod.ravno(),
  ost_vod[0].ravno(nomer),
  ost_vod[1].ravno(nomer),
  ost_vod[2].ravno(nomer),
  ost_vod[3].ravno(nomer),
  ost_vod[4].ravno(nomer),
  kolih);


 }


podpis(ff);
fclose(ff);


printw_vr(tmmn);


OSTANOV();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Оборотная ведомость движения топлива по водителям и автомобилям"));

oth->spis_imaf.plus(imaf_avt);
oth->spis_naim.plus(gettext("Оборотная ведомость движения топлива по автомобилям"));

oth->spis_imaf.plus(imaf_vod);
oth->spis_naim.plus(gettext("Оборотная ведомость движения топлива по водителям"));



for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),0);

return(0);

}
/**************************/
/*Шапка                   */
/**************************/

void hpov(short dn,short mn,short gn,
short dk,short mk,short gk,
FILE *ff)
{
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
           Автомобиль          |      Водитель      | Остаток  |           Движение топлива за период      | Остаток  |\n\
      Марка    |Государс. номер|                    |%02d.%02d.%04d| Принято  | Получено |Потрачено | Передано |%02d.%02d.%04d|\n"),
dn,mn,gn,dk,mk,gk);
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------\n");


}
/******************/
/*Надпись         */
/******************/

void uplovdt_n(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *kodvod,
const char *kodavt,
const char *podr,
const char *nadp,
FILE *ff)
{
iceb_t_zagolov(nadp,dn,mn,gn,dk,mk,gk,ff);

if(kodvod[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Водитель"),kodvod);
if(kodavt[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Автомобиль"),kodavt);
if(podr[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Подразделение"),podr);
}
