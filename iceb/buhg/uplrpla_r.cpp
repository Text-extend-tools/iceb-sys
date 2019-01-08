/*$di:$*/
/*02.02.2017	04.05.2012	Белых А.И.	uplrpla_r.c
Реестр путевых листов по автомобилям
*/
#include <errno.h>
#include "buhg.h"


/*************************/
/*Смотрим от дня последней путёвки в периоде до конца заданного периода - не было ли передач топлива*/
/********************************************/
double uplrpla_pt(short dpp,short mpp,short gpp,
short dk,short mk,short gk,
const char *kodavt)
{
SQL_str row;
SQLCURSOR cur;
int kolstr;
char strsql[512];
double suma=0.;
//short d,m,g;
double isuma=0.;

sprintf(strsql,"select datd,nomd,kolih from Upldok1 where datd >= '%d-%d-%d' \
and datd <= '%d-%d-%d' and kap=%s and tz=1 and ka <> '%s'",
gpp,mpp,dpp,gk,mk,dk,kodavt,kodavt);

//printw("strsql=%s\n",strsql);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(0.);
 }

while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s\n",row[0],row[1],row[2]);  
  suma=atof(row[2]);
  isuma+=suma;
 }

return(isuma);

}

/*******************************/
/*Смотрим былали передача топлива между предыдущей путёвкой и настоящей*/
/****************************************/
double uplrpla_per_top_avt(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *kodavt)
{
SQL_str row;
SQLCURSOR cur;
int kolstr;
char strsql[512];
double suma=0.;
//short d,m,g;
double isuma=0.;


sprintf(strsql,"select datd,nomd,kolih from Upldok1 where datd >= '%d-%d-%d' and \
datd < '%d-%d-%d' and kap=%s and tz=1 and ka <> '%s'",
gn,mn,dn,gk,mk,dk,kodavt,kodavt);

//printw("strsql=%s\n",strsql);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(0.);
 }

while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s\n",row[0],row[1],row[2]);  
  suma=atof(row[2]);
  isuma+=suma;
 }

return(isuma);

}

/***************************************/


int uplrpla_r(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *kodavt,
const char *kodvod,
const char *podr,
class spis_oth *oth)
{
char strsql[1024];
int	kolstr=0;
SQL_str row,row1,row2;
class SQLCURSOR cur,cur2;
class SQLCURSOR cur1;
time_t  tmmn;
time(&tmmn);

clear();
printw("%s\n",gettext("Реестр путевых листов"));
printw("%d.%d.%d => %d.%d.%d\n",dn,mn,gn,dk,mk,gk);
GDITE();


//Определяем список aвтомобилей
/********
sprintf(strsql,"select distinct ka from Upldok1 where datd >= '%d-%d-%d' and \
datd <= '%d-%d-%d' order by kv,ka asc",gn,mn,dn,gk,mk,dk);
**********/
sprintf(strsql,"select distinct ka from Upldok1 where datd >= '%d-%d-%d' and \
datd <= '%d-%d-%d'",gn,mn,dn,gk,mk,dk);

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

class SPISOK avtomobil;

int kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s\n",row[0],row[1]);
  
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(kodavt,row[0],0,0) != 0)
   continue;

  avtomobil.plus(row[0]);

 }

char imaf[64];
FILE *ff;

sprintf(imaf,"rplpa%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

char imaf_pl[64];
FILE *ff_pl;

sprintf(imaf_pl,"rplpap%d.lst",getpid());

if((ff_pl = fopen(imaf_pl,"w")) == NULL)
 {
  error_op_nfil(imaf_pl,errno,"");
  return(1);
 }
class iceb_tu_str kodav("");
class iceb_tu_str naimavt("");
class iceb_tu_str rnavt("");
int kol=avtomobil.kolih();
int kolstr2=0;
for(int nomer=0; nomer < kol; nomer++)
 {
  strzag(LINES-1,0,kol,++kolstr2);
  kodav.new_plus(avtomobil.ravno(nomer));

  class SPISOK KZMT; //Код заправки|марка топлива
  class masiv_din_double SMT;

  //Читаем автомобиль
  rnavt.new_plus("");
  naimavt.new_plus("");
  sprintf(strsql,"select nomz,naik from Uplavt where kod=%s",kodav.ravno());
  if(sql_readkey(&bd,strsql,&row,&cur1) == 1)  
   {
    rnavt.new_plus(row[0]);
    naimavt.new_plus(row[1]);
   }

  if(nomer != 0)
    fprintf(ff,"\f");

  iceb_t_zagolov(gettext("Реестр путевых листов по автомобилям"),dn,mn,gn,dk,mk,gk,ff);

  fprintf(ff,"\n\
%s %s %s %s %s\n",
  gettext("Автомобиль"),
  kodav.ravno(),
  naimavt.ravno(),
  gettext("регистрационный номер"),
  rnavt.ravno());

  fprintf(ff,"\
----------------------------------------------------------------------------------------\n");

  fprintf(ff,gettext("\
Номер  |  Дата    |Показ.спидом.|Пробег|Остаток|Приня- |Получе-|Затраты|Переда-|Остаток|\n\
п.листа|          |выезд |возвр.|      |       |  то   |  но   |       |  но   |       |\n"));
//1        2         3      4      5      6        7         8     9      10     11    
  fprintf(ff,"\
----------------------------------------------------------------------------------------\n");

   iceb_t_zagolov(gettext("Путевые листы"),dn,mn,gn,dk,mk,gk,ff_pl);

   fprintf(ff_pl,"\n\
%s: %s %s %s %s\n",
  gettext("Автомобиль"),
  kodav.ravno(),
  naimavt.ravno(),
  gettext("регистрационный номер"),
  rnavt.ravno());




  sprintf(strsql,"select datd,kp,nomd,psv,psz,kv,nz,otmet,denn,denk,vremn,vremk,nomb from Upldok where \
datd >= '%d-%d-%d' and datd <= '%d-%d-%d' and ka=%s order by datd,nomd asc",
  gn,mn,dn,gk,mk,dk,kodav.ravno()); 

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }
  printw("\n%s=%s\n",
  gettext("Код автомобиля"),
  kodav.ravno());
  refresh();    
  short dd=0,md=0,gd=0;  
  short dp=0,mp=0,gp=0;  
  int nom_zap=0;
  double iperedano=0.;
  double ost1=0.,ost2=0.;
  double iprin=0.;
  double ipoluh=0.;
  double irash=0.;
  int itogo_probeg=0;
  class iceb_tu_str naimvod("");
  while(cur.read_cursor(&row) != 0)
   {
    if(proverka(podr,row[1],0,0) != 0)
     continue;
    if(proverka(kodvod,row[18],0,0) != 0)
     continue;
   //Читаем водителя
   naimvod.new_plus("");
   sprintf(strsql,"select naik from Uplouot where kod=%s",row[5]);
   if(readkey(strsql,&row1,&cur1) == 1)  
     naimvod.new_plus(row1[0]);

    rsdat(&dd,&md,&gd,row[0],2);
    nom_zap=atoi(row[6]);
    printw("%02d.%02d.%d %2s %s\n",dd,md,gd,row[1],row[2]);
    refresh();    
/************************
   Несколько передач топлива с одной машины на другие в течение одного дня не должно быть
   а если будут то пусть она их все покажет после первого путевого листа в общей сумме
*********************/
    if(dp != 0 && sravmydat(dp,mp,gp,dd,md,gd) != 0)
     {
      //смотрим былали передача топлива между двумя путевыми листами
      iperedano+=uplrpla_per_top_avt(dp,mp,gp,dd,md,gd,kodav.ravno());
     }
    dp=dd;
    mp=md;
    gp=gd;

    sprintf(strsql,"select kodsp,kodtp,kolih,tz,kap,ka from Upldok1 where datd='%s' and kp=%s and nomd='%s'",row[0],row[1],row[2]);

    if((kolstr1=cur1.make_cursor(&bd,strsql)) < 0)
     {
      msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
      continue;
     }
    ost1=ostva(dd,md,gd,"",kodav.ravno(),nom_zap,NULL,NULL,NULL);

    fprintf(ff_pl,"\n%s:%02d.%02d.%d %s:%s\n",gettext("Дата"),dd,md,gd,gettext("Номер"),row[2]);

    fprintf(ff_pl,"%s:%.10g %s:%.10g %s:%.10g\n",
    gettext("Показания спидометра при выезде"),atof(row[3]),
    gettext("Показания спидометра при возвращении"),atof(row[4]),
    gettext("Пробег"),atof(row[4])-atof(row[3]));

    fprintf(ff_pl,"%s:%10s %s:%s\n",gettext("Дата выезда"),iceb_tu_datzap(row[8]),gettext("Время выезда"),row[10]);    
    fprintf(ff_pl,"%s:%10s %s:%s\n",gettext("Дата возвращения"),iceb_tu_datzap(row[9]),gettext("Время возвращения"),row[11]);    
    fprintf(ff_pl,"%s:%10s %s:%s\n",gettext("Номер бланка"),row[12],gettext("Отметки"),row[7]);
    fprintf(ff_pl,"%s:%s %s\n",gettext("Водитель"),row[5],naimvod.ravno());
    fprintf(ff_pl,"%*.*s:%10.10g\n",
    iceb_tu_kolbait(10,gettext("Сальдо")),
    iceb_tu_kolbait(10,gettext("Сальдо")),
    gettext("Сальдо"),
    ost1);

    fprintf(ff_pl,"\
--------------------------------------\n");
    fprintf(ff_pl,"\
+/-|Код т.|Количество|Поставщик\n");
/*
123 123456 1234567890 
*/
    fprintf(ff_pl,"\
--------------------------------------\n");

    int probeg=0;
    double iprobeg=0.;
    double kolih=0.;
    double kolihi=0.;
    double kolspis=0;
    double prin=0.,poluh=0.;    
    int nomer1=0;
    char plus_minus[16];
    while(cur1.read_cursor(&row1) != 0)
     {
//      printw("*****%s %s %s %s %s %s\n",row1[0],row1[1],row1[2],row1[3],row1[4],row1[5]);
      if(row1[3][0] == '1')
       {
        strcpy(plus_minus,"+");     
        
        kolih=atof(row1[2]);

        if(atoi(row1[4]) != 0)
         prin+=kolih;
        else
         poluh+=kolih;
         
        kolihi+=kolih;
        sprintf(strsql,"%s|%s",row1[0],row1[1]);

        if((nomer1=KZMT.find(strsql)) < 0)
          KZMT.plus(strsql);
        SMT.plus(kolih,nomer1);
       }
      else
       {
        strcpy(plus_minus,"-");     
        kolspis+=atof(row1[2]);
       }
     
      memset(strsql,'\0',sizeof(strsql));
      if(atoi(row1[0]) != 0)
       {
        if(atoi(row1[4]) == 0)
          sprintf(strsql,"select naik from Uplouot where kod=%s and mt=0",row1[0]); //заправка
        else
          sprintf(strsql,"select naik from Uplouot where kod=%s and mt=1",row1[0]); //водитель

        if(readkey(strsql,&row2,&cur2) == 1)
          sprintf(strsql,"%s %s",row1[0],row2[0]);
        else
          sprintf(strsql,"%s",row1[0]);
       }

      fprintf(ff_pl,"%3s %*s %10.10g %s\n",
      plus_minus,
      iceb_tu_kolbait(6,row1[1]),
      row1[1],
      atof(row1[2]),
      strsql);    

     }
    iprin+=prin;
    ipoluh+=poluh;
    
    ost2=ost1+kolihi-kolspis;
    ost2=okrug(ost2,0.001);

    fprintf(ff_pl,"\
--------------------------------------\n");


    fprintf(ff_pl,"%*.*s:%10.10g\n",
    iceb_tu_kolbait(10,gettext("Сальдо")),
    iceb_tu_kolbait(10,gettext("Сальдо")),
    gettext("Сальдо"),
    ost2);
    
    iprobeg+=atof(row[6]);
    irash+=kolspis;



    probeg=atoi(row[4])-atoi(row[3]);
    itogo_probeg+=probeg;
    


    fprintf(ff,"%*s %02d.%02d.%d %6s %6s \
%6d %7.7g %7.7g %7.7g %7.7g %7s %7.7g %s\n",
    iceb_tu_kolbait(7,row[2]),row[2],
    dd,md,gd,
    row[3],
    row[4],
    probeg,
    ost1,
    prin,
    poluh,
    kolspis,
    " ",
    ost2,
    row[7]);

   fprintf(ff,"\
 ----------------------------------------------------------------------------------------\n");
   fprintf(ff,"%*s %6d %7s %7.7g %7.7g %7.7g %7.7g\n",
   iceb_tu_kolbait(32,gettext("Итого")),gettext("Итого"),
   itogo_probeg,
   "",
   iprin,
   ipoluh,
   irash,
   iperedano);


     fprintf(ff_pl,"%s\n",gettext("Расчёт списания топлива по нормам"));
     fprintf(ff_pl,"\
---------------------------------------------------------------------------------------------------------------------------------------\n");
     fprintf(ff_pl,gettext("\
Код н.с.|  Наименование нормы списания |Ед.изм. |Количество|Норма списан.|Затраты по норме|Затраты по факту| Разница  |Коментарий\n"));
 /*************
 12345678 123456789012345678901234567890 12345678 1234567890 1234567890123 1234567890123456 1234567890123456 1234567890
 *******************/
     fprintf(ff_pl,"\
---------------------------------------------------------------------------------------------------------------------------------------\n");

     /*смотрим расчёт списания*/
     sprintf(strsql,"select knst,kl,nst,zn,zf,ei,kom from Upldok3 where datd='%s' and kp=%s \
   and nomd='%s'",row[0],row[1],row[2]);

     if((kolstr1=cur1.make_cursor(&bd,strsql)) < 0)
         msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
     
     class iceb_tu_str naim_nst("");
     
     double itogo_zt[3];
     memset(itogo_zt,'\0',sizeof(itogo_zt));
     
     while(cur1.read_cursor(&row1) != 0)
      {
       naim_nst.new_plus("");
       sprintf(strsql,"select naik from Uplnst where kod='%s'",row1[0]);
       if(readkey(strsql,&row2,&cur2) == 1)
        naim_nst.new_plus(row2[0]);    
       fprintf(ff_pl,"%-*s %-*.*s %-*s %10.10g %13.13g %16.16g %16.16g %10.10g %s\n",
       iceb_tu_kolbait(8,row1[0]),
       row1[0],
       iceb_tu_kolbait(30,naim_nst.ravno()),
       iceb_tu_kolbait(30,naim_nst.ravno()),
       naim_nst.ravno(),
       iceb_tu_kolbait(8,row1[5]),
       row1[5],
       atof(row1[1]),
       atof(row1[2]),
       atof(row1[3]),
       atof(row1[4]),
       atof(row1[4])-atof(row1[3]),
       row1[6]);    

       itogo_zt[0]+=atof(row1[3]);
       itogo_zt[1]+=atof(row1[4]);
       itogo_zt[2]+=atof(row1[4])-atof(row1[3]);
      }
     fprintf(ff_pl,"\
---------------------------------------------------------------------------------------------------------------------------------------\n");

     fprintf(ff_pl,"%*s:%16.16g %16.16g %10.10g\n",
     iceb_tu_kolbait(73,gettext("Итого")),
     gettext("Итого"),
     itogo_zt[0],itogo_zt[1],itogo_zt[2]);
    
   }

  if(dd != 0)
   iperedano+=uplrpla_pt(dd,md,gd,dk,mk,gk,kodav.ravno());
   
    



  int kol1=KZMT.kolih();
  class iceb_tu_str kodvd("");

//  uplrpl_sapi(ff); //uplrpl.c
  fprintf(ff,"\
-----------------------------------------------------\n");
  fprintf(ff,gettext("\
Код|Наименование поставщика|Марка топлива|Количество|\n"));
  fprintf(ff,"\
-----------------------------------------------------\n");

  for(int nomer1=0; nomer1 < kol1; nomer1++)
    {
     polen(KZMT.ravno(nomer1),&kodvd,1,'|');
     polen(KZMT.ravno(nomer1),&kodav,2,'|');

     //Читаем поставщика
     naimvod.new_plus("");
     sprintf(strsql,"select naik from Uplouot where kod=%s",kodvd.ravno());
     if(sql_readkey(&bd,strsql,&row,&cur1) == 1)  
       naimvod.new_plus(row[0]);

     fprintf(ff,"%3s %-*.*s %-13.13s %10.10g\n",
     kodvd.ravno(),
     iceb_tu_kolbait(23,naimvod.ravno()),iceb_tu_kolbait(23,naimvod.ravno()),naimvod.ravno(),
     kodav.ravno(),SMT.ravno(nomer1));  
    }

  fprintf(ff,"\
-----------------------------------------------------\n");

  fprintf(ff,"\n%s__________________\n",gettext("Бухгалтер"));


 }

podpis(ff);
fclose(ff);
podpis(ff_pl);
fclose(ff_pl);

printw_vr(tmmn);


OSTANOV();
oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Реестр путевых листов по автомобилям"));
oth->spis_imaf.plus(imaf_pl);
oth->spis_naim.plus(gettext("Путевые листы"));

for(int nom=0; nom < oth->spis_imaf.kolih();nom++)
  iceb_t_ustpeh(imaf,3);

return(0); 
}
 
