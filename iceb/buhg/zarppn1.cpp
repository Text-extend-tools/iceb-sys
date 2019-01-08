/*$Id:$*/
/*02.09.2016	25.02.2011	Белых А.И.	zarpnn1.c
Перерасчёт подоходного налога начиная с 1.1.2011 (введён единый социальный взнос)
*/
#include <math.h>
#include        <errno.h>
#include        "buhg.h"

struct ppnal1
 {
  int   tabn;      //табельный номер
  char   fio[512];  //фамилия
  double osuma[2][12]; //общая сумма начислений
  double suma_nvrpn[2][12]; //сумма не входящая в расчёт для подоходного налога (общая)
  double zarpl[2][12]; //сумма зарплаты 
  double sumalgot[2][12]; // сумма социальных льгот
  double podnal[12];   // сумма подоходного налога  
  double sumasoc[12];  //сумма соц-отчислений

  double suma_ndfv[2][12]; //сумма не денежных форм выплаты
  double suma_ndfv_slg[2][12]; /*Сумма не денежных форм выплаты на которую распространяется соц. льгота*/
  double suma_ndfv_blg[2][12]; /*сумма не денежных форм выплаты без соц. льготы*/    

  double blag_pom[2][12];  //Сумма благотворительной помощи
  double nn_blag_pom[2][12];  //Сумма необлагаемой налогом благотворительной помощи

  double suma_bez_soc_lgot[2][12]; /*Сумма на которую не распространяестя социальная льгота*/

  double suma_sk_esv[2][12]; /*Сумма для расчёта единого социального взноса для которой применяется льгота*/
  double suma_sk_esv_bol[2][12]; /*Сумма для расчёта единого социального взноса c больничного*/


  char   kodlg[12][64]; //коды льгот введенные по месяцам
  short  dn,mn,gn; //Дата приёма на работу
  short  dk,mk,gk;  //Дата увольнения с работы
 };


double  zarppn1_rdp(short mn,short mk,short god,struct ppnal1 *PP,masiv_din_double *suma_ne_zarpl,FILE *ff,FILE *ff_spis,FILE *ff_spis0);

extern short *kodbl; /*Код больничного*/
extern short    *kn_bsl; //Коды начислений на которые не распространяется льгота по подоходному налогу
extern class iceb_tu_str kodpn_all; /*все коды подоходного налога*/
extern short kodperpn;   /*Код перерасчёта подоходного налога*/
extern short    *knvr;/*Коды начислений не входящие в расчёт подоходного налога*/
extern short    *kodmp;  //Коды благотворительной помощи
extern int kod_esv; /*Код удержания единого социального взноса*/
extern int kod_esv_bol; /*Код удержания единого социального взноса*/
extern int kod_esv_inv; /*Код удержания единого социального взноса*/
extern int kod_esv_dog; /*Код удержания единого социального взноса*/
extern float proc_esv; /*процент единого социального взноса с работника*/
extern float proc_esv_bol; /*процент единого социального взноса с работника*/
extern float proc_esv_inv; /*процент единого социального взноса с работника*/
extern float proc_esv_dog; /*процент единого социального взноса с работника*/
extern class iceb_tu_str knvr_esv_r; /*Коды не входящие в расчёт единого социального взноса с работника*/
extern short *knnf; //Коды начислений недежными формами 

int  zarppn1(short mnr,short mkr,short godr,const char *tabnom,const char *podr,class spis_oth *oth)
{
char strsql[1024];

SQLCURSOR cur;
SQLCURSOR cur1;
int kolstr;
SQL_str row;
SQL_str row1;

sprintf(strsql,"select datz,tabn,prn,knah,suma,mesn,godn,podr from Zarp where \
datz >= '%04d-%d-1' and datz <= '%04d-%d-31' and suma <> 0. \
order by tabn asc, datz asc",godr,mnr,godr,mkr);


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

char imaf[64];
FILE *ff;

sprintf(imaf,"ppn%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

char imaf_spis[64];
FILE *ff_spis;
char imaf_spis0[64];
FILE *ff_spis0;

sprintf(imaf_spis,"ppnsp%d.lst",getpid());
if((ff_spis = fopen(imaf_spis,"w")) == NULL)
 {
  error_op_nfil(imaf_spis,errno,"");
  return(1);
 }
sprintf(imaf_spis0,"ppnsp0%d.lst",getpid());
if((ff_spis0 = fopen(imaf_spis0,"w")) == NULL)
 {
  error_op_nfil(imaf_spis0,errno,"");
  return(1);
 }
time_t vrem;
time(&vrem);

iceb_t_zagolov(gettext("Перерасчёт подоходного налога"),1,mnr,godr,31,mkr,godr,ff_spis);
fprintf(ff_spis,"\n");
iceb_t_zagolov(gettext("Перерасчёт подоходного налога"),1,mnr,godr,31,mkr,godr,ff_spis0);
fprintf(ff_spis0,"\n");
char imaf_p[64];

sprintf(imaf_p,"ppnp%d.lst",getpid());
if(zar_pr_start(imaf_p) != 0)
 return(1);
 
zar_pr_ins("Код больничного:");
if(kodbl != NULL)
 for(int nom=1; nom <= kodbl[0]; nom++)
  {
   sprintf(strsql,"%d ",kodbl[nom]);
   zar_pr_ins(strsql);
  }
zar_pr_ins("\n");
  
long tabnz=0;
int kolstr1=0;
struct ppnal1 PP;
memset(&PP,'\0',sizeof(PP));    

double suma;
short d,m,g;
int knah;
short mesz=0;

class masiv_din_double suma_ne_zarpl;
if(kn_bsl != NULL)
 if(kn_bsl[0] != 0)
  suma_ne_zarpl.make_class(kn_bsl[0]*12);
int kol_tab_nom=0;  
short mes_vshet;
short god_vshet;
double suma_per=0.;
int tabn=0;

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(tabnom,row[1],0,0) != 0)
   continue;
  if(proverka(podr,row[7],0,0) != 0)
   continue;

  tabn=atol(row[1]);

  if(tabnz != tabn)
   {

    if(tabnz != 0 )
     {
      if(kol_tab_nom > 0)
       fprintf(ff,"\f");
      
      suma_per+=zarppn1_rdp(mnr,mkr,godr,&PP,&suma_ne_zarpl,ff,ff_spis,ff_spis0);
      kol_tab_nom++;
     }

    memset(&PP,'\0',sizeof(PP));    
    suma_ne_zarpl.clear_class();
    PP.tabn=tabnz=tabn;
    memset(PP.fio,'\0',sizeof(PP.fio));    
    sprintf(strsql,"select fio,datn,datk from Kartb where tabn=%d",PP.tabn);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     {
      strncpy(PP.fio,row1[0],sizeof(PP.fio)-1);                   
      rsdat(&PP.dn,&PP.mn,&PP.gn,row1[1],2);
      rsdat(&PP.dk,&PP.mk,&PP.gk,row1[2],2);
      
     }
    printw("%5d %s\n",PP.tabn,PP.fio);
    mesz=0;
   }


  
  rsdat(&d,&m,&g,row[0],2);
  knah=atoi(row[3]);    
  suma=atof(row[4]);
  mes_vshet=atoi(row[5]);
  god_vshet=atoi(row[6]);
  
  if(mesz != m)
   {
    sprintf(strsql,"select lgot from Zarn where tabn=%d and god=%d and mes=%d",tabn,g,m);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
      strncpy(PP.kodlg[m-1],row1[0],iceb_tu_kolbait(10,row1[0]));
    mesz=m;
   }  

  if(atoi(row[2]) == 1)
   {
    PP.osuma[0][m-1]+=suma;
    if(g == god_vshet)
      PP.osuma[1][mes_vshet-1]+=suma;

    if(provkod(knvr,knah) >= 0)
     {
      PP.suma_nvrpn[0][m-1]+=suma;
      if(g == god_vshet)
        PP.suma_nvrpn[1][mes_vshet-1]+=suma;

      continue;
     }

    if(provkod(kodmp,knah) >= 0)
     {
      PP.blag_pom[0][m-1]+=suma;
      if(g == god_vshet)
        PP.blag_pom[1][mes_vshet-1]+=suma;

     //Код благотворительности должен быть в списке не зарплаты
//      continue;
     }

    double budget=0.;
    /*добавляем НДС*/
    double suma_s_nds=knnfnds(1,knah,"",suma,&budget,0); /*возвращает увеличенную сумму*/
    if(suma_s_nds > 0.)
     suma=suma_s_nds;
    int nomer=0; 

    if((nomer=provkod(kn_bsl,knah)) >= 0) /*не применяется соц. льгота*/
     {

      if(provkod(knnf,knah) >= 0) /*неденежная форма выплаты*/
       {
        PP.suma_ndfv_blg[1][mes_vshet-1]+=suma;
       }      
      suma_ne_zarpl.plus(suma,(nomer)*12+(m-1));
      PP.suma_bez_soc_lgot[1][mes_vshet-1]+=suma;
   
     }
    else /*применяется соц. льгота*/
     {
      PP.zarpl[0][m-1]+=suma;
      if(g == god_vshet)
        PP.zarpl[1][mes_vshet-1]+=suma;

      if(proverka(knvr_esv_r.ravno(),knah,0,1) != 0)
       {
        if(provkod(kodbl,knah) >= 0) /*больничный*/
         {
          PP.suma_sk_esv_bol[0][m-1]+=suma;
          if(g == god_vshet)
            PP.suma_sk_esv_bol[1][mes_vshet-1]+=suma;
         }
        else /*всё остальное*/
         {
          PP.suma_sk_esv[0][m-1]+=suma;
          if(g == god_vshet)
            PP.suma_sk_esv[1][mes_vshet-1]+=suma;
         }       
       }

      if(provkod(knnf,knah) >= 0)
       {


        PP.suma_ndfv[0][m-1]+=suma;
        PP.suma_ndfv_slg[0][m-1]+=suma;
  
        if(g == god_vshet)
         {
          PP.suma_ndfv[1][mes_vshet-1]+=suma;
          PP.suma_ndfv_slg[1][mes_vshet-1]+=suma;
         }

       }

     }

   }
  else
   {
    
    if(kodperpn == knah)   /*Код перерасчёта подоходного налога*/
     continue;
    if(proverka(kodpn_all.ravno(),knah,0,1) == 0)
      PP.podnal[m-1]+=suma;

    if(knah == kod_esv)
      PP.sumasoc[m-1]+=suma;

    if(knah == kod_esv_bol)
      PP.sumasoc[m-1]+=suma;

    if(knah == kod_esv_inv)
      PP.sumasoc[m-1]+=suma;

    if(knah == kod_esv_dog)
      PP.sumasoc[m-1]+=suma;

   }
   
 }

if(kol_tab_nom > 0)
     fprintf(ff,"\f");

suma_per+=zarppn1_rdp(mnr,mkr,godr,&PP,&suma_ne_zarpl,ff,ff_spis,ff_spis0);

fprintf(ff_spis,"%*s:%10.2f\n",iceb_tu_kolbait(37,"Разом"),"Разом",suma_per);
fprintf(ff_spis0,"%*s:%10.2f\n",iceb_tu_kolbait(37,"Разом"),"Разом",suma_per);

podpis(ff);
podpis(ff_spis);
fclose(ff);
zar_pr_end();

fclose(ff_spis);
fclose(ff_spis0);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Перерасчёт подоходного налога за год"));

oth->spis_imaf.plus(imaf_spis);
oth->spis_naim.plus(gettext("Общий итог"));

oth->spis_imaf.plus(imaf_spis0);
oth->spis_naim.plus(gettext("Общий итог без нулевых сумм"));

oth->spis_imaf.plus(imaf_p);
oth->spis_naim.plus(gettext("Протокол расчёта"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);



return(0);
}
/****************************************/
/*Распечатка данных и перерасчёт*/
/**************************************/

double  zarppn1_rdp(short mn,short mk,short god,struct ppnal1 *PP,masiv_din_double *suma_ne_zarpl,
FILE *ff,FILE *ff_spis,FILE *ff_spis0)
{
double itogo[10];
char strsql[1024];
memset(&itogo,'\0',sizeof(itogo));

fprintf(ff,"\n%d %s\n",PP->tabn,PP->fio);
fprintf(ff,"Дата початку роботи:%02d.%02d.%04d\n",PP->dn,PP->mn,PP->gn);
fprintf(ff,"Дата звільнення    :%02d.%02d.%04d\n",PP->dk,PP->mk,PP->gk);

sprintf(strsql,"\n%s-%d %s\n",__FUNCTION__,PP->tabn,PP->fio);
zar_pr_ins(strsql);
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"\
 Дата  |Нараховано|Не оподат.|Благодійна|Неопо.благ|  Податок |Зарплата  |Не гро.фор|Соціальні |Податок з |Соц.відра.|Код пільги|\n\
       |          |податком  |допомога  |одійна доп|загальний |          |ми нар.зар|від.загал.|зарплати  |з зарплати|          |\n");

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
double nalsz;
//double it_soc=0.;
double podoh_sz[12];
zarrnesv();

for(int i=mn-1 ; i < mk; i++)
 {

  nalsz=PP->podnal[i];
//  it_soc=PP->sumasoc[i];

  fprintf(ff,"%02d.%04d %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %s\n",
  i+1,god,
  PP->osuma[0][i],
  PP->suma_nvrpn[0][i],
  PP->blag_pom[0][i],
  PP->nn_blag_pom[0][i],
  PP->podnal[i],
  PP->zarpl[0][i],
  PP->suma_ndfv[0][i],
  PP->sumasoc[i],
  nalsz,
//  it_soc,
  PP->kodlg[i]);


  podoh_sz[i]=nalsz;
  itogo[0]+=PP->osuma[0][i];
  itogo[1]+=PP->suma_nvrpn[0][i];
  itogo[2]+=PP->blag_pom[0][i];
  itogo[3]+=PP->nn_blag_pom[0][i];
  itogo[4]+=PP->podnal[i];
  itogo[5]+=PP->zarpl[0][i];
  itogo[6]+=PP->suma_ndfv[0][i];
  itogo[7]+=PP->sumasoc[i];
  itogo[8]+=nalsz;
//  itogo[9]+=it_soc;
  
 }

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"%7s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
" ",itogo[0],itogo[1],itogo[2],itogo[3],itogo[4],itogo[5],itogo[6],itogo[7],itogo[8],itogo[9]);


if(kn_bsl != NULL)
 if(kn_bsl[0] != 0)
 if(suma_ne_zarpl->suma() != 0.)
  {
   double itogo_nz=0.;
   class iceb_tu_str naim_nash("");
   SQL_str row;
   SQLCURSOR cur;

   fprintf(ff,"\n%s:\n","Нарахування, на які не розповсюджуеться социальна пільга");
   fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------\n");   
   fprintf(ff,"Код|  Найменування нарахування    |");

   for(int i=0; i < 12 ; i++)
    fprintf(ff," %02d.%04d|",i+1,god);

   fprintf(ff,"%-*.*s|\n",iceb_tu_kolbait(8,"Разом"),iceb_tu_kolbait(8,"Разом"),"Разом");
   fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------\n");   
   double itogo_nzi[12];
   memset(&itogo_nzi,'\0',sizeof(itogo_nzi));
   for(int i=1; i <= kn_bsl[0]; i++)
    {
     itogo_nz=0.;
     for(int ii=0; ii < 12; ii++)
      {
       itogo_nz+=suma_ne_zarpl->ravno((i-1)*12+ii);
       itogo_nzi[ii]+=suma_ne_zarpl->ravno((i-1)*12+ii);
      }
     if(itogo_nz == 0.)
      continue;
    //Читаем наименование начисления
     sprintf(strsql,"select naik from Nash where kod=%d",kn_bsl[i]);
     if(sql_readkey(&bd,strsql,&row,&cur) == 1)
      naim_nash.new_plus(row[0]);
     else
      naim_nash.new_plus("");
     fprintf(ff,"%3d %-*.*s ",
     kn_bsl[i],
     iceb_tu_kolbait(30,naim_nash.ravno()),iceb_tu_kolbait(30,naim_nash.ravno()),naim_nash.ravno());
     
     for(int ii=0; ii < 12; ii++)
       fprintf(ff,"%8.2f ",suma_ne_zarpl->ravno((i-1)*12+ii));
     fprintf(ff,"%8.2f\n",itogo_nz);     

    }
   fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------\n");   

   fprintf(ff,"%*s ",iceb_tu_kolbait(34,"Разом"),"Разом");
   for(int ii=0; ii < 12; ii++)
       fprintf(ff,"%8.2f ",itogo_nzi[ii]);
   fprintf(ff,"%8.2f\n",suma_ne_zarpl->suma());
  }

fprintf(ff,"\nРозрахунок по місяцях в рахунок яких були зроблені нарахування\n");

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"\
 Дата  |Нараховано|Не оподат.|Благдійна |Неопо.благ|Зарплата  |Не гро.фор|Соц.відра.|Податок   |Податок   |Різниця|\n\
       |          |податком  |допомога  |одійна доп|          |ми нар.зар|з зарплати|розрахунк.|утриманий |       |\n");

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------\n");

memset(&itogo,'\0',sizeof(itogo));

sprintf(strsql,"\nПерерасчёт\n");
zar_pr_ins(strsql);
for(int i=mn-1 ; i < mk; i++)
 {
  sprintf(strsql,"\n\nМесяц-%d.%d\n",i+1,god);
  zar_pr_ins(strsql);

  sprintf(strsql,"Cумма для расчёта единого социального взноса=%.2f Больничный=%.2f\n",PP->suma_sk_esv[1][i],PP->suma_sk_esv_bol[1][i]);
  zar_pr_ins(strsql);
/************  
  class zaresv_rk resv;
  
  resv.mp=i+1;
  resv.gp=god;
  resv.tabn=PP->tabn;
  resv.suma_s_lgot=PP->suma_sk_esv[1][i];
  resv.suma_s_lgot_bol=PP->suma_sk_esv_bol[1][i];
  resv.metka_zap=1;  
  zaresv_rs(&resv);
  it_soc=resv.esv_s_lgot;
  
  sprintf(strsql,"Сумма посчитанного единого социального взноса=%.2f\n",it_soc);
  zar_pr_ins(strsql);
*****************/       
  class zar_sumtn tnsum;
  tnsum.suma_nah_all_podoh=PP->osuma[1][i]-PP->suma_nvrpn[1][i];
//  tnsum.suma_esv_all=it_soc;
  tnsum.suma_slg=PP->zarpl[1][i];
  tnsum.suma_ndfv_slg=PP->suma_ndfv_slg[1][i];
  tnsum.suma_ndfv_blg=PP->suma_ndfv_blg[1][i];
  tnsum.suma_blg=PP->suma_bez_soc_lgot[1][i];
//  tnsum.suma_esv_s_zarslg=it_soc;

/**********
  if(sravmydat(1,1,2016,1,i+1,god) > 0)
    nalsz=podohrs3(PP->tabn,i+1,god,&tnsum,1,1);
  else
************/
    nalsz=podohrs2016(PP->tabn,i+1,god,1,1,&tnsum);

  fprintf(ff,"%02d.%04d %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %7.2f\n",
  i+1,god,
  PP->osuma[1][i],
  PP->suma_nvrpn[1][i],
  PP->blag_pom[1][i],
  PP->nn_blag_pom[1][i],
  PP->zarpl[1][i],
  PP->suma_ndfv[1][i],
//  it_soc,
  nalsz,
  podoh_sz[i],
  nalsz-podoh_sz[i]);



  itogo[0]+=PP->osuma[1][i];
  itogo[1]+=PP->suma_nvrpn[1][i];
  itogo[2]+=PP->blag_pom[1][i];
  itogo[3]+=PP->nn_blag_pom[1][i];
  itogo[4]+=PP->zarpl[1][i];
  itogo[5]+=PP->suma_ndfv[1][i];
//  itogo[6]+=it_soc;
  itogo[7]+=nalsz;
  itogo[8]+=podoh_sz[i];
  itogo[9]+=nalsz-podoh_sz[i];  
 }

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"%7s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %7.2f\n",
" ",itogo[0],itogo[1],itogo[2],itogo[3],itogo[4],itogo[5],itogo[6],itogo[7],itogo[8],itogo[9]);


fprintf(ff_spis,"%6d %-*.*s %10.2f\n",
PP->tabn,
iceb_tu_kolbait(30,PP->fio),iceb_tu_kolbait(30,PP->fio),PP->fio,
itogo[9]);

if(fabs(itogo[9]) > 0.009)
 fprintf(ff_spis0,"%6d %-*.*s %10.2f\n",
 PP->tabn,
 iceb_tu_kolbait(30,PP->fio),iceb_tu_kolbait(30,PP->fio),PP->fio,
 itogo[9]);

return(itogo[9]);
 

}
