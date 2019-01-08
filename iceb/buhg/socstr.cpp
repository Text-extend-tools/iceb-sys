/* $Id: socstr.c,v 5.50 2013/09/26 09:43:40 sasa Exp $ */
/*11.07.2017	04.04.2000	Белых А.И.	socstr.c
Получение отчёта по всем отчислениям в социальные фоды
за любой период времени
*/
#include        <math.h>
#include        <errno.h>
#include        "buhg.h"

double	socnul(short,short,short,short,short,FILE*);
void  socstrh(FILE *ff);
void socstr_rek(const char *podr,const char *tabnom,const char *kodf,FILE *ff1);
double socstr_rud(short mn,short gn,short mk,short gk,int tabnom,int kodud);
void socstr_rn(short mn,short gn,short mk,short gk,const char *podr,const char *tabnom,const char *kodf,char *imaf,class SPISOK *kodsocm,double *sumasn,double *sumasnb,double *kodsum,double *kodsumb);
void socstr_hi(FILE *ff);

extern double   okrg;
extern short	kodfsons;   /*Код фонда страхования от несчасного случая*/
extern char     *shetb; /*Бюджетные счета начислений*/


extern int kod_esv; /*Код удержания единого социального взноса*/
extern int kod_esv_bol; /*Код удержания единого социального взноса*/
extern int kod_esv_inv; /*Код удержания единого социального взноса*/
extern int kod_esv_dog; /*Код удержания единого социального взноса*/
extern int kod_esv_vs;

extern int kodf_esv; /*Код фонда удержания единого социального взноса*/
extern int kodf_esv_bol; /*Код фонда удержания единого социального взноса*/
extern int kodf_esv_inv; /*Код фонда удержания единого социального взноса*/
extern int kodf_esv_dog; /*Код фонда удержания единого социального взноса*/
extern int kodf_esv_vs;






void socstr(short mn,short gn,short mk,short gk,const char *podr,
const char *tabnom,const char *kodf,char *imaf_dol,FILE *ff,FILE *ff1)
{
int kodbzr=0;  /*Код отчисления на безработицу*/
int kodbezf=0;  /*Код фонда занятости*/
int kodsoc=0;   /*Код фонда социального страхования*/
int kodsocstr=0;  /*Код отчисления на соц-страх*/
int kodpenf=0; /*Код пенсионного фонда*/
int kodpen=0;  /*Код пенсионных отчислений*/
class SPISOK kodsocm; /*Код фонда|процент|счет*/
short		kolsoc;
int		i;
long		kolstr,kolstr1;
SQL_str         row,row1;
char		strsql[1024];
class iceb_tu_str naim("");
class iceb_tu_str shet("");
double		itogo,itogob,itogsn;
class iceb_tu_str procent("");
class iceb_tu_str kod("");
class iceb_tu_str bros("");
short		d,m,g;
class iceb_tu_str tabn("");
class iceb_tu_str fio("");
short		dkm;
double		sum1,sum2,sum3;
double		suma;
short		kodz=0;
short		kodzz=0;
double		sumkodz[5];
double		sumproc[5];
class iceb_tu_str naimf("");
double		sumanah=0.;
double		isumanah=0.;
double		isumaproc=0.;
char		metkasum[8];

GDITE();

/*Определяем количество начислений и удержаний*/
sprintf(strsql,"select kod from Zarsoc");
class SQLCURSOR cur;
if((kolsoc=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolsoc == 0)
 {
  iceb_t_soob(gettext("Не введены коды социальных отчислений !"));
  return;
 }

sprintf(strsql,"select distinct kodz,proc,shet from Zarsocz where \
datz >= '%04d-%d-01' and datz <= '%04d-%d-31'",gn,mn,gk,mk);

if((kolsoc=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolsoc == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;
 }

i=0;
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s|%s|%s",row[0],row[1],row[2]);
  kodsocm.plus(strsql);
 }

double	kodsum[kolsoc]; //Сумма перечисления в фонд
memset(&kodsum,'\0',sizeof(kodsum));

double sumasn[kolsoc]; //Сумма с которой начислялось
double sumasnb[kolsoc]; //Сумма с которой начислялось бюджет
memset(&sumasn,'\0',sizeof(sumasn));
memset(&sumasnb,'\0',sizeof(sumasnb));

double	kodsumb[kolsoc]; //Сумма перечисления в фонд с бюджетной части
memset(&kodsumb,'\0',sizeof(kodsumb));


sprintf(strsql,"select tabn,kodz,sumap,proc,datz,shet,sumas,sumapb,sumasb from \
Zarsocz where datz >= '%04d-%d-01' and datz <= '%04d-%d-31' \
order by kodz,proc,tabn asc",gn,mn,gk,mk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;
 }

dpm(&dkm,&mk,&gk,5);


iceb_t_poldan("Код пенсионного отчисления",&kodpen,"zarnast.alx");
iceb_t_poldan("Код пенсионного фонда",&kodpenf,"zarnast.alx");

iceb_t_poldan("Код фонда социального страхования",&kodsoc,"zarnast.alx");
iceb_t_poldan("Код отчисления в соц-страх",&kodsocstr,"zarnast.alx");
iceb_t_poldan("Код фонда занятости",&kodbezf,"zarnast.alx");
iceb_t_poldan("Код отчисления на безработицу",&kodbzr,"zarnast.alx");



iceb_t_zagolov(gettext("Расчёт начисления ЕСВ"),1,mn,gn,dkm,mk,gk,ff);

socstr_rek(podr,tabnom,kodf,ff);


iceb_t_zagolov(gettext("Расчёт начисления ЕСВ"),1,mn,gn,dkm,mk,gk,ff1);
socstr_rek(podr,tabnom,kodf,ff1);


fprintf(ff1,"\n\
Расшифровка колонок:\n\
1-код фонда\n\
2-общая сумма всех начислений\n\
3-сумма взятая в расчёт\n\
4-сумма налога с фонда оплаты\n\
5-сумма налога для бюджета\n\
6-процент налога\n\
7-счет\n\
8-дата записи\n\
9-сумма налога с работника\n\n");

sum1=sum2=sum3=0.;
double bb=0.;

int nomerstr=0;
SPISOK FONDSHET(0); //Список код фонда-счет
masiv_din_double SUMA(0); //Массив сумм к списку код фонда-счет
float procz=0.,proc=0.;
memset(&sumkodz,'\0',sizeof(sumkodz));
memset(&sumproc,'\0',sizeof(sumproc));
kolstr1=0;
SQLCURSOR curr;
while(cur.read_cursor(&row) != 0)
 {
//printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(tabnom,row[0],0,0) != 0)
    continue;

  if(proverka(kodf,row[1],0,0) != 0)
    continue;

  if(podr[0] != '\0')
   {
    
    rsdat(&d,&m,&g,row[4],2);

    sprintf(strsql,"select podr from Zarn where god=%d and mes=%d and tabn=%s",g,m,row[0]);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
      continue;    
    kod.new_plus(row1[0]);

    if(proverka(podr,kod.ravno(),0,0) != 0)
      continue;
    
   }
  kodz=atoi(row[1]);
  proc=atof(row[3]);
  if(procz != proc)
   {

    isumaproc=0.;
    memset(&sumproc,'\0',sizeof(sumproc));
    procz=proc;
   }
  
  if(kodz != kodzz)
   {

    if(kodzz != 0)
     {
      fprintf(ff1,"%*.*s:%11.2f %11.2f %10.2f %10.2f %29s %10.2f\n",
      iceb_tu_kolbait(39,naimf.ravno()),iceb_tu_kolbait(39,naimf.ravno()),naimf.ravno(),
      sumkodz[0],sumkodz[1],sumkodz[2],sumkodz[3],"",sumkodz[4]);
      
      if((bb=socnul(mn,gn,mk,gk,kodzz,ff1)) != 0.)
       {
        fprintf(ff1,"%*s:%11.2f\n",
        iceb_tu_kolbait(39,gettext("Общий итог по фонду")),gettext("Общий итог по фонду"),sumkodz[0]+bb);

       }


     }

    //Читаем наименование фонда
    sprintf(strsql,"select naik from Zarsoc where kod=%d",kodz);
    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
      naimf.new_plus(row1[0]);
    else
      naimf.new_plus(""); 
    fprintf(ff1,"%d %s\n",kodz,naimf.ravno());
    socstrh(ff1);
  //  isumanah=0.;
    memset(&sumkodz,'\0',sizeof(sumkodz));
    kodzz=kodz;
   }

  sprintf(strsql,"%s|%s|%s",row[1],row[3],row[5]);

  if((i=kodsocm.find(strsql)) < 0)
   {
    beep();
    printw("Не найдено %s в массиве кодов фондов/процент !\n",strsql);
    OSTANOV();  
    continue;
   }       
  suma=atof(row[2]);
  suma=okrug(suma,okrg);
  
  kodsum[i]+=suma;
  sum2+=suma;
  sumkodz[2]+=suma;
  sumproc[2]+=suma;

  suma=atof(row[6]);
  suma=okrug(suma,okrg);
  sumasn[i]+=suma;
  sumasnb[i]+=atof(row[8]);
  sum1+=suma;
  sumkodz[1]+=suma;
  sumproc[1]+=suma;

  suma=atof(row[7]);
  suma=okrug(suma,okrg);
  kodsumb[i]+=suma;
  sum3+=suma;
  sumkodz[3]+=suma;
  sumproc[3]+=suma;

  if(SRAV(tabn.ravno(),row[0],0) != 0)
   {
    sprintf(strsql,"select fio,shet from Kartb where tabn=%s",row[0]);
    if(readkey(strsql,&row1,&curr) != 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найдено табельный номер"),row[0]);
      iceb_t_soob(strsql);
      continue;      
     }
    fio.new_plus(row1[0]);
    shet.new_plus(row1[1]);
   }
  rsdat(&d,&m,&g,row[4],2);
  isumanah+=sumanah=sosnsum(row[0],1,m,g,31,m,g);
  isumaproc+=sumanah;
  
  sumkodz[0]+=sumanah;
  sumproc[0]+=sumanah;
  
  memset(metkasum,'\0',sizeof(metkasum));
  if(fabs(sumanah-atof(row[6])) > 0.009)
     metkasum[0]='*';
  else
     metkasum[0]=' ';

  
  double suma_sr=0.; /*Сумма с работника*/
  
  if(kodpenf  == kodz)
    suma_sr=socstr_rud(mn,gn,mk,gk,atoi(row[0]),kodpen);
  
  if(kodsoc  == kodz)
    suma_sr=socstr_rud(mn,gn,mk,gk,atoi(row[0]),kodsocstr);
  
  if(kodbezf  == kodz)
    suma_sr=socstr_rud(mn,gn,mk,gk,atoi(row[0]),kodbzr);
  /********************/
  if(kodf_esv  == kodz)
    suma_sr=socstr_rud(mn,gn,mk,gk,atoi(row[0]),kod_esv);
  if(kodf_esv_bol  == kodz)
    suma_sr=socstr_rud(mn,gn,mk,gk,atoi(row[0]),kod_esv_bol);
  if(kodf_esv_inv  == kodz)
    suma_sr=socstr_rud(mn,gn,mk,gk,atoi(row[0]),kod_esv_inv);
  if(kodf_esv_dog  == kodz)
    suma_sr=socstr_rud(mn,gn,mk,gk,atoi(row[0]),kod_esv_dog);
  if(kodf_esv_vs  == kodz)
    suma_sr=socstr_rud(mn,gn,mk,gk,atoi(row[0]),kod_esv_vs);
  
  sumkodz[4]+=suma_sr;
  sumproc[4]+=suma_sr;
    
  fprintf(ff1,"%5s %-*.*s %2s %11.2f %11s%s%10s %10s %5s%% %-6s %s %10.2f\n",
  row[0],
  iceb_tu_kolbait(30,fio.ravno()),iceb_tu_kolbait(30,fio.ravno()),fio.ravno(),
  row[1],sumanah,row[6],metkasum,row[2],row[7],row[3],row[5],iceb_tu_datzap(row[4]),suma_sr);

  tabn.new_plus(row[0]);

  suma=atof(row[2]);
  suma=okrug(suma,okrg);
  sprintf(strsql,"%d|%s",kodzz,shet.ravno());
  if((nomerstr=FONDSHET.find(strsql)) == -1)
   {
    FONDSHET.plus(strsql);
    SUMA.plus(suma,-1);
   }    
  else    
    SUMA.plus(suma,nomerstr);
 }

GDITE();

fprintf(ff1,"%*s:%11.2f %11.2f %10.2f %10.2f %29s %10.2f\n",
iceb_tu_kolbait(39,naimf.ravno()),naimf.ravno(),sumkodz[0],sumkodz[1],sumkodz[2],sumkodz[3],"",sumkodz[4]);

if((bb=socnul(mn,gn,mk,gk,kodzz,ff1)) != 0.)
 {
  fprintf(ff1,"%*s:%11.2f\n",
  iceb_tu_kolbait(39,gettext("Общий итог по фонду")),gettext("Общий итог по фонду"),sumkodz[0]+bb);
 }
fprintf(ff1,"\
----------------------------------------------------------------------------------------------------------------\n");
fprintf(ff1,"%*s:%11.2f %11.2f %10.2f %10.2f\n",
iceb_tu_kolbait(39,gettext("Итого по всем фондам")),gettext("Итого по всем фондам"),isumanah+bb,sum1,sum2,sum3);

itogsn=itogo=itogob=0.;

socstr_hi(ff); /*шапка итоговой таблицы*/
socstr_hi(ff1); /*шапка итоговой таблицы*/

/*Распечатываем массивы*/
for(i=0; i < kolsoc ; i++)
 {
  if(kodsum[i] == 0.)
    continue;
  bros.new_plus(kodsocm.ravno(i));
  polen(bros.ravno(),&kod,1,'|');
  polen(bros.ravno(),&procent,2,'|');
  polen(bros.ravno(),&shet,3,'|');
  sprintf(strsql,"select naik,shet from Zarsoc where kod=%s",kod.ravno());
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
   {
    naim.new_plus(row1[0]);
   }
  else
   naim.new_plus("");

  printw("%3s %-*.*s %5s%% %-10s %11.2f %11.2f %10.2f\n",
  kod.ravno(),
  iceb_tu_kolbait(20,naim.ravno()),iceb_tu_kolbait(20,naim.ravno()),naim.ravno(),
  procent.ravno(),shet.ravno(),sumasn[i],kodsum[i],kodsumb[i]);

  fprintf(ff,"%3s %-*.*s %5s%% %-10s %11.2f %10.2f %11.2f/%11.2f %11.2f/%11.2f\n",
  kod.ravno(),
  iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
  procent.ravno(),shet.ravno(),sumasn[i],kodsum[i],kodsumb[i],
  sumasnb[i],
  kodsum[i]-kodsumb[i],
  sumasn[i]-sumasnb[i]);

  if(iceb_tu_strlen(naim.ravno()) > 30)
   fprintf(ff,"%3s %s\n","",iceb_tu_adrsimv(30,naim.ravno()));

  fprintf(ff1,"%3s %-*.*s %5s%% %-10s %11.2f %10.2f %11.2f/%11.2f %11.2f/%11.2f\n",
  kod.ravno(),
  iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
  procent.ravno(),shet.ravno(),
  sumasn[i],kodsum[i],kodsumb[i],
  sumasnb[i],
  kodsum[i]-kodsumb[i],
  sumasn[i]-sumasnb[i]);
  if(iceb_tu_strlen(naim.ravno()) > 30)
   fprintf(ff1,"%3s %s\n","",iceb_tu_adrsimv(30,naim.ravno()));
  itogo+=kodsum[i];
  itogob+=kodsumb[i];
  itogsn+=sumasn[i];
 }
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%11.2f %10.2f %11.2f %11s %11.2f\n",
iceb_tu_kolbait(53,gettext("Итого")),gettext("Итого"),itogsn,itogo,itogob,"",itogo-itogob);


fprintf(ff1,"\
-----------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff1,"%*s:%11.2f %10.2f %11.2f %11s %11.2f\n",
iceb_tu_kolbait(53,gettext("Итого")),gettext("Итого"),itogsn,itogo,itogob,"",itogo-itogob);

printw("%*s:%11.2f %11.2f %10.2f\n",
iceb_tu_kolbait(43,gettext("Итого")),gettext("Итого"),itogsn,itogo,itogob);

socstr_rn(mn,gn,mk,gk,podr,tabnom,kodf,imaf_dol,&kodsocm,sumasn,sumasnb,kodsum,kodsumb);

}
/*************************************************************************/
/*Получение списка имеющих начисления и не имеющих отчислений в соц фонды*/
/*************************************************************************/

double	socnul(short mn,short gn,short mk,short gk,
short	kodsf,
FILE *ff)
{
char	strsql[512];
int	kolstr=0;
SQL_str	row,row1;
class iceb_tu_str fio("");
double  suma=0.;
double  isuma=0.;
SQLCURSOR curr;
sprintf(strsql,"select distinct tabn from Zarp where \
datz >= '%04d-%d-%d' and datz <= '%04d-%d-%d' and prn='1'",
gn,mn,1,gk,mk,31);

SQLCURSOR cur;
SQLCURSOR cur1;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(0.);
 }
fprintf(ff,"\n%s:\n",
gettext("Список работников не имеющих отчислений в соц. фонд"));

while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);

  sprintf(strsql,"select distinct kodz from Zarsocz where \
datz >= '%04d-%d-01' and datz <= '%04d-%d-31' and tabn=%s and kodz=%d",
  gn,mn,gk,mk,row[0],kodsf);
  if(readkey(strsql,&row1,&curr) == 0)
   {
    sprintf(strsql,"select fio from Kartb where tabn=%s",row[0]);
    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
     fio.new_plus(row1[0]);
    else
     fio.new_plus("");

    suma=sosnsum(row[0],1,mn,gn,31,mk,gk);
    isuma+=suma;
    
    if(suma != 0.)
     fprintf(ff,"%5s %-*.*s %2s %11.2f\n",
     row[0],
     iceb_tu_kolbait(30,fio.ravno()),
     iceb_tu_kolbait(30,fio.ravno()),
     fio.ravno(),
     " ",suma);

   }

 }

if(isuma != 0. && isuma > suma)
 fprintf(ff,"%*s:%11.2f\n",iceb_tu_kolbait(39,gettext("Итого по не имеющим начисления")),gettext("Итого по не имеющим начисления"),isuma);

return(isuma); 
}
/*******************************/
/*шапка*/
/*******************************/
void  socstrh(FILE *ff)
{
fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
 Т/н |Фамилия, имя, отчество        |1 |     2     |     3     |    4     |     5    |  6   |     7     |     8    |    9     |\n");
fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------\n");
}
/*************************/
/*Реквизиты расчёта*/
/*************************/
void socstr_rek(const char *podr,
const char *tabnom,
const char *kodf,
FILE *ff1)
{

if(podr[0] != '\0')
 {
  fprintf(ff1,"%s:%s\n",gettext("Коды подразделений вошедшие в расчёт"),podr);
  int klst=0;
  printcod(ff1,"Podr","kod","naik",0,podr,&klst);
 }
else
 fprintf(ff1,gettext("По всем подразделениям.\n"));

if(tabnom[0] != '\0')
 fprintf(ff1,"%s:%s\n",gettext("Табельные номера вошедшие в расчёт"),tabnom);
if(kodf[0] != '\0')
 fprintf(ff1,"%s:%s\n",gettext("Коды фондов вошедшие в расчёт"),kodf);

}
/**********************/
/*Поиск суммы отчисления в фонд с работника*/
/********************************************/

double socstr_rud(short mn,short gn,short mk,short gk,int tabnom,int kodud)
{
SQL_str row;
class SQLCURSOR cur;

char strsql[512];
int kolstr=0;

sprintf(strsql,"select suma from Zarp where datz >= '%04d-%d-%d' and datz <= '%04d-%d-%d' and tabn=%d and prn='2' and knah=%d and suma <> 0.",
gn,mn,1,gk,mk,31,tabnom,kodud);
//printw("strsql=%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(0.);
 }

double suma=0.;
while(cur.read_cursor(&row) != 0)
 {
  suma+=atof(row[0]);  
 }

return(suma);
}

/***************************************/
/*Расчёт долей*/
/****************************************/

void socstr_rn(short mn,short gn,short mk,short gk,const char *podr,
const char *tabnom,const char *kodf,char *imaf,
class SPISOK *kodsocm,
double *sumasn,
double *sumasnb,
double *kodsum,
double *kodsumb)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
char kodsoc[64];
char stroka[1024];
char stroka1[1024];
int nomer_fonda=0;

sprintf(strsql,"select str from Alx where fil='socstr.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"socstr.alx");
  iceb_t_soob(strsql);
  return;
 }

/*Определяем количество настроек*/
class masiv_din_int kod_fond;

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(SRAV("Код фонда|",row_alx[0],1) == 0)
   {
    polen(row_alx[0],kodsoc,sizeof(kodsoc),2,'|');
    if(atoi(kodsoc) == 0)
     continue;
    kod_fond.plus(kodsoc);
   }
 }
if(kod_fond.kolih() == 0)
 {
  return;
 }
/*Суммируем по одинаковым кодам фондов*/
//class masiv_din_int kodsocm1;
double sumasn1[kod_fond.kolih()];
double sumasnb1[kod_fond.kolih()];
double kodsum1[kod_fond.kolih()];
double kodsumb1[kod_fond.kolih()];

memset(sumasn1,'\0',sizeof(sumasn1));
memset(sumasnb1,'\0',sizeof(sumasnb1));
memset(kodsum1,'\0',sizeof(kodsum1));
memset(kodsumb1,'\0',sizeof(kodsumb1));

/*код фонда может быть несколко раз с разными процентами*/
for(int nomer_fonda=0; nomer_fonda < kod_fond.kolih(); nomer_fonda++)
 for(int nomer_fonda1=0; nomer_fonda1 < kodsocm->kolih(); nomer_fonda1++)
 {
  polen(kodsocm->ravno(nomer_fonda1),kodsoc,sizeof(kodsoc),1,'|');
  if(kod_fond.ravno(nomer_fonda) != atoi(kodsoc))
    continue;
 
  sumasn1[nomer_fonda]+=sumasn[nomer_fonda1];
  sumasnb1[nomer_fonda]+=sumasnb[nomer_fonda1];
  kodsum1[nomer_fonda]+=kodsum[nomer_fonda1];
  kodsumb1[nomer_fonda]+=kodsumb[nomer_fonda1];
 }

printw("%s\n",gettext("Расчёт частей в социальных отчислениях"));

class SPISOK sp_naim[kod_fond.kolih()]; /*Список наименований настроек*/
class SPISOK sp_kodn[kod_fond.kolih()]; /*Список начислений*/
class masiv_din_double sum_fond[kod_fond.kolih()];
class masiv_din_double sum_fond_bud[kod_fond.kolih()];

/*Заряжаем списки настроек*/
nomer_fonda=0;
int kodfond=0;
int metka_kodf=0;
cur_alx.poz_cursor(0);
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(SRAV("Код фонда|",row_alx[0],1) == 0)
   {
    polen(row_alx[0],&kodfond,2,'|');
    if(kodfond == 0)
     {
      metka_kodf=0;
      continue;
     }
    metka_kodf=1;
    nomer_fonda=kod_fond.find(kodfond);

   }
  if(metka_kodf == 0)
   continue;
  if(SRAV("Наименование расчёта|",row_alx[0],1) == 0)
   {
    polen(row_alx[0],stroka1,sizeof(stroka1),2,'|');   
    if(stroka1[0] == '\0')
     continue;
    polen(row_alx[0],&sp_naim[nomer_fonda],2,'|');   
   }
  if(SRAV("Коды начислений|",row_alx[0],1) == 0)
   {
    polen(row_alx[0],stroka1,sizeof(stroka1),2,'|');   
    if(stroka1[0] == '\0')
     continue;
    polen(row_alx[0],&sp_kodn[nomer_fonda],2,'|');   
    sum_fond[nomer_fonda].plus(0.,-1);
    sum_fond_bud[nomer_fonda].plus(0.,-1);
   }
 }
short dkm=0;
dpm(&dkm,&mk,&gk,5);
SQL_str row;
class SQLCURSOR cur;
sprintf(stroka,"select tabn,knah,suma,shet,podr from Zarp where datz >= '%04d-%02d-%02d' and datz <= '%04d-%02d-%02d' \
and prn='1' and suma <> 0.",gn,mn,1,gk,mk,dkm);

if((kolstr=cur.make_cursor(&bd,stroka)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),stroka);
  return;
 }
int kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(tabnom,row[0],0,0) != 0)
   continue;
  if(proverka(podr,row[4],0,0) != 0)
   continue;

  for(int ii=0; ii < kod_fond.kolih();ii++)
   for(int kk=0; kk < sp_kodn[ii].kolih(); kk++)
   {
    if(proverka(sp_kodn[ii].ravno(kk),row[1],0,0) != 0)
      continue;

    sum_fond[ii].plus(atoi(row[2]),kk);
    if(proverka(shetb,row[3],0,1) == 0)
        sum_fond_bud[ii].plus(atoi(row[2]),kk);
       
   }
 }
FILE *ff;
sprintf(imaf,"fondd%d.lst",getpid());

if((ff=fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта

iceb_t_zagolov(gettext("Расчёт долей в соц. отчислениях"),1,mn,gn,dkm,mk,gk,ff);

double kof=0.;
double suma_vid=0.;
class iceb_tu_str naimf("");

for(int kf=0; kf < kod_fond.kolih(); kf++)
 {

  sprintf(stroka,"select naik from Zarsoc where kod=%d",kod_fond.ravno(nomer_fonda));

  if(readkey(stroka,&row,&cur) == 1)
   naimf.new_plus(row[0]);
  else
   naimf.new_plus("");   

  fprintf(ff,"\n%s:%d %s\n",gettext("Фонд"),kod_fond.ravno(nomer_fonda),naimf.ravno());
  fprintf(ff,"%s:%.2f\n",gettext("Сумма с которой начислено отчисление"),sumasn1[kf]);
  fprintf(ff,"%s:%.2f\n",gettext("Сумма отчисления"),kodsum1[kf]);

  for(int nomer_nast=0; nomer_nast < sp_kodn[nomer_fonda].kolih(); nomer_nast++)
   {
    fprintf(ff,"\n%s\n",sp_naim[nomer_fonda].ravno(nomer_nast));
    fprintf(ff,"%s:%s\n",gettext("Коды начислений, которые вошли в расчёт"),sp_kodn[nomer_fonda].ravno(nomer_nast));
    fprintf(ff,"%s:%.2f\n",gettext("Сумма начислений"),sum_fond[nomer_fonda].ravno(nomer_nast));
    kof=sumasn1[kf]/sum_fond[nomer_fonda].ravno(nomer_nast);
    
    fprintf(ff,"%s: %.2f/%.2f=%f\n",gettext("Расчёт доли"),
    sumasn1[kf],
    sum_fond[nomer_fonda].ravno(nomer_nast),
    kof);
    
    double suma_vid_ob=kodsum1[kf]/kof;
    fprintf(ff,"%s: %.2f/%f=%.2f\n",gettext("Сумма отчисления"),kodsum1[kf],kof,suma_vid_ob);        

    if(shetb != NULL)
     {
      fprintf(ff,"\n%s\n",gettext("Бюджет"));
 
      fprintf(ff,"%s:%.2f\n",gettext("Сумма начислений"),sum_fond_bud[nomer_fonda].ravno(nomer_nast));
      kof=0.;
      if(sum_fond_bud[nomer_fonda].ravno(nomer_nast) != 0)
        kof=sumasn1[kf]/sum_fond_bud[nomer_fonda].ravno(nomer_nast);
     
      fprintf(ff,"%s: %.2f/%.2f=%f\n",gettext("Расчёт доли"),
      sumasn1[kf],
      sum_fond_bud[nomer_fonda].ravno(nomer_nast),
      kof);

      double suma_vid_b=0.;

      if(kof != 0.)     
        suma_vid_b=kodsumb1[kf]/kof;

      fprintf(ff,"%s: %.2f/%f=%.2f\n",gettext("Сумма отчисления"),kodsumb1[kf],kof,suma_vid_b);        

      /*******************/

      fprintf(ff,"\n%s\n",gettext("Хозрасчёт"));
      fprintf(ff,"%s:%.2f\n",gettext("Сумма начислений"),sum_fond[nomer_fonda].ravno(nomer_nast)-sum_fond_bud[nomer_fonda].ravno(nomer_nast));

      kof=sumasn1[kf]/(sum_fond[nomer_fonda].ravno(nomer_nast)-sum_fond_bud[nomer_fonda].ravno(nomer_nast));
   
      fprintf(ff,"%s: %.2f/%.2f=%f\n",gettext("Расчёт доли"),
      sumasn1[kf],
      sum_fond[nomer_fonda].ravno(nomer_nast)-sum_fond_bud[nomer_fonda].ravno(nomer_nast),
      kof);
      
      suma_vid=suma_vid_ob-suma_vid_b;
      fprintf(ff,"%s: %.2f-%f=%.2f\n",gettext("Сумма отчисления"),
      suma_vid_ob,
      suma_vid_b,
      suma_vid);        
            
     }    
   }
 }

podpis(ff);

fclose(ff);

}
/***************************/
/*Шапка итоговой таблицы*/
/*************************/
void socstr_hi(FILE *ff)
{
fprintf(ff,"\n\
-----------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,
gettext("\
Код|       Наименование фонда     |Проц. |   Счёт    |Сумма в рас|Сумма ЕСВ |     Сумма бюджет      |     Сумма небюджет    |\n"));
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------\n");
}

