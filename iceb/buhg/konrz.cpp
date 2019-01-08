/* $Id: konrz.c,v 5.58 2013/09/26 09:43:35 sasa Exp $ */
/*25.05.2016    30.11.1992      Белых А.И.      konrz.c
Контроль расчёта зарплаты
*/
#include <math.h>
#include        <errno.h>
#include        "buhg.h"

extern double   okrg; /*Округление*/
extern short    *kodmp;   /*Код материальной помощи*/
extern short    mmm,ggg;
extern class iceb_tu_str shrpz; /*Счет расчётов по зарплате*/
extern short kodpn;   /*Код подоходного налога*/
extern short kodpn_sbol;/*Код подоходного налога с больничного*/

void            konrz()
{
short           sovm; /*Совместитель 0-нет 1- да*/
short		d,m,g;
struct  tm      *bf;
time_t          tmm;
int            tabb;   /*Табельный номер*/
class iceb_tu_str pr(""); /*Дата приема и увольнения с работы*/
FILE            *f,*ff;
char            imaf[64],imaf1[64];
int            kol;  /*Количество человек*/
short           metk=1; /*1-нет записей 0-есть*/
double          sum,sumn,sumu;
double          itsl,itsn,itsu,itdl;
short           kolkd,kolkd1; /*Количество календарных дней*/
short		kolrd=0; //Количество рабочих дней
class iceb_tu_str sss("");
double		sal;
double		sumd,sumk;
long		kolstr,kolstr1,kols;
int kolstr2=0;
SQL_str         row,row1,row2;
char		strsql[1024];
class iceb_tu_str fio("");
int		podr,kateg;
short		zvan;
class iceb_tu_str shet("");
class iceb_tu_str lgoti("");
double		saldb;
short		dn,mn,gn,dk,mk,gk;
short		mkont,gkont;
double		sumnprov=0.;//Сумма начисленная на которую надо сделать проводки
double		sumuprov=0.;//Сумма удержаний на которую надо сделать проводки
int itabn=0;

int kom=0;
class iceb_tu_str vstr("");
static class iceb_tu_str dat1("");
static class iceb_tu_str tabnom("");
clear(); /*Очистить экран и закрасить фоновым цветом*/

if(dat1.getdlinna() <= 1)
 {
  short dt,mt,gt;
  poltekdat(&dt,&mt,&gt);
  if(dt < 15)
   dpm(&dt,&mt,&gt,4); //Уменьшить на месяц     
  sprintf(strsql,"%02d.%d",mt,gt);
  dat1.new_plus(strsql);
 }
class VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Контроль расчёта зарплаты"));

VV.VVOD_SPISOK_add_MD(gettext("Дата расчёта..(м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер.(,,)."));

VV.VVOD_SPISOK_add_data(dat1.ravno(),8);
VV.VVOD_SPISOK_add_data(tabnom.ravno(),128);



naz:;

helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("таб.ном."),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   iceb_t_pdoc("buhg5_1.txt"); 
   goto naz;

  case ESC:
  case FK10:
   return;

  case FK2:
  case PLU:
     break;


  case FK3:
    if(dirtab(&itabn,&fio,0,1) == 0)
     {
      VV.data_z_plus(1,itabn);
     }
    clear();
    goto naz;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
tabnom.new_plus(VV.VVOD_SPISOK_return_data(1));

if(rsdat1(&mkont,&gkont,dat1.ravno()) != 0)
 {
  iceb_t_soob(gettext("Не верно введене дата !"));
  goto naz; 
 }

clear();
GDITE();
time(&tmm);
bf=localtime(&tmm);

//читаем текущие настройки
//zar_read_tn(1,mkont,gkont,NULL);

sprintf(strsql,"select tabn,fio,datk,sovm,datn,podr,kateg,shet,zvan,lgoti,datn,dolg from Kartb");
SQLCURSOR cur,cur1;
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


sprintf(imaf,"nnz%d.lst",getpid());
sprintf(imaf1,"szr%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }
if((f = fopen(imaf1,"w")) == NULL)
 {
  error_op_nfil(imaf1,errno,"");
  return;
 }

char imaf_p[64];
sprintf(imaf_p,"konrz%d.lst",getpid());

if(zar_pr_start(imaf_p) != 0)
 return;

/*Чтение настроек для расчёта единого социального взноса*/
zarrnesv();




fprintf(f,"%s\n\n\
	     %s %d.%d%s\n\
	   %s %d.%d.%d%s %s: %d:%d\n",
iceb_t_get_pnk("00",0),
gettext("Зарплата по организации за"),
mkont,gkont,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(f,"\
------------------------------------------------------------------------------------------\n");
fprintf(f,gettext("Т.Н.|    Фамилия  Имя Отчество               | Сальдо   |Начислено |  Удержано |К выдаче\n"));

fprintf(f,"\
------------------------------------------------------------------------------------------\n");

fprintf(ff,"%s\n%s %d.%d%s\n\n\
%s %d.%d.%d%s %s: %d:%d\n\
---------------------------------------------------------------\n\n",
iceb_t_get_pnk("00",0),
gettext("Контроль расчёта зарплаты за"),
mkont,gkont,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);
SQLCURSOR curr;
kols=kol=0;
itdl=itsl=itsn=itsu=0.;
sovm=0;
short dp,mp,gp; /*Дата приёма на работу*/
short du,mu,gu; /*Дата увольнения*/

while(cur.read_cursor(&row) != 0)
 {

  strzag(LINES-1,0,kolstr,++kols);
  if(proverka(tabnom.ravno(),row[0],0,0) != 0)
   continue;
     
  tabb=atoi(row[0]);
/*
  printw("%d\n",tabb);
  refresh();
*/  
  fio.new_plus(row[1]);

  rsdat(&dp,&mp,&gp,row[4],2);
  rsdat(&du,&mu,&gu,row[2],2);
  
//  memset(pr,'\0',sizeof(pr));
//  sprintf(pr,"%s/%s",row[4],row[2]);
  pr.new_plus(row[4]);
  pr.plus("/",row[2]);
  
  sovm=atoi(row[3]);
  podr=atoi(row[5]);
  kateg=atoi(row[6]);
  shet.new_plus(row[7]);
  zvan=atoi(row[8]);
  lgoti.new_plus(row[9]);

  sum=sumn=sumu=0.;

  /*Читаем сальдо*/
  sal=0.;
  sum=sal=zarsald(1,mkont,gkont,tabb,&saldb);
  
  if(fabs(sal) > 0.009)
   {
    /*Проверяем был ли вход в карточку*/
    sprintf(strsql,"select tabn from Zarn where tabn=%d and god=%d and \
mes=%d",tabb,gkont,mkont);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      rsdat(&dn,&mn,&gn,row[10],2);
      rsdat(&dk,&mk,&gk,row[2],2);
      zapzarn(mkont,gkont,podr,tabb,kateg,sovm,zvan,shet.ravno(),lgoti.ravno(),dn,mn,gn,dk,mk,gk,0,row[11]);
     }

    if(rsdat(&d,&m,&g,row[2],2) == 0)
     {
      printw("%d %20.20s %s %d.%d.%d - %s %.2f\n",
      tabb,fio.ravno(),gettext("Уволен"),d,m,g,
      gettext("не нулевое сальдо"),sal);

      fprintf(ff,"\n%d %s %s %d.%d.%d - %s %.2f\n",
      tabb,fio.ravno(),gettext("Уволен"),d,m,g,
      gettext("не нулевое сальдо"),sal);
      continue;
     }
   }
  else
   {
    //Проверяем нет ли начислений или удержаний 
    sprintf(strsql,"select prn from Zarp where tabn=%d and datz >= '%04d-%2d-%2d' \
and datz <= '%04d-%2d-%2d' and suma <> 0.",tabb,gkont,mkont,1,gkont,mkont,31);
    if(readkey(strsql,&row1,&curr) == 0)
     {
      /*Проверяем был ли вход в карточку*/
      sprintf(strsql,"select tabn from Zarn where tabn=%d and god=%d and \
mes=%d",tabb,gkont,mkont);
      if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
       {
        rsdat(&dn,&mn,&gn,row[10],2);
        rsdat(&dk,&mk,&gk,row[2],2);
        zapzarn(mkont,gkont,podr,tabb,kateg,sovm,zvan,shet.ravno(),lgoti.ravno(),dn,mn,gn,dk,mk,gk,0,row[11]);
       }
     }    
   }
   
  itsl+=sal;

  if(rsdat(&d,&m,&g,row[2],2) == 0)
    continue;

  sumnprov=sumuprov=0.;

  sprintf(strsql,"select prn,knah,suma from Zarp where datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' and tabn=%d and \
  suma <> 0. order by prn,knah asc",gkont,mkont,gkont,mkont,tabb);

  class SQLCURSOR cur1;
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }

  metk=0;
  if(kolstr2 == 0)
    metk=1;

  double sumanah=0;
  while(cur1.read_cursor(&row1) != 0)
   {
    sumanah=atof(row1[2]);
    sum+=sumanah;
    if(atoi(row1[0]) == 1)
     {
      sumn+=sumanah;
      //Определяем нужно ли делать проводки
      sprintf(strsql,"select prov from Nash where kod=%d",atoi(row1[1]));
      if(readkey(strsql,&row2,&curr) == 1)
       if(atoi(row2[0]) == 0)
         sumnprov+=sumanah;
     }
    else
     {
      sumu+=sumanah;
       //Определяем нужно ли делать проводки
       
       sprintf(strsql,"select prov from Uder where kod=%d",atoi(row1[1]));
       if(readkey(strsql,&row2,&curr) == 1)
        if(atoi(row2[0]) == 0)
          sumuprov+=sumanah;
     }
   }





    itsn+=sumn;
    itsu+=sumu;

    if(sal+sumn+sumu < 0.)
      itdl+=sal+sumn+sumu;

    /*расчитываем подоходный налог*/
    double suma_podoh_ras=ras_podoh1(tabb,mkont,gkont,podr,1,0.,"");
    double suma_podoh_nah=0.;
    /*смотрим сколько его посчитано*/

    sprintf(strsql,"select suma from Zarp where datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' \
 and tabn=%d and prn='2' and (knah=%d or knah=%d)",gkont,mkont,gkont,mkont,tabb,
    kodpn,
    kodpn_sbol);

    if((kolstr1=cur1.make_cursor(&bd,strsql)) < 0)
     {
      msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
      continue;
     }
    while(cur1.read_cursor(&row1) != 0)
     {
      suma_podoh_nah+=atof(row1[0]);
     }     

    if(fabs(suma_podoh_nah - suma_podoh_ras) > 0.009)
     {
      printw("%4d %-*s %s\n",tabb,iceb_tu_kolbait(30,fio.ravno()),fio.ravno(),pr.ravno());
      printw("%s %.2f != %.2f\n",gettext("Неправильно посчитан подоходный налог"),suma_podoh_nah,suma_podoh_ras);
      fprintf(ff,"%4d %-*s %s\n",tabb,iceb_tu_kolbait(30,fio.ravno()),fio.ravno(),pr.ravno());
      fprintf(ff,"%s %.2f != %.2f\n",gettext("Неправильно посчитан подоходный налог"),suma_podoh_nah,suma_podoh_ras);
      refresh();
      kol++;
     }


  if(metk == 1 || sum <=0. || sumn == 0. || sumu == 0.)
   {

    if(metk == 1)
     {
      printw("%s ",gettext("Нет записей"));
      fprintf(ff,"%s ",gettext("Нет записей"));
     }
    else
     {
      if(sumn == 0.)
       {
        printw("%s ",gettext("Нет начислений"));
        fprintf(ff,"%s ",gettext("Нет начислений"));
       }
      if(sumu == 0.)
       {
        printw("%s ",gettext("Нет удержаний"));
        fprintf(ff,"%s ",gettext("Нет удержаний"));
       }

      if(sumn != 0. && sumu != 0.)
       {
        if(fabs(sum) <= 0.0001 )
         {
          printw("%s ",gettext("Сальдо равно нолю"));
          fprintf(ff,"%s ",gettext("Сальдо равно нолю"));
         }
        else
         {
          printw("%s ",gettext("Отрицательное сальдо"));
          fprintf(ff,"%s ",gettext("Отрицательное сальдо"));
         }
       }
     }
    printw("%4d %-30s %s\n",tabb,fio.ravno(),pr.ravno());
    fprintf(ff,"%4d %-30s %s\n",tabb,fio.ravno(),pr.ravno());
    kol++;
    refresh();
   }

  /*Смотрим сумму выполненых проводок*/ 
  if(shrpz.ravno()[0] != '\0')
   {
//    sprintf(sss,"%d-%d",mkont,tabb);
    sss.new_plus(mkont);
    sss.plus("-");
    sss.plus(tabb);

    sumprz(mkont,gkont,sss.ravno(),0,&sumd,&sumk);


    if(fabs(sumd-sumnprov) > 0.009 || fabs(sumuprov*(-1)-sumk) > 0.009)
     {
      printw("%4d %-*s  %s (%f %f/%f %f)\n",
      tabb,iceb_tu_kolbait(30,fio.ravno()),fio.ravno(),gettext("Не выполнены все проводки !"),
      sumd,sumnprov,sumuprov*-1,sumk);
      fprintf(ff,"%4d %-*s  %s (%f %f/%f %f)\n",
      tabb,iceb_tu_kolbait(30,fio.ravno()),fio.ravno(),gettext("Не выполнены все проводки !"),
      sumd,sumnprov,sumuprov*-1,sumk);
     }      
   }

  /*Смотрим введён ли табель и проверяем его*/
  kolrd=kolkd=0;

  sprintf(strsql,"select dnei,kdnei from Ztab where god=%d and \
mes=%d and tabn=%d",gkont,mkont,tabb);
  if((kolstr1=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }

  if(kolstr1 == 0)
   {        
    printw("%s - %d %s %s %d.%d%s\n",
    gettext("Табель не введен"),tabb,fio.ravno(),
    gettext("за"),mkont,gkont,
    gettext("г."));
    fprintf(ff,"%s - %d %s %s %d.%d%s\n",
    gettext("Табель не введен"),tabb,fio.ravno(),
    gettext("за"),mkont,gkont,
    gettext("г."));
    goto vp;
   }

  while(cur1.read_cursor(&row1) != 0)
   {
    kolrd+=atoi(row1[0]);
    kolkd+=atoi(row1[1]);
   }  
  if(mkont == 1 || mkont == 3 || mkont == 5 || mkont == 7 || mkont == 8 || mkont == 10 || mkont == 12)
    kolkd1=31;
  else
    kolkd1=30;
  /*В месяц приёма\увольнения количество календарных дней другое*/  
  if(mp != mkont && mu != mkont) 
   {
    if(mkont == 2 )
     {
      if(kolkd > 29 || kolkd < 28)
       {
        printw("%s %d %s\n",
        gettext("Не верно введено количество календарных дней"),
        tabb,fio.ravno());
        fprintf(ff,"\n%d %s %s\n%s\n",
        tabb,fio.ravno(),pr.ravno(),
        gettext("Не верно введено количество календарных дней"));
       }
     }
    else
     if(kolkd != kolkd1)
      {
       printw("%s %d %s\n",
       gettext("Не верно введено количество календарных дней"),
       tabb,fio.ravno());
       fprintf(ff,"\n%d %s %s %d != %d %d %d\n%s\n",
       tabb,fio.ravno(),pr.ravno(),kolkd,kolkd1,mp,mu,
       gettext("Не верно введено количество календарных дней"));
      }
   }
  if(kolkd != 0 && kolrd > kolkd)
   {
     printw("%s %d %s\n",
     gettext("Количество рабочих дней больше количества календарных дней"),
     tabb,fio.ravno());
     fprintf(ff,"%s %d %s\n",
     gettext("Количество рабочих дней больше количества календарных дней"),
     tabb,fio.ravno());
   }
 vp:;

   /*Проверяем был ли вход в карточку*/
   sprintf(strsql,"select tabn from Zarn where tabn=%d and god=%d and \
mes=%d",tabb,gkont,mkont);
   if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
    {
     printw("%s %d %s\n",
     gettext("Не было входа в карточку для"),tabb,fio.ravno());
     fprintf(ff,"%s %d %s\n",
     gettext("Не было входа в карточку для"),tabb,fio.ravno());
    }
  fprintf(f,"%-4d %-*s %10.2f %10.2f %10.2f %10.2f\n",
  tabb,iceb_tu_kolbait(40,fio.ravno()),fio.ravno(),sal,sumn,sumu,sal+sumn+sumu);

 }


fprintf(f,"\
------------------------------------------------------------------------------------------\n");
fprintf(f,"%-*s %10.2f %10.2f %10.2f %10.2f\n",
iceb_tu_kolbait(45,gettext("Итого")),gettext("Итого"),itsl,itsn,itsu,itsl+itsn+itsu-itdl);

fprintf(ff,"\
---------------------------------------------------------------\n\
%s %d\n",gettext("Количество работников"),kol);
printw("%s %d\n",gettext("Количество работников"),kol);

fclose(f);
fclose(ff);

zar_pr_end();


printw("\n\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n\
%s %15.2f\n",
gettext("Сальдо         :"),itsl,
gettext("Итого начислено:"),itsn,
gettext("Итого удержано :"),itsu,
gettext("Итого долгов   :"),itdl,
gettext("К выдаче       :"),itsl+itsn+itsu-itdl);


OSTANOV();

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Контроль расчёта зарплаты"));
oth.spis_imaf.plus(imaf1);
oth.spis_naim.plus(gettext("Свод начислений и удержаний"));
oth.spis_imaf.plus(imaf_p);
oth.spis_naim.plus(gettext("Протокол хода расчёта"));

for(int nom=0; nom < oth.spis_imaf.kolih();nom++)
 iceb_t_ustpeh(oth.spis_imaf.ravno(nom),3);

iceb_t_rabfil(&oth,"");
}
