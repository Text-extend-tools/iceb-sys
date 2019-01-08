/*$Id: kasothr1.c,v 5.28 2013/09/26 09:43:34 sasa Exp $*/
/*14.07.2015	27.12.2002	Белых А.И.	kasothr1.c
Послучение сводного отчёта по видам операций.
*/
#include        <errno.h>
#include        "buhg.h"

void rasoth(short,short,short,SPISOK*,int,int,double*,int,FILE*);
double  sumavk(int,int,int,double*);

void rasothdn(short,short,short,SPISOK*,int,int,double*,int,FILE*);
void rasoth_it(short dn,short mn,short gn,SPISOK *OPP,int kolopp,int kolden,double *prih,int metka,FILE *ff);
void kasothr1_rp(const char *kassa,const char *kodop,const char *shet,const char *vpr,const char *kod_kontr,FILE *ffdn);


int kasothr1(short dn,short mn,short gn,//Дата начала
short dk,short mk,short gk, //Дата конца
const char *kassa,const char *kodop,const char *shet,const char *vpr,const char *kod_kontr,class spis_oth *oth)
{
FILE            *ff,*ffdn;
char		imaf[64],imafdn[64];;
SQL_str		row,row1;
int		kolstr=0,kolstr1=0,kolstr2=0;
char		strsql[512];
time_t		tmm;
int		tipz=0;
int		kolopp=0; //Количество операций приходов
int		kolopr=0; //Количество операций расходов
int		kolden=0; //Количество дней в периоде
int		nomop=0;
int		nomden=0;
double		suma=0.;
short		den,mes,god;

time(&tmm);

clear();
printw("%s\n",gettext("Распечатка свода по операциям"));
printw("%d.%d.%d%s => %d.%d.%d%s\n",
dn,mn,gn,gettext("г."),
dk,mk,gk,gettext("г."));
GDITE();

//Вычисляем количество дней в периоде
kolden=period(dn,mn,gn,dk,mk,gk,0);

SQLCURSOR cur;
SQLCURSOR cur1;

//Читаем и запоминаем список операций на приход
sprintf(strsql,"select kod from Kasop1");

if((kolopp=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

SPISOK OPP(0);
while(cur.read_cursor(&row) != 0)
  OPP.plus(row[0]);

//Читаем и запоминаем список операций на расход
sprintf(strsql,"select kod from Kasop2");

if((kolopr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

SPISOK OPR(0);
while(cur.read_cursor(&row) != 0)
  OPR.plus(row[0]);

double  prih[kolden*kolopp]; //Массив данных приходов
double  rash[kolden*kolopr]; //Массив данных расходов

memset(&prih,'\0',sizeof(prih));
memset(&rash,'\0',sizeof(rash));

sprintf(strsql,"select kassa,tp,datd,nomd,shetk,kodop \
from Kasord where datd >= '%04d-%d-%d' and datd <= '%04d-%d-%d' \
order by datd,tp,nomd asc",gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }


kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4],row[5]);
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(kassa,row[0],0,0) != 0)
    continue;
  if(proverka(kodop,row[5],0,0) != 0)
    continue;
  tipz=atoi(row[1]);

  if(vpr[0] != '\0')
   {
    if(vpr[0] == '+' && tipz == 2)
      continue;
    if(vpr[0] == '-' && tipz == 1)
      continue;
   }
  if(proverka(shet,row[4],0,0) != 0)
    continue;
  
  rsdat(&den,&mes,&god,row[2],2);

  if(kod_kontr[0] != '\0')
   {
    sprintf(strsql,"select kontr from Kasord1 where kassa=%s and god=%d and tp=%s and nomd='%s'",
    row[0],god,row[1],row[3]);

    if(cur1.make_cursor(&bd,strsql) < 0)
      msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    int metka_kontr=0;
    while(cur1.read_cursor(&row1) != 0)
     {
      if(proverka(kod_kontr,row1[0],0,0) == 0)
       {
        metka_kontr=1;
        break;
        
       } 
     }
    if(metka_kontr == 0)
     continue;
   }
  
  //Читаем подтвержденные суммы в документе
  sprintf(strsql,"select suma from Kasord1 where kassa=%s and god=%d and \
tp=%d and nomd='%s' and datp <> '0000-00-00'",row[0],god,tipz,row[3]);

  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

  suma=0.;
  while(cur1.read_cursor(&row1) != 0)
   {
//    printw("suma=%s\n",row1[0]);
    suma+=atof(row1[0]);    
   }

  if(suma == 0.)
   continue;
  
  nomden=period(dn,mn,gn,den,mes,god,0)-1;
  
  if(tipz == 1)
    if((nomop=OPP.find(row[5])) >= 0)
     {
      prih[nomop*kolden+nomden]+=suma;
     }

  if(tipz == 2)
    if((nomop=OPR.find(row[5])) >= 0)
     {
      rash[nomop*kolden+nomden]+=suma;
     }
 }

GDITE();

//Распечатываем результаты
sprintf(imaf,"kasvop%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }


iceb_t_zagolov(gettext("Свод по видам операций"),dn,mn,gn,dk,mk,gk,ff);
kasothr1_rp(kassa,kodop,shet,vpr,kod_kontr,ff);

  
//Распечатываем результаты
sprintf(imafdn,"kasvopd%d.lst",getpid());
if((ffdn = fopen(imafdn,"w")) == NULL)
 {
  error_op_nfil(imafdn,errno,"");
  return(1);
 }


iceb_t_zagolov(gettext("Свод по видам операций"),dn,mn,gn,dk,mk,gk,ffdn);
kasothr1_rp(kassa,kodop,shet,vpr,kod_kontr,ffdn);

char imaf_it[64];
sprintf(imaf_it,"kasopi%d.lst",getpid());
class iceb_fopen ff_it;
if(ff_it.start(imaf_it,"w") != 0)
 return(1);

iceb_t_zagolov(gettext("Общие итоги по операциям"),dn,mn,gn,dk,mk,gk,ff_it.ff);
kasothr1_rp(kassa,kodop,shet,vpr,kod_kontr,ff_it.ff);

//Распечатываем приходы по операциям
rasoth(dn,mn,gn,&OPP,kolopp,kolden,prih,1,ff);

//Распечатываем расходы по операциям
rasoth(dn,mn,gn,&OPR,kolopr,kolden,rash,2,ff);

//Распечатываем приходы по дням
rasothdn(dn,mn,gn,&OPP,kolopp,kolden,prih,1,ffdn);

//Распечатываем расходы по дням
rasothdn(dn,mn,gn,&OPR,kolopr,kolden,rash,2,ffdn);

//Распечатываем общий итог приходы по операциям
rasoth_it(dn,mn,gn,&OPP,kolopp,kolden,prih,1,ff_it.ff);

//Распечатываем общий итог расходы по операциям
rasoth_it(dn,mn,gn,&OPR,kolopr,kolden,rash,2,ff_it.ff);

podpis(ff);
fclose(ff);

podpis(ffdn);
fclose(ffdn);

podpis(ff_it.ff);
ff_it.end();

printw_vr(tmm);

OSTANOV();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Свод по видам операций (даты по горизонтали)"));
oth->spis_imaf.plus(imafdn);
oth->spis_naim.plus(gettext("Свод по видам операций (даты по вертикали)"));
oth->spis_imaf.plus(imaf_it);
oth->spis_naim.plus(gettext("Общий итог по операциям"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),0);

return(0);
}
/***************************************/
/*перчать реквизитов поиска*/
/***********************************/
void kasothr1_rp(const char *kassa,const char *kodop,const char *shet,const char *vpr,const char *kod_kontr,FILE *ffdn)
{
if(kassa[0] != '\0')
  fprintf(ffdn,"%s:%s\n",gettext("Касса"),kassa);
if(kodop[0] != '\0')
  fprintf(ffdn,"%s:%s\n",gettext("Код операции"),kodop);
if(shet[0] != '\0')
  fprintf(ffdn,"%s:%s\n",gettext("Счёт"),shet);
if(vpr[0] != '\0')
  fprintf(ffdn,"%s:%s\n",gettext("Вид оп."),vpr);
if(kod_kontr[0] != '\0')
  fprintf(ffdn,"%s:%s\n",gettext("Код контрагента"),kod_kontr);
}

/**************************************************/
/*Распечатка результатов расчёта по операциям  (операции по вертикали - дни по горизонтали)  */
/*************************************************/

void 	rasoth(short dn,short mn,short gn,
SPISOK *OPP, //Список операций
int kolopp, //Количество операций
int kolden, //Количество дней в периоде
double *prih, //Данные
int metka, //1-приходные операции 2-расходные
FILE *ff)
{
int nomop=0;
int nomden=0;
char strsql[512];
SQL_str row;
double itog=0.,itogo=0.;
short   d,m,g;
int	kolnpk=0; //Количество не пустых колонок
SQLCURSOR curr;
//Определяем количество не пустых колонок 
for(nomden=0; nomden < kolden; nomden++)
 if(sumavk(nomden,kolopp,kolden,prih) != 0.)
   kolnpk++;

char	stroka[36+11*(kolnpk+1)+1];
memset(stroka,'-',sizeof(stroka)-1);
stroka[sizeof(stroka)-1]='\0';

if(metka == 1)
 fprintf(ff,"\n%s\n",gettext("Приходы"));
if(metka == 2)
 fprintf(ff,"\n%s\n",gettext("Расходы"));


fprintf(ff,"%s\n",stroka);

fprintf(ff,"К.о.|   Наименование операции      |");

d=dn; m=mn; g=gn;

for(nomden=0; nomden < kolden; nomden++)
 {
  if(sumavk(nomden,kolopp,kolden,prih) == 0.)
   {
    dpm(&d,&m,&g,1);
    continue;    
   }

  fprintf(ff,"%02d.%02d.%04d|",d,m,g);
  dpm(&d,&m,&g,1);
 }

fprintf(ff,"%*s|\n",iceb_tu_kolbait(10,gettext("Итого")),gettext("Итого"));

fprintf(ff,"%s\n",stroka);
class iceb_tu_str naimop("");

for(nomop=0; nomop < kolopp; nomop++)
 {
  /*Определяем нужно ли печатать горизонтальную строку*/
  itog=0;
  for(nomden=0; nomden < kolden; nomden++)
   {
    int nom=nomop*kolden+nomden;
    itog+=prih[nom];
    if(itog != 0.)
     break;
   }
  if(itog == 0)
   continue;

  
  //Читаем наименование операции
  if(metka == 1)
   sprintf(strsql,"select naik from Kasop1 where kod='%s'",OPP->ravno(nomop));
  if(metka == 2)
   sprintf(strsql,"select naik from Kasop2 where kod='%s'",OPP->ravno(nomop));
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    naimop.new_plus(row[0]);  
  else
    naimop.new_plus("");
  fprintf(ff,"%-*s %-*.*s",iceb_tu_kolbait(4,OPP->ravno(nomop)),OPP->ravno(nomop),
  iceb_tu_kolbait(30,naimop.ravno()),iceb_tu_kolbait(30,naimop.ravno()),naimop.ravno());
  itog=0.;
  for(nomden=0; nomden < kolden; nomden++)
   {
    if(sumavk(nomden,kolopp,kolden,prih) == 0.)
     continue;    
    int nom=nomop*kolden+nomden;
    fprintf(ff," %10.2f",prih[nom]);
    itog+=prih[nom];
   }
  fprintf(ff," %10.2f\n",itog);
 }
fprintf(ff,"%s\n",stroka);

fprintf(ff,"%*s",iceb_tu_kolbait(35,gettext("Итого")),gettext("Итого"));
for(nomden=0; nomden < kolden; nomden++)
 {
  if(sumavk(nomden,kolopp,kolden,prih) == 0.)
   continue;    
  itog=0.;
  for(nomop=0; nomop < kolopp; nomop++)
   {
    itog+=prih[nomop*kolden+nomden];
   }
  fprintf(ff," %10.2f",itog);
  itogo+=itog;
 }
fprintf(ff," %10.2f\n",itogo);
 
}

/******************************************/
/* Вычислить сумму по вертикальной колонке для распечатки по операциям*/
/******************************************/

double  sumavk(int nomden,int kolopp,int kolden,double *prih)
{
double  itog=0.;

for(int nomop=0; nomop < kolopp; nomop++)
 {
  itog+=prih[nomop*kolden+nomden];
 }

return(itog);

}

/****************************************************/
/*Вычислить сумму по вертикали для распечатки по дням*/
/*****************************************************/
double kasoth1_spvd(int nomop,int kolopp,int kolden,double *prih)
{
double itog=0.;
for(int nomden=0; nomden < kolden; nomden++)
  itog+=prih[nomop*kolden+nomden];
return(itog);
}

/*****************************************/
/*Распечатка результата отчёта по дням  (дни по вертикали - операции по горизонтали) */
/*****************************************/
void 	rasothdn(short dn,short mn,short gn,
SPISOK *OPP, //Список операций
int kolopp, //Количество операций
int kolden, //Количество дней в периоде
double *prih, //Данные
int metka, //1-приходные операции 2-расходные
FILE *ff)
{

int nomop=0;
int nomden=0;
double itog=0.,itogo=0.;
short   d,m,g;
int     nom=0;
char strsql[512];
class iceb_tu_str stroka;
SQL_str row;
class SQLCURSOR cur;
stroka.plus("-----------");
for(nomop=0; nomop < kolopp; nomop++) 
 {
  if(kasoth1_spvd(nomop,kolopp,kolden,prih) == 0)
   continue;
  stroka.plus("-----------");
 }
stroka.plus("-----------"); /*Итого*/


if(metka == 1)
 fprintf(ff,"\n%s\n",gettext("Приходы"));
if(metka == 2)
 fprintf(ff,"\n%s\n",gettext("Расходы"));


fprintf(ff,"%s\n",stroka.ravno());

fprintf(ff,"   Дата   |");
class iceb_tu_str naim_op("");
for(nomop=0; nomop < kolopp; nomop++)
 {
  if(kasoth1_spvd(nomop,kolopp,kolden,prih) == 0)
   continue;
  /*Читаем наименование операции*/
  sprintf(strsql,"select naik from Kasop%d where kod='%s'",metka,OPP->ravno(nomop));
  if(readkey(strsql,&row,&cur) == 1)
    naim_op.new_plus(row[0]);  
  else
    naim_op.new_plus("");
    
  sprintf(strsql,"%s %s",OPP->ravno(nomop),naim_op.ravno());
  fprintf(ff,"%-*.*s|",iceb_tu_kolbait(10,strsql),iceb_tu_kolbait(10,strsql),strsql);
 }
fprintf(ff,"%*s|\n",iceb_tu_kolbait(10,gettext("Итого")),gettext("Итого"));
fprintf(ff,"%s\n",stroka.ravno());

d=dn; m=mn; g=gn;
for(nomden=0; nomden < kolden; nomden++)
 {
  itog=0.;
  for(nomop=0; nomop < kolopp; nomop++)
    itog+=prih[nomop*kolden+nomden];

  if(itog == 0.)
   {
    dpm(&d,&m,&g,1);
    continue;
   }

  fprintf(ff,"%02d.%02d.%04d|",d,m,g);
     
  for(nomop=0; nomop < kolopp; nomop++)
   {
    if(kasoth1_spvd(nomop,kolopp,kolden,prih) == 0)
     continue;

    nom= nomop*kolden+nomden;
//    printw("nomden=%d kolopp=%d nomop=%d nom=%d suma=%f\n",
//    nomden,kolopp,nomop,nom,prih[nom]);
    
    fprintf(ff," %10.2f",prih[nom]);
   
   }  
  fprintf(ff," %10.2f\n",itog);
  dpm(&d,&m,&g,1);
 }
fprintf(ff,"%s\n",stroka.ravno());

fprintf(ff,"%*s|",iceb_tu_kolbait(10,gettext("Итого")),gettext("Итого"));

itogo=0.;
for(nomop=0; nomop < kolopp; nomop++)
 {
    if(kasoth1_spvd(nomop,kolopp,kolden,prih) == 0)
     continue;
  itog=0.;
  for(nomden=0; nomden < kolden; nomden++)
    itog+=prih[nomop*kolden+nomden];
  fprintf(ff," %10.2f",itog);
  itogo+=itog;   
 }
  fprintf(ff," %10.2f\n",itogo);
}
/************************************/
/*распечатываем общий итог*/
/***********************************/
void rasoth_it(short dn,short mn,short gn,
SPISOK *OPP, //Список операций
int kolopp, //Количество операций
int kolden, //Количество дней в периоде
double *prih, //Данные
int metka, //1-приходные операции 2-расходные
FILE *ff)
{
int nomop=0;
int nomden=0;
char strsql[512];
SQL_str row;
double itog=0.,itogo=0.;
SQLCURSOR curr;

if(metka == 1)
 fprintf(ff,"\n%s\n",gettext("Приходы"));
if(metka == 2)
 fprintf(ff,"\n%s\n",gettext("Расходы"));


fprintf(ff,"\
-----------------------------------------------\n");
fprintf(ff,"\
К.о.|   Наименование операции      |  Итого   |\n");
fprintf(ff,"\
-----------------------------------------------\n");


class iceb_tu_str naimop("");

for(nomop=0; nomop < kolopp; nomop++)
 {
  /*Определяем нужно ли печатать горизонтальную строку*/
  itog=0;
  for(nomden=0; nomden < kolden; nomden++)
   {
    int nom=nomop*kolden+nomden;
    itog+=prih[nom];
    if(itog != 0.)
     break;
   }
  if(itog == 0)
   continue;

  
  //Читаем наименование операции
  if(metka == 1)
   sprintf(strsql,"select naik from Kasop1 where kod='%s'",OPP->ravno(nomop));
  if(metka == 2)
   sprintf(strsql,"select naik from Kasop2 where kod='%s'",OPP->ravno(nomop));
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    naimop.new_plus(row[0]);
  else
    naimop.new_plus("");
  fprintf(ff,"%-*s %-*.*s",iceb_tu_kolbait(4,OPP->ravno(nomop)),OPP->ravno(nomop),
  iceb_tu_kolbait(30,naimop.ravno()),iceb_tu_kolbait(30,naimop.ravno()),naimop.ravno());
  itog=0.;
  for(nomden=0; nomden < kolden; nomden++)
   {
    if(sumavk(nomden,kolopp,kolden,prih) == 0.)
     continue;    
    int nom=nomop*kolden+nomden;
//    fprintf(ff," %10.2f",prih[nom]);
    itog+=prih[nom];
   }
  fprintf(ff," %10.2f\n",itog);
 }
fprintf(ff,"\
-----------------------------------------------\n");

fprintf(ff,"%*s",iceb_tu_kolbait(35,gettext("Итого")),gettext("Итого"));
for(nomden=0; nomden < kolden; nomden++)
 {
  if(sumavk(nomden,kolopp,kolden,prih) == 0.)
   continue;    
  itog=0.;
  for(nomop=0; nomop < kolopp; nomop++)
   {
    itog+=prih[nomop*kolden+nomden];
   }
//fprintf(ff," %10.2f",itog);
  itogo+=itog;
 }
fprintf(ff," %10.2f\n",itogo);
 
}
