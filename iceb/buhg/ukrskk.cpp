/*$Id: ukrskk.c,v 5.20 2013/09/26 09:43:41 sasa Exp $*/
/*16.07.2015	04.07.2006	Белых А.И.	ukrdvr.c
Отчёт по счетам и кодам командировочных расходов
*/
#include <errno.h>
#include "buhg.h"


int ukrskk(class spis_oth *oth)
{
static class iceb_tu_str dat1(""),dat2("");
static class iceb_tu_str sheta("");
static class iceb_tu_str kod_zatrat("");
static class iceb_tu_str kod_gr_zatrat("");
class iceb_tu_str naim("");
class iceb_tu_str kod("");
class iceb_tu_str naik("");
char strsql[1024];


int kom=0,kom1=0;

VVOD MENU(3);

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Распечатать свод по счетам-кодам командировочных"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.....(д.м.г).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца......(д.м.г).."));//1 
VV.VVOD_SPISOK_add_MD(gettext("Счёт...............(,,).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Код затрат.........(,,).."));//3
VV.VVOD_SPISOK_add_MD(gettext("Код группы затрат..(,,).."));//4

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11); 
VV.VVOD_SPISOK_add_data(sheta.ravno(),128);
VV.VVOD_SPISOK_add_data(kod_zatrat.ravno(),128);
VV.VVOD_SPISOK_add_data(kod_gr_zatrat.ravno(),128);

naz:;

clear();
 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
//"F5",gettext("настройка"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
sheta.new_plus(VV.data_ravno(2));
kod_zatrat.new_plus(VV.data_ravno(3));
kod_gr_zatrat.new_plus(VV.data_ravno(4));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("ukr4_4.txt");
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка кодов затрат"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп кодов затрат"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 3:
      case -1:
       break;

      case 0 :
        vibrek("Plansh",&sheta);
        VV.VVOD_SPISOK_zapis_data(2,sheta.ravno());
        break;

      case 1 :
        naik.new_plus("");
        if(dirkkr(&kod,&naik,1) == 0)
          VV.data_z_plus(3,kod.ravno());
        break;

      case 2 :
        vibrek("Ukrgrup",&kod_gr_zatrat);
        VV.VVOD_SPISOK_zapis_data(4,kod_gr_zatrat.ravno());
        break;
     }
   goto naz;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }
GDITE();

short dn,mn,gn;
short dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
 goto naz;


SQL_str row;
SQLCURSOR cur;
SQL_str row1;
SQLCURSOR cur1;
int kolstr=0;

sprintf(strsql,"select datd,nomd from Ukrdok where datao >= '%04d-%02d-%02d' and \
datao <= '%04d-%02d-%02d'",gn,mn,dn,gk,mk,dk);
if((kolstr=cur1.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }
class SPISOK sp_shet; //Список счетов

while(cur1.read_cursor(&row1) != 0)
 {
  sprintf(strsql,"select distinct shet,sn,snds from Ukrdok1 where datd='%s' and nomd='%s'",row1[0],row1[1]);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return(1);
   }

  while(cur.read_cursor(&row) != 0)
   {
    if(row[1][0] != '\0' && atof(row[2]) != 0.)
     {
  
      if(sp_shet.find(row[1]) < 0)
       sp_shet.plus(row[1]);
     }
    if(sp_shet.find(row[0]) < 0)
     sp_shet.plus(row[0]);
     
   }
 }

sprintf(strsql,"select kod from Ukrkras");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не введены коды расходов !"));
  return(1);
 }
class SPISOK sp_kod_ras; //Список кодов расхода

while(cur.read_cursor(&row) != 0)
 sp_kod_ras.plus(row[0]);

class masiv_din_double m_kolih; //Массив с количеством
class masiv_din_double m_suma;  //Массив с суммами без НДС
class masiv_din_double m_nds;  //Массив с суммами НДС

m_kolih.make_class(sp_shet.kolih()*sp_kod_ras.kolih());
m_suma.make_class(sp_shet.kolih()*sp_kod_ras.kolih());
m_nds.make_class(sp_shet.kolih()*sp_kod_ras.kolih());

sprintf(strsql,"select datd,nomd,datao from Ukrdok where datao >= '%04d-%02d-%02d' and \
datao <= '%04d-%02d-%02d' order by datao asc",gn,mn,dn,gk,mk,dk);
if((kolstr=cur1.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }

char imaf_prot[64];
FILE *ff_prot;

sprintf(imaf_prot,"skkp%d.lst",getpid());
if((ff_prot = fopen(imaf_prot,"w")) == NULL)
 {
  error_op_nfil(imaf_prot,errno,"");
  return(1);
 }

int kolstr1=0;
int nomer_sh;
int nomer_kr;
double kolih=0.;
double suma=0.;
int kolih_kod_ras=sp_kod_ras.kolih();
int kolstr2=0;
class iceb_tu_str shet_nds("");
while(cur1.read_cursor(&row1) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  sprintf(strsql,"select kodr,shet,kolih,cena,snds,datd,nomd,sn from Ukrdok1 \
where datd='%s' and nomd='%s'",row1[0],row1[1]);

  if((kolstr2=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return(1);
   }

  if(kolstr2 == 0)
   continue;
  while(cur.read_cursor(&row) != 0)
   {
            
    if(proverka(sheta.ravno(),row[1],1,0) != 0)
     if(proverka(sheta.ravno(),row[7],1,0) != 0)
       continue;

    if(proverka(kod_zatrat.ravno(),row[0],0,0) != 0)
      continue;

    if(kod_gr_zatrat.ravno()[0] != '\0')
     {
      SQL_str row2;
      SQLCURSOR cur2;
      //Узнаём код группы
      sprintf(strsql,"select kgr from Ukrkras where kod=%s",row[0]);
      if(readkey(strsql,&row2,&cur2) == 1)
        if(proverka(kod_gr_zatrat.ravno(),row2[0],0,0) != 0)
          continue;
     }

    fprintf(ff_prot,"%s %s %s %s %s %s %s %s %f\n",row1[2],row[5],row[6],row[0],row[1],row[2],row[3],row[4],atof(row[3])+atof(row[4]));
     
    nomer_sh=sp_shet.find(row[1]);
    nomer_kr=sp_kod_ras.find(row[0]);
    kolih=atof(row[2]);
    suma=atof(row[3]);

    m_kolih.plus(kolih,kolih_kod_ras*nomer_sh+nomer_kr);
    m_suma.plus(suma,kolih_kod_ras*nomer_sh+nomer_kr);
    
    if((suma=atof(row[4])) == 0.)
     continue;
    
    if(row[7][0] != '\0')
      shet_nds.new_plus(row[7]);
    else 
      shet_nds.new_plus(row[1]);

    nomer_sh=sp_shet.find(shet_nds.ravno());
    

    m_kolih.plus(kolih,kolih_kod_ras*nomer_sh+nomer_kr);
    m_nds.plus(suma,kolih_kod_ras*nomer_sh+nomer_kr);
          
   }
 }

char imaf[64];
FILE *ff;

sprintf(imaf,"skk%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

iceb_t_zagolov(gettext("Отчёт по счетам-кодам затрат"),dn,mn,gn,dk,mk,gk,ff);

if(sheta.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Счёт"),sheta.ravno());
if(kod_zatrat.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код затрат"),kod_zatrat.ravno());
if(kod_gr_zatrat.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код группы затрат"),kod_gr_zatrat.ravno());
 

double is_kolih,is_suma,is_suma_nds;
double suma_nds=0.;
double itogo=0.;
for(int ss=0; ss < sp_shet.kolih(); ss++)
 {
  if(proverka(sheta.ravno(),sp_shet.ravno(ss),1,0) != 0)
    continue;

  is_kolih=is_suma=is_suma_nds=itogo=0.;
  
  sprintf(strsql,"select nais from Plansh where ns='%s'",sp_shet.ravno(ss));

  if(readkey(strsql,&row,&cur) == 1)
   naim.new_plus(row[0]);
  else
   naim.new_plus("");

  fprintf(ff,"\n%s:%s %s\n",gettext("Счёт"),sp_shet.ravno(ss),naim.ravno()); 
  fprintf(ff,"\
-------------------------------------------------------------------------------\n");
  fprintf(ff,gettext("\
Код| Наименование затрат          |Количество|С-ма б.НДС|С-ма НДС  |  Итого   |\n"));
  fprintf(ff,"\
-------------------------------------------------------------------------------\n");

  for(int kk=0; kk < kolih_kod_ras; kk++)
   {
    if(proverka(kod_zatrat.ravno(),sp_kod_ras.ravno(kk),0,0) != 0)
      continue;
    
    kolih=m_kolih.ravno(kolih_kod_ras*ss+kk);
    suma=m_suma.ravno(kolih_kod_ras*ss+kk);
    suma_nds=m_nds.ravno(kolih_kod_ras*ss+kk);
  
    if(kolih == 0. && suma == 0. && suma_nds == 0.)
     continue;
    
    sprintf(strsql,"select naim from Ukrkras where kod=%s",sp_kod_ras.ravno(kk));

    if(readkey(strsql,&row,&cur) == 1)
     naim.new_plus(row[0]);
    else
     naim.new_plus("");
   
    fprintf(ff,"%-3s %-*.*s %10.10g %10.2f %10.2f %10.2f\n",
    sp_kod_ras.ravno(kk),
    iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
    kolih,suma,suma_nds,suma+suma_nds); 
    
    is_kolih+=kolih;
    is_suma+=suma; 
    is_suma_nds+=suma_nds; 
    itogo+=suma+suma_nds;
   }
  fprintf(ff,"\
-------------------------------------------------------------------------------\n");
  fprintf(ff,"%*s %10.10g %10.2f %10.2f %10.2f\n",
  iceb_tu_kolbait(34,gettext("Итого по счёту")),gettext("Итого по счёту"),is_kolih,is_suma,is_suma_nds,itogo);
  
 }

podpis(ff);

fclose(ff);
fclose(ff_prot);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Отчёт по счетам-кодам затрат"));
oth->spis_imaf.plus(imaf_prot);
oth->spis_naim.plus(gettext("Протокол расчёта"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);
 
return(0);
}
