/*$Id: zarrasp2.c,v 5.9 2013/09/26 09:43:45 sasa Exp $*/
/*03.02.2011	03.02.2011	Белых А.И.	zarrasp2.c
Определение сумм выплат в соц.фонды и налоговую сумм на выплаченную зарплату
*/
#include <errno.h>
#include "buhg.h"

//extern short kodpn;   /*Код подоходного налога*/
extern class iceb_tu_str kodpn_all;
extern short	*obud; /*Обязательные удержания*/
extern int kod_esv; /*Код удержания единого социального взноса*/
extern int kod_esv_bol; /*Код удержания единого социального взноса*/
extern int kod_esv_inv; /*Код удержания единого социального взноса*/
extern int kod_esv_dog; /*Код удержания единого социального взноса*/

extern int kodf_esv; /*Код фонда удержания единого социального взноса*/
extern int kodf_esv_bol; /*Код фонда удержания единого социального взноса*/
extern int kodf_esv_inv; /*Код фонда удержания единого социального взноса*/
extern int kodf_esv_dog; /*Код фонда удержания единого социального взноса*/

int zarzasp2(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *tabnom,short mv,short gv,const char *podr,
class spis_oth *oth)
{
char strsql[512];
int kolstr=0;
int kolstr1=0;
int kolstr2=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
char imaf[64];
char imaf_prot[64];
sprintf(imaf,"esvpn%d.lst",getpid());
sprintf(imaf_prot,"esvpnp%d.lst",getpid());

sprintf(strsql,"select distinct tabn from Zarp where datz >= '%04d-%02d-%02d' and datz <= '%04d-%02d-%02d' \
and prn='2' and suma <> 0.",gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(0);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }

FILE *ff;
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
FILE *ff_prot;
if((ff_prot = fopen(imaf_prot,"w")) == NULL)
 {
  error_op_nfil(imaf_prot,errno,"");
  return(1);
 }

iceb_t_zagolov(gettext("Расчёт отчислений на выданную зарплату"),dn,mn,gn,dk,mk,gk,ff);

if(tabnom[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Табельный номер"),tabnom);
if(podr[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Подразделение"),podr);
 
fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
 Т/н |       Прізвище               |Нараховано|Обов'язков|До видачі |   ЄСВ               |  ЄСВ з лікарняних   |   ЄСВ з інвалідів   |  ЄСВ з договорів    |Прибуткови|\n\
     |                              |          |і утриман.|          |Нарахуван.|Утримання |Нарахуван.|Утримання |Нарахуван.|Утримання |Нарахуван.|Утримання |й податок |\n");
/*********
                                         0          1          2          3         4           5          6           7         8          9         10        11
                                                               
12345 123456789012345678901234567890 1234567890 1234567890 1234567890
**********/
fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
int knah=0;
double sumanu=0.;
short m,g;
int nomer_vsp=0;
const int RAZMER_MAS=12;
double suma_str[RAZMER_MAS];
double suma_str_mes[RAZMER_MAS];
double suma_str_v[RAZMER_MAS];
double isuma_str[RAZMER_MAS];
double isuma_str_v[RAZMER_MAS];

memset(isuma_str,'\0',sizeof(isuma_str));
memset(isuma_str_v,'\0',sizeof(isuma_str_v));

int tabn=0;
class iceb_tu_str fio("");
zarrnesv();

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(proverka(tabnom,row[0],0,0) != 0)
    continue;
  tabn=atoi(row[0]);

  fio.new_plus("");
  sprintf(strsql,"select fio,podr from Kartb where tabn=%d",tabn);
  if(readkey(strsql,&row1,&cur1) == 1)
   {
    if(proverka(podr,row1[1],0,0) != 0)
     continue;
    fio.new_plus(row1[0]);
   }
  fprintf(ff_prot,"\n%d %s\n",tabn,fio.ravno());
  
  /*Находим все выплаты*/
  if(mv == 0)
   sprintf(strsql,"select knah,suma,godn,mesn from Zarp where \
datz >= '%04d-%d-%d' and datz <= '%04d-%d-%d' and tabn=%d and prn='2' and suma <> 0. ",
   gn,mn,dn,gk,mk,dk,tabn);
  else
   sprintf(strsql,"select knah,suma,godn,mesn from Zarp where \
datz >= '%04d-%d-%d' and datz <= '%04d-%d-%d' and tabn=%d and prn='2' and godn=%d and mesn=%d and suma <> 0. ",
   gn,mn,dn,gk,mk,dk,tabn,gv,mv);
  
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }

  if(kolstr2 == 0)
    continue;
 
  class SPISOK mes_god;
  class masiv_din_double suma_mes_god;
  fprintf(ff_prot,"Находим все выплаты:\n");    
  while(cur1.read_cursor(&row1) != 0)
   {
    knah=atoi(row1[0]);
    if(provkod(obud,knah) >= 0)
     continue;  
    
    fprintf(ff_prot,"%s %s %s %s\n",row1[0],row1[1],row1[2],row1[3]);
    
    sumanu=atof(row1[1]);

    sprintf(strsql,"%02d.%04d",atoi(row1[3]),atoi(row1[2]));

    if((nomer_vsp=mes_god.find(strsql)) < 0)
     mes_god.plus(strsql);

    suma_mes_god.plus(sumanu,nomer_vsp);
    
   }  

  /*Узнаём какая сумма к выплате и суммы всех отчислений*/
  memset(suma_str,'\0',sizeof(suma_str));
  memset(suma_str_v,'\0',sizeof(suma_str_v));
  for(int nom=0; nom < mes_god.kolih(); nom++)
   {
    rsdat1(&m,&g,mes_god.ravno(nom));
    
    sprintf(strsql,"select prn,knah,suma,datz,godn,mesn from Zarp where \
datz >= '%04d-%d-%d' and datz <= '%04d-%d-%d' and tabn=%s and suma <> 0.",
    g,m,1,g,m,31,row[0]);
/*
    printw("%s\n",strsql);
    refresh();
*/
    if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
     {
      msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
      continue;
     }

    memset(suma_str_mes,'\0',sizeof(suma_str_mes));    

    while(cur1.read_cursor(&row1) != 0)
     {
      knah=atoi(row1[1]);
      sumanu=atof(row1[2]);
      if(atoi(row1[0]) == 1)
       {
        suma_str_mes[0]+=sumanu;
       }
      else
       {
        if(knah == kod_esv)
         suma_str_mes[4]+=sumanu;
        if(knah == kod_esv_bol)
         suma_str_mes[6]+=sumanu;
        if(knah == kod_esv_inv)
         suma_str_mes[8]+=sumanu;
        if(knah == kod_esv_dog)
         suma_str_mes[10]+=sumanu;

//        if(knah == kodpn)
        if(proverka(kodpn_all.ravno(),knah,0,1) == 0)
         suma_str_mes[11]+=sumanu;

        if(provkod(obud,knah) >= 0)
         {
          suma_str_mes[1]+=sumanu;
          continue;  
         }        
       }
     }

    /*Смотрим начисления на фонд зарплаты*/
    sprintf(strsql,"select kodz,sumap from Zarsocz where datz='%04d-%02d-01' and tabn=%s",g,m,row[0]);
    if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
     {
      msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
      continue;
     }

    int kod_fonda=0;

    while(cur1.read_cursor(&row1) != 0)
     {
      kod_fonda=atoi(row1[0]);
      sumanu=atof(row1[1]);

      if(kod_fonda == kodf_esv)
       suma_str_mes[3]+=sumanu;    
      if(kod_fonda == kodf_esv_bol)
       suma_str_mes[5]+=sumanu;    
      if(kod_fonda == kodf_esv_inv)
       suma_str_mes[7]+=sumanu;    
      if(kod_fonda == kodf_esv_dog)
       suma_str_mes[9]+=sumanu;    

     }
    suma_str_mes[2]=suma_str_mes[0]+suma_str_mes[1];    
    for(int nomm=0; nomm < RAZMER_MAS; nomm++)
      suma_str[nomm]+=suma_str_mes[nomm];
    
    /*Определяем процент выплаты*/
    double suma_vip=suma_mes_god.ravno(nom);
    suma_vip*=-1;
    suma_str_v[2]+=suma_vip;
    double suma_k_vid=suma_str_mes[0]+suma_str_mes[1];
    double proc_vip=0.;
 
    if(suma_k_vid != 0.)
      proc_vip=suma_vip*100./suma_k_vid;
    fprintf(ff_prot,"Процент выплаты=%.2f*100./%.2f=%.2f\n",suma_vip,suma_k_vid,proc_vip);
       
    /*Делаем расчёт налогов на выплаченную зарплату*/
    if(proc_vip != 0.)
     for(int nomm=3; nomm < RAZMER_MAS; nomm++)
       suma_str_v[nomm]+=suma_str_mes[nomm]*proc_vip/100.;


   }  
  fprintf(ff_prot,"suma_str[0]=%f\n",suma_str[0]);

  fprintf(ff,"%5s %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  row[0],
  iceb_tu_kolbait(30,fio.ravno()),
  iceb_tu_kolbait(30,fio.ravno()),
  fio.ravno(),
  suma_str[0],
  suma_str[1],
  suma_str[2],
  suma_str[3],
  suma_str[4],
  suma_str[5],
  suma_str[6],
  suma_str[7],
  suma_str[8],
  suma_str[9],
  suma_str[10],
  suma_str[11]);

  fprintf(ff,"%*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  iceb_tu_kolbait(58,gettext("Выплачено")),
  gettext("Выплачено"),
  suma_str_v[2],
  suma_str_v[3],
  suma_str_v[4],
  suma_str_v[5],
  suma_str_v[6],
  suma_str_v[7],
  suma_str_v[8],
  suma_str_v[9],
  suma_str_v[10],
  suma_str_v[11]);



  for(int nomm=0; nomm < RAZMER_MAS; nomm++)
   {
    isuma_str[nomm]+=suma_str[nomm];
    isuma_str_v[nomm]+=suma_str_v[nomm];
   }  
 }

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"%*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
iceb_tu_kolbait(36,gettext("Итого начислено")),
gettext("Итого начислено"),
isuma_str[0],
isuma_str[1],
isuma_str[2],
isuma_str[3],
isuma_str[4],
isuma_str[5],
isuma_str[6],
isuma_str[7],
isuma_str[8],
isuma_str[9],
isuma_str[10],
isuma_str[11]);

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"%*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
iceb_tu_kolbait(58,gettext("Итого выплачено")),
gettext("Итого выплачено"),
isuma_str_v[2],
isuma_str_v[3],
isuma_str_v[4],
isuma_str_v[5],
isuma_str_v[6],
isuma_str_v[7],
isuma_str_v[8],
isuma_str_v[9],
isuma_str_v[10],
isuma_str_v[11]);
 
podpis(ff);
fclose(ff);
podpis(ff_prot);
fclose(ff_prot);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт отчислений на выданную зарплату"));
oth->spis_imaf.plus(imaf_prot);
oth->spis_naim.plus(gettext("Протокол хода расчёта"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++) 
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);

return(0);

}
