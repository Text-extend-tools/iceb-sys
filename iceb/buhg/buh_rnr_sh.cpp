/*$Id: buh_rnr_sh.c,v 5.19 2013/09/26 09:43:29 sasa Exp $*/
/*13.07.2015	16.07.2007	Белых А.И.	buh_rnr_sh.c
Расчёт шахматки по заданному счёту
*/
#include <math.h>
#include <errno.h>
#include "buhg.h"

void buh_hps(class SPISOK *deb,class SPISOK *kre,class masiv_din_double *suma,class SPISOK *debsh,class SPISOK *kresh,class masiv_din_double *sumash);



int buh_rnr_sh(class spis_oth *oth)
{
static class iceb_tu_str shet("");
static class iceb_tu_str dat1(""),dat2("");
char strsql[512];

VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт шахматки по счёту"));

VV.VVOD_SPISOK_add_MD(gettext("Номер счета.................."));
VV.VVOD_SPISOK_add_MD(gettext("Дата начала........(д.м.г)..."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца.........(д.м.г)..."));

VV.VVOD_SPISOK_add_data(shet.ravno(),32);
VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);

naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);      


int kom=VV.vvod(0,1,1,-1,-1);

shet.new_plus(VV.data_ravno(0));
dat1.new_plus(VV.data_ravno(1));
dat2.new_plus(VV.data_ravno(2));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   prosf(strsql);
   goto naz;
 
  case FK2:
  case PLU:
    break;

  case FK3:
    shet.new_plus("");
    vibrek("Plansh",&shet);
    VV.data_plus(0,shet.ravno());

    goto naz;

  case FK4:
   VV.VVOD_clear_data();         
   goto naz;

  case FK10:
  case ESC:
    return(1);
 
  default:
   kom=0;
   goto naz;
 }

GDITE();

if(shet.ravno()[0] == '\0')
 {
  iceb_t_soob(gettext("Не введён счёт !"));
  goto naz;
 }

short dn,mn,gn;
short dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
  goto naz;

SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select nais,saldo,vids,stat from Plansh where ns='%s'",shet.ravno());

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,gettext("Не найден счёт %s в плане счетов !"),shet.ravno());
  iceb_t_soob(strsql);
  goto naz;
 }


int kolstr=0;
//узнаём список субсчетов заданого счёта
class SPISOK shet_r;

sprintf(strsql,"select distinct sh from Prov where datp >= '%04d-%02d-%02d' and datp <= '%04d-%02d-%02d' \
and sh like '%s%%' order by sh asc",gn,mn,dn,gk,mk,dk,shet.ravno());

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

  if(shet_r.find(row[0]) < 0)
   shet_r.plus(row[0]);  
 }

GDITE();
//узнаём список счетов корреспондентов по дебету
class SPISOK shet_rd;

sprintf(strsql,"select distinct shk from Prov where datp >= '%04d-%02d-%02d' and datp <= '%04d-%02d-%02d' \
and sh like '%s%%' and deb <> 0. and kre = 0. order by shk asc",gn,mn,dn,gk,mk,dk,shet.ravno());

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
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(shet_rd.find(row[0]) < 0)
   shet_rd.plus(row[0]);  
 }

GDITE();
//узнаём список счетов корреспондентов по кредиту
class SPISOK shet_rk;

sprintf(strsql,"select distinct shk from Prov where datp >= '%04d-%02d-%02d' and datp <= '%04d-%02d-%02d' \
and sh like '%s%%' and kre <> 0. and deb = 0. order by shk asc",gn,mn,dn,gk,mk,dk,shet.ravno());

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
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(shet_rk.find(row[0]) < 0)
   shet_rk.plus(row[0]);  
 }

//создаём массивы дебетов и кредитов
class masiv_din_double sum_deb;
class masiv_din_double sum_kre;

sum_deb.make_class(shet_r.kolih()*shet_rd.kolih());
sum_kre.make_class(shet_r.kolih()*shet_rk.kolih());

//заполняем массивы
GDITE();
sprintf(strsql,"select sh,shk,deb,kre from Prov where datp >= '%04d-%02d-%02d' and datp <= '%04d-%02d-%02d' \
and sh like '%s%%'",gn,mn,dn,gk,mk,dk,shet.ravno());

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
double deb=0.,kre=0.;
int nomer_shet_r;
int nomer_shet_k;

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  deb=atof(row[2]);
  kre=atof(row[3]);

  if(deb == 0. && kre == 0.) //бывает и такое
   continue;

  nomer_shet_r=shet_r.find(row[0]);

  if(deb != 0. && fabs(deb) > 0.009)
   {
    if((nomer_shet_k=shet_rd.find(row[1])) < 0)
     {
      sprintf(strsql,"Не найден счёт %s в списке дебетовых счетов !",row[1]);
      iceb_t_soob(strsql);
      continue;
     }
    
    sum_deb.plus(deb,nomer_shet_k*shet_r.kolih()+nomer_shet_r);
   }  
  else
   {
    if((nomer_shet_k=shet_rk.find(row[1])) < 0)
     {
      printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
      sprintf(strsql,"Не найден счёт %s в списке кредитовых счетов !",row[1]);
      iceb_t_soob(strsql);
      continue;
     }
    
    sum_kre.plus(kre,nomer_shet_k*shet_r.kolih()+nomer_shet_r);
   }  
 }

GDITE();

/*Распечатываем шахматку--------------------------------*/ 
char imaf_hax[64];
sprintf(imaf_hax,"haxg%d.lst",getpid());
class iceb_fopen ff_h;
if(ff_h.start(imaf_hax,"w") != 0)
 return(1);

zagolov(gettext("Расчёт шахматки по счёту"),dn,mn,gn,dk,mk,gk,iceb_t_get_pnk("00",0),ff_h.ff);

fprintf(ff_h.ff,"%s %s\n",gettext("Дебет счёта"),shet.ravno());


buh_rh(0,&shet_rd,&shet_r,&sum_deb,ff_h.ff);

fprintf(ff_h.ff,"\n\n%s %s\n",gettext("Кредит счёта"),shet.ravno());


buh_rh(0,&shet_rk,&shet_r,&sum_kre,ff_h.ff);



podpis(ff_h.ff);
ff_h.end();



char imaf_hax1[64];
sprintf(imaf_hax1,"haxv%d.lst",getpid());
if(ff_h.start(imaf_hax1,"w") != 0)
 return(1);

zagolov(gettext("Расчёт шахматки по счёту"),dn,mn,gn,dk,mk,gk,iceb_t_get_pnk("00",0),ff_h.ff);

fprintf(ff_h.ff,"%s %s\n",gettext("Дебет счёта"),shet.ravno());


buh_rh(1,&shet_rd,&shet_r,&sum_deb,ff_h.ff);

fprintf(ff_h.ff,"\n\n%s %s\n",gettext("Кредит счёта"),shet.ravno());


buh_rh(1,&shet_rk,&shet_r,&sum_kre,ff_h.ff);



podpis(ff_h.ff);
ff_h.end();

/*Распечатка шахматки по счетам*/
class SPISOK shet_r_s;

class SPISOK shet_rd_s;
class masiv_din_double suma_deb_s;

class SPISOK shet_rk_s;
class masiv_din_double suma_kre_s;

/*получение шахматки по счетам*/
buh_hps(&shet_rd,&shet_r,&sum_deb,&shet_rd_s,&shet_r_s,&suma_deb_s);

shet_r_s.free_class(); /*повторно создаётся*/

buh_hps(&shet_rk,&shet_r,&sum_kre,&shet_rk_s,&shet_r_s,&suma_kre_s);



char imaf_haxs[64];
sprintf(imaf_haxs,"haxs%d.lst",getpid());
if(ff_h.start(imaf_haxs,"w") != 0)
 return(1);
zagolov(gettext("Расчёт шахматки по счёту"),dn,mn,gn,dk,mk,gk,iceb_t_get_pnk("00",0),ff_h.ff);

fprintf(ff_h.ff,"%s %s\n",gettext("Дебет счёта"),shet.ravno());

buh_rh(0,&shet_rd_s,&shet_r_s,&suma_deb_s,ff_h.ff);

fprintf(ff_h.ff,"\n\n%s %s\n",gettext("Кредит счёта"),shet.ravno());


buh_rh(0,&shet_rk_s,&shet_r_s,&suma_kre_s,ff_h.ff);



podpis(ff_h.ff);
ff_h.end();






oth->spis_imaf.plus(imaf_hax);

sprintf(strsql,"%s %s %s 1 (%s)",
gettext("Шахматка по счёту"),
shet.ravno(),
gettext("форма"),
gettext("по субсчетам"));

oth->spis_naim.plus(strsql);

oth->spis_imaf.plus(imaf_hax1);

sprintf(strsql,"%s %s %s 2 (%s)",
gettext("Шахматка по счёту"),
shet.ravno(),
gettext("форма"),
gettext("по субсчетам"));

oth->spis_naim.plus(strsql);


oth->spis_imaf.plus(imaf_haxs);

sprintf(strsql,"%s %s (%s)",
gettext("Шахматка по счёту"),
shet.ravno(),
gettext("по счетам"));

oth->spis_naim.plus(strsql);

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),0);

return(0);
}



