/*$Id: buhrznp.c,v 5.22 2014/06/30 06:36:34 sasa Exp $*/
/*13.07.2015	13.07.2006	Белых А.И.	buhrznp.c
Расчёт распределения административных затрат и расходов на прибыли
*/
#include <errno.h>
#include "buhg.h"

int buhrznp_prov(const char *sp_shet,const char *shet_prov,const char *shet_provk,char *shet_vib);
int buhrznp(class iceb_tu_str *sp_shet_d,class iceb_tu_str *sp_shet_z,class iceb_tu_str *naim_ras);


int buhrznp(class spis_oth *oth)
{
char strsql[512];
static class iceb_tu_str dat1(""),dat2("");
int kom=0;

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт распределения административных затрат на доходы"));

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
   iceb_t_pdoc("buhg5_9.txt");
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;

  case FK5:
     iceb_redfil("buhrznp.alx",0);
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

class iceb_tu_str doh_sheta("");
class iceb_tu_str zat_sheta("");
class iceb_tu_str naim_ras("");


buhrznp(&doh_sheta,&zat_sheta,&naim_ras);


if(zat_sheta.getdlinna() <= 1)
 {
  iceb_t_soob(gettext("Не введены \"Затратные счета\" !"));
  goto naz;
 }

if(doh_sheta.getdlinna() <= 1)
 {
  iceb_t_soob(gettext("Не введены \"Доходные счета\" !"));
  goto naz;
 }

int kolstr;
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select sh,shk,deb from Prov where datp >= '%04d-%02d-%02d' and \
datp <= '%04d-%02d-%02d' and val=0 and deb <> 0. order by sh asc",gn,mn,dn,gk,mk,dk);

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
class SPISOK sp_doh;
class SPISOK sp_zat;
class masiv_din_double sum_doh;
class masiv_din_double sum_zat;
int nomer=0;
double suma=0.;
char shet_vib[64];

while(cur.read_cursor(&row) != 0)
 {
  suma=atof(row[2]);
  memset(shet_vib,'\0',sizeof(shet_vib));
    
  if(buhrznp_prov(doh_sheta.ravno(),row[0],row[1],shet_vib) == 0)
   {
    if((nomer=sp_doh.find(shet_vib)) < 0)
     sp_doh.plus(shet_vib);
    sum_doh.plus(suma,nomer);
   
    continue;
   }
  
  if(buhrznp_prov(zat_sheta.ravno(),row[0],row[1],shet_vib) == 0)
   {
    if((nomer=sp_zat.find(shet_vib)) < 0)
     sp_zat.plus(shet_vib);
    sum_zat.plus(suma,nomer);
   
    continue;
   }
 } 


char imaf[64];
sprintf(imaf,"rznp%d.lst",getpid());
FILE *ff;
if((ff=fopen(imaf,"w")) == NULL) 
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

if(naim_ras.getdlinna() <= 1)
 iceb_t_zagolov(gettext("Расчёт распределения административных затрат на доходы"),dn,mn,gn,dk,mk,gk,ff);
else
 iceb_t_zagolov(naim_ras.ravno(),dn,mn,gn,dk,mk,gk,ff);

fprintf(ff,"%s:%s\n",gettext("Доходные счета"),doh_sheta.ravno());
fprintf(ff,"%s:%s\n",gettext("Затратные счета"),zat_sheta.ravno());

double itogo_doh=sum_doh.suma();

//создаём массив для коэффициентов

double koef[sp_zat.kolih()];
fprintf(ff,"\n%s\n",gettext("Расчёт коэффициентов"));

for(int ii=0; ii < sp_zat.kolih(); ii++)
 {
  if(itogo_doh != 0.)
    koef[ii]=sum_zat.ravno(ii)/itogo_doh;
  else
    koef[ii]=0.;
  
  fprintf(ff,"%-*s %.2f/%.2f=%f\n",
  iceb_tu_kolbait(7,sp_zat.ravno(ii)),sp_zat.ravno(ii),sum_zat.ravno(ii),itogo_doh,koef[ii]);
 }

fprintf(ff,"\n");

class iceb_tu_str liniq("----------------------");
for(int ii=0; ii < sp_zat.kolih()+1; ii++)
 liniq.plus("-----------");

fprintf(ff,"%s\n",liniq.ravno());
 
fprintf(ff,"%s",gettext("Счёт дохо.|Сумма дох.|"));
for(int ii=0; ii < sp_zat.kolih(); ii++)
 fprintf(ff,"%-*.*s|",iceb_tu_kolbait(10,sp_zat.ravno(ii)),iceb_tu_kolbait(10,sp_zat.ravno(ii)),sp_zat.ravno(ii));
fprintf(ff,"%-*.*s|\n",iceb_tu_kolbait(10,gettext("Итого")),iceb_tu_kolbait(10,gettext("Итого")),gettext("Итого"));

fprintf(ff,"%s\n",liniq.ravno());

double itogo=0.;
for(int ii=0; ii < sp_doh.kolih() ; ii++)
 {
  fprintf(ff,"%-*.*s|%10.2f|",
  iceb_tu_kolbait(10,sp_doh.ravno(ii)),iceb_tu_kolbait(10,sp_doh.ravno(ii)),sp_doh.ravno(ii),
  sum_doh.ravno(ii));
  itogo=0.;
  for(int kk=0; kk < sp_zat.kolih() ; kk++)
   {
    suma=sum_doh.ravno(ii)*koef[kk];
    suma=okrug(suma,0.01);
    fprintf(ff,"%10.2f|",suma);
    itogo+=suma;
   }
  fprintf(ff,"%10.2f|\n",itogo);
 }

fprintf(ff,"%s\n",liniq.ravno());

fprintf(ff,"%*.*s|%10.2f|",
iceb_tu_kolbait(10,gettext("Итого")),
iceb_tu_kolbait(10,gettext("Итого")),
gettext("Итого"),itogo_doh);
//Вычисляем итоги по колонкам
double itogo_ob=0.;

for(int ii=0; ii < sp_zat.kolih(); ii++)
 {
  itogo=0.;
  for(int kk=0; kk < sp_doh.kolih(); kk++)
   {
    suma=sum_doh.ravno(kk)*koef[ii];
    suma=okrug(suma,0.01);
    itogo+=suma;
   } 
  fprintf(ff,"%10.2f|",itogo);
  itogo_ob+=itogo;
 }
fprintf(ff,"%10.2f|\n",itogo_ob);

podpis(ff);
fclose(ff);

iceb_t_ustpeh(imaf,0);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт распределения административных затрат на доходы"));

return(0);
}
/**********************************/
/*Проверка счёта*/
/*********************/
int buhrznp_prov(const char *sp_shet,const char *shet_prov,const char *shet_provk,char *shet_vib)
{
class iceb_tu_str shet_shetk("");
class iceb_tu_str shet("");
class iceb_tu_str shetk("");

int kol=pole2(sp_shet,',');
for(int ii=0; ii < kol; ii++)
 {
  
  polen(sp_shet,&shet_shetk,ii+1,',');
  if(shet_shetk.ravno()[0] == '\0')
   continue;

  shetk.new_plus("");

  if(polen(shet_shetk.ravno(),&shet,1,'>') != 0)
   shet.new_plus(shet_shetk.ravno());
  else
   polen(shet_shetk.ravno(),&shetk,2,'>');

  if(proverka(shet.ravno(),shet_prov,0,0) == 0)
   {
    if(shetk.ravno()[0] != '\0')
     {
      if(proverka(shetk.ravno(),shet_provk,0,0) == 0)
       {
        strcpy(shet_vib,shet_shetk.ravno());
        return(0);
       }
     }
    else
     {
      strcpy(shet_vib,shet_shetk.ravno());
      return(0);
     }
   }      
 }
return(1);
}

/*******************************/
/*Чтение файла настроек*/
/****************************/
int buhrznp(class iceb_tu_str *sp_shet_d,
class iceb_tu_str *sp_shet_z,
class iceb_tu_str *naim_ras)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='buhrznp.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob("Не найдены настройки buhrznp.alx");
  return(1);
 }

class iceb_tu_str stroka1("");

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(polen(row_alx[0],&stroka1,2,'|') != 0)
   continue;
 
  if(SRAV("Доходные счета",row_alx[0],1) == 0)
   {
    iceb_t_fplus(stroka1.ravno(),sp_shet_d,&cur_alx);
    continue;
   }
  if(SRAV("Затратные счета",row_alx[0],1) == 0)
   {
    iceb_t_fplus(stroka1.ravno(),sp_shet_z,&cur_alx);
    continue;
   }

  if(SRAV("Наименование расчёта",row_alx[0],1) == 0)
   {
    naim_ras->new_plus(stroka1.ravno());
    continue;
   }

 }
return(0); 

}
