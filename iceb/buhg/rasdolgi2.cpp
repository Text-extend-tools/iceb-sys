/*$Id: rasdolgi2.c,v 5.9 2013/09/26 09:43:38 sasa Exp $*/
/*15.07.2015	14.03.2011	Белых А.И.	rasdolgi2.c
Распечатать долги предприятия по зарплате своим работникам
Расчёт применяется после 1.1.2011
*/
#include <math.h>
#include <errno.h>
#include "buhg.h"

void rasdolgi2_s(const char *data_nah,short dd,short md,short gd,const char *tabnom,const char *podr,FILE *ff);


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

int rasdolgi2(const char *data_nah,
const char *data_rash,
const char *podr,
const char *tabnom,
int mras,
class spis_oth *oth)
{
short mes_nah,god_nah;
short dd,md,gd;
if(rsdat1(&mes_nah,&god_nah,data_nah) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата начисления !"));
  return(1);
 }
if(rsdat(&dd,&md,&gd,data_rash,0) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата долга !"));
  return(1);
 }

char imaf[64];
sprintf(imaf,"golg2%d.lst",getpid());
char imaf_soc[64];
sprintf(imaf_soc,"golg2s%d.lst",getpid());
char imaf_prot[64];
sprintf(imaf_prot,"golgp2%d.lst",getpid());

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

FILE *ff_soc;
if((ff_soc = fopen(imaf_soc,"w")) == NULL)
 {
  error_op_nfil(imaf_soc,errno,"");
  return(1);
 }

rasdolgi2_s(data_nah,dd,md,gd,tabnom,podr,ff);
rasdolgi2_s(data_nah,dd,md,gd,tabnom,podr,ff_soc);
rasdolgi2_s(data_nah,dd,md,gd,tabnom,podr,ff_prot);

zarrnesv();
int kolstr=0;
int kolstr1=0;
int kolstr2=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
char strsql[1024];
sprintf(strsql,"select tabn,podr from Zarn where god=%d and mes=%d \
order by podr,tabn asc",god_nah,mes_nah);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr ==  0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи!"));
  return(1);
 }

fprintf(ff,"\
----------------------------------------------------------------------\n");
fprintf(ff,gettext(" Т/н | Фамилия  , Імя , Отчество    |Начислено | Удержано |  Долг    |\n"));
fprintf(ff,"\
----------------------------------------------------------------------\n");
fprintf(ff_soc,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff_soc,"\
 Т/н |       Прізвище               |Нараховано|Обов'язков|До видачі |   ЄСВ               |  ЄСВ з лікарняних   |   ЄСВ з інвалідів   |  ЄСВ з договорів    |Прибуткови|\n\
     |                              |          |і утриман.|          |Нарахуван.|Утримання |Нарахуван.|Утримання |Нарахуван.|Утримання |Нарахуван.|Утримання |й податок |\n");
/*********
                                         0          1          2          3         4           5          6           7         8          9         10        11
                                                               
12345 123456789012345678901234567890 1234567890 1234567890 1234567890
**********/
fprintf(ff_soc,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
const int RAZMER_MAS=12;
double suma_str[RAZMER_MAS];
double suma_str_mes[RAZMER_MAS];
double suma_str_dolg[RAZMER_MAS];
double isuma_str[RAZMER_MAS];
double isuma_str_dolg[RAZMER_MAS];

memset(isuma_str,'\0',sizeof(isuma_str));
memset(isuma_str_dolg,'\0',sizeof(isuma_str_dolg));
class iceb_tu_str fio("");
int tabn=0;
int knah=0;
double sumanu=0.;
double i_suma_uder=0.;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(podr,row[1],0,0) != 0)
    continue;

  if(proverka(tabnom,row[0],0,0) != 0)
    continue;
  tabn=atoi(row[0]);  
  fio.new_plus("");
  sprintf(strsql,"select fio from Kartb where tabn=%d",tabn);
  if(readkey(strsql,&row1,&cur1) == 1)
    fio.new_plus(row1[0]);

  fprintf(ff_prot,"\n%d %s\n",tabn,fio.ravno());

  /*Узнаём какая сумма к выплате и суммы всех отчислений*/
  memset(suma_str,'\0',sizeof(suma_str));
  memset(suma_str_dolg,'\0',sizeof(suma_str_dolg));

  
  sprintf(strsql,"select prn,knah,suma,datz,godn,mesn from Zarp where \
 datz >= '%04d-%d-%d' and datz <= '%04d-%d-%d' and tabn=%s and suma <> 0.",
  god_nah,mes_nah,1,god_nah,mes_nah,31,row[0]);
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

      if(proverka(kodpn_all.ravno(),knah,0,1) == 0)
       suma_str_mes[11]+=sumanu;

      if(provkod(obud,knah) >= 0)
       {
        suma_str_mes[1]+=sumanu;
        continue;  
       }        
     }
   }
  suma_str_mes[2]=suma_str_mes[0]+suma_str_mes[1];    
  fprintf(ff_prot,"Cумма обязательных удержаний=%.2f\n",suma_str_mes[1]);
  
  /*определяем сумму всех удержаний в счёт этого месяца*/
  double suma_uder=prbper1(mes_nah,god_nah,dd,md,gd,tabn,ff_prot);
  fprintf(ff_prot,"Сумма удержаний в счёт этого месяца=%.2f\n",suma_uder);
  suma_str_dolg[2]+=suma_str_mes[0]+suma_uder;
  
  double koef=0.;
  if(suma_str_mes[2]+suma_uder != 0.)
    koef=suma_str_mes[2]/(suma_str_mes[0]+suma_uder);

  fprintf(ff_prot,"Коэффициент: %f=%.2f/(%.2f %.2f)\n",
  koef,suma_str_mes[2],suma_str_mes[0],suma_uder);

  if(mras == 1 && fabs(suma_str_mes[0]+suma_uder) < 0.009 )
    continue;
  i_suma_uder+=suma_uder;
  
  /*Смотрим начисления на фонд зарплаты*/
  sprintf(strsql,"select kodz,sumap from Zarsocz where datz='%04d-%02d-01' and tabn=%s",god_nah,mes_nah,row[0]);
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

  for(int nomm=0; nomm < RAZMER_MAS; nomm++)
    suma_str[nomm]+=suma_str_mes[nomm];

  /*Делаем расчёт налогов на долг*/
  if(koef != 0.)
   for(int nomm=3; nomm < RAZMER_MAS; nomm++)
     suma_str_dolg[nomm]+=suma_str_mes[nomm]*koef;

  fprintf(ff,"%-5d %-*.*s %10.2f %10.2f %10.2f\n",
  tabn,
  iceb_tu_kolbait(30,fio.ravno()),iceb_tu_kolbait(30,fio.ravno()),fio.ravno(),
  suma_str_mes[0],suma_uder,suma_str_mes[0]+suma_uder);

  fprintf(ff_soc,"%5s %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
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

  fprintf(ff_soc,"%*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  iceb_tu_kolbait(58,gettext("Долг")),
  gettext("Долг"),
  suma_str_dolg[2],
  suma_str_dolg[3],
  suma_str_dolg[4],
  suma_str_dolg[5],
  suma_str_dolg[6],
  suma_str_dolg[7],
  suma_str_dolg[8],
  suma_str_dolg[9],
  suma_str_dolg[10],
  suma_str_dolg[11]);



  for(int nomm=0; nomm < RAZMER_MAS; nomm++)
   {
    isuma_str[nomm]+=suma_str[nomm];
    isuma_str_dolg[nomm]+=suma_str_dolg[nomm];
   }  

 }

fprintf(ff,"\
----------------------------------------------------------------------\n");
fprintf(ff,"%*s %10.2f %10.2f %10.2f\n",
iceb_tu_kolbait(36,gettext("Итого")),
gettext("Итого"),
isuma_str[0],i_suma_uder,isuma_str[0]+i_suma_uder);

fprintf(ff_soc,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff_soc,"%*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
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

fprintf(ff_soc,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff_soc,"%*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
iceb_tu_kolbait(58,gettext("Итого долгов")),
gettext("Итого долгов"),
isuma_str_dolg[2],
isuma_str_dolg[3],
isuma_str_dolg[4],
isuma_str_dolg[5],
isuma_str_dolg[6],
isuma_str_dolg[7],
isuma_str_dolg[8],
isuma_str_dolg[9],
isuma_str_dolg[10],
isuma_str_dolg[11]);
 



podpis(ff);
fclose(ff);
podpis(ff_soc);
fclose(ff_soc);
podpis(ff_prot);
fclose(ff_prot);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Распечатка долгов"));

oth->spis_imaf.plus(imaf_soc);
oth->spis_naim.plus(gettext("Расчёт долгов по соц. фондам"));

oth->spis_imaf.plus(imaf_prot);
oth->spis_naim.plus(gettext("Протокол розрахунку"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),0);

return(0);
}
/****************************************/
/*Шапка файла*/
/******************************************/
void rasdolgi2_s(const char *data_nah,
short dd,short md,short gd,
const char *tabnom,
const char *podr,
FILE *ff)
{

struct  tm      *bf;
time_t tmm;
time(&tmm);
bf=localtime(&tmm);

fprintf(ff,"%s\n\
%s %s %s %d.%d.%d%s\n\
%s %d.%d.%d%s  %s: %02d:%02d\n",
iceb_t_get_pnk("00",0),
gettext("Долг по зарплате за"),
data_nah,
gettext("на"),
dd,md,gd,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

if(tabnom[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Табельный номер"),tabnom);

if(podr[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Подразделение"),podr);
 
}
