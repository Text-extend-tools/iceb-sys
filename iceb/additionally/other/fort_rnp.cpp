/*$Id:$*/
/*02.03.2017	02.03.2017	Белых А.И.	fort_rnp.cpp
Распечатка отчётов по незавершённому производству
*/
#include <buhl.h>

class fort_rnp_rp
 {
  public:
   class iceb_tu_str data;
   class iceb_tu_str kod_ceh;
   class iceb_tu_str kod_det;
   class iceb_tu_str kod_izd;
   
  fort_rnp_rp()
   {
    clear();
   }

  void clear()
   {
    data.new_plus("");
    kod_ceh.new_plus("");
    kod_det.new_plus("");
    kod_izd.new_plus("");
        
   }   
 };
 
void fort_rnp_sap(short mr,short gr,FILE *ff);
void fort_rnp_pr(short mr,short gr,int nom_zap,class SPISOK *spp);
void fort_rnp_itog(double *i_izd,const char *naim,FILE *ff);

int fort_rnp(class spis_oth *oth)
{
char strsql[1024];
int kom=0;
static class fort_rnp_rp rp;


class VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт незавершённого производства"));

VV.VVOD_SPISOK_add_MD(gettext("Дата .............(м.г).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения..(,,).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Код детали.........(,,).."));//3
VV.VVOD_SPISOK_add_MD(gettext("Код изделия........(,,).."));//4

VV.VVOD_SPISOK_add_data(rp.data.ravno(),8);//0
VV.VVOD_SPISOK_add_data(rp.kod_ceh.ravno(),128);//1
VV.VVOD_SPISOK_add_data(rp.kod_det.ravno(),128);//2
VV.VVOD_SPISOK_add_data(rp.kod_izd.ravno(),128);//3

naz:;

clear();
 
helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

rp.data.new_plus(VV.data_ravno(0));
rp.kod_ceh.new_plus(VV.data_ravno(1));
rp.kod_det.new_plus(VV.data_ravno(2));
rp.kod_izd.new_plus(VV.data_ravno(3));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   iceb_t_pdoc("buhg5_1.txt");
   goto naz;

  case ESC:
  case FK10:
   return(1);


  case FK2:
  case PLU:
     break;


  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }
short mr=0,gr=0;

if(rsdat1(&mr,&gr,rp.data.ravno()) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата!"));
  goto naz;
 }

int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;

class iceb_t_tmptab tabtmp;
const char *imatmptab={"fortnp"};
char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
kodceh INT NOT NULL DEFAULT 0,\
kodizd INT NOT NULL DEFAULT 0,\
koddet INT NOT NULL DEFAULT 0,\
ei CHAR(8) NOT NULL DEFAULT '',\
kom VARCHAR(60) NOT NULL DEFAULT '',\
nz INT UNSIGNED NOT NULL DEFAULT 0,\
INDEX(kodceh,kodizd,koddet)) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  

sprintf(strsql,"select * from Fortnp");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи!"));
  goto naz;
 }
int kod_izd=0; 
class iceb_tu_str koment("");
class iceb_tu_str ei("");
while(cur.read_cursor(&row) != 0)
 {

  if(proverka(rp.kod_ceh.ravno(),row[1],0,0) != 0)
   continue;

  if(proverka(rp.kod_det.ravno(),row[2],0,0) != 0)
   continue;

  kod_izd=0;

  /*определяем код изделия*/
  sprintf(strsql,"select kodi from Specif where kodd=%d limit 1",atoi(row[2]));
  if(readkey(strsql,&row1,&cur1) >= 1)
   {
    kod_izd=atoi(row1[0]);
   }

  if(proverka(rp.kod_izd.ravno(),kod_izd,0,0) != 0)
    continue;
 
  koment.new_plus(row[4]);
  ei.new_plus(row[3]);

  sprintf(strsql,"insert into %s values('%s',%d,%d,'%s','%s',%d)",
  imatmptab,
  row[1],
  kod_izd,
  atoi(row[2]),
  ei.ravno_filtr(),
  koment.ravno_filtr(),
  atoi(row[0]));  
 
  sql_zapis(strsql,0,0);

 }
 
sprintf(strsql,"select * from %s order by kodceh asc,kodizd asc,koddet asc",imatmptab);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи!"));
  return(1);
 }

class iceb_fopen filact;
char imaf_act[64];

sprintf(imaf_act,"act%d.lst",getpid());

if(filact.start(imaf_act,"w") != 0)
 return(1);

short d=1;
dpm(&d,&mr,&gr,5);

iceb_t_zagolov(gettext("Акт незавершённого производства"),1,mr,gr,d,mr,gr,filact.ff);


class iceb_tu_str kod_ceh("");
class iceb_tu_str kod_ceh_z("");
int kod_izd_z=-1;
int kod_det=0; 
int kod_det_z=0;
class iceb_tu_str naim_izd("");
class iceb_tu_str naim_det("");
class iceb_tu_str naim_ceh("");
double cena_z_ed=0.;
int nom_zap=0;
short mk=mr,gk=gr;
int kol_sp1=0;
int kol_sp2=0;
d=1;
dpm(&d,&mk,&gk,3);
double proc1=0.;
double kolih1=0.;
double proc2=0.;
double kolih2=0.;
double suma1=0.;
double suma2=0.;
#define ITOGO_IZD 4
double i_izd[ITOGO_IZD];
memset(i_izd,'\0',sizeof(i_izd));
double i_det[ITOGO_IZD];
memset(i_det,'\0',sizeof(i_det));
double i_all[ITOGO_IZD];
memset(i_all,'\0',sizeof(i_all));
int kol_det=0;
while(cur.read_cursor(&row) != 0)
 {
  kod_ceh.new_plus((row[0]));
  kod_izd=atoi(row[1]);

  kod_det=atoi(row[2]);
  ei.new_plus(row[3]);
  nom_zap=atoi(row[5]);

  class SPISOK spis1;
  class SPISOK spis2;

  fort_rnp_pr(mr,gr,nom_zap,&spis1);

  fort_rnp_pr(mk,gk,nom_zap,&spis2);

  kol_sp1=spis1.kolih();
  kol_sp2=spis2.kolih();

  if(kol_sp1 == 0 && kol_sp2 == 0)
   continue;
   
  /*выравниваем списки*/
  if(kol_sp1 > kol_sp2)
   for(int nom=0; nom < kol_sp1-kol_sp2; nom++)
     spis2.plus("0|0");

  if(kol_sp1 < kol_sp2)
   for(int nom=0; nom < kol_sp2-kol_sp1; nom++)
     spis1.plus("0|0");
    
  if(SRAV(kod_ceh.ravno(),kod_ceh_z.ravno(),0) != 0)
   {
    if(kod_ceh_z.getdlinna() > 1)
     {
      if(kol_det > 1)
       fort_rnp_itog(i_det,gettext("Итого по детали"),filact.ff);
      fort_rnp_itog(i_izd,gettext("Итого по изделию"),filact.ff);/*распечатка итога по изделию обнуление массива*/
      
      fprintf(filact.ff,"\f");
     }
    sprintf(strsql,"select naik from Fortsc where kod='%s'",kod_ceh.ravno_filtr());
    if(readkey(strsql,&row1,&cur1) == 1)
     naim_ceh.new_plus(row1[0]);
    else
     naim_ceh.new_plus("");

    fprintf(filact.ff,"%s:%s %s\n",gettext("Подразделение"),kod_ceh.ravno(),naim_ceh.ravno());

    fort_rnp_sap(mr,gr,filact.ff);

    kod_det_z=0;
    kod_izd_z=-1;
    kod_ceh_z.new_plus(kod_ceh.ravno());
   }

  if(kod_izd != kod_izd_z)
   {
    if(kod_izd_z != -1)
     {
      if(kol_det > 1)
       fort_rnp_itog(i_det,gettext("Итого по детали"),filact.ff);
      fort_rnp_itog(i_izd,gettext("Итого по изделию"),filact.ff);/*распечатка итога по изделию обнуление массива*/
     }

    if(kod_izd > 0)
     {
      sprintf(strsql,"select naimat from Material where kodm=%d",kod_izd);
      if(readkey(strsql,&row1,&cur1) == 1)
       naim_izd.new_plus(row1[0]);
      else
       naim_izd.new_plus("");
     }
    else
     naim_izd.new_plus(gettext("Не входит ни в одно изделие"));
     
    fprintf(filact.ff,"%s:%d %s\n",gettext("Изделие"),kod_izd,naim_izd.ravno());
    fprintf(filact.ff,"\
-----------------------------------------------------------------------------------------------------------------------------\n");

    kod_det_z=0;
    kod_izd_z=kod_izd;
   }

  if(kod_det != kod_det_z)
   {
    if(kod_det_z > 0)
     {
      if(kol_det > 1)
       fort_rnp_itog(i_det,gettext("Итого по детали"),filact.ff);
     }
    kol_det=0;
    kod_det_z=kod_det;
   }

  sprintf(strsql,"select naimat from Material where kodm=%d",kod_det);
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_det.new_plus(row1[0]);
  else
   naim_det.new_plus("");
  
  cena_z_ed=0.;
  
  sprintf(strsql,"select kop from Fortti where ceh='%s' and kd=%d",kod_ceh.ravno_filtr(),kod_det);
  if(readkey(strsql,&row1,&cur1) == 1)
    cena_z_ed=atof(row1[0]);
  
  kol_det++;
       
  fprintf(filact.ff,"%8d %-*.*s %-*.*s %-*.*s %9.9g ",
  kod_det,
  iceb_tu_kolbait(30,naim_det.ravno()),
  iceb_tu_kolbait(30,naim_det.ravno()),
  naim_det.ravno(),
  iceb_tu_kolbait(7,ei.ravno()),
  iceb_tu_kolbait(7,ei.ravno()),
  ei.ravno(),
  iceb_tu_kolbait(10,row[4]),
  iceb_tu_kolbait(10,row[4]),
  row[4],
  cena_z_ed);


  for(int nom=0; nom < spis1.kolih(); nom++)
   {
    polen(spis1.ravno(nom),&kolih1,1,'|');
    polen(spis1.ravno(nom),&proc1,1,'|');

    polen(spis2.ravno(nom),&kolih2,1,'|');
    polen(spis2.ravno(nom),&proc2,1,'|');

    suma1=0.;
    if(cena_z_ed != 0. && kolih1 != 0. && proc1 != 0.)
     suma1=cena_z_ed*kolih1*proc1/100.;

    suma2=0.;
    if(cena_z_ed != 0. && kolih2 != 0. && proc2 != 0.)
     suma2=cena_z_ed*kolih2*proc2/100.;
    
    i_izd[0]+=kolih1;
    i_izd[1]+=suma1;
    i_izd[2]+=kolih2;
    i_izd[3]+=suma2;

    i_det[0]+=kolih1;
    i_det[1]+=suma1;
    i_det[2]+=kolih2;
    i_det[3]+=suma2;

    i_all[0]+=kolih1;
    i_all[1]+=suma1;
    i_all[2]+=kolih2;
    i_all[3]+=suma2;
         
    if(nom > 0)
     {
      fprintf(filact.ff,"%69s","");
      kol_det++;
     }
    if(kolih1 == 0.)
     fprintf(filact.ff,"%5s %10s %10s ","","","");
    else      
     fprintf(filact.ff,"%5.5g %10.10g %10.2f ",
     kolih1,proc1,suma1);

    if(kolih2 == 0.)
     fprintf(filact.ff,"\n");
    else
     fprintf(filact.ff,"%5.5g %10.10g %10.2f\n",
     kolih2,proc2,suma2);

   }
 }

if(kol_det > 1)
 fort_rnp_itog(i_det,gettext("Итого по детали"),filact.ff);

fprintf(filact.ff,"\
-----------------------------------------------------------------------------------------------------------------------------\n");

fort_rnp_itog(i_izd,gettext("Итого по изделию"),filact.ff);/*распечатка итога по изделию обнуление массива*/
fort_rnp_itog(i_all,gettext("Общий итог"),filact.ff);/*распечатка итога*/


podpis(filact.ff);
filact.end();

oth->spis_imaf.plus(imaf_act);
oth->spis_naim.plus(gettext("Акт незавершённого производства"));

iceb_t_ustpeh(oth->spis_imaf.ravno(0),3);

return(0);

}
/****************************************/
/*шапка */
/**************************************/
void fort_rnp_sap(short mr,short gr,FILE *ff)
{
short d=1;
dpm(&d,&mr,&gr,5);

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
        |                              |       |          |         |          01.%02d.%04d       |         %02d.%02d.%04d        |\n\
Код дет.|    Наименование детали       |Ед.изм.|Коментарий|Ст.на ед.|---------------------------|---------------------------|\n\
        |                              |       |          |         |Проц.|  Остаток |Стоимость |Проц.|  Остаток |Стоимость |\n"),mr,gr,d,mr,gr); 
/*
12345678 123456789012345678901234567890 1234567 1234567890 123456789 1234567890 12345 1234567890
*/
 
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------\n");


}
/******************************/
/**/
/********************************/
void fort_rnp_pr(short mr,short gr,int nom_zap,class SPISOK *spp)
{
int kolstr=0;
char strsql[1024];
SQL_str row1;
class SQLCURSOR cur1;

sprintf(strsql,"select kol,pr from Fortnpz where nzh=%d and data='%04d-%02d-01' order by pr asc",
nom_zap,gr,mr);
  
if((kolstr=cur1.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
 }

while(cur1.read_cursor(&row1) != 0)
 {
  sprintf(strsql,"%s|%s",row1[0],row1[1]);
  spp->plus(strsql);
 }


}
/******************************************/
/*Распечатка итога по изделию*/
/**********************************/
void fort_rnp_itog(double *i_izd,const char *naim,FILE *ff)
{

fprintf(ff,"%*s:%10.10g %10.2f       %10.10g %10.2f\n",
iceb_tu_kolbait(74,naim),
naim,
i_izd[0],i_izd[1],i_izd[2],i_izd[3]);
for(int nom=0; nom < ITOGO_IZD; nom++)
 i_izd[nom]=0.;
}
