/* $Id: rppvo1.c,v 5.54 2013/09/26 09:43:40 sasa Exp $ */
/*19.01.2017    19.10.1994      Белых А.И.      rppvo1.c
Материальный учет реестр проводок по видам операций
Сортировка проводок и выдача распечатки
Возвращаем 0 - если нет проводок
	   1 - если есть
*/
#include        <errno.h>
#include        "buhg.h"

void rasio(const char *orgn,double itd,short dn,short mn,short gn,short dk,short mk,short gk,int skl1,const char *naiskl,const char *naik,class SPISOK *mso,class masiv_din_double *mspo,int *sli, int *kst,FILE *ff);
void gsapr(short dn,short mn,short gn,short dk,short mk,short gk,int *sl,int *kst,int skl1,const char *naiskl,FILE *ff);
void rasid(double itd,short dn,short mn,short gn,short dk,short mk,short gk,int skl1,const char *naiskl,int *sli,int *kst,FILE *ff);
void rasis(const char *orgn,double itd,short dn,short mn,short gn,short dk,short mk,short gk,int skl1,const char *naiskl,const char *naik,class SPISOK *mso,class masiv_din_double *mspo,class SPISOK *mss,class masiv_din_double *msps,int *sli,int *kst,FILE *ff);
void            rasoi(SPISOK *mssi,masiv_din_double *mspsi,short dn,short mn,short gn,short dk,short mk,short gk,int *sli,int *kst,int skl1,const char *naiskl,FILE *ff);
void  rppvo1_sapit(short dn,short mn,short gn,short dk,short mk,short gk,int *sli,int *kst,int skl1,const char *naiskl,FILE *ff);
void rppvo1_s(int skl1,const char *naiskl,short dn,short mn,short gn,short dk,short mk,short gk,int *sl,int *kst,FILE *ff);


extern int      ddd,mmm,ggg;
extern int kol_strok_na_liste;

int rppvo1(VVOD *VV,class spis_oth *oth)
{
int           kst,sli;
SPISOK           mso; /*Массив счетов по организации*/
masiv_din_double mspo; /*Массив сумм проводок по организации*/
SPISOK           mss; /*Массив счетов по складу*/
masiv_din_double msps; /*Массив сумм проводок по складу*/
SPISOK           mssi; /*Массив счетов по всем складам*/
masiv_din_double mspsi; /*Массив сумм проводок по всем складам по операции*/
SPISOK           mssio; /*Массив счетов по всем складам и всем операциям*/
masiv_din_double mspsio; /*Массив сумм проводок по всем складам по всем операциям*/
FILE            *ff;
class iceb_tu_str bros("");
struct  tm      *bf;
time_t          tmm;
time_t          tmmn;
short           kom;
class iceb_tu_str kome(""); /*Коментарий*/
short           dp=0,mp=0,gp=0;
short           dd,md,gd;
class iceb_tu_str orgn("");
char            imaf[64];
class iceb_tu_str shett("");
class iceb_tu_str shett1("");
double          itd; /*Итого по счёту*/
int             i;
double          k1;
short           tipzz;
int		tz; /*1-Приход 2-расход*/
short           klr;
short           dn,mn,gn;
short           dk,mk,gk;
double		deb;
long		kolstr,kolstr1;
SQL_str         row,row1;
char		strsql[1024];
class iceb_tu_str kop("");
int		skl=0,skl1=0;
class iceb_tu_str naik("");
class iceb_tu_str naimpr("");
class iceb_tu_str naiskl("");
class iceb_tu_str nomdok("");
class iceb_tu_str nomdokv("");
short		tp;
class iceb_tu_str kprr(""),kprr1("");
class iceb_tu_str kpos("");
class iceb_tu_str kor("");		
class iceb_tu_str nomnalnak("");
char imafitog[64];
FILE *ffitog;

time(&tmm);
bf=localtime(&tmm);

if(rsdatp(&dn,&mn,&gn,VV->VVOD_SPISOK_return_data(0),&dk,&mk,&gk,VV->VVOD_SPISOK_return_data(1)) != 0)
  return(1);

time(&tmmn);
printw("%s %d.%d.%d => %d.%d.%d\n",gettext("Даты поиска:"),dn,mn,gn,dk,mk,gk);
GDITE();


tz=0;
if(VV->VVOD_SPISOK_return_data(4)[0] == '+')
  tz=1;
if(VV->VVOD_SPISOK_return_data(4)[0] == '-')
  tz=2;

  
sprintf(strsql,"select datp,sh,shk,nomd,pod,deb,datd,komen,kodkon,oper,tz from Prov where val=0 and \
datp >= '%04d-%02d-%02d' and \
datp <= '%04d-%02d-%02d' and kto='%s' and deb != 0.",
gn,mn,dn,gk,mk,dk,ICEB_MP_MATU);
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

printw(gettext("Сортируем проводки\n"));
/*Сортировка через временную таблицу делается так как сортировать нужно по коду контрагента
а он может отсутствовать в проводке*/

class iceb_t_tmptab tabtmp;
const char *imatmptab={"rppvow"};

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
tz int not null,\
nomnalnak char(32) not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  
kolstr1=0;
SQLCURSOR cur1;
class iceb_tu_str koment("");
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);  

  /*Смотрим только дебеты*/
//  if( fabs(atof(row[10])) > 0.009)
//   continue;

  /*Склад*/
  if(proverka(VV->VVOD_SPISOK_return_data(2),row[4],0,0) != 0)
    continue;
   
  /*Читаем документ*/
  sprintf(strsql,"select * from Dokummat where datd='%s' and \
sklad=%s and nomd='%s'",row[6],row[4],row[3]);
/*  printw("strsql=%s\n",strsql);*/
  if(readkey(strsql,&row1,&cur1) != 1)
   {
    beep();  
    printw(gettext("Не найден документ N %s Склад %s Дата=%s\n"),
    row[3],row[4],row[6]);
    printw("%s %s %s %s\n",gettext("Проводка"),
    row[0],row[1],row[2]);
    OSTANOV();
    continue;
   }

  nomnalnak.new_plus(row1[5]);
  kprr.new_plus(row1[6]);    
  
  tp=atoi(row1[0]);
  kor.new_plus(row1[3]);    
  kop.new_plus(row1[6]);    
  
  if(tz != 0 && tz != tp)
   continue;

  /*Организация*/
  if(proverka(VV->VVOD_SPISOK_return_data(3),kor.ravno(),0,0) != 0)
    continue;
   
  /*Операция*/
  if(proverka(VV->VVOD_SPISOK_return_data(5),kop.ravno(),0,0) != 0)
    continue;

  /*Счёт*/
  if(proverka(VV->VVOD_SPISOK_return_data(6),row[1],0,0) != 0)
    if(proverka(VV->VVOD_SPISOK_return_data(6),row[2],0,0) != 0)
      continue;
  /*Группа контрагента*/
  if(VV->data_ravno(8)[0] != '\0')
   {
    if(SRAV(kor.ravno(),"00-",1) == 0)
      sprintf(strsql,"select grup from Kontragent where kodkon='00'");
    else
      sprintf(strsql,"select grup from Kontragent where kodkon='%s'",kor.ravno());
    if(readkey(strsql,&row1,&cur1) == 1)
     {
      if(proverka(VV->data_ravno(8),row1[0],0,0) != 0)
       continue;
     }
   }
  nomdok.new_plus(row[3]);
  nomdok.plus(" ",row1[11]);
  koment.new_plus(row[7]);
  sprintf(strsql,"insert into %s values ('%s','%s','%s','%s',%s,%s,'%s','%s','%s','%s',%s,'%s')",
  imatmptab,
  row[0],row[1],row[2],nomdok.ravno_filtr(),row[4],row[5],row[6],koment.ravno_filtr(),kor.ravno(),kprr.ravno(),row[10],nomnalnak.ravno_filtr());

  sql_zapis(strsql,1,0);    

 }
sprintf(strsql,"select * from %s order by tz asc,oper asc,pod asc,kodkon asc,nomd asc",imatmptab);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
sprintf(imafitog,"itog%d.lst",getpid());
if((ffitog = fopen(imafitog,"w")) == NULL)
 {
  error_op_nfil(imafitog,errno,"");
  return(1);
 }

iceb_t_zagolov(gettext("Реестр проводок по видам операций."),dn,mn,gn,dk,mk,gk,ffitog);


sprintf(imaf,"rp%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

kst=0;


if(VV->VVOD_SPISOK_return_data(2)[0] != '\0')
 {
  kst++;
  fprintf(ff,gettext("Склад: %s\n"),VV->VVOD_SPISOK_return_data(2));
  fprintf(ffitog,gettext("Склад: %s\n"),VV->VVOD_SPISOK_return_data(2));
 }

if(VV->VVOD_SPISOK_return_data(3)[0] != '\0')
 {
  kst++;
  fprintf(ff,"%s: %s\n",gettext("Контрагент"),VV->VVOD_SPISOK_return_data(3));
  fprintf(ffitog,"%s: %s\n",gettext("Контрагент"),VV->VVOD_SPISOK_return_data(3));
 }
if(VV->VVOD_SPISOK_return_data(4)[0] != '\0')
 {
  kst++;
  fprintf(ff,"%s: %s\n",gettext("Вид операции"),VV->VVOD_SPISOK_return_data(4));
  fprintf(ffitog,"%s: %s\n",gettext("Вид операции"),VV->VVOD_SPISOK_return_data(4));
 }
if(VV->VVOD_SPISOK_return_data(5)[0] != '\0')
 {
  kst++;
  fprintf(ff,"%s: %s\n",gettext("Код операции"),VV->VVOD_SPISOK_return_data(5));
  fprintf(ffitog,"%s: %s\n",gettext("Код операции"),VV->VVOD_SPISOK_return_data(5));
 }
if(VV->VVOD_SPISOK_return_data(6)[0] != '\0')
 {
  kst++;
  fprintf(ff,"%s: %s\n",gettext("Счёт"),VV->VVOD_SPISOK_return_data(6));
  fprintf(ffitog,"%s: %s\n",gettext("Счёт"),VV->VVOD_SPISOK_return_data(6));
 }
if(VV->data_ravno(8)[0] != '\0')
 {
  kst++;
  fprintf(ff,"%s: %s\n",gettext("Код группы контрагента"),VV->data_ravno(8));
  fprintf(ffitog,"%s: %s\n",gettext("Код группы контрагента"),VV->data_ravno(8));
 }

skl=0;
nomdok.new_plus("");

tipzz=kom=0;
kolstr1=klr=sli=0;
itd=0.;

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  shett.new_plus(row[1]);
  shett.plus(",",row[2]);
  nomdok.new_plus(row[3]);
  skl1=atoi(row[4]);
  deb=atof(row[5]);
  rsdat(&dp,&mp,&gp,row[6],2);
  kome.new_plus(row[7]);
  
  rsdat(&dd,&md,&gd,row[6],2);
  
  orgn.new_plus(row[8]);

  kprr.new_plus(row[9]);
  tz=atoi(row[10]);
  nomnalnak.new_plus(row[11]);
  
  if(SRAV(kprr.ravno(),kprr1.ravno(),0) != 0 || tipzz != tz)
   {
    if(kprr1.ravno()[0] != '\0' )
     {
      rasis(orgn.ravno(),itd,dn,mn,gn,dk,mk,gk,skl,naiskl.ravno(),naik.ravno(),&mso,&mspo,&mss,&msps,&sli,&kst,ff);
      itd=0.;
      rasoi(&mssi,&mspsi,dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl.ravno(),ff);

      fprintf(ff,"\f");
      kst=0;
     }
    skl=0;
    nomdokv.new_plus("");
    kpos.new_plus("");

    /*Определяем наименоввание операции*/
    if(tz == 1)
     sprintf(strsql,"select naik from Prihod where kod='%s'",
     kprr.ravno());
    if(tz == 2)
     sprintf(strsql,"select naik from Rashod where kod='%s'",
     kprr.ravno());
    
    
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
       beep();
       printw("Не нашли операцию %s\n",kprr.ravno());
       OSTANOV();
      naimpr.new_plus("");
     }
    else
      naimpr.new_plus(row1[0]);
    
    kprr1.new_plus(kprr.ravno());
    tipzz=tz;
    /*move(5,0); */
    printw("%d/%s %s\n",tz,kprr.ravno(),naimpr.ravno());
    if(tz == 1)
     bros.new_plus(gettext("(приход)"));
    if(tz == 2)
     bros.new_plus(gettext("(расход)"));

    /*Определяем наименование склада*/
     sprintf(strsql,"select naik from Sklad where kod=%d",
     skl1);
    
    
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
       beep();
       printw("Не нашли склад %d\n",skl1);
       OSTANOV();
      naiskl.new_plus("");
     }
    else
      naiskl.new_plus(row1[0]);

    fprintf(ff,"%s\n\
%s %s - \"%s\" %s %s %d.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d%s  %s: %d:%d\n",
iceb_t_get_pnk("00",0),gettext("Реестр по виду операции"),
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
    rppvo1_s(skl1,naiskl.ravno(),dn,mn,gn,dk,mk,gk,&sli,&kst,ff);

   }

  if(skl != skl1)
   {
    if(skl != 0)
     {
      rasis(orgn.ravno(),itd,dn,mn,gn,dk,mk,gk,skl,naiskl.ravno(),naik.ravno(),&mso,&mspo,&mss,&msps,&sli,&kst,ff);
      itd=0.;
      kst=0;
      fprintf(ff,"\f");
     }
    nomdokv.new_plus("");
    kpos.new_plus("");
    kom=1;

    /*Определяем наименование склада*/
    sprintf(strsql,"select naik from Sklad where kod=%d",skl1);
    
    
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
       beep();
       printw(gettext("Не нашли склад %d в списке складов !"),skl1);
       printw("\n");
       OSTANOV();
      naiskl.new_plus("");
     }
    else
      naiskl.new_plus(row1[0]);

    printw("%s: %d %s\n",gettext("Склад"),skl1,naiskl.ravno());

    if(skl != 0)
      rppvo1_s(skl1,naiskl.ravno(),dn,mn,gn,dk,mk,gk,&sli,&kst,ff);

    skl=skl1;
   }

  if(SRAV(kpos.ravno(),orgn.ravno(),0) != 0)
   {
    if(kpos.ravno()[0] != '\0')
     {
      rasio(orgn.ravno(),itd,dn,mn,gn,dk,mk,gk,skl1,naiskl.ravno(),naik.ravno(),&mso,&mspo,&sli,&kst,ff);
      itd=0.;
     }
    nomdokv.new_plus("");
    kom=1;
    /*Читаем наименование организации*/
    kor.new_plus("");
    if(orgn.ravno_atoi() == 0 && polen(orgn.ravno(),&bros,2,'-') == 0 && orgn.ravno()[0] == '0')
     { 
      kor.new_plus(bros.ravno());
      sprintf(strsql,"select naik from Sklad where kod=%s",kor.ravno());
     }
    else
     {
      kor.new_plus(orgn.ravno());
      sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
      kor.ravno());
     }

    
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      beep();
      printw("%s %s\n",
      gettext("Не нашли код контрагента"),kor.ravno());
      OSTANOV(); 
      naik.new_plus("");
     }
    else
      naik.new_plus(row1[0]);

    printw(gettext("Контрагент %s\n"),orgn.ravno());
    kpos.new_plus(orgn.ravno());
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
   

  printw("%02d.%02d.%4d %02d.%02d.%4d %-*s %-*s %-*s %15.2f\n",
  dp,mp,gp,dd,md,gd,
  iceb_tu_kolbait(11,nomdok.ravno()),nomdok.ravno(),
  iceb_tu_kolbait(6,nomnalnak.ravno()),nomnalnak.ravno(),
  iceb_tu_kolbait(10,shett.ravno()),shett.ravno(),
  deb);

  gsapr(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl.ravno(),ff);
  if(kom == 1)
     fprintf(ff,"\
%-7s %-*.*s %02d.%02d.%4d %02d.%02d.%4d %-*s %-*s %-*s %20s %.13s\n",
     orgn.ravno(),
     iceb_tu_kolbait(40,naik.ravno()),
     iceb_tu_kolbait(40,naik.ravno()),
     naik.ravno(),
     dd,md,gd,dp,mp,gp,
     iceb_tu_kolbait(11,nomdok.ravno()),nomdok.ravno(),
     iceb_tu_kolbait(6,nomnalnak.ravno()),nomnalnak.ravno(),
     iceb_tu_kolbait(10,shett.ravno()),shett.ravno(),
     prnbr(deb),kome.ravno());

  if(kom == 0)
     fprintf(ff,"\
%48s %02d.%02d.%4d %02d.%02d.%4d %-*s %-*s %-*s %20s %.13s\n",
    " ",dd,md,gd,dp,mp,gp,
      iceb_tu_kolbait(11,nomdok.ravno()),nomdok.ravno(),
      iceb_tu_kolbait(6,nomnalnak.ravno()),nomnalnak.ravno(),
      iceb_tu_kolbait(10,shett.ravno()),shett.ravno(),
    prnbr(deb),kome.ravno());

  itd+=deb;
  kom=0;

 }

rasis(orgn.ravno(),itd,dn,mn,gn,dk,mk,gk,skl,naiskl.ravno(),naik.ravno(),&mso,&mspo,&mss,&msps,&sli,&kst,ff);
rasoi(&mssi,&mspsi,dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl.ravno(),ff);


class iceb_tu_str shdeb("");
class iceb_tu_str shkre("");

if(VV->VVOD_SPISOK_return_data(5)[0] == '\0' || ( VV->VVOD_SPISOK_return_data(5)[0] != '\0' && POLE(VV->VVOD_SPISOK_return_data(5),strsql,2,',') == 0))
 {
  k1=0.;
  fprintf(ff,"\n==================================================\n\
%s\n",gettext("Общий итог по всем операциям:"));
  rppvo1_sapit(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl.ravno(),ff);
  for(i=0; i< mssio.kolih() ;i++)
   {
    shett1.new_plus(mssio.ravno(i));
    polen(shett1.ravno(),&shdeb,1,',');
    polen(shett1.ravno(),&shkre,2,',');

    fprintf(ff,"%-*s %-*s %15s\n",
    iceb_tu_kolbait(6,shdeb.ravno()),shdeb.ravno(),
    iceb_tu_kolbait(6,shkre.ravno()),shkre.ravno(),
    prnbr(mspsio.ravno(i)));

    k1+=mspsio.ravno(i);
   }
  fprintf(ff,"%*s:%15s\n",iceb_tu_kolbait(13,gettext("Итого")),gettext("Итого"),prnbr(k1));
 }


printw("\n%s\n",gettext("Общий итог по всем операциям:"));

fprintf(ffitog,"\n%s\n",gettext("Общий итог по всем операциям:"));

k1=0.;
fprintf(ffitog,"\
------------------------------\n");
fprintf(ffitog,gettext("Дебет |Кредит|    Сумма      |\n"));
fprintf(ffitog,"\
------------------------------\n");

for(i=0; i< mssio.kolih() ;i++)
 {
  shett1.new_plus(mssio.ravno(i));
  printw("%-*s - %15s\n",
  iceb_tu_kolbait(15,shett1.ravno()),
  shett1.ravno(),
  prnbr(mspsio.ravno(i)));
  
  polen(shett1.ravno(),&shdeb,1,',');
  polen(shett1.ravno(),&shkre,2,',');
  fprintf(ffitog,"%-*s %-*s %15s\n",
    iceb_tu_kolbait(6,shdeb.ravno()),shdeb.ravno(),
    iceb_tu_kolbait(6,shkre.ravno()),shkre.ravno(),
  prnbr(mspsio.ravno(i)));
  k1+=mspsio.ravno(i);
 }
fprintf(ffitog,"%*s:%15s\n",iceb_tu_kolbait(13,gettext("Итого")),gettext("Итого"),prnbr(k1));

podpis(ffitog);
podpis(ff);

fclose(ffitog);
fclose(ff);

printw_vr(tmmn);

OSTANOV();

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
void gsapr(short dn,short mn,short gn,short dk,short mk,short gk,int *sl,int *kst,int skl1,const char *naiskl,FILE *ff)
{
*kst+=1;

if(*kst <=  kol_strok_na_liste)
 return;

fprintf(ff,"\f");
*kst=1;
rppvo1_s(skl1,naiskl,dn,mn,gn,dk,mk,gk,sl,kst,ff);
  

}

/*******************************/
/*Распечатка итога по документа*/
/*******************************/
void rasid(double itd,short dn,short mn,short gn,short dk,
short mk,short gk,int skl1,const char *naiskl,int *sli,int *kst,FILE *ff)
{
gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);

fprintf(ff,"%*s %20s\n",iceb_tu_kolbait(100,gettext("Итого по документу:")),gettext("Итого по документу:"),prnbr(itd));

}

/*******************************/
/*Распечатка итога по контрагенту*/
/*******************************/
void rasio(const char *orgn,double itd,short dn,short mn,short gn,
short dk,short mk,short gk,int skl1,const char *naiskl,const char *naik,
class SPISOK *mso,
class masiv_din_double *mspo,
int *sli, 
int *kst,
FILE *ff)
{
short           i;
class iceb_tu_str shett1("");
double          bb;
class iceb_tu_str shdeb("");
class iceb_tu_str shkre("");


rasid(itd,dn,mn,gn,dk,mk,gk,skl1,naiskl,sli,kst,ff);

/*Распечатываем результаты по организации*/
bb=0.;
if(mso->kolih() > 0)
 {
  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%s \"%s\":\n",gettext("Итого по контрагенту"),naik);
 }
/*printw("kso- %d\n",kso);*/

rppvo1_sapit(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);

for(i=0; i< mso->kolih() ;i++)
 {
  shett1.new_plus(mso->ravno(i));
  polen(shett1.ravno(),&shdeb,1,',');
  polen(shett1.ravno(),&shkre,2,',');

  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%-*s %-*s %15s\n",
  iceb_tu_kolbait(6,shdeb.ravno()),shdeb.ravno(),
  iceb_tu_kolbait(6,shkre.ravno()),shkre.ravno(),
  prnbr(mspo->ravno(i)));

  bb+=mspo->ravno(i);
 }
if(mso->kolih() > 0)
 {
  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%*s:%15s\n",iceb_tu_kolbait(13,gettext("Итого")),gettext("Итого"),prnbr(bb));

  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"\
. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .\n");
 }

mso->free_class();
mspo->free_class();
}
/*******************************/
/*Распечатка итога по складу*/
/*******************************/
void rasis(const char *orgn,double itd,short dn,short mn,short gn,
short dk,short mk,short gk,int skl1,const char *naiskl,const char *naik,
class SPISOK *mso,
class masiv_din_double *mspo,
class SPISOK *mss,
class masiv_din_double *msps,
int *sli,
int *kst,
FILE *ff)
{
short           i;
class iceb_tu_str shett1("");
double          bb;
class iceb_tu_str shdeb("");
class iceb_tu_str shkre("");

rasio(orgn,itd,dn,mn,gn,dk,mk,gk,skl1,naiskl,naik,mso,mspo,sli,kst,ff);

/*Распечатываем результаты по складу*/
gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
fprintf(ff,"%s \"%s\":\n",gettext("Итого по складу"),naiskl);

rppvo1_sapit(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);

bb=0.;
for(i=0; i< mss->kolih() ;i++)
 {
  shett1.new_plus(mss->ravno(i));

  polen(shett1.ravno(),&shdeb,1,',');
  polen(shett1.ravno(),&shkre,2,',');
  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%-*s %-*s %15s\n",
  iceb_tu_kolbait(6,shdeb.ravno()),shdeb.ravno(),
  iceb_tu_kolbait(6,shkre.ravno()),shkre.ravno(),
  prnbr(msps->ravno(i)));

  bb+=msps->ravno(i);
 }

gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
fprintf(ff,"%*s:%15s\n",iceb_tu_kolbait(13,gettext("Итого")),gettext("Итого"),prnbr(bb));

gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
fprintf(ff,"\
......................................................................................................................................\n");

mss->free_class();
msps->free_class();
}

/*************************/
/*Распечатка общего итога*/
/*************************/
void            rasoi(SPISOK *mssi,
masiv_din_double *mspsi,
short dn,short mn,short gn,short dk,short mk,short gk,
int *sli,int *kst,int skl1,const char *naiskl,
FILE *ff)
{
double          k1;
short           i;
class iceb_tu_str shett1("");
class iceb_tu_str shdeb("");
class iceb_tu_str shkre("");


k1=0.;
fprintf(ff,"\n%s:\n",gettext("Общий итог"));

rppvo1_sapit(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
for(i=0; i< mssi->kolih() ;i++)
 {
  shett1.new_plus(mssi->ravno(i));
  polen(shett1.ravno(),&shdeb,1,',');
  polen(shett1.ravno(),&shkre,2,',');
  fprintf(ff,"%-*s %-*s %15s\n",
  iceb_tu_kolbait(6,shdeb.ravno()),shdeb.ravno(),
  iceb_tu_kolbait(6,shkre.ravno()),shkre.ravno(),
  prnbr(mspsi->ravno(i)));
  k1+=mspsi->ravno(i);
 }
fprintf(ff,"%*s:%15s\n",iceb_tu_kolbait(13,gettext("Итого")),gettext("Итого"),prnbr(k1));


mspsi->free_class();
mssi->free_class();
}

/******************************/
/* Шапка итога по счетам      */
 /******************************/

void  rppvo1_sapit(short dn,short mn,short gn,short dk,short mk,short gk,
int *sli,int *kst,int skl1,const char *naiskl,FILE *ff)
{
gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
fprintf(ff,"\
------------------------------\n");

gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);

fprintf(ff,gettext("Дебет |Кредит|    Сумма      |\n"));


gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
fprintf(ff,"\
------------------------------\n");

}
/************************************/
/*Шапка*/
/************************************/
void rppvo1_s(int skl1,const char *naiskl,
short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,
int *kst,
FILE *ff)
{
*sl+=1;
*kst+=4;

fprintf(ff,"\
%s%-2d %-30s %s %02d.%02d.%4d%s %s %02d.%02d.%4d%s %*s%d\n",
gettext("Склад N"),
skl1,naiskl,
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."),
iceb_tu_kolbait(30,gettext("Лист N")),gettext("Лист N"),
*sl);

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext(
" 	 Контрагент                              |Дата док. |Дата пров.|N документа|Nнал.н|  Счета   |       Сумма        |Коментарий\n"));

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------\n");

}
