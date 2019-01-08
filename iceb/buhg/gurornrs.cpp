/*$Id: gurornrs.c,v 5.27 2013/09/26 09:43:33 sasa Exp $*/
/*28.04.2016	19.06.2001	Белых А.И.	gurornrs.c
Расчёт журнала-ордера для небалансовых счетов с развернутым
сальдо
*/
#include        <errno.h>
#include        "buhg.h"

void	sapnbs(short,short,short,short,short,short,int,FILE*);
void	shetnbs(short,short,short,short,short,short,int*,int*,FILE*);

extern char	*imabaz;
extern short	startgodb; /*Стартовый год*/
extern short	vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/
extern int kol_strok_na_liste;

int gurornrs(class guror_rr *rekrs,class spis_oth *oth)
{
short dn=0,mn=0,gn=0;
short dk=0,mk=0,gk=0;
time_t		tmm;
char		strsql[1024];
SQL_str		row,row1;
int		kolstr;
int		koo;
short		godn;
int		i1;
short		d,m,g;
SQLCURSOR cur;
SQLCURSOR cur1;
FILE		*ff;
char		imaf[64];
int		kollist=0;
int		kolstras=0;
class iceb_tu_str kontr("");
class iceb_tu_str naikontr("");
class iceb_getkue_dat getval(rekrs->kod_val.ravno());
class iceb_tu_str naim("");

sprintf(strsql,"select nais,vids from Plansh where ns='%s'",rekrs->shet.ravno());

if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,gettext("Не найден счёт %s в плане счетов !"),rekrs->shet.ravno());
  iceb_t_soob(strsql);
  return(1);
 }
int vds=atoi(row[1]);
naim.new_plus(row[0]);
if(rsdatp(&dn,&mn,&gn,rekrs->dat1.ravno(),&dk,&mk,&gk,rekrs->dat2.ravno()) != 0)
  return(1);

godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;
class iceb_tu_str start_god(godn);
start_god.plus("-01-01");


GDITE();
clear();

time(&tmm);

printw("\n%s - %s %s\n%s %d.%d.%d%s %s %d.%d.%d%s\n",
gettext("Счёт"),
rekrs->shet.ravno(),naim.ravno(),
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."));

refresh();

/*Определяем все организации и открываем массивы дебетов и
кредитов организаций
*/

if(vds == 0 || vplsh == 1)
 sprintf(strsql,"select distinct kodkon from Skontr where ns like '%s%%'",rekrs->shet.ravno());

if(vds == 1 && vplsh == 0)
 sprintf(strsql,"select kodkon from Skontr where ns='%s'",rekrs->shet.ravno());
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,gettext("Не введён список контрагентов для счёта %s !"),rekrs->shet.ravno());
  iceb_t_soob(strsql);
  return(1);
 }

class SPISOK KONTR;

koo=0;

while(cur.read_cursor(&row) != 0)
 {
  if(rekrs->mso.ravno()[0] != '\0')
   {
    sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[0]);
    if(readkey(strsql,&row1,&cur1) != 1)
     {
      beep();
      printw("%s %s !\n",
      gettext("Не найден код контрагента"),row[0]);
      refresh();
      continue;
     }
    if(SRAV(row1[0],rekrs->mso.ravno(),0) != 0)
      continue;  
   }
    
  KONTR.plus(row[0]);
  koo++;

 }

printw("%s: %d\n",
gettext("Количество контрагентов"),koo);
refresh();
if(koo == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }


masiv_din_double MDO;
masiv_din_double MKO;
masiv_din_double MDO1;
masiv_din_double MKO1;
masiv_din_double MDO2;
masiv_din_double MKO2;
masiv_din_int PROV;

MKO.make_class(koo);
MDO.make_class(koo);
MKO1.make_class(koo);
MDO1.make_class(koo);
MKO2.make_class(koo);
MDO2.make_class(koo);

PROV.make_class(koo);

/*Узнаем начальное сальдо по каждой организации*/
printw(gettext("Вычисляем стартовое сальдо по всем контрагентам\n"));
refresh();

if(vds == 0 || vplsh == 1)
 sprintf(strsql,"select kodkon,deb,kre from Saldo where kkk='1' and gs=%d \
and ns like '%s%%'",godn,rekrs->shet.ravno());

if(vds == 1 && vplsh == 0)
 sprintf(strsql,"select kodkon,deb,kre from Saldo where kkk='1' and gs=%d \
and ns='%s'",godn,rekrs->shet.ravno());

//printw("%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
double deb=0.,kre=0.;

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  if(rekrs->mso.ravno()[0] != '\0')
   {
    sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[0]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      sprintf(strsql,gettext("Не найден код %s в списке счета %s !"),row[0],rekrs->shet.ravno());
      iceb_t_soob(strsql);
      continue;
     }
    if(SRAV(row1[0],rekrs->mso.ravno(),0) != 0)
      continue;  
   }

  if((i1=KONTR.find_r(row[0])) >= 0)
   {
    deb=getval.toue(row[1],start_god.ravno());
    kre=getval.toue(row[2],start_god.ravno());
    MDO.plus(deb,i1);
    MKO.plus(kre,i1);
   }      
  else
   {
    sprintf(strsql,"%s %s %s !",
    gettext("Не найден код контрагента"),row[0],
    gettext("в массиве кодов"));
    iceb_t_soob(strsql);
   }

 }
printw("%s\n",gettext("Смотрим проводки, заполняем массивы."));
refresh();

sprintf(imaf,"nbs%s_%d.lst",rekrs->shet.ravno(),getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

char imaf_spr[64];
FILE *ff_spr;

sprintf(imaf_spr,"nbsspr%d.lst",getpid());

if((ff_spr = fopen(imaf_spr,"w")) == NULL)
 {
  error_op_nfil(imaf_spr,errno,"");
  return(1);
 }

//Расчёт
d=1; m=1; g=godn;
deb=kre=0.;
double deb1=0.,kre1=0.;
double deb2=0.,kre2=0.;
double ideb=0.,ikre=0.;
double ideb1=0.,ikre1=0.;
double ideb2=0.,ikre2=0.;

do
 {
//  printw("%d.%d.%d\n",d,m,g);
//  refresh();

  if(vds == 0 || vplsh == 1)
   sprintf(strsql,"select shk,kodkon,deb,kre,sh,datp from Prov \
where val=-1 and datp='%04d-%02d-%02d' and sh like '%s%%'",
  g,m,d,rekrs->shet.ravno());

  if(vds == 1 && vplsh == 0)
   sprintf(strsql,"select shk,kodkon,deb,kre,sh,datp from Prov \
where val=-1 and datp='%04d-%02d-%02d' and sh='%s'",
   g,m,d,rekrs->shet.ravno());
  //printw("%s\n",strsql);
  //OSTANOV();

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

  if(kolstr > 0)
  while(cur.read_cursor(&row) != 0)
   {
//    printw("%s %s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4],row[6]);
//    refresh();
    if(rekrs->mso.ravno()[0] != '\0')
     {
      sprintf(strsql,"select grup from Kontragent where kodkon='%s'",
      row[1]);
      if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
       {
        beep();
        printw("%s %s %s %s %s %s\n",row[5],row[4],row[0],row[1],row[2],row[3]);
        fprintf(ff,"%s %s %s %s %s %s\n",row[5],row[4],row[0],row[1],row[2],row[3]);
        printw("%s %s !\n",
        gettext("Не найден код контрагента"),
        row[1]);
        refresh();
        continue;
       }

      if(SRAV(rekrs->mso.ravno(),row1[0],0) != 0)
        continue;
     }    

    if(row[1][0] == '\0')
     {
      fprintf(ff,"%s %s %s %s %s %s\n",row[5],row[4],row[0],row[1],row[2],row[3]);
      fprintf(ff,gettext("В проводке нет кода контрагента !"));
      fprintf(ff,"\n");

      VVOD SOOB(1);
      sprintf(strsql,"%d.%d.%d %s %s %s %s %s",d,m,g,row[4],row[0],row[1],row[2],row[3]);
      SOOB.VVOD_SPISOK_add_MD(strsql);
      sprintf(strsql,gettext("В проводке нет кода контрагента !"));
      SOOB.VVOD_SPISOK_add_MD(strsql);
      soobshw(&SOOB,stdscr,-1,-1,0,1);

      continue;
     }
   
    if((i1=KONTR.find_r(row[1])) == -1)
     {
      if(ff != NULL)
       {
        fprintf(ff,"%s %s %s %s %s %s\n",row[5],row[4],row[0],row[1],row[2],row[3]);
        fprintf(ff,gettext("Не найден код %s в списке счета %s !"),
        row[1],rekrs->shet.ravno());
        fprintf(ff,"\n");
       }

      VVOD SOOB(1);
      sprintf(strsql,"%d.%d.%d %s %s %s %s %s",d,m,g,row[4],row[0],row[1],row[2],row[3]);
      SOOB.VVOD_SPISOK_add_MD(strsql);
      sprintf(strsql,gettext("Не найден код %s в списке счета %s !"),
      row[1],rekrs->shet.ravno());
      SOOB.VVOD_SPISOK_add_MD(strsql);
      soobshw(&SOOB,stdscr,-1,-1,0,1);

     }
    else
     {
      PROV.plus(1,i1);
      deb=getval.toue(row[2],row[5]);
      kre=getval.toue(row[3],row[5]);
       
      if(sravmydat(d,m,g,dn,mn,gn) < 0)
       {
        MDO1.plus(deb,i1);
        MKO1.plus(kre,i1);
       }
      else
       {
        MDO2.plus(deb,i1);
        MKO2.plus(kre,i1);
       }
     }


   }


  dpm(&d,&m,&g,1); //Увеличиваем дату на день
 }
while(sravmydat(d,m,g,dk,mk,gk) <= 0);


printw(gettext("Распечатываем оборот по счёту\n"));
refresh();

class iceb_tu_str zagolov;
sprintf(strsql,"%s %s %s (%s)",gettext("Журнал ордер по счёту"),
rekrs->shet.ravno(),naim.ravno(),gettext("Внебалансовый"));

zagolov.plus(strsql);
if(rekrs->kod_val.getdlinna() > 1)
 {
  sprintf(strsql," %s:%s",gettext("Валюта"),rekrs->kod_val.ravno());
  zagolov.plus(strsql);
 }
 
iceb_t_zagolov(zagolov.ravno(),dn,mn,gn,dk,mk,gk,ff);

kollist=1;
sapnbs(dn,mn,gn,dk,mk,gk,kollist,ff);
kolstras=11;

for(int i=0 ;i<koo;i++)
 {
  /*Проверяем были ли сделаны проводки несмотряна общий ноль*/
  if(PROV.ravno(i) == 0) //Не было проводок
  if(MDO.ravno(i) == 0. && MKO.ravno(i) == 0.\
  && MDO1.ravno(i) == 0. && MKO1.ravno(i) == 0.\
  && MDO2.ravno(i) == 0. && MKO2.ravno(i) == 0.)
    continue;

  naikontr.new_plus("");
  kontr.new_plus(KONTR.ravno(i));
/*******************
  if(KONTR.pol_masiv_char(i+1,kontr.ravno()) < 0)
   {
    printw("Ошибка поиска в массиве контрагентов !\n");
    OSTANOV();
    continue;
   }
********************/
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr.ravno());
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    fprintf(ff,"\n\n%s %s!!!\n\n",gettext("Не найден код контрагента"),kontr.ravno());

    sprintf(strsql,"%s %s",gettext("Не найден код контрагента"),kontr.ravno());
    iceb_t_soob(strsql);
    continue;
   }
  else
   {  
    naikontr.new_plus(row1[0]);
   }
  deb=MDO.ravno(i);
  kre=MKO.ravno(i);
  deb+=MDO1.ravno(i);
  kre+=MKO1.ravno(i);
  deb1=MDO2.ravno(i);
  kre1=MKO2.ravno(i);
  ideb1+=MDO2.ravno(i);
  ikre1+=MKO2.ravno(i);
  deb2=deb+deb1;    
  kre2=kre+kre1;    
  shetnbs(dn,mn,gn,dk,mk,gk,&kolstras,&kollist,ff);
  if(deb > kre)
   {
    ideb+=deb-kre;
    fprintf(ff,"%*s %-*.*s %14.2f %14s %14.2f %14.2f ",
    iceb_tu_kolbait(10,kontr.ravno()),kontr.ravno(),
    iceb_tu_kolbait(20,naikontr.ravno()),iceb_tu_kolbait(20,naikontr.ravno()),naikontr.ravno(),
    deb-kre," ",deb1,kre1);
   }
  else
   {
    ikre+=kre-deb;
    fprintf(ff,"%*s %-*.*s %14s %14.2f %14.2f %14.2f ",
    iceb_tu_kolbait(10,kontr.ravno()),kontr.ravno(),
    iceb_tu_kolbait(20,naikontr.ravno()),iceb_tu_kolbait(20,naikontr.ravno()),naikontr.ravno(),
    " ",kre-deb,deb1,kre1);
   }

  shetnbs(dn,mn,gn,dk,mk,gk,&kolstras,&kollist,ff);
  if(deb2 > kre2)
   {
    ideb2+=deb2-kre2;
    fprintf(ff,"%14.2f\n",deb2-kre2);
   }
  else
   {
    ikre2+=kre2-deb2;
    fprintf(ff,"%14s %14.2f\n"," ",kre2-deb2);
   }

  if(iceb_tu_strlen(naikontr.ravno()) > 20)
   {
    fprintf(ff,"%-10s %s\n"," ",iceb_tu_adrsimv(20,naikontr.ravno()));
    shetnbs(dn,mn,gn,dk,mk,gk,&kolstras,&kollist,ff);
   }

  if(i > 0)
   fprintf(ff_spr,"\n\n");  

  iceb_t_spk(dk,mk,gk,rekrs->shet.ravno(),naim.ravno(),kontr.ravno(),naikontr.ravno(),kre2-deb2,ff_spr);
 }
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------\n\
%-*.*s %14.2f %14.2f %14.2f %14.2f %14.2f %14.2f\n",
iceb_tu_kolbait(31,gettext(" И т о г о")),
iceb_tu_kolbait(31,gettext(" И т о г о")),
gettext(" И т о г о"),
ideb,ikre,ideb1,ikre1,ideb2,ikre2);


podpis(ff);


fclose(ff);
fclose(ff_spr);

printw("\n%*s:%20s",iceb_tu_kolbait(18,gettext("Сальдо начальное")),gettext("Сальдо начальное"),prnbr(ideb));

printw(" %20s\n",prnbr(ikre));

printw("%*s:%20s",iceb_tu_kolbait(18,gettext("Оборот за период")),gettext("Оборот за период"),prnbr(ideb1));

printw(" %20s\n",prnbr(ikre1));

printw("%*s:%20s",iceb_tu_kolbait(18,gettext("Сальдо конечное")),gettext("Сальдо конечное"),prnbr(ideb2));

printw(" %20s\n\n",prnbr(ikre2));

printw_vr(tmm);


OSTANOV();

oth->spis_imaf.plus(imaf);
sprintf(strsql,"%s %s",gettext("Журнал-ордер по счёту"),rekrs->shet.ravno());
oth->spis_naim.plus(strsql);

oth->spis_imaf.plus(imaf_spr);
oth->spis_naim.plus(gettext("Справка про сальдо по контрагенту"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),1);

return(0);
}

/*****************/
/*Шапка документа*/
/*****************/
void	sapnbs(short dn,short mn,short gn,short dk,short mk,short gk,
int sl,FILE *ff)
{
  fprintf(ff,"%100s%s N%d\n","",gettext("Лист"),sl);
  fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff,gettext("  Код     |    Наименование    | C а л ь д о  на  %02d.%02d.%d |    Оборот за перiод         | С а л ь д о  на  %02d.%02d.%d |\n"),
  dn,mn,gn,dk,mk,gk);
  fprintf(ff,gettext("контраген-|     контрагента    -------------------------------------------------------------------------------------------\n"));
  fprintf(ff,gettext("   та     |                    |   Дебет      |   Кредит     |   Дебет      |    Кредит    |   Дебет      |   Кредит     |\n"));

  fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------\n");
}
/********************/
/*Счетчик стpок*/
/********************/
void	shetnbs(short dn,short mn,short gn,short dk,short mk,short gk,
int *kolstrok,int *kollist,FILE *ff)
{
*kolstrok+=1;
if(*kolstrok > kol_strok_na_liste)
 {
  fprintf(ff,"\f");
  *kolstrok=6;
  sapnbs(dn,mn,gn,dk,mk,gk,*kollist,ff);
  *kollist+=1;  
 }
}
