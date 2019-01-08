/* $Id: raspol.c,v 5.27 2013/09/26 09:43:39 sasa Exp $ */
/*11.04.2018	08.05.2001	Белых А.И.	raspol.c
Получение отчёта по начисленной зарплате мужчины/женщины
*/
#include        <errno.h>
#include        "buhg.h"
void raspol_rm(short metka,double tablic[4][3], 
int kolmug,int koltabel,int kolnah,class masiv_din_int *NASH,
class masiv_din_int *TABEL,class masiv_din_int *DNIMUG,class masiv_din_int *HASMUG,class masiv_din_int *KDNMUG,
class masiv_din_double *ZNAHMUG,double *itogo,FILE *ff,FILE *ffi);

extern short	mmm,ggg;

int raspol(class spis_oth *oth)
{
time_t          tmm;
int		kom1;
static class iceb_tu_str dat1("");
static class iceb_tu_str kod_pod("");
char		strsql[512];
SQL_str		row,row1,row2;
int		kolstr,kolstr1,kolstr2;
char		imaf[64],imafi[64];
short		mn,gn;
short		dkm=0;
double		tablic[4][3]; 
short		pol=0; //0-мужчина 1-женщина
double		sum=0.;
int		kolmug=0;
int		kolgen=0;
int		kolmug_s=0;
int		kolgen_s=0;
int		koltabel=0;
int		kodt,i;
class iceb_tu_str naimtab("");
int		kolnah=0;
class iceb_tu_str kod("");
class iceb_tu_str naim("");
int kom=0;
SQLCURSOR curr;
if(dat1.ravno()[0] == '\0')
 {
  sprintf(strsql,"%d.%d",mmm,ggg);
  dat1.new_plus(strsql);
 }

class VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт распределения начислений мужчина/женщина"));

VV.VVOD_SPISOK_add_MD(gettext("Введите дату (м.г)......"));
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения.(,,).."));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(kod_pod.ravno(),124);



naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
kod_pod.new_plus(VV.VVOD_SPISOK_return_data(1));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   iceb_t_pdoc("buhg5_1.txt");
   goto naz;

  case ESC:
  case FK10:
   return(1);


  case FK2:
  case PLU:
     break;


  case FK3:
    kod.new_plus("");
    naim.new_plus("");
    if(dirzarpodr(1,&kod,&naim) == 0)
     VV.data_z_plus(1,kod.ravno());
    goto naz;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;


  default:
     goto naz;
 }

if(rsdat1(&mn,&gn,dat1.ravno()) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz;
 }

GDITE();
clear();

time(&tmm);


//Создаем массивы для видов табеля

class masiv_din_int TABEL;

sprintf(strsql,"select kod from Tabel");


class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не введены виды табеля !"));
  return(1);  
 }

while(cur.read_cursor(&row) != 0)
 {
  koltabel++;
  TABEL.plus(atoi(row[0]),-1);
 }

class masiv_din_int DNIMUG;
class masiv_din_int HASMUG;
class masiv_din_int KDNMUG;
class masiv_din_int DNIMUG_S;
class masiv_din_int HASMUG_S;
class masiv_din_int KDNMUG_S;

class masiv_din_int DNIGEN;
class masiv_din_int HASGEN;
class masiv_din_int KDNGEN;
class masiv_din_int DNIGEN_S;
class masiv_din_int HASGEN_S;
class masiv_din_int KDNGEN_S;

DNIMUG.make_class(koltabel);
HASMUG.make_class(koltabel);
KDNMUG.make_class(koltabel);
DNIMUG_S.make_class(koltabel);
HASMUG_S.make_class(koltabel);
KDNMUG_S.make_class(koltabel);

DNIGEN.make_class(koltabel);
HASGEN.make_class(koltabel);
KDNGEN.make_class(koltabel);

DNIGEN_S.make_class(koltabel);
HASGEN_S.make_class(koltabel);
KDNGEN_S.make_class(koltabel);


//Создаем массивы для видов начислений

class masiv_din_int NASH;

sprintf(strsql,"select kod from Nash");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не введены начисления !"));
  return(1);  
 }

while(cur.read_cursor(&row) != 0)
 {
  kolnah++;
  NASH.plus(atoi(row[0]),-1);
 }

class masiv_din_double ZNAHMUG;
class masiv_din_double ZNAHMUG_S;
class masiv_din_double ZNAHGEN;
class masiv_din_double ZNAHGEN_S;
ZNAHMUG.make_class(kolnah);
ZNAHMUG_S.make_class(kolnah);
ZNAHGEN.make_class(kolnah);
ZNAHGEN_S.make_class(kolnah);

memset(&tablic,'\0',sizeof(tablic));
sprintf(strsql,"select tabn,datk,sovm,podr from Zarn where god=%d and mes=%d",gn,mn);

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

char imaf_prot[64];
sprintf(imaf_prot,"pol%d.lst",getpid());

class iceb_fopen ff_prot;
if(ff_prot.start(imaf_prot,"w") != 0)
 return(1);
 
short metka_sovm=0;
kolstr1=0;
class SQLCURSOR cur1;  
double suma_r=0.; /*сумма начисленная одному работнику*/
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(kod_pod.ravno(),row[3],0,0) != 0)
   continue;
  metka_sovm=atoi(row[2]);
  
  sprintf(strsql,"select pl,fio from Kartb where tabn=%s",row[0]);
  pol=0;
  
  if(readkey(strsql,&row1,&curr) == 1)
    pol=atoi(row1[0]);
  else
   {
    printw("%s %s\n",gettext("Не найден табельный номер"),row[0]);
    OSTANOV();
    continue;
   }

  if(pol == 0)
   {
    fprintf(ff_prot.ff,"\n%-5s %-5s %-*.*s мужчина",row[0],row[3],iceb_tu_kolbait(30,row1[1]),iceb_tu_kolbait(30,row1[1]),row1[1]);
   }
 
  if(pol == 1)
   {
    fprintf(ff_prot.ff,"\n%-5s %-5s %-*.*s женщина",row[0],row[3],iceb_tu_kolbait(30,row1[1]),iceb_tu_kolbait(30,row1[1]),row1[1]);
   }   
  
  printw("%6s %s\n",row[0],row1[1]);
  refresh();
  
  if(metka_sovm == 1)
   fprintf(ff_prot.ff," %s\n",gettext("совместитель"));
  else
   fprintf(ff_prot.ff,"\n");

  //Читаем отработанное время
  sprintf(strsql,"select kodt,dnei,has,kdnei from Ztab where god=%d and \
mes=%d and tabn=%s",gn,mn,row[0]);
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }

  int it=0;

  while(cur1.read_cursor(&row1) != 0)
   {
    kodt=atoi(row1[0]);
    i=TABEL.find(kodt);

    it+=atoi(row1[1])+atoi(row1[2])+atoi(row1[3]);
    
    if(pol == 0)
     {
      DNIMUG.plus(atoi(row1[1]),i);
      HASMUG.plus(atoi(row1[2]),i);
      KDNMUG.plus(atoi(row1[3]),i);
      if(metka_sovm == 1)
       {
        DNIMUG_S.plus(atoi(row1[1]),i);
        HASMUG_S.plus(atoi(row1[2]),i);
        KDNMUG_S.plus(atoi(row1[3]),i);
       }
     }
    if(pol == 1)
     {
      DNIGEN.plus(atoi(row1[1]),i);
      HASGEN.plus(atoi(row1[2]),i);
      KDNGEN.plus(atoi(row1[3]),i);
      if(metka_sovm == 1)
       {
        DNIGEN_S.plus(atoi(row1[1]),i);
        HASGEN_S.plus(atoi(row1[2]),i);
        KDNGEN_S.plus(atoi(row1[3]),i);
       }
     }
   }

  if(it != 0) /*если введён табель то человек не уволен даже если ему ничего не начислено*/
   {
    fprintf(ff_prot.ff,"Введён табель\n");
    if(pol == 0)
     {
      kolmug++;  
      if(metka_sovm == 1)
       kolmug_s++;
     }
    if(pol == 1)
     {
      kolgen++;  
      if(metka_sovm == 1)
       kolgen_s++;
     }
   }
  else
   fprintf(ff_prot.ff,"Не введён талбель\n");
     
  sprintf(strsql,"select knah,suma from Zarp where datz >= '%d-%d-1' \
and datz <= '%d-%d-31' and tabn=%s and prn='1' and suma <> 0.",
  gn,mn,gn,mn,row[0]);

  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }
  suma_r=0.;
  while(cur1.read_cursor(&row1) != 0)
   {
//    printw("%s %s\n",row1[0],row1[1]);
//    refresh();
    suma_r+=sum=atof(row1[1]);
    sprintf(strsql,"select vidn from Nash where kod=%s",row1[0]);
    if(readkey(strsql,&row2,&curr) != 1)
     {
      printw("%s %s !",gettext("Не найден код начисления"),row1[0]);
      OSTANOV();
     }
    kom1=atoi(row2[0]);
    tablic[pol][kom1]+=sum;
    if(metka_sovm == 1)
     tablic[pol+2][kom1]+=sum;
    
    i=NASH.find(atoi(row1[0]));
    if(pol == 0)
     { 
      ZNAHMUG.plus(sum,i);
      if(metka_sovm == 1)
       ZNAHMUG_S.plus(sum,i);
     }
    if(pol == 1)
     { 
      ZNAHGEN.plus(sum,i);
      if(metka_sovm == 1)
        ZNAHGEN_S.plus(sum,i);
      
     }

   }    
  fprintf(ff_prot.ff,"Начислено %.2f\n",suma_r);
 }

ff_prot.end();

GDITE();

class iceb_fopen fil;

sprintf(imaf,"m-g%d.lst",getpid());

if(fil.start(imaf,"w") != 0)
 return(1);

dpm(&dkm,&mn,&gn,5);

iceb_t_zagolov(gettext("Расчёт распределения начислений мужчина/женщина"),1,mn,gn,dkm,mn,gn,fil.ff);


class iceb_fopen oi;
sprintf(imafi,"poli%d.lst",getpid());
if(oi.start(imafi,"w") != 0)
 return(1);

iceb_t_zagolov(gettext("Расчёт распределения начислений мужчина/женщина"),1,mn,gn,dkm,mn,gn,oi.ff);

if(kod_pod.getdlinna() > 1)
 {
  fprintf(fil.ff,"%s:%s\n",gettext("Код подразделения"),kod_pod.ravno());
  fprintf(oi.ff,"%s:%s\n",gettext("Код подразделения"),kod_pod.ravno());
 }

double itogo_mug=0.;
double itogo_mug_sov=0.;
double itogo_gen=0.;
double itogo_gen_sov=0.;

raspol_rm(0,tablic,kolmug,koltabel,kolnah,&NASH,&TABEL,&DNIMUG,&HASMUG,&KDNMUG,&ZNAHMUG,&itogo_mug,fil.ff,oi.ff);
raspol_rm(2,tablic,kolmug_s,koltabel,kolnah,&NASH,&TABEL,&DNIMUG_S,&HASMUG_S,&KDNMUG_S,&ZNAHMUG_S,&itogo_mug_sov,fil.ff,oi.ff);

raspol_rm(1,tablic,kolgen,koltabel,kolnah,&NASH,&TABEL,&DNIGEN,&HASGEN,&KDNGEN,&ZNAHGEN,&itogo_gen,fil.ff,oi.ff);
raspol_rm(3,tablic,kolgen_s,koltabel,kolnah,&NASH,&TABEL,&DNIGEN_S,&HASGEN_S,&KDNGEN_S,&ZNAHGEN_S,&itogo_gen_sov,fil.ff,oi.ff);


printw("%s:%.2f\n",gettext("Ощая сумма всех начислений"),itogo_gen+itogo_mug);

fprintf(oi.ff,"%*s:%12.2f\n",
iceb_tu_kolbait(40,gettext("Ощая сумма всех начислений")),
gettext("Ощая сумма всех начислений"),itogo_gen+itogo_mug);

fprintf(fil.ff,"\n%*s:%12.2f\n",
iceb_tu_kolbait(34,gettext("Ощая сумма всех начислений")),
gettext("Ощая сумма всех начислений"),itogo_gen+itogo_mug);

podpis(fil.ff);
fil.end();

podpis(oi.ff);
oi.end();

OSTANOV();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт распределения начислений мужчина/женщина"));
oth->spis_imaf.plus(imafi);
oth->spis_naim.plus(gettext("Итоги расчёта"));
oth->spis_imaf.plus(imaf_prot);
oth->spis_naim.plus(gettext("Протокол расчёта"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),0);

return(0);

}

/*******************************************************/
/****************************************************/
void raspol_rm(short metka,/*0-мужчины 2-мужчины совместители 1-женщины 3-женщины совместители*/
double tablic[4][3],
int kolmug,
int koltabel,
int kolnah,
class masiv_din_int *NASH,
class masiv_din_int *TABEL,
class masiv_din_int *DNIMUG,
class masiv_din_int *HASMUG,
class masiv_din_int *KDNMUG,
class masiv_din_double *ZNAHMUG,
double *itogo,
FILE *ff,FILE *ffi)
{
SQL_str row;
class SQLCURSOR curr;
char strsql[1024];

if(metka == 0)
 {
  fprintf(ff,"\n%s\n",gettext("Пол:мужчина"));
  printw("\n%s\n",gettext("Пол:мужчина"));
 }
if(metka == 2)
 {
  fprintf(ff,"\n%s\n",gettext("В том числе мужчины совместители"));
  printw("\n%s\n",gettext("В том числе мужчины совместители"));
 }
if(metka == 1)
 {
  fprintf(ff,"\n%s\n",gettext("Пол:женщина"));
  printw("\n%s\n",gettext("Пол:женщина"));
 }
if(metka == 3)
 {
  fprintf(ff,"\n%s\n",gettext("В том числе женщины совместители"));
  printw("\n%s\n",gettext("В том числе женщины совместители"));
 }

fprintf(ff,"----------------------------------------------------------\n");

fprintf(ff,"\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%10d\n\n",
iceb_tu_kolbait(15,gettext("Основная")),gettext("Основная"),
tablic[metka][0],
iceb_tu_kolbait(15,gettext("Дополнительная")),gettext("Дополнительная"),
tablic[metka][1],
iceb_tu_kolbait(15,gettext("Прочая")),gettext("Прочая"),
tablic[metka][2],
iceb_tu_kolbait(15,gettext("Итого")),gettext("Итого"),
tablic[metka][0]+tablic[metka][1]+tablic[metka][2],
iceb_tu_kolbait(15,gettext("Количество")),gettext("Количество"),kolmug);

printw("\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%10d\n\n",
iceb_tu_kolbait(15,gettext("Основная")),gettext("Основная"),
tablic[metka][0],
iceb_tu_kolbait(15,gettext("Дополнительная")),gettext("Дополнительная"),
tablic[metka][1],
iceb_tu_kolbait(15,gettext("Прочая")),gettext("Прочая"),
tablic[metka][2],
iceb_tu_kolbait(15,gettext("Итого")),gettext("Итого"),
tablic[metka][0]+tablic[metka][1]+tablic[metka][2],
iceb_tu_kolbait(15,gettext("Количество")),gettext("Количество"),kolmug);

int s1=0,s2=0,s3=0;
int kodt=0;
class iceb_tu_str naimtab("");
for(int i=0; i < koltabel; i++)
 {
  kodt=TABEL->ravno(i);
  sprintf(strsql,"select naik from Tabel where kod=%d",kodt);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    naimtab.new_plus(row[0]);
  else
    naimtab.new_plus("");

  fprintf(ff,"%3d %-*.*s %10d %10d %10d\n",
  kodt,
  iceb_tu_kolbait(30,naimtab.ravno()),iceb_tu_kolbait(30,naimtab.ravno()),naimtab.ravno(),
  DNIMUG->ravno(i),HASMUG->ravno(i),
  KDNMUG->ravno(i));
  s1+=DNIMUG->ravno(i);
  s2+=HASMUG->ravno(i);
  s3+=KDNMUG->ravno(i);

 }
fprintf(ff,"%3s %*s %10d %10d %10d\n\n",
" ",iceb_tu_kolbait(30,gettext("Итого")),gettext("Итого"),s1,s2,s3);
class iceb_tu_str vidn("");
double sum=0.;
for(int i=0; i < kolnah; i++)
 {
  if(ZNAHMUG->ravno(i) == 0.)
    continue;
  kodt=NASH->ravno(i);
  sprintf(strsql,"select naik,vidn from Nash where kod=%d",kodt);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
   {
    naimtab.new_plus(row[0]);
    if(atoi(row[1]) == 0)
     vidn.new_plus("основная");
    if(atoi(row[1]) == 1)
     vidn.new_plus("дополнительная");
    if(atoi(row[1]) == 2)
     vidn.new_plus("прочая");
    }
  else
   {
    naimtab.new_plus("");
    vidn.new_plus("");
   }
  fprintf(ff,"%3d %-*.*s %12.2f %s\n",
  kodt,
  iceb_tu_kolbait(30,naimtab.ravno()),iceb_tu_kolbait(30,naimtab.ravno()),naimtab.ravno(),
  ZNAHMUG->ravno(i),vidn.ravno());
  sum+=ZNAHMUG->ravno(i);
 }

fprintf(ff,"%3s %*s %12.2f\n"," ",iceb_tu_kolbait(30,gettext("Итого")),gettext("Итого"),sum);
*itogo=sum;

if(metka == 0)
 fprintf(ffi,"%*s:%12.2f\n",
iceb_tu_kolbait(40,gettext("Мужчины")),
gettext("Мужчины"),
sum);

if(metka == 2)
  fprintf(ffi,"%*s:%12s %12.2f\n",
iceb_tu_kolbait(40,gettext("В том числе мужчины совместители")),
gettext("В том числе мужчины совместители"),
"",
sum);

if(metka == 1)
 fprintf(ffi,"%*s:%12.2f\n",
iceb_tu_kolbait(40,gettext("Женщины")),
gettext("Мужчины"),
sum);

if(metka == 3)
  fprintf(ffi,"%*s:%12s %12.2f\n",
iceb_tu_kolbait(40,gettext("В том числе женщины совместители")),
gettext("В том числе женщины совместители"),
"",
sum);

}
