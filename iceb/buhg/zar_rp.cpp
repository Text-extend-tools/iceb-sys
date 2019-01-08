/*$Id: zar_rp.c,v 5.21 2013/09/26 09:43:45 sasa Exp $*/
/*28.03.2018	11.05.2007	Белых А.И.	zar_rp.c
Расчёт премии
*/
#include        <errno.h>
#include        "buhg.h"


void zar_rp()
{
char strsql[512];
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
static class iceb_tu_str podr("");
static class iceb_tu_str kategr("");
static class iceb_tu_str nahis("");
static class iceb_tu_str tabnom("");
static class iceb_tu_str proc_prem("");
class iceb_tu_str fiov("");
int kom1=0;
int tabn=0;

VVOD VV(0);
VVOD MENU(3);

if(dat1.ravno()[0] == '\0')
 {
  short dt,mt,gt;
  poltekdat(&dt,&mt,&gt);
  sprintf(strsql,"%d.%d",mt,gt);
  dat1.new_plus(strsql);
 }
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт премии"));

VV.VVOD_SPISOK_add_data(dat1.ravno(),8);
VV.VVOD_SPISOK_add_data(dat2.ravno(),8);
VV.VVOD_SPISOK_add_data(podr.ravno(),128);
VV.VVOD_SPISOK_add_data(kategr.ravno(),128);
VV.VVOD_SPISOK_add_data(nahis.ravno(),128);
VV.VVOD_SPISOK_add_data(tabnom.ravno(),128);
VV.VVOD_SPISOK_add_data(proc_prem.ravno(),16);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.....(м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца......(м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения.(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Код категории.....(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Код начисления...(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер..(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Процент премии........."));

naz:;

clear(); 

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

int kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
podr.new_plus(VV.data_ravno(2));
kategr.new_plus(VV.data_ravno(3));
nahis.new_plus(VV.data_ravno(4));
tabnom.new_plus(VV.data_ravno(5));
proc_prem.new_plus(VV.data_ravno(6));

class iceb_tu_str naim("");

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp4_3_15.txt");
   goto naz;

  case FK10:
  case ESC:
   return;

  case FK5:
  case FK2:
  case PLU:
   break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка категорий"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка начислений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка табельных номеров"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 6 :
      case -1:
       goto naz;

      case 0 :
        if(dirzarpodr(2,&podr,&naim) == 0)
         VV.data_z_plus(2,podr.ravno());
        goto naz;

      case 1 :
        vibrek("Kateg",&kategr,&naim);
        VV.VVOD_SPISOK_zapis_data(3,kategr.ravno());
        goto naz;

      case 2 :
        if(dirnach(1,&nahis,&naim) == 0)
         VV.VVOD_SPISOK_zapis_data(4,nahis.ravno());
        goto naz;


      case 3 :
      
        fiov.new_plus("");
        tabn=0;
        if(dirtab(&tabn,&fiov,0,1) == 0)
         {
          tabnom.z_plus(tabn);
          VV.VVOD_SPISOK_zapis_data(6,tabnom.ravno());
         }
        goto naz;

     }

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
    goto naz;
 }

GDITE();

short mn,gn;
short mk,gk;

if(rsdat1(&mn,&gn,VV.VVOD_SPISOK_return_data(0)) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата начала !"));
  goto naz;
 }

if(VV.VVOD_SPISOK_return_data(1)[0] == '\0')
 {
  mk=mn; gk=gn;
 }
else
 if(rsdat1(&mk,&gk,VV.VVOD_SPISOK_return_data(1)) != 0)
  {
   iceb_t_soob(gettext("Не верно введена дата конца !"));
   goto naz;
  }

//составляем список табельных номеров
sprintf(strsql,"select distinct tabn from Zarp where datz >= '%04d-%02d-01' and \
datz <= '%04d-%02d-31' and suma <> 0. order by tabn asc",gn,mn,gk,mk);

class SQLCURSOR cur;
int kolih_tabnom=0;

if((kolih_tabnom=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolih_tabnom == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;
 }

class masiv_din_int tab_nomer;
SQL_str row;

while(cur.read_cursor(&row) != 0)
 {
  tab_nomer.plus(atoi(row[0]));
 }

int kolih_mes=period(1,mn,gn,1,mk,gk,1);

class masiv_din_int mes_nah;
short d=1,m=mn,g=gn;

while(sravmydat(d,m,g,1,mk,gk) <= 0)
 {
  mes_nah.plus(m);
  dpm(&d,&m,&g,3);
 }
//Масив сумм 
class masiv_din_double suma_nah;
suma_nah.make_class(kolih_tabnom*kolih_mes);


sprintf(strsql,"select datz,tabn,knah,suma,podr from Zarp where datz >= '%04d-%02d-01' and \
datz <= '%04d-%02d-31' and prn='1' and suma <> 0.",gn,mn,gk,mk);

int kolstr=0;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
class SQLCURSOR cur1;
SQL_str row1;
int nomer_tabnom;
int nomer_mes;
int kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(tabnom.ravno(),row[1],0,0) != 0)
   continue;

  if(proverka(podr.ravno(),row[4],0,0) != 0)
   continue;

  if(proverka(nahis.ravno(),row[2],0,0) != 0)
   continue;
  if(kategr.ravno()[0] != '\0')
   {
    sprintf(strsql,"select kateg from Kartb where tabn=%s",row[1]);
    if(readkey(strsql,&row1,&cur1) == 1)
     if(proverka(kategr.ravno(),row1[0],0,0) != 0)
      continue;
   }   
  rsdat(&d,&m,&g,row[0],2);
  nomer_tabnom=tab_nomer.find(atoi(row[1]));
  nomer_mes=mes_nah.find(m);
  suma_nah.plus(atof(row[3]),nomer_tabnom*kolih_mes+nomer_mes);  
 }


FILE *ff;
char imaf[64];
sprintf(imaf,"rp%d.lst",getpid());
if((ff=fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(strsql,errno,"");
  return;
 }

short dkm;
dpm(&dkm,&mk,&gk,5);

iceb_t_zagolov(gettext("Расчёт премии"),1,mn,gn,dkm,mk,gk,ff);

if(podr.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Подразделение"),podr.ravno());
if(kategr.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Категория"),kategr.ravno());
if(nahis.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код начисления"),nahis.ravno());
if(tabnom.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Табельный номер"),tabnom.ravno());
if(proc_prem.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Процент премии"),proc_prem.ravno());



class iceb_tu_str str_pod;
str_pod.plus("--------------------------------------");
for(int ii=0; ii < kolih_mes+2; ii++)
 str_pod.plus("-----------");

fprintf(ff,"%s\n",str_pod.ravno());

fprintf(ff,"%s",gettext("  Т/н |       Фамилия                |"));

d=1;
g=gn;
for(nomer_mes=0; nomer_mes < kolih_mes; nomer_mes++)
 {
  m=mes_nah.ravno(nomer_mes);
  fprintf(ff,"%02d.%d   |",m,g);
  dpm(&d,&m,&g,3);
 }
fprintf(ff,"%s",gettext(" Итого    | Премия   |\n"));
fprintf(ff,"%s\n",str_pod.ravno());

class iceb_tu_str fio("");
double itogo_gor=0;
double suma;
double itogo_prem=0.;
double procent_prem=proc_prem.ravno_atof()/100.;

for(nomer_tabnom=0; nomer_tabnom < kolih_tabnom; nomer_tabnom++)
 {
  itogo_gor=0.;
  for(nomer_mes=0; nomer_mes < kolih_mes; nomer_mes++)
    itogo_gor+=suma_nah.ravno(nomer_tabnom*kolih_mes+nomer_mes);

  if(itogo_gor == 0.)
   continue;
   
  sprintf(strsql,"select fio from Kartb where tabn=%d",tab_nomer.ravno(nomer_tabnom));
  if(readkey(strsql,&row,&cur) == 1)
   fio.new_plus(row[0]);
  else
   fio.new_plus("");

  fprintf(ff,"%6d %-*.*s",
  tab_nomer.ravno(nomer_tabnom),
  iceb_tu_kolbait(30,fio.ravno()),
  iceb_tu_kolbait(30,fio.ravno()),
  fio.ravno());

  itogo_gor=0.;

  for(nomer_mes=0; nomer_mes < kolih_mes; nomer_mes++)
   {
    suma=suma_nah.ravno(nomer_tabnom*kolih_mes+nomer_mes);
    fprintf(ff," %10.2f",suma);
    itogo_gor+=suma;
   }
  fprintf(ff," %10.2f",itogo_gor);
  suma=itogo_gor*procent_prem;
  suma=okrug(suma,0.01);  
  fprintf(ff," %10.2f",suma);
  itogo_prem+=suma;
  fprintf(ff,"\n");     
 }
fprintf(ff,"%s\n",str_pod.ravno());
 
fprintf(ff,"%*s",iceb_tu_kolbait(37,gettext("Итого")),gettext("Итого"));

for(nomer_mes=0; nomer_mes < kolih_mes; nomer_mes++)
 {
  suma=0.;
  for(nomer_tabnom=0; nomer_tabnom < kolih_tabnom; nomer_tabnom++)
   suma+=suma_nah.ravno(nomer_tabnom*kolih_mes+nomer_mes);
  fprintf(ff," %10.2f",suma);
 }
fprintf(ff," %10.2f %10.2f\n",itogo_gor,itogo_prem);
fprintf(ff,"%s\n",str_pod.ravno());

podpis(ff);

fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Расчёт премии"));


iceb_t_rabfil(&oth,"");


}
