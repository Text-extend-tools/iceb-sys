/*$Id: uosrbs.c,v 5.18 2013/08/13 05:49:57 sasa Exp $*/
/*16.04.2017	03.11.2006	Белых А.И.	uosrbs.c
Расчёт балансовой стоимости по мат. ответственным и счетам учёта
*/
#include <errno.h>
#include "buhg.h"
#include "poiinpd.h"

void uosrbs_ras(class masiv_din_int *sp_kod_ol,class SPISOK *sp_shet,class masiv_din_double *masiv_sum,class masiv_din_int *m_kolih,FILE *ff);


int uosrbs(class spis_oth *oth)
{
static class iceb_tu_str dat1("");
static class iceb_tu_str kod_ol("");
static class iceb_tu_str shet_uh("");
char strsql[512];
int kom=0,kom1=0;




VVOD MENU(3);
VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт балансовой стоимости по мат.ответственным и счетам учёта"));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);//0
VV.VVOD_SPISOK_add_data(kod_ol.ravno(),128);//1
VV.VVOD_SPISOK_add_data(shet_uh.ravno(),128);//2

VV.VVOD_SPISOK_add_MD(gettext("Дата расчёта..(д.м.г)..."));//0
VV.VVOD_SPISOK_add_MD(gettext("Код мат.ответственного.."));//1
VV.VVOD_SPISOK_add_MD(gettext("Счёт...................."));//2

naz:;

clear();


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,0,-1,-1);

dat1.new_plus(VV.data_ravno(0));
kod_ol.new_plus(VV.data_ravno(1));
shet_uh.new_plus(VV.data_ravno(2));

class iceb_tu_str kod_v("");
class iceb_tu_str naim_v("");

switch (kom)
 {

  case FK10:
  case ESC:
   return(1);

  case FK1:
    GDITE();
//    prosf(bros);
    goto naz;

  case FK2:
  case PLU:
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка материально-ответвенных"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 2 :
      case -1:
       goto naz;


      case 0 :
        /*vibrek("Uosol",kod_ol,sizeof(kod_ol),naim);*/
        if(dirmatot(1,&kod_v,&naim_v,0) == 0)
         VV.data_z_plus(1,kod_v.ravno());
        goto naz;

      case 1 :
        vibrek("Plansh",&shet_uh);
        VV.VVOD_SPISOK_zapis_data(2,shet_uh.ravno());
        goto naz;


     }
    goto naz;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;
     
  default:
    goto naz;
 }
GDITE();

short dk,mk,gk;

if(rsdat(&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(0),1) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата !"));
  goto naz;
 }

SQL_str row;
class SQLCURSOR cur;
int kolstr=0;

//создаём список кодов материально ответственных
sprintf(strsql,"select kod from Uosol");
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
class masiv_din_int sp_kod_ol;

while(cur.read_cursor(&row) != 0)
 {
  if(proverka(kod_ol.ravno(),row[0],0,0) != 0)
   continue;
  sp_kod_ol.plus(atoi(row[0]));
 }
//создаём список счетов учёта
sprintf(strsql,"select distinct shetu from Uosinp");
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

class SPISOK sp_shet;
while(cur.read_cursor(&row) != 0)
 {
  if(proverka(shet_uh.ravno(),row[0],0,0) != 0)
   continue;
  sp_shet.plus(row[0]);
 }
/********
printw("%s:%d\n",gettext("Количество счетов"),sp_shet.kolih());
printw("%s:%d\n",gettext("Количество материально-ответственных"),sp_kod_ol.kolih());
***********/

//масив сумм
class masiv_din_double mas_sum;
//массив количества
class masiv_din_int mas_kolih;
mas_sum.make_class(sp_shet.kolih()*sp_kod_ol.kolih());
mas_kolih.make_class(sp_shet.kolih()*sp_kod_ol.kolih());

sprintf(strsql,"select innom from Uosin");
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
int podr;
int ikod_ol;
long inn;
class bsiz_data data_per;
int nomer_kod_ol=0;
int nomer_shet=0;

//Переменные данные по инвентарному номеру
class poiinpdw_data rekin;
int kol_shet=sp_shet.kolih();
while(cur.read_cursor(&row) != 0)
 {
  inn=atol(row[0]);
  if(poiin(inn,dk,mk,gk,&podr,&ikod_ol) != 0)
   continue;  

  sprintf(strsql,"%d",ikod_ol);
  if(proverka(kod_ol.ravno(),strsql,0,0) != 0)
    continue;

// if(poiinpd(inn,mk,gk,shetu,hzt,hau,hna,&popkf,&soso,nomz,&m1,&g1,hnaby,&popkfby) != 0)
  if(poiinpd(inn,mk,gk,&rekin) != 0)
   continue;

  if(proverka(shet_uh.ravno(),rekin.shetu.ravno(),0,0) != 0)
   continue;

  
  bsizp(inn,podr,0,1,1,gk,dk,mk,gk,&data_per,NULL);
  
  if((nomer_kod_ol=sp_kod_ol.find(ikod_ol)) < 0)
   {
    sprintf(strsql,"Не найден код ответственного лица %d в массиве !",ikod_ol);
    
    iceb_t_soob(strsql);
    continue;
   }
  
  if((nomer_shet=sp_shet.find(rekin.shetu.ravno())) < 0)
   {
    sprintf(strsql,"Не найден счёт %s в массиве !",rekin.shetu.ravno());
    
    iceb_t_soob(strsql);
    continue;
   }
  mas_sum.plus(data_per.bu.bs_kon_per,nomer_kod_ol*kol_shet+nomer_shet);
  mas_kolih.plus(1,nomer_kod_ol*kol_shet+nomer_shet);
  
 }

FILE *ff;
char imaf[64];
sprintf(imaf,"rbs%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

iceb_t_zagolov(gettext("Расчёт балансовой стоимости по мат.ответственным и счетам учёта"),0,0,0,dk,mk,gk,ff);
if(kod_ol.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Материально-ответственное лицо (лица)"),kod_ol.ravno());
if(shet_uh.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Счёт учёта"),shet_uh.ravno());
 


uosrbs_ras(&sp_kod_ol,&sp_shet,&mas_sum,&mas_kolih,ff);

podpis(ff);
fclose(ff);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт балансовой стоимости по мат.ответственным и счетам учёта"));

iceb_t_ustpeh(imaf,0);

return(0);

}

/*****************************/
/*Строка подчёркивания*/
/***************************/

void uosrbs_sp(int kol,FILE *ff)
{

fprintf(ff,"---------------------------");
//          123456789012345678901234567
for(int ii=0; ii < kol+1; ii++)
 fprintf(ff,"---------------------");
//           123456789012345678901
fprintf(ff,"\n");
}
/*******************************/
/*Распечатка */
/****************************/

void uosrbs_ras(class masiv_din_int *sp_kod_ol,class SPISOK *sp_shet,
class masiv_din_double *masiv_sum,
class masiv_din_int *m_kolih,
FILE *ff)
{
char strsql[512];
class iceb_tu_str naim("");
SQL_str row;
SQLCURSOR cur;


int kolih_shet=sp_shet->kolih();
int kolih_ol=sp_kod_ol->kolih();

uosrbs_sp(kolih_shet,ff); //Строка подчёркивания


fprintf(ff,"%-*.*s|%-*.*s|",
iceb_tu_kolbait(5,gettext("Код")),
iceb_tu_kolbait(5,gettext("Код")),
gettext("Код"),
iceb_tu_kolbait(20,gettext("Мат.ответственный")),
iceb_tu_kolbait(20,gettext("Мат.ответственный")),
gettext("Мат.ответственный"));

//Пишем первую строку со счетами
for(int ii=0; ii < kolih_shet; ii++)
 {
  //узнаём наименование 

  sprintf(strsql,"select nais from Plansh where ns='%s'",sp_shet->ravno(ii));  
  if(readkey(strsql,&row,&cur) == 1)
   naim.new_plus(row[0]);  
  else
   naim.new_plus("");

  sprintf(strsql,"%s %s",sp_shet->ravno(ii),naim.ravno());

  fprintf(ff,"%-*.*s|",iceb_tu_kolbait(20,strsql),iceb_tu_kolbait(20,strsql),strsql);
 }
fprintf(ff,"%-*.*s|",
iceb_tu_kolbait(20,gettext("Итого")),
iceb_tu_kolbait(20,gettext("Итого")),
gettext("Итого"));
fprintf(ff,"\n");

fprintf(ff,"     |                    |");

//строка разделения

for(int ii=0; ii < kolih_shet+1; ii++)
 {
  fprintf(ff,"---------------------");
 }
fprintf(ff,"\n");


fprintf(ff,"     |                    |");


for(int ii=0; ii < kolih_shet+1; ii++)
 {
  fprintf(ff,"%-*.*s|%-*.*s|",
  iceb_tu_kolbait(9,gettext("Количество")),
  iceb_tu_kolbait(9,gettext("Количество")),
  gettext("Количество"),
  iceb_tu_kolbait(10,gettext("Сумма")),
  iceb_tu_kolbait(10,gettext("Сумма")),
  gettext("Сумма"));
 }
fprintf(ff,"\n");

uosrbs_sp(kolih_shet,ff); //Строка подчёркивания

//Распечатываем массив
for(int s_kol=0; s_kol < kolih_ol; s_kol++)
 {
  //узнаём наименование 

  sprintf(strsql,"select naik from Uosol where kod=%d",sp_kod_ol->ravno(s_kol));  
  if(readkey(strsql,&row,&cur) == 1)
   naim.new_plus(row[0]);
  else
   naim.new_plus("");

  fprintf(ff,"%-5d|%-*.*s|",
  sp_kod_ol->ravno(s_kol),
  iceb_tu_kolbait(20,naim.ravno()),iceb_tu_kolbait(20,naim.ravno()),naim.ravno());

  int itog_str_k=0;
  double itog_str_s=0.;
  for(int s_shet=0; s_shet < kolih_shet; s_shet++)
   {
    int kolih=m_kolih->ravno(s_kol*kolih_shet+s_shet);
    double suma=masiv_sum->ravno(s_kol*kolih_shet+s_shet);

    if(kolih == 0.)
      fprintf(ff,"%9s|","");
    else
      fprintf(ff,"%9d|",kolih);

    if(suma == 0.)
      fprintf(ff,"%10s|","");
    else
      fprintf(ff,"%10.2f|",suma);
    itog_str_k+=kolih;
    itog_str_s+=suma;
   }
  fprintf(ff,"%9d|%10.2f|",itog_str_k,itog_str_s);
  fprintf(ff,"\n");    
   
 }

uosrbs_sp(kolih_shet,ff); //Строка подчёркивания

fprintf(ff,"%*s|",iceb_tu_kolbait(26,gettext("Итого")),gettext("Итого"));

//Распечатка итоговой строки
int itogo_po_kol_kolih=0;
double itogo_po_kol_suma=0.;
double itogo_k=0.;
double itogo_s=0.;

for(int ii=0; ii < kolih_shet; ii++)
 {
  itogo_po_kol_kolih=0;
  itogo_po_kol_suma=0.;
  for(int skontr=0; skontr < kolih_ol; skontr++)
   {
    itogo_po_kol_kolih+=m_kolih->ravno(skontr*kolih_shet+ii);
    itogo_po_kol_suma+=masiv_sum->ravno(skontr*kolih_shet+ii);
   }
  fprintf(ff,"%9d|%10.2f|",itogo_po_kol_kolih,itogo_po_kol_suma);
  itogo_k+=itogo_po_kol_kolih;
  itogo_s+=itogo_po_kol_suma;  
 }
fprintf(ff,"%9.9g|%10.2f|",itogo_k,itogo_s);
fprintf(ff,"\n");

}
