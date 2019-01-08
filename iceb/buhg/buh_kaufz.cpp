/*$Id: buh_kaufz.c,v 5.15 2013/09/26 09:43:29 sasa Exp $*/
/*22.11.2011	06.02.2008	Белых А.И.	buh_kaufz.c
Расчёт карточки аналитического учёта фактических затрат
*/


#include <errno.h>
#include "buhg.h"

void buh_kaufz_rs(class SPISOK *sp_dat_deb,class masiv_din_int *sp_kekv_deb,class masiv_din_double *sum_deb,class masiv_din_double *saldo_deb,FILE *ff);


extern short	startgodb; /*Стартовый год*/

int buh_kaufz(class spis_oth *oth)
{
char strsql[512];
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
static class iceb_tu_str kpk(""); /*код програмной классификации*/

int kom=0;

VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);

VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт карточки аналитического учёта кассовых расходов"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала..........(д.м.г)..."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца...........(д.м.г)..."));//1
VV.VVOD_SPISOK_add_MD(gettext("Код програмной классификации..."));//2

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(kpk.ravno(),16);

naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F4",gettext("очистить"),
"F5",gettext("настройка"),
"F10",gettext("выход"),NULL);      


kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
kpk.new_plus(VV.data_ravno(2));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   prosf(bros);
   goto naz;
 
  case FK2:
  case PLU:
    break;

  case FK4:
    VV.VVOD_clear_data();         
    goto naz;

  case FK5:
    iceb_redfil("buh_kaufz.alx",0);
    goto naz;

  case FK10:
  case ESC:
    return(1);
     
  default:
   kom=0;
   goto naz;
 }

GDITE();

short dn,mn,gn;
short dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
  goto naz;

if(kpk.ravno()[0] == '\0')
 {
  iceb_t_soob("Не введён код програмной классификации!");
  goto naz;
 }

int kolstr;
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select str from Alx where fil='buh_kaukz.alx' order by ns asc");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob("Не найдены настройки buh_kaukz.alx");
  return(1);
 }

int metka_kpk=0;
class iceb_tu_str kod_pk("");
class SPISOK deb_nastr;
class SPISOK kre_nastr;

while(cur.read_cursor(&row) != 0)
 {
  if(row[0][0] == '#')
   continue;
  if(polen(row[0],strsql,sizeof(strsql),1,'|') != 0)
   continue;
  
  if(SRAV("Код програмной классификации",strsql,0) == 0)
   {
    if(metka_kpk == 1)
     break;
    polen(row[0],&kod_pk,2,'|');
    if(SRAV(kpk.ravno(),kod_pk.ravno(),0) != 0)
     continue;

    metka_kpk=1;
    
   } 
  if(metka_kpk == 0)
   continue;

  if(row[0][0] == 'd')
   {
    deb_nastr.plus(row[0]);
    continue;
   }
  if(row[0][0] == 'k')
   {
    kre_nastr.plus(row[0]);
    continue;
   }
 }

if(metka_kpk == 0)
 {
  iceb_t_soob("Не нашли настройки для заданного кода програмной классификации!");
  goto naz;
 }


sprintf(strsql,"select datp,sh,shk,deb,kre,kekv,nomd from Prov where datp >= '%04d-%02d-%02d' and \
datp <= '%04d-%02d-%02d' order by datp asc,nomd asc",gk,1,1,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
class SPISOK sp_dat_nomd_deb; /*Список датa/номер документа*/
class SPISOK sp_dat_nomd_kre; /*Список дата/номер документа*/
class masiv_din_int sp_kekv_deb; /*Список кодов экономической классификации*/
class masiv_din_int sp_kekv_kre; /*Список кодов экономической классификации*/

double suma_deb=0.;
double suma_kre=0.;

int kolstr1=0;
short d,m,g;

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  suma_deb=atof(row[3]);
  suma_kre=atof(row[4]);
  if(suma_deb != 0.)
   {
    if(buh_prov_shet(row[1],row[2],&deb_nastr) != 0)
     continue;

    rsdat(&d,&m,&g,row[0],2);
    if(sravmydat(d,m,g,dn,mn,gn) >= 0)
     {
      sprintf(strsql,"%02d.%02d.%04d|%s",d,m,g,row[6]);

      if(sp_dat_nomd_deb.find(strsql) < 0)
        sp_dat_nomd_deb.plus(strsql);
     }


    if(sp_kekv_deb.find(row[5]) < 0)
      sp_kekv_deb.plus(row[5]);    
   }
  else
   {
    if(buh_prov_shet(row[1],row[2],&kre_nastr) != 0)
     continue;

    rsdat(&d,&m,&g,row[0],2);
    if(sravmydat(d,m,g,dn,mn,gn) >= 0)
     {
      sprintf(strsql,"%02d.%02d.%04d|%s",d,m,g,row[6]);

      if(sp_dat_nomd_kre.find(strsql) < 0)
        sp_dat_nomd_kre.plus(strsql);
     }

    if(sp_kekv_kre.find(row[5]) < 0)
      sp_kekv_kre.plus(row[5]);    
   }

 }
GDITE();
int kolih_kekv_deb=sp_kekv_deb.kolih();
int kolih_kekv_kre=sp_kekv_kre.kolih();

class masiv_din_double sum_deb; /*Суммы дебетов за период*/
class masiv_din_double sum_kre; /*Суммы кредитов за период*/

sum_deb.make_class(sp_dat_nomd_deb.kolih()*kolih_kekv_deb);
sum_kre.make_class(sp_dat_nomd_kre.kolih()*kolih_kekv_kre);

class masiv_din_double saldo_deb; /*Сальдо на начало периода*/
class masiv_din_double saldo_kre; /*Сальдо на начало периода*/
saldo_deb.make_class(kolih_kekv_deb);
saldo_kre.make_class(kolih_kekv_kre);

/*записываем суммы*/

cur.poz_cursor(0);
kolstr1=0;
int nomer_dat;
int nomer_kekv;

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  suma_deb=atof(row[3]);
  suma_kre=atof(row[4]);


  if(suma_deb != 0.)
   {
    if(buh_prov_shet(row[1],row[2],&deb_nastr) != 0)
     continue;

    

    nomer_kekv=sp_kekv_deb.find(row[5]);
    rsdat(&d,&m,&g,row[0],2);
    if(sravmydat(d,m,g,dn,mn,gn) < 0) /*До периода*/
     {
      saldo_deb.plus(suma_deb,nomer_kekv);  
      continue;
     }
    sprintf(strsql,"%02d.%02d.%04d|%s",d,m,g,row[6]);

    nomer_dat=sp_dat_nomd_deb.find(strsql);
    sum_deb.plus(suma_deb,nomer_dat*kolih_kekv_deb+nomer_kekv);
   }
  else
   {
    if(buh_prov_shet(row[1],row[2],&kre_nastr) != 0)
     continue;
    nomer_kekv=sp_kekv_kre.find(row[5]);
    rsdat(&d,&m,&g,row[0],2);
    if(sravmydat(d,m,g,dn,mn,gn) < 0) /*До периода*/
     {
      saldo_kre.plus(suma_kre,nomer_kekv);  
      continue;
     }

    sprintf(strsql,"%02d.%02d.%04d|%s",d,m,g,row[6]);

    nomer_dat=sp_dat_nomd_kre.find(strsql);
    sum_kre.plus(suma_kre,nomer_dat*kolih_kekv_kre+nomer_kekv);
   }

 }
GDITE();
class iceb_tu_str kod_edrpou("");

sprintf(strsql,"select kod from Kontragent where kodkon='00'");
if(readkey(strsql,&row,&cur) == 1)
 kod_edrpou.new_plus(row[0]);

/*Распечатываем результаты расчёта*/

FILE *ff;
char imaf[64];
sprintf(imaf,"kaufz%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

fprintf(ff,"%s\n",iceb_t_get_pnk("00",0));
fprintf(ff,"Ідентифікаційний код ЄДРПОУ %s\n",kod_edrpou.ravno());
fprintf(ff,"Код програмної класифікації %s\n",kpk.ravno());
fprintf(ff,"Вид коштів______________________\n\n");

fprintf(ff,"             Картка аналітичного обліку фактичних видатків\n");
fprintf(ff,"               за період з %02d.%02d.%d р. по %02d.%02d.%d р.\n\n",dn,mn,gn,dk,mk,gk);


buh_kaufz_rs(&sp_dat_nomd_kre,&sp_kekv_kre,&sum_kre,&saldo_kre,ff);


fprintf(ff,"\nЗворотний бік картки аналітичного обліку фактичних видатків\n");

buh_kaufz_rs(&sp_dat_nomd_deb,&sp_kekv_deb,&sum_deb,&saldo_deb,ff);


podpis(ff);
fclose(ff);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Карточка аналитического учёта фактических расходов"));
iceb_t_ustpeh(imaf,1);
return(0);
}




/*************************/
/*распечатка массива*/
/***************************/
void buh_kaufz_rs(class SPISOK *sp_dat_deb,
class masiv_din_int *sp_kekv_deb,
class masiv_din_double *sum_deb,
class masiv_din_double *saldo_deb,
FILE *ff)
{
int kolih_kekv_deb=sp_kekv_deb->kolih();

class iceb_tu_str liniq;
liniq.plus("---------------------------------");
for(int nomer_kekv=0; nomer_kekv < kolih_kekv_deb; nomer_kekv++)
 liniq.plus("-----------");

fprintf(ff,"%s\n",liniq.ravno());

fprintf(ff," Дата     |Ном.докум.|Усього    |");

for(int nomer_kekv=0; nomer_kekv < kolih_kekv_deb; nomer_kekv++)
 fprintf(ff,"%10d|",sp_kekv_deb->ravno(nomer_kekv));


fprintf(ff,"\n%s\n",liniq.ravno());

fprintf(ff,"                       Сальдо   |");

for(int nomer_kekv=0; nomer_kekv < kolih_kekv_deb; nomer_kekv++)
 fprintf(ff,"%10.2f|",saldo_deb->ravno(nomer_kekv));
fprintf(ff,"\n");

fprintf(ff,"%s\n",liniq.ravno());



double itogo_gor=0.;
double suma=0.;
for(int nomer_dat=0; nomer_dat < sp_dat_deb->kolih(); nomer_dat++)
 {
  for(int nomer_kekv=0; nomer_kekv < kolih_kekv_deb; nomer_kekv++)
    itogo_gor+=sum_deb->ravno(nomer_dat*kolih_kekv_deb+nomer_kekv);
  fprintf(ff,"%-21s|%10.2f|",sp_dat_deb->ravno(nomer_dat),itogo_gor);
  itogo_gor=0.;

  for(int nomer_kekv=0; nomer_kekv < kolih_kekv_deb; nomer_kekv++)
   {
    suma=sum_deb->ravno(nomer_dat*kolih_kekv_deb+nomer_kekv);
    if(suma != 0.)
      fprintf(ff,"%10.2f|",suma);
    else
      fprintf(ff,"%10s|","");
      
   }
  fprintf(ff,"\n");
 }
fprintf(ff,"%s\n",liniq.ravno());

//fprintf(ff,"За період |");
fprintf(ff,"          За період  |%10.2f|",sum_deb->suma());
double itogo_ver=0.;
/*Печатаем итоговую горизонтальную строку*/

for(int nomer_kekv=0 ; nomer_kekv < kolih_kekv_deb; nomer_kekv++)
 {
  itogo_ver=0.;
  for(int nomer_dat=0; nomer_dat < sp_dat_deb->kolih(); nomer_dat++)
   {
    itogo_ver+=sum_deb->ravno(nomer_dat*kolih_kekv_deb+nomer_kekv);
   }
  fprintf(ff,"%10.2f|",itogo_ver);
 }
fprintf(ff,"\n");
}
