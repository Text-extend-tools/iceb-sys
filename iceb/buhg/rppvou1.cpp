/* $Id: rppvou1.c,v 5.33 2013/09/26 09:43:40 sasa Exp $ */
/*18.05.2017    19.10.1994      Белых А.И.      rppvou1.c
Учёт услуг
Сортировка проводок и выдача распечатки
Возвращаем 0 - если нет проводок
	   1 - если есть
*/
#include        <errno.h>
#include        "buhg.h"

void gsapr(short,short,short,short,short,short,int*,int*,int,const char*,FILE*);
void rasid(double,short,short,short,short,short,short,int,const char*,int*,int*,FILE*);
void rasio(const char*,double,short,short,short,short,short,short,int,const char*,const char*,class SPISOK *mso,class masiv_din_double *mspo,int*,int*,FILE*);
void rasis(const char*,double,short,short,short,short,short,short,int,const char*,const char*,class SPISOK *mso,class masiv_din_double *mspo,class SPISOK *mss,class masiv_din_double *msps,int*,int*,FILE*);
void rasoi(class SPISOK *mssi,class masiv_din_double *mspsi,FILE*);
void rppvou1_sap(short dn,short mn,short gn,short dk,short mk,short gk,int *sl,int *kst,int skl1,const char *naiskl,FILE *ff);
void rppvou1rp(const char *podr,const char *kontr,const char *prihr,const char *kodop,const char *shet,int *kst,FILE *ff);

extern int kol_strok_na_liste;

int rppvou1(short dn,short mn,short gn,//Дата начала
short dk,short mk,short gk, //Дата конца
const char *podr, //Код подразделения
const char *kontr, //Код контрагента
const char *kodop, //Код операции
const char *shet,  //Счёт 
const char *prihr,  //"+" приход "-" расход
class spis_oth *oth)
{
int           kst=0,sli=0;
FILE            *ff;
class iceb_tu_str bros("");
FILE            *ff1;
struct  tm      *bf;
time_t          tmmn;
short           kom;
class iceb_tu_str kome(""); /*Коментарий*/
short           dp,mp,gp;
short           dd,md,gd;
class iceb_tu_str orgn("");
char            imaf[64];
class iceb_tu_str shett("");
double          itd; /*Итого по счёту*/
int             i;
double          k1;
short           tipzz;
int		tz; /*1-Приход 2-расход*/
short           klr;
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
SQLCURSOR curr;
GDITE();
time(&tmmn);


tz=0;
if(prihr[0] == '+')
  tz=1;
if(prihr[0] == '-')
  tz=2;
  
sprintf(strsql,"select datp,sh,shk,nomd,pod,deb,datd,komen,kodkon,oper,tz from Prov \
where val=0 and \
datp >= '%04d-%02d-%02d' and \
datp <= '%04d-%02d-%02d' and kto='%s' and deb != 0.",
gn,mn,dn,gk,mk,dk,ICEB_MP_USLUGI);
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
const char *imatmptab={"rppvouw"};

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

printw("%s %d.%d.%d > %d.%d.%d\n",gettext("Даты поиска:"),dn,mn,gn,dk,mk,gk);
printw(gettext("Сортируем проводки.\n"));
GDITE();

kolstr1=0;
class iceb_tu_str koment("");
while(cur.read_cursor(&row) != 0)
 {
  kolstr1++;
  strzag(LINES-1,0,kolstr,kolstr1);  

  /*Подразделение*/
  if(proverka(podr,row[4],0,0) != 0)
    continue;

  /*Счёт*/
  if(proverka(shet,row[1],0,0) != 0)
    continue;
   
  /*Читаем документ*/
  sprintf(strsql,"select tp,datd,kontr,kodop,nomnn from Usldokum where datd='%s' and \
podr=%s and nomd='%s'",row[6],row[4],row[3]);
/*  printw("strsql=%s\n",strsql);*/
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    printw("Не найден документ N %s Подразделение %s Дата=%s\n",
    row[3],row[4],row[6]);
    printw("%s %s %s %s\n",gettext("Проводка"),
    row[1],row[2],row[3]);
    OSTANOV();
    continue;
   }
  tp=atoi(row1[0]);
  if(tz != 0 && tz != tp)
   continue;
  kor.new_plus(row1[2]);    
  kop.new_plus(row1[3]);    


  /*Организация*/
  if(proverka(kontr,kor.ravno(),0,0) != 0)
    continue;
   
  /*Операция*/
  if(proverka(kodop,kop.ravno(),0,0) != 0)
    continue;

  nomdok.new_plus(row[3]);
  nomdok.plus(" ",row1[4]);
  koment.new_plus(row[7]);
  sprintf(strsql,"insert into %s values ('%s','%s','%s','%s',%s,%s,'%s','%s','%s','%s',%s)",
  imatmptab,
  row[0],row[1],row[2],nomdok.ravno_filtr(),row[4],row[5],row[5],koment.ravno_filtr(),kor.ravno(),kop.ravno(),row[10]);

  sql_zapis(strsql,1,0);    

 }
sprintf(strsql,"select * from %s order by tz asc,oper asc,pod asc,kodkon asc,nomd asc",imatmptab);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
class SPISOK mssi;
class SPISOK mssio;
class masiv_din_double mspsi;
class masiv_din_double mspsio;

class SPISOK mso; //Массив счетов по организации
class masiv_din_double mspo;
class SPISOK mss; //Массив счетов по организации
class masiv_din_double msps;


sprintf(imaf,"uslrp%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

bf=localtime(&tmmn);

rppvou1rp(podr,kontr,prihr,kodop,shet,&kst,ff);



skl=0;
nomdok.new_plus("");

tipzz=kom=0;
kolstr1=klr=sli=0;
itd=0.;
int nomer=0;
while(cur.read_cursor(&row) != 0)
 {

  rsdat(&dp,&mp,&gp,row[0],2);

  shett.new_plus(row[1]);
  shett.plus(",",row[2]);
  nomdok.new_plus(row[3]);
  skl1=atoi(row[4]);
  deb=atof(row[5]);
  rsdat(&dd,&md,&gd,row[6],2);
  kome.new_plus(row[7]);

  orgn.new_plus(row[8]);

  kprr.new_plus(row[9]);
  tz=atoi(row[10]);
  
   
  if(SRAV(kprr.ravno(),kprr1.ravno(),0) != 0 || tipzz != tz)
   {
    if(kprr1.ravno()[0] != '\0' )
     {
      rasis(orgn.ravno(),itd,dn,mn,gn,dk,mk,gk,skl1,naiskl.ravno(),naik.ravno(),&mso,&mspo,&mss,&msps,&sli,&kst,ff);
      itd=0.;
      rasoi(&mssi,&mspsi,ff);

      fprintf(ff,"\f");
      kst=0;
     }
    skl=0;
    nomdokv.new_plus("");
    kpos.new_plus("");

    /*Определяем наименоввание операции*/
    if(tz == 1)
     sprintf(strsql,"select naik from Usloper1 where kod='%s'",kprr.ravno());
    if(tz == 2)
     sprintf(strsql,"select naik from Usloper2 where kod='%s'",kprr.ravno());
    
    
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
       beep();
       printw("%s %s!\n",gettext("Не нашли код операции"),kprr.ravno());
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

    /*Определяем наименование подразделения*/
     sprintf(strsql,"select naik from Uslpodr where kod=%d",
     skl1);
    
    
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
       beep();
       printw("%s %d\n",gettext("Не найден код подразделения"),skl1);
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
    rppvou1_sap(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl.ravno(),ff);

   }

  if(skl != skl1)
   {
    if(skl != 0)
     {
      rasis(orgn.ravno(),itd,dn,mn,gn,dk,mk,gk,skl1,naiskl.ravno(),naik.ravno(),&mso,&mspo,&mss,&msps,&sli,&kst,ff);
      itd=0.;
      klr+=sli;
      sli=0;
      kst=0;
      fprintf(ff,"\f");
     }
    nomdokv.new_plus("");
    kpos.new_plus("");
    kom=1;

    /*Определяем наименование подразделения*/
     sprintf(strsql,"select naik from Uslpodr where kod=%d",
     skl1);
    
    
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
       beep();
       printw("%s %d\n",gettext("Не найден код подразделения"),skl1);
       OSTANOV();
      naiskl.new_plus("");
     }
    else
      naiskl.new_plus(row1[0]);

    printw("%s: %d %s\n",gettext("Подразделение"),skl1,naiskl.ravno());
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
    if(ATOFM(orgn.ravno()) == 0. && polen(orgn.ravno(),&bros,1,'-') == 0 && orgn.ravno()[0] == '0')
      kor.new_plus(bros.ravno());
    else
      kor.new_plus(orgn.ravno());

    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kor.ravno());
    
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      beep();
      printw("%s %s\"\n",
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
  if((nomer=mso.find(shett.ravno())) < 0)
      mso.plus(shett.ravno());
  mspo.plus(deb,nomer);
  if((nomer=mss.find(shett.ravno())) < 0)
      mss.plus(shett.ravno());
  msps.plus(deb,nomer);

   /*Записываем в массив по подразделениям */
  if((nomer=mssi.find(shett.ravno())) < 0)
      mssi.plus(shett.ravno());
  mspsi.plus(deb,nomer);
  if((nomer=mssio.find(shett.ravno())) < 0)
      mssio.plus(shett.ravno());
  mspsio.plus(deb,nomer);

   gsapr(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl.ravno(),ff);

   printw("%02d.%02d.%4d %02d.%02d.%4d %-*s %-*s %15.2f\n",
   dp,mp,gp,dd,md,gd,
   iceb_tu_kolbait(11,nomdok.ravno()),nomdok.ravno(),
   iceb_tu_kolbait(11,shett.ravno()),shett.ravno(),
   deb);

   if(kom == 1)
     fprintf(ff,"\
%-7s %-*.*s %02d.%02d.%4d %02d.%02d.%4d %-*s %-*s %20s %s\n",
     orgn.ravno(),
     iceb_tu_kolbait(40,naik.ravno()),
     iceb_tu_kolbait(40,naik.ravno()),
     naik.ravno(),
     dd,md,gd,dp,mp,gp,
     iceb_tu_kolbait(11,nomdok.ravno()),nomdok.ravno(),
     iceb_tu_kolbait(11,shett.ravno()),shett.ravno(),
     prnbr(deb),kome.ravno());

   if(kom == 0)
     fprintf(ff,"\
%48s %02d.%02d.%4d %02d.%02d.%4d %-*s %-*s %20s %s\n",
    " ",dd,md,gd,dp,mp,gp,
     iceb_tu_kolbait(11,nomdok.ravno()),nomdok.ravno(),
     iceb_tu_kolbait(11,shett.ravno()),shett.ravno(),
    prnbr(deb),kome.ravno());

   itd+=deb;
   kom=0;

 }

rasis(orgn.ravno(),itd,dn,mn,gn,dk,mk,gk,skl1,naiskl.ravno(),naik.ravno(),&mso,&mspo,&mss,&msps,&sli,&kst,ff);
rasoi(&mssi,&mspsi,ff);



int kssio=mssio.kolih();
if(kodop[0] == '\0' || ( kodop[0] != '\0' && POLE(kodop,strsql,2,',') == 0))
 {
  k1=0.;
  fprintf(ff,"\n==================================================\n\
%s\n",gettext("Общий итог по всем операциям:"));
  for(i=0; i< kssio ;i++)
   {
    fprintf(ff,"%*s: %20s\n",
    iceb_tu_kolbait(15,mssio.ravno(i)),mssio.ravno(i),prnbr(mspsio.ravno(i)));
    k1+=mspsio.ravno(i);
   }
  fprintf(ff,"%*s: %20s\n",iceb_tu_kolbait(15,gettext("Итого")),gettext("Итого"),prnbr(k1));
 }

printw("\n%s\n",gettext("Общий итог по всем операциям:"));

char imaf_oi[50];
sprintf(imaf_oi,"rpoi%d.lst",getpid());

if((ff1 = fopen(imaf_oi,"w")) == NULL)
 {
  error_op_nfil(imaf_oi,errno,"");
  return(1);
 }
iceb_t_zagolov(gettext("Реестр по виду операции"),dn,mn,gn,dk,mk,gk,ff1);
rppvou1rp(podr,kontr,prihr,kodop,shet,&kst,ff1);

fprintf(ff1,"%s\n",gettext("Общий итог по всем операциям:"));
for(i=0; i< kssio ;i++)
 {
  
  printw("%*s: %20s\n",
  iceb_tu_kolbait(15,mssio.ravno(i)),mssio.ravno(i),prnbr(mspsio.ravno(i)));
  fprintf(ff1,"%*s: %20s\n",
  iceb_tu_kolbait(15,mssio.ravno(i)),mssio.ravno(i),prnbr(mspsio.ravno(i)));
 }
fprintf(ff,"%-15s - %20s\n",gettext("Итого"),prnbr(mspsio.suma()));
podpis(ff1);
fclose(ff1);



fclose(ff);

klr+=sli;
printw("%s - %d\n",gettext("Количество листов"),klr);
printw_vr(tmmn);

OSTANOV();
oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Реестр проводок по видам операций"));
oth->spis_imaf.plus(imaf_oi);
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
rppvou1_sap(dn,mn,gn,dk,mk,gk,sl,kst,skl1,naiskl,ff);


}

/*******************************/
/*Распечатка итога по документа*/
/*******************************/
void rasid(double itd,short dn,short mn,short gn,short dk,
short mk,short gk,int skl1,const char *naiskl,int *sli,int *kst,FILE *ff)
{
 gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
 fprintf(ff,"%*s %20s\n",iceb_tu_kolbait(94,gettext("Итого по документу:")),gettext("Итого по документу:"),prnbr(itd));
}

/*******************************/
/*Распечатка итога по организации*/
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

rasid(itd,dn,mn,gn,dk,mk,gk,skl1,naiskl,sli,kst,ff);

/*Распечатываем результаты по организации*/
int kso=mso->kolih();
bb=0.;
if(kso > 0)
 {
  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%s \"%s\":\n",gettext("Итого по контрагенту"),naik);
 }
/*printw("kso- %d\n",kso);*/

for(i=0; i< kso ;i++)
 {
  shett1.new_plus(mso->ravno(i));

  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%*s: %20s\n",
  iceb_tu_kolbait(15,shett1.ravno()),shett1.ravno(),prnbr(mspo->ravno(i)));

  bb+=mspo->ravno(i);
 }
if(kso > 0)
 {
  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%*s: %20s\n",iceb_tu_kolbait(15,gettext("Итого")),gettext("Итого"),prnbr(bb));

  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"\
. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .\n");
  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
 }
mspo->free_class();
mso->free_class();
}
/*******************************/
/*Распечатка итога по подразделению*/
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
double          bb;

rasio(orgn,itd,dn,mn,gn,dk,mk,gk,skl1,naiskl,naik,mso,mspo,sli,kst,ff);

/*Распечатываем результаты по подразделению*/
gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
fprintf(ff,"%s \"%s\":\n",gettext("Итого по подразделению"),naiskl);
bb=0.;
int kss=mss->kolih();
for(i=0; i< kss ;i++)
 {
  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%*s: %20s\n",iceb_tu_kolbait(15,mss->ravno(i)),mss->ravno(i),prnbr(msps->ravno(i)));
  bb+=msps->ravno(i);
 }
gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
fprintf(ff,"%*s: %20s\n",iceb_tu_kolbait(15,gettext("Итого")),gettext("Итого"),prnbr(bb));

gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
fprintf(ff,"\
...............................................................................................................................\n");

msps->free_class();
mss->free_class();

}

/*************************/
/*Распечатка общего итога*/
/*************************/
void rasoi(class SPISOK *mssi,class masiv_din_double *mspsi,FILE *ff)
{
double          k1;
short           i;

k1=0.;
fprintf(ff,"\n%s:\n",gettext("Общий итог"));
int kssi=mssi->kolih();
for(i=0; i< kssi ;i++)
 {
  fprintf(ff,"%*s: %20s\n",iceb_tu_kolbait(15,mssi->ravno(i)),mssi->ravno(i),prnbr(mspsi->ravno(i)));
  k1+=mspsi->ravno(i);
 }
fprintf(ff,"%*s: %20s\n",iceb_tu_kolbait(15,gettext("Итого")),gettext("Итого"),prnbr(k1));
mspsi->free_class();
mssi->free_class();

}
/***********************/
/*Шапка*/
/***************/
void rppvou1_sap(short dn,short mn,short gn,short dk,short mk,
short gk,int *sl,int *kst,int skl1,const char *naiskl,FILE *ff)
{

*sl+=1;
*kst+=4;

fprintf(ff,"\
%s N%-2d %-*s %s %02d.%02d.%4d%s %s %02d.%02d.%4d%s %*s%d\n",
gettext("Подразделение"),
skl1,
iceb_tu_kolbait(30,naiskl),
naiskl,
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."),
iceb_tu_kolbait(30,gettext("Лист N")),gettext("Лист N"),
*sl);

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext(" 	 Контрагент                              |Дата док. |Дата пров.|N документа|  Счета   |       Сумма        |Коментарий\n"));

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------\n");

}
/***********************************/
/*Реквизиты поиска*/
/*************************/
void rppvou1rp(const char *podr,const char *kontr,const char *prihr,const char *kodop,const char *shet,int *kst,FILE *ff)
{
if(podr[0] != '\0')
 {
  *kst+=1;
  fprintf(ff,"%s: %s\n",gettext("Код подразделения"),podr);
 }
if(kontr[0] != '\0')
 {
  *kst+=1;
  fprintf(ff,"%s: %s\n",gettext("Контрагент"),kontr);
 }
if(prihr[0] != '\0')
 {
  *kst+=1;
  fprintf(ff,"%s: %s\n",gettext("Вид операции"),prihr);
 }
if(kodop[0] != '\0')
 {
  *kst+=1;
  fprintf(ff,"%s: %s\n",gettext("Код операции"),kodop);
 }
if(shet[0] != '\0')
 {
  *kst+=1;
  fprintf(ff,"%s: %s\n",gettext("Счёт"),shet);
 }
}
