/*$Id: buhsdkr.c,v 5.29 2013-04-18 09:26:03 sasa Exp $*/
/*13.07.2015	05.07.2006	Белых А.И.	buhsdkr.c
Расчёт по заданным спискам дебетовых и кредитовых счетов
*/

#include <errno.h>
#include "buhg.h"

double buhsdkr_r(short dn,short mn,short gn,short dk,short mk,short gk,class SPISOK *sp_sheta_deb,
class SPISOK *sp_sheta_kre,const char *shet_d,const char *shet_k,const char*,int kolstr,class SQLCURSOR *cur,FILE*,FILE*);


int buhsdkr(class spis_oth *oth)
{
static class iceb_tu_str dat1(""),dat2("");
static class iceb_tu_str variant_r("");

char strsql[512];

int kom=0;

VVOD MENU(3);

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт по заданным спискам дебетовых и кредитовых счетов"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.....(д.м.г).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца......(д.м.г).."));//1 
VV.VVOD_SPISOK_add_MD(gettext("Вариант расчёта....(,,).."));//2

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11); 
VV.VVOD_SPISOK_add_data(variant_r.ravno(),128); 

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
variant_r.new_plus(VV.data_ravno(2));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("buhg5_8.txt");
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;

  case FK5:
     iceb_redfil("buhsdkr.alx",0);
     goto naz;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }
time_t time_n;
time(&time_n);

clear();
GDITE();

short dn,mn,gn;
short dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
 goto naz;


class iceb_tu_str zag_ras("");
iceb_t_poldan("Заголовок распечатки",&zag_ras,"buhsdkr.alx");

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








sprintf(strsql,"select sh,shk,deb,kre from Prov where datp >= '%04d-%02d-%02d' \
and datp <= '%04d-%02d-%02d' and val=0",gn,mn,dn,gk,mk,dk);


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

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstralx=0;
sprintf(strsql,"select str from Alx where fil='buhsdkr.alx' order by ns asc");
if((kolstralx=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstralx == 0)
 {
  iceb_t_soob("Не найдены настройки buhsdkr.alx");
  return(1);
 }


char imaf_dv[64];
sprintf(imaf_dv,"sdkr%d.lst",getpid());
FILE *ff_dv;

if((ff_dv=fopen(imaf_dv,"w")) == NULL) //Открываем файл с распечаткой
 {
  error_op_nfil(imaf_dv,errno,"");
  return(1);
 }
iceb_t_zagolov(zag_ras.ravno(),dn,mn,gn,dk,mk,gk,ff_dv);

char imaf_kv[64];
sprintf(imaf_kv,"skdr%d.lst",getpid());
FILE *ff_kv;

if((ff_kv=fopen(imaf_kv,"w")) == NULL) //Открываем файл с распечаткой
 {
  error_op_nfil(imaf_kv,errno,"");
  return(1);
 }
iceb_t_zagolov(zag_ras.ravno(),dn,mn,gn,dk,mk,gk,ff_kv);

char imaf_it[64];
sprintf(imaf_it,"skdri%d.lst",getpid());
FILE *ff_it;

if((ff_it=fopen(imaf_it,"w")) == NULL) //Открываем файл с распечаткой
 {
  error_op_nfil(imaf_it,errno,"");
  return(1);
 }
iceb_t_zagolov(gettext("Свод по вариантам расчёта"),dn,mn,gn,dk,mk,gk,ff_it);

fprintf(ff_it,"\
------------------------------------------------\n");
fprintf(ff_it,gettext("\
 Код |     Наименование             |  Сумма   |\n"));
fprintf(ff_it,"\
------------------------------------------------\n");

class iceb_tu_str stroka1("");
class iceb_tu_str variant("");
class iceb_tu_str naim_r("");
class iceb_tu_str shet_k_minus;

double itogo_var=0.;
double it=0.;
class iceb_tu_str shet_d;
class iceb_tu_str shet_k;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(polen(row_alx[0],&stroka1,1,'|') != 0 )
    continue; 
  if(SRAV(stroka1.ravno(),"Вариант расчёта",0) != 0 )
   continue;
  else
   {

    polen(row_alx[0],&variant,2,'|');
    if(variant.ravno()[0] == '\0')
     continue;
    
    printw("%s",row_alx[0]);
    
    
    if(proverka(variant_r.ravno(),variant.ravno(),0,0) != 0)
     continue;

    
    cur_alx.read_cursor(&row_alx);
    printw("%s",row_alx[0]);
    polen(row_alx[0],&naim_r,2,'|');
    
    shet_d.new_plus("");
    shet_k.new_plus("");
    shet_k_minus.new_plus("");
            
    cur_alx.read_cursor(&row_alx);
    printw("%s",row_alx[0]);
    polen(row_alx[0],&stroka1,2,'|');
    iceb_t_fplus(stroka1.ravno(),&shet_d,&cur_alx);    

    cur_alx.read_cursor(&row_alx);
    printw("%s",row_alx[0]);
    polen(row_alx[0],&stroka1,2,'|');
    iceb_t_fplus(stroka1.ravno(),&shet_k,&cur_alx);
        
    cur_alx.read_cursor(&row_alx);
    
    if(SRAV("Отнять кредит для счетов",row_alx[0],1) == 0)
     {
      polen(row_alx[0],&stroka1,2,'|');
      iceb_t_fplus(stroka1.ravno(),&shet_k_minus,&cur_alx);
     }      

    printw("%s",row_alx[0]);
    refresh();

    fprintf(ff_dv,"\n%s\n",naim_r.ravno());  
    fprintf(ff_kv,"\n%s\n",naim_r.ravno());  
    
    itogo_var=buhsdkr_r(dn,mn,gn,dk,mk,gk,&sp_sheta_deb,&sp_sheta_kre,shet_d.ravno(),shet_k.ravno(),shet_k_minus.ravno(),kolstr,&cur,ff_dv,ff_kv);

    fprintf(ff_it,"%*s %-*.*s %10.2f\n",iceb_tu_kolbait(5,variant.ravno()),variant.ravno(),iceb_tu_kolbait(30,naim_r.ravno()),iceb_tu_kolbait(30,naim_r.ravno()),naim_r.ravno(),itogo_var);
    it+=itogo_var;    
      
    fprintf(ff_dv,"\n\n\n\n");  
    fprintf(ff_kv,"\n\n\n\n");  
     
   }
    
 }

fprintf(ff_it,"\
------------------------------------------------\n");
fprintf(ff_it,"%*s %10.2f\n",iceb_tu_kolbait(36,gettext("Итого")),gettext("Итого"),it);

podpis(ff_dv);
fclose(ff_dv);

podpis(ff_kv);
fclose(ff_kv);




podpis(ff_it);
fclose(ff_it);


printw_vr(time_n);

OSTANOV();
oth->spis_imaf.plus(imaf_dv);
oth->spis_naim.plus(gettext("Дебеты в таблице по вертикали"));
oth->spis_imaf.plus(imaf_kv);
oth->spis_naim.plus(gettext("Кредити в таблице по вертикали"));
oth->spis_imaf.plus(imaf_it);
oth->spis_naim.plus(gettext("Сводная ведомость по счетам"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),0);

return(0);
}

/*******************************/
/*Расчёт по заданным настройкам*/
/********************************/

double buhsdkr_r(short dn,short mn,short gn,
short dk,short mk,short gk,
class SPISOK *sp_sheta_deb,
class SPISOK *sp_sheta_kre,
const char *shet_d,const char *shet_k,const char *shet_k_minus,
int kolstrp,
class SQLCURSOR *curp,
FILE *ff_dv,FILE *ff_kv)
{
SQL_str row;
class SPISOK sheta_deb;
class SPISOK sheta_kre;

for(int ii=0; ii < sp_sheta_deb->kolih(); ii++)
 {
  if(proverka(shet_d,sp_sheta_deb->ravno(ii),1,0) != 0)
   continue;
  sheta_deb.plus(sp_sheta_deb->ravno(ii));
 } 

for(int ii=0; ii < sp_sheta_kre->kolih(); ii++)
 {
  if(proverka(shet_k,sp_sheta_kre->ravno(ii),1,0) != 0)
   continue;
  sheta_kre.plus(sp_sheta_kre->ravno(ii));
 } 
 
class masiv_din_double sum_pr; //Массив сумм проводок
int kolih_d=sheta_deb.kolih();
int kolih_k=sheta_kre.kolih();

sum_pr.make_class(kolih_d*kolih_k);

int nom_sd;
int nom_sk;
int kolstr1=0;
double suma=0.;
curp->poz_cursor(0);
while(curp->read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  strzag(LINES-1,0,kolstrp,++kolstr1);
  if((nom_sd=sheta_deb.find(row[0])) < 0)
    continue;
  if((nom_sk=sheta_kre.find(row[1])) < 0)
    continue;
  suma=atof(row[2]);
  
//  printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
  if(proverka(shet_k_minus,row[0],1,1) == 0)
   suma-=atof(row[3]);

  sum_pr.plus(suma,nom_sd*kolih_k+nom_sk);
  
 }
//Распечатываем дебет по вертикали
buh_rh(0,&sheta_deb,&sheta_kre,&sum_pr,ff_dv); 

//Распечатываем кредит по вертикали
suma=buh_rh(1,&sheta_deb,&sheta_kre,&sum_pr,ff_kv);

printw("\n%s:%.2f\n\n",gettext("Итого"),suma);

return(suma);

}

