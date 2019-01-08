/*$Id: zaruoz.c,v 5.9 2013/08/13 05:50:03 sasa Exp $*/
/*20.04.2016	25.04.2007	Белых А.И.	zaruoz.c
Расчёт удержание однодневного заработка
*/
#include "buhg.h"

extern short kod_ud_dnev_zar; //Код удержания дневного заработка
extern short *kodnah_nvr_dnev_zar; //Коды начислений не входящие в расчёт удержания дневного заработка
extern short *kodtab_nvr_dnev_zar; //Коды табеля не входящие в расчёт удержания однодневного заработка
extern double   okrg;
extern char *shetb; /*Бюджетные счета начислений*/

void zaruoz(int tabnom,short mr,short gr,int podr,const char *uder_only)
{
char strsql[1024];

if(kod_ud_dnev_zar == 0)
 return;

sprintf(strsql,"\n%s-Расчёт удержания однодневного заработка %d %02d.%d\n\
---------------------------------------------------------\n",__FUNCTION__,kod_ud_dnev_zar,mr,gr); 
zar_pr_ins(strsql);

SQL_str row;
class SQLCURSOR cur;

class iceb_tu_str shet_uder("");
class iceb_tu_str shet_uderb("");
sprintf(strsql,"select shet,shetb from Uder where kod=%d",kod_ud_dnev_zar);

if(readkey(strsql,&row,&cur) == 1)
 {
  shet_uder.new_plus(row[0]);
  shet_uderb.new_plus(row[1]);
 } 
else
 {
  sprintf(strsql,"%s-Не найден код удержания %d в списке удержаний !",__FUNCTION__,kod_ud_dnev_zar);
  zar_pr_ins(strsql);
  return;
 }

if(proverka(uder_only,kod_ud_dnev_zar,0,0) != 0)
 {
  sprintf(strsql,"%s-Код %d исключён из расчёта\n",__FUNCTION__,kod_ud_dnev_zar);
  zar_pr_ins(strsql);
  return; 
 }

if(shet_uder.getdlinna() <= 1)
 {
  sprintf(strsql,"%s-Не введён счёт в справочнике удержаний для удержания %d\n",__FUNCTION__,kod_ud_dnev_zar);
  zar_pr_ins(strsql);
  return;
 }
 
double suma_nah=0.;
double suma_nahb=0.;

sprintf(strsql,"select knah,suma,shet from Zarp where \
datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' and tabn=%d and prn='1' and mesn=%d and \
suma <> 0. order by knah asc",gr,mr,gr,mr,tabnom,mr);


if(cur.make_cursor(&bd,strsql) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {

  if(provkod(kodnah_nvr_dnev_zar,row[0]) >= 0)
   continue;
  if(proverka(shetb,row[2],0,1) == 0)
   suma_nahb+=atof(row[1]);
  suma_nah+=atof(row[1]);

 }



sprintf(strsql,"Сумма взятая в расчёт=%.2f/%.2f\n",suma_nah,suma_nahb);
zar_pr_ins(strsql);

if(suma_nah <= 0.)
 return;
  
sprintf(strsql,"select kodt,dnei from Ztab where god=%d and mes=%d and tabn=%d",gr,mr,tabnom);
int kolstr;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
int kolih_dnei=0;
while(cur.read_cursor(&row) != 0)
 {
  if(provkod(kodtab_nvr_dnev_zar,row[0]) >= 0)
   continue;
  kolih_dnei+=atoi(row[1]);
 }

sprintf(strsql,"%s-Количество дней взятое в расчёт=%d\n",__FUNCTION__,kolih_dnei);
zar_pr_ins(strsql);

if(kolih_dnei == 0)
 return;
double suma_udb=0.;

if(suma_nahb != 0.)
 {
  suma_udb=suma_nahb/kolih_dnei; 
  suma_udb=okrug(suma_udb,okrg);
  sprintf(strsql,"%s-Бюджет-%.2f/%d=%.2f\n",__FUNCTION__,suma_nahb,kolih_dnei,suma_udb);
  zar_pr_ins(strsql);
  
 } 
double suma_ud=suma_nah/kolih_dnei-suma_udb;
suma_ud=okrug(suma_ud,okrg);

sprintf(strsql,"%s-Хозрасчёт-%.2f/%d-%.2f=%.2f\n",__FUNCTION__,suma_nah,kolih_dnei,suma_udb,suma_ud);
zar_pr_ins(strsql);
suma_ud*=-1;
suma_udb*=-1;

short d=1;
dpm(&d,&mr,&gr,5);
class ZARP     zp;
zp.dz=d;
zp.mesn=mr; zp.godn=gr;
zp.nomz=0;
zp.podr=podr;
strcpy(zp.shet,shet_uder.ravno());
zapzarp(d,mr,gr,tabnom,2,kod_ud_dnev_zar,suma_ud,shet_uder.ravno(),mr,gr,0,0,"",podr,"",zp); 
if(shet_uderb.getdlinna() > 1)
 {
  strcpy(zp.shet,shet_uderb.ravno());
  zapzarp(d,mr,gr,tabnom,2,kod_ud_dnev_zar,suma_udb,shet_uderb.ravno(),mr,gr,0,0,"",podr,"",zp); 
 } 
 
}
