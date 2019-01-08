/*$Id: mord12.c,v 5.15 2013/09/26 09:43:35 sasa Exp $*/
/*15.07.2015	29.08.2007	Белых А.И.	mord12.c
Расчёт и распечатка мемориального ордера N12
*/
#include <errno.h>
#include "buhg.h"

extern short	startgodb; /*Стартовый год*/

int mord12(class spis_oth *oth)
{
char strsql[512];
static class iceb_tu_str shet_or("");
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
int kom=0;

if(shet_or.ravno()[0] == '\0')
 shet_or.new_plus("232");

VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);

VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт мемориального ордера N12."));

VV.VVOD_SPISOK_add_MD(gettext("Номер счета.................."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата начала........(д.м.г)..."));//1
VV.VVOD_SPISOK_add_MD(gettext("Дата конца.........(д.м.г)..."));//2

VV.VVOD_SPISOK_add_data(shet_or.ravno(),32);
VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);

naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F4",gettext("очистить"),
//"F5",gettext("настройка"),
"F10",gettext("выход"),NULL);      


kom=VV.vvod(0,1,1,-1,-1);

shet_or.new_plus(VV.data_ravno(0));
dat1.new_plus(VV.data_ravno(1));
dat2.new_plus(VV.data_ravno(2));

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



sprintf(strsql,"select sh,shk,kre,pod from Prov where datp >= '%04d-%02d-%02d' \
and datp <= '%04d-%02d-%02d' and sh like '%s%%' and kre <> 0. order by datp,nomd asc",gn,mn,dn,gk,mk,dk,shet_or.ravno());
int kolstr=0;

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
FILE *ff;
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

fprintf(ff,"%s\n",iceb_t_get_pnk("00",0));

fprintf(ff,"%32sТипова форма N 411-авт (бюджет)\n","");

iceb_t_insfil("mord_start.alx",ff);
fprintf(ff,"\n%15s Меморіальний ордер N12\n","");

fprintf(ff,"%5s за період з %02d.%02d.%d р. по %02d.%02d.%d р.\n","",dn,mn,gn,dk,mk,gk);

fprintf(ff,"\n\
   Зведення накопичувальних відомостей про витрачання\n\
                 продуктів харчування\n\n");

//fprintf(ff,"\x1B\x4D"); /*12-знаков*/
//fprintf(ff,"\x0F");  /*Ужатый режим*/

fprintf(ff,"\
--------------------------------------------------------------------------------\n");
fprintf(ff,"\
  N  |Матеріально відповідальна особа (установа)|Кредіт суб.|Дебет  |  Сумма    |\n\
 з/п |                                          |%-*.*s|суб-ків|          |\n",
iceb_tu_kolbait(11,shet_or.ravno()),iceb_tu_kolbait(11,shet_or.ravno()),shet_or.ravno());
/*
12345 123456789012345678901234567890123456789012 12345678901 1234567 1234567890
*/
fprintf(ff,"\
--------------------------------------------------------------------------------\n");
int kolstr1=0;
class SPISOK shet_deb;
class SPISOK shet_kre;
class masiv_din_double sum_deb;
class masiv_din_double sum_kre;
SQL_str row,row1;
class SQLCURSOR cur1;
double itogo=0.;
int nomer_sh=0;
double suma=0.;
class iceb_tu_str mat_otv("");
int nom_zap=0;

while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
  strzag(LINES-1,0,kolstr,++kolstr1);
  mat_otv.new_plus("");
  //Узнаём материально-ответственного
  sprintf(strsql,"select naik,fmol from Sklad where kod=%s",row[3]);
  if(readkey(strsql,&row1,&cur1) == 1)
    {
     if(row1[1][0] != '\0')
      mat_otv.new_plus(row1[1]);    
     else
      mat_otv.new_plus(row1[0]);    
    }
           
  suma=atof(row[2]);
  
  itogo+=suma;
    
  fprintf(ff,"%5d %-*.*s %-*s %-*s %10.2f\n",
  ++nom_zap,
  iceb_tu_kolbait(42,mat_otv.ravno()),iceb_tu_kolbait(42,mat_otv.ravno()),mat_otv.ravno(),
  iceb_tu_kolbait(11,row[0]),row[0],
  iceb_tu_kolbait(7,row[1]),row[1],
  suma);


  if((nomer_sh=shet_deb.find(row[1])) < 0)
   shet_deb.plus(row[1]);        
  sum_deb.plus(suma,nomer_sh);

  if((nomer_sh=shet_kre.find(row[0])) < 0)
   shet_kre.plus(row[0]);        
  sum_kre.plus(suma,nomer_sh);

 }


fprintf(ff,"\
--------------------------------------------------------------------------------\n");
fprintf(ff,"%*s %10.2f\n",iceb_tu_kolbait(68,"Разом"),"Разом",itogo);

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
oth->spis_naim.plus("Меморіальний ордер N12");

iceb_t_ustpeh(oth->spis_imaf.ravno(0),0);

return(0);


}
