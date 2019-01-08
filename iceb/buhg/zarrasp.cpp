/* $Id: zarrasp.c,v 5.48 2013/09/26 09:43:45 sasa Exp $ */
/*28.03.2018	18.05.2000	Белых А.И.	zarrasp.c
Расчёт перечислений которые нужно сделать по результатам
выплаты заработной платы
*/
#include        <errno.h>
#include        <math.h>
#include        "buhg.h"

int zarzasp1(short,short,short,short,short,short,const char[],short,short,const char*,FILE*,FILE*,FILE*);
int zarzasp2(short dn,short mn,short gn,short dk,short mk,short gk,const char *tabnom,short mv,short gv,const char *podr,class spis_oth *oth);

extern short	*obud; /*Обязательные удержания*/

extern class iceb_tu_str kodpn_all;


int zarrasp(class spis_oth *oth)
{
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
static class iceb_tu_str tabnom("");
static class iceb_tu_str datv("");
static class iceb_tu_str podr("");
int		kom=0,kom1=0;
short		dn,mn,gn; /*Дата начала поиска*/
short		dk,mk,gk; /*Дата конца поиска*/
char		imaf[64],imafp[64],imafsv[64];
FILE		*ff,*ffp,*ffsv;
time_t          tmm;
int		i;
short		prot;
short		mv,gv;
int		tabb=0;
class iceb_tu_str fiov("");
class VVOD MENU(3);
class VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт отчислений на выданную зарплату"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала...(д.м.г)......."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца....(д.м.г)......."));
VV.VVOD_SPISOK_add_MD(gettext("В счёт какого месяца.(м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер.....(,,)...."));
VV.VVOD_SPISOK_add_MD(gettext("Подразделение.......(,,)...."));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(datv.ravno(),8);
VV.VVOD_SPISOK_add_data(tabnom.ravno(),128);
VV.VVOD_SPISOK_add_data(podr.ravno(),128);

naz:;

clear(); 

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
datv.new_plus(VV.data_ravno(2));
tabnom.new_plus(VV.data_ravno(3));
podr.new_plus(VV.data_ravno(4));

class iceb_tu_str naim("");

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();

   iceb_t_pdoc("zarp4_7_5.txt");

   goto naz;

  case FK10:
  case ESC:
   return(1);

  case FK2:
  case PLU:
   break;

  case FK3:


    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка табельных номеров"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход."));
    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 2:
      case -1:
       goto naz;

      case 0:
       if(dirzarpodr(2,&podr,&naim) == 0)
        VV.data_z_plus(4,podr.ravno());
       break;
       
      case 1:
        tabb=0;
        fiov.new_plus("");
        if(dirtab(&tabb,&fiov,0,1) == 0)
         tabnom.z_plus(tabb);
        VV.data_plus(3,tabnom.ravno());
        break;
     }
    goto naz;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
 }

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
 goto naz;

mv=gv=0;
if(datv.ravno()[0] != '\0')
 if(rsdat1(&mv,&gv,datv.ravno()) != 0)
  {
   iceb_t_soob(gettext("Не верно введена дата выплаты !"));
   goto naz;
  }  


if(sravmydat(1,mn,gn,ICEB_DNDESV,ICEB_MNDESV,ICEB_GNDESV) >= 0)
 {
  return(zarzasp2(dn,mn,gn,dk,mk,gk,tabnom.ravno(),mv,gv,podr.ravno(),oth));
 }



VVOD DANET(1);

DANET.VVOD_SPISOK_add_MD(gettext("Протокол хода расчёта сделать ?"));
memset(imafp,'\0',sizeof(imafp));
ffp=NULL;
if((prot=danet(&DANET,2,stdscr)) ==1)
 {
  sprintf(imafp,"othp%d.lst",getpid());
  if((ffp = fopen(imafp,"w")) == NULL)
   {
    error_op_nfil(imafp,errno,"");
    return(1);
   }
 } 

time(&tmm);

sprintf(imaf,"oth%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return(1);
   }

sprintf(imafsv,"oths%d.lst",getpid());

if((ffsv = fopen(imafsv,"w")) == NULL)
   {
    error_op_nfil(imafsv,errno,"");
    return(1);
   }

fprintf(ff,"\x1B\x33%c\n",30); /*Уменьшаем межстрочный интервал*/

i=zarzasp1(dn,mn,gn,dk,mk,gk,tabnom.ravno(),mv,gv,podr.ravno(),ff,ffp,ffsv);

podpis(ff);
podpis(ffsv);
fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/

fclose(ff);
fclose(ffsv);
if(prot == 1)
 fclose(ffp);

if(i == 0)
 {
  unlink(imaf);
  unlink(imafsv);
  unlink(imafp);
  
  return(1);
 }
printw_vr(tmm);

printw("\n%s %s %s\n\n",gettext("Распечатка выгружена в файл"),imaf,imafp);

OSTANOV();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт отчислений на выданную зарплату"));
oth->spis_imaf.plus(imafsv);
oth->spis_naim.plus(gettext("Общие итоги"));

if(imafp[0] != '\0')
 {
  oth->spis_imaf.plus(imafp);
  oth->spis_naim.plus(gettext("Протокол расчёта"));
 }

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++) 
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);

return(0); 
}

/********/
/*Расчёт*/
/********/
/*Если вернули 0 - не нашли выдач зарплаты
               1 - нашли
*/
               
int zarzasp1(short dn,short mn,short gn,short dk,short mk,
short gk,const char tabnom[],short mv,short gv,const char *podr,FILE *ff,FILE *ffp,
FILE *ffsv)
{
int kodfsons=0;   /*Код фонда страхования от несчасного случая*/
double procbez=0.; /*Процент отчисления на безработицу*/
int kodbzr=0;  /*Код отчисления на безработицу*/
int kodsoci=0;   /*Код социального страхования инвалидов*/
int kodsoc=0;   /*Код фонда социального страхования*/
int kodsocstr=0;  /*Код отчисления на соц-страх*/
int kodpenf=0; /*Код пенсионного фонда*/
int kodpen=0;  /*Код пенсионных отчислений*/
SQLCURSOR       curr;
 SQL_str         row,row1,row2;
char		strsql[512];
class iceb_tu_str bros("");
struct  tm      *bf;
time_t          tmm;
int		knah,i,i2,im;
int		tabn,tabnz;
int		kolstr,kolstr1,kolstr2;
#define		razmv 62 //Невероятно но это у некоторых бывает
double		masv[razmv];/*Суммы выплат*/
short		masm[razmv];/*Месяцы выплат*/
short		masg[razmv];/*Годы выплат*/
short           masmk[razmv]; /*В каком месяце выплата*/
short           masgk[razmv]; /*В каком году выплата*/
short		mz,gz;
double		suman; /*Сумма начислений за месяц*/
double		sumaou; /*Сумма обязательных удержаний  за месяц*/
double		sumani; /*Сумма начислений за период*/
double		sumaoui; /*Сумма обязательных удержаний  за период*/
double		sumavi;  /*Сумма сделаных выплат за период*/
short		prn;
class iceb_tu_str fio("");
double		sumasbp; /*Сумма с которой брались пенсионные*/
double		sumasbp1; /*Сумма с которой брались пенсионные*/
double		penot,penot1m,penot2m,penot32,penot1,penot2;
double		penot4,penot4p;
double		sumasbpp; /*Сумма с которой брались пересчитанная*/
double		penot32p,penot1p,penot2p; /*Пересчитанные*/
short		kodz;
double		sfondsc,sfondscp,fondsc,fondscp;
double		sfondsc1,sfondscp1,fondsc1,fondscp1;
double		sfondsc2,sfondscp2,fondsc2,fondscp2;
double		sfondzn,sfondznp,fondzn,fondznp,fondzn05,fondzn05p,fondzn05m;
double		sfondsci,sfondscip,fondsci,fondscip;
double		sfondsons,sfondsonsp,fondsons,fondsonsp;
double		spodoh,spodohp,spodohm;
double		koef; /*Коэффициент пересчета*/
double		sum,sump;
#define		RAZMER 23
double		itog1[RAZMER],itog2[RAZMER];
class iceb_tu_str naim("");
double		prpf=0.; //Процент пенсионного фонда
double		socstrah=0.;
double		sumasoc=0.;
class SQLCURSOR cur;
float    procpen[2]={0.,0.}; /*Процент отчисления в пенсионный фонд*/
double	ogrzp[2]={0.,0.}; /*Разграниечение зарплаты для расчёта пенсионных*/
int kodbezf=0;  /*Код фонда занятости*/

clear();
sprintf(strsql,"select pm,ms from Zarmzpm where data <= '%04d-%02d-%d' order by data desc limit 1",gn,mn,dn);
if(readkey(strsql,&row,&cur) > 0)
 {

  if(gn <= 2006)
   ogrzp[0]=150;
  else
    ogrzp[0]=atof(row[0]);

  ogrzp[1]=atof(row[1]);

 }
sprintf(strsql,"select str from Alx where fil='zarnast.alx' and str like 'Процент отчисления в пенсионный фонд%%'");
if(readkey(strsql,&row,&cur) == 1)
 {
     polen(row[0],strsql,sizeof(strsql),2,'|');
     procpen[0]=(float)ATOFM(strsql);
     if(polen(row[0],strsql,sizeof(strsql),3,'|') == 0)
       procpen[1]=(float)ATOFM(strsql);
     else 
       procpen[1]=procpen[0];
     
 
 }

float    procsoc=0; /*Процент отчисления в соц-страх*/
float    procsoc1=0; /*Процент отчисления в соц-страх*/

sprintf(strsql,"select str from Alx where fil='zarnast.alx' and str like 'Процент отчисления в соц-страх%%'");
if(readkey(strsql,&row,&cur) == 1)
 {
  polen(row[0],strsql,sizeof(strsql),2,'|');
  procsoc=(float)ATOFM(strsql);
  polen(row[0],strsql,sizeof(strsql),3,'|');
  procsoc1=(float)ATOFM(strsql);
 }
iceb_t_poldan("Код пенсионного отчисления",&kodpen,"zarnast.alx");
iceb_t_poldan("Код пенсионного фонда",&kodpenf,"zarnast.alx");
iceb_t_poldan("Код отчисления в соц-страх",&kodsocstr,"zarnast.alx");
iceb_t_poldan("Код фонда социального страхования",&kodsocstr,"zarnast.alx");
iceb_t_poldan("Код фонда социальной защиты инвалидов",&kodsocstr,"zarnast.alx");
iceb_t_poldan("Код фонда занятости",&kodbezf,"zarnast.alx");
iceb_t_poldan("Код отчисления на безработицу",&kodbzr,"zarnast.alx");
iceb_t_poldan("Процент отчисления в фонд безработицы",&procbez,"zarnast.alx");
iceb_t_poldan("Код фонда страхования от несчастного случая",&kodfsons,"zarnast.alx");

if(gn != gk)
   sprintf(strsql,"select distinct tabn from Zarn where \
(god > %d and god < %d) or \
(god = %d and mes >= %d) or \
(god = %d and mes <= %d) order by tabn asc",
gn,gk,gn,mn,gk,mk);
else
   sprintf(strsql,"select distinct tabn from Zarn where god=%d and \
mes >= %d and mes <= %d order by tabn asc",gn,mn,mk);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(0);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(0);
 }

time(&tmm);
bf=localtime(&tmm);

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
if(iceb_t_poldan("Тариф отчисления в пенсионный фонд инвалидами",&bros,"zarnast.alx") == 0)
  tarif2=bros.ravno_atof();  


iceb_t_zagolov(gettext("Расчёт отчислений на выданную зарплату"),dn,mn,gn,dk,mk,gk,ff);

if(tabnom[0] != '\0')
  fprintf(ff,"%s: %s\n",gettext("Табельные номера которые вошли в расчёт"),tabnom);
if(podr[0] != '\0')
  fprintf(ff,"%s: %s\n",gettext("Подразделение"),podr);
if(mv != 0)
  fprintf(ff,"%s: %d.%d\n",gettext("Дата в счёт которой сделаны выплаты"),mv,gv);

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext(" Т/н| Фамилия  |Начисл. |Об.удер.|К выдач. |Подоход-|                 П е н с и о н н ы е                 |       Фонд социального страхования         |          Фонд зянятости           |Фонд инвалидов   |Страх.от несч.слу| И т о г о\n"));

fprintf(ff,gettext("    |          |        |        |Выплачено|ный нал.|C суммы | %3.3g%%   | %3.3g%%   | %3.3g%%   | %3.3g%%   |  итого |C суммы |  %3.3g%%  |  %4.4g%% |  %4.4g%% |  итого |C суммы | %3.3g%%   | %4.4g%%  |  итого |C суммы |  1%%    |C суммы | %4.4g%%  |\n"),
prpf,tarif2,procpen[0],procpen[1],prsc,procsoc,procsoc1,prbez,procbez,prsons);

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
 1  |    2     |   3    |   4    |    5    |    6   |   7    |   8    |   9    |   10   |   11   |   12   |   13   |   14   |   15   |   16   |   17   |   18   |   19   |   20   |   21   |   22   |   23   |   24   |   25   |6+12+17+21+23+25|\n\
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

for(i=0; i < RAZMER; i++)
 itog1[i]=itog2[i]=0.;
 

kolstr1=tabnz=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(proverka(tabnom,row[0],0,0) != 0)
    continue;

  tabn=atol(row[0]);  

  /*Читаем фамилию*/
  sprintf(strsql,"select fio,podr from Kartb where tabn=%d",tabn);
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    beep();
    printw("%s %d !\n",gettext("Не найден табельный номер"),tabn);
    OSTANOV();
    fio.new_plus("");
   }
  else
   fio.new_plus(row1[0]);

  if(proverka(podr,row1[1],0,0) != 0)
    continue;

  printw("%4ld %s\n",tabn,fio.ravno());

  if(ffp != NULL)
     fprintf(ffp,"\n%4d %s\n-------------------------------\n",tabn,fio.ravno());
    
  /*Узнаем были ли выплаты в периоде*/ 
  sprintf(strsql,"select datz,knah,suma,mesn,godn from Zarp where \
datz >= '%04d-%d-%d' and datz <= '%04d-%d-%d' and tabn=%d and prn='2' and \
suma <> 0.",
  gn,mn,dn,gk,mk,dk,tabn);
  class SQLCURSOR cur1;
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }

  if(kolstr2 == 0)
    continue;

  for(i=0; i < razmv; i++)
    masv[i]=masm[i]=masg[i]=masmk[i]=masgk[i]=0;

  sum=0.;
  while(cur1.read_cursor(&row1) != 0)
   {
/*
    printw("%s %s %s %s %s\n",row1[0],row1[1],row1[2],row1[3],row1[4]);
    OSTANOV();
*/
    knah=atoi(row1[1]);
    sum=atof(row1[2]);    
    mz=atoi(row1[3]);    
    gz=atoi(row1[4]);    

    if(mv != 0)
     if(mv != mz || gv != gz)
       continue;
               
    if(obud != NULL)
     {
      for(i=1; i <= obud[0] ; i++)
       if(knah == obud[i])
         break;
      if(i <= obud[0])
       continue;
     }

    for(i2=0; i2 < razmv ; i2++)
     {
//      if(sum < 0.)
      if(masm[i2] == 0 || (masm[i2] == mz && masg[i2] == gz))
       {
        masm[i2]=mz;
        masg[i2]=gz;
        masv[i2]+=sum;
        break;
       }
     }
    if(i2 >= razmv)
     {
      VVOD SOOB(1);
      sprintf(strsql,"%s !!!",gettext("Внимание"));
      SOOB.VVOD_SPISOK_add_MD(strsql);
      SOOB.VVOD_SPISOK_add_MD(gettext("Не хватает массива для записи в счёт каких месяцев выплачена зарплата !"));
      soobshw(&SOOB,stdscr,-1,-1,0,1);
     }
   } 
  
   if(ffp != NULL)
    {
     fprintf(ffp,"Найденные выплаты:\n");
     for(i=0; i < razmv; i++)
      {
       if(masv[i] == 0.)
         continue;
       fprintf(ffp,"%d.%d %8.2f\n",
       masm[i],masg[i],masv[i]);
      }
    }
  /*Определяем сумму на руки и часть в этой сумме найденных выплат*/

/*
  printw("Определяем полноту выплаты.\n");
  refresh();
*/
  sfondsc2=sfondscp2=fondsc2=fondscp2=0.;
  sfondsc1=sfondscp1=fondsc1=fondscp1=0.;
  sumani=sumaoui=sumavi=0.;
  spodoh=0.;
  penot4=penot4p=0.;
  fondzn05=fondzn05p=penot=suman=sumaou=0.;
  sumasbpp=penot32p=penot1p=penot2p=fondscp=fondznp=fondscip=spodohp=0.;
  sfondsons=sfondsonsp=fondsons=fondsonsp=0.;
  sumasbp=0.;
  sfondsc=sfondscp=fondsc=fondscp=0.;
  sfondzn=fondzn=fondznp=0.;
  sfondsci=fondsci=fondscip=0.;
  penot32=penot1=penot2=fondsc=fondzn=fondsci=0.;
  sfondznp=sfondscip=0.;
  penot1m=penot2m=0;
  for(im=0; im < razmv; im++)
   {
    if(masv[im] == 0.)
     continue;
    if(ffp != NULL)
      fprintf(ffp,"Просматриваем %d.%d\n",masm[im],masg[im]);
    sprintf(strsql,"select prn,knah,suma,datz,godn,mesn from Zarp where \
datz >= '%04d-%d-%d' and datz <= '%04d-%d-%d' and tabn=%d and suma <> 0.",
    masg[im],masm[im],1,masg[im],masm[im],31,tabn);
/*
    printw("%s\n",strsql);
    refresh();
*/
    if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
     {
      msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
      continue;
     }

    if(kolstr2 == 0)
     {

      VVOD SOOB(1);
      sprintf(strsql,"%s %d",gettext("Табельный номер"),tabn);
      SOOB.VVOD_SPISOK_add_MD(strsql);
      sprintf(strsql,"%s %d.%d%s",
      gettext("Не найдено начислений за"),masm[im],masg[im],
      gettext("г."));
      SOOB.VVOD_SPISOK_add_MD(strsql);
      soobshw(&SOOB,stdscr,-1,-1,0,1);

      continue;
     } 
    
    sumavi+=masv[im];
    spodohm=fondzn05m=penot=suman=sumaou=0.;
    penot1m=penot2m=0;
    socstrah=0.;
    sumasbp1=0.;
    sumasoc=0.;        
    while(cur1.read_cursor(&row1) != 0)
     {
      prn=atoi(row1[0]);
      if(ffp != NULL)
       {
        if(prn == 1)
         sprintf(strsql,"select naik from Nash where kod=%s",row1[1]);
        if(prn == 2)
         sprintf(strsql,"select naik from Uder where kod=%s",row1[1]);

        if(sql_readkey(&bd,strsql,&row2,&curr) == 1)
          naim.new_plus(row2[0]);
        else
          naim.new_plus("");
        fprintf(ffp,"%s %2s %2s %-*s %8s %s.%s\n",
        row1[3],row1[0],row1[1],
        iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
        row1[2],row1[5],row1[4]);
       }      
      knah=atoi(row1[1]);
      sum=atof(row1[2]);
      if(prn == 1)
       {
        suman+=sum;      
        //Узнаем сумму с которой брались отчисления в пенсионный фонд
//        if(provkod(kodnvpen,knah) < 0)
//          sumasbp1+=sum;
        
        //Узнаем сумму с которой брались отчисления в соцстрах
//        if(provkod(kodsocstrnv,knah) < 0)
//         sumasoc+=sum;
       }

      if(prn == 2)
       {
        if(obud != NULL)
         {
          for(i=1; i <= obud[0] ; i++)
           if(knah == obud[i])
            {
             sumaou+=sum;      
             break;
            } 

         }
        if(knah == kodpen )
          penot+=sum;         
        if(knah == kodbzr)
          fondzn05m+=sum;         
//        if(knah == kodpn || knah == kodperpn)
        if(proverka(kodpn_all.ravno(),knah,0,1) == 0)
          spodohm+=sum;
        if(knah == kodsocstr)
         {
          socstrah+=sum;       
         }
       }
//      printw("%f %f\n",suman,sumaou);

     }
    sumani+=suman;
    sumaoui+=sumaou;
    spodoh+=spodohm;
    
    if(ffp != NULL)
      fprintf(ffp,"Начислено:%.2f Обязательные удержания:%.2f\n",suman,sumaou);

    koef=0.;
    if(masv[im] != 0.)
     koef=(suman+sumaou)/masv[im]*(-1);

    if(ffp != NULL)
      fprintf(ffp,"Коэффициент выплаты: %f=(%.2f %.2f)/%.2f\n",
      koef,suman,sumaou,masv[im]*-1);


    /*Читаем в файле соцотчислений пенсионное отчисление*/
    
        
    sprintf(strsql,"select kodz,sumas,sumap,proc from Zarsocz where \
datz='%d-%d-1' and tabn=%d",masg[im],masm[im],tabn);

    if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
     {
      msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
      continue;
     }
/*
    if(kolstr2 == 0)
     {
      continue;
     } 
*/
    //читаем настройки
//    zar_read_tn(1,masm[im],masg[im],NULL);
    
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
        if(fabs(atof(row1[3])-prpf) <= 0.001)
         {
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
      fondzn05p+=fondzn05m/koef;
    if(koef != 0.)
      spodohp+=spodohm/koef;
    if(ffp != NULL)
     {
      fprintf(ffp,"Подоходный: %.2f/%f=%.2f\n",spodohm,koef,spodohm/koef);
     }
   }
  if(sumavi == 0.)
    continue;

  itog1[0]+=sumani;
  itog1[1]+=sumaoui;

  itog1[2]+=sumani+sumaoui;
  itog2[2]+=sumavi;

  itog1[3]+=spodoh;
  itog2[3]+=spodohp;

  itog1[4]+=sumasbp;
  itog2[4]+=sumasbpp;

  itog1[5]+=penot32;
  itog2[5]+=penot32p;

  itog1[6]+=penot4;
  itog2[6]+=penot4p;

  itog1[7]+=penot1;
  itog2[7]+=penot1p;

  itog1[8]+=penot2;
  itog2[8]+=penot2p;

  itog1[9]+=penot32+penot4+(penot1+penot2)*-1;
  itog2[9]+=penot32p+penot4p+(penot1p+penot2p)*-1;

  itog1[10]+=sfondsc;
  itog2[10]+=sfondscp;

  itog1[11]+=fondsc;
  itog2[11]+=fondscp;

  itog1[12]+=sfondsc1;
  itog2[12]+=sfondscp1;

  itog1[13]+=sfondsc2;
  itog2[13]+=sfondscp2;
  
  
  itog1[14]+=fondsc+(sfondsc1+sfondsc2)*-1;
  itog2[14]+=fondscp+(sfondscp1+sfondscp2)*-1;

  itog1[15]+=sfondzn;
  itog2[15]+=sfondznp;

  itog1[16]+=fondzn;
  itog2[16]+=fondznp;

  itog1[17]+=fondzn05;
  itog2[17]+=fondzn05p;

  itog1[18]+=fondzn+fondzn05*-1;
  itog2[18]+=fondznp+fondzn05p*-1;

  itog1[19]+=sfondsci;
  itog2[19]+=sfondscip;

  itog1[20]+=fondsci;
  itog2[20]+=fondscip;

  itog1[21]+=sfondsons;
  itog2[21]+=sfondsonsp;

  itog1[22]+=fondsons;
  itog2[22]+=fondsonsp;
 
  fprintf(ff,"%4d|%-*.*s|%8.2f|%8.2f|%9.2f|\
%8.2f|\
%8.2f|%8.2f|%8.2f|%8.2f|%8.2f|%8.2f|\
%8.2f|%8.2f|\
%8.2f|%8.2f|%8.2f|%8.2f|\
%8.2f|%8.2f|%8.2f|%8.2f|%8.2f|%8.2f|%8.2f|%13.2f|\n",
  tabn,
  iceb_tu_kolbait(10,fio.ravno()),iceb_tu_kolbait(10,fio.ravno()),fio.ravno(),
  sumani,sumaoui*(-1),sumani+sumaoui,
  spodoh*(-1),
  sumasbp,penot32,penot4,penot1*-1,penot2*(-1),
  penot32+penot4+penot1*-1+penot2*(-1),
  sfondsc,fondsc,sfondsc1,sfondsc2,fondsc+sfondsc1+sfondsc2,
  sfondzn,fondzn,fondzn05*(-1),
  fondzn+fondzn05*(-1),
  sfondsci,fondsci,
  sfondsons,fondsons,
  spodoh*(-1)+\
  penot32+penot4+penot1*-1+penot2*(-1)+
  fondzn+fondzn05*(-1)+\
  fondsc+fondsci+fondsons); 
  
      
  fprintf(ff,"%33s|%9.2f|%8.2f|%8.2f|%8.2f|%8.2f|%8.2f|\
%8.2f|%8.2f|%8.2f|\
%8.2f|%8.2f|\
%8.2f|%8.2f|%8.2f|\
%8.2f|%8.2f|%8.2f|%8.2f|%8.2f|%8.2f|%8.2f|%13.2f|\n",
  " ",sumavi*(-1),
  spodohp*(-1),
  sumasbpp,penot32p,penot4p,penot1p*(-1),penot2p*(-1),
  penot32p+penot4p+penot1p*(-1)+penot2p*(-1),
  sfondscp,fondscp,sfondscp1,sfondscp2,fondscp+sfondscp1+sfondscp2,
  sfondznp,fondznp,fondzn05p*-1,
  fondznp+fondzn05p*-1,
  sfondscip,fondscip,
  sfondsonsp,fondsonsp,
  spodohp*(-1)+\
  penot32p+penot4p+penot1p*(-1)+penot2p*(-1)+\
  fondznp+fondzn05p*-1+\
  fondscip+fondscp+fondsonsp); 
  

 }

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"%*s|%8.2f|%8.2f|%9.2f|\
%8.2f|%8.2f|%8.2f|\
%8.2f|%8.2f|%8.2f|%8.2f|%8.2f|\
%8.2f|%8.2f|\
%8.2f|%8.2f|%8.2f|\
%8.2f|%8.2f|%8.2f|%8.2f|%8.2f|%8.2f|%8.2f|%13.2f|\n",
iceb_tu_kolbait(15,gettext("Итого")),gettext("Итого"),
itog1[0],
itog1[1]*-1,
itog1[2],
itog1[3]*(-1),
itog1[4],itog1[5],itog1[6],itog1[7]*-1,itog1[8]*-1,
itog1[9],itog1[10],
itog1[11],itog1[12],itog1[13]*-1,
itog1[14],itog1[15],
itog1[16],itog1[17]*-1,
itog1[18],
itog1[19],itog1[20],
itog1[21],itog1[22],
itog1[3]*(-1)+itog1[9]+itog1[14]+itog1[18]+itog1[20]+itog1[22]);

fprintf(ff,"%*s|%8s|%8s|%9.2f|\
%8.2f|%8.2f|%8.2f|\
%8.2f|%8.2f|%8.2f|%8.2f|%8.2f|\
%8.2f|%8.2f|\
%8.2f|%8.2f|%8.2f|\
%8.2f|%8.2f|%8.2f|%8.2f|%8.2f|%8.2f|%8.2f|%13.2f|\n",
iceb_tu_kolbait(15,gettext("Итого")),gettext("Итого")," "," ",
itog2[2]*-1,
itog2[3]*(-1),
itog2[4],itog2[5],itog2[6],itog2[7]*-1,itog2[8]*-1,
itog2[9],itog2[10],
itog2[11],itog2[12],itog2[13]*-1,
itog2[14],itog2[15],
itog2[16],itog2[17]*-1,
itog2[18],
itog2[19],itog2[20],
itog2[21],itog2[22],
itog2[3]*(-1)+itog2[9]+itog2[14]+itog2[18]+itog2[20]+itog2[22]);

//Распечатываем свод

fprintf(ffsv,"%s\n\
%s %s %d.%d.%d%s => %d.%d.%d%s\n\
%s %d.%d.%d%s  %s: %d:%d\n",
iceb_t_get_pnk("00",0),
"Отчисления в гос. фонды",
gettext("Период с"),
dn,mn,gn,
gettext("г."),
dk,mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ffsv,"\n%30s   %s\n",
gettext("Начислено"),
gettext("Получено"));


fprintf(ffsv,"\n\
%22s:%8.2f\n\
%22s:%8.2f\n\
%22s:%8.2f %8.2f\n\
%22s:%8.2f %8.2f\n",
gettext("Начислено"),
itog1[0],
gettext("Обязательные удержания"),
itog1[1]*-1,
gettext("К выдаче"),
itog1[2],itog2[2]*-1,
gettext("Подоходный налог"),
itog1[3]*-1,itog2[3]*-1);

//Читаем пенсионный фонд 
sprintf(strsql,"select naik from Zarsoc where kod=%d",kodpenf);
if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
  bros.new_plus(row1[0]);

fprintf(ffsv,"\n\
%s\n\
%22s:%8.2f %8.2f\n\
%21g%%:%8.2f %8.2f\n\
%21g%%:%8.2f %8.2f\n\
%21g%%:%8.2f %8.2f\n\
%21g%%:%8.2f %8.2f\n\
%22s:%8.2f %8.2f\n",
bros.ravno(),
gettext("C суммы"),
itog1[4],itog2[4],
prpf,
itog1[5],itog2[5],
tarif2,
itog1[6],itog2[6],
procpen[0],
itog1[7]*-1,itog2[7]*-1,
procpen[1],
itog1[8]*-1,itog2[8]*-1,
gettext("Итого"),
itog1[9],itog2[9]);

//Читаем процент отчисления в соц-страх
sprintf(strsql,"select naik from Zarsoc where kod=%d",kodsoc);
if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
  bros.new_plus(row1[0]);
else
  bros.new_plus("");
fprintf(ffsv,"\n\
%s\n\
%22s:%8.2f %8.2f\n\
%21g%%:%8.2f %8.2f\n\
%21g%%:%8.2f %8.2f\n\
%21g%%:%8.2f %8.2f\n\
%22s:%8.2f %8.2f\n",
bros.ravno(),
gettext("C суммы"),
itog1[10],itog2[10],
prsc,
itog1[11],itog2[11],
procsoc,
itog1[12]*-1,itog2[12]*-1,
procsoc1,
itog1[13]*-1,itog2[13]*-1,
gettext("Итого"),
itog1[14],itog2[14]);

//Читаем фонд безработицы
sprintf(strsql,"select naik from Zarsoc where kod=%d",kodbezf);
if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
  bros.new_plus(row1[0]);
else
  bros.new_plus("");

fprintf(ffsv,"\n\
%s\n\
%22s:%8.2f %8.2f\n\
%21g%%:%8.2f %8.2f\n\
%21g%%:%8.2f %8.2f\n\
%22s:%8.2f %8.2f\n",
bros.ravno(),
gettext("C суммы"),
itog1[15],itog2[15],
prbez,
itog1[16],itog2[16],
procbez,
itog1[17]*-1,itog2[17]*-1,
gettext("Итого"),
itog1[18],itog2[18]);

fprintf(ffsv,"\n\
%s\n\
%22s:%8.2f %8.2f\n\
%21g%%:%8.2f %8.2f\n",
"Фонд инвалидов",
gettext("C суммы"),
itog1[19],itog2[19],
1.,
itog1[20],itog2[20]);


//Читаем фонд страхования от несчасного случая
sprintf(strsql,"select naik from Zarsoc where kod=%d",kodfsons);
if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
  bros.new_plus(row1[0]);
else
  bros.new_plus(""); 

fprintf(ffsv,"\n\
%s\n\
%22s:%8.2f %8.2f\n\
%21g%%:%8.2f %8.2f\n",
bros.ravno(),
gettext("C суммы"),
itog1[21],itog2[21],
prsons,
itog1[22],itog2[22]);

fprintf(ffsv,"\n\
%s\n\
%22s:%8.2f %8.2f\n",
gettext("Итого"),
"6+12+17+21+23+25",
itog1[3]*(-1)+itog1[9]+itog1[14]+itog1[18]+itog1[20]+itog1[22],
itog2[3]*(-1)+itog2[9]+itog2[14]+itog2[18]+itog2[20]+itog2[22]);
return(1);

}
