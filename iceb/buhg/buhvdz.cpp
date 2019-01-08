/*$Id: buhvdz.c,v 5.26 2013-04-18 09:26:03 sasa Exp $*/
/*13.07.2015	05.07.2006	Белых А.И.	buhvdz.c
Расчёт валових доходов и затрат.
*/

#include <errno.h>
#include "buhg.h"

void buhvdz_fn1(int metka_ras,short dn,short mn,short gn,short dk,short mk,short gk,class SPISOK *sp_sheta_deb,class SPISOK *sp_sheta_kre,int kolstr,class SQLCURSOR *cur,FILE *ff_dv,FILE *ff_it);
void buhvdz_spshet(const char *str_nast,class SPISOK *deb,class SPISOK *kre,class SPISOK *kontr);



int buhvdz(class spis_oth *oth)
{
static  class iceb_tu_str dat1(""),dat2("");

char strsql[512];

int kom=0,kom1=0;

VVOD MENU(3);

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт валовых доходов и затрат"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.....(д.м.г).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца......(д.м.г).."));//1 

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11); 

naz:;

clear();
 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
//"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F5",gettext("настройка"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("buhg5_12.txt");
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;

  case FK5:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Настройка расчёта валовых доходов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Настройка расчёта валовых затрат"));
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
        iceb_redfil("buhvdz_d.alx",0);
        break;
      case 1 :
        iceb_redfil("buhvdz_z.alx",0);
        break;
     }

   goto naz;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

MENU.VVOD_delete();

MENU.VVOD_SPISOK_add_MD(gettext("Расчёт валовых доходов и затрат"));
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт валовых доходов"));
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт валовых затрат"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

clearstr(LINES-1,0);

int var_ras=0;
while(menu3w(stdscr,&MENU,&var_ras,-1,-1,0) != 0);

if(var_ras == -1 || var_ras == 3)
 goto naz;



time_t time_n;
time(&time_n);

clear();
GDITE();

short dn,mn,gn;
short dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
 goto naz;



int kolstr=0;
SQL_str row;
class SQLCURSOR cur;

//Создаём список дебетовых счетов
sprintf(strsql,"select distinct sh from Prov where datp >= '%04d-%02d-%02d' \
and datp <= '%04d-%02d-%02d' and val=0 and deb <> 0. order by sh asc",gn,mn,dn,gk,mk,dk);

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

class SPISOK sp_sheta_deb;

while(cur.read_cursor(&row) != 0)
  sp_sheta_deb.plus(row[0]);

//Создаём список кредитовых счетов
sprintf(strsql,"select distinct shk from Prov where datp >= '%04d-%02d-%02d' \
and datp <= '%04d-%02d-%02d' and val=0 and deb <> 0. order by shk asc",gn,mn,dn,gk,mk,dk);

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

class SPISOK sp_sheta_kre;

while(cur.read_cursor(&row) != 0)
  sp_sheta_kre.plus(row[0]);



sprintf(strsql,"select sh,shk,deb,kodkon from Prov where datp >= '%04d-%02d-%02d' \
and datp <= '%04d-%02d-%02d' and val=0 and deb <> 0.",gn,mn,dn,gk,mk,dk);


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



char imaf_dv[64];
sprintf(imaf_dv,"buhvdz%d.lst",getpid());
FILE *ff_dv;

if((ff_dv=fopen(imaf_dv,"w")) == NULL) //Открываем файл с распечаткой
 {
  error_op_nfil(imaf_dv,errno,"");
  return(1);
 }


char imaf_it[64];
sprintf(imaf_it,"buhvdzi%d.lst",getpid());
FILE *ff_it;

if((ff_it=fopen(imaf_it,"w")) == NULL) //Открываем файл с распечаткой
 {
  error_op_nfil(imaf_it,errno,"");
  return(1);
 }


if(var_ras == 0 || var_ras == 1)
  buhvdz_fn1(0,dn,mn,gn,dk,mk,gk,&sp_sheta_deb,&sp_sheta_kre,kolstr,&cur,ff_dv,ff_it);

if(var_ras == 0 || var_ras == 2)
 {
  if(var_ras == 0)
   {
    fprintf(ff_dv,"\f");
    fprintf(ff_it,"\f");
   }
  buhvdz_fn1(1,dn,mn,gn,dk,mk,gk,&sp_sheta_deb,&sp_sheta_kre,kolstr,&cur,ff_dv,ff_it);
 }


podpis(ff_dv);
fclose(ff_dv);



podpis(ff_it);
fclose(ff_it);


printw_vr(time_n);

OSTANOV();

oth->spis_imaf.plus(imaf_it);
oth->spis_naim.plus(gettext("Свод по валовым доходам/затратам"));
oth->spis_imaf.plus(imaf_dv);
oth->spis_naim.plus(gettext("Расшифровка расчёта"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),0);
 
return(0);

}

/********************************/
/*Получить из строки настройки список счетов*/
/*****************************************/
void buhvdz_spshet(const char *str_nast,class SPISOK *deb,class SPISOK *kre,class SPISOK *kontr)
{
if(str_nast[0] == '\0')
 {
  deb->plus("");
  kre->plus("");
 }
int koltz=pole2(str_nast,';');
class iceb_tu_str nastr("");

if(koltz == 0)
 {
  if(pole2(str_nast,':') == 0)
   {
    deb->plus(str_nast);
    kre->plus("");
    kontr->plus("");
   }
  else
   {
    polen(str_nast,deb,1,':');
    polen(str_nast,kre,2,':');
    polen(str_nast,kontr,3,':');
   }     
  return;
 }

for(int ii=0; ii < koltz; ii++)
 {
  polen(str_nast,&nastr,ii+1,';');
  if(nastr.ravno()[0] == '\0')
   break;
  if(pole2(nastr.ravno(),':') == 0)
   {
    deb->plus(nastr.ravno());
    kre->plus("");
    kontr->plus("");
   }
  else
   {
    polen(nastr.ravno(),deb,1,':');
    polen(nastr.ravno(),kre,2,':');
    polen(nastr.ravno(),kontr,3,':');
   }     
  
 }

}



/*************************************/
/*Расчёт однопроходный*/
/***************************/

void buhvdz_fn1(int metka_ras, //0-расчёт валовых доходов 1-расчёт валовых затрат
short dn,short mn,short gn,
short dk,short mk,short gk,
class SPISOK *sp_sheta_deb,
class SPISOK *sp_sheta_kre,
int kolstr,
class SQLCURSOR *cur,
FILE *ff_dv,FILE *ff_it)
{
char strsql[512];
class iceb_tu_str imaf;
int kolstr_alx=0;
if(metka_ras == 0)
  imaf.new_plus("buhvdz_d.alx");
if(metka_ras == 1)
  imaf.new_plus("buhvdz_z.alx");

SQL_str row_alx;
class SQLCURSOR cur_alx;

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf.ravno());
if((kolstr_alx=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr_alx == 0)
 {
  sprintf(strsql,"Не найдены настройки %s",imaf.ravno());
  iceb_t_soob(strsql);
  return;
 }


//Вычисляем количество настроек
int kolih_nast=0;
class iceb_tu_str stroka1("");
class iceb_tu_str kod_dz("");
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(polen(row_alx[0],&stroka1,1,'|') != 0 )
    continue; 
  if(SRAV(stroka1.ravno(),"Код",0) != 0 )
   continue;

  polen(row_alx[0],&kod_dz,2,'|');

  if(kod_dz.ravno()[0] == '\0')
   continue;

  kolih_nast++;    
 }

class SPISOK sheta_deb[kolih_nast];
class SPISOK sheta_deb_p[kolih_nast]; //Персональные дебетовые счета для кредитового счета
class SPISOK spkontr_deb_p[kolih_nast]; /*Персональный список контрагентов*/

class SPISOK sheta_kre[kolih_nast];
class SPISOK sheta_kre_p[kolih_nast]; //персональные кредитовые счета для дебетового счёта
class SPISOK spkontr_kre_p[kolih_nast]; /*Персональный список контрагентов*/
class masiv_din_double sum_pr[kolih_nast]; //Массив сумм проводок

class iceb_tu_str naim_r("");
class SPISOK kod_dz_s;
class SPISOK naim_r_s;

int nomer_dz=0;
cur_alx.poz_cursor(0);
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(polen(row_alx[0],&stroka1,1,'|') != 0 )
    continue; 
  if(SRAV(stroka1.ravno(),"Код",0) != 0 )
   continue;
  else
   {

    polen(row_alx[0],&kod_dz,2,'|');
    if(kod_dz.ravno()[0] == '\0')
     continue;
    
    kod_dz_s.plus(kod_dz.ravno());    
    
    cur_alx.read_cursor(&row_alx);
    polen(row_alx[0],&naim_r,2,'|');
    naim_r_s.plus(naim_r.ravno());

    class iceb_tu_str shet_d;
    class iceb_tu_str shet_k;
    
    cur_alx.read_cursor(&row_alx);
    polen(row_alx[0],&stroka1,2,'|');
    iceb_t_fplus(stroka1.ravno(),&shet_d,&cur_alx);
       
    cur_alx.read_cursor(&row_alx);
    polen(row_alx[0],&stroka1,2,'|');
    iceb_t_fplus(stroka1.ravno(),&shet_k,&cur_alx);    


    buhvdz_spshet(shet_d.ravno(),&sheta_deb[nomer_dz],&sheta_kre_p[nomer_dz],&spkontr_deb_p[nomer_dz]);
    buhvdz_spshet(shet_k.ravno(),&sheta_kre[nomer_dz],&sheta_deb_p[nomer_dz],&spkontr_kre_p[nomer_dz]);
    
    
    
    nomer_dz++;  
   }
    
 }

for(int ii=0; ii < kolih_nast; ii++)
  sum_pr[ii].make_class(sp_sheta_deb->kolih()*sp_sheta_kre->kolih());

int nom_sd;
int nom_sk;
int kolstr1=0;
double suma=0.;
cur->poz_cursor(0);
int kolih_k=sp_sheta_kre->kolih();
SQL_str row;

while(cur->read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  for(int ii=0; ii < kolih_nast; ii++)
   {

    if((nom_sd=sheta_deb[ii].find(row[0],1)) < 0)
      continue;
    
    if(strlen(sheta_kre_p[ii].ravno(nom_sd)) > 0)
     {
      if(proverka(sheta_kre_p[ii].ravno(nom_sd),row[1],1,0) != 0)
        continue;
      if(proverka(spkontr_kre_p[ii].ravno(nom_sd),row[3],0,0) != 0)
        continue;
     }   
    else
     {
      if((nom_sk=sheta_kre[ii].find(row[1],1)) < 0)
       continue;
//      printw("row[1]=%s nom_sk=%d *%s\n",row[1],nom_sk,sheta_kre[ii].ravno(nom_sk));
      
      if(strlen(sheta_deb_p[ii].ravno(nom_sk)) > 0)
       {
        if(proverka(sheta_deb_p[ii].ravno(nom_sk),row[0],1,0) != 0)
          continue;
        if(proverka(spkontr_deb_p[ii].ravno(nom_sk),row[3],0,0) != 0)
          continue;
       }   
     }  
    
    suma=atof(row[2]);
    

    if((nom_sd=sp_sheta_deb->find(row[0],1)) < 0)
     continue;

    if((nom_sk=sp_sheta_kre->find(row[1],1)) < 0)
     continue;
  //  printw("%s %s %s\n",row[0],row[1],row[2]);
    
    sum_pr[ii].plus(suma,nom_sd*kolih_k+nom_sk);
  }  
 }

class iceb_tu_str zag_ras("");
iceb_t_poldan("Заголовок распечатки",&zag_ras,imaf.ravno());

printw("\n%s\n",zag_ras.ravno());

iceb_t_zagolov(zag_ras.ravno(),dn,mn,gn,dk,mk,gk,ff_dv);


iceb_t_zagolov(zag_ras.ravno(),dn,mn,gn,dk,mk,gk,ff_it);

fprintf(ff_it,"\
------------------------------------------------\n");

if(metka_ras == 0)
 fprintf(ff_it,gettext("\
 Код |Наименование статей доходов   |  Сумма   |\n"));

if(metka_ras == 1)
 fprintf(ff_it,gettext("\
 Код |Наименование статей затрат    |  Сумма   |\n"));

fprintf(ff_it,"\
------------------------------------------------\n");

/****** Распечатываем ******************/
double it=0;
for(int ii=0 ; ii < kolih_nast; ii++)
 {

  fprintf(ff_dv,"\n%s %s\n",kod_dz_s.ravno(ii),naim_r_s.ravno(ii));  

  //Распечатываем дебет по вертикали
  suma=buh_rh(0,sp_sheta_deb,sp_sheta_kre,&sum_pr[ii],ff_dv); 
  it+=suma;

  fprintf(ff_it,"%5s %-*.*s %10.2f\n",
  kod_dz_s.ravno(ii),
  iceb_tu_kolbait(30,naim_r_s.ravno(ii)),
  iceb_tu_kolbait(30,naim_r_s.ravno(ii)),
  naim_r_s.ravno(ii),
  suma);

  for(int nom=30; nom < iceb_tu_strlen(naim_r_s.ravno(ii)); nom+=30)
   fprintf(ff_it,"%5s %-*.*s\n",
   "",
   iceb_tu_kolbait(30,iceb_tu_adrsimv(nom,naim_r_s.ravno(ii))),
   iceb_tu_kolbait(30,iceb_tu_adrsimv(nom,naim_r_s.ravno(ii))),
   iceb_tu_adrsimv(nom,naim_r_s.ravno(ii)));

  printw("%2s %-*.*s %10.2f\n",
  kod_dz_s.ravno(ii),
  iceb_tu_kolbait(30,naim_r_s.ravno(ii)),
  iceb_tu_kolbait(30,naim_r_s.ravno(ii)),
  naim_r_s.ravno(ii),
  suma);  
  refresh();
  fprintf(ff_dv,"\n\n\n\n");  
  
 }
printw("%*s:%10.2f\n",iceb_tu_kolbait(33,gettext("Итого")),gettext("Итого"),it);
fprintf(ff_it,"\
------------------------------------------------\n");
fprintf(ff_it,"%*s %10.2f\n",iceb_tu_kolbait(36,gettext("Итого")),gettext("Итого"),it);
 
}
