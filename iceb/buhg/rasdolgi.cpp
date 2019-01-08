/* $Id: rasdolgi.c,v 5.53 2013/09/26 09:43:38 sasa Exp $ */
/*28.03.2018	20.04.1999	Белых А.И.	rasdolgi.c
Получение списка долгов по зарплате организации перед своими
работниками
*/
#include <math.h>
#include        <errno.h>
#include        "buhg.h"

int rasdolgi2(const char *data_nah,const char *data_rash,const char *podr,const char *tabnom,int mras,class spis_oth *oth);
void rasitog(double*,double*,FILE*,int);

extern short	*obud; /*Обязательные удержания*/

extern double	ogrzp[2]; /*Разграниечение зарплаты для расчёта пенсионных*/
extern float    procpen[2]; /*Процент отчисления в пенсионный фонд*/
extern short    kodpn;   /*Код подоходного налога*/
extern short    kodperpn;   /*Код перерасчёта подоходного налога*/

int rasdolgi(class spis_oth *oth)
{
int kodfsons=0;   /*Код фонда страхования от несчасного случая*/
double procbez=0.; /*Процент отчисления на безработицу*/
int kodbezf=0;  /*Код фонда занятости*/
int kodbzr=0;  /*Код отчисления на безработицу*/

float procsoc=0; /*Процент отчисления в соц-страх*/
float procpen[2]={0.,0.}; /*Процент отчисления в пенсионный фонд*/
int kodpenf=0; /*Код пенсионного фонда*/
int kodpen=0;  /*Код пенсионных отчислений*/
double	ogrzp[2]={0.,0.}; /*Разграниечение зарплаты для расчёта пенсионных*/
int		i,ii;
short		dd,md,gd;
FILE		*ff=NULL,*ff1=NULL,*ff2=NULL;
char		imaf[64],imafprot[64],imaft[64],imafsvod[64];
class iceb_tu_str bros("");
double		sumn=0.,sumu=0.;
double		sumno,sumuo;
double		sumnop,sumuop;
double		osn,osu;
struct  tm      *bf;
time_t          tmm,tmmk;
short		pod;
static class iceb_tu_str mesc("");
static class iceb_tu_str tabn("");
static class iceb_tu_str kp("");
static class iceb_tu_str dat1("");
short		kd;
long		kolstr,kolstr1,kolstr2;
SQL_str         row,row1;
char		strsql[1024];
int		tbn;
int		kpd;
short		mras; /*1-только кому должны 2- всех*/
class iceb_tu_str fio("");
int		kom1;
double		sumasbp; /*Сумма с которой брались пенсионные*/
double		sumasbp1=0.; /*Сумма с которой брались пенсионные*/
double		penot,penot1m,penot2m,penot32,penot1,penot2;
double		penot4,penot4p;
double		sumasbpp; /*Сумма с которой брались пересчитанная*/
double		penot32p,penot1p,penot2p; /*Пересчитанные*/
short		kodz;
double		sfondsc,sfondscp,fondsc,fondscp;
double		sfondzn,sfondznp,fondzn,fondznp,fondzn05,fondzn05p,fondzn05m;
double		sfondsci,sfondscip,fondsci,fondscip;
double		sfondsons,sfondsonsp,fondsons,fondsonsp;
double		spodoh,spodohp;
double		koef; /*Коэффициент пересчета*/
double		sumaou;
#define		RAZMER 25
double		itog1[RAZMER],itog2[RAZMER];
double		itog1o[RAZMER],itog2o[RAZMER];
double		sum,sump;
short		kolpd=0;
short		metod; //0-по заданому списку 1-от заданной даты
short		d,m,g;
double		sfondsc1=0.;
double		sfondscp1=0.;
double		sfondscp2=0.;
double		sfondsc2=0.;
double		socstrah=0.;
double		sumasoc=0.;
class iceb_tu_str naipod("");
class iceb_tu_str fiov("");
SQLCURSOR       curr;
VVOD MENU(3);

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт долгов по выплате заработной платы"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начисления.(м.г,,)..."));
VV.VVOD_SPISOK_add_MD(gettext("Дата долга..(д.м.г)......."));
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения..(,,)..."));
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер....(,,)..."));

if(dat1.ravno()[0] == '\0')
 {
  time(&tmm);
  bf=localtime(&tmm);
  sprintf(strsql,"%d.%d.%d",bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900);
  dat1.new_plus(strsql);  
 }

VV.VVOD_SPISOK_add_data(mesc.ravno(),128);
VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(kp.ravno(),128);
VV.VVOD_SPISOK_add_data(tabn.ravno(),128);

naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F5",gettext("расчёт"),
"F10",gettext("выход"),NULL);

i=VV.vvod(0,1,1,-1,-1);

mesc.new_plus(VV.data_ravno(0));
dat1.new_plus(VV.data_ravno(1));
kp.new_plus(VV.data_ravno(2));
tabn.new_plus(VV.data_ravno(3));

class iceb_tu_str nm("");

switch(i)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp4_3_9.txt");
   goto naz;

  case FK2:
  case PLU:
    metod=0;
    break;

  case FK5:
    metod=1;
    break;

  case FK3:

    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка табельных номеров"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3(&MENU,&kom1,1,-1,-1) != 0);

    switch (kom1)
     {
      case 2 :
      case -1:
       goto naz;

      case 0 :
        if(dirzarpodr(2,&kp,&nm) == 0)
          VV.data_z_plus(2,kp.ravno());
        goto naz;

      case 1 :
      
        fiov.new_plus("");
        int tmm=0;
        if(dirtab(&tmm,&fiov,0,1) == 0)
         {
//          sprintf(bros,"%d",tmm);
//          VV.data_z_plus(3,bros);
          VV.data_z_plus(3,tmm);
         }
        goto naz;

     }
   
  case FK4:
   VV.VVOD_clear_data();
   goto naz;

  case ESC:
  case FK10:
    return(1);
  default:
    goto naz;
 }

if(VV.VVOD_SPISOK_return_data(0)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введено даты начисления !"));
  goto naz;
 }

if(rsdat(&dd,&md,&gd,dat1.ravno(),0) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата долга !"));
  goto naz;
 }


VVOD DANET(1);
 
DANET.VVOD_SPISOK_add_MD(gettext("Распечатать только кому должны ?"));

mras=danet(&DANET,2,stdscr);

if(rsdat1(&m,&g,mesc.ravno()) == 0)
 if(sravmydat(1,m,g,ICEB_DNDESV,ICEB_MNDESV,ICEB_GNDESV) >= 0)
  {
   return(rasdolgi2(mesc.ravno(),dat1.ravno(),kp.ravno(),tabn.ravno(),mras,oth));
  }
  
memset(imafprot,'\0',sizeof(imafprot));
sprintf(imafprot,"prot%d.lst",getpid());



time(&tmm);
bf=localtime(&tmm);

int kodsoci=0;   /*Код социального страхования инвалидов*/
int kodsoc=0;   /*Код фонда социального страхования*/
int kodsocstr=0;
iceb_t_poldan("Код отчисления в соц-страх",&kodsocstr,"zarnast.alx");
iceb_t_poldan("Код фонда социального страхования",&kodsocstr,"zarnast.alx");
iceb_t_poldan("Код фонда социальной защиты инвалидов",&kodsocstr,"zarnast.alx");
iceb_t_poldan("Код фонда занятости",&kodbezf,"zarnast.alx");
iceb_t_poldan("Код отчисления на безработицу",&kodbzr,"zarnast.alx");
iceb_t_poldan("Процент отчисления в фонд безработицы",&procbez,"zarnast.alx");
iceb_t_poldan("Код фонда страхования от несчастного случая",&kodfsons,"zarnast.alx");

sprintf(strsql,"select pm,ms from Zarmzpm where data <= '%04d-%02d-%d' order by data desc limit 1",gd,md,dd);
if(readkey(strsql,&row,&curr) > 0)
 {

  if(gd <= 2006)
   ogrzp[0]=150;
  else
    ogrzp[0]=atof(row[0]);

  ogrzp[1]=atof(row[1]);

 }


clear();
GDITE();

iceb_t_poldan("Код пенсионного отчисления",&kodpen,"zarnast.alx");
iceb_t_poldan("Код пенсионного фонда",&kodpenf,"zarnast.alx");



float prpf=0.;

//Читаем процент отчисления в пенсионный фонд 
sprintf(strsql,"select proc from Zarsoc where kod=%d",kodpenf);
if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
  prpf=atof(row1[0]);
//Читаем процент отчисления в соц-страх
float prsc=0.;
sprintf(strsql,"select proc from Zarsoc where kod=%d",kodsoc);
if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
  prsc=atof(row1[0]);
//Читаем процент отчисления в фонд безработицы
float prbez=0.;
sprintf(strsql,"select proc from Zarsoc where kod=%d",kodbezf);
if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
  prbez=atof(row1[0]);

//Читаем процент отчисления в фонд страхования от несчасного случая
float prsons=0.;
sprintf(strsql,"select proc from Zarsoc where kod=%d",kodfsons);
if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
  prsons=atof(row1[0]);

float tarif2=0.;
//sprintf(strsql,"%s",iceb_t_imafnsi("zarnast.alx"));
if(iceb_t_poldan("Тариф отчисления в пенсионный фонд инвалидами",&bros,"zarnast.alx") == 0)
  tarif2=bros.ravno_atof();  

class masiv_din_int mm;
class masiv_din_int gg;
if(metod == 0)
 {
  kolpd=pole2(kp.ravno(),',');
  kd=pole2(mesc.ravno(),',');
  if(kd == 0)
   {
    if(rsdat1(&m,&g,mesc.ravno()) != 0)
     {
      iceb_t_soob(gettext("Не верно введена дата !"));
      goto naz;       
     }
    mm.plus(m,-1);
    gg.plus(g,-1);
    kd=1;
   }
  else
   {
    for(i=0; i< kd;i++)
     {
      polen(mesc.ravno(),&bros,i+1,',');
      if(rsdat1(&m,&g,bros.ravno()) != 0)
       {
        iceb_t_soob(gettext("Не верно введена дата !"));
        goto naz;       
       }
      mm.plus(m,-1);
      gg.plus(g,-1);
     }  
   }
 }
else
 {
  if(rsdat1(&m,&g,mesc.ravno()) != 0)
   {
    iceb_t_soob(gettext("Не верно введена дата !"));
    goto naz;       
   }
  mm.plus(m,-1);
  gg.plus(g,-1);
  d=1; 
  kd=1;
  if(sravmydat(d,m,g,dd,md,gd) > 0)
   {
    iceb_t_soob(gettext("Дата начисления больше даты долга !"));
    goto naz;       
   }
  
  for(; ;)
   {
    if(m == md && g == gd)
     break;
    dpm(&d,&m,&g,3); //Увеличить на месяц    
    mm.plus(m,-1);
    gg.plus(g,-1);
    kd++;
   }
 }


sprintf(imaf,"dolg%02d%d.lst",md,gd);
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

sprintf(imaft,"dolgt%d.lst",getpid());
if((ff2 = fopen(imaft,"w")) == NULL)
 {
  error_op_nfil(imaft,errno,"");
  return(1);
 }

  if((ff1 = fopen(imafprot,"w")) == NULL)
   {
    error_op_nfil(imafprot,errno,"");
    return(1);
   }

fprintf(ff,"%s\n\
%s %s %s %d.%d.%d%s\n\
%s %d.%d.%d%s  %s: %02d:%02d\n",
iceb_t_get_pnk("00",0),
gettext("Долг по зарплате за"),
mesc.ravno(),
gettext("на"),
dd,md,gd,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ff2,"%s\n\
%s %s %s %d.%d.%d%s\n\
%s %d.%d.%d%s  %s: %02d:%02d\n",
iceb_t_get_pnk("00",0),
gettext("Долг по зарплате за"),
mesc.ravno(),
gettext("на"),
dd,md,gd,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

if(kp.ravno()[0] == '\0')
 {
  fprintf(ff,gettext("По всем полразделениям.\n"));
  fprintf(ff2,gettext("По всем полразделениям.\n"));
 }
else
 {
  fprintf(ff,"%s %s\n",gettext("Подразделение:"),kp.ravno());
  fprintf(ff2,"%s %s\n",gettext("Подразделение:"),kp.ravno());
 }
if(mras == 1)
 {
  fprintf(ff,gettext("Распечатка только тех, кому должны.\n"));
  fprintf(ff2,gettext("Распечатка только тех, кому должны.\n"));
 }
if(tabn.ravno()[0] != '\0')
 {
  fprintf(ff,"%s - %s\n",gettext("Табельный номер"),tabn.ravno());
  fprintf(ff2,"%s - %s\n",gettext("Табельный номер"),tabn.ravno());
 }

for(i=0; i < RAZMER; i++)
 itog1[i]=itog2[i]=itog1o[i]=itog2o[i]=0.;

class SQLCURSOR cur1;

sprintf(strsql,"select str from Alx where fil='zarnast.alx' and str like 'Процент отчисления в пенсионный фонд%%'");
if(readkey(strsql,&row,&cur1) == 1)
 {
     polen(row[0],strsql,sizeof(strsql),2,'|');
     procpen[0]=(float)ATOFM(strsql);
     if(polen(row[0],strsql,sizeof(strsql),3,'|') == 0)
       procpen[1]=(float)ATOFM(strsql);
     else 
       procpen[1]=procpen[0];
     
 
 }

sprintf(strsql,"select str from Alx where fil='zarnast.alx' and str like 'Процент отчисления в соц-страх%%'");
if(readkey(strsql,&row,&cur1) == 1)
 {
  polen(row[0],strsql,sizeof(strsql),2,'|');
  procsoc=(float)ATOFM(strsql);
 }
 
osn=osu=0.;
for(ii=0; ii < kd; ii++)
 {
  printw("\n%s %d.%d%s\n",
  gettext("Дата начисления зарплаты"),mm.ravno(ii),gg.ravno(ii),
  gettext("г."));

  fprintf(ff,"\n%s %d.%d%s\n",
  gettext("Дата начисления зарплаты"),mm.ravno(ii),gg.ravno(ii),
  gettext("г."));
  fprintf(ff2,"\n%s %d.%d%s\n",
  gettext("Дата начисления зарплаты"),mm.ravno(ii),gg.ravno(ii),
  gettext("г."));

  sprintf(strsql,"select tabn,podr from Zarn where god=%d and mes=%d \
order by podr,tabn asc",
  gg.ravno(ii),mm.ravno(ii));
  class SQLCURSOR cur;
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }
  if(kolstr == 0)
   {
    printw("%s %d.%d%s!\n",
    gettext("Нет данних на"),mm.ravno(ii),gg.ravno(ii),
    gettext("г."));
    OSTANOV();
    continue;
   }
  
  //читаем настройки
//  zar_read_tn(1,mm.ravno(ii),gg.ravno(ii),NULL);
  
  sumno=sumuo=sumnop=sumuop=0.;
  pod=0;
  kolstr1=0;
  while(cur.read_cursor(&row) != 0)
   {
//    printw("%s %s\n",row[0],row[1]);  
    strzag(LINES-1,0,kolstr,++kolstr1);

    if(proverka(kp.ravno(),row[1],0,0) != 0)
      continue;

    if(proverka(tabn.ravno(),row[0],0,0) != 0)
      continue;
    tbn=atol(row[0]);
    kpd=atoi(row[1]);


    if(pod != kpd)
     {
      if(pod != 0)
       {
        printw("%*s: %10.2f %10.2f %10.2f\n",
        iceb_tu_kolbait(35,gettext("Итого по подразделению")),gettext("Итого по подразделению"),sumnop,sumuop,sumnop+sumuop);
        fprintf(ff,"%*s: %10.2f %10.2f %10.2f\n",
        iceb_tu_kolbait(35,gettext("Итого по подразделению")),gettext("Итого по подразделению"),sumnop,sumuop,sumnop+sumuop);

	rasitog(itog1,itog2,ff2,0);
        for(i=0; i < RAZMER; i++)
         itog1[i]=itog2[i]=0.;

        sumnop=sumuop=0.;
       }    
      sprintf(strsql,"select naik from Podr where kod=%d",kpd);
      if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
       {
        naipod.new_plus("");
        beep();
        printw("%s %d !\n",gettext("Не найден код подразделения"),kpd);
        OSTANOV();
       }
      else
        naipod.new_plus(row1[0]);     
      printw("%s %d %s\n",gettext("Подразделение"),kpd,naipod.ravno());
      refresh();
      fprintf(ff,"\n%s %d %s\n",gettext("Подразделение"),kpd,naipod.ravno());
      fprintf(ff2,"\n%s %d %s\n",gettext("Подразделение"),kpd,naipod.ravno());

      fprintf(ff,"\
----------------------------------------------------------------------\n");
      fprintf(ff,gettext(" Т/н | Фамилия  , Імя , Отчество    |Начислено | Удержано |  Долг    |\n"));
      fprintf(ff,"\
----------------------------------------------------------------------\n");

      for(i=0; i < RAZMER; i++)
       itog1[i]=itog2[i]=0.;

      fprintf(ff2,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
      fprintf(ff2,gettext("\
 Т/н| Фамилия  |Начисле- |Об.удер-|К выдач. |Подоход-|                  П е н с и о н н ы е                |       Фонд социального страхования         |          Фонд зянятости           |Фонд инвалидов   |Страх.от несч.сл.| И т о г о\n"));
      fprintf(ff2,gettext("\
    |          |   ния   | жания  |  Долг   |ный нал.|C суммы | %4.4g%%  | %4.4g%%  | %4.4g%%  | %4.4g%%  | итого  |C суммы | %4.4g%%  | %4.4g%%  | %4.4g%%  | итого  |C суммы | %4.4g%%  | %4.4g%%  | итого  |C суммы |  1%%    |C суммы | %4.4g%%  |\n"),
      prpf,tarif2,procpen[0],procpen[1],prsc,procsoc,procsoc,prbez,procbez,prsons);
      fprintf(ff2,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
 1  |    2     |    3    |   4    |    5    |    6   |   7    |   8    |   9    |   10   |   11   |   12   |   13   |   14   |   15   |   16   |   17   |   18   |   19   |   20   |   21   |   22   |   23   |   24   |   25   |6+12+17+21+23+25|\n\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
      sumnop=sumuop=0.;
      pod=kpd;
     }

    
    sfondsc1=0.;
    sfondscp1=0.;
    sfondscp2=0.;
    sfondsc2=0.;
    spodoh=0.;
    penot4=penot4p=0.;
    fondzn05=fondzn05p=penot=sumaou=0.;
    sumasbpp=penot32p=penot1p=penot2p=fondscp=fondznp=fondscip=spodohp=0.;
    sfondsons=sfondsonsp=fondsons=fondsonsp=0.;
    sumasbp=0.;
    sfondsc=sfondscp=fondsc=fondscp=0.;
    sfondzn=fondzn=fondznp=0.;
    sfondsci=fondsci=fondscip=0.;
    penot32=penot1=penot2=fondsc=fondzn=fondsci=0.;
    sfondznp=sfondscip=0.;
    penot1m=penot2m=0;
    sumn=sumu=0.;
    fondzn05m=0.;
    socstrah=0.;
    sumasbp1=0.;
    sumasoc=0.;        


    sprintf(strsql,"select prn,knah,suma from Zarp where datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' and tabn=%d and \
 suma <> 0. order by prn,knah asc",gg.ravno(ii),mm.ravno(ii),gg.ravno(ii),mm.ravno(ii),tbn);
    if(cur1.make_cursor(&bd,strsql) < 0)
     {
      msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
      continue;
     }
    double suma_nah=0;
    int kodnu=0;
    while(cur1.read_cursor(&row1) != 0)
     {
      suma_nah=atof(row1[2]);
      kodnu=atoi(row1[1]);
      if(atoi(row1[0]) == 1)
       {
        sumn+=suma_nah;
       }
      else
       {
        //Узнаем сумму обязательных удержаний
        if(obud != NULL)
         {
          if(provkod(obud,kodnu) >= 0)
             sumaou+=suma_nah;      
         }
        if(kodnu == kodpn || kodnu == kodperpn)
          spodoh+=suma_nah;
        if(kodnu == kodpen)
          penot+=suma_nah;         
        if(kodnu == kodbzr)
          fondzn05m+=suma_nah;         
        if(kodnu == kodsocstr)
          socstrah+=suma_nah;       
        
       }
     }
    sumu+=prbper1(mm.ravno(ii),gg.ravno(ii),dd,md,gd,tbn,NULL);

    koef=0.;
    if(sumn+sumu != 0.)
      koef=(sumn+sumaou)/(sumn+sumu);

    fprintf(ff1,"Коэффициент: %f=(%.2f %.2f)/(%.2f %.2f)\n",
    koef,sumn,sumaou,sumn,sumu);

    sprintf(strsql,"select fio from Kartb where tabn=%d",tbn);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      fio.new_plus("");
      beep();
      printw("%s %d !\n",gettext("Не найден табельный номер"),tbn);
      OSTANOV();
     }
    else
     fio.new_plus(row1[0]);
      
    rasnuptn1(mm.ravno(ii),gg.ravno(ii),dd,md,gd,tbn,fio.ravno(),ff1);


    if(mras == 1 && fabs(sumn+sumu) < 0.009 )
      continue;

    /*Смотрим отчисления в соц. фонды*/
    sprintf(strsql,"select kodz,sumas,sumap,proc from Zarsocz where datz='%d-%d-1' \
and tabn=%d",gg.ravno(ii),mm.ravno(ii),tbn);
    if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
     {
      msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
      continue;
     }
    while(cur1.read_cursor(&row1) != 0)
     {
      kodz=atoi(row1[0]);
      sum=atof(row1[1]);
      sump=atof(row1[2]);
      if(kodz == kodpenf)
       {
        sumasbp+=sum;
        if(koef != 0.)
          sumasbpp+=sum/koef;
        if(atof(row1[3]) > tarif2)
         {
          penot32+=sump;
          if(koef != 0.)
            penot32p+=sump/koef;
         }
        else
         {
          penot4+=sump;
          if(koef != 0.)
            penot4p+=sump/koef;
         }
       }
      if(kodz == kodbezf)
       {
        sfondzn+=sum;
        if(koef != 0.)
          sfondznp+=sum/koef;
        fondzn+=sump;
        if(koef != 0.)
          fondznp+=sump/koef;
       }
      if(kodz == kodsoc)
       {
        sfondsc+=sum;
        if(koef != 0.)
          sfondscp+=sum/koef;
        fondsc+=sump;
        if(koef != 0.)
          fondscp+=sump/koef;
       }
      if(kodz == kodsoci)
       {
        sfondsci+=sum;
        if(koef != 0.)
          sfondscip+=sum/koef;
        fondsci+=sump;
        if(koef != 0.)
          fondscip+=sump/koef;
       }
      if(kodz == kodfsons)
       {
        sfondsons+=sum;
        if(koef != 0.)
          sfondsonsp+=sum/koef;
        fondsons+=sump;
        if(koef != 0.)
          fondsonsp+=sump/koef;
       }

     }

    if(sumasbp1 <= ogrzp[0])
      penot1m=penot;
    else
      penot2m=penot;

    penot1+=penot1m;
    penot2+=penot2m;

    double soc1m=0.;
    double soc2m=0.;
    if(sumasoc <= ogrzp[0])
      soc1m=socstrah;
    else
      soc2m=socstrah;

    sfondsc1+=soc1m;
    sfondsc2+=soc2m;

    if(koef != 0.)
     {
      penot1p+=penot1m/koef;
      penot2p+=penot2m/koef;
      sfondscp1+=soc1m/koef;
      sfondscp2+=soc2m/koef;
     }
    fondzn05+=fondzn05m;
    if(koef != 0.)
     {
      fondzn05p+=fondzn05m/koef;
      spodohp+=spodoh/koef;
     }

    printw("%-5d %-*.*s %10.2f %10.2f %10.2f\n",
    tbn,
    iceb_tu_kolbait(30,fio.ravno()),iceb_tu_kolbait(30,fio.ravno()),fio.ravno(),
    sumn,sumu,sumn+sumu);
    
    GDITE();
    fprintf(ff,"%-5d %-*.*s %10.2f %10.2f %10.2f\n",
    tbn,
    iceb_tu_kolbait(30,fio.ravno()),iceb_tu_kolbait(30,fio.ravno()),fio.ravno(),
    sumn,sumu,sumn+sumu);

    itog1[0]+=sumn;
    itog1[1]+=sumaou;
    itog1o[0]+=sumn;
    itog1o[1]+=sumaou;

    itog1[2]+=sumn+sumaou;
    itog2[2]+=sumn+sumu;
    itog1o[2]+=sumn+sumaou;
    itog2o[2]+=sumn+sumu;

    itog1[3]+=spodoh;
    itog2[3]+=spodohp;
    itog1o[3]+=spodoh;
    itog2o[3]+=spodohp;

    itog1[4]+=sumasbp;
    itog2[4]+=sumasbpp;
    itog1o[4]+=sumasbp;
    itog2o[4]+=sumasbpp;

    itog1[5]+=penot32;
    itog2[5]+=penot32p;
    itog1o[5]+=penot32;
    itog2o[5]+=penot32p;

    itog1[6]+=penot4;
    itog2[6]+=penot4p;
    itog1o[6]+=penot4;
    itog2o[6]+=penot4p;

    itog1[7]+=penot1;
    itog2[7]+=penot1p;
    itog1o[7]+=penot1;
    itog2o[7]+=penot1p;

    itog1[8]+=penot2;
    itog2[8]+=penot2p;
    itog1o[8]+=penot2;
    itog2o[8]+=penot2p;

    itog1[9]+=penot32+penot4+(penot1+penot2)*-1;
    itog2[9]+=penot32p+penot4p+(penot1p+penot2p)*-1;
    itog1o[9]+=penot32+penot4+(penot1+penot2)*-1;
    itog2o[9]+=penot32p+penot4p+(penot1p+penot2p)*-1;

    itog1[10]+=sfondsc;
    itog2[10]+=sfondscp;
    itog1o[10]+=sfondsc;
    itog2o[10]+=sfondscp;

    itog1[11]+=fondsc;
    itog2[11]+=fondscp;
    itog1o[11]+=fondsc;
    itog2o[11]+=fondscp;

    itog1[12]+=sfondsc1;
    itog2[12]+=sfondscp1;
    itog1o[12]+=sfondsc1;
    itog2o[12]+=sfondscp1;

    itog1[13]+=sfondsc2;
    itog2[13]+=sfondscp2;
    itog1o[13]+=sfondsc2;
    itog2o[13]+=sfondscp2;

    itog1[14]+=fondsc+(sfondsc1+sfondsc2)*-1;
    itog2[14]+=fondscp+(sfondscp1+sfondscp2)*-1;
    itog1o[14]+=fondsc+(sfondsc1+sfondsc2)*-1;
    itog2o[14]+=fondscp+(sfondscp1+sfondscp2)*-1;

    itog1[15]+=sfondzn;
    itog2[15]+=sfondznp;
    itog1o[15]+=sfondzn;
    itog2o[15]+=sfondznp;

    itog1[16]+=fondzn;
    itog2[16]+=fondznp;
    itog1o[16]+=fondzn;
    itog2o[16]+=fondznp;

    itog1[17]+=fondzn05;
    itog2[17]+=fondzn05p;
    itog1o[17]+=fondzn05;
    itog2o[17]+=fondzn05p;

    itog1[18]+=fondzn+fondzn05*-1;
    itog2[18]+=fondznp+fondzn05p*-1;
    itog1o[18]+=fondzn+fondzn05*-1;
    itog2o[18]+=fondznp+fondzn05p*-1;

    itog1[19]+=sfondsci;
    itog2[19]+=sfondscip;
    itog1o[19]+=sfondsci;
    itog2o[19]+=sfondscip;

    itog1[20]+=fondsci;
    itog2[20]+=fondscip;
    itog1o[20]+=fondsci;
    itog2o[20]+=fondscip;

    itog1[21]+=sfondsons;
    itog2[21]+=sfondsonsp;
    itog1o[21]+=sfondsons;
    itog2o[21]+=sfondsonsp;

    itog1[22]+=fondsons;
    itog2[22]+=fondsonsp;
    itog1o[22]+=fondsons;
    itog2o[22]+=fondsonsp;
    
    fprintf(ff2,"%4d|%-*.*s|%9.2f|%8.2f|%9.2f|\
%8.2f|\
%8.2f|%8.2f|%8.2f|%8.2f|%8.2f|%8.2f|\
%8.2f|%8.2f|%8.2f|%8.2f|%8.2f|\
%8.2f|%8.2f|%8.2f|%8.2f|\
%8.2f|%8.2f|%8.2f|%8.2f|%13.2f|\n",
    tbn,
    iceb_tu_kolbait(10,fio.ravno()),iceb_tu_kolbait(10,fio.ravno()),fio.ravno(),
    sumn,sumaou*(-1),sumn+sumaou,
    spodoh*(-1),
    sumasbp,penot32,penot4,penot1*-1,penot2*(-1),
    penot32+penot4+penot1*-1+penot2*(-1),
    sfondsc,fondsc,sfondsc1*-1,sfondsc2*-1,
    fondsc+sfondsc1*-1+sfondsc2*-1,
    sfondzn,fondzn,fondzn05*(-1),
    fondzn+fondzn05*(-1),
    sfondsci,fondsci,
    sfondsons,fondsons,
    spodoh*(-1)+penot32+penot4+penot1*-1+penot2*(-1)+\
    fondsc+\
    fondzn+fondzn05*(-1)+\
    fondsci+fondsons);
  
      
    fprintf(ff2,"%34s|%9.2f|%8.2f|%8.2f|%8.2f|%8.2f|%8.2f|\
%8.2f|%8.2f|%8.2f|\
%8.2f|%8.2f|%8.2f|%8.2f|%8.2f|\
%8.2f|%8.2f|%8.2f|\
%8.2f|%8.2f|%8.2f|%8.2f|%13.2f|\n",
    " ",sumn+sumu,
    spodohp*(-1),
    sumasbpp,penot32p,penot4p,penot1p*(-1),penot2p*(-1),
    penot32p+penot4p+penot1p*(-1)+penot2p*(-1),
    sfondscp,fondscp,sfondscp1*-1,sfondscp2*-1,
    fondscp+sfondscp1*-1+sfondscp2*-1,
    sfondznp,fondznp,fondzn05p*-1,
    fondznp+fondzn05p*-1,
    sfondscip,fondscip,
    sfondsonsp,fondsonsp,
    spodohp*(-1)+penot32p+penot4p+penot1p*(-1)+penot2p*(-1)+\
    fondscp+\
    fondznp+fondzn05p*-1+\
    fondscip+fondsonsp); 

    sumno+=sumn;
    sumuo+=sumu;
    sumnop+=sumn;
    sumuop+=sumu;
   }

  printw("%*s: %10.2f %10.2f %10.2f\n",
  iceb_tu_kolbait(35,gettext("Итого по подразделению")),gettext("Итого по подразделению"),sumnop,sumuop,sumnop+sumuop);
  printw("%*s: %10.2f %10.2f %10.2f\n",
  iceb_tu_kolbait(35,gettext("Итого")),gettext("Итого"),sumno,sumuo,sumno+sumuo);

  fprintf(ff,"%*s: %10.2f %10.2f %10.2f\n",
  iceb_tu_kolbait(35,gettext("Итого по подразделению")),gettext("Итого по подразделению"),sumnop,sumuop,sumnop+sumuop);

  fprintf(ff,"%*s: %10.2f %10.2f %10.2f\n",
  iceb_tu_kolbait(35,gettext("Итого")),gettext("Итого"),sumno,sumuo,sumno+sumuo);

  osn+=sumno;
  osu+=sumuo;
  sumnop=sumuop=0.;
  sumno=sumuo=0.;

  rasitog(itog1,itog2,ff2,0);

 }
GDITE();
if(kd > 1 || kolpd == 0)
 {
  printw("\n%*s: %10.2f %10.2f %10.2f\n",
  iceb_tu_kolbait(35,gettext("Общий итог")),gettext("Общий итог"),osn,osu,osn+osu);
  fprintf(ff,"\n%*s: %10.2f %10.2f %10.2f\n",
  iceb_tu_kolbait(35,gettext("Общий итог")),gettext("Общий итог"),osn,osu,osn+osu);

  rasitog(itog1o,itog2o,ff2,1);
 }
podpis(ff);
podpis(ff2);

fclose(ff);
fclose(ff2);
fclose(ff1);

//Распечатываем свод

sprintf(imafsvod,"dolgs%02d%d_%d.lst",md,gd,getpid());
if((ff = fopen(imafsvod,"w")) == NULL)
 {
  error_op_nfil(imafsvod,errno,"");
  return(1);
 }

fprintf(ff,"%s\n\
%s %s %s %d.%d.%d%s\n\
%s %d.%d.%d%s  %s: %d:%d\n",
iceb_t_get_pnk("00",0),
"Отчисления в гос. фонды",
mesc.ravno(),
gettext("на"),
dd,md,gd,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ff,"\n%30s.  %s\n",
gettext("Начислено"),
gettext("Долг."));


fprintf(ff,"\n\
%*s:%8.2f\n\
%*s:%8.2f\n\
%*s:%8.2f %8.2f\n\
%*s:%8.2f %8.2f\n",
iceb_tu_kolbait(22,gettext("Начислено")),gettext("Начислено"),
itog1o[0],
iceb_tu_kolbait(22,gettext("Обязательные удержания")),gettext("Обязательные удержания"),
itog1o[1]*-1,
iceb_tu_kolbait(22,gettext("К выдаче")),gettext("К выдаче"),
itog1o[2],itog2o[2],
iceb_tu_kolbait(22,gettext("Подоходный налог")),gettext("Подоходный налог"),
itog1o[3]*-1,itog2o[3]*-1);

//Читаем пенсионный фонд 
sprintf(strsql,"select naik from Zarsoc where kod=%d",kodpenf);
if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
  bros.new_plus(row1[0]);
else
  bros.new_plus("");
fprintf(ff,"\n\
%s\n\
%*s:%8.2f %8.2f\n\
%21g%%:%8.2f %8.2f\n\
%21g%%:%8.2f %8.2f\n\
%21g%%:%8.2f %8.2f\n\
%21g%%:%8.2f %8.2f\n\
%*s:%8.2f %8.2f\n",
bros.ravno(),
iceb_tu_kolbait(22,gettext("C суммы")),gettext("C суммы"),
itog1o[4],itog2o[4],
prpf,
itog1o[5],itog2o[5],
tarif2,
itog1o[6],itog2o[6],
procpen[0],
itog1o[7]*-1,itog2o[7]*-1,
procpen[1],
itog1o[8]*-1,itog2o[8]*-1,
iceb_tu_kolbait(22,gettext("Итого")),gettext("Итого"),
itog1o[9],itog2o[9]);

//Читаем процент отчисления в соц-страх
sprintf(strsql,"select naik from Zarsoc where kod=%d",kodsoc);
if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
  bros.new_plus(row1[0]);
else
  bros.new_plus("");

fprintf(ff,"\n\
%s\n\
%*s:%8.2f %8.2f\n\
%21g%%:%8.2f %8.2f\n\
%21g%%:%8.2f %8.2f\n\
%21g%%:%8.2f %8.2f\n\
%*s:%8.2f %8.2f\n",
bros.ravno(),
iceb_tu_kolbait(22,gettext("C суммы")),gettext("C суммы"),
itog1o[10],itog2o[10],
prsc,
itog1o[11],itog2o[11],
procsoc,
itog1o[12]*-1,itog2o[12]*-1,
procsoc,
itog1o[13]*-1,itog2o[13]*-1,
iceb_tu_kolbait(22,gettext("Итого")),gettext("Итого"),
itog1o[14],itog2o[14]);

//Читаем фонд безработицы
sprintf(strsql,"select naik from Zarsoc where kod=%d",kodbezf);
if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
  bros.new_plus(row1[0]);
else
  bros.new_plus("");

fprintf(ff,"\n\
%s\n\
%*s:%8.2f %8.2f\n\
%21g%%:%8.2f %8.2f\n\
%21g%%:%8.2f %8.2f\n\
%*s:%8.2f %8.2f\n",
bros.ravno(),
iceb_tu_kolbait(22,gettext("C суммы")),gettext("C суммы"),
itog1o[15],itog2o[15],
prbez,
itog1o[16],itog2o[16],
procbez,
itog1o[17]*-1,itog2o[17]*-1,
iceb_tu_kolbait(22,gettext("Итого")),gettext("Итого"),
itog1o[18],itog2o[18]);

fprintf(ff,"\n\
%s\n\
%*s:%8.2f %8.2f\n\
%21g%%:%8.2f %8.2f\n",
"Фонд инвалидов",
iceb_tu_kolbait(22,gettext("C суммы")),gettext("C суммы"),
itog1o[19],itog2o[19],
1.,
itog1o[20],itog2o[20]);


//Читаем фонд страхования от несчасного случая
sprintf(strsql,"select naik from Zarsoc where kod=%d",kodfsons);
if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
  bros.new_plus(row1[0]);
else
  bros.new_plus("");
fprintf(ff,"\n\
%s\n\
%*s:%8.2f %8.2f\n\
%21g%%:%8.2f %8.2f\n",
bros.ravno(),
iceb_tu_kolbait(22,gettext("C суммы")),gettext("C суммы"),
itog1o[21],itog2o[21],
prsons,
itog1o[22],itog2o[22]);

fprintf(ff,"\n\
%s\n\
%22s:%8.2f %8.2f\n",
gettext("Итого"),
"6+12+17+21+23+25",
itog1o[3]*-1+itog1o[9]+itog1o[14]+itog1o[18]+itog1o[20]+itog1o[22],
itog2o[3]*-1+itog2o[9]+itog2o[14]+itog2o[18]+itog2o[20]+itog2o[22]);

podpis(ff);

fclose(ff);

time(&tmmk);
sumn=sumu=0.;
sumn=(tmmk-tmm)/60.;
sumn=modf(sumn,&sumu);

printw("\n%s %.f%s %.f%s\n",
gettext("Время расчёта"),sumu,
gettext("мин."),sumn*60.,
gettext("сек."));


OSTANOV();
sprintf(strsql,"%s %s %s",imaf,imaft,imafsvod);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Распечатка долгов"));

oth->spis_imaf.plus(imaft);
oth->spis_naim.plus(gettext("Расчёт долгов по соц. фондам"));

oth->spis_imaf.plus(imafsvod);
oth->spis_naim.plus(gettext("Общий итог"));

oth->spis_imaf.plus(imafprot);
oth->spis_naim.plus(gettext("Протокол розрахунку"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),0);
 
return(0);
}

/*******************/
/*Распечатка итогов*/
/*******************/

void	rasitog(double *itog1,double *itog2,FILE *ff2,int metka)
{
class iceb_tu_str strok("");

if(metka == 0)
 strok.new_plus(gettext("Итого"));
if(metka == 1)
 strok.new_plus(gettext("Общий итог"));

fprintf(ff2,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff2,"%*.*s|%9.2f|%8.2f|%9.2f|\
%8.2f|\
%8.2f|%8.2f|%8.2f|%8.2f|%8.2f|%8.2f|\
%8.2f|%8.2f|%8.2f|%8.2f|%8.2f|\
%8.2f|%8.2f|%8.2f|%8.2f|\
%8.2f|%8.2f|%8.2f|%8.2f|%13.2f|\n",
iceb_tu_kolbait(15,strok.ravno()),iceb_tu_kolbait(15,strok.ravno()),strok.ravno(),
itog1[0],itog1[1]*(-1),itog1[2],itog1[3]*(-1),itog1[4],
itog1[5],itog1[6],itog1[7]*-1,itog1[8]*-1,
itog1[9],itog1[10],
itog1[11],itog1[12]*-1,itog1[13]*-1,
itog1[14],itog1[15],itog1[16],itog1[17]*-1,itog1[18],itog1[19],itog1[20],
itog1[21],itog1[22],
itog1[3]*-1+itog1[9]+itog1[14]+itog1[18]+itog1[20]+itog1[22]);

fprintf(ff2,"%*.*s|%9s|%8s|%9.2f|\
%8.2f|\
%8.2f|%8.2f|%8.2f|%8.2f|%8.2f|%8.2f|\
%8.2f|%8.2f|%8.2f|%8.2f|%8.2f|\
%8.2f|%8.2f|%8.2f|%8.2f|\
%8.2f|%8.2f|%8.2f|%8.2f|%13.2f|\n",
iceb_tu_kolbait(15,strok.ravno()),iceb_tu_kolbait(15,strok.ravno()),strok.ravno(),
" "," ",
itog2[2],itog2[3]*(-1),itog2[4],
itog2[5],itog2[6],itog2[7]*-1,itog2[8]*-1,
itog2[9],itog2[10],
itog2[11],itog2[12]*-1,itog2[13]*-1,
itog2[14],itog2[15],itog2[16],itog2[17]*-1,itog2[18],itog2[19],itog2[20],
itog2[21],itog2[22],
itog2[3]*-1+itog2[9]+itog2[14]+itog2[18]+itog2[20]+itog2[22]);
}
