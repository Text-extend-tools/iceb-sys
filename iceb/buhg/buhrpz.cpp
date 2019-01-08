/*$Id: buhrpz.c,v 5.18 2013-04-18 09:26:03 sasa Exp $*/
/*13.07.2015	03.08.2006	Белых А.И.	buhrpz.c
Расчёт распределения административных затрат на доходы
*/
#include <errno.h>
#include "buhg.h"

void buhrpz_lin(int kolih_podr,FILE *ff);


int buhrpz(class spis_oth *oth)
{
char strsql[512];
static class iceb_tu_str dat1(""),dat2("");
int kom=0;

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт видов затрат по элементам"));

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
   iceb_t_pdoc("buhg5_10.txt");
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;

  case FK5:
     iceb_redfil("buhrpz.alx",0);
     goto naz;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }
time_t time_n;
time(&time_n);

GDITE();

short dn,mn,gn;
short dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
 goto naz;

int kolstr;
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select str from Alx where fil='buhrpz.alx' order by ns asc");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob("Не найдены настройки buhrpz.alx");
  return(1);
 }



class SPISOK sp_vid_zat;
class SPISOK sp_podr;
int metka_vid_zat=0;
class iceb_tu_str naim_ras("");
class iceb_tu_str naim_it_str("");

//Читаем настройки
while(cur.read_cursor(&row) != 0)
 {
  if(row[0][0] == '#')
   continue;
  if(polen(row[0],strsql,sizeof(strsql),1,'|') != 0)
   continue;

  if(SRAV("Наименование расчёта",strsql,0) == 0)
   {
    polen(row[0],&naim_ras,2,'|');
    continue;
   }
  if(SRAV("Наименование итоговой строки",strsql,0) == 0)
   {
    polen(row[0],&naim_it_str,2,'|');
    continue;
   }
  if(SRAV("Наименование вида затрат",strsql,0) == 0)
   {
    polen(row[0],strsql,sizeof(strsql),2,'|');
    if(strsql[0] == '\0')
     {
      metka_vid_zat=0;
      continue;
     }
    metka_vid_zat=1;

    sp_vid_zat.plus(strsql);
    continue;
   }

  if(SRAV("Наименование элемента",strsql,0) == 0)
   {
    if(metka_vid_zat == 0)
     continue;

    polen(row[0],strsql,sizeof(strsql),2,'|');

    if(strsql[0] == '\0')
     continue;

    if(sp_podr.find(strsql) < 0)
      sp_podr.plus(strsql);
    continue;
   }


 }
if(naim_ras.ravno()[0] == '\0')
 naim_ras.new_plus(gettext("Расчёт видов затрат по элементам"));
if(naim_it_str.ravno()[0] == '\0')
 naim_it_str.new_plus(gettext("Итого"));

int kol_naim_podr=sp_podr.kolih();

class SPISOK sp_deb[sp_vid_zat.kolih()][sp_podr.kolih()];
class SPISOK sp_kre[sp_vid_zat.kolih()][kol_naim_podr];
int nomer_pod=0;
int nomer_vid_zat=0;
cur.poz_cursor(0);
while(cur.read_cursor(&row) != 0)
 {
  if(row[0][0] == '#')
   continue;
  if(polen(row[0],strsql,sizeof(strsql),1,'|') != 0)
   continue;

  if(SRAV("Наименование вида затрат",strsql,0) == 0)
   {
    polen(row[0],strsql,sizeof(strsql),2,'|');
    if(strsql[0] == '\0')
     {
      metka_vid_zat=0;
      continue;
     }
    nomer_vid_zat=sp_vid_zat.find(strsql);
    metka_vid_zat=1;
    continue;
   }


  if(SRAV("Наименование элемента",strsql,0) == 0)
   {
    if(metka_vid_zat == 0)
     continue;

    polen(row[0],strsql,sizeof(strsql),2,'|');

    if(strsql[0] == '\0')
     continue;

    nomer_pod=sp_podr.find(strsql);

    cur.read_cursor(&row);
    polen(row[0],strsql,sizeof(strsql),2,'|');
    sp_deb[nomer_vid_zat][nomer_pod].plus(strsql);

    cur.read_cursor(&row);
    polen(row[0],strsql,sizeof(strsql),2,'|');
    sp_kre[nomer_vid_zat][nomer_pod].plus(strsql);

    continue;
   }


 }

int kolih_vid_zat=sp_vid_zat.kolih();
int kolih_podr=sp_podr.kolih();
if(kolih_vid_zat == 0)
 {
  sprintf(strsql,"%s %d !",gettext("Количество видов затрат"),kolih_vid_zat);
  iceb_t_soob(strsql);
  return(1);
 }
if(kolih_podr == 0)
 {
  sprintf(strsql,"%s %d !",gettext("Количество подразделений"),kolih_podr);
  iceb_t_soob(strsql);
  return(1);
 }


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

double suma=0.;
double mas_sum[kolih_vid_zat][kolih_podr];

memset(mas_sum,'\0',sizeof(mas_sum));
int kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);    
  for(int vz=0; vz < kolih_vid_zat; vz++)
   for(int pd=0; pd < kolih_podr; pd++)
    {
     if(sp_deb[vz][pd].kolih() == 0)
      continue;

     if(proverka(sp_deb[vz][pd].ravno(0),row[0],1,0) != 0)
      continue;     

     if(sp_kre[vz][pd].kolih() == 0)
      continue;

     if(proverka(sp_kre[vz][pd].ravno(0),row[1],1,0) != 0)
      continue;     

     suma=atof(row[2]);
     mas_sum[vz][pd]+=suma;

    }

 }

FILE *ff;
char imaf[64];

sprintf(imaf,"brpz%d.lst",getpid());

if((ff=fopen(imaf,"w")) == NULL) 
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
iceb_t_zagolov(naim_ras.ravno(),dn,mn,gn,dk,mk,gk,ff);

//линия
buhrpz_lin(kolih_podr,ff);
fprintf(ff,"%-20.20s|","");
for(int pd=0; pd < kolih_podr; pd++)
 fprintf(ff,"%-*.*s|",iceb_tu_kolbait(10,sp_podr.ravno(pd)),iceb_tu_kolbait(10,sp_podr.ravno(pd)),sp_podr.ravno(pd));
fprintf(ff,"%-*.*s|\n",iceb_tu_kolbait(10,gettext("Итого")),iceb_tu_kolbait(10,gettext("Итого")),gettext("Итого"));

buhrpz_lin(kolih_podr,ff);


double itogo_gor=0.;
for(int vz=0; vz < kolih_vid_zat; vz++)
 {
  fprintf(ff,"%-*.*s|",iceb_tu_kolbait(20,sp_vid_zat.ravno(vz)),iceb_tu_kolbait(20,sp_vid_zat.ravno(vz)),sp_vid_zat.ravno(vz));
  itogo_gor=0.;
  for(int pd=0; pd < kolih_podr; pd++)
   {
    fprintf(ff,"%10.2f|",mas_sum[vz][pd]);
    itogo_gor+=mas_sum[vz][pd];
   }
  fprintf(ff,"%10.2f|\n",itogo_gor);
 }

buhrpz_lin(kolih_podr,ff);
//печатаем итоговую стороку
fprintf(ff,"%-*.*s|",iceb_tu_kolbait(20,naim_it_str.ravno()),iceb_tu_kolbait(20,naim_it_str.ravno()),naim_it_str.ravno());
double itogo_kol=0.;
itogo_gor=0.;
for(int pd=0; pd < kolih_podr; pd++)
 {
  itogo_kol=0.;
  for(int vz=0; vz < kolih_vid_zat; vz++)
   itogo_kol+=mas_sum[vz][pd];
  fprintf(ff,"%10.2f|",itogo_kol);
  itogo_gor+=itogo_kol;
 }
fprintf(ff,"%10.2f|\n",itogo_gor);

podpis(ff);

fclose(ff);
oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт видов затрат по элементам"));

iceb_t_ustpeh(imaf,0);

return(0);
}
/********************************/
/*подчёркивание*/
/*****************************/
void buhrpz_lin(int kolih_podr,FILE *ff)
{
fprintf(ff,"---------------------");
for(int ii=0; ii <= kolih_podr; ii++)
  fprintf(ff,"-----------");
fprintf(ff,"\n");

}
