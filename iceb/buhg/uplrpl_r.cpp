/*12.07.2016	03.05.2012	Белых А.И.	uplrpl_r.c
Реестр путевых листов по водителям
*/
#include <errno.h>
#include "buhg.h"


/**************************/
/*Надпись над шапкой*/
/**************************/
void uplrpl_r_nad(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *kodvod,
const char *naimvod,
const char *kodavt,
const char *naimavt,
const char *rnavt,
FILE *ff)
{
iceb_t_zagolov(gettext("Реестр путевых листов по водителям"),dn,mn,gn,dk,mk,gk,ff);

fprintf(ff,"\n\
%s %s %s\n\
%s %s %s %s %s\n",
gettext("Водитель"),
kodvod,naimvod,
gettext("Автомобиль"),
kodavt,
naimavt,
gettext("регистрационный номер"),
rnavt);
}

/*******************************/
/*Смотрим былали передача топлива между двумя датами*/
/****************************************/
double uplrpl_r_per_top(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *kodvod,
const char *kodavt,
int nom_zap,
int metka)  //0-до последней даты 1-включая последнюю дату
{
SQL_str row;
SQLCURSOR cur;
int kolstr;
char strsql[512];
double suma=0.;

//Смотрим передачи топлива другим водителям в тойже дате на тотже автомобиль

sprintf(strsql,"select datd,nomd,kolih from Upldok1 where datd = '%d-%d-%d' and \
kap=%s and kodsp=%s and tz=1 and nz = %d and ka=%s",
gn,mn,dn,kodavt,kodvod,nom_zap+1,kodavt);

//printw("strsql=%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(0);
 }

double isuma=0.;

while(cur.read_cursor(&row) != 0)
 {
  suma=atof(row[2]);
  suma=okrug(suma,0.001);

  isuma+=suma;
 }

//Смотрим передачи топлива другим автомобилям в тойже дате 
sprintf(strsql,"select datd,nomd,kolih from Upldok1 where datd = '%d-%d-%d' and \
kap=%s and kodsp = %s and tz=1 and ka <> %s",
gn,mn,dn,kodavt,kodvod,kodavt);

//printw("strsql=%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  suma=atof(row[2]);
  suma=okrug(suma,0.001);
  isuma+=suma;
 }



if(sravmydat(dn,mn,gn,dk,mk,gk) == 0)
 return(isuma); 
//Смотрим передачи топлива в других датах
if(metka == 0)
 sprintf(strsql,"select datd,nomd,kolih from Upldok1 where datd > '%d-%d-%d' and \
datd < '%d-%d-%d' and kap=%s and kodsp=%s and tz=1 order by datd,nomd asc",
gn,mn,dn,gk,mk,dk,kodavt,kodvod);

if(metka == 1)
 sprintf(strsql,"select datd,nomd,kolih from Upldok1 where datd > '%d-%d-%d' and \
datd <= '%d-%d-%d' and kap=%s and kodsp=%s and tz=1 order by datd,nomd asc",
gn,mn,dn,gk,mk,dk,kodavt,kodvod);

//printw("strsql=%s\n",strsql);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  suma=atof(row[2]);
  suma=okrug(suma,0.001);
  isuma+=suma;
 }

return(isuma);

}

/*********************************************************/
/*Расчёт*/
/***********************************************************/

int uplrpl_r(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *kodavt,
const char *kodvod,
const char *podr,
class spis_oth *oth)
{
char	strsql[1024];
int	kolstr=0;
SQL_str row,row1;
char	imaf[64],imaf_pl[64];
FILE    *ff;
time_t  tmmn;
SQLCURSOR curr;
time(&tmmn);

clear();
printw("%s\n",gettext("Реестр путевых листов"));
printw("%d.%d.%d => %d.%d.%d\n",dn,mn,gn,dk,mk,gk);
GDITE();

//Определяем список водителей 

sprintf(strsql,"select distinct kv,ka from Upldok1 where datd >= '%d-%d-%d' and \
datd <= '%d-%d-%d' order by kv,ka asc",gn,mn,dn,gk,mk,dk);

SQLCURSOR cur;
SQLCURSOR cur1;
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
class SPISOK KVKA;

int kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s\n",row[0],row[1]);
  
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(kodvod,row[0],0,0) != 0)
   continue;
  if(proverka(kodavt,row[1],0,0) != 0)
   continue;
  sprintf(strsql,"%s|%s",row[0],row[1]);
  KVKA.plus(strsql);

 }

class iceb_fopen fil_prot;
char imaf_prot[64];
sprintf(imaf_prot,"plprot%d.lst",getpid());
if(fil_prot.start(imaf_prot,"w") != 0)
 return(1);

iceb_t_zagolov(gettext("Протокол хода расчёта"),dn,mn,gn,dk,mk,gk,fil_prot.ff);



sprintf(imaf,"rpl%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

sprintf(imaf_pl,"pl%d.lst",getpid());
FILE *ff_pl;
if((ff_pl = fopen(imaf_pl,"w")) == NULL)
 {
  error_op_nfil(imaf_pl,errno,"");
  return(1);
 }

class iceb_tu_str kodvd("");
class iceb_tu_str kodav("");
class iceb_tu_str naimvod("");
class iceb_tu_str naimavt("");
class iceb_tu_str nom_znak("");
int kolstr2=0;
double iperedano=0.;
double prin=0.,poluh=0.;
double iprin=0.,ipoluh=0.;
double	kolih=0.;
double  kolihi=0.;
SQL_str row2;
class SQLCURSOR cur2;


int kol=KVKA.kolih();
for(int nomer=0; nomer < kol; nomer++)
 {
  strzag(LINES-1,0,kol,++kolstr2);

  class SPISOK ispei;
  class masiv_din_double ikolei;

  polen(KVKA.ravno(nomer),&kodvd,1,'|');
  polen(KVKA.ravno(nomer),&kodav,2,'|');
  class SPISOK KZMT; //Код заправки|марка топлива
  class masiv_din_double SMT;
  double  irash=0.;

  //Читаем водителя
  naimvod.new_plus("");
  sprintf(strsql,"select naik from Uplouot where kod=%s",kodvd.ravno());
//  printw("%s\n",strsql);
  if(readkey(strsql,&row,&curr) == 1)  
    naimvod.new_plus(row[0]);

  //Читаем автомобиль
  nom_znak.new_plus("");
  naimavt.new_plus("");
  sprintf(strsql,"select nomz,naik from Uplavt where kod=%s",kodav.ravno());
  if(readkey(strsql,&row,&curr) == 1)  
   {
    nom_znak.new_plus(row[0]);
    naimavt.new_plus(row[1]);
   }

  if(nomer != 0)
   {
    fprintf(ff,"\f");
    fprintf(ff_pl,"\f");
   }   

   uplrpl_r_nad(dn,mn,gn,dk,mk,gk,kodvd.ravno(),naimvod.ravno(),kodav.ravno(),naimavt.ravno(),nom_znak.ravno(),ff);


   fprintf(ff,"\
----------------------------------------------------------------------------------\n");

   fprintf(ff,gettext("\
Номер  |  Дата    |Показ.спидом.|Остаток|Принято|Получе-|Затраты|Передача|Остаток|\n\
п.листа|          |выезд |возвр.|       |       | но    |       | топлива|       |\n"));
                                                                                     
   fprintf(ff,"\
----------------------------------------------------------------------------------\n");

   iceb_t_zagolov(gettext("Путевые листы"),dn,mn,gn,dk,mk,gk,ff_pl);

   fprintf(ff_pl,"\n\
%s: %s %s\n\
%s: %s %s %s %s\n",
  gettext("Водитель"),
  kodvd.ravno(),naimvod.ravno(),
  gettext("Автомобиль"),
  kodav.ravno(),
  naimavt.ravno(),
  gettext("регистрационный номер"),
  nom_znak.ravno());
 
  sprintf(strsql,"select datd,kp,nomd,psv,psz,nz,otmet,denn,denk,vremn,vremk,nomb from Upldok where \
datd >= '%d-%d-%d' and datd <= '%d-%d-%d' and kv=%s and ka=%s order by datd asc",
  gn,mn,dn,gk,mk,dk,kodvd.ravno(),kodav.ravno()); 

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }

  printw("\n%s=%s %s=%s\n",
  gettext("Код водителя"),
  kodvd.ravno(),
  gettext("Код автомобиля"),
  kodav.ravno());
  refresh();    
  short dp=0,mp=0,gp=0;
  short dd=0,md=0,gd=0;
  int nom_zap=0;
  double ost1=0.,ost2=0.;
  while(cur.read_cursor(&row) != 0)
   {
    if(proverka(podr,row[1],0,0) != 0)
     continue;
    rsdat(&dd,&md,&gd,row[0],2);

    printw("%02d.%02d.%d %2s %s\n",dd,md,gd,row[1],row[2]);
    refresh();    
    if(dp != 0)
     {
      //смотрим былали передача топлива между двумя путевыми листами
      iperedano+=uplrpl_r_per_top(dp,mp,gp,dd,md,gd,kodvd.ravno(),kodav.ravno(),nom_zap,0);
     }
    nom_zap=atoi(row[5]);
    dp=dd;
    mp=md;
    gp=gd;

    sprintf(strsql,"select kodsp,kodtp,kolih,tz,kap from Upldok1 where datd='%s' and kp=%s and nomd='%s'",row[0],row[1],row[2]);

    if((kolstr1=cur1.make_cursor(&bd,strsql)) < 0)
      msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    
    ost1=ostva(dd,md,gd,kodvd.ravno(),kodav.ravno(),nom_zap,NULL,NULL,fil_prot.ff);
    printw("%s-ost1-0=%f\n",__FUNCTION__,ost1);

    fprintf(ff_pl,"\n%s:%02d.%02d.%d %s:%s\n",gettext("Дата"),dd,md,gd,gettext("Номер"),row[2]);

    fprintf(ff_pl,"%s:%.10g %s:%.10g %s:%.10g\n",
    gettext("Показания спидометра при выезде"),atof(row[3]),
    gettext("Показания спидометра при возвращении"),atof(row[4]),
    gettext("Пробег"),atof(row[4])-atof(row[3]));

    fprintf(ff_pl,"%s:%10s %s:%s\n",gettext("Дата выезда"),iceb_tu_datzap(row[7]),gettext("Время выезда"),row[9]);    
    fprintf(ff_pl,"%s:%10s %s:%s\n",gettext("Дата возвращения"),iceb_tu_datzap(row[8]),gettext("Время возвращения"),row[10]);    
    fprintf(ff_pl,"%s:%10s %s:%s\n",gettext("Номер бланка"),row[11],gettext("Отметки"),row[6]);
    
    fprintf(ff_pl,"%*.*s:%10.10g\n",
    iceb_tu_kolbait(10,gettext("Сальдо")),
    iceb_tu_kolbait(10,gettext("Сальдо")),
    gettext("Сальдо"),
    ost1);

    fprintf(ff_pl,"\
--------------------------------------\n");
    fprintf(ff_pl,gettext("\
+/-|Код т.|Количество|Поставщик\n"));
/*
123 123456 1234567890 
*/
    fprintf(ff_pl,"\
--------------------------------------\n");
    kolihi=0.;
    double kolspis=0;
    prin=poluh=0.;    
    int nomer1=0;
    char plus_minus[16];
    while(cur1.read_cursor(&row1) != 0)
     {
      if(row1[3][0] == '1')
       {
        strcpy(plus_minus,"+");     

        kolih=atof(row1[2]);
        kolih=okrug(kolih,0.001);

        kolihi+=kolih;
        if(atoi(row1[4]) != 0)
         prin+=kolih;
        else
         poluh+=kolih;
        sprintf(strsql,"%s|%s",row1[0],row1[1]);

        if((nomer1=KZMT.find_r(strsql)) < 0)
          KZMT.plus(strsql);
        SMT.plus(kolih,nomer1);
       }
      else
       {
       strcpy(plus_minus,"-");     

       kolspis+=atof(row1[2]);
       kolspis=okrug(kolspis,0.001);
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
    ost2=ost1+kolihi-kolspis;
    ost2=okrug(ost2,0.001);

    fprintf(ff_pl,"\
--------------------------------------\n");


    fprintf(ff_pl,"%*.*s:%10.10g\n",
    iceb_tu_kolbait(10,gettext("Сальдо")),
    iceb_tu_kolbait(10,gettext("Сальдо")),
    gettext("Сальдо"),
    ost2);

    irash+=kolspis;

    printw("%*s %02d.%02d.%d %6s %6s %7.7g %7.7g %7.7g %7.7g %8s %7.7g %s\n",
    iceb_tu_kolbait(7,row[2]),row[2],
    dd,md,gd,
    row[3],
    row[4],
    ost1,
    prin,
    poluh,
    kolspis,
    "",
    ost2,
    row[6]);
    refresh();
    
    fprintf(ff,"%*s %02d.%02d.%d %6s %6s %7.7g %7.7g %7.7g %7.7g %8s %7.7g %s\n",
    iceb_tu_kolbait(7,row[2]),row[2],
    dd,md,gd,
    row[3],
    row[4],
    ost1,
    prin,
    poluh,
    kolspis,
    "",
    ost2,
    row[6]);

    printw("%s-ost1=%f\n",__FUNCTION__,ost1);

    iprin+=prin;
    ipoluh+=poluh;    


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
    int nomei=0;
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


      if((nomei=ispei.find_r(row1[5])) < 0)
        ispei.plus(row1[5]);             
      ikolei.plus(atof(row1[1]),nomei);
     }
    fprintf(ff_pl,"\
---------------------------------------------------------------------------------------------------------------------------------------\n");

    fprintf(ff_pl,"%*s:%16.16g %16.16g %10.10g\n",
    iceb_tu_kolbait(73,gettext("Итого")),
    gettext("Итого"),
    itogo_zt[0],itogo_zt[1],itogo_zt[2]);

    iperedano+=uplrpl_r_per_top(dp,mp,gp,dk,mk,gk,kodvd.ravno(),kodav.ravno(),nom_zap,1);
 
    fprintf(ff,"\
----------------------------------------------------------------------------------\n");

    fprintf(ff,"%*s %7.7g %7.7g %7.10g %8.8g\n",
    iceb_tu_kolbait(40,gettext("Итого")),gettext("Итого"),
    iprin,ipoluh,irash,iperedano);


   }


  fprintf(ff,"%s\n",gettext("Свод по маркам топлива"));
  fprintf(ff,"\
-----------------------------------------------------\n");
  fprintf(ff,gettext("\
Код|Наименование поставщика|Марка топлива|Количество|\n"));
  fprintf(ff,"\
-----------------------------------------------------\n");
  int kol1=KZMT.kolih();

  for(int nomer1=0; nomer1 < kol1; nomer1++)
    {
     polen(KZMT.ravno(nomer1),&kodvd,1,'|');
     polen(KZMT.ravno(nomer1),&kodav,2,'|');

     //Читаем поставщика
     naimvod.new_plus("");
     sprintf(strsql,"select naik from Uplouot where kod=%s",kodvd.ravno());
     if(sql_readkey(&bd,strsql,&row,&curr) == 1)  
       naimvod.new_plus(row[0]);
 
     fprintf(ff,"%*s %-*.*s %-*.*s %10.10g\n",
     iceb_tu_kolbait(3,kodvd.ravno()),
     kodvd.ravno(),
     iceb_tu_kolbait(23,naimvod.ravno()),
     iceb_tu_kolbait(23,naimvod.ravno()),
     naimvod.ravno(),
     iceb_tu_kolbait(13,kodav.ravno()),
     iceb_tu_kolbait(13,kodav.ravno()),
     kodav.ravno(),
     SMT.ravno(nomer1));  

    }
  fprintf(ff,"\
-----------------------------------------------------\n");

  fprintf(ff,"\n\%s\n",gettext("Свод по единицам измерения"));
  fprintf(ff,"\
---------------------------------------------------\n");
  fprintf(ff,gettext("\
Ед.изм.|Наименование единицы измерения|Количество\n"));
/*
1234567 123456789012345678901234567890 
*/

  fprintf(ff,"\
---------------------------------------------------\n");
  class iceb_tu_str naimei("");
  for(int nom=0; nom < ispei.kolih(); nom++)
   {
      naimei.new_plus("");
      sprintf(strsql,"select naik from Edizmer where kod='%s'",ispei.ravno(nom));
      if(readkey(strsql,&row1,&cur1) == 1)
       naimei.new_plus(row1[0]);      
      fprintf(ff,"%-*s %-*.*s %10.2f\n",
      iceb_tu_kolbait(7,ispei.ravno(nom)),
      ispei.ravno(nom),
      iceb_tu_kolbait(30,naimei.ravno()),
      iceb_tu_kolbait(30,naimei.ravno()),
      naimei.ravno(),
      ikolei.ravno(nom));
   }    
  fprintf(ff,"\
---------------------------------------------------\n");


  fprintf(ff,"\n%s__________________\n",gettext("Бухгалтер"));

 }
podpis(ff);
fclose(ff);
podpis(ff_pl);
fclose(ff_pl);
fil_prot.end();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Реестр путевых листов"));

oth->spis_imaf.plus(imaf_pl);
oth->spis_naim.plus(gettext("Путевые листы"));

oth->spis_imaf.plus(imaf_prot);
oth->spis_naim.plus(gettext("Протокол хода расчёта"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);


return(0);
}
