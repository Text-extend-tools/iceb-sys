/*$Id: opss_u.c,v 5.30 2013/09/26 09:43:36 sasa Exp $*/
/*10.01.2018	20.03.2005	Белых А.И.	opss_u.c
Отчёт по счетам списания услуг
*/
#include        <errno.h>
#include "buhg.h"

void opss_h(int *nomlist,int *kol_str,FILE *ff);
void opss_itogo(int metka,const char *kod,double itog,FILE *ff,FILE *ffi);
void opps_shet_str(int *nomlist,int *kol_str,FILE *ff);
void opss_u_svod(FILE *ff,class SPISOK *shet_kodzap,class masiv_din_double *shet_kodzap_suma,class masiv_din_double *shet_kodzap_kolih);

extern double	okrcn;  /*Округление цены*/
extern double	okrg1;  /*Округление суммы*/
extern int kol_strok_na_liste;

int opss_u(int metka, //1-по счетам приобретения 2-по счетам списания
class spis_oth *oth)
{
static class iceb_tu_str dat1(""),dat2("");
static class iceb_tu_str sklad_p("");
static class iceb_tu_str shets_p(""); //Счёт списания 
static class iceb_tu_str shetu_p(""); //Счёт учёта
static class iceb_tu_str grupa_p("");
static class iceb_tu_str kodmat_p("");
static class iceb_tu_str kodop_p("");
static class iceb_tu_str kontr_p("");
static class iceb_tu_str artikul("");
class iceb_tu_str naik("");
class iceb_tu_str naim("");
class iceb_tu_str kod("");
int kodm;
int kgr;
char strsql[512];
int kom=0;
int kom1=0;

VVOD MENU(3);
VVOD VV(0);

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(shets_p.ravno(),512);
VV.VVOD_SPISOK_add_data(shetu_p.ravno(),512);
VV.VVOD_SPISOK_add_data(sklad_p.ravno(),128);
VV.VVOD_SPISOK_add_data(grupa_p.ravno(),128);
VV.VVOD_SPISOK_add_data(kodmat_p.ravno(),512);
VV.VVOD_SPISOK_add_data(kodop_p.ravno(),128);
VV.VVOD_SPISOK_add_data(kontr_p.ravno(),128);
VV.VVOD_SPISOK_add_data(artikul.ravno(),128);

if(metka == 1)
 VV.VVOD_SPISOK_add_ZAG(gettext("Распечатать движение услуг по счетам получения"));

if(metka == 2)
 VV.VVOD_SPISOK_add_ZAG(gettext("Распечатать движение услуг по счетам списания"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала...(д.м.г)..."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца....(д.м.г)..."));//1

if(metka == 1)
VV.VVOD_SPISOK_add_MD(gettext("Счёт приобретения.(,,).."));//2
if(metka == 2)
VV.VVOD_SPISOK_add_MD(gettext("Счёт списания...(,,)...."));//2

VV.VVOD_SPISOK_add_MD(gettext("Счёт учёта......(,,)...."));//3
VV.VVOD_SPISOK_add_MD(gettext("Подразделение...(,,)...."));//4
VV.VVOD_SPISOK_add_MD(gettext("Группа услуг....(,,)...."));//5
VV.VVOD_SPISOK_add_MD(gettext("Код услуги......(,,)...."));//6
VV.VVOD_SPISOK_add_MD(gettext("Код операции....(,,)...."));//7
VV.VVOD_SPISOK_add_MD(gettext("Код контрагента.(,,)...."));//8
VV.VVOD_SPISOK_add_MD(gettext("Артикул.........(,,)...."));//9



naz:;
clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,0,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
shets_p.new_plus(VV.data_ravno(2));
shetu_p.new_plus(VV.data_ravno(3));
sklad_p.new_plus(VV.data_ravno(4));
grupa_p.new_plus(VV.data_ravno(5));
kodmat_p.new_plus(VV.data_ravno(6));
kodop_p.new_plus(VV.data_ravno(7));
kontr_p.new_plus(VV.data_ravno(8));
artikul.new_plus(VV.data_ravno(9));

switch (kom)
 {

  case FK10:
  case ESC:
   return(1);

  case FK1:
    GDITE();
    iceb_t_pdoc("matu5_19.txt");
    clear();
    goto naz;

  case FK2:
  case PLU:
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп"));
    if(metka == 1)
      MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций приходов"));
    if(metka == 2)
      MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций расходов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка услуг"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
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
        vibrek("Uslpodr",&kod,&naim);
        VV.VVOD_SPISOK_zapis_data(4,kod.ravno());
        goto naz;

      case 1 :
        vibrek("Kontragent",&kod,&naim);
        VV.VVOD_SPISOK_zapis_data(8,kod.ravno());
        goto naz;

      case 2 :
        vibrek("Uslgrup",&kod,&naim);
        VV.VVOD_SPISOK_zapis_data(5,kod.ravno());
        goto naz;

      case 3 :
        if(metka == 1)
         {
          if(diruslop1(1,&kod,&naim) == 0)
            VV.data_z_plus(7,kod.ravno());
         }
        if(metka == 2)
         {
          if(diruslop2(1,&kod,&naim) == 0)
            VV.data_z_plus(7,kod.ravno());
         }
        goto naz;
        
      case 4 :
        naik.new_plus("");
        if(dirusl(&kgr,&kodm,&naik,1,"") == 0)
         {
          kodmat_p.z_plus(kodm);
          VV.VVOD_SPISOK_zapis_data(6,kodmat_p.ravno());
         }
        
        goto naz;

      case 5 :
        vibrek("Plansh",&kod,&naim);
        goto naz;

     }
    goto naz;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;
     
  default:
    goto naz;
 }
short dn,mn,gn;
short dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(0),&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(1)) != 0)
  goto naz;


sprintf(strsql,"select datd,podr,nomd,metka,kodzap,kolih,cena,ei,shsp,shetu,nz from Usldokum1 \
where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' and tp=%d order by shsp asc",
gn,mn,dn,gk,mk,dk,metka);

int kolstr;
SQL_str row;
SQLCURSOR cur;
SQL_str row1;
SQLCURSOR cur1;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }
class iceb_t_tmptab tabtmp;
const char *imatmptab={"opssuw"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
shsp char(24) not null,\
shuh char(24) not null,\
datd DATE not null,\
podr int not null,\
nomd char(24) not null,\
kodzap int not null,\
kolih double(16,6) not null,\
cena double(16,6) not null,\
ei char(24) not null,\
kodop char(24) not null,\
kontr char(24) not null,\
naim char(112) not null) ENGINE = MYISAM",imatmptab);

if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  

class iceb_tu_str kodop("");
class iceb_tu_str kontr("");
class iceb_tu_str shet_uheta("");
class iceb_tu_str shet_sp("");
double kolih;
short dd,md,gd;

int kolstr1=0;

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);  

  if(proverka(shets_p.ravno(),row[8],1,0) != 0)
    continue;  
  if(proverka(shetu_p.ravno(),row[9],1,0) != 0)
    continue;  
  if(proverka(sklad_p.ravno(),row[1],0,0) != 0)
    continue;  

  if(proverka(kodmat_p.ravno(),row[4],0,0) != 0)
    continue;  

  //читаем материал
  naim.new_plus("");
  if(row[3][0] == '0')
    sprintf(strsql,"select kodgr,naimat,artikul from Material where kodm=%s",row[4]);
  if(row[3][0] == '1')
    sprintf(strsql,"select kodgr,naius,art from Uslugi where kodus=%s",row[4]);

  if(readkey(strsql,&row1,&cur1) == 1)
   {
    naim.new_plus(row1[1]);
    if(proverka(grupa_p.ravno(),row1[0],0,0) != 0)
      continue;  
    if(proverka(artikul.ravno(),row1[2],0,0) != 0)
      continue;  

   }

  rsdat(&dd,&md,&gd,row[0],2);
  kolih=readkolu(dd,md,gd,atoi(row[1]),row[2],metka,atoi(row[3]),atoi(row[4]),atoi(row[10]));
  if(kolih == 0.)
   continue;  

  kodop.new_plus("");
  kontr.new_plus("");
  
  //читаем шапку документа
  sprintf(strsql,"select kontr,kodop from Usldokum where datd='%s' and podr=%s and \
nomd='%s' and tp=%d",row[0],row[1],row[2],metka);
  if(readkey(strsql,&row1,&cur1) == 1)
   {
    if(proverka(kodop_p.ravno(),row1[1],0,0) != 0)
      continue;  
    if(proverka(kontr_p.ravno(),row1[0],0,0) != 0)
      continue;  
    kodop.new_plus(row1[1]);
    kontr.new_plus(row1[0]);
   }

  shet_sp.new_plus(row[8]);
  if(shet_sp.ravno()[0] == '\0')
   shet_sp.new_plus("???");
  sprintf(strsql,"insert into %s values ('%s','%s','%s',%s,'%s',%s,%.10g,%s,'%s','%s','%s','%s')",
  imatmptab,
  shet_sp.ravno(),row[9],
  row[0],row[1],row[2],row[4],kolih,row[6],row[7],
  kodop.ravno(),kontr.ravno(),naim.ravno_filtr());

  sql_zapis(strsql,1,0);    


 }

sprintf(strsql,"select * from %s order by shsp asc",imatmptab);

int kolstrtmp=0;
if((kolstrtmp=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

char imaf[64];
FILE *ff;

sprintf(imaf,"opss%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

char imafi[64];
FILE *ffi;

sprintf(imafi,"opssi%d.lst",getpid());

if((ffi = fopen(imafi,"w")) == NULL)
 {
  error_op_nfil(imafi,errno,"");
  return(1);
 }

int kol_str=0;

char imaf_svod[64];
sprintf(imaf_svod,"opsss%d.lst",getpid());
FILE *ff_svod;
if((ff_svod = fopen(imaf_svod,"w")) == NULL)
 {
  error_op_nfil(imaf_svod,errno,"");
  return(1);
 }


if(metka == 1)
 {
  iceb_t_zagolov(gettext("Движение услуг по счетам получения"),dn,mn,gn,dk,mk,gk,ff);
  iceb_t_zagolov(gettext("Движение услуг по счетам получения"),dn,mn,gn,dk,mk,gk,ffi);
  iceb_t_zagolov(gettext("Движение услуг по счетам получения"),dn,mn,gn,dk,mk,gk,ff_svod);
 }
if(metka == 2)
 {
  iceb_t_zagolov(gettext("Движение услуг по счетам списания"),dn,mn,gn,dk,mk,gk,ff);
  iceb_t_zagolov(gettext("Движение услуг по счетам списания"),dn,mn,gn,dk,mk,gk,ffi);
  iceb_t_zagolov(gettext("Движение услуг по счетам списания"),dn,mn,gn,dk,mk,gk,ff_svod);
 }
kol_str=5;

if(sklad_p.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Склад"),sklad_p.ravno());
  fprintf(ffi,"%s:%s\n",gettext("Склад"),sklad_p.ravno());
  fprintf(ff_svod,"%s:%s\n",gettext("Склад"),sklad_p.ravno());
  kol_str++;
 }
if(shets_p.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Счёт списания"),shets_p.ravno());
  fprintf(ffi,"%s:%s\n",gettext("Счёт списания"),shets_p.ravno());
  fprintf(ff_svod,"%s:%s\n",gettext("Счёт списания"),shets_p.ravno());
  kol_str++;
 }
if(shetu_p.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Счёт учёта"),shetu_p.ravno());
  fprintf(ffi,"%s:%s\n",gettext("Счёт учёта"),shetu_p.ravno());
  fprintf(ff_svod,"%s:%s\n",gettext("Счёт учёта"),shetu_p.ravno());
  kol_str++;
 }
if(grupa_p.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Группа"),grupa_p.ravno());
  fprintf(ffi,"%s:%s\n",gettext("Группа"),grupa_p.ravno());
  fprintf(ff_svod,"%s:%s\n",gettext("Группа"),grupa_p.ravno());
  kol_str++;
 }
if(kodmat_p.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Код материалла"),kodmat_p.ravno());
  fprintf(ffi,"%s:%s\n",gettext("Код материалла"),kodmat_p.ravno());
  fprintf(ff_svod,"%s:%s\n",gettext("Код материалла"),kodmat_p.ravno());
  kol_str++;
 }
if(kodop_p.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Код операции"),kodop_p.ravno());
  fprintf(ffi,"%s:%s\n",gettext("Код операции"),kodop_p.ravno());
  fprintf(ff_svod,"%s:%s\n",gettext("Код операции"),kodop_p.ravno());
  kol_str++;
 }
if(kontr_p.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Контрагент"),kontr_p.ravno());
  fprintf(ffi,"%s:%s\n",gettext("Контрагент"),kontr_p.ravno());
  fprintf(ff_svod,"%s:%s\n",gettext("Контрагент"),kontr_p.ravno());
  kol_str++;
 }

if(artikul.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Артикул"),artikul.ravno());
  fprintf(ffi,"%s:%s\n",gettext("Артикул"),artikul.ravno());
  fprintf(ff_svod,"%s:%s\n",gettext("Артикул"),artikul.ravno());
  kol_str++;
 }

int nomlist=0;

//Шапка 

opss_h(&nomlist,&kol_str,ff);

fprintf(ffi,"----------------------\n");
fprintf(ffi,gettext("  Счёт    |   Сумма  |\n"));
fprintf(ffi,"----------------------\n");

double suma;
double cena;
class iceb_tu_str shet_sp_z("");
class iceb_tu_str shet_uheta_z("");
double itogo_shetu=0.;
double itogo_shets=0.;
double itogo_shets_o=0.;

class iceb_tu_str sklad("");
class iceb_tu_str nomd("");
class iceb_tu_str kodmat("");
class iceb_tu_str ei("");

class SPISOK shet_kodzap;
class masiv_din_double shet_kodzap_suma;
class masiv_din_double shet_kodzap_kolih;

int nomer_shet_kodzap=0;
while(cur.read_cursor(&row) != 0)
 {
//  iceb_pbar(data->bar,kolstrtmp,++kolstr1);    

  shet_sp.new_plus(row[0]);
  shet_uheta.new_plus(row[1]);
  rsdat(&dd,&md,&gd,row[2],2);
  sklad.new_plus(row[3]);
  nomd.new_plus(row[4]);
  kodmat.new_plus(row[5]);
  kolih=atof(row[6]);
  cena=atof(row[7]);
  ei.new_plus(row[8]);
  kodop.new_plus(row[9]);
  kontr.new_plus(row[10]);
  naim.new_plus(row[11]);

  if(SRAV(shet_uheta_z.ravno(),shet_uheta.ravno(),0) != 0)
   {
    if(shet_uheta_z.ravno()[0] != '\0')
     {

      opps_shet_str(&nomlist,&kol_str,ff);
      opss_itogo(1,shet_uheta_z.ravno(),itogo_shetu,ff,ffi);

      opps_shet_str(&nomlist,&kol_str,ff);
      fprintf(ff,"\n");
     }    
    itogo_shetu=0.;
    shet_uheta_z.new_plus(shet_uheta.ravno());
   }

  if(SRAV(shet_sp_z.ravno(),shet_sp.ravno(),0) != 0)
   {
    if(shet_sp_z.ravno()[0] != '\0')
     {

      opps_shet_str(&nomlist,&kol_str,ff);
      opss_itogo(1,shet_uheta_z.ravno(),itogo_shetu,ff,ffi);
      itogo_shetu=0.;

      opps_shet_str(&nomlist,&kol_str,ff);
      opss_itogo(2,shet_sp_z.ravno(),itogo_shets,ff,ffi);

      opps_shet_str(&nomlist,&kol_str,ff);
      fprintf(ff,"\n");
     }    
    itogo_shets_o+=itogo_shets;
    itogo_shets=0.;
    shet_sp_z.new_plus(shet_sp.ravno());
   }

  
  cena=okrug(cena,okrcn);
  
  suma=cena*kolih;
  suma=okrug(suma,okrg1);
  itogo_shetu+=suma;
  itogo_shets+=suma;  

  sprintf(strsql,"%s|%s",shet_sp.ravno(),kodmat.ravno());
  
  if((nomer_shet_kodzap=shet_kodzap.find(strsql)) < 0)
    shet_kodzap.plus(strsql);
  
  shet_kodzap_kolih.plus(kolih,nomer_shet_kodzap);
  shet_kodzap_suma.plus(suma,nomer_shet_kodzap);
  
  opps_shet_str(&nomlist,&kol_str,ff);

  fprintf(ff,"%-8s %-*.*s %-*s %-*s %10.2f %-*s %8.8g %10.10g %02d.%02d.%d %-9s %-5s %-*s %-*s\n",
  kodmat.ravno(),
  iceb_tu_kolbait(20,naim.ravno()),iceb_tu_kolbait(20,naim.ravno()),naim.ravno(),
  iceb_tu_kolbait(10,shet_sp.ravno()),shet_sp.ravno(),
  iceb_tu_kolbait(10,shet_uheta.ravno()),shet_uheta.ravno(),
  suma,
  iceb_tu_kolbait(6,ei.ravno()),ei.ravno(),
  kolih,cena,dd,md,gd,nomd.ravno(),sklad.ravno(),
  iceb_tu_kolbait(6,kodop.ravno()),kodop.ravno(),
  iceb_tu_kolbait(6,kontr.ravno()),kontr.ravno());

  for(int nom=20; nom < iceb_tu_strlen(naim.ravno()); nom+=20)
   {
    fprintf(ff,"%8s %-*.*s\n",
    "",
    iceb_tu_kolbait(20,iceb_tu_adrsimv(nom,naim.ravno())),
    iceb_tu_kolbait(20,iceb_tu_adrsimv(nom,naim.ravno())),
    iceb_tu_adrsimv(nom,naim.ravno()));
    
   }

 }

itogo_shets_o+=itogo_shets;

opss_itogo(1,shet_uheta_z.ravno(),itogo_shetu,ff,ffi);
opss_itogo(2,shet_sp_z.ravno(),itogo_shets,ff,ffi);
opss_itogo(3,"",itogo_shets_o,ff,ffi);

podpis(ff);
podpis(ffi);

fclose(ff);
fclose(ffi);

opss_u_svod(ff_svod,&shet_kodzap,&shet_kodzap_suma,&shet_kodzap_kolih);

fclose(ff_svod);


oth->spis_imaf.plus(imaf);
if(metka == 1)
 oth->spis_naim.plus(gettext("Движение услуг по счетам получения"));
if(metka == 2)
 oth->spis_naim.plus(gettext("Движение услуг по счетам списания"));
oth->spis_imaf.plus(imafi);
oth->spis_naim.plus(gettext("Общие итоги"));

oth->spis_imaf.plus(imaf_svod);
oth->spis_naim.plus(gettext("Свод по кодам услуг и счетам"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),0);

return(0);
 
}

/********************************/
/*шапка*/
/**********************************/

void opss_h(int *nomlist,int *kol_str,FILE *ff)
{
*nomlist+=1;
fprintf(ff,"%110s%s N%d\n","",gettext("Лист"),*nomlist);
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
  Код   |    Наименование    |Счёт спис.|   Счёт   |   Сумма  |Един. |Количec-|   Цена   |   Дата   |Номер    |Склад|Опера-|Контра-|\n\
материа.|     материалла     |/получения|  учёта   |          |измер.|  тво   |          | документа|документа|     |ция   |гент   |\n"));

fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------\n");
*kol_str+=5;


}

/*****************************/
/*Выдача итога*/
/*****************************/

void opss_itogo(int metka, //1-счёт учёта 2-счёт списания
const char *kod,double itog,FILE *ff,FILE *ffi)
{
if(itog == 0.)
 return;
 
char bros[512];
if(metka == 1)
  sprintf(bros,"%s %s",gettext("Итого по счёту учёта"),kod);

if(metka == 2)
 {
  sprintf(bros,"%s %s",gettext("Итого по счёту списания"),kod);
  fprintf(ffi,"%-10s %10.2f\n",kod,itog);
 }

if(metka == 3)
 {
  sprintf(bros,"%s",gettext("Общий итог"));
  fprintf(ffi,"%*.*s %10.2f\n",
  iceb_tu_kolbait(10,gettext("Итого")),
  iceb_tu_kolbait(10,gettext("Итого")),gettext("Итого"),itog);
 }
fprintf(ff,"%*s:%10.2f\n",iceb_tu_kolbait(51,bros),bros,itog);

}
/************************/
/*Счётчик строк*/
/*************************/

void opps_shet_str(int *nomlist,int *kol_str,FILE *ff)
{
*kol_str+=1;
if(*kol_str <= kol_strok_na_liste)
 return;

fprintf(ff,"\f");
*kol_str=0;
opss_h(nomlist,kol_str,ff);
*kol_str+=1;
}

/**************************************/
/*Распечатываем свод по услугам*/
/******************************/
void opss_u_svod(FILE *ff,class SPISOK *shet_kodzap,
class masiv_din_double *shet_kodzap_suma,
class masiv_din_double *shet_kodzap_kolih)
{

fprintf(ff,"\
-------------------------------------------------------------\n");

fprintf(ff,gettext("\
  Код   |    Наименование    |Счёт спис.|   Сумма  |Количec-|\n\
 услуги |      услуги        |/получения|          |  тво   |\n"));

/*
12345678 12345678901234567890 1234567890 1234567890 12345678
*/

fprintf(ff,"\
-------------------------------------------------------------\n");

class iceb_tu_str kodzap("");
class iceb_tu_str shet_spis("");
double suma;
double kolih;
class iceb_tu_str naim("");
SQL_str row1;
SQLCURSOR cur1;
char strsql[512];
double itogo_suma=0.;
double itogo_kolih=0.;
class iceb_tu_str shet_zap("");
double itogo_suma_shet=0.;
double itogo_kolih_shet=0.;
for(int i=0; i < shet_kodzap->kolih(); i++)
 {
  polen(shet_kodzap->ravno(i),&shet_spis,1,'|');
  polen(shet_kodzap->ravno(i),&kodzap,2,'|');

  if(SRAV(shet_zap.ravno(),shet_spis.ravno(),0) != 0)
   {
    if(shet_zap.ravno()[0] != '\0')
     {
      sprintf(strsql,"%s %s",gettext("Итого по счёту"),shet_zap.ravno());
      fprintf(ff,"%40s:%10.2f %8.2f\n",strsql,itogo_suma_shet,itogo_kolih_shet);
     }
    itogo_suma_shet=itogo_kolih_shet=0.;
    shet_zap.new_plus(shet_spis.ravno());
    
   }

  naim.new_plus("");
  
  //читаем материал
  sprintf(strsql,"select naius from Uslugi where kodus=%s",kodzap.ravno());
  if(readkey(strsql,&row1,&cur1) == 1)
   {
    naim.new_plus(row1[0]);
   }
  else
   {
    sprintf(strsql,"select naimat from Material where kodm=%s",kodzap.ravno());
    if(readkey(strsql,&row1,&cur1) == 1)
      naim.new_plus(row1[0]);
   } 
  suma=shet_kodzap_suma->ravno(i);
  kolih=shet_kodzap_kolih->ravno(i);
  
  itogo_suma+=suma;
  itogo_kolih+=kolih;
  
  itogo_suma_shet+=suma;
  itogo_kolih_shet+=kolih;
    
  fprintf(ff,"%-8s %-*.*s %-*s %10.2f %8.2f\n",
  kodzap.ravno(),
  iceb_tu_kolbait(20,naim.ravno()),iceb_tu_kolbait(20,naim.ravno()),naim.ravno(),
  iceb_tu_kolbait(10,shet_spis.ravno()),shet_spis.ravno(),
  suma,kolih);

  for(int nom=20; nom < iceb_tu_strlen(naim.ravno()); nom+=20)
   {
    fprintf(ff,"%8s %-*.*s\n",
    "",
    iceb_tu_kolbait(20,iceb_tu_adrsimv(nom,naim.ravno())),
    iceb_tu_kolbait(20,iceb_tu_adrsimv(nom,naim.ravno())),
    iceb_tu_adrsimv(nom,naim.ravno()));
    
   }
   
 }

sprintf(strsql,"%s %s",gettext("Итого по счёту"),shet_spis.ravno());
fprintf(ff,"%*s:%10.2f %8.2f\n",
iceb_tu_kolbait(40,strsql),strsql,
itogo_suma_shet,itogo_kolih_shet);

fprintf(ff,"\
-------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f %8.2f\n",iceb_tu_kolbait(40,gettext("Итого")),gettext("Итого"),itogo_suma,itogo_kolih);

podpis(ff);

}
