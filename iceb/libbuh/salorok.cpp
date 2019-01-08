/* $Id: salorok.c,v 5.76 2013/08/13 05:50:23 sasa Exp $ */
/*13.01.2018    27.08.1997      Белых А.И.      salorok.c
Получение сальдо по организации по всем счетам
*/
#include        <errno.h>
#include        "buhl.h"

void saloro1(const char*,short,short,short,short,short,short,const char*,const char*,FILE*,FILE*,FILE*,double*,double*,short,class SPISOK*,class masiv_din_double*,const char*,const char*,const char *imaftmptab,const char *sheta_kor,FILE*,FILE*,FILE*);
void obit(double,double,short,int,FILE*);
void obit_ac(double db,double kr,FILE *ff1);
void potf(const char*,const char*,short,short,short,short,short,short,char*,char*,class SPISOK*,class masiv_din_double*,const char *imatmptab);
void vsali(double,double,short,short,FILE*,class masiv_din_double*);
const char *salorok_gvd(const char *podsistema,short dd,short md,short gd,int podr,int tz,const char *nom_dok);


int salorok(const char *kodo,const char *nai,class spis_oth *oth)
{
static class iceb_tu_str dat1(""),dat2("");
static class iceb_tu_str sheta("");
static class iceb_tu_str sheta_kor("");
short           par;
int             ks=0;
int kom1=0;
short           dn,mn,gn;
short           dk,mk,gk;
int             i;
class iceb_tu_str sh("");
FILE            *ff1,*ff4,*ffact;
char            imaf[64],imaf1[64],imaf2[64],imaf4[64],imafacts[64];
time_t          tmm;
struct  tm      *bf;
double          db,kr;
double          db1,kr1;
char		strsql[1024];
SQL_str         row,row1;
long		kolstr;
short	dt,mt,gt;

clear();
if(dat1.ravno()[0] == '\0')
 {
  poltekdat(&dt,&mt,&gt);
  sprintf(strsql,"01.01.%d",gt);
  dat1.new_plus(strsql);
  sprintf(strsql,"%02d.%02d.%d",dt,mt,gt);
  dat2.new_plus(strsql);
 }

class VVOD MENU(3);
class VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт сальдо по всем счетам с развернутым сальдо"));

sprintf(strsql,"%s %s",kodo,nai);
VV.VVOD_SPISOK_add_ZAG(strsql);
VV.VVOD_SPISOK_add_MD(gettext("Дата начала...(д.м.г)...."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца....(д.м.г)...."));//1
VV.VVOD_SPISOK_add_MD(gettext("Счёт...............(,,).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Счёт кореспондент..(,,).."));//3

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(sheta.ravno(),128);
VV.VVOD_SPISOK_add_data(sheta_kor.ravno(),128);

naz:;

helstr(LINES-1,0,"F2/+",gettext("расчитать"),
"F3",gettext("счета"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

par=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
sheta.new_plus(VV.data_ravno(2));
sheta_kor.new_plus(VV.data_ravno(3));

switch(par)
 {
  case FK10:
  case ESC:
    return(1);
  case FK2:
  case PLU:
    break;

  case FK3:
    MENU.VVOD_delete();

    MENU.VVOD_SPISOK_add_MD(gettext("Выбор счёта"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выбор счёта корреспондента"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 2 :
      case -1:
        goto naz;
    
      case 0 :
        vibrek("Plansh",&sheta);
        VV.data_plus(2,sheta.ravno());
        break;

      case 1 :
        vibrek("Plansh",&sheta_kor);
        VV.data_plus(3,sheta_kor.ravno());
                 
        break;
     }
    goto naz;
  case FK4:
    VV.VVOD_clear_data();  
    goto naz;

  default:
    goto naz;
 }

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
  goto naz;

class iceb_tu_str kodgr("");
class iceb_tu_str naimgr("");
SQLCURSOR cur,cur1;

sprintf(strsql,"select grup from Kontragent where kodkon='%s'",kodo);
if(readkey(strsql,&row,&cur) == 1)
 kodgr.new_plus(row[0]);

if(kodgr.ravno()[0] != '\0')
 {
  sprintf(strsql,"select naik from Gkont where kod=%s",kodgr.ravno());
  if(readkey(strsql,&row,&cur) == 1)
   naimgr.new_plus(row[0]);
 }
 
par=0;
VVOD DANET(1);
DANET.VVOD_SPISOK_add_MD(gettext("Встречные проводки учесть ?"));
if(danet(&DANET,1,stdscr) == 2)
  par=1;

clear();
GDITE();

time(&tmm);
bf=localtime(&tmm);


short	startgodb=0; /*Стартовый год*/

if(iceb_t_poldan("Стартовый год",strsql,"nastrb.alx") == 0)
 startgodb=atoi(strsql);

short godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;

class SPISOK sheta_srs; /*Список счетов с развёрнутым сальдо*/
/*Смотрим по каким счетам введено сальдо*/
sprintf(strsql,"select ns from Saldo where kkk=1 and gs=%d and kodkon='%s' order by ns asc",
godn,kodo);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

while(cur.read_cursor(&row) != 0)
 {
  sheta_srs.plus(row[0]);
 }

/*Смотрим по каким счетам были сделаны проводки*/
sprintf(strsql,"select distinct sh from Prov where datp >= '%04d-1-1' and \
datp <= '%04d-%02d-%02d' and kodkon='%s' order by sh asc",godn,gk,mk,dk,kodo);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }


while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"select saldo from Plansh where ns='%s'",row[0]);
  if(readkey(strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,gettext("Не найден счёт %s в плане счетов!"),row[0]);
    iceb_t_soob(strsql);
    continue;
   }
  if(atoi(row1[0]) != 3)
   continue;  
  if(sheta_srs.find(row[0]) < 0)
    sheta_srs.plus(row[0]);
 }


ks=sheta_srs.kolih();
if(ks == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной проводки!"));
  return(1);
 }

printw("%s\n",gettext("Расчёт сальдо по всем счетам с развернутым сальдо"));
printw("%s %s.\n",kodo,nai);
printw("\n%s %d.%d.%d %s %d.%d.%d\n",
gettext("Период с"),
dn,mn,gn,
gettext("до"),
dk,mk,gk);
printw(gettext("Количество счетов с развернутым сальдо:%d\n"),ks);
for(int ii=0; ii < ks ; ii++)
 printw(" %s",sheta_srs.ravno(ii));
printw("\n");


class masiv_din_double mdo;
mdo.make_class(ks*2);

char imaf_spr[64];
sprintf(imaf_spr,"sprk%d.lst",getpid());
FILE *ff_spr;
if((ff_spr = fopen(imaf_spr,"w")) == NULL)
 {
  error_op_nfil(imaf_spr,errno,"");
  return(1);
 }

sprintf(imaf,"sl%d.lst",getpid());
if((ff1 = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

class iceb_t_tmptab tabtmp;
const char *imatmptab={"salorok"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
sh char(24) not null,\
nomd char(24) not null,\
datp DATE NOT NULL,\
shk char(24) not null,\
kto char(8) not null,\
deb double(14,2) not null,\
kre double(14,2) not null,\
koment char(112) not null,\
pod INT NOT NULL DEFAULT 0,\
tz TINYINT NOT NULL DEFAULT 0,\
INDEX(sh,datp)) ENGINE = MYISAM",imatmptab);



if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  

sprintf(imaf1,"sla%d.lst",getpid());
sprintf(imaf2,"slb%d.lst",getpid());
sprintf(imaf4,"slbc%d.lst",getpid());
if((ff4 = fopen(imaf4,"w")) == NULL)
 {
  error_op_nfil(imaf4,errno,"");
  return(1);
 }

sprintf(imafacts,"acts%d.lst",getpid());
if((ffact = fopen(imafacts,"w")) == NULL)
 {
  error_op_nfil(imafacts,errno,"");
  return(1);
 }

char imaf_acts[64];
sprintf(imaf_acts,"asv%d.lst",getpid());
FILE *ff_act;
if((ff_act = fopen(imaf_acts,"w")) == NULL)
 {
  error_op_nfil(imaf_acts,errno,"");
  return(1);
 }

char imaf_acts_bi[64];
sprintf(imaf_acts_bi,"asvbi%d.lst",getpid());
FILE *ff_act_bi;
if((ff_act_bi = fopen(imaf_acts_bi,"w")) == NULL)
 {
  error_op_nfil(imaf_acts_bi,errno,"");
  return(1);
 }

zagacts(dn,mn,gn,dk,mk,gk,bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,kodo,nai,ffact);
zagacts(dn,mn,gn,dk,mk,gk,bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,kodo,nai,ff_act);
zagacts(dn,mn,gn,dk,mk,gk,bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,kodo,nai,ff_act_bi);
int kolshet=0;
db=kr=0.;
for(i=0; i < ks; i++)
 {
  strzag(LINES-1,0,ks,i);
  sh.new_plus(sheta_srs.ravno(i));
  
  if(proverka(sheta.ravno(),sh.ravno(),1,0) != 0)
    continue;

  printw("%s\n",sh.ravno());
  refresh();
  kolshet++;
  db1=kr1=0.;
  saloro1(sh.ravno(),dn,mn,gn,dk,mk,gk,kodo,nai,ff1,ff4,ffact,&db1,&kr1,par,&sheta_srs,&mdo,kodgr.ravno(),naimgr.ravno(),imatmptab,sheta_kor.ravno(),ff_act,ff_act_bi,ff_spr);
  obit(db1,kr1,0,0,ff1);
  obit(db1,kr1,0,1,ff4);
  db+=db1; kr+=kr1;
 }
obit(db,kr,1,0,ff1);
obit(db,kr,1,1,ff4);
podpis(ff1);
podpis(ff4);
fclose(ff1);
fclose(ff4);
//Концовка распечатки

if(kolshet > 1)
 {
  obit_ac(db,kr,ffact);
  obit_ac(db,kr,ff_act);
  obit_ac(db,kr,ff_act_bi);
 }
konact(ffact);
fclose(ffact);

konact(ff_act);
fclose(ff_act);

konact(ff_act_bi);
fclose(ff_act_bi);

fclose(ff_spr);

potf(kodo,nai,dn,mn,gn,dk,mk,gk,imaf1,imaf2,&sheta_srs,&mdo,imatmptab);


printw_vr(tmm);


OSTANOV();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Распечатка проводок c дневными итогами по контрагенту"));

oth->spis_imaf.plus(imaf1);
oth->spis_naim.plus(gettext("Распечатка проводок по контрагенту с итогами по документам"));

oth->spis_imaf.plus(imaf2);
oth->spis_naim.plus(gettext("Распечатка проводок по контрагенту с итогами по комментариям"));

oth->spis_imaf.plus(imaf4);
oth->spis_naim.plus(gettext("Распечатка проводок по контрагенту"));

oth->spis_imaf.plus(imafacts);
oth->spis_naim.plus(gettext("Акт сверки"));

oth->spis_imaf.plus(imaf_acts);
oth->spis_naim.plus(gettext("Акт сверки с общими суммами по документам"));

oth->spis_imaf.plus(imaf_acts_bi);
oth->spis_naim.plus(gettext("Акт сверки с общими суммами по документам без месячных итогов"));

oth->spis_imaf.plus(imaf_spr);
oth->spis_naim.plus(gettext("Справка про сальдо по контаргенту"));

for(int nomer=0; nomer < oth->spis_imaf.kolih(); nomer++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nomer),1);
return(0);
}

/*************************************/
/*Получение сальдо по заданному счету*/
/*************************************/
void saloro1(const char *sh,short dn,short mn,short gn,
short dk,short mk,short gk,const char *kor,const char *nai,FILE *ff1,
FILE *ff4,FILE *ffact,
double *db,double *kr,
short mks, //0-все проводки 1-без взаимно кореспондирующих
class SPISOK *sheta_srs,
class masiv_din_double *mdo,
const char *kodgr,
const char *naimgr,
const char *imatmptab,
const char *sheta_kor,
FILE *ff_act,
FILE *ff_act_bi,
FILE *ff_spr)
{
time_t          tmm;
struct  tm      *bf;
double          ddn,kkn;
short           mvs;
short           d,m,g;
class iceb_tu_str shh("");
double		dd,kk;
int		nm;
long		kolstr;
SQL_str         row;
SQLCURSOR       cur;
char		strsql[1024];
class iceb_tu_str naish("");
short		godn;
short		mes=0,god=0;
short		shet=0;
double		deb=0.,kre=0.;
double		debm=0.,krem=0.;
int		val=0;
class iceb_tu_str koment;
short	startgodb=0; /*Стартовый год*/

if(iceb_t_poldan("Стартовый год",strsql,"nastrb.alx") == 0)
 startgodb=atoi(strsql);

godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;

shh.new_plus(sh);


sprintf(strsql,"select nais,stat from Plansh where ns='%s'",sh);
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  naish.new_plus(row[0]);
  val=atoi(row[1]);
 }
if(val == 1)
 val=-1;
 
ddn=kkn=0.;
dd=kk=0.;
sprintf(strsql,"select deb,kre from Saldo where kkk='1' and gs=%d and \
ns='%s' and kodkon='%s'",godn,shh.ravno(),kor);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  ddn+=atof(row[0]);
  kkn+=atof(row[1]);
 }
if(ddn > kkn)
 {
  ddn=ddn-kkn; 
  kkn=0.;
 }
else
 {
  kkn=kkn-ddn;
  ddn=0;
 }
time(&tmm);
bf=localtime(&tmm);

fprintf(ff1,"%s\n\
%s: %s %s\n\
%s %s %s %s\n\
%s %d.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d  %s - %d:%d\n",
iceb_t_get_pnk("00",0),
gettext("Счет"),sh,naish.ravno(),
gettext("Распечатка проводок"),
gettext("для контрагента"),
kor,nai,
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("Время"),
bf->tm_hour,bf->tm_min);

if(kodgr[0] != '\0')
 fprintf(ff1,"%s:%s %s\n",gettext("Группа"),kodgr,naimgr);
 
fprintf(ff1,"\
--------------------------------------------------------------------------------------------\n");

fprintf(ff1,gettext("\
  Дата    |Сче.к.|Записал|Номер док. |    Дебет   |   Кредит   |    К о м е н т а р и й    |\n"));
/********
                                                                123456789012345678901234567
*********/

fprintf(ff1,"\
--------------------------------------------------------------------------------------------\n");

fprintf(ff4,"%s\n\
%s: %s %s\n\
%s %s %s %s\n\
%s %d.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d  %s - %d:%d\n",
iceb_t_get_pnk("00",0),
gettext("Счет"),sh,naish.ravno(),
gettext("Распечатка проводок"),
gettext("для контрагента"),
kor,nai,
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ff4,"\
--------------------------------------------------------------------------------------------\n");

fprintf(ff4,gettext("\
  Дата    |Сче.к.|Записал|Номер док. |    Дебет   |   Кредит   |    К о м е н т а р и й    |\n"));

fprintf(ff4,"\
--------------------------------------------------------------------------------------------\n");

zagsh(sh,ffact);

class akt_sverki akt_svr;
akt_svr.shet.new_plus(sh);

mvs=0;
g=godn;
d=1;
m=1;
int nomer=0;
int kolbsn37=iceb_tu_kolbait(37,gettext("Сальдо начальное"));
int kolbsn20=iceb_tu_kolbait(20,gettext("Сальдо начальное"));
while(sravmydat(d,m,g,dk,mk,gk) <= 0)
 {

  if(sravmydat(d,m,g,dn,mn,gn) >= 0 && mvs == 0)
   {
    nm=sheta_srs->find(sh);
    akt_svr.dn=d;
    akt_svr.mn=m;
    akt_svr.gn=g;

    if(ddn > kkn)    
     {
      akt_svr.start_saldo_deb=ddn-kkn;
      ddn=ddn-kkn;
      mdo->plus(ddn,nm*2);
      kkn=0.;
      printw("%20s\n",prnbr(ddn));
      
      fprintf(ff1,"%*s:%12s\n",kolbsn37,gettext("Сальдо начальное"),prnbr(ddn));
      
      fprintf(ff4,"%*s:%12s\n",kolbsn37,gettext("Сальдо начальное"),prnbr(ddn));

      fprintf(ffact,"%02d.%02d.%4d %-*.*s %10.2f %10s| |\n",
      d,m,g,kolbsn20,kolbsn20,gettext("Сальдо начальное"),ddn," ");

      fprintf(ffact,"\
------------------------------------------------------ ---------------------------------------------------\n");

     }
    else
     {
      akt_svr.start_saldo_kre=kkn-ddn;
      kkn=kkn-ddn;
      mdo->plus(kkn,nm*2+1);
      ddn=0.;
      printw("%20s %20s\n"," ",prnbr(kkn));

      fprintf(ff1,"%*s:%12s %12s\n",kolbsn37,
      gettext("Сальдо начальное")," ",prnbr(kkn));

      fprintf(ff4,"%*s:%12s %12s\n",kolbsn37,
      gettext("Сальдо начальное")," ",prnbr(kkn));

      fprintf(ffact,"%02d.%02d.%4d %-*.*s %10s %10.2f| |\n",
      d,m,g,kolbsn20,kolbsn20,gettext("Сальдо начальное")," ",kkn);

      fprintf(ffact,"\
------------------------------------------------------ ---------------------------------------------------\n");
     }
      mvs=1;
   }

  sprintf(strsql,"select * from Prov where val=%d and datp='%04d-%02d-%02d' and sh='%s' and kodkon='%s' order by sh asc",
  val,g,m,d,shh.ravno(),kor);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    dpm(&d,&m,&g,1);
    continue;
   }

  if(kolstr == 0)
   {
    dpm(&d,&m,&g,1);
    continue;
   }
  shet=0;
  while(cur.read_cursor(&row) != 0)
   {
/*
      printw("%s %s %s %s %s deb=%s kre=%s\n",
      row[0],row[1],row[2],row[3],row[4],row[9],row[10]);
      refresh();
*/ 
    if(mks == 1)
     if(SRAV(sh,row[3],1) == 0 || sheta_srs->find(row[3]) == 0)
       continue;
    deb=atof(row[9]);
    kre=atof(row[10]);


    if(sravmydat(d,m,g,dn,mn,gn) >= 0)/*период*/
    if(sravmydat(dk,mk,gk,d,m,g) >= 0)
     {
      if(shet == 0)
       {
        if(sravmydat(1,m,g,1,mes,god) != 0)
         {
          if(mes != 0)
           {
            salorksl(dd,kk,ddn,kkn,debm,krem,ff1,ffact);
            debm=krem=0.;
           }
          mes=m;
          god=g;
         }
        shet=1;
       }

      if(proverka(sheta_kor,row[3],1,0) == 0)
       {
        dd+=deb;
        kk+=kre;
        debm+=deb;
        krem+=kre;

        int kbsh=iceb_tu_kolbait(6,row[3]);
        int kbkto=iceb_tu_kolbait(3,row[5]);
        int kbnomd=iceb_tu_kolbait(11,row[6]);
        int kbkom=iceb_tu_kolbait(27,row[13]);
        
        fprintf(ff1,"%02d.%02d.%4d %-*s %-*s%4s %-*s %12s",
        d,m,g,kbsh,row[3],kbkto,row[5],row[11],kbnomd,row[6],prnbr(deb));

        fprintf(ff1," %12s %-*.*s\n",prnbr(kre),kbkom,kbkom,row[13]);
        for(int nom=27; nom < iceb_tu_strlen(row[13]); nom+=27)
          fprintf(ff1,"%63s %.*s\n","",iceb_tu_kolbait(27,iceb_tu_adrsimv(nom,row[13])),iceb_tu_adrsimv(nom,row[13]));

        fprintf(ff4,"%02d.%02d.%4d %-*s %-*s%4s %-*s %12s",
        d,m,g,kbsh,row[3],kbkto,row[5],row[11],kbnomd,row[6],prnbr(deb));

        fprintf(ff4," %12s %-*.*s\n",prnbr(kre),kbkom,kbkom,row[13]);
        for(int nom=27; nom < iceb_tu_strlen(row[13]); nom+=27)
          fprintf(ff4,"%63s %.*s\n","",iceb_tu_kolbait(27,iceb_tu_adrsimv(nom,row[13])),iceb_tu_adrsimv(nom,row[13]));

        koment.new_plus(row[13]);
        sprintf(strsql,"insert into %s values ('%s','%s','%04d-%02d-%02d','%s','%s',%s,%s,'%s',%s,%s)",
        imatmptab,
        row[2],row[6],g,m,d,row[3],row[5],row[9],row[10],koment.ravno_filtr(),row[7],row[15]);
               
        sql_zapis(strsql,1,1);    

        koment.new_plus(row[6]);
        koment.plus(" ",row[13]);
        
        fprintf(ffact,"%02d.%02d.%4d %-*.*s %10.2f %10.2f| |\n",
        d,m,g,
        iceb_tu_kolbait(20,koment.ravno()),
        iceb_tu_kolbait(20,koment.ravno()),
        koment.ravno(),
        deb,kre);

        //заряжаем массивы
        sprintf(strsql,"%d.%d.%d|%s",d,m,g,row[6]);
        if(row[6][0] == '\0')
           nomer=-1;
        else
           nomer=akt_svr.data_nomd.find(strsql);

        if(nomer < 0)
         {
          akt_svr.data_nomd.plus(strsql);
          if(strstrm(koment.ravno(),"-ПДВ") == 1) //есть образец в строке
           {
            iceb_tu_cpbstr(&akt_svr.koment,koment.ravno(),"-ПДВ");
           }
          else
            akt_svr.koment.plus(koment.ravno());
         }
        akt_svr.suma_deb.plus(deb,nomer);
        akt_svr.suma_kre.plus(kre,nomer);

        fprintf(ffact,"\
------------------------------------------------------ ---------------------------------------------------\n");
       }
     }


    if(sravmydat(d,m,g,dn,mn,gn) < 0) /*до периода*/
     {
      ddn+=deb;
      kkn+=kre;
     }

   }
  dpm(&d,&m,&g,1);
 }

printw("%20s",prnbr(dd));
printw(" %20s\n",prnbr(kk));

salorksl(dd,kk,ddn,kkn,debm,krem,ff1,ffact);
salorksl(dd,kk,ddn,kkn,0.,0.,ff4,NULL);

iceb_t_spk(dk,mk,gk,sh,naish.ravno(),kor,nai,kk+kkn-dd-ddn,ff_spr);
fprintf(ff_spr,"\n\n");

*db+=dd+ddn;
*kr+=kk+kkn;

akt_sv(&akt_svr,ff_act,ff_act_bi);

}
/*********************/
/*Выдача общего итога*/
/*********************/
void obit(double db,double kr,
short mt, //0-пром. итог 1- общий
int metkascr, //0-выводить на экран 1-нет
FILE *ff1)
{
double          brr;

if(mt == 1 && metkascr == 0)
  printw("--------------------------------------------\n");

if(mt == 1)
 {
  fprintf(ff1,"\
---------------------------------------------------------------------------------------\n\
%*s:%12s",
  iceb_tu_kolbait(37,gettext("Общий итог")),gettext("Общий итог"),prnbr(db));
  fprintf(ff1," %12s\n",prnbr(kr));
 }

if(db >= kr)
 {
  brr=db-kr;
  if(metkascr == 0)
    printw("%20s\n",prnbr(brr));
  if(mt == 1)
      fprintf(ff1,"%38s%12s\n"," ",prnbr(brr));
 }
else
 {
  brr=kr-db;
  if(metkascr == 0)
    printw("%20s %20s\n"," ",prnbr(brr));
  if(mt == 1)
      fprintf(ff1,"%38s%12s %12s\n"," "," ",prnbr(brr));
 }

}

/*********************************/
/*Работа с отсортированным файлом*/
/*********************************/
void potf(const char *kor,const char *nai,short dn,short mn,short gn,
short dk,short mk,short gk,char *imaf1,char *imaf2,class SPISOK *sheta_srs,class masiv_din_double *mdo,
const char *imatmptab)
{
time_t          tmm;
struct  tm      *bf;
FILE		*ff1;
short		d,m,g;
class iceb_tu_str sh(""),shz("");
class iceb_tu_str shk("");
class iceb_tu_str nn(""),nnz("");
double		deb,deb1=0.,debo,debs;
double		kre,kre1=0.,kreo,kres;
class iceb_tu_str kom(""),komz("");
class iceb_tu_str kto("");
short		kop;
int		nm;
class iceb_tu_str naish("");
char		strsql[1024];
SQL_str         row;
char		bros[512];

time(&tmm);
bf=localtime(&tmm);

class SQLCURSOR curtmp;
SQL_str rowtmp;

sprintf(strsql,"select * from %s order by sh asc,datp asc,nomd asc",imatmptab);

int kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  error_op_nfil(imaf1,errno,"");
  return;
 }

fprintf(ff1,gettext("Проводки по одному документу просумированы\n"));

fprintf(ff1,"%s\n\
%s %s %s %s\n\
     %s %d.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d  %s - %d:%d\n",
iceb_t_get_pnk("00",0),
gettext("Распечатка проводок"),
gettext("для контрагента"),
kor,nai,
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("Время"),
bf->tm_hour,bf->tm_min);

kre=deb=debo=kreo=debs=kres=0.;
nm=kop=0;
SQLCURSOR curr;
int podrz=0;
int tipz=0;
short ddz=0,mdz=0,gdz=0;
class iceb_tu_str podsistemaz("");
class iceb_tu_str nom_dokz("");
while(curtmp.read_cursor(&rowtmp) != 0)
 {
  sh.new_plus(rowtmp[0]);
  rsdat(&d,&m,&g,rowtmp[2],2);
  kom.new_plus(rowtmp[7]);
  nn.new_plus(rowtmp[1]);
  shk.new_plus(rowtmp[3]);
  kto.new_plus(rowtmp[4]);
  deb1=atof(rowtmp[5]);
  kre1=atof(rowtmp[6]);
   
  if(SRAV(nn.ravno(),nnz.ravno(),0) != 0)
   {
    sprintf(bros,"%s %s:",gettext("Итог по документу"),nnz.ravno());
    fprintf(ff1,"%*s %12s",iceb_tu_kolbait(32,bros),bros,prnbr(deb));
    fprintf(ff1," %12s %s\n\n",prnbr(kre),salorok_gvd(podsistemaz.ravno(),ddz,mdz,gdz,podrz,tipz,nom_dokz.ravno()));

    kop=0;
    nnz.new_plus(nn.ravno());
    deb=kre=0.;
   }

  if(SRAV(sh.ravno(),shz.ravno(),0) != 0)
   {
    if(shz.ravno()[0] != '\0')
     {
      if(deb != 0. || kre != 0.)
       {
        sprintf(bros,"%s %s:",gettext("Итог по документу"),nnz.ravno());
        fprintf(ff1,"%*s %12s",iceb_tu_kolbait(32,bros),bros,prnbr(deb));
        fprintf(ff1," %12s %s\n\n",prnbr(kre),salorok_gvd(podsistemaz.ravno(),ddz,mdz,gdz,podrz,tipz,nom_dokz.ravno()));
        deb=kre=0.;
        
       }
      vsali(debs,kres,nm,0,ff1,mdo);
      debs=kres=0.;
     }

    nm=sheta_srs->find(sh.ravno());
    debo+=mdo->ravno(nm*2);
    kreo+=mdo->ravno(nm*2+1);


    sprintf(strsql,"select nais from Plansh where ns='%s'",sh.ravno());
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
      naish.new_plus(row[0]);
    else
      naish.new_plus("");
          
    fprintf(ff1,"\n%s: %s %s\n",
    gettext("Счёт"),sh.ravno(),naish.ravno());

    fprintf(ff1,"\
---------------------------------------------------------------------------------------\n");
    fprintf(ff1,gettext("\
  Дата    |  Счёт  | Номер |Подс|   Дебет    |   Кредит   |      Коментарий           |\n\
          |        |докумен|ист.|            |            |     Вид договора          |\n"));
    fprintf(ff1,"\
---------------------------------------------------------------------------------------\n");


    if(mdo->ravno(nm*2) > mdo->ravno(nm*2+1) )
      {
       fprintf(ff1,"%*s:%12s\n",iceb_tu_kolbait(32,gettext("Сальдо начальное")),gettext("Сальдо начальное"),prnbr(mdo->ravno(nm*2)));
      }
     else
      {
       fprintf(ff1,"%*s:%12s %12s\n",iceb_tu_kolbait(32,gettext("Сальдо начальное")),gettext("Сальдо начальное")," ",prnbr(mdo->ravno(nm*2+1)));
      }

    shz.new_plus(sh.ravno());
   }

  rsdat(&ddz,&mdz,&gdz,rowtmp[2],2);
  nom_dokz.new_plus(rowtmp[1]);
  podrz=atoi(rowtmp[8]); /*именно здесь*/
  tipz=atoi(rowtmp[9]);
  podsistemaz.new_plus(rowtmp[4]);
  
  fprintf(ff1,"%02d.%02d.%4d %-*s %-*s %-*s %12s",
  d,m,g,
  iceb_tu_kolbait(8,shk.ravno()),shk.ravno(),
  iceb_tu_kolbait(7,nn.ravno()),nn.ravno(),
  iceb_tu_kolbait(4,kto.ravno()),kto.ravno(),
  prnbr(deb1));

  fprintf(ff1," %12s %.*s\n",prnbr(kre1),iceb_tu_kolbait(27,kom.ravno()),kom.ravno());

  for(int nom=27; nom < iceb_tu_strlen(kom.ravno()); nom+=27)
        fprintf(ff1,"%58s %.*s\n","",iceb_tu_kolbait(27,iceb_tu_adrsimv(nom,kom.ravno())),iceb_tu_adrsimv(nom,kom.ravno()));

  if(nn.ravno()[0] != '\0' )
   {
    kop++;
   }
  deb+=deb1;
  kre+=kre1;
  debs+=deb1;
  kres+=kre1;
  debo+=deb1;
  kreo+=kre1;
 }

sprintf(bros,"%s %s:",gettext("Итог по документу"),nnz.ravno());
fprintf(ff1,"%*s %12s",iceb_tu_kolbait(32,bros),bros,prnbr(deb));
fprintf(ff1," %12s %s\n\n",prnbr(kre),salorok_gvd(podsistemaz.ravno(),ddz,mdz,gdz,podrz,tipz,nom_dokz.ravno()));

vsali(debs,kres,nm,0,ff1,mdo);
vsali(debo,kreo,nm,1,ff1,mdo);
podpis(ff1);

fclose(ff1);

sprintf(strsql,"select * from %s order by sh asc,datp asc,koment asc,nomd asc,shk asc",imatmptab);

if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }



if((ff1 = fopen(imaf2,"w")) == NULL)
 {
  error_op_nfil(imaf2,errno,"");
  return;
 }

fprintf(ff1,gettext("Проводки по одному комментарию просумированы\n"));

fprintf(ff1,"%s\n\
%s %s %s %s\n\
     %s %d.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d  %s - %d:%d\n",
iceb_t_get_pnk("00",0),
gettext("Распечатка проводок"),
gettext("для контрагента"),
kor,nai,
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("Время"),
bf->tm_hour,bf->tm_min);

shz.new_plus("");
komz.new_plus("");
kre=deb=debo=kreo=debs=kres=0.;
kop=0;
while(curtmp.read_cursor(&rowtmp) != 0)
 {
  sh.new_plus(rowtmp[0]);
  rsdat(&d,&m,&g,rowtmp[2],2);
  kom.new_plus(rowtmp[7]);
  nn.new_plus(rowtmp[1]);
  shk.new_plus(rowtmp[3]);
  kto.new_plus(rowtmp[4]);
  deb1=atof(rowtmp[5]);
  kre1=atof(rowtmp[6]);
 
 
  if(SRAV(kom.ravno(),komz.ravno(),0) != 0)
   {
    if(komz.getdlinna() > 1)
     {
      sprintf(bros,"%s-%.*s:",
      gettext("Итог по комментарию"),
      iceb_tu_kolbait(10,komz.ravno()),
      komz.ravno());

      fprintf(ff1,"%*s %12s",iceb_tu_kolbait(32,bros),bros,prnbr(deb));
      fprintf(ff1," %12s\n\n",prnbr(kre));
     }
    kop=0;
    komz.new_plus(kom.ravno());
    deb=kre=0.;
   }

  if(SRAV(sh.ravno(),shz.ravno(),0) != 0)
   {
    if(shz.ravno()[0] != '\0')
     {
      vsali(debs,kres,nm,0,ff1,mdo);
      debs=kres=0.;
     }

    nm=sheta_srs->find(sh.ravno());
    debo+=mdo->ravno(nm*2);
    kreo+=mdo->ravno(nm*2+1);
    
    sprintf(strsql,"select nais from Plansh where ns='%s'",sh.ravno());
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
      naish.new_plus(row[0]);
    else
      naish.new_plus("");
    fprintf(ff1,"\n%s %s %s\n",gettext("Счёт"),sh.ravno(),naish.ravno());
    fprintf(ff1,"\
---------------------------------------------------------------------------------------\n");
    fprintf(ff1,gettext("\
  Дата    |  Счёт  | Номер |Подс|   Дебет    |   Кредит   |      Коментарий           |\n\
          |        |докумен|ист.|            |            |                           |\n"));
    fprintf(ff1,"\
---------------------------------------------------------------------------------------\n");

    if(mdo->ravno(nm*2) > mdo->ravno(nm*2+1))    
       fprintf(ff1,"%*s:%12s\n",iceb_tu_kolbait(32,gettext("Сальдо начальное")),gettext("Сальдо начальное"),prnbr(mdo->ravno(nm*2)));
     else
       fprintf(ff1,"%*s:%12s %12s\n",iceb_tu_kolbait(32,gettext("Сальдо начальное")),gettext("Сальдо начальное")," ",prnbr(mdo->ravno(nm*2+1)));
    shz.new_plus(sh.ravno());
   }

  fprintf(ff1,"%02d.%02d.%4d %-*s %-*s %-*s %12s",
  d,m,g,
  iceb_tu_kolbait(8,shk.ravno()),shk.ravno(),
  iceb_tu_kolbait(7,nn.ravno()),nn.ravno(),
  iceb_tu_kolbait(4,kto.ravno()),kto.ravno(),
  prnbr(deb1));

  fprintf(ff1," %12s %.*s\n",prnbr(kre1),iceb_tu_kolbait(27,kom.ravno()),kom.ravno());

  for(int nom=27; nom < iceb_tu_strlen(kom.ravno()); nom+=27)
        fprintf(ff1,"%58s %.*s\n","",iceb_tu_kolbait(27,iceb_tu_adrsimv(nom,kom.ravno())),iceb_tu_adrsimv(nom,kom.ravno()));

  if(kom.ravno()[0] != '\0' )
    kop++;
  deb+=deb1;
  kre+=kre1;
  debo+=deb1;
  kreo+=kre1;
  debs+=deb1;
  kres+=kre1;
  
 }

sprintf(bros,"%s-%.*s:",
gettext("Итог по комментарию"),
iceb_tu_kolbait(10,komz.ravno()),
komz.ravno());

fprintf(ff1,"%*s %12s",iceb_tu_kolbait(32,bros),bros,prnbr(deb));
fprintf(ff1," %12s\n\n",prnbr(kre));

vsali(debs,kres,nm,0,ff1,mdo);
vsali(debo,kreo,nm,1,ff1,mdo);

podpis(ff1);
fclose(ff1);

}


/*********************/
/*Выдача итога в файл*/
/*********************/
void		vsali(double debs,double kres,short nm,short met,FILE *ff1,class masiv_din_double *mdo)
{
double		brr;

if(met == 1)
 {
  fprintf(ff1,"\n\
---------------------------------------------------------------------------------------\n\
%*s:%12s",iceb_tu_kolbait(32,gettext("Общий итог")),gettext("Общий итог"),prnbr(debs));

  fprintf(ff1," %12s\n",prnbr(kres));

  if(debs >= kres)
   {
    brr=debs -kres;
    fprintf(ff1,"%33s%12s\n"," ",prnbr(brr));
   }
  else
   {
    brr=kres-debs;
    fprintf(ff1,"%33s%12s %12s\n"," "," ",prnbr(brr));
   }

  return;
 }

fprintf(ff1,"\
---------------------------------------------------------------------------------------\n\
%*s:%12s",iceb_tu_kolbait(32,gettext("Оборот за период")),gettext("Оборот за период"),prnbr(debs));

fprintf(ff1," %12s\n",prnbr(kres));

brr=debs+mdo->ravno(nm*2);
fprintf(ff1,"%*s:%12s",iceb_tu_kolbait(32,gettext("Сальдо развернутое")),gettext("Сальдо развернутое"),prnbr(brr));

brr=kres+mdo->ravno(nm*2+1);
fprintf(ff1," %12s\n",prnbr(brr));
 
if(mdo->ravno(nm*2)+ debs >= mdo->ravno(nm*2+1)+kres)
 {
  brr=mdo->ravno(nm*2)+ debs - (mdo->ravno(nm*2+1)+kres);
  fprintf(ff1,"%*s:%12s\n",iceb_tu_kolbait(32,gettext("Сальдо конечное")),gettext("Сальдо конечное"),prnbr(brr));
 }
else
 {
  brr=mdo->ravno(nm*2+1)+kres-(mdo->ravno(nm*2)+ debs) ;
  fprintf(ff1,"%*s:%12s %12s\n",iceb_tu_kolbait(32,gettext("Сальдо конечное")),gettext("Сальдо конечное")," ",prnbr(brr));
 }

}
/*********************/
/*Выдача общего итога в акты сверки*/
/*********************/
void obit_ac(double db,double kr,FILE *ff1)
{
double          brr=0.;

fprintf(ff1,"----------------------------------------------------------------------------------------------------------\n");
fprintf(ff1,"%*s:%10.2f %10.2f\n",iceb_tu_kolbait(31,gettext("Оборот общий")),gettext("Оборот общий"),db,kr);
if(db >= kr)
 {
  brr=db-kr;
  fprintf(ff1,"%*s:%10s\n",iceb_tu_kolbait(31,gettext("Сальдо общее")),gettext("Сальдо общее"),prnbr(brr));
 }
else
 {
  brr=kr-db;
  fprintf(ff1,"%*s:%10s %10s\n",iceb_tu_kolbait(31,gettext("Сальдо общее")),gettext("Сальдо общее")," ",prnbr(brr));
 }

}
/**********************************************/
/*Взять из шапки документа вид договора*/
/********************************************/
const char *salorok_gvd(const char *podsistema,short dd,short md,short gd,int podr,int tz,const char *nom_dok)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
static class iceb_tu_str vid_dog;

vid_dog.new_plus("");

if(SRAV(podsistema,ICEB_MP_MATU,0) == 0) /*подсистема материальный учёт*/
 {
  
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=7",
  gd,podr,nom_dok);
  if(readkey(strsql,&row,&cur) == 1)
   vid_dog.new_plus(row[0]);
 }

if(SRAV(podsistema,ICEB_MP_USLUGI,0) == 0) /*подсистема учёт услуг*/
 {
  
  sprintf(strsql,"select uslpr from Usldokum where datd='%04d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d",
  gd,md,dd,podr,nom_dok,tz);
  if(readkey(strsql,&row,&cur) == 1)
   vid_dog.new_plus(row[0]);
 }


return(vid_dog.ravno());
}
