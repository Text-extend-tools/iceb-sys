/*$Id: zarpd.c,v 5.8 2013/09/26 09:43:45 sasa Exp $*/
/*22.02.2011	22.02.2011	Белых А.И.	zarpd.c
Податкова декларация з податку на доходи фізичних осіб
*/
#include <errno.h>
#include "buhg.h"

void zarpd_skv(const char *tbn,short mes,short god,double *suma_kvp,double *podoh_kvp,double *suma_dohoda,FILE *ff_prot);

extern class iceb_tu_str kodpn_all;
extern short *obud; /*Обязательные удержания*/
extern short *knvr;/*Коды начислений не входящие в расчёт подоходного налога*/
extern short *knnf; //Коды начислений недежными формами 

int zarpd(class spis_oth *oth)
{
int tbn;
int kom=0;
class iceb_tu_str fiov("");
static class iceb_tu_str datr("");
static class iceb_tu_str tabnom("");

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт подоходного на выплаченную зарплату"));

VV.VVOD_SPISOK_add_data(datr.ravno(),8);
VV.VVOD_SPISOK_add_data(tabnom.ravno(),112);

VV.VVOD_SPISOK_add_MD(gettext("Дата....(м.г)........."));//0
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер.(,,).."));//1

naz:;
clear();

helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("таб.номер"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

datr.new_plus(VV.data_ravno(0));
tabnom.new_plus(VV.data_ravno(1));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   iceb_t_pdoc("zarp4_7_1_1.txt");
   goto naz;

  case FK2:
  case PLU:
    break;
      
  case FK3: //Просмотр списка табельных номеров
    tbn=0;
    fiov.new_plus("");
    if(dirtab(&tbn,&fiov,0,1) == 0)
     {
      VV.data_z_plus(1,tbn);
     }    
    goto naz;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;

  case FK10:
  case ESC:
    return(1);

  default:
    goto naz;
 }


short mr=0,gr=0;
if(rsdat1(&mr,&gr,datr.ravno()) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата!"));
  goto naz;
 }

char imaf[64];
char strsql[512];
int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kolstr1=0;
int kolstr2=0;
double suma_str[11];
double i_suma_str[11];
short m,g;

double sumanu=0;
int knah=0;
sprintf(strsql,"select tabn from Zarn where god=%d and mes=%d",gr,mr);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одного табельного номера !"));
  return(1);
 }

FILE *ff;
sprintf(imaf,"pd%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

FILE *ff_prot;
char imaf_prot[64];
sprintf(imaf_prot,"pdp%d.lst",getpid());

if((ff_prot = fopen(imaf_prot,"w")) == NULL)
 {
  error_op_nfil(imaf_prot,errno,"");
  return(1);
 }

iceb_t_zagolov(gettext("Расчёт подоходного на выплаченную зарплату"),0,0,0,0,mr,gr,ff);
fprintf(ff,"Коды начислений неденежными формами:");

if(knnf != NULL)
 {
  for(int nom=1; nom <= knnf[0]; nom++)
   fprintf(ff,"%d ",knnf[nom]);
  fprintf(ff,"\n");
 } 

fprintf(ff,"\n");

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
Таб.н.|       Фамилия                |Начислено |Взято в   |Начисленны|Выплаченый|Выплаченый|Выплаченый|Выплаченый|Выплаченый|Выплаченый|Выплачено |Выплачено |\n\
      |                              |          |расчёт    |й подоходн| доход    |доход за  |доход за  |под-ный   |под-ный за|под-ный за|на руки за|на руки за|\n\
      |                              |          |          |ный налог |          |отчё.месяц|другие пер|налог     |отчё.месяц|другие пер|отч.месяц.|другие пер|\n");

/****
123456 123456789012345678901234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567890                       1234567890 1234567890
**********/

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
memset(i_suma_str,'\0',sizeof(i_suma_str));
int nom_mes_god=0;
double suma_nfn=0.;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(tabnom.ravno(),row[0],0,0) != 0)
   continue;

  fiov.new_plus("");
  sprintf(strsql,"select fio from Kartb where tabn=%s",row[0]);
  if(readkey(strsql,&row1,&cur1) == 1)
   fiov.new_plus(row1[0]);

  fprintf(ff_prot,"\n\n%s %s\n",row[0],fiov.ravno());
  
  sprintf(strsql,"select prn,knah,suma,godn,mesn from Zarp where datz >= '%04d-%02d-%02d' and datz <= '%04d-%02d-%d' and tabn=%s and suma <> 0.",
  gr,mr,1,gr,mr,31,row[0]);

  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }

  if(kolstr2 == 0)
   {
    fprintf(ff_prot,"Не найдено ни одного начисления или удержания!\n");
    continue;
   }
  memset(suma_str,'\0',sizeof(suma_str));

  class SPISOK mes_god; /*месяц.год в счёт которого выплата*/
  class masiv_din_double suma_mes_god; /*сумма выплаты в счёт месяца*/
  double suma_ob_ud_rm=0.; /*Cумма обязательных удержаний расчётного месяца*/
  while(cur1.read_cursor(&row1) != 0)
   {
    
    sumanu=atof(row1[2]);
    knah=atoi(row1[1]);
    
    if(atoi(row1[0]) == 1) /*начисления*/
     {
      suma_str[0]+=sumanu;

      if(provkod(knnf,knah) >= 0)
        suma_nfn+=sumanu;
        
      if(provkod(knvr,knah) >= 0)
       continue;      
      suma_str[1]+=sumanu;

     }
    else
     {

//      if(knah == kodpn)
      if(proverka(kodpn_all.ravno(),knah,0,1) == 0)
       {
        suma_str[2]+=sumanu;
       }

      if(provkod(obud,knah) >= 0)
       {
        suma_ob_ud_rm+=sumanu;
        continue;
       }
      suma_str[3]+=sumanu;    

      sprintf(strsql,"%02d.%04d",atoi(row1[4]),atoi(row1[3]));
      if((nom_mes_god=mes_god.find(strsql)) < 0)
       mes_god.plus(strsql);
      suma_mes_god.plus(sumanu,nom_mes_god);
             

     }
   }
    
  double suma_viplat_tm=0.;
  double suma_viplat_pp=0.;
  double suma_kvp=0.; /*Сумма к выплате*/
  double podoh_kvp=0.; /*Cумма подоходного*/
  double suma_dohoda=0.;/*Сумма дохода*/
  
  fprintf(ff_prot,"Cуммы выплат:\n");
  
  for(int nom=0; nom < mes_god.kolih(); nom++)
   {
    rsdat1(&m,&g,mes_god.ravno(nom));
    fprintf(ff_prot,"%02d.%04d %.2f\n",m,g,suma_mes_god.ravno(nom));
    
    if(m == mr && g == gr)
     {
      suma_viplat_tm+=suma_mes_god.ravno(nom);
      suma_str[9]+=suma_mes_god.ravno(nom);

      /*сумма обязательных удержаний отрицательная - поэтому прибавляем*/
      double suma_k_vip=suma_str[1]+suma_ob_ud_rm;

      if(ff_prot != NULL)
       fprintf(ff_prot,"Сумма начиленная=%.2f Сумма обязательных удержаний=%.2f\n\
Cумма к выплате=%.2f+%.2f=%.2f\n",
        suma_str[1],suma_ob_ud_rm,suma_str[1],suma_ob_ud_rm,suma_k_vip);
        
      /*Считаем процент выплаты*/
          
      double proc_vip=0.;
      /*сумма обязательных удержаний отрицательная - поэтому прибавляем*/
      if(suma_k_vip  != 0.)
        proc_vip=suma_mes_god.ravno(nom)*100./suma_k_vip*-1;
       
      /*Считаем сколько на эту выплату приходтся подоходного налога*/
      suma_str[7]+=suma_str[2]*proc_vip/100.;

      /*Считаем сколько на эту выплату приходтся дохода*/
      suma_str[4]+=suma_str[1]*proc_vip/100.;     

      fprintf(ff_prot,"Процент выплаты=%.2f*100./%.2f=%f\n",
      suma_mes_god.ravno(nom),suma_k_vip,proc_vip);

      fprintf(ff_prot,"Расчёт доли подоходного налога = %.2f*%f/100.=%.2f\n",
      suma_str[2],proc_vip,suma_str[2]*proc_vip/100.);

      fprintf(ff_prot,"Расчёт доли дохода=%.2f*%f/100.=%.2f\n",
      suma_str[1],proc_vip,suma_str[1]*proc_vip/100.);      
     }
    else
     {
      fprintf(ff_prot,"Выплата в счёт другого периода!\n");

      suma_viplat_pp+=suma_mes_god.ravno(nom);
      suma_str[10]+=suma_mes_god.ravno(nom);

      zarpd_skv(row[0],m,g,&suma_kvp,&podoh_kvp,&suma_dohoda,ff_prot);
      fprintf(ff_prot,"Cумма к выдаче=%.2f Cумма подоходного налога=%.2f Cумма дохода=%.2f\n",
      suma_kvp,podoh_kvp,suma_dohoda);

      /*Считаем процент выплаты*/
      double proc_vip=0.;
      if(suma_kvp != 0.)
        proc_vip=suma_mes_god.ravno(nom)*100./suma_kvp*-1;
      fprintf(ff_prot,"Процент выплаты=%.2f*100./%.2f=%f\n",
      suma_mes_god.ravno(nom),suma_kvp,proc_vip);
      
      /*Считаем сколько на эту выплату приходтся подоходного налога*/
      suma_str[8]+=podoh_kvp*proc_vip/100.;
      fprintf(ff_prot,"Расчёт доли подоходного налога=%.2f*%f/100.=%.2f\n",
      podoh_kvp,proc_vip,podoh_kvp*proc_vip/100.);
      
      /*Считаем сколько на эту выплату приходтся дохода*/
      suma_str[5]+=suma_dohoda*proc_vip/100.;     
      fprintf(ff_prot,"Расчёт доли дохода=%.2f*%f/100.=%.2f\n",
      suma_dohoda,proc_vip,suma_dohoda*proc_vip/100.);
      
     }
   }
  
  suma_str[6]=suma_str[7]+suma_str[8];
  suma_str[3]=suma_str[4]+suma_str[5];
  
  fprintf(ff,"%6s %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  row[0],
  iceb_tu_kolbait(30,fiov.ravno()),
  iceb_tu_kolbait(30,fiov.ravno()),
  fiov.ravno(),
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
  suma_str[10]);
  
  i_suma_str[0]+=suma_str[0];
  i_suma_str[1]+=suma_str[1];
  i_suma_str[2]+=suma_str[2];
  i_suma_str[3]+=suma_str[3];
  i_suma_str[4]+=suma_str[4];
  i_suma_str[5]+=suma_str[5];
  i_suma_str[6]+=suma_str[6];
  i_suma_str[7]+=suma_str[7];
  i_suma_str[8]+=suma_str[8];
  i_suma_str[9]+=suma_str[9];
  i_suma_str[10]+=suma_str[10];
    
 }

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"%6s %*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
"",
iceb_tu_kolbait(30,gettext("Итого")),
iceb_tu_kolbait(30,gettext("Итого")),
gettext("Итого"),
i_suma_str[0],
i_suma_str[1],
i_suma_str[2],
i_suma_str[3],
i_suma_str[4],
i_suma_str[5],
i_suma_str[6],
i_suma_str[7],
i_suma_str[8],
i_suma_str[9],
i_suma_str[10]);



podpis(ff);

fclose(ff);
fclose(ff_prot);




oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт подоходного на выплаченную зарплату"));
oth->spis_imaf.plus(imaf_prot);
oth->spis_naim.plus(gettext("Протокол расчёта"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);


return(0);

}
/**************************************/
/*Узнаём сумму к выплате и сумму подоходного налога*/
/*****************************************************/
void zarpd_skv(const char *tbn,short mes,short god,double *suma_kvp,double *podoh_kvp,double *suma_dohoda,FILE *ff_prot)
{
char strsql[512];
int kolstr=0;
double suma_nah=0.;
double suma_uder=0.;
double suma=0.;
SQL_str row;
class SQLCURSOR cur;
int knah=0;
*suma_kvp=0.;
*podoh_kvp=0.;
*suma_dohoda=0.;

sprintf(strsql,"select prn,knah,suma from Zarp where datz >= '%04d-%02d-%02d' and datz <= '%04d-%02d-%d' and tabn=%s and suma <> 0.",
god,mes,1,god,mes,31,tbn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  suma=atof(row[2]);
  knah=atoi(row[1]);
  if(atoi(row[0]) == 1) /*начисления*/
   {
    suma_nah+=suma;

    if(provkod(knvr,knah) >= 0)
     continue;      
    *suma_dohoda+=suma;
   }
  else
   {

//    if(knah == kodpn)
    if(proverka(kodpn_all.ravno(),knah,0,1) == 0)
     {
      *podoh_kvp+=suma;
      continue;
     }

    if(provkod(obud,knah) < 0)
     continue;
    suma_uder+=suma;
   }

 }

*suma_kvp=suma_nah+suma_uder+*podoh_kvp;


}
