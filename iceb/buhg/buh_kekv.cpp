/*$Id: buh_kekv.c,v 5.12 2013-04-07 10:22:41 sasa Exp $*/
/*13.07.2015	16.02.2008	Белых А.И.	buh_kekv.c
Расчёт оборота ко кекам
*/
#include <errno.h>
#include "buhg.h"

extern short	startgodb; /*Стартовый год*/

int buh_kekv(class spis_oth *oth)
{
static class iceb_tu_str dat1(""),dat2("");
static class iceb_tu_str shet("");

char strsql[512];

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт оборота по КЭКЗ"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт........(,,)....."));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11); 
VV.VVOD_SPISOK_add_data(shet.ravno(),128); 

naz:;

clear();
 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
//"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

int kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
shet.new_plus(VV.data_ravno(2));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   prosf(strsql);
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

short dn=0,mn=0,gn=0;
short dk=0,mk=0,gk=0;

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
 goto naz;

GDITE();

/*Определяем список кеков*/
class SPISOK sp_kekv;
short godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;

int kolstr=0;
SQL_str row;
class SQLCURSOR cur;

/*Узнаём список кеков*/

sprintf(strsql,"select distinct sh,kekv from Prov where datp >= '%04d-%02d-%02d' \
and datp <= '%04d-%02d-%02d' and val=0 and kekv != 0",godn,1,1,gk,mk,dk);

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
int kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(shet.ravno(),row[0],0,0) != 0)
   continue;
  if(sp_kekv.find(row[1]) < 0)
    sp_kekv.plus(row[1]);
 }
GDITE();

class masiv_din_double saldo_deb;
saldo_deb.make_class(sp_kekv.kolih());

class masiv_din_double saldo_kre;
saldo_kre.make_class(sp_kekv.kolih());

class masiv_din_double sum_kre;
sum_kre.make_class(sp_kekv.kolih());


class masiv_din_double sum_deb;
sum_deb.make_class(sp_kekv.kolih());


sprintf(strsql,"select datp,sh,deb,kre,kekv from Prov where datp >= '%04d-%02d-%02d' \
and datp <= '%04d-%02d-%02d' and val=0 and kekv != 0",godn,1,1,gk,mk,dk);

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
kolstr1=0;
double deb=0.;
double kre=0.;
short d,m,g;
int nomer_kekv;

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(shet.ravno(),row[1],0,0) != 0)
   continue;

  nomer_kekv=sp_kekv.find(row[4]);
  deb=atof(row[2]);
  kre=atof(row[3]);
  rsdat(&d,&m,&g,row[0],2);
  if(sravmydat(d,m,g,dn,mn,gn) < 0) /*До периода*/
   {
    saldo_deb.plus(deb,nomer_kekv);  
    saldo_kre.plus(kre,nomer_kekv);  
    continue;
   }  

  sum_deb.plus(deb,nomer_kekv);
  sum_kre.plus(kre,nomer_kekv);

 }
GDITE();
/*Распечатываем результат*/

char imaf[64];
sprintf(imaf,"kekv%d.lst",getpid());
FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

iceb_t_zagolov(gettext("Расчёт оборотов по КЭКЗ"),dn,mn,gn,dk,mk,gk,ff);
if(shet.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Счёт"),shet.ravno());
 
fprintf(ff,"\
------------------------------------------------------------------------\n");

fprintf(ff,"\
КЭКЗ |Сальдо на %02d.%02d.%04d |  Оборот за период   |Сальдо на %02d.%02d.%04d |\n\
     | Дебет    |  Кредит  |   Дебет  |  Кредит  | Дебет    |  Кредит  |\n",
dn,mn,gn,dk,mk,gk);

fprintf(ff,"\
------------------------------------------------------------------------\n");
double i_saldok_deb=0.;
double i_saldok_kre=0.;
double i_start_saldo_deb=0.;
double i_start_saldo_kre=0.;
for(nomer_kekv=0; nomer_kekv < sp_kekv.kolih(); nomer_kekv++)
 {
  deb=kre=0.;

  fprintf(ff,"%-5s ",sp_kekv.ravno(nomer_kekv));
  if(saldo_deb.ravno(nomer_kekv) > saldo_kre.ravno(nomer_kekv))
   {
    deb=saldo_deb.ravno(nomer_kekv)- saldo_kre.ravno(nomer_kekv);
    fprintf(ff,"%10.2f %10s",deb,"");
    i_start_saldo_deb+=deb;
   }
  else
   {
    kre=saldo_kre.ravno(nomer_kekv)-saldo_deb.ravno(nomer_kekv);
    fprintf(ff,"%10s %10.2f","",kre);
    i_start_saldo_kre+=kre;
   }

  fprintf(ff," %10.2f %10.2f",sum_deb.ravno(nomer_kekv),sum_kre.ravno(nomer_kekv));
  
  deb=deb+sum_deb.ravno(nomer_kekv);
  kre=kre+sum_kre.ravno(nomer_kekv);

  if(deb > kre)
   {
    
    fprintf(ff," %10.2f\n",deb-kre);
    printw("DEB %10.2f\n",deb-kre);
    i_saldok_deb+=deb-kre;
   }
  else
   {
    fprintf(ff," %10s %10.2f\n","",kre-deb);
    i_saldok_kre+=kre-deb;
   }
 }
fprintf(ff,"\
------------------------------------------------------------------------\n");

fprintf(ff,"%5s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
"",
i_start_saldo_deb,
i_start_saldo_kre,
sum_deb.suma(),
sum_kre.suma(),
i_saldok_deb,
i_saldok_kre);

podpis(ff);
fclose(ff);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт оборота по КЭКЗ"));
iceb_t_ustpeh(imaf,1);
return(0);
}

