/*$Id: snu.c,v 5.19 2013/09/26 09:43:40 sasa Exp $*/
/*28.03.2018	23.03.2005	Белых А.И.	snu.c
Свод по начислениям и удержаниям зарплаты
*/
#include        <errno.h>
#include "buhg.h"

void snu_ras(int metka,masiv_din_int *kod,double *suma,FILE *ff);
int snu_tf(short mn,short gn,short mk,short gk,const char *tabnom,class masiv_din_int *kodn,double *suma_n,class masiv_din_int *kodu,double *suma_u,const char *imaf);


int snu(class spis_oth *oth)
{
class iceb_tu_str fiov("");
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
static class iceb_tu_str podr("");
static class iceb_tu_str tabnom("");
static class iceb_tu_str shet("");
static class iceb_tu_str kategor("");
short dt,mt,gt;
int kom=0;
int kom1=0;
char strsql[512];
int tabn;

VVOD VV(0);
VVOD MENU(3);
poltekdat(&dt,&mt,&gt);
if(dat1.ravno()[0] == '\0')
 {
  sprintf(strsql,"%d.%d",mt,gt);
  dat1.new_plus(strsql);
 }
VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка свода начислений и удержаний"));

VV.VVOD_SPISOK_add_data(dat1.ravno(),8);
VV.VVOD_SPISOK_add_data(dat2.ravno(),8);
VV.VVOD_SPISOK_add_data(podr.ravno(),128);
VV.VVOD_SPISOK_add_data(tabnom.ravno(),128);
VV.VVOD_SPISOK_add_data(shet.ravno(),128);
VV.VVOD_SPISOK_add_data(kategor.ravno(),128);


VV.VVOD_SPISOK_add_MD(gettext("Дата начала.....(м.г).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца......(м.г).."));//1
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения.(,,)."));//2
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер..(,,).."));//3
VV.VVOD_SPISOK_add_MD(gettext("Счёт.............(,,).."));//4
VV.VVOD_SPISOK_add_MD(gettext("Категории........(,,).."));//5



naz:;

clear(); 

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
podr.new_plus(VV.data_ravno(2));
tabnom.new_plus(VV.data_ravno(3));
shet.new_plus(VV.data_ravno(4));
kategor.new_plus(VV.data_ravno(5));

class iceb_tu_str naim("");

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp4_9.txt");
   goto naz;

  case FK10:
  case ESC:
   return(1);

  case FK2:
  case PLU:
   break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка табельных номеров"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка категорий"));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//4

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 4 :
      case -1:
       goto naz;

      case 0 :
        if(dirzarpodr(2,&podr,&naim) == 0)
         VV.data_z_plus(2,podr.ravno());
        goto naz;

      case 1 :
      
        fiov.new_plus("");
        tabn=0;
        if(dirtab(&tabn,&fiov,0,1) == 0)
         {
          tabnom.z_plus(tabn);
          VV.VVOD_SPISOK_zapis_data(3,tabnom.ravno());
         }
        goto naz;

      case 2 :
        vibrek("Plansh",&shet,&naim);
        VV.VVOD_SPISOK_zapis_data(4,shet.ravno());
        goto naz;
      
      case 3 :
        vibrek("Kateg",&kategor,&naim);
        VV.VVOD_SPISOK_zapis_data(5,kategor.ravno());
        goto naz;


     }

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
    goto naz;
 }
GDITE();
short mn,gn;
short mk,gk;

if(rsdat1(&mn,&gn,VV.VVOD_SPISOK_return_data(0)) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата начала !"));
  goto naz;
 }

if(VV.VVOD_SPISOK_return_data(1)[0] == '\0')
 {
  mk=mn; gk=gn;
 }
else
 if(rsdat1(&mk,&gk,VV.VVOD_SPISOK_return_data(1)) != 0)
  {
   iceb_t_soob(gettext("Не верно введена дата конца !"));
   goto naz;
  }
/************расчёт********************/

int kolstr;
SQL_str row;
SQLCURSOR cur;
masiv_din_int nahis;
masiv_din_int uder;

//формируем масив начислений
sprintf(strsql,"select kod from Nash");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не введены начисления !"));
  return(1);
 }
 
while(cur.read_cursor(&row) != 0)
  nahis.plus(atoi(row[0]),-1);

double suma_nahis[nahis.kolih()];
memset(&suma_nahis,'\0',sizeof(suma_nahis));
//формируем масив удержаний
sprintf(strsql,"select kod from Uder");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не введены удержания !"));
  return(1);
 }

while(cur.read_cursor(&row) != 0)
  uder.plus(atoi(row[0]),-1);

double suma_uder[uder.kolih()];
memset(&suma_uder,'\0',sizeof(suma_uder));


sprintf(strsql,"select tabn,prn,knah,suma,shet,podr,datz from Zarp where \
datz >= '%04d-%02d-%02d' and datz <= '%04d-%02d-%02d' and suma <> 0. \
order by tabn asc",gn,mn,1,gk,mk,31);

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

int kolstr1=0;
int prn=0;
double suma=0.;
int knah=0;
int nomer=0;
short dz,mz,gz;
SQL_str row1;
class SQLCURSOR cur1;
while(cur.read_cursor(&row) != 0)
 {

  strzag(LINES-1,0,kolstr,++kolstr1);

  if(proverka(podr.ravno(),row[5],0,0) != 0)
    continue;
  
  if(proverka(tabnom.ravno(),row[0],0,0) != 0)
    continue;

  if(proverka(shet.ravno(),row[4],0,0) != 0)
    continue;

  if(kategor.ravno()[0] != '\0')
   {
    rsdat(&dz,&mz,&gz,row[6],2);    
    sprintf(strsql,"select kateg from Zarn where tabn=%s and god=%d and mes=%d",row[0],gz,mz);
    if(readkey(strsql,&row1,&cur1) == 1)
     {
      if(proverka(kategor.ravno(),row1[0],0,0) != 0)
       continue;
     }
    else
     {
      printw("%s\n",strsql);
      printw("Не найдена категория для %s\n",row[0]);
      OSTANOV();
     }
   }

  prn=atoi(row[1]);
  knah=atoi(row[2]);
  suma=atof(row[3]);  
  if(prn == 1)//начисления
   {
    if((nomer=nahis.find(knah)) < 0)
     continue;
    suma_nahis[nomer]+=suma;
   }

  if(prn == 2) //удержания
   {
    if((nomer=uder.find(knah)) < 0)
     continue;
    suma_uder[nomer]+=suma;
   }
  
 }
char imaf[64];
FILE *ff;
sprintf(imaf,"snu%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

iceb_t_zagolov(gettext("Рacчёт свода начислений и удержаний"),1,mn,gn,31,mk,gk,ff);

if(shet.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Счёт"),shet.ravno());

if(podr.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Подразделение"),podr.ravno());

if(tabnom.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Табельный номер"),tabnom.ravno());
if(kategor.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Категория"),kategor.ravno());

fprintf(ff,"\n");

snu_ras(1,&nahis,suma_nahis,ff);

fprintf(ff,"\n");

snu_ras(2,&uder,suma_uder,ff);

podpis(ff);

fclose(ff);
char imaf_tf[64];
sprintf(imaf_tf,"snutf%d.lst",getpid());

snu_tf(mn,gn,mk,gk,tabnom.ravno(),&nahis,suma_nahis,&uder,suma_uder,imaf_tf);



oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Рacчёт свода начислений и удержаний"));

oth->spis_imaf.plus(imaf_tf);
if(tabnom.getdlinna() > 1)
 oth->spis_naim.plus(gettext("Типовая форма N6"));
else
 oth->spis_naim.plus(gettext("Типовая форма N7"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);

return(0);
}
/*******************************/
/*Распечатка*/
/*******************************/

void snu_ras(int metka,//1-Начисления 2-Удержания
masiv_din_int *kod,
double *suma,
FILE *ff)
{
char strsql[512];
class iceb_tu_str naim("");
SQL_str row;
SQLCURSOR cur;

if(metka == 1)
 fprintf(ff,"%s.\n",gettext("Начисления")); 
if(metka == 2)
 fprintf(ff,"%s.\n",gettext("Удержания")); 

fprintf(ff,"\
----------------------------------------------\n");

fprintf(ff,"\
Код|     Наименование             |  Сумма   |\n");

fprintf(ff,"\
----------------------------------------------\n");
double itogo=0.;
for(int i=0; i < kod->kolih() ; i++)
 {
  if(suma[i] == 0.)
   continue;
  if(metka == 1)
   sprintf(strsql,"select naik from Nash where kod=%d",kod->ravno(i));
  if(metka == 2)
   sprintf(strsql,"select naik from Uder where kod=%d",kod->ravno(i));

  if(readkey(strsql,&row,&cur) == 1)
    naim.new_plus(row[0]);
  else
    naim.new_plus("");
    
  fprintf(ff,"%3d %-*.*s %10.2f\n",
  kod->ravno(i),
  iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
  suma[i]);
  itogo+=suma[i];
 }
fprintf(ff,"\
----------------------------------------------\n");
fprintf(ff,"%*s:%10.2f\n",iceb_tu_kolbait(34,gettext("Итого")),gettext("Итого"),itogo);

}
/***************************/
/*печать тивой формы N6,7*/
/*****************************/
int snu_tf(short mn,short gn,
short mk,short gk,
const char *tabnom,
class masiv_din_int *kodn,
double *suma_n,
class masiv_din_int *kodu,
double *suma_u,
const char *imaf)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
class iceb_tu_str naim("");

class iceb_fopen fil;
if(fil.start(imaf,"w") != 0)
 return(1);

class iceb_tu_str naim_kontr(iceb_t_get_pnk("00",0));

if(tabnom[0] != '\0')
 fprintf(fil.ff,"%-*s Типова форма N П-6\n",iceb_tu_kolbait(60,naim_kontr.ravno()),naim_kontr.ravno());
else
 fprintf(fil.ff,"%-*s Типова форма N П-7\n",iceb_tu_kolbait(60,naim_kontr.ravno()),naim_kontr.ravno());

fprintf(fil.ff,"%60s ЗАТВЕРДЖЕНО\n","");
fprintf(fil.ff,"%60s наказом Деркомстату України\n","");
fprintf(fil.ff,"%60s від 5 грудня 2008 р. N 489\n\n","");

if(tabnom[0] == '\0')
 fprintf(fil.ff,"%30sРОЗРАХУНКОВО-ПЛАТІЖНА ВІДОМІСТЬ (зведена)\n","");
else
 {
  class iceb_tu_str fio;
  class iceb_tu_str tn;
  
  fprintf(fil.ff,"%30sРОЗРАХУНКОВО-ПЛАТІЖНА ВІДОМІСТЬ ПРАЦІВНИКА\n","");
  int kolr=pole2(tabnom,',');
  if(kolr == 0)
   kolr=1;

  for(int nom=1; nom <= kolr; nom++)
   {
    if(polen(tabnom,&tn,nom,',') != 0)
     tn.new_plus(tabnom);

    sprintf(strsql,"select fio from Kartb where tabn=%d",tn.ravno_atoi());
    if(readkey(strsql,&row,&cur) == 1)
     fio.new_plus(row[0]);
    else
     fio.new_plus("");
    fprintf(fil.ff,"%30s%s %s\n","",tn.ravno(),fio.ravno());
   }       
 } 
short d;
dpm(&d,&mk,&gk,5);

fprintf(fil.ff,"%30sза період з 01.%02d.%d р. по %02d.%02d.%d р.\n","",mn,gn,d,mk,gk);

fprintf(fil.ff,"\
---------------------------------------------------------------------------------------------------------\n\
| N |    Нараховано за видами оплат                 | N |    Утримано                                   |\n\
|п/п|-----------------------------------------------|п/п|-----------------------------------------------|\n\
|   |Код|вид оплати                    | нараховано |   |Код|вид утримання                 |  утримано  |\n\
|   |   |                              |    грн.    |   |   |                              |    грн.    |\n\
---------------------------------------------------------------------------------------------------------\n");
/*
 123 123 123456789012345678901234567890 1234567890 123 123 123456789012345678901234567890 1234567890
*/
class masiv_din_int kod_nah;
class masiv_din_int kod_uder;
class masiv_din_double suma_nah;
class masiv_din_double suma_uder;

int kolih_nah=kodn->kolih();
int kolih_uder=kodu->kolih();


for(int nom=0; nom < kolih_nah; nom++)
 {
  if(suma_n[nom] == 0.)
   continue;
  kod_nah.plus(kodn->ravno(nom));     
  suma_nah.plus(suma_n[nom]);
 }
for(int nom=0; nom < kolih_uder; nom++)
 {
  if(suma_u[nom] == 0.)
   continue;
  kod_uder.plus(kodu->ravno(nom));     
  suma_uder.plus(suma_u[nom]*-1);
 }

kolih_nah=kod_nah.kolih();
kolih_uder=kod_uder.kolih();

int kol=kolih_nah;
if(kolih_nah < kolih_uder)
 kol=kolih_uder;


double itogo_n=0;
double itogo_u=0;
for(int nom=0; nom < kol; nom++)
 {
  class SPISOK sn;
  class SPISOK su;
  
  if(nom < kolih_nah)
   {
    sprintf(strsql,"select naik from Nash where kod=%d",kod_nah.ravno(nom));

    if(readkey(strsql,&row,&cur) == 1)
      naim.new_plus(row[0]);
    else
      naim.new_plus("");

    sprintf(strsql,"|%3d|%3d|%-*.*s|%12.2f|",
    nom+1,
    kod_nah.ravno(nom),
    iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
    suma_nah.ravno(nom));

    sn.plus(strsql);
    for(int nom1=30; nom1 < iceb_tu_strlen(naim.ravno()); nom1+=30)
     {
      sprintf(strsql,"|%3s|%3s|%-*.*s|%12s|",
      "",
      "",
      iceb_tu_kolbait(30,iceb_tu_adrsimv(nom1,naim.ravno())),
      iceb_tu_kolbait(30,iceb_tu_adrsimv(nom1,naim.ravno())),
      iceb_tu_adrsimv(nom1,naim.ravno()),
      "");
  
      sn.plus(strsql);
     }   

    itogo_n+=suma_nah.ravno(nom);
   
       
   }  
  else
   {
    sprintf(strsql,"|%3s|%3s|%30s|%12s|","","","",""); sn.plus(strsql);
    sn.plus(strsql);
   }

  if(nom < kolih_uder)
   {
    sprintf(strsql,"select naik from Uder where kod=%d",kod_uder.ravno(nom));
    if(readkey(strsql,&row,&cur) == 1)
     naim.new_plus(row[0]);
    else
     naim.new_plus("");

    sprintf(strsql,"%3d|%3d|%-*.*s|%12.2f|",
    nom+1,
    kod_uder.ravno(nom),
    iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
    suma_uder.ravno(nom));

    su.plus(strsql);

    for(int nom1=30; nom1 < iceb_tu_strlen(naim.ravno()); nom1+=30)
     {
      sprintf(strsql,"%3s|%3s|%-*.*s|%12s|",
      "",
      "",
      iceb_tu_kolbait(30,iceb_tu_adrsimv(nom1,naim.ravno())),
      iceb_tu_kolbait(30,iceb_tu_adrsimv(nom1,naim.ravno())),
      iceb_tu_adrsimv(nom1,naim.ravno()),
      "");
  
      su.plus(strsql);
     }   
    itogo_u+=suma_uder.ravno(nom);



   }  
  else
   {
    sprintf(strsql,"%3s|%3s|%30s|%12s|","","","","");
    su.plus(strsql);    
   } 

//  fprintf(fil.ff,"\n");
  int kolstrn=sn.kolih();
  int kolstru=su.kolih();
  int kolstr=kolstrn;
  if(kolstrn < kolstru)
   kolstr=kolstru;
  for(int nom1=0; nom1 < kolstr; nom1++)
   {
    if(nom1 < kolstrn)
     fprintf(fil.ff,"%s",sn.ravno(nom1));
    else
     fprintf(fil.ff,"|%3s|%3s|%30s|%12s|","","","","");
    if(nom1 < kolstru)
     fprintf(fil.ff,"%s\n",su.ravno(nom1));
    else
     fprintf(fil.ff,"%3s|%3s|%30s|%12s|\n","","","","");
   }   

  fprintf(fil.ff,"\
---------------------------------------------------------------------------------------------------------\n");

 }
fprintf(fil.ff,"|%*s|%12.2f|%*s|%12.2f|\n",
iceb_tu_kolbait(38,"Разом"),
"Разом",
itogo_n,
iceb_tu_kolbait(38,"Разом"),
"Разом",
itogo_u);

class iceb_t_fioruk_rk gb;

iceb_t_fioruk(2,&gb);


fprintf(fil.ff,"\
---------------------------------------------------------------------------------------------------------\n");
fprintf(fil.ff,"\n%20sГоловний бухгалтер __________________________%s\n","",gb.fio.ravno());
fprintf(fil.ff,"\n%20s         Бухгалтер __________________________%s\n","",iceb_t_getfioop());

podpis(fil.ff);

return(0);

}
