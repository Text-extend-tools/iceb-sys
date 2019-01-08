/*$Id: zarboln.c,v 5.6 2013/09/26 09:43:45 sasa Exp $*/
/*06.05.2011	06.05.2011	Белых А.И.	zarboln.c
Отчёт о больничных
*/
#include <errno.h>
#include "buhg.h"

int zarboln(class spis_oth *oth)
{
char strsql[512];
static class iceb_tu_str datan("");
static class iceb_tu_str datak("");

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт справки по больничным"));


VV.VVOD_SPISOK_add_data(datan.ravno(),8);
VV.VVOD_SPISOK_add_data(datak.ravno(),8);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.(м.г).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..(м.г).."));//1

naz:;
clear();

helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
//"F3",gettext("таб.номер"),
"F4",gettext("очистить"),
"F5",gettext("настройка"),
"F10",gettext("выход"),NULL);


int kom=VV.vvod(0,1,1,-1,-1);

datan.new_plus(VV.data_ravno(0));
datak.new_plus(VV.data_ravno(1));


switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   iceb_t_pdoc("zarp4_7_1_1.txt");
   goto naz;

  case FK2:
  case PLU:
    break;
      
  case FK4:
    VV.VVOD_clear_data();
    goto naz;

  case FK5 :  
    iceb_redfil("zarboln.alx",0);
    goto naz;

  case FK10:
  case ESC:
    return(1);

  default:
    goto naz;
 }
short mn,gn;
short mk,gk;

if(iceb_t_rsdatp1(&mn,&gn,datan.ravno(),&mk,&gk,datak.ravno()) != 0)
  goto naz;

class iceb_tu_str kodb_pred;
class iceb_tu_str kodb_fond;
class iceb_tu_str kodt_pred;
class iceb_tu_str kodt_fond;

if(iceb_t_poldan("Код начисления больничного за счёт предприятия",&kodb_pred,"zarboln.alx") != 0)
 {
  iceb_t_soob(gettext("Не найден \"Код начисления больничного за счёт предприятия\"!"));
  goto naz;
 }

if(iceb_t_poldan("Код начисления больничного за счёт фонда",&kodb_fond,"zarboln.alx") != 0)
 {
  iceb_t_soob(gettext("Не найден \"Код начисления больничного за счёт фонда\"!"));
  goto naz;
 }

if(iceb_t_poldan("Код табеля больничного за счёт предприятия",&kodt_pred,"zarboln.alx") != 0)
 {
  iceb_t_soob(gettext("Не найден \"Код табеля больничного за счёт предприятия\"!"));
  goto naz;
 }

if(iceb_t_poldan("Код табеля больничного за счёт фонда",&kodt_fond,"zarboln.alx") != 0)
 {
  iceb_t_soob(gettext("Не найден \"Код табеля больничного за счёт фонда\"!"));
  goto naz;
 }

if(kodb_pred.getdlinna() <= 1)
 {
  iceb_t_soob(gettext("Не введено \"Код начисления больничного за счёт предприятия\"!"));
  goto naz;
 }

if(kodb_fond.getdlinna() <= 1)
 {
  iceb_t_soob(gettext("Не введено \"Код начисления больничного за счёт фонда\"!"));
  goto naz;
 }


if(kodt_pred.getdlinna() <= 1)
 {
  iceb_t_soob(gettext("Не введено \"Код табеля больничного за счёт предприятия\"!"));
  goto naz;
 }

if(kodt_fond.getdlinna() <= 1)
 {
  iceb_t_soob(gettext("Не введено \"Код табеля больничного за счёт фонда\"!"));
  goto naz;
 }

int kolstr=0;
int kolstr1=0;
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select tabn,knah,suma,godn,mesn from Zarp where datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' \
and prn='1' and suma <> 0. order by tabn asc",gn,mn,gk,mk);

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

class SPISOK tabn_mesgod; /*табельный номер|месяц год в счёт которого выплата*/
class masiv_din_double pred; /*суммы за счёт предприятия*/
class masiv_din_double fond; /*Суммы за счёт фонда*/

char imaf[64];
FILE *ff;
int tabnom=0;

double suma=0.;
double suma_pred=0.;
double suma_fond=0.;
double isuma_pred=0.;
double isuma_fond=0.;
short mesn=0;
short godn=0;
int nomer_v_sp=0;


while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  tabnom=atoi(row[0]);
  suma=atof(row[2]);
  godn=atoi(row[3]);
  mesn=atoi(row[4]);
  
  suma_pred=0.;
  suma_fond=0.;


  if(proverka(kodb_pred.ravno(),row[1],0,0) == 0)
   suma_pred=suma;

  if(proverka(kodb_fond.ravno(),row[1],0,0) == 0)
   suma_fond=suma;

  if(suma_pred != 0. || suma_fond != 0.)
   {
    sprintf(strsql,"%d|%02d.%04d",tabnom,mesn,godn);
    if((nomer_v_sp=tabn_mesgod.find(strsql)) < 0)
      tabn_mesgod.plus(strsql);

    pred.plus(suma_pred,nomer_v_sp);
    fond.plus(suma_fond,nomer_v_sp);
    
   }
 }
GDITE();

class iceb_tu_str datan_pred;
class iceb_tu_str datak_pred;
class iceb_tu_str datan_fond;
class iceb_tu_str datak_fond;
class iceb_tu_str koment_pred;
class iceb_tu_str koment_fond;
int kol_dnr_pred=0;
int kol_dnk_pred=0;
int kol_dnr_fond=0;
int kol_dnk_fond=0;
class iceb_tu_str fio("");
class iceb_tu_str inn("");
/*Распечатка результата*/
sprintf(imaf,"boln%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
short dkm=1;
dpm(&dkm,&mk,&gk,5);

iceb_t_zagolov(gettext("Расчёт справки по больничным"),1,mn,gn,dkm,mk,gk,ff);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"\
 Т/н  |      Фамилия                 |Индивидуал| Дата  |           За счёт предприятия                    |                   За счёт фонда                  |\n\
      |                              |ьный нал. |       |--------------------------------------------------|--------------------------------------------------|\n\
      |                              | номер    |       | Сумма    |Период нетрудоспособн.|К.дн.|Коментарий|  Сумма   |Период нетрудоспособн.|К.дн.|Коментарий|\n");
 
/**********
123456 123456789012345678901234567890 1234567 1234567890 12345678901234567890 12345 1234567890 
***********/
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
for(int nom=0; nom < tabn_mesgod.kolih(); nom++)
 {
  polen(tabn_mesgod.ravno(nom),&tabnom,1,'|');
  polen(tabn_mesgod.ravno(nom),strsql,sizeof(strsql),2,'|');
  rsdat1(&mesn,&godn,strsql);

  suma_pred=pred.ravno(nom);      
  suma_fond=fond.ravno(nom);
  isuma_pred+=suma_pred;
  isuma_fond+=suma_fond;

  datan_pred.new_plus("");
  datak_pred.new_plus("");
  datan_fond.new_plus("");
  datak_fond.new_plus("");
  koment_pred.new_plus("");
  koment_fond.new_plus("");
  
  sprintf(strsql,"select datn,datk,dnei,kdnei,kom from Ztab where tabn=%d and god=%d and mes=%d and kodt=%d",tabnom,godn,mesn,kodt_pred.ravno_atoi());
  
  if(readkey(strsql,&row,&cur) == 1)
   {
    datan_pred.new_plus(row[0]);
    datak_pred.new_plus(row[1]);
    kol_dnr_pred=atoi(row[2]);
    kol_dnk_pred=atoi(row[3]);
    koment_pred.new_plus(row[4]);
   }
  
  sprintf(strsql,"select datn,datk,dnei,kdnei,kom from Ztab where tabn=%d and god=%d and mes=%d and kodt=%d",tabnom,godn,mesn,kodt_fond.ravno_atoi());
  if(readkey(strsql,&row,&cur) == 1)
   {
    datan_fond.new_plus(row[0]);
    datak_fond.new_plus(row[1]);
    kol_dnr_fond=atoi(row[2]);
    kol_dnk_fond=atoi(row[3]);
    koment_fond.new_plus(row[4]);
   }
  fio.new_plus("");
  inn.new_plus("");
  sprintf(strsql,"select fio,inn from Kartb where tabn=%d",tabnom);
  if(readkey(strsql,&row,&cur) == 1)
   {
    fio.new_plus(row[0]);
    inn.new_plus(row[1]);
   }     
  fprintf(ff,"%6d|%-*.*s|%-10s|%02d.%04d|%10.2f|%10s->%10s|%2d/%2d|%-*.*s|%10.2f|%10s->%10s|%2d/%2d|%-*.*s|\n",
  tabnom,
  iceb_tu_kolbait(30,fio.ravno()),
  iceb_tu_kolbait(30,fio.ravno()),
  fio.ravno(),
  inn.ravno(),
  mesn,godn,
  suma_pred,
  iceb_tu_datzap(datan_pred.ravno()),
  iceb_tu_datzap(datak_pred.ravno()),
  kol_dnr_pred,
  kol_dnk_pred,
  iceb_tu_kolbait(10,koment_pred.ravno()),  
  iceb_tu_kolbait(10,koment_pred.ravno()),  
  koment_pred.ravno(),
  suma_fond,
  iceb_tu_datzap(datan_fond.ravno()),
  iceb_tu_datzap(datak_fond.ravno()),
  kol_dnr_fond,
  kol_dnk_fond,
  iceb_tu_kolbait(10,koment_fond.ravno()),  
  iceb_tu_kolbait(10,koment_fond.ravno()),  
  koment_fond.ravno());
  
 }
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f %39s %10.2f\n",
iceb_tu_kolbait(56,gettext("Итого")),
gettext("Итого"),
isuma_pred,
"",
isuma_fond);

podpis(ff);
fclose(ff);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт справки по больничным"));

for(int nomer=0; nomer < oth->spis_imaf.kolih(); nomer++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nomer),1);
return(0);
}
