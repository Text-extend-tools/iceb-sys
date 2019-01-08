/*$Id: mord9.c,v 5.20 2013/09/26 09:43:35 sasa Exp $*/
/*15.07.2015	27.08.2007	Белых А.И.	mord9.c
Расчёт и распечатка мемориального ордера N9 и N10
*/
#include <errno.h>
#include "buhg.h"


extern short	startgodb; /*Стартовый год*/

int mord9(int metka_ras, //0-9ордер 1-10ордер 2-13 ордер
class spis_oth *oth)
{
char strsql[512];
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
int kom=0;

VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);

if(metka_ras == 0)
 VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт мемориального ордера N9"));
if(metka_ras == 1)
 VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт мемориального ордера N10"));
if(metka_ras == 2)
 VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт мемориального ордера N13"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала........(д.м.г)..."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца.........(д.м.г)..."));//1

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);

naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
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
//   prosf(bros);
   goto naz;
 
  case FK2:
  case PLU:
    break;

  case FK4:
    VV.VVOD_clear_data();         
    goto naz;

  case FK5:
    if(metka_ras == 0)
      iceb_redfil("mord9.alx",0);
     
    if(metka_ras == 1)
        iceb_redfil("mord10.alx",0);
 
    if(metka_ras == 2)
        iceb_redfil("mord13.alx",0);
    goto naz;

  case FK10:
  case ESC:
    return(1);
     
  default:
   kom=0;
   goto naz;
 }

GDITE();

short dn,mn,gn;
short dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
  goto naz;

class SPISOK deb_nastr;
class SPISOK kre_nastr;

FILE *ff;
class iceb_tu_str imaf_nastr;

if(metka_ras == 0)
  imaf_nastr.new_plus("mord9.alx");
if(metka_ras == 1)
  imaf_nastr.new_plus("mord10.alx");
if(metka_ras == 2)
  imaf_nastr.new_plus("mord13.alx");

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_nastr.ravno());
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_nastr.ravno());
  iceb_t_soob(strsql);
  return(1);
 }

char stroka[1024];
memset(stroka,'\0',sizeof(stroka));
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
    continue;  
  if(row_alx[0][0] == 'd')
   {
    deb_nastr.plus(row_alx[0]);
    continue;
   }
  if(row_alx[0][0] == 'k')
   {
    kre_nastr.plus(row_alx[0]);
    continue;
   }
  
 }

//printw("Количество настроек по дебету=%d\n",deb_nastr.kolih());
//printw("Количество настроек по кредиту=%d\n",kre_nastr.kolih());

sprintf(strsql,"select datp,sh,shk,kodkon,nomd,deb,kre,komen,kto,pod from Prov where datp >= '%04d-%02d-%02d' \
and datp <= '%04d-%02d-%02d' order by datp,nomd asc",gn,mn,dn,gk,mk,dk);
SQLCURSOR cur ;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  goto naz;
 }

char imaf[64];
sprintf(imaf,"mord%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

fprintf(ff,"%s\n",iceb_t_get_pnk("00",0));

if(metka_ras == 0)
 fprintf(ff,"%32sТипова форма N 438-авт (бюджет)\n","");
if(metka_ras == 1)
 fprintf(ff,"%32sТипова форма N 439-авт (бюджет)\n","");
if(metka_ras == 2)
 fprintf(ff,"%32sТипова форма N 496-авт (бюджет)\n","");

iceb_t_insfil("mord_start.alx",ff);

if(metka_ras == 0)
 fprintf(ff,"\n%15s Меморіальний ордер N9\n","");
if(metka_ras == 1)
 fprintf(ff,"\n%15s Меморіальний ордер N10\n","");
if(metka_ras == 2)
 fprintf(ff,"\n%15s Меморіальний ордер N13\n","");

fprintf(ff,"%5s за період з %02d.%02d.%d р. по %02d.%02d.%d р.\n","",dn,mn,gn,dk,mk,gk);
fprintf(ff,"  Накопичувальна відомість про вибуття та переміщення\n\
       малоцінних та швидкозношуваних предметів\n");
                    
fprintf(ff,"\
-----------------------------------------------------------------------------------------\n");
fprintf(ff,"\
N з/п|  Дата    |Назва і номер документа, МВО (установа) |  Дебет  | Кредіт  |   Сумма  |\n");
/*
12345 1234567890 1234567890123456789012345678901234567890 1234567890 1234567890 1234567890
*/
fprintf(ff,"\
-----------------------------------------------------------------------------------------\n");
class SPISOK shet_deb;
class SPISOK shet_kre;
class masiv_din_double sum_deb;
class masiv_din_double sum_kre;
double suma_deb=0.,suma_kre=0.;
int nomer_sh=0;
int nomer_zap=0;
SQL_str row;
short d,m,g;
class iceb_tu_str naim_kontr("");
SQL_str row1;
class SQLCURSOR cur1; 
int kolstr1=0;
double itogo=0.;

while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
  strzag(LINES-1,0,kolstr,++kolstr1);

  suma_deb=atof(row[5]);
  suma_kre=atof(row[6]);
  if(suma_deb != 0.)
   {
    if(buh_prov_shet(row[1],row[2],&deb_nastr) != 0)
     continue;
   }
  else
   {
    if(buh_prov_shet(row[1],row[2],&kre_nastr) != 0)
     continue;
   }

  naim_kontr.new_plus("");

  if(row[3][0] != '\0')
   {
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[3]);
    if(readkey(strsql,&row1,&cur1) == 1)
     naim_kontr.new_plus(row1[0]);
   }
  if(SRAV(ICEB_MP_MATU,row[8],0) == 0)
   {
    naim_kontr.new_plus("");
    //Читаем наименование склада
    sprintf(strsql,"select naik from Sklad where kod=%s",row[9]);
    if(readkey(strsql,&row1,&cur1) == 1)
     naim_kontr.new_plus(row1[0]);
     
   }


  rsdat(&d,&m,&g,row[0],2);
      
  memset(stroka,'\0',sizeof(stroka)); 
  sprintf(stroka,"%s %s %s %s",row[4],row[7],row[3],naim_kontr.ravno());

  if(suma_deb != 0.)
   {
    fprintf(ff,"%5d %02d.%02d.%04d %-*.*s %-*s %-*s %10.2f\n",
    ++nomer_zap,
    d,m,g,
    iceb_tu_kolbait(40,stroka),iceb_tu_kolbait(40,stroka),stroka,
    iceb_tu_kolbait(9,row[1]),row[1],
    iceb_tu_kolbait(9,row[2]),row[2],
    suma_deb);

    if((nomer_sh=shet_deb.find(row[1])) < 0)
     shet_deb.plus(row[1]);        
    sum_deb.plus(suma_deb,nomer_sh);

    if((nomer_sh=shet_kre.find(row[2])) < 0)
     shet_kre.plus(row[2]);        
    sum_kre.plus(suma_deb,nomer_sh);
   }
  
  if(suma_kre != 0.)
   {
    fprintf(ff,"%5d %02d.%02d.%04d %-*.*s %-*s %-*s %10.2f\n",
    ++nomer_zap,
    d,m,g,
    iceb_tu_kolbait(40,stroka),iceb_tu_kolbait(40,stroka),stroka,
    iceb_tu_kolbait(9,row[2]),row[2],
    iceb_tu_kolbait(9,row[1]),row[1],
    suma_kre);

    if((nomer_sh=shet_deb.find(row[2])) < 0)
     shet_deb.plus(row[2]);        
    sum_deb.plus(suma_kre,nomer_sh);

    if((nomer_sh=shet_kre.find(row[1])) < 0)
     shet_kre.plus(row[1]);        
    sum_kre.plus(suma_kre,nomer_sh);
    
   }
  
  if(iceb_tu_strlen(stroka) > 40)
   fprintf(ff,"%5s %10s %s\n","","",iceb_tu_adrsimv(40,stroka));
  
  itogo+=suma_kre+suma_deb;
 }


fprintf(ff,"\
-----------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s %10.2f\n",iceb_tu_kolbait(77,"Разом"),"Разом",itogo);

fprintf(ff,"\nУсього до кредіту субрахунків: Субсчёт   Сумма\n");
itogo=0.;                               
for(int ii=0; ii < shet_kre.kolih(); ii++)
 {
  fprintf(ff,"%30s %-*s %10.2f\n",
  "",
  iceb_tu_kolbait(10,shet_kre.ravno(ii)),shet_kre.ravno(ii),
  sum_kre.ravno(ii));
  itogo+=sum_kre.ravno(ii);
  
 }

fprintf(ff,"%30s ---------------------\n","");
fprintf(ff,"%30s %-*s %10.2f\n","",iceb_tu_kolbait(10,"Разом"),"Разом",itogo);


fprintf(ff,"\nУсього до дебету субрахунків: Субсчёт   Сумма\n");
itogo=0.;
for(int ii=0; ii < shet_deb.kolih(); ii++)
 {
  fprintf(ff,"%30s %-*s %10.2f\n",
  "",
  iceb_tu_kolbait(10,shet_deb.ravno(ii)),shet_deb.ravno(ii),
  sum_deb.ravno(ii));
  itogo+=sum_deb.ravno(ii);
  
 }

fprintf(ff,"%30s ---------------------\n","");
fprintf(ff,"%30s %-*s %10.2f\n","",iceb_tu_kolbait(10,"Разом"),"Разом",itogo);


//imafn("mord_end.alx",&imaf_alx);

fprintf(ff,"\n\n");

iceb_t_insfil("mord_end.alx",ff);

podpis(ff);
fclose(ff);

oth->spis_imaf.plus(imaf);
memset(strsql,'\0',sizeof(strsql));

if(metka_ras == 0)
 sprintf(strsql,"Меморіальний ордер N9");
if(metka_ras == 1)
 sprintf(strsql,"Меморіальний ордер N10");
if(metka_ras == 2)
 sprintf(strsql,"Меморіальний ордер N13");

oth->spis_naim.plus(strsql);

iceb_t_ustpeh(oth->spis_imaf.ravno(0),0);

return(0);
}

