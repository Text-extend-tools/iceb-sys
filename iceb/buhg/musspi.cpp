/*$Id:$*/
/*21.04.2017	23.02.2017	Белых А.И.	musspi.cpp
Расчёт свода списания матрериалов по изделиям
*/
#include        <errno.h>
#include        "buhg.h"

class musspi_poi
 {
  public:
   class iceb_tu_str dat1;
   class iceb_tu_str dat2;
   class iceb_tu_str sklad;
   class iceb_tu_str grup_mat;
   class iceb_tu_str kodmat;
   class iceb_tu_str shetz;
   
  musspi_poi()
   {
    clear();
   }
  void clear()
   {
    dat1.new_plus("");
    dat2.new_plus("");
    sklad.new_plus("");
    grup_mat.new_plus("");
    kodmat.new_plus("");
    shetz.new_plus("");
   }   
 };

void musspi_prp(class musspi_poi *rp,FILE *ff);
void musspi_sapkm(int nom_str,int *kolstr,FILE *ff);
void musspi_sapkd(int nom_str,int *kolstr,FILE *ff);
int musspi_prov(SQL_str row,class musspi_poi *rekp);
void musspi_naim(SQL_str row,class iceb_tu_str *naim_km,class iceb_tu_str *naim_kd,class iceb_tu_str *ei,double *cena);
void musspi_sd(int nom_str,int *kolstr,FILE *ff);
void musspi_iz(int nom_str,int *kolstr,FILE *ff);

int musspi(class spis_oth *oth)
{
char strsql[512];
static musspi_poi rekp;
int kom=0,kom1=0;
int kodgr=0;
int kodmati=0;
VVOD MENU(3);
VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт свода списания материалов по изделиям"));

VV.VVOD_SPISOK_add_data(rekp.dat1.ravno(),11);//0
VV.VVOD_SPISOK_add_data(rekp.dat2.ravno(),11);//1

VV.VVOD_SPISOK_add_data(rekp.sklad.ravno(),112);//2
VV.VVOD_SPISOK_add_data(rekp.grup_mat.ravno(),112);//3
VV.VVOD_SPISOK_add_data(rekp.kodmat.ravno(),112);//4
VV.VVOD_SPISOK_add_data(rekp.shetz.ravno(),112);//5

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.........."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..........."));//1
VV.VVOD_SPISOK_add_MD(gettext("Склад...........(,,)."));//2
VV.VVOD_SPISOK_add_MD(gettext("Группа мат-лла..(,,)."));//3
VV.VVOD_SPISOK_add_MD(gettext("Код материалла..(,,)."));//4
VV.VVOD_SPISOK_add_MD(gettext("Счёт учёта.....(,,).."));//5

naz:;

clear();


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("поиск"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,0,-1,-1);

rekp.dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
rekp.dat2.new_plus(VV.VVOD_SPISOK_return_data(1));
rekp.sklad.new_plus(VV.VVOD_SPISOK_return_data(2));
rekp.grup_mat.new_plus(VV.VVOD_SPISOK_return_data(3));
rekp.kodmat.new_plus(VV.VVOD_SPISOK_return_data(4));
rekp.shetz.new_plus(VV.VVOD_SPISOK_return_data(5));

class iceb_tu_str kods("");
class iceb_tu_str naims("");

switch (kom)
 {

  case FK10:
  case ESC:
   return(1);

  case FK1:
    GDITE();
//    iceb_t_pdoc("matu5_21.txt");
//    clear();
    goto naz;

  case FK2:
  case PLU:
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка складов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка материалов"));
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
        if(dirsklad(1,&kods,&naims) == 0)
          VV.data_z_plus(2,kods.ravno());
        goto naz;

      case 1 :
        vibrek("Grup",&rekp.grup_mat);
        VV.VVOD_SPISOK_zapis_data(3,rekp.grup_mat.ravno());
        goto naz;


      case 2:
        if(dirmatr(&kodgr,&kodmati,&naims,1,0,"",0,0) == 0)
         {
          VV.data_z_plus(4,kodmati);
         }
        
        goto naz;

      case 3 :
        vibrek("Plansh",&rekp.shetz);
        VV.VVOD_SPISOK_zapis_data(5,rekp.shetz.ravno());
        goto naz;


     }
    goto naz;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;
     
  default:
    goto naz;
 }

short dn,mn,gn,dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(0),&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(1)) != 0)
  goto naz;

GDITE();
SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kolstr=0;

sprintf(strsql,"select * from Dokummat4 where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' order by km asc,nk asc,kd asc",gn,mn,dn,gk,mk,dk);
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
class iceb_tu_str naim_km("");
class iceb_tu_str naim_kd("");
class iceb_tu_str ei("");

class iceb_fopen fkm;

char imaf_km[64];
sprintf(imaf_km,"sspim%d.lst",getpid());

if(fkm.start(imaf_km,"w") != 0)
 return(1);

iceb_t_zagolov(gettext("Свод списания по материалам"),dn,mn,gn,dk,mk,gk,fkm.ff);

musspi_prp(&rekp,fkm.ff);


musspi_sapkm(1,NULL,fkm.ff);




double kolih_mat=0.;
double kolih_zap=0.;
int kod_mat=0;
int kod_matz=0;
int kolv=0;
int kolstr1=0;
double cena=0.;
double suma=0.;
double itogo_suma=0.;
double suma_mat=0.;

while(cur.read_cursor(&row) != 0)
 {
  
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(musspi_prov(row,&rekp) != 0)
   continue;

  /*узнаём наименования*/ 
  musspi_naim(row,&naim_km,&naim_kd,&ei,&cena);


  kod_mat=atoi(row[3]);

  if(kod_mat != kod_matz)
   {
    if(kod_matz != 0)
     {
      if(kolv > 1)
       {
        sprintf(strsql,"%s %s",gettext("Итого по материалу"),row[3]);
        fprintf(fkm.ff,"%*s:%10.6g,%10.2f\n",iceb_tu_kolbait(107,strsql),strsql,kolih_mat,suma_mat);
       }
      suma_mat=0.;
      kolih_mat=0.;
      kolv=0;
     }
    
    kod_matz=kod_mat;
    
   }  

  kolih_zap=atof(row[6]);
  
  kolih_mat+=kolih_zap;

  suma=cena*kolih_zap;
  suma=okrug(suma,0.01);

  itogo_suma+=suma;
  suma_mat+=suma;
        
  if(kolv == 0)
   fprintf(fkm.ff,"%-6s %-*.*s %-6s %-*.*s %-5s %-6s %*s %10.10g %10.10g %10.2f\n",
   row[3],
   iceb_tu_kolbait(30,naim_km.ravno()),
   iceb_tu_kolbait(30,naim_km.ravno()),
   naim_km.ravno(),
   row[5],
   iceb_tu_kolbait(30,naim_kd.ravno()),
   iceb_tu_kolbait(30,naim_kd.ravno()),
   naim_kd.ravno(),  
   row[1],
   row[4],
   iceb_tu_kolbait(7,ei.ravno()),
   ei.ravno(),  
   cena,
   kolih_zap,
   suma);
  else
   fprintf(fkm.ff,"%-6s %-30.30s %-6s %-*.*s %-5s %-6s %*s %10.10g %10.10g %10.2f\n",
   "",
   "",
   row[5],
   iceb_tu_kolbait(30,naim_kd.ravno()),
   iceb_tu_kolbait(30,naim_kd.ravno()),
   naim_kd.ravno(),  
   row[1],
   row[4],
   iceb_tu_kolbait(7,ei.ravno()),
   ei.ravno(),  
   cena,
   kolih_zap,
   suma);
  
  kolv++;


 }

fprintf(fkm.ff,"ICEB_LST_END\n");

if(kolv > 1)
 {
  sprintf(strsql,"%s %d",gettext("Итого по материалу"),kod_mat);
  fprintf(fkm.ff,"%*s:%10.6g %10.2f\n",iceb_tu_kolbait(107,strsql),strsql,kolih_mat,suma_mat);
 }


fprintf(fkm.ff,"\
----------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(fkm.ff,"%*s:%10.2f\n",
iceb_tu_kolbait(118,gettext("Общий итог")),
gettext("Общий итог"),
itogo_suma);

podpis(fkm.ff);
fkm.end();

/*****************************************************/
sprintf(strsql,"select * from Dokummat4 where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' order by kd asc,km asc",gn,mn,dn,gk,mk,dk);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return(1);
 }






class iceb_t_tmptab tabtmp;
const char *imatmptab={"sspi"};
char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
iz INT NOT NULL DEFAULT 0,\
km INT NOT NULL DEFAULT 0,\
kd INT NOT NULL DEFAULT 0,\
nm CHAR(100) NOT NULL DEFAULT '',\
nd CHAR(100) NOT NULL DEFAULT '',\
cena DOUBLE(15,6) NOT NULL DEFAULT 0.,\
kol DOUBLE(15,6) NOT NULL DEFAULT 0.,\
unique(iz,km,kd,cena)) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  







char imaf_kd[64];
sprintf(imaf_kd,"sspid%d.lst",getpid());

if(fkm.start(imaf_kd,"w") != 0)
 return(1);

iceb_t_zagolov(gettext("Свод списания по деталям"),dn,mn,gn,dk,mk,gk,fkm.ff);

musspi_prp(&rekp,fkm.ff);

musspi_sapkd(1,NULL,fkm.ff);
int kod_det=0;
int kod_detz=0;
kolstr1=0;
kolv=0;
kolih_mat=0.;
itogo_suma=0.;
suma_mat=0.;
int kod_izdel=0;
while(cur.read_cursor(&row) != 0)
 {
  
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(musspi_prov(row,&rekp) != 0)
   continue;

  /*узнаём наименования*/ 
  musspi_naim(row,&naim_km,&naim_kd,&ei,&cena);




  kod_det=atoi(row[5]);
  kod_mat=atoi(row[3]);
  
  if(kod_det != kod_detz)
   {
    if(kod_detz != 0)
     {
      if(kolv > 1)
       {
        sprintf(strsql,"%s %s",gettext("Итого по материалу"),row[3]);
        fprintf(fkm.ff,"%*s:%10.6g %10.2f\n",iceb_tu_kolbait(107,strsql),strsql,kolih_mat,suma_mat);
       }

               

      suma_mat=0.;
      kolih_mat=0.;
      kolv=0;
     }
    /*узнаём в какое изделие входит*/
    sprintf(strsql,"select kodi from Specif where kodd=%d limit 1",kod_det);
    if(readkey(strsql,&row1,&cur1) == 1)
      kod_izdel=atoi(row1[0]);
    else
      kod_izdel=0.;
    
    kod_detz=kod_det;
    
   }  
  
  kolih_zap=atof(row[6]);
  
  kolih_mat+=kolih_zap;

  suma=cena*kolih_zap;
  suma=okrug(suma,0.01);

  itogo_suma+=suma;
  suma_mat+=suma;

  if(kolv == 0)
   fprintf(fkm.ff,"%-6s %-*.*s %-6s %-*.*s %-5s %-6s %*s %10.10g %10.6g %10.2f\n",
   row[5],
   iceb_tu_kolbait(30,naim_kd.ravno()),
   iceb_tu_kolbait(30,naim_kd.ravno()),
   naim_kd.ravno(),
   row[3],
   iceb_tu_kolbait(30,naim_km.ravno()),
   iceb_tu_kolbait(30,naim_km.ravno()),
   naim_km.ravno(),  
   row[1],
   row[4],
   iceb_tu_kolbait(7,ei.ravno()),
   ei.ravno(),  
   cena,
   kolih_zap,
   suma);
  else
   fprintf(fkm.ff,"%-6s %-30.30s %-6s %-*.*s %-5s %-6s %*s %10.10g %10.6g %10.2f\n",
   "",
   "",
   row[3],
   iceb_tu_kolbait(30,naim_km.ravno()),
   iceb_tu_kolbait(30,naim_km.ravno()),
   naim_km.ravno(),  
   row[1],
   row[4],
   iceb_tu_kolbait(7,ei.ravno()),
   ei.ravno(),  
   cena,
   kolih_zap,
   suma);
  
  kolv++;


  /*проверяем есть ли запись в таблице*/
  sprintf(strsql,"select kol from %s where iz=%d and km=%d and kd=%d and cena=%f",imatmptab,kod_izdel,kod_mat,kod_det,cena);
  if(readkey(strsql,&row1,&cur1) == 1)
   {
    kolih_zap+=atof(row1[0]);
    sprintf(strsql,"update %s set kol=%f where iz=%d and km=%d and kd=%d",
    imatmptab,
    kolih_zap,
    kod_izdel,
    kod_mat,
    kod_det);

    sql_zapis(strsql,1,0);
   }
  else
   {
    /*записываем во временную талицу*/
  
    sprintf(strsql,"insert into %s values(%d,%d,%d,'%s','%s',%f,%f)",
    imatmptab,
    kod_izdel,
    atoi(row[3]),
    atoi(row[5]),
    naim_km.ravno_filtr(),
    naim_kd.ravno_filtr(),
    cena,
    kolih_zap);

    sql_zapis(strsql,1,0);
  }


 }
fprintf(fkm.ff,"ICEB_LST_END\n");

if(kolv > 1)
 {
  sprintf(strsql,"%s %d",gettext("Итого по детали"),kod_det);
  fprintf(fkm.ff,"%*s:%10.6g\n",iceb_tu_kolbait(107,strsql),strsql,kolih_mat);
 }
fprintf(fkm.ff,"\
----------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(fkm.ff,"%*s:%10.2f\n",
iceb_tu_kolbait(118,gettext("Общий итог")),
gettext("Общий итог"),
itogo_suma);

podpis(fkm.ff);
fkm.end();


/**********************************************************************/

sprintf(strsql,"select * from Dokummat4 where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' order by datd asc,skl asc,nomd asc",gn,mn,dn,gk,mk,dk);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return(1);
 }

char imaf_dok[64];
sprintf(imaf_dok,"sspidok%d.lst",getpid());

if(fkm.start(imaf_dok,"w") != 0)
 return(1);

iceb_t_zagolov(gettext("Список документов списания"),dn,mn,gn,dk,mk,gk,fkm.ff);

musspi_prp(&rekp,fkm.ff);

musspi_sd(1,NULL,fkm.ff);
kolstr1=0;
kolv=0;
class iceb_tu_str data_dok("");
class iceb_tu_str nomer_dok("");
itogo_suma=0.;
suma_mat=0.;

while(cur.read_cursor(&row) != 0)
 {
  
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(musspi_prov(row,&rekp) != 0)
   continue;

  /*узнаём наименования*/ 
  musspi_naim(row,&naim_km,&naim_kd,&ei,&cena);

  if(SRAV(data_dok.ravno(),row[0],0) != 0)
   {
    data_dok.new_plus(row[0]);
    kolv=0;
   }

  if(SRAV(nomer_dok.ravno(),row[2],0) != 0)
   {
    nomer_dok.new_plus(row[2]);
    kolv=0;
   }
  
  kolih_zap=atof(row[6]);
  
  kolih_mat+=kolih_zap;

  suma=cena*kolih_zap;
  suma=okrug(suma,0.01);

  itogo_suma+=suma;

  if(kolv == 0)
   fprintf(fkm.ff,"%10s %-*.*s %-5s %-6s %-6s %-*.*s %-6s %-*.*s %*s %10.10g %10.6g %10.2f\n",
   iceb_tu_datzap(row[0]),
   iceb_tu_kolbait(10,row[2]),
   iceb_tu_kolbait(10,row[2]),
   row[2],
   row[1],
   row[4],
   row[3],
   iceb_tu_kolbait(30,naim_km.ravno()),
   iceb_tu_kolbait(30,naim_km.ravno()),
   naim_km.ravno(),
   row[5],
   iceb_tu_kolbait(30,naim_kd.ravno()),
   iceb_tu_kolbait(30,naim_kd.ravno()),
   naim_kd.ravno(),
   iceb_tu_kolbait(7,ei.ravno()),
   ei.ravno(),
   cena,
   kolih_zap,
   suma);  
  else
   fprintf(fkm.ff,"%10s %10s %-5s %-6s %-6s %-*.*s %-6s %-*.*s %*s %10.10g %10.6g %10.2f\n",
   "",
   "",
   row[1],
   row[4],
   row[3],
   iceb_tu_kolbait(30,naim_km.ravno()),
   iceb_tu_kolbait(30,naim_km.ravno()),
   naim_km.ravno(),
   row[5],
   iceb_tu_kolbait(30,naim_kd.ravno()),
   iceb_tu_kolbait(30,naim_kd.ravno()),
   naim_kd.ravno(),
   iceb_tu_kolbait(7,ei.ravno()),
   ei.ravno(),
   cena,
   kolih_zap,
   suma);  

  kolv++;
  
 }
fprintf(fkm.ff,"ICEB_LST_END\n");
fprintf(fkm.ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(fkm.ff,"%*s:%10.2f\n",
iceb_tu_kolbait(140,gettext("Общий итог")),
gettext("Общий итог"),
itogo_suma);

podpis(fkm.ff);
fkm.end();
 



/*распечатка по изделиям*/


sprintf(strsql,"select * from %s order by iz asc,km asc,kd asc",imatmptab);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return(1);
 }
char imaf_izd[64];
sprintf(imaf_izd,"sspiiz%d.lst",getpid());

if(fkm.start(imaf_izd,"w") != 0)
 return(1);

iceb_t_zagolov(gettext("Свод списания материалов по изделиям"),dn,mn,gn,dk,mk,gk,fkm.ff);

musspi_prp(&rekp,fkm.ff);

musspi_iz(1,NULL,fkm.ff);
kolstr1=0;
kolv=0;
itogo_suma=0.;
int kod_iz=0;
int kod_izz=-1;
double itogo_iz=0.;
double itogo_mat=0.;
kod_matz=0;
class iceb_tu_str naim_iz("");
while(cur.read_cursor(&row) != 0)
 {
  
  strzag(LINES-1,0,kolstr,++kolstr1);

  kod_iz=atoi(row[0]);
  kod_mat=atoi(row[1]);
  cena=atof(row[5]);

  if(kod_mat != kod_matz)
   {
    if(kod_matz != 0)
     {
      if(kolv > 1)
       {
        sprintf(strsql,"%s %d",gettext("Итого по материалу"),kod_matz);
        fprintf(fkm.ff,"%*s:%10.2f\n",iceb_tu_kolbait(97,strsql),strsql,itogo_mat);
       }
     }    
    itogo_mat=0.;
    kolv=0;
    kod_matz=kod_mat;
   }
  
  if(kod_iz != kod_izz)
   {
    if(kod_izz != -1)
     {
      fprintf(fkm.ff,"%*s:%10.2f\n",iceb_tu_kolbait(97,gettext("Итого по изделию")),gettext("Итого по изделию"),itogo_iz);
      itogo_iz=0.;
     }    

    if(kod_iz == 0)
     {
      naim_iz.new_plus(gettext("Детали не введённые в спецификацию ни одного изделия"));
     }
    else
     {
      /*узнаём наименование изделия*/
      sprintf(strsql,"select naimat from Material where kodm=%d",kod_iz);
      if(readkey(strsql,&row1,&cur1) == 1)
       naim_iz.new_plus(row1[0]);
      else
       {
        sprintf(strsql,"%s %d %s",gettext("Изделие"),kod_iz,gettext("не найдено в справочнике материалов"));
        naim_iz.new_plus(strsql);
       }      
     }
    fprintf(fkm.ff,"%s:%d %s\n",gettext("Изделие"),kod_iz,naim_iz.ravno());
    fprintf(fkm.ff,"\
-------------------------------------------------------------------------------------------------------------\n");
    kod_izz=kod_iz;
   }



  kolih_zap=atof(row[6]);
  
  suma=cena*kolih_zap;
  suma=okrug(suma,0.01);
  
  itogo_suma+=suma;
  itogo_iz+=suma;
  itogo_mat+=suma;
          
  fprintf(fkm.ff,"%6s %-*.*s %6s %-*.*s %10.10g %10.10g %10.2f\n",
  row[1],
  iceb_tu_kolbait(30,row[3]),
  iceb_tu_kolbait(30,row[3]),
  row[3],
  row[2],
  iceb_tu_kolbait(30,row[4]),
  iceb_tu_kolbait(30,row[4]),
  row[4],
  cena,
  kolih_zap,
  suma);    

  kolv++;
  
 }
fprintf(fkm.ff,"ICEB_LST_END\n");

if(kolv > 1)
 {
  sprintf(strsql,"%s %d",gettext("Итого по материалу"),kod_mat);
  fprintf(fkm.ff,"%*s:%10.2f\n",iceb_tu_kolbait(97,strsql),strsql,itogo_mat);
 }

fprintf(fkm.ff,"%*s:%10.2f\n",iceb_tu_kolbait(97,gettext("Итого по изделию")),gettext("Итого по изделию"),itogo_iz);

fprintf(fkm.ff,"\
-------------------------------------------------------------------------------------------------------------\n");

fprintf(fkm.ff,"%*s:%10.2f\n",
iceb_tu_kolbait(97,gettext("Общий итог")),
gettext("Общий итог"),
itogo_suma);

podpis(fkm.ff);
fkm.end();



int orient=iceb_t_ustpeh(imaf_km,3);
iceb_t_rnl(imaf_km,orient,NULL,&musspi_sapkm);

orient=iceb_t_ustpeh(imaf_kd,3);
iceb_t_rnl(imaf_kd,orient,NULL,&musspi_sapkd);

orient=iceb_t_ustpeh(imaf_dok,3);
iceb_t_rnl(imaf_dok,orient,NULL,&musspi_sd);

orient=iceb_t_ustpeh(imaf_izd,3);
iceb_t_rnl(imaf_izd,orient,NULL,&musspi_iz);


oth->spis_imaf.plus(imaf_km);
oth->spis_naim.plus(gettext("Cвод списания по материалам"));
oth->spis_imaf.plus(imaf_kd);
oth->spis_naim.plus(gettext("Cвод списания по деталям"));
oth->spis_imaf.plus(imaf_dok);
oth->spis_naim.plus(gettext("Cписок документов списания"));
oth->spis_imaf.plus(imaf_izd);
oth->spis_naim.plus(gettext("Cвод списания материалов по изделиям"));


return(0);


}
/*********************************/
/*печать реквизитов поиска*/
/*********************************/

void musspi_prp(class musspi_poi *rp,FILE *ff)
{

if(rp->sklad.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Склад"),rp->sklad.ravno());
if(rp->grup_mat.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Группа материала"),rp->grup_mat.ravno());
if(rp->kodmat.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код материала"),rp->kodmat.ravno());
if(rp->shetz.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Счёт"),rp->shetz.ravno());

}
/***************/
/*печать шапки свода по материалам*/
/***************/
void musspi_sapkm(int nom_str,int *kolstr,FILE *ff)
{

if(kolstr != NULL)
 *kolstr+=4;

fprintf(ff,"%80s %s N%d\n","",gettext("Страница"),nom_str);

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Код м.|   Наименование материала     |Код д.|   Наименование детали        |Склад|Ном.к.|Ед.изм.|   Цена   |Количество|   Сумма  |\n"));

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------------\n");
}
/***************/
/*печать шапки свода по деталям*/
/***************/
void musspi_sapkd(int nom_str,int *kolstr,FILE *ff)
{

if(kolstr != NULL)
 *kolstr+=4;

fprintf(ff,"%80s %s N%d\n","",gettext("Страница"),nom_str);

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Код д.|   Наименование детали        |Код м.|   Наименование материала     |Склад|Ном.к.|Ед.изм.|   Цена   |Количество|   Сумма  |\n"));

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------------\n");
}
/***************/
/*печать шапки списка документов*/
/***************/
void musspi_sd(int nom_str,int *kolstr,FILE *ff)
{

if(kolstr != NULL)
 *kolstr+=4;

fprintf(ff,"%90s %s N%d\n","",gettext("Страница"),nom_str);

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Дата докум|Номер док.|Склад|Ном к.|Код м.|   Наименование материала     |Код д.|   Наименование детали        |Ед.изм.|   Цена   |Количество|   Сумма  |\n"));

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------\n");
}
/***************/
/*печать шапки свода списания по изделиям*/
/***************/
void musspi_iz(int nom_str,int *kolstr,FILE *ff)
{

if(kolstr != NULL)
 *kolstr+=4;

fprintf(ff,"%90s %s N%d\n","",gettext("Страница"),nom_str);

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Код м.|   Наименование материала     |Код д.|   Наименование детали        |   Цена   |Количество|   Сумма  |\n"));

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------\n");
}
/****************************************/
/*проверка реквизитов поиска*/
/**************************************/
int musspi_prov(SQL_str row,class musspi_poi *rekp)
{
char strsql[1024];
SQL_str row1;
class SQLCURSOR cur1;

if(proverka(rekp->sklad.ravno(),row[1],0,0) != 0)
  return(1);

if(proverka(rekp->kodmat.ravno(),row[3],0,0) != 0)
  return(1);

if(rekp->shetz.getdlinna() > 1)
 {
  sprintf(strsql,"seslect shetu from Kart where sklad=%s and nomk=%s",row[1],row[4]);

  if(readkey(strsql,&row1,&cur1) == 1)
   if(proverka(rekp->shetz.ravno(),row1[0],1,0) != 0)
    return(1);
 }
 
if(rekp->grup_mat.getdlinna() > 1)
 {
  sprintf(strsql,"select kodgr from Material where kodm=%s",row[3]);
  if(readkey(strsql,&row1,&cur1) == 1)
   if(proverka(rekp->grup_mat.ravno(),row1[0],0,0) != 0)
    return(1);
 }
return(0);

}
/***************************/
/*узнаём наименования*/
/***************************/
void musspi_naim(SQL_str row,class iceb_tu_str *naim_km,class iceb_tu_str *naim_kd,class iceb_tu_str *ei,double *cena)
{
char strsql[1024];
SQL_str row1;
class SQLCURSOR cur1;
/*узнаём наименование материала*/
sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[3]));
if(readkey(strsql,&row1,&cur1) == 1)
 naim_km->new_plus(row1[0]);
else
 naim_km->new_plus("");

/*узнаём наименование детали*/
sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[5]));
if(readkey(strsql,&row1,&cur1) == 1)
 naim_kd->new_plus(row1[0]);
else
 naim_kd->new_plus("");
 

/*узнаём единицу измерения*/
sprintf(strsql,"select ei,cena from Kart where sklad=%s and nomk=%s",row[1],row[4]);
if(readkey(strsql,&row1,&cur1) == 1)
 {
  ei->new_plus(row1[0]);
  *cena=atof(row1[1]);
 }
else
 {
  ei->new_plus("");
  *cena=0.;
 }
}
