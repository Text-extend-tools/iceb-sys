/*$Id: zarszk.c,v 5.13 2013/08/13 05:50:03 sasa Exp $*/
/*28.03.2018	09.03.2007	Белых А.И.	zarszk.c
Свод помесячной начисленной зарплаты и командировочных расходов
*/
#include <errno.h>
#include "buhg.h"

void zarszk_ps(int kolmes,FILE *ff);
void zarszk_itogo_gor(int podr,class masiv_din_int *spis_podr,int kolih_mes,int kolih_tab_nom,class masiv_din_double *suma_m,FILE *ff);


int zarszk(class spis_oth *oth)
{
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
static class iceb_tu_str podr("");
static class iceb_tu_str kod_kom("");
static class iceb_tu_str tabn("");
static class iceb_tu_str nahis("");
static class iceb_tu_str shet("");
int tabnom=0;
int kom1=0;
char strsql[512];


VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт общей суммы доходов"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.................(м.г).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..................(м.г).."));//1
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения............(,,).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Код командировочных расходов.(,,).."));//3
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер..............(,,).."));//4
VV.VVOD_SPISOK_add_MD(gettext("Код начисления...............(,,).."));//5
VV.VVOD_SPISOK_add_MD(gettext("Счёт.........................(,,).."));//6

VV.VVOD_SPISOK_add_data(dat1.ravno(),8);
VV.VVOD_SPISOK_add_data(dat2.ravno(),8); 
VV.VVOD_SPISOK_add_data(podr.ravno(),128);
VV.VVOD_SPISOK_add_data(kod_kom.ravno(),128);
VV.VVOD_SPISOK_add_data(tabn.ravno(),128);
VV.VVOD_SPISOK_add_data(nahis.ravno(),128);
VV.VVOD_SPISOK_add_data(shet.ravno(),128);

naz:;

clear();
 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

int kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
podr.new_plus(VV.data_ravno(2));
kod_kom.new_plus(VV.data_ravno(3));
tabn.new_plus(VV.data_ravno(4));
nahis.new_plus(VV.data_ravno(5));
shet.new_plus(VV.data_ravno(6));

class iceb_tu_str kod("");
class iceb_tu_str naim("");


switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp4_3_13.txt");
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;

  case FK3:
    
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка кодов командировочных расходов"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка начислений"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка табельных номеров"));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр плана счётов"));//4
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//5

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 5 :
      case -1:
       goto naz;


      case 0 :
        if(dirzarpodr(2,&podr,&naim) == 0)
         VV.data_z_plus(2,podr.ravno());
        goto naz;

      case 1:
        naim.new_plus("");
        if(dirkkr(&kod,&naim,1) == 0)
         VV.data_z_plus(3,kod.ravno());
        break;

      case 2:
        if(dirnach(1,&kod,&naim) == 0)
         VV.data_z_plus(5,kod.ravno());
        break;

      case 3:
        if(dirtab(&tabnom,&naim,0,1) == 0)
         VV.data_z_plus(4,tabnom);
        break;

      case 4 :
        vibrek("Plansh",&kod);
        VV.VVOD_SPISOK_zapis_data(6,kod.ravno());
        goto naz;
     }
    goto naz;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

short mn,gn;
short mk,gk;

if(iceb_t_rsdatp1(&mn,&gn,dat1.ravno(),&mk,&gk,dat2.ravno()) != 0)
 goto naz;

GDITE();

SQL_str row;
class SQLCURSOR cur;
int kolstr=0;
//Создаём список табельных номеров
sprintf(strsql,"select distinct tabn from Zarp where datz >= '%04d-%02d-01' \
and datz <= '%04d-%02d-31' and suma <> 0.",
gn,mn,gk,mk);

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


class masiv_din_int spis_tab_nom;
int kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
 
  spis_tab_nom.plus(atoi(row[0]));
 }

sprintf(strsql,"select distinct kont from Ukrdok where datd >= '%04d-%02d-01' \
and datd <= '%04d-%02d-31'",gn,mn,gk,mk);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
int dlina_prist=iceb_tu_strlen(ICEB_ZAR_PKTN);
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(iceb_tu_srav_r(ICEB_ZAR_PKTN,row[0],1) != 0)
   continue;


  tabnom=atoi(iceb_tu_adrsimv(dlina_prist,row[0]));

  if(proverka(tabn.ravno(),strsql,0,0) != 0)
   continue;
  if(spis_tab_nom.find(tabnom) < 0)
   spis_tab_nom.plus(tabnom);
 }
GDITE();
int kolih_tab_nom=spis_tab_nom.kolih();
int kolih_mes=period(1,mn,gn,1,mk,gk,1);

class masiv_din_double suma_zar;
class masiv_din_double suma_kom;

suma_zar.make_class(kolih_mes*kolih_tab_nom);
suma_kom.make_class(kolih_mes*kolih_tab_nom);



//читаем зарплату
sprintf(strsql,"select datz,tabn,suma,knah,shet from Zarp where datz >= '%04d-%02d-01' \
and datz <= '%04d-%02d-31' and prn='1' and suma <> 0.",
gn,mn,gk,mk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }


kolstr1=0;
int nomer_tabnom=0;
int nomer_mes=0;
double suma=0.;
short d,m,g;
short d1=1,m1=0,g1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(nahis.ravno(),row[3],0,0) != 0)
   continue;  
  if(proverka(shet.ravno(),row[4],0,0) != 0)
   continue;  

  rsdat(&d,&m,&g,row[0],2);
  tabnom=atoi(row[1]);
  suma=atof(row[2]);
  nomer_mes=0;  
  d1=1;
  m1=mn;
  g1=gn;
  while(sravmydat(1,m,g,1,m1,g1) > 0)
   {
    nomer_mes++;
    dpm(&d1,&m1,&g1,3);
   } 
  nomer_tabnom=spis_tab_nom.find(tabnom);  
  suma_zar.plus(suma,nomer_tabnom*kolih_mes+nomer_mes);
    
 }

GDITE();

//читаем командировочные расходы

sprintf(strsql,"select nomd,datd,kont from Ukrdok where datd >= '%04d-%02d-01' \
and datd <= '%04d-%02d-31'",gn,mn,gk,mk);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
kolstr1=0;
int kolstr2=0;
SQL_str row1;
class SQLCURSOR cur1;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(iceb_tu_srav_r(ICEB_ZAR_PKTN,row[2],1) != 0)
   continue;
  //Читаем содержимое документа
  sprintf(strsql,"select kodr,cena from Ukrdok1 where datd='%s' and nomd='%s'",row[1],row[0]);
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }

  suma=0.;
  while(cur1.read_cursor(&row1) != 0)
   {
    if(proverka(kod_kom.ravno(),row1[0],0,0) != 0)
     continue;
    suma+=atof(row1[1]);
   }

  tabnom=atoi(iceb_tu_adrsimv(dlina_prist,row[2]));
//  printw("row=%s strsql=%s tabnom=%d\n",row[2],strsql,tabnom);
//  OSTANOV();
  rsdat(&d,&m,&g,row[1],2);
  nomer_mes=0;  
  d1=1;
  m1=mn;
  g1=gn;
  while(sravmydat(1,m,g,1,m1,g1) > 0)
   {
    nomer_mes++;
    dpm(&d1,&m1,&g1,3);
   } 
  nomer_tabnom=spis_tab_nom.find(tabnom);  
  suma_kom.plus(suma,nomer_tabnom*kolih_mes+nomer_mes);
  
 }
GDITE();

//Узнаём в каких подразделениях числятся работники
class masiv_din_int spis_podr;
spis_podr.make_class(kolih_tab_nom);
for(int ii=0; ii < kolih_tab_nom; ii++)
 {
  sprintf(strsql,"select podr from Kartb where tabn=%d",spis_tab_nom.ravno(ii));
  if(readkey(strsql,&row,&cur) == 1)
   spis_podr.plus(atoi(row[0]),ii);
 }

//распечатка по подразделениям
sprintf(strsql,"select kod,naik from Podr order by kod asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

FILE *ff;
char imaf[64];
sprintf(imaf,"zarkom%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

short dkm=1;
dpm(&dkm,&mk,&gk,5);

iceb_t_zagolov(gettext("Расчёт общей суммы доходов"),1,mn,gn,dkm,mk,gk,ff);
if(podr.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код подразделения"),podr.ravno());
if(kod_kom.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код командировочных расходов"),kod_kom.ravno());
if(nahis.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код начисления"),nahis.ravno());
if(shet.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Счёт"),shet.ravno());
if(tabn.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Табельный номер"),tabn.ravno());
 
int podrr=0;
class iceb_tu_str fio("");
double itogo_gor=0.;
while(cur.read_cursor(&row) != 0)
 {
  if(proverka(podr.ravno(),row[0],0,0) != 0)
   continue;
    
  podrr=atoi(row[0]);  
  if(spis_podr.find(podrr) < 0) //Подразделения по которым нет начислений пропускаем
   continue;
  /*двойная ширина символа до конца строки*/
  fprintf(ff,"\x0E%s %s\n",row[0],row[1]);

  zarszk_ps(kolih_mes,ff);
  //распечатываем строку с месяцами  
  
  fprintf(ff,"Таб.н|       Фамилия                |");
  d=1;
  m=mn;
  g=gn;
  for(int ii=0; ii < kolih_mes; ii++)
   {
    fprintf(ff," %02d.%04d  |",m,g);
    dpm(&d,&m,&g,3);  
   }
  fprintf(ff,"%*s|\n",iceb_tu_kolbait(10,gettext("Итого")),gettext("Итого"));
  zarszk_ps(kolih_mes,ff);
  
  for(int ii=0; ii < kolih_tab_nom; ii++)
   {
    if(podrr != spis_podr.ravno(ii))
     continue;


    //читаем фамилию
    sprintf(strsql,"select fio from Kartb where tabn=%d",spis_tab_nom.ravno(ii));
    if(readkey(strsql,&row1,&cur1) == 1)
     fio.new_plus(row1[0]);
    else
     fio.new_plus("");

    fprintf(ff,"%5d %-*.*s",spis_tab_nom.ravno(ii),iceb_tu_kolbait(30,fio.ravno()),iceb_tu_kolbait(30,fio.ravno()),fio.ravno());
    itogo_gor=0.;
    for(int mes=0; mes < kolih_mes; mes++)
     {
      suma=suma_zar.ravno(ii*kolih_mes+mes);
      fprintf(ff," %10.2f",suma);
      itogo_gor+=suma;
     }
    fprintf(ff," %10.2f\n",itogo_gor);

    fprintf(ff,"%5s %-30.30s","","");

    itogo_gor=0.;
    for(int mes=0; mes < kolih_mes; mes++)
     {
      suma=suma_kom.ravno(ii*kolih_mes+mes);
      fprintf(ff," %10.2f",suma);
      itogo_gor+=suma;
     }
    fprintf(ff," %10.2f\n",itogo_gor);
   }
  zarszk_ps(kolih_mes,ff);

  zarszk_itogo_gor(podrr,&spis_podr,kolih_mes,kolih_tab_nom,&suma_zar,ff);
  zarszk_itogo_gor(podrr,&spis_podr,kolih_mes,kolih_tab_nom,&suma_kom,ff);

 }

podpis(ff);
fclose(ff);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт общей суммы доходов"));
iceb_t_ustpeh(imaf,3);

return(0);
}

/****************************/
/*Печать подчёркивающей строки*/
/******************************/
void zarszk_ps(int kolmes,FILE *ff)
{

fprintf(ff,"-------------------------------------");
//          1234567890123456789012345678901234567
for(int ii=0; ii <= kolmes; ii++)
 fprintf(ff,"-----------");
fprintf(ff,"\n");
}
/*************************/
/*Расчёт  суммы по колонке*/
/**************************/

double zarszk_sum_kol(int podr,class masiv_din_int *spis_podr,int nomer_kol,int kolih_mes,int kolih_tab_nom,class masiv_din_double *suma_m)
{
double itogo_kol=0.;
for(int ii=0; ii < kolih_tab_nom; ii++)
 {
  if(podr != spis_podr->ravno(ii))
   continue;
  itogo_kol+=suma_m->ravno(ii*kolih_mes+nomer_kol);  
 }
return(itogo_kol);
}
/**************************/
/*Печать горизонтальной строки с итогами по колонкам*/
/****************************************************/
void zarszk_itogo_gor(int podr,class masiv_din_int *spis_podr,int kolih_mes,int kolih_tab_nom,class masiv_din_double *suma_m,FILE *ff)
{
fprintf(ff,"%36s","");
double itogo_gor=0.;
double suma=0.;
for(int ii=0; ii < kolih_mes; ii++)
 {
  suma=zarszk_sum_kol(podr,spis_podr,ii,kolih_mes,kolih_tab_nom,suma_m);
  itogo_gor+=suma;
  fprintf(ff," %10.2f",suma);
 }
fprintf(ff," %10.2f\n",itogo_gor);
}
