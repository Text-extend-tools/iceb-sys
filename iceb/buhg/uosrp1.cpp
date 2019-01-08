/* $Id: uosrp1.c,v 5.29 2013/09/26 09:43:42 sasa Exp $ */
/*19.01.2017    19.10.1994      Белых А.И.      uosrp1.c
Сортировка проводок и выдача распечатки
Возвращаем 0 - если нет проводок
	   1 - если есть
*/
#include <math.h>
#include        <errno.h>
#include        "buhg.h"


void            uosrp1_shetchik(short,short,short,short,short,short,int*,int*,int,const char*,FILE*);
void            rasid(double,short,short,short,short,short,short,int,const char*,int*,int*,FILE*);
void            rasio(const char*,double,short,short,short,short,short,short,int,const char*,const char*,int*,int*,class SPISOK *mso,class masiv_din_double *mspo,FILE*);
void            rasis(const char*,double,short,short,short,short,short,short,int,const char*,const char*,int*,int*,class SPISOK *mso,class masiv_din_double *mspo,class SPISOK *mss,class masiv_din_double *msps,FILE*);
void            rasoi(class SPISOK*,class masiv_din_double*,FILE*);
void uosrp1_sapka(int skl1,const char *naiskl,short dn,short mn,short gn,short dk,short mk,short gk,int *kollist,int *kolstrok,FILE *ff);

extern short    ddd,mmm,ggg;
extern int kol_strok_na_liste;

int uosrp1(VVOD *VV,class spis_oth *oth)
{
class SPISOK mso;
class masiv_din_double mspo;
class SPISOK mss;  /*Массив счетов по складу*/
class masiv_din_double msps; /*Массив сумм по складу*/
class SPISOK mssi; /*Массив счетов по всем складам*/
class masiv_din_double mspsi; /*Массив сумм проводок по всем складам по операции*/
class SPISOK mssio; /*Массив счетов по всем складам и всем операциям*/
class masiv_din_double mspsio; /*Массив сумм проводок по всем складам по всем операциям*/

int           kst=0,sli=0;
FILE            *ff;
FILE            *ffitog;
class iceb_tu_str kop("");
class iceb_tu_str bros("");
class iceb_tu_str naiskl("");
struct  tm      *bf;
time_t          tmm;
time_t          tmmn;
time_t          tmmk;
short           kom=0;
class iceb_tu_str kome(""); /*Коментарий*/
long            muf; /*Количество записей в файле*/
short           dp,mp,gp;
short           dd,md,gd;
class iceb_tu_str orgn("");
char            imaf[64],imaftmp[64],imafitog[64];
class iceb_tu_str shett("");
double          itd; /*Итого по счёту*/
int             i;
double          k1,s1;
short           tipzz;
int		tz; /*1-Приход 2-расход*/
short           klr;
short           dn,mn,gn;
short           dk,mk,gk;
double		deb;
int		skl=0,skl1=0;
class iceb_tu_str nomdok("");
class iceb_tu_str nomdokv("");
class iceb_tu_str kprr(""),kprr1("");		
class iceb_tu_str naimpr("");
long		kolstr,kolstr1;
SQL_str         row,row1;
char		strsql[1024];
class iceb_tu_str kontr("");
int		tipz;
class iceb_tu_str naikont("");
class iceb_tu_str koment("");
SQLCURSOR curr;
clear();
GDITE();

time(&tmmn);


tz=0;
if(VV->VVOD_SPISOK_return_data(4)[0] == '+')
  tz=1;
if(VV->VVOD_SPISOK_return_data(4)[0] == '-')
  tz=2;

if(rsdatp(&dn,&mn,&gn,VV->VVOD_SPISOK_return_data(0),&dk,&mk,&gk,VV->VVOD_SPISOK_return_data(1)) != 0)
  return(1);

sprintf(strsql,"select datp,sh,shk,nomd,pod,deb,datd,komen,kodkon,oper,tz from Prov \
where val=0 and \
datp >= '%04d-%02d-%02d' and \
datp <= '%04d-%02d-%02d' and kto='%s' and deb <> 0.",
gn,mn,dn,gk,mk,dk,ICEB_MP_UOS);

/*
printw("\n%s\n",strsql);
refresh();
*/
SQLCURSOR cur;
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

class iceb_t_tmptab tabtmp;
const char *imatmptab={"uosrp"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
datp DATE not null,\
sh char(32) not null,\
shk char(32) not null,\
nomd char(32) not null,\
pod int not null,\
deb double(14,2) not null,\
datd DATE not null,\
komen char(255) not null,\
kodkon char(32) not null,\
oper char(32) not null,\
tz int not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  

printw("%s: %d.%d.%d%s => %d.%d.%d%s\n",
gettext("Дата"),
dn,mn,gn,
gettext("г."),
dk,mk,gk,
gettext("г."));

printw("%s\n",gettext("Сортируем проводки"));

refresh();
kolstr1=muf=0;
while(cur.read_cursor(&row) != 0)
 {
/*
  printw("%s %s %s %s %s %s %s %s %s %s %s\n",
  row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8],
  row[9],row[10]);
*/
  strzag(LINES-1,0,kolstr,++kolstr1);
  
  /*Смотрим только дебеты*/
/*
  if( fabs(atof(row[8])) > 0.009)
    continue;
*/    

  /*Подразделение*/
  if(proverka(VV->VVOD_SPISOK_return_data(2),row[5],0,0) != 0)
    continue;
   
  /*Читаем документ*/
  sprintf(strsql,"select tipz,kodop,kontr,nomdv \
from Uosdok where datd='%s' and nomd='%s'",
  row[6],row[3]);
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    beep();  
    printw("%s N%s %s %s !",gettext("Не найден документ !"),
    row[3],gettext("Дата"),row[6]);
    OSTANOV();
    continue;
   }  
  tipz=atoi(row1[0]);
  kprr.new_plus(row1[1]);
  kontr.new_plus(row1[2]);
  nomdokv.new_plus(row1[3]);
  if(tz != 0 && tz != tipz)
   continue;

  /*Организация*/
  if(proverka(VV->VVOD_SPISOK_return_data(3),kontr.ravno(),0,0) != 0)
    continue;

  /*Операция*/
  if(proverka(VV->VVOD_SPISOK_return_data(5),kprr.ravno(),0,0) != 0)
    continue;


  /*Счёт*/
  if(proverka(VV->VVOD_SPISOK_return_data(6),row[1],0,0) != 0)
    continue;
      
  muf++;  
  koment.new_plus(row[7]);      

  sprintf(strsql,"insert into %s values ('%s','%s','%s','%s',%s,%s,'%s','%s','%s','%s',%s)",
  imatmptab,
  row[0],row[1],row[2],nomdokv.ravno_filtr(),row[4],row[5],row[6],koment.ravno_filtr(),kontr.ravno(),kprr.ravno(),row[10]);

  sql_zapis(strsql,1,0);    

 }


if(muf == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }

sprintf(strsql,"select * from %s order by tz asc,oper asc,pod asc,kodkon asc,nomd asc",imatmptab);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

sprintf(imaf,"rp%d%d.lst",mn,mk);

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }


time(&tmm);
bf=localtime(&tmm);
kst=0;

if(VV->VVOD_SPISOK_return_data(2)[0] != '\0')
 {
  kst++;
  fprintf(ff,"%s: %s\n",gettext("Подразделение"),VV->VVOD_SPISOK_return_data(2));
 }
if(VV->VVOD_SPISOK_return_data(3)[0] != '\0')
 {
  kst++;
 fprintf(ff,"%s: %s\n",gettext("Код контрагента"),VV->VVOD_SPISOK_return_data(3));
 }
if(VV->VVOD_SPISOK_return_data(4)[0] != '\0')
 {
  kst++;
  fprintf(ff,"%s: %s\n",gettext("Вид операции"),VV->VVOD_SPISOK_return_data(4));
 }
if(VV->VVOD_SPISOK_return_data(5)[0] != '\0')
 {
  kst++;
  fprintf(ff,"%s: %s\n",gettext("Код операции"),VV->VVOD_SPISOK_return_data(5));
 }
if(VV->VVOD_SPISOK_return_data(6)[0] != '\0')
 {
  kst++;
  fprintf(ff,"%s: %s\n",gettext("Счёт"),VV->VVOD_SPISOK_return_data(6));
 }
if(VV->VVOD_SPISOK_return_data(7)[0] != '\0')
 {
  kst++;
  fprintf(ff,"%s:%s\n",gettext("Набор операций"),VV->VVOD_SPISOK_return_data(7));
 }



skl=0;
nomdok.new_plus("");

tipzz=kom=0;
klr=sli=0;
itd=0.;
nomdokv.new_plus("");
nomdok.new_plus("");
kolstr1=0;
kontr.new_plus("");
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,kolstr1);
  rsdat(&dp,&mp,&gp,row[0],2);

  shett.new_plus(row[1]);
  shett.plus(",",row[2]);
  nomdok.new_plus(row[3]);
  skl1=atoi(row[4]);
  deb=atof(row[5]);
  rsdat(&dd,&md,&gd,row[6],2);
  kome.new_plus(row[7]);
  
  rsdat(&dd,&md,&gd,row[6],2);
  
  orgn.new_plus(row[8]);

  kprr.new_plus(row[9]);
  tz=atoi(row[10]);
   
  if(SRAV(kprr.ravno(),kprr1.ravno(),0) != 0 || tipzz != tz)
   {
    if(kprr1.ravno()[0] != '\0' )
     {
      rasis(orgn.ravno(),itd,dn,mn,gn,dk,mk,gk,skl1,naikont.ravno(),naiskl.ravno(),&sli,&kst,&mso,&mspo,&mss,&msps,ff);
      itd=0.;
      rasoi(&mssi,&mspsi,ff);


      fprintf(ff,"\f");
     }
    skl=0;
    nomdokv.new_plus("");
    kontr.new_plus("");

    /*Читаем наименование операции*/
    if(tz == 1)
     sprintf(strsql,"select naik from Uospri where kod='%s'",kprr.ravno());
    if(tz == 2)
     sprintf(strsql,"select naik from Uosras where kod='%s'",kprr.ravno());

    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      beep();
      printw(gettext("Не нашли код операции %s !"),kprr.ravno());
      printw("\n");
      fprintf(ff,gettext("Не нашли код операции %s !"),kprr.ravno());
      fprintf(ff,"\n");
      OSTANOV();
      naimpr.new_plus("");
     }
    else
     naimpr.new_plus(row1[0]);
     
    kprr1.new_plus(kprr.ravno());
    tipzz=tz;
    /*move(5,0); */
    printw("%d/%s %s\n",tz,kprr.ravno(),naimpr.ravno());
//    refresh();
    if(tz == 1)
     bros.new_plus(gettext("(приход)"));
    if(tz == 2)
     bros.new_plus(gettext("(расход)"));

    /*Читаем наименование подразделения*/
    sprintf(strsql,"select naik from Uospod where kod=%d",skl1);

    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      beep();
      printw("%s %d !\n",gettext("Нет найден код подразделения"),skl1);
      fprintf(ff,"%s %d !\n",gettext("Нет найден код подразделения"),skl1);
      OSTANOV();
      naiskl.new_plus("");
     }
    else
     naiskl.new_plus(row1[0]);
          
    fprintf(ff,"%s\n\
%s %s - \"%s\" (%s) %s %d.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d%s  %s: %d:%d\n",
iceb_t_get_pnk("00",0),
gettext("Реестр проводок по виду операции"),
kprr.ravno(),naimpr.ravno(),bros.ravno(),
gettext("за период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

    kst+=3;
    klr+=sli;
    sli=0;
    uosrp1_sapka(skl1,naiskl.ravno(),dn,mn,gn,dk,mk,gk,&sli,&kst,ff);

   }

  if(skl != skl1)
   {
    if(skl != 0)
     {
      rasis(orgn.ravno(),itd,dn,mn,gn,dk,mk,gk,skl1,naikont.ravno(),naiskl.ravno(),&sli,&kst,&mso,&mspo,&mss,&msps,ff);
      itd=0.;
      kst=0;
      klr+=sli;
      sli=0;
      fprintf(ff,"\f");
     }
    nomdokv.new_plus("");
    kontr.new_plus("");
    kom=1;


    /*Читаем наименование подразделения*/
    sprintf(strsql,"select naik from Uospod where kod=%d",skl1);

    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      beep();
      printw("%s %d !\n",gettext("Нет найден код подразделения"),skl1);
      fprintf(ff,"%s %d !\n",gettext("Нет найден код подразделения"),skl1);
      OSTANOV();
      naiskl.new_plus("");
     }
    else
     naiskl.new_plus(row1[0]);

    printw("%s %d %s\n",gettext("Подразделение"),skl1,naiskl.ravno());
//    refresh();
    skl=skl1;
   }

  if(SRAV(kontr.ravno(),orgn.ravno(),0) != 0)
   {
    if(kontr.ravno()[0] != '\0')
     {
      rasio(orgn.ravno(),itd,dn,mn,gn,dk,mk,gk,skl1,naikont.ravno(),naiskl.ravno(),&sli,&kst,&mso,&mspo,ff);
      itd=0.;
     }
    nomdokv.new_plus("");
    kom=1;

    if(ATOFM(orgn.ravno()) == 0. && polen(orgn.ravno(),&bros,1,'-') == 0 && orgn.ravno()[0] == '0')
     kop.new_plus(bros.ravno());
    else
     kop.new_plus(orgn.ravno());

    naikont.new_plus("");

    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kop.ravno());

    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      beep();
      printw("%s %s !\n",gettext("Не нашли код контрагента"),kop.ravno());
      OSTANOV(); 
     }
    else
     naikont.new_plus(row1[0]);

    printw("%s %s %s",gettext("Контрагент"),orgn.ravno(),naikont.ravno());
    printw("\n");
//    refresh();
    kontr.new_plus(orgn.ravno());
   }
  if(SRAV(nomdok.ravno(),nomdokv.ravno(),0) != 0)
   {
      if(nomdokv.ravno()[0] != '\0')
       {
	rasid(itd,dn,mn,gn,dk,mk,gk,skl1,naiskl.ravno(),&sli,&kst,ff);
	itd=0.;
       }
      nomdokv.new_plus(nomdok.ravno());
   }



     /*Записываем в массив организаций*/
   if((i=mso.find(shett.ravno())) < 0)
    mso.plus(shett.ravno());
   mspo.plus(deb,i);

   /*Записываем в массив по складу*/
   if((i=mss.find(shett.ravno())) < 0)
    mss.plus(shett.ravno());
   msps.plus(deb,i);

   /*Записываем в массив по складам */
   if((i=mssi.find(shett.ravno())) < 0)
    mssi.plus(shett.ravno());
    
   mspsi.plus(deb,i);

   /*Записываем в массив по складам и операциям*/
   if((i=mssio.find(shett.ravno())) < 0)
    mssio.plus(shett.ravno());
   mspsio.plus(deb,i);

   uosrp1_shetchik(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl.ravno(),ff);

   printw("%02d.%02d.%4d %02d.%02d.%4d %-11s %-11s %15.2f\n",
   dp,mp,gp,dd,md,gd,nomdok.ravno(),shett.ravno(),deb);
//   refresh();
   if(kom == 1)
     fprintf(ff,"\
%-*s %-*.*s %02d.%02d.%4d %02d.%02d.%4d %-*s %-*s %20s\n",
     iceb_tu_kolbait(7,orgn.ravno()),orgn.ravno(),
     iceb_tu_kolbait(52,naikont.ravno()),
     iceb_tu_kolbait(52,naikont.ravno()),
     naikont.ravno(),
     dd,md,gd,dp,mp,gp,
     iceb_tu_kolbait(11,nomdok.ravno()),nomdok.ravno(),
     iceb_tu_kolbait(11,shett.ravno()),shett.ravno(),
     prnbr(deb));

   if(kom == 0)
     fprintf(ff,"\
%60s %02d.%02d.%4d %02d.%02d.%4d %-*s %-*s %20s %s\n",
    " ",dd,md,gd,dp,mp,gp,
     iceb_tu_kolbait(11,nomdok.ravno()),nomdok.ravno(),
     iceb_tu_kolbait(11,shett.ravno()),shett.ravno(),
    prnbr(deb),kome.ravno());

   itd+=deb;
   kom=0;

 }
rasis(orgn.ravno(),itd,dn,mn,gn,dk,mk,gk,skl1,naikont.ravno(),naiskl.ravno(),&sli,&kst,&mso,&mspo,&mss,&msps,ff);
rasoi(&mssi,&mspsi,ff);

if(VV->VVOD_SPISOK_return_data(5)[0] == '\0' || ( VV->VVOD_SPISOK_return_data(5)[0] != '\0' && POLE(VV->VVOD_SPISOK_return_data(5),strsql,2,',') == 0))
 {
  k1=0.;
  fprintf(ff,"\n==================================================\n\
%s:\n",
  gettext("Общий итог по всем операциям"));
  for(i=0; i< mssio.kolih() ;i++)
   {
    fprintf(ff,"%-*s - %20s\n",iceb_tu_kolbait(10,mssio.ravno(i)),mssio.ravno(i),prnbr(mspsio.ravno(i)));
    k1+=mspsio.ravno(i);
   }
  fprintf(ff,"%-*s - %20s\n",iceb_tu_kolbait(10,gettext("Итого")),gettext("Итого"),prnbr(k1));
 }

unlink(imaftmp);


podpis(ff);
fclose(ff);

sprintf(imafitog,"rpit%d.lst",getpid());

if((ffitog = fopen(imafitog,"w")) == NULL)
   {
    error_op_nfil(imafitog,errno,"");
    return(1);
   }
iceb_t_zagolov(gettext("Реестр проводок по виду операции"),dn,mn,gn,dk,mk,gk,ffitog);

if(VV->VVOD_SPISOK_return_data(2)[0] != '\0')
 fprintf(ffitog,"%s: %s\n",gettext("Подразделение"),VV->VVOD_SPISOK_return_data(2));
if(VV->VVOD_SPISOK_return_data(3)[0] != '\0')
 fprintf(ffitog,"%s: %s\n",gettext("Код контрагента"),VV->VVOD_SPISOK_return_data(3));
if(VV->VVOD_SPISOK_return_data(4)[0] != '\0')
 fprintf(ffitog,"%s: %s\n",gettext("Вид операции"),VV->VVOD_SPISOK_return_data(4));
if(VV->VVOD_SPISOK_return_data(5)[0] != '\0')
 fprintf(ffitog,"%s: %s\n",gettext("Код операции"),VV->VVOD_SPISOK_return_data(5));
if(VV->VVOD_SPISOK_return_data(6)[0] != '\0')
 fprintf(ffitog,"%s: %s\n",gettext("Счёт"),VV->VVOD_SPISOK_return_data(6));
if(VV->VVOD_SPISOK_return_data(7)[0] != '\0')
 fprintf(ffitog,"%s:%s\n",gettext("Набор операций"),VV->VVOD_SPISOK_return_data(7));


k1=0.;
fprintf(ffitog,"\n==================================================\n\
%s:\n",
gettext("Общий итог по всем операциям"));
for(i=0; i< mssio.kolih() ;i++)
 {
  fprintf(ffitog,"%-*s - %20s\n",iceb_tu_kolbait(10,mssio.ravno(i)),mssio.ravno(i),prnbr(mspsio.ravno(i)));
  k1+=mspsio.ravno(i);
 }
fprintf(ffitog,"%-*s - %20s\n",iceb_tu_kolbait(10,gettext("Итого")),gettext("Итого"),prnbr(k1));

podpis(ffitog);
fclose(ffitog);


klr+=sli;

printw("%s: %d\n",
gettext("Количество листов"),klr);

time(&tmmk);
k1=(tmmk-tmmn)/60.;
k1=modf(k1,&s1);

printw("\n%s %.f%s %.f%s\n",
gettext("Время расчёта"),
s1,
gettext("мин."),
k1*60.,
gettext("сек."));


OSTANOV();

sprintf(strsql,"%s %s",imaf,imafitog);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Реестр проводок по видам операций."));
oth->spis_imaf.plus(imafitog);
oth->spis_naim.plus(gettext("Общие итоги"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),1);


return(0);
}


/*******/
/*Шапка*/
/*******/
void uosrp1_sapka(int skl1,const char *naiskl,
short dn,short mn,short gn,
short dk,short mk,short gk,
int *kollist,int *kolstrok,FILE *ff)
{
*kolstrok+=4;
*kollist+=1;

fprintf(ff,"\
%s N%-2d %-30s %s %02d.%02d.%4d%s %s %02d.%02d.%4d%s                               %s%d\n",
gettext("Подразделение"),
skl1,naiskl,
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."),
gettext("Лист N"),
*kollist);

fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
	        Контрагент                                  |Дата док. |Дата пров.|N документа|  Счета   |       Сумма        |Коментарий\n"));
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------------\n");

}

/*******/
/*Счётчик строк*/
/*******/
void uosrp1_shetchik(short dn,short mn,short gn,short dk,short mk,
short gk,int *sl,int *kst,int skl1,const char *naiskl,FILE *ff)
{
*kst+=1;

if(*kst <=  kol_strok_na_liste)
 return;

*kst=1;
uosrp1_sapka(skl1,naiskl,dn,mn,gn,dk,mk,gk,sl,kst,ff);


}

/*******************************/
/*Распечатка итога по документа*/
/*******************************/
void            rasid(double itd,short dn,short mn,short gn,short dk,
short mk,short gk,int skl1,const char *naiskl,int *sli,int *kst,FILE *ff)
{
 fprintf(ff,"%*s %20s\n",iceb_tu_kolbait(106,gettext("Итого по документа:")),gettext("Итого по документа:"),prnbr(itd));
 uosrp1_shetchik(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
}

/*******************************/
/*Распечатка итога по организации*/
/*******************************/
void rasio(const char *orgn,double itd,short dn,short mn,short gn,
short dk,short mk,short gk,int skl1,
const char *naikont, //Наименование контрагента
const char *naiskl,  //Наименование подразделения
int *sli,int *kst,
class SPISOK *mso,
class masiv_din_double *mspo,
FILE *ff)
{
short           i;
double          bb;

rasid(itd,dn,mn,gn,dk,mk,gk,skl1,naiskl,sli,kst,ff);

/*Распечатываем результаты по организации*/
bb=0.;
if(mso->kolih() > 0)
 {
  uosrp1_shetchik(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%s \"%s\":\n",gettext("Итого по контрагенту"),naikont);
 }
/*printw("kso- %d\n",kso);*/

for(i=0; i < mso->kolih() ;i++)
 {
  uosrp1_shetchik(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%-*s - %20s\n",iceb_tu_kolbait(10,mso->ravno(i)),mso->ravno(i),prnbr(mspo->ravno(i)));
  bb+=mspo->ravno(i);
 }
if(mso->kolih() > 0)
 {
  fprintf(ff,"%-*s - %20s\n",iceb_tu_kolbait(10,gettext("Итого")),gettext("Итого"),prnbr(bb));
  kst++;
  fprintf(ff,"\
. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .\n");
  uosrp1_shetchik(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
 }
mso->free_class();
mspo->free_class();
}
/*******************************/
/*Распечатка итога по складу*/
/*******************************/
void rasis(const char *orgn,double itd,short dn,short mn,short gn,
short dk,short mk,short gk,int skl1,const char *naikont,const char *naiskl,
int *sli,int *kst,
class SPISOK *mso,
class masiv_din_double *mspo,
class SPISOK *mss,
class masiv_din_double *msps,
FILE *ff)
{
short           i;
double          bb;

rasio(orgn,itd,dn,mn,gn,dk,mk,gk,skl1,naikont,naiskl,sli,kst,mso,mspo,ff);

/*Распечатываем результаты по складу*/
uosrp1_shetchik(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
fprintf(ff,"%s \"%s\":\n",gettext("Итого по подразделению"),naiskl);
bb=0.;
for(i=0; i< msps->kolih() ;i++)
 {
  uosrp1_shetchik(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%-*s - %20s\n",iceb_tu_kolbait(10,mss->ravno(i)),mss->ravno(i),prnbr(msps->ravno(i)));
  bb+=msps->ravno(i);
 }
fprintf(ff,"%-*s - %20s\n",iceb_tu_kolbait(10,gettext("Итого")),gettext("Итого"),prnbr(bb));

fprintf(ff,"\
...............................................................................................................................\n");
uosrp1_shetchik(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
mss->free_class();
msps->free_class();
}

/*************************/
/*Распечатка общего итога*/
/*************************/
void            rasoi(class SPISOK *mssi,class masiv_din_double *mspsi,FILE *ff)
{
double          k1;
short           i;

k1=0.;
fprintf(ff,"\n%s:\n",gettext("Общий итог"));
for(i=0; i< mssi->kolih() ;i++)
 {
  fprintf(ff,"%-*s - %20s\n",iceb_tu_kolbait(10,mssi->ravno(i)),mssi->ravno(i),prnbr(mspsi->ravno(i)));
  k1+=mspsi->ravno(i);
 }
fprintf(ff,"%-*s - %20s\n",iceb_tu_kolbait(10,gettext("Итого")),gettext("Итого"),prnbr(k1));
mssi->free_class();
mspsi->free_class();
}
