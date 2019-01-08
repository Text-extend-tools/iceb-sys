/*$Id: ukrspdok.c,v 5.20 2013/08/13 05:49:56 sasa Exp $*/
/*16.07.2015	17.09.2006	Белых А.И.	uskspdok.c
Распечатка списка документов по датам документов
*/
#include        <errno.h>
#include        "buhg.h"

double ukrspdok_svk(int kolih_dat_nomd,int kolih_kod_zat,int nomer_kol,class masiv_din_double *kol_kod,class masiv_din_double *sum_kod,double *kolih);
void ukrspdok_rp(const char *kontr,const char *vkom,const char *kod_zatrat,const char *punkt_naz,const char *organiz,const char *cel_kom,FILE *ff);


int ukrspdok(class spis_oth *oth)
{
class iceb_tu_str kod("");
class iceb_tu_str naik("");

char strsql[512];
int           kom,kom1;
static class iceb_tu_str dat1(""),dat2("");
static class iceb_tu_str kontr("");
static class iceb_tu_str vkom("");
static class iceb_tu_str kod_zatrat("");
static class iceb_tu_str punkt_naz("");
static class iceb_tu_str organiz("");
static class iceb_tu_str cel_kom("");

VVOD VV(0);
VVOD MENU(3);
VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(kontr.ravno(),128);
VV.VVOD_SPISOK_add_data(vkom.ravno(),128);
VV.VVOD_SPISOK_add_data(kod_zatrat.ravno(),128);
VV.VVOD_SPISOK_add_data(punkt_naz.ravno(),124);
VV.VVOD_SPISOK_add_data(organiz.ravno(),124);
VV.VVOD_SPISOK_add_data(cel_kom.ravno(),124);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала....(д.м.г).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца.....(д.м.г).."));//1
VV.VVOD_SPISOK_add_MD(gettext("Код контрагента...(,,).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Вид кома-ировки...(,,).."));//3
VV.VVOD_SPISOK_add_MD(gettext("Код затрат........(,,).."));//4
VV.VVOD_SPISOK_add_MD(gettext("Пунк назначения...(,,).."));//5
VV.VVOD_SPISOK_add_MD(gettext("Организация.......(,,).."));//6
VV.VVOD_SPISOK_add_MD(gettext("Цель командировки.(,,).."));//7

VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка списка документов по датам документов"));
VV.VVOD_SPISOK_add_ZAG(gettext("Введите реквизиты поиска"));

naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
kontr.new_plus(VV.data_ravno(2));
vkom.new_plus(VV.data_ravno(3));
kod_zatrat.new_plus(VV.data_ravno(4));
punkt_naz.new_plus(VV.data_ravno(5));
organiz.new_plus(VV.data_ravno(6));
cel_kom.new_plus(VV.data_ravno(7));

switch (kom)
 {
  case ESC:
  case FK10:
     clear();
     return(1);
     break;

  case FK1:
    GDITE();
    iceb_t_pdoc("ukr4_2.txt");
    clear();
    goto naz;

  case FK2:
  case PLU:
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка видов командировок"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка кодов затрат"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 3 :
      case -1:
       goto naz;

      case 0 :
        vibrek("Kontragent",&kontr);
        VV.VVOD_SPISOK_zapis_data(2,kontr.ravno());
        goto naz;

      case 1 :
        vibrek("Ukrvkr",&vkom);
        VV.VVOD_SPISOK_zapis_data(3,vkom.ravno());
        goto naz;

      case 2 :
        naik.new_plus("");
        if(dirkkr(&kod,&naik,1) == 0)
          VV.data_z_plus(4,kod.ravno());
        goto naz;

     }
    goto naz;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
    break;
 }
GDITE();

short dn,mn,gn;
short dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
 goto naz;

SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kolstr=0;

sprintf(strsql,"select nomd,datd,kont,vkom,pnaz,organ,celk from Ukrdok where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' \
order by datd,nomd asc",gn,mn,dn,gk,mk,dk);

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

//Составляем список документов
class SPISOK data_nomd;
int kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(kontr.ravno(),row[2],0,0) != 0)
    continue;

  if(proverka(vkom.ravno(),row[3],0,0) != 0)
    continue;

  if(proverka(punkt_naz.ravno(),row[4],4,0) != 0)
   continue;

  if(proverka(organiz.ravno(),row[5],4,0) != 0)
   continue;

  if(proverka(cel_kom.ravno(),row[6],4,0) != 0)
   continue;

  sprintf(strsql,"%s|%s",row[1],row[0]);
  if(data_nomd.find(strsql) < 0)
   data_nomd.plus(strsql);  
 }
GDITE();

int kolih_datnomd=data_nomd.kolih();
if(kolih_datnomd == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }

//Читаем список кодов затрат
sprintf(strsql,"select kod from Ukrkras");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
class masiv_din_int kod_zat;

while(cur.read_cursor(&row) != 0)
 kod_zat.plus(row[0]);

int kolih_kod_zat=kod_zat.kolih();
class masiv_din_double kol_kod;
class masiv_din_double sum_kod;

kol_kod.make_class(kolih_kod_zat*kolih_datnomd);
sum_kod.make_class(kolih_kod_zat*kolih_datnomd);
class iceb_tu_str datadok("");
class iceb_tu_str nomdok("");
kolstr1=0;
double suma=0.;
short dd,md,gd;
short dvnn,mvnn,gvnn;

FILE *ff;
char imaf_rt[64];
sprintf(imaf_rt,"ukrspdrt%d.lst",getpid());

if((ff = fopen(imaf_rt,"w")) == NULL)
 {
  error_op_nfil(imaf_rt,errno,"");
  return(1);
 }


iceb_t_zagolov(gettext("Содержимое авансових отчётов"),dn,mn,gn,dk,mk,gk,ff);

ukrspdok_rp(kontr.ravno(),vkom.ravno(),kod_zatrat.ravno(),punkt_naz.ravno(),organiz.ravno(),cel_kom.ravno(),ff);
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
 Дата     |Номер док.|Наименование расхода|Счёт  |Ед.из.|Колич.|Сумма б.НДС|Cумма НДС|Номер н.н.|Дата в.н.н.|Счёт НДС|Контрагент НДС\n");
//         1234567890 12345678901234567890 123456 123456 123456 1234567890 123456789 1234567890 12345678901 12345678
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------\n");
//Заряжаем массивы
class iceb_tu_str naim_kontr("");
class iceb_tu_str naim_rash("");
double i_suma_bnds=0.;
double i_suma_nds=0.;
for(int ii=0; ii < kolih_datnomd; ii++)
 {
  strzag(LINES-1,0,kolih_datnomd,++kolstr1);
  polen(data_nomd.ravno(ii),&datadok,1,'|');
  polen(data_nomd.ravno(ii),&nomdok,2,'|');
  sprintf(strsql,"select kodr,kolih,cena,snds,shet,ediz,ndrnn,dvnn,kdrnn,sn,ss from Ukrdok1 where datd='%s' and nomd='%s'",datadok.ravno(),nomdok.ravno());
  
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }
  suma=0.;
  int kodr=0;
  int kolih_zat=0;
  int nomer_kodzat=0;
  while(cur.read_cursor(&row) != 0)
   {
    if(proverka(kod_zatrat.ravno(),row[0],0,0) != 0)
     continue;
    kodr=atoi(row[0]);
    kolih_zat=atoi(row[1]);
    suma=atof(row[2])+atof(row[3])+atof(row[10]);
    nomer_kodzat=kod_zat.find(kodr);
    kol_kod.plus(kolih_zat,ii*kolih_kod_zat+nomer_kodzat);
    sum_kod.plus(suma,ii*kolih_kod_zat+nomer_kodzat);
    
    i_suma_bnds+=atof(row[2])+atof(row[10]);
    i_suma_nds+=atof(row[3]);
    /*Узнаём наименование расхода*/
    naim_rash.new_plus(row[0]);
    sprintf(strsql,"select naim from Ukrkras where kod=%d",naim_rash.ravno_atoi());
    if(readkey(strsql,&row1,&cur1) == 1)
     {
      naim_rash.plus(" ");
      naim_rash.plus(row1[0]);
     }  
    /*узнаём наименование контрагента*/
    naim_kontr.new_plus(row[8]);
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",naim_kontr.ravno());
    if(readkey(strsql,&row1,&cur1) == 1)
     {
      naim_kontr.plus(" ");
      naim_kontr.plus(row1[0]);
     }    
    rsdat(&dd,&md,&gd,datadok.ravno(),2);
    rsdat(&dvnn,&mvnn,&gvnn,row[7],2);

    
    fprintf(ff,"%02d.%02d.%04d %-*s %-*s %-*s %-*s %6.6g %10.2f %9.2f %-10s %02d.%02d.%04d  %-8s %.35s\n",
    dd,md,gd,
    iceb_tu_kolbait(10,row[0]),row[0],
    iceb_tu_kolbait(20,naim_rash.ravno()),naim_rash.ravno(),
    iceb_tu_kolbait(6,row[4]),row[4],
    iceb_tu_kolbait(6,row[5]),row[5],
    atof(row[1]),atof(row[2]),atof(row[3]),row[6],dvnn,mvnn,gvnn,
    row[9],naim_kontr.ravno());

            
   } 
  

 }

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s %10.2f %9.2f\n",iceb_tu_kolbait(63,gettext("Итого")),gettext("Итого"),i_suma_bnds,i_suma_nds);

GDITE();
podpis(ff);
fclose(ff);

char imaf[64];
sprintf(imaf,"ukrspd%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }


iceb_t_zagolov(gettext("Список авансовых отчётов"),dn,mn,gn,dk,mk,gk,ff);

ukrspdok_rp(kontr.ravno(),vkom.ravno(),kod_zatrat.ravno(),punkt_naz.ravno(),organiz.ravno(),cel_kom.ravno(),ff);

 
class iceb_tu_str liniq;
class iceb_tu_str strpb("");

liniq.plus("-------------------------------------------------------------------------------------------");

double kolih_kol;
int kolih_znk=0;
for(int ii=0; ii < kolih_kod_zat; ii++)
 {
  if(ukrspdok_svk(kolih_datnomd,kolih_kod_zat,ii,&kol_kod,&sum_kod,&kolih_kol) == 0.)
   continue;
  kolih_znk++;
  liniq.plus("-----------------");
  strpb.plus("                 ");
 }
//полка над Итого
if(kolih_znk > 1)
 {
  liniq.plus("-----------------");
  strpb.plus("                 ");
 }

liniq.plus("--------------------");

fprintf(ff,"%s\n",liniq.ravno());

fprintf(ff,gettext("\
   Дата   | Номер  |     Фамилия        |  Дата    |   Дата   | Номер  | Номер |   Дата   |"));
char naim_kod_zat[100];
for(int ii=0; ii < kolih_kod_zat; ii++)
 {
  if(ukrspdok_svk(kolih_datnomd,kolih_kod_zat,ii,&kol_kod,&sum_kod,&kolih_kol) == 0.)
   continue;
  memset(naim_kod_zat,'\0',sizeof(naim_kod_zat));
  sprintf(naim_kod_zat,"%d ",kod_zat.ravno(ii));
  sprintf(strsql,"select naim from Ukrkras where kod=%d",kod_zat.ravno(ii));

  if(readkey(strsql,&row,&cur) == 1)
   strcat(naim_kod_zat,row[0]);  
  fprintf(ff,"%-*.*s|",
  iceb_tu_kolbait(16,naim_kod_zat),iceb_tu_kolbait(16,naim_kod_zat),naim_kod_zat);

 }

if(kolih_znk > 1)
 fprintf(ff,"%-*.*s|",
 iceb_tu_kolbait(16,gettext("Итого")),
 iceb_tu_kolbait(16,gettext("Итого")),
 gettext("Итого"));

fprintf(ff,"%s\n",gettext("Пунк назначения"));

fprintf(ff,gettext("\
документа |доку-нта|                    | начала   |   конца  |приказа |авансо.|авансового|"));

for(int ii=0; ii < kolih_kod_zat; ii++)
 {
  if(ukrspdok_svk(kolih_datnomd,kolih_kod_zat,ii,&kol_kod,&sum_kod,&kolih_kol) == 0.)
   continue;
  memset(naim_kod_zat,'\0',sizeof(naim_kod_zat));
  sprintf(naim_kod_zat,"%d ",kod_zat.ravno(ii));
  sprintf(strsql,"select naim from Ukrkras where kod=%d",kod_zat.ravno(ii));
  if(readkey(strsql,&row,&cur) == 1)
   strcat(naim_kod_zat,row[0]);  

  fprintf(ff,"%-*.*s|",
  iceb_tu_kolbait(16,iceb_tu_adrsimv(16,naim_kod_zat)),
  iceb_tu_kolbait(16,iceb_tu_adrsimv(16,naim_kod_zat)),
  iceb_tu_adrsimv(16,naim_kod_zat));

 }

if(kolih_znk > 1)
 fprintf(ff,"%-16.16s|","");

fprintf(ff,"%s\n",gettext("Организация"));


fprintf(ff,gettext("\
          |        |                    |командиро.|командиро.|        |отчёта |  отчёта  |"));

for(int ii=0; ii < kolih_kod_zat; ii++)
 {
  if(ukrspdok_svk(kolih_datnomd,kolih_kod_zat,ii,&kol_kod,&sum_kod,&kolih_kol) == 0.)
   continue;
  fprintf(ff,"%-*.*s|%-*.*s|",
  iceb_tu_kolbait(5,gettext("Коли.")),
  iceb_tu_kolbait(5,gettext("Коли.")),
  gettext("Коли."),
  iceb_tu_kolbait(10,gettext("Сумма")),
  iceb_tu_kolbait(10,gettext("Сумма")),
  gettext("Сумма"));
 }

if(kolih_znk > 1)
  fprintf(ff,"%-*.*s|%-*.*s|",
  iceb_tu_kolbait(5,gettext("Коли.")),
  iceb_tu_kolbait(5,gettext("Коли.")),
  gettext("Коли."),
  iceb_tu_kolbait(10,gettext("Сумма")),
  iceb_tu_kolbait(10,gettext("Сумма")),
  gettext("Сумма"));

fprintf(ff,"%s\n",gettext("Цель командировки"));
   
fprintf(ff,"%s\n",liniq.ravno());

/*****************
fprintf(ff,gettext("\
   Дата   | Номер  |     Фамилия        |  Дата    |   Дата   | Номер  | Номер |   Дата   |Пунк назначения\n\
документа |доку-нта|                    | начала   |   конца  |приказа |авансо.|авансового|Организация\n\
          |        |                    |командиро.|командиро.|        |отчёта |  отчёта  |Цель командировки\n"));
fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------------\n");
1234567890 12345678 12345678901234567890 1234567890 1234567890 12345678 1234567 1234567890
*/


short d,m,g; 
class iceb_tu_str data_n("");
class iceb_tu_str data_k("");
class iceb_tu_str data_ao("");
class iceb_tu_str fio("");
kolstr1=0;
double itogo_gor_kol=0.;
double itogo_gor_sum=0.;

for(int ii=0; ii < kolih_datnomd; ii++)
 {
  strzag(LINES-1,0,kolih_datnomd,++kolstr1);

  polen(data_nomd.ravno(ii),&datadok,1,'|');
  polen(data_nomd.ravno(ii),&nomdok,2,'|');

  sprintf(strsql,"select * from Ukrdok where datd='%s' and nomd='%s'",datadok.ravno(),nomdok.ravno());
  if(readkey(strsql,&row,&cur) != 1)
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено документ !"));
    SOOB.VVOD_SPISOK_add_MD(datadok.ravno());
    SOOB.VVOD_SPISOK_add_MD(nomdok.ravno());
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    
    continue;
   }
  
  fio.new_plus(row[3]);
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[3]);
  if(readkey(strsql,&row1,&cur1) == 1)
   {
    fio.plus(" ",row1[0]);
   }

  rsdat(&dd,&md,&gd,row[2],2);
  
  rsdat(&d,&m,&g,row[7],2);
  if(d != 0)
   {
    sprintf(strsql,"%02d.%02d.%d",d,m,g);
    data_n.new_plus(strsql);
   }   
  else
   data_n.new_plus("");
   
  rsdat(&d,&m,&g,row[8],2);
  if(d != 0)
   {
    sprintf(strsql,"%02d.%02d.%d",d,m,g);
    data_k.new_plus(strsql);
   }  
  else
   data_k.new_plus("");

  rsdat(&d,&m,&g,row[15],2);
  if(d != 0)
   {
    sprintf(strsql,"%02d.%02d.%d",d,m,g);
    data_ao.new_plus(strsql);
   }
  else
   data_ao.new_plus("");
  fprintf(ff,"%02d.%02d.%d %-*s %-*.*s %10s %10s %-8s %-7s %10s ",
  dd,md,gd,
  iceb_tu_kolbait(8,row[1]),row[1],
  iceb_tu_kolbait(20,fio.ravno()),iceb_tu_kolbait(20,fio.ravno()),fio.ravno(),
  data_n.ravno(),data_k.ravno(),row[9],row[14],data_ao.ravno());
  
  itogo_gor_kol=0.;
  itogo_gor_sum=0.;
  for(int ii1=0; ii1 < kolih_kod_zat; ii1++)
   {
    if(ukrspdok_svk(kolih_datnomd,kolih_kod_zat,ii1,&kol_kod,&sum_kod,&kolih_kol) == 0.)
     continue;
  
    fprintf(ff,"%5.5g %10.2f ",kol_kod.ravno(ii*kolih_kod_zat+ii1),sum_kod.ravno(ii*kolih_kod_zat+ii1));
    itogo_gor_kol+=kol_kod.ravno(ii*kolih_kod_zat+ii1);
    itogo_gor_sum+=sum_kod.ravno(ii*kolih_kod_zat+ii1);
   }
  if(kolih_znk > 1)
   fprintf(ff,"%5.5g %10.2f ",itogo_gor_kol,itogo_gor_sum);

  fprintf(ff,"%s\n",row[4]);

  fprintf(ff,"%19s %-*.*s %49s %s%s\n",
  " ",
  iceb_tu_kolbait(20,iceb_tu_adrsimv(20,fio.ravno())),
  iceb_tu_kolbait(20,iceb_tu_adrsimv(20,fio.ravno())),
  iceb_tu_adrsimv(20,fio.ravno()),
  " ",strpb.ravno(),row[5]);    
  
  fprintf(ff,"%19s %-*.*s %49s %s%s\n",
  " ",
  iceb_tu_kolbait(20,iceb_tu_adrsimv(40,fio.ravno())),
  iceb_tu_kolbait(20,iceb_tu_adrsimv(40,fio.ravno())),
  iceb_tu_adrsimv(40,fio.ravno()),
  " ",strpb.ravno(),row[6]);    

 }
fprintf(ff,"%s\n",liniq.ravno());

if(kolih_znk > 0)
 {
  fprintf(ff,"%*s ",iceb_tu_kolbait(90,gettext("Итого")),gettext("Итого"));

  itogo_gor_kol=0.;
  itogo_gor_sum=0.;
  for(int ii=0; ii < kolih_kod_zat; ii++)
   {
    
    if((suma=ukrspdok_svk(kolih_datnomd,kolih_kod_zat,ii,&kol_kod,&sum_kod,&kolih_kol)) == 0.)
       continue;
    fprintf(ff,"%5.5g %10.2f ",kolih_kol,suma);
    itogo_gor_kol+=kolih_kol;
    itogo_gor_sum+=suma;  
   }
  if(kolih_znk > 1)
    fprintf(ff,"%5.5g %10.2f",itogo_gor_kol,itogo_gor_sum);
  fprintf(ff,"\n");  
 }
podpis(ff);

fclose(ff);


oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Список авансовых отчётов"));
oth->spis_imaf.plus(imaf_rt);
oth->spis_naim.plus(gettext("Содержимое авансовых отчётов"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);

return(0);

}
/*******************************************************/
/*Получить сумму по вертикальной колонке*/
/*******************************************************/

double ukrspdok_svk(int kolih_dat_nomd,
int kolih_kod_zat,
int nomer_kol,
class masiv_din_double *kol_kod,
class masiv_din_double *sum_kod,
double *kolih)
{
*kolih=0.;
double suma=0;
for(int ii=0; ii < kolih_dat_nomd; ii++)
 {
  *kolih+=kol_kod->ravno(ii*kolih_kod_zat+nomer_kol);
  suma+=sum_kod->ravno(ii*kolih_kod_zat+nomer_kol);
 }

return(suma);

}

/*******************/
/*вывод реквизитов поиска*/
/*******************/

void ukrspdok_rp(const char *kontr,const char *vkom,const char *kod_zatrat,const char *punkt_naz,const char *organiz,const char *cel_kom,FILE *ff)
{
if(kontr[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Контрагент"),kontr);
if(vkom[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Вид командировки"),vkom);
if(kod_zatrat[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код затрат"),kod_zatrat);
if(punkt_naz[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Пункт назначения"),punkt_naz);
if(organiz[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Организация"),organiz);
if(cel_kom[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Цель командировки"),cel_kom);

}
