/*$Id: zarpdoh.c,v 5.7 2013/09/26 09:43:45 sasa Exp $*/
/*28.03.2018	02.09.2011	Белых А.И.	zarpdoh.c
Перерасчёт дохода
*/
#include <errno.h>
#include "buhg.h"

double zarpdoh_rs(int tabn,short godr,double nahislenop[2][12],double min_zar,FILE *ff,FILE *ff_spis);


int  zarpdoh(class spis_oth *oth)
{
int kom1=0;
int tabn=0;
class iceb_tu_str fam("");
char strsql[512];
static class iceb_tu_str mn("");
static class iceb_tu_str mk("");
static class iceb_tu_str god("");
static class iceb_tu_str tabnom("");
static class iceb_tu_str podr("");
short mnr,mkr,godr;
class iceb_tu_str fiov("");
VVOD MENU(3);
VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0));
VV.VVOD_SPISOK_add_ZAG(gettext("Перерасчёт дохода"));

VV.VVOD_SPISOK_add_data(mn.ravno(),3);
VV.VVOD_SPISOK_add_data(mk.ravno(),3);
VV.VVOD_SPISOK_add_data(god.ravno(),5);
VV.VVOD_SPISOK_add_data(tabnom.ravno(),124);
VV.VVOD_SPISOK_add_data(podr.ravno(),124);

VV.VVOD_SPISOK_add_MD(gettext("Месяц начала............"));//0
VV.VVOD_SPISOK_add_MD(gettext("Месяц конца............."));//1
VV.VVOD_SPISOK_add_MD(gettext("Год....................."));//2
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер...(,,).."));//3
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения.(,,).."));//4

naz:;

clear();

helstr(LINES-1,0,\
//"F1",gettext("помощь"),
"F2/+",gettext("расчeт"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F5",gettext("настройка"),
"F10",gettext("выход"),NULL);

kom1=VV.vvod(0,1,1,-1,-1);

mn.new_plus(VV.data_ravno(0));
mk.new_plus(VV.data_ravno(1));
god.new_plus(VV.data_ravno(2));
tabnom.new_plus(VV.VVOD_SPISOK_return_data(3));
podr.new_plus(VV.VVOD_SPISOK_return_data(4));

switch(kom1)
 {
  case FK1:
    GDITE();
//    prosf(bros);
    goto naz;

  case FK2:
  case PLU:
   break;

  case FK10:
   return(1);

  case FK3:

    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка табельных номеров"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 2 :
      case -1:
       goto naz;

      case 0 :
        clear();
        tabn=0;
        fiov.new_plus("");
        if(dirtab(&tabn,&fiov,0,1) == 0)
         {
          sprintf(strsql,"%d",tabn);
          VV.data_z_plus(3,strsql);
         }
        
        break;

      case 1 :
        if(dirzarpodr(2,&podr,&fam) == 0)
         VV.data_z_plus(4,podr.ravno());
        break;
     }
    goto naz;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;

  case FK5:
   iceb_redfil("zarpdoh.alx",0);
   goto naz;

  default:
   goto naz;
 }

mnr=mn.ravno_atoi();
mkr=mk.ravno_atoi();
godr=god.ravno_atoi();

if(mnr == 0 || mnr > 11)
 {
  iceb_t_soob(gettext("Не верно введён месяц начала !"));
  goto naz;
 }

if(mkr == 0 || mkr > 12 || mkr < 2)
 {
  iceb_t_soob(gettext("Не верно введён месяц конца !"));

  goto naz;
 }

if(godr == 0)
 {
  iceb_t_soob(gettext("Не верно введён год !"));
  goto naz;
 }

GDITE();
class iceb_tu_str kod_nvr;
iceb_t_poldan("Коды не входящие в расчёт",&kod_nvr,"zarpdoh.alx");

double nahisleno[2][12];
memset(nahisleno,'\0',sizeof(nahisleno));
SQLCURSOR cur;
SQLCURSOR cur1;
int kolstr=0;
int kolstr1=0;
SQL_str row;
SQL_str row1;

sprintf(strsql,"select datz,tabn,knah,suma,mesn,godn,podr from Zarp where \
datz >= '%04d-%d-1' and datz <= '%04d-%d-31' and prn='1' and suma <> 0. \
order by tabn asc, datz asc",godr,mnr,godr,mkr);


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

char imaf_tabl[64];
FILE *ff_tabl;

sprintf(imaf_tabl,"%s%d.lst",__FUNCTION__,getpid());
if((ff_tabl = fopen(imaf_tabl,"w")) == NULL)
 {
  error_op_nfil(imaf_tabl,errno,"");
  return(1);
 }

char imaf_spis[64];
FILE *ff_spis;

sprintf(imaf_spis,"%s1%d.lst",__FUNCTION__,getpid());
if((ff_spis = fopen(imaf_spis,"w")) == NULL)
 {
  error_op_nfil(imaf_spis,errno,"");
  return(1);
 }

class zar_read_tn1h nast_mz;
zar_read_tn1(1,mnr,godr,&nast_mz);


zagolov(gettext("Перерасчёт дохода"),1,mnr,godr,31,mkr,godr,iceb_t_get_pnk("00",0),ff_tabl);
zagolov(gettext("Перерасчёт дохода"),1,mnr,godr,31,mkr,godr,iceb_t_get_pnk("00",0),ff_spis);
fprintf(ff_tabl,"\n");

if(tabnom.getdlinna() > 1)
 {
  fprintf(ff_spis,"%s:%s\n",gettext("Табельный номер"),tabnom.ravno());
  fprintf(ff_tabl,"%s:%s\n",gettext("Табельный номер"),tabnom.ravno());
 }

if(podr.getdlinna() > 1)
 {
  fprintf(ff_spis,"%s:%s\n",gettext("Подразделение"),podr.ravno());
  fprintf(ff_tabl,"%s:%s\n",gettext("Подразделение"),podr.ravno());
 }

if(kod_nvr.getdlinna() > 1)
 {
  fprintf(ff_spis,"%s:%s\n",gettext("Коды не входящие в расчёт"),kod_nvr.ravno());
  fprintf(ff_tabl,"%s:%s\n",gettext("Коды не входящие в расчёт"),kod_nvr.ravno());
  
 }
fprintf(ff_spis,"\
-------------------------------------------------\n");
fprintf(ff_spis,"\
 Т/н  |      Прізвище                |Сума приб.|\n");
fprintf(ff_spis,"\
-------------------------------------------------\n");

int tabnz=0;
short mes_vshet;
short god_vshet;
short d,m,g;
double suma=0.;
double suma_podoh=0.;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(tabnom.ravno(),row[1],0,0) != 0)
   continue;
  if(proverka(podr.ravno(),row[6],0,0) != 0)
   continue;
  if(proverka(kod_nvr.ravno(),row[2],0,1) == 0)
   continue;
  god_vshet=atoi(row[5]);
  if(god_vshet != godr)
   continue;

  tabn=atol(row[1]);

  if(tabnz != tabn)
   {

    if(tabnz != 0 )
     {
      suma_podoh+=zarpdoh_rs(tabnz,godr,nahisleno,nast_mz.minzar_ng,ff_tabl,ff_spis);
      memset(nahisleno,'\0',sizeof(nahisleno));      
     }

    sprintf(strsql,"select fio from Kartb where tabn=%d",tabn);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     {
      fam.new_plus(row1[0]);
      
     }
//    printw("%5d %s\n",tabn,fam.ravno());
    
    tabnz=tabn;
   }
  rsdat(&d,&m,&g,row[0],2);
  suma=atof(row[3]);
  mes_vshet=atoi(row[4]);

  nahisleno[0][m-1]+=suma;
  if(g == god_vshet)
      nahisleno[1][mes_vshet-1]+=suma;

 }

suma_podoh+=zarpdoh_rs(tabnz,godr,nahisleno,nast_mz.minzar_ng,ff_tabl,ff_spis);
fprintf(ff_spis,"\
-------------------------------------------------\n");
fprintf(ff_spis,"%*s:%10.2f\n",iceb_tu_kolbait(37,gettext("Итого")),gettext("Итого"),suma_podoh);


podpis(ff_tabl);
fclose(ff_tabl);
podpis(ff_spis);
fclose(ff_spis);

oth->spis_imaf.plus(imaf_tabl);
oth->spis_naim.plus(gettext("Перерасчёт доходов"));
oth->spis_imaf.plus(imaf_spis);
oth->spis_naim.plus(gettext("Общие итоги"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);

return(0);
}
/*********************************/
/*Вывод результата*/
/**********************************/
double zarpdoh_rs(int tabn,short godr,double nahisleno[2][12],double min_zar,FILE *ff_tabl,FILE *ff_spis)
{
double suma_podoh=0.;
int kolmes=0;
double itogo[2];
SQL_str row;
class SQLCURSOR cur;
class iceb_tu_str fio("");
char strsql[512];
sprintf(strsql,"select fio from Kartb where tabn=%d",tabn);
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  fio.new_plus(row[0]);

 }




memset(itogo,'\0',sizeof(itogo));

fprintf(ff_tabl,"\n%d %s\n",tabn,fio.ravno());

fprintf(ff_tabl,"\
------------------------------\n");

fprintf(ff_tabl,"\
 Дата  |Нараховано|Нар.в р.м.|\n");

fprintf(ff_tabl,"\
------------------------------\n");
for(int nom=0; nom < 12; nom++)
 {
  if(nahisleno[0][nom] == 0. && nahisleno[1][nom] == 0.)
   continue;

  if(nahisleno[1][nom] != 0.)
   kolmes++;

  fprintf(ff_tabl,"%02d.%04d|%10.2f|%10.2f|\n",nom+1,godr,nahisleno[0][nom],nahisleno[1][nom]);
  itogo[0]+=nahisleno[0][nom];
  itogo[1]+=nahisleno[1][nom];
    
 }

fprintf(ff_tabl,"\
------------------------------\n");

fprintf(ff_tabl,"%*s|%10.2f|%10.2f|\n\n",iceb_tu_kolbait(7,gettext("Итого")),gettext("Итого"),itogo[0],itogo[1]);
fprintf(ff_tabl,"Кількість місяців:%d\n",kolmes);

double suma_for_r=min_zar*10.;
fprintf(ff_tabl,"Cумма для расчёта %.2f*%.2f=%.2f\n",min_zar,10.,suma_for_r);

double sr_zar=itogo[1]/kolmes;
fprintf(ff_tabl,"Середня заробітна плата:%.2f/%d=%.2f\n",itogo[1],kolmes,sr_zar);

if(sr_zar < suma_for_r)
 {
  fprintf(ff_tabl,"%.2f < %.2f Розрахунок робити нетреба\n",sr_zar,suma_for_r);
 }
else
 {
  double proc=17./100.;
  double sumavr=sr_zar-suma_for_r;
  suma_podoh=sumavr*proc;
  suma_podoh=okrug(suma_podoh,0.01);
  fprintf(ff_tabl,"Сума прибуткового=(%2f-%.2f)*%.2f=%.2f\n",sr_zar,suma_for_r,proc,suma_podoh);
 } 
fprintf(ff_spis,"%6d|%-*.*s|%10.2f|\n",
tabn,
iceb_tu_kolbait(30,fio.ravno()),
iceb_tu_kolbait(30,fio.ravno()),
fio.ravno(),
suma_podoh);

return(suma_podoh);
}
