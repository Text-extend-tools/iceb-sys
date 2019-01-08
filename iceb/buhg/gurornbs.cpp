/* $Id: gurornbs.c,v 5.21 2013/09/26 09:43:33 sasa Exp $ */
/*14.07.2015	18.06.2001	Белых А.И.	gurornbs.c
Журнал ордер по не балансовым счетам
*/
#include        <errno.h>
#include        "buhg.h"

extern char	*imabaz;
extern short	startgodb; /*Стартовый год*/
extern short	vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/
/*********************
int gurornbs(const char *sh, //Номер счета
short dn,short mn,short gn,
short dk,short mk,short gk,
short vds,  //0-счет 1-субсчет
const char *naim, //Наименование счета
class spis_oth *oth)
**********************/
int gurornbs(class guror_rr *rekrs,class spis_oth *oth)
{
short		d2,m2,g2;
short		godn;
FILE		*ff;
char		imaf[64];
time_t		tmm;
char		strsql[1024];
int		kolstr=0;
SQL_str		row;
double 		sdeb=0.,skre=0.; //Стартовое сальдо по счёту
double		sndeb=0.,snkre=0.; //Сальдо на начало периода
double		odeb=0.,okre=0.; //Оборот за период
double		deb=0.,kre=0.;
short		metkasaldo=0;
class iceb_tu_str naim("");
SQLCURSOR cur;
short dn=0,mn=0,gn=0;
short dk=0,mk=0,gk=0;
class iceb_getkue_dat getval(rekrs->kod_val.ravno());

int vds=0;
clear();
GDITE();

time(&tmm);
sprintf(strsql,"select nais,vids from Plansh where ns='%s'",rekrs->shet.ravno());

if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,gettext("Не найден счёт %s в плане счетов !"),rekrs->shet.ravno());
  iceb_t_soob(strsql);
  return(1);
 }
vds=atoi(row[1]);
naim.new_plus(row[0]);
if(rsdatp(&dn,&mn,&gn,rekrs->dat1.ravno(),&dk,&mk,&gk,rekrs->dat2.ravno()) != 0)
  return(1);

printw("%s %s (%s)\n%d.%d.%d => %d.%d.%d\n",
rekrs->shet.ravno(),naim.ravno(),
gettext("Внебалансовий"),
dn,mn,gn,dk,mk,gk);





godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;

class iceb_tu_str start_god(godn);
start_god.plus("-01-01");

/*Узнаем начальное сальдо по счёту*/

/*Отчет по для многопорядкового плана счетов*/
if(vplsh == 1)
 sprintf(strsql,"select deb,kre from Saldo where kkk='0' and gs=%d and \
ns like '%s%%'",godn,rekrs->shet.ravno());

/*Отчет по субсчету для двух-порядкового плана счетов*/
if(vds == 1 && vplsh == 0)
 sprintf(strsql,"select deb,kre from Saldo where kkk='0' and gs=%d and \
ns='%s'",godn,rekrs->shet.ravno());

/*Отчет по счёту для двух-порядкового плана счетов*/
if(vds == 0 && vplsh == 0)
 sprintf(strsql,"select deb,kre from Saldo where kkk='0' and gs=%d and \
ns='%s%%'",godn,rekrs->shet.ravno());

//printw("%s\n",strsql);
//refresh();
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  sdeb+=getval.toue(row[0],start_god.ravno());
  skre+=getval.toue(row[1],start_god.ravno());

//  sdeb+=atof(row[0]);
//  skre+=atof(row[1]);
 }


printw("%s %.2f %.2f %d\n",
gettext("Стартовое сальдо"),sdeb,skre,godn);

sprintf(imaf,"gnbs%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }


sprintf(strsql,"%s %s %s (%s)",gettext("Журнала ордер по счёту"),
rekrs->shet.ravno(),naim.ravno(),gettext("Внебалансовый"));

iceb_t_zagolov(strsql,dn,mn,gn,dk,mk,gk,ff);
if(rekrs->kod_val.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Валюта"),rekrs->kod_val.ravno());

fprintf(ff,"\
-------------------------------------------------------------\n");
fprintf(ff,gettext("  Дата    |  Кто   |  Дебет   |  Кредит  | Коментарий\n"));

fprintf(ff,"\
-------------------------------------------------------------\n");

d2=1; m2=1; g2=godn;

do
 {
  if(metkasaldo == 0 && sravmydat(d2,m2,g2,dn,mn,gn) == 0)
   {
    
    metkasaldo=1;
//    printw("%f %f %f %f\n",sdeb,sndeb,skre,snkre);
    
    
    if(sdeb+sndeb > skre+snkre)
     {
      printw("%*s %10.2f\n",
      iceb_tu_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),
      (sdeb+sndeb)-(skre+snkre));
      refresh();
      fprintf(ff,"%*.*s %10.2f\n",
      iceb_tu_kolbait(19,gettext("Сальдо начальное")),iceb_tu_kolbait(19,gettext("Сальдо начальное")),gettext("Сальдо начальное"),
      (sdeb+sndeb)-(skre+snkre));
     }
    else
     {
      printw("%*s %10s %10.2f\n",
      iceb_tu_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),
      " ",(skre+snkre)-(sdeb+sndeb));
      refresh();
      fprintf(ff,"%*.*s %10s %10.2f\n",
      iceb_tu_kolbait(19,gettext("Сальдо начальное")),iceb_tu_kolbait(19,gettext("Сальдо начальное")),gettext("Сальдо начальное"),
      " ",(skre+snkre)-(sdeb+sndeb));
     }
   }

  //Для многопорядкового плана счетов
  if(vplsh == 1)
    sprintf(strsql,"select deb,kre,kto,ktoi,komen,datp from Prov \
where val=-1 and datp = '%d-%02d-%02d' and sh like '%s%%'",
    g2,m2,d2,rekrs->shet.ravno());

  /*Отчет по субсчету для двух-порядкового плана счетов*/
  if(vds == 1 && vplsh == 0)
    sprintf(strsql,"select deb,kre,kto,ktoi,komen,datp from Prov \
where val=-1 and datp = '%d-%02d-%02d' and sh='%s'",
    g2,m2,d2,rekrs->shet.ravno());

  /*Отчет по счёту для двух-порядкового плана счетов*/
  if(vds == 1 && vplsh == 0)
    sprintf(strsql,"select deb,kre,kto,ktoi,komen,datp from Prov \
where val=-1 and datp = '%d-%02d-%02d' and sh like '%s%%'",
    g2,m2,d2,rekrs->shet.ravno());

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

  if(kolstr > 0)
  while(cur.read_cursor(&row) != 0)
   {
    deb=getval.toue(row[0],row[5]);
    kre=getval.toue(row[1],row[5]);
//    deb=atof(row[0]);
//    kre=atof(row[1]);
    if(sravmydat(d2,m2,g2,dn,mn,gn) < 0)
     {
      sndeb+=deb;
      snkre+=kre;
     }
    if(sravmydat(d2,m2,g2,dn,mn,gn) >= 0)
     {
      odeb+=deb;
      okre+=kre;
      fprintf(ff,"%02d.%02d.%d %-4s%4s %10.2f %10.2f %s\n",
      d2,m2,g2,row[2],row[3],deb,kre,row[4]);
     }


   }

  dpm(&d2,&m2,&g2,1);

 }
while(sravmydat(d2,m2,g2,dk,mk,gk) <= 0);

fprintf(ff,"\
-------------------------------------------------------------\n");

printw("%*s %10.2f %10.2f\n",
iceb_tu_kolbait(20,gettext("Оборот за период")),gettext("Оборот за период"),
odeb,okre);
refresh();
fprintf(ff,"%*.*s %10.2f %10.2f\n",
iceb_tu_kolbait(19,gettext("Оборот за период")),iceb_tu_kolbait(19,gettext("Оборот за период")),gettext("Оборот за период"),
odeb,okre);

if(sdeb+sndeb+odeb > skre+snkre+okre)
 {
  printw("%*s %10.2f\n",
  iceb_tu_kolbait(20,gettext("Сальдо конечное")),gettext("Сальдо конечное"),
  (sdeb+sndeb+odeb)-(skre+snkre+okre));
  refresh();

  fprintf(ff,"%*.*s %10.2f\n",
  iceb_tu_kolbait(19,gettext("Сальдо конечное")),iceb_tu_kolbait(19,gettext("Сальдо конечное")),gettext("Сальдо конечное"),
  (sdeb+sndeb+odeb)-(skre+snkre+okre));
 }
else
 {
  printw("%*s %10s %10.2f\n",
  iceb_tu_kolbait(20,gettext("Сальдо конечное")),gettext("Сальдо конечное"),
  " ",(skre+snkre+okre)-(sdeb+sndeb+odeb));
  refresh();

  fprintf(ff,"%*.*s %10s %10.2f\n",
  iceb_tu_kolbait(19,gettext("Сальдо конечное")),iceb_tu_kolbait(19,gettext("Сальдо конечное")),gettext("Сальдо конечное"),
  " ",(skre+snkre+okre)-(sdeb+sndeb+odeb));
 }

podpis(ff);


fclose(ff);

printw_vr(tmm);




OSTANOV();
oth->spis_imaf.plus(imaf);
sprintf(strsql,"%s %s",gettext("Журнал-ордер по счёту"),rekrs->shet.ravno());
oth->spis_naim.plus(strsql);

iceb_t_ustpeh(oth->spis_imaf.ravno(0),1);

return(0);

}
