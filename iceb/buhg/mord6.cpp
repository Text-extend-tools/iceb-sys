/*$Id: mord6.c,v 5.13 2013/09/26 09:43:35 sasa Exp $*/
/*15.07.2015	04.09.2007	Белых А.И.	mord4.c
Расчёт и распечатка мемориальных ордеров 6 и 7
Записи оплаты по документам показываются в дебетовой колонке
Проводки к документам в кредитовой колонке
*/
#include <math.h>
#include <errno.h>
#include "buhg.h"

extern short	startgodb; /*Стартовый год*/


int mord6(int nom_ord,class spis_oth *oth)
{
char strsql[512];
static class iceb_tu_str shet_or("");
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
int kom=0;

if(nom_ord == 6)
 shet_or.new_plus("675");
if(nom_ord == 7)
 shet_or.new_plus("361");

VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
if(nom_ord == 6)
  VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт мемориального ордера N6")); /*Оплата по кредиту*/
if(nom_ord == 7)
  VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт мемориального ордера N7")); /*Оплата по кредиту*/

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


sprintf(strsql,"select distinct kto,pod,nomd,datd,kekv from Prov where \
datp >= '%04d-%02d-%02d' and datp <= '%04d-%02d-%02d' and sh like '%s%%' and kto <> '' order by datp asc",
gn,1,1,gk,mk,dk,shet_or.ravno());

int kolstr=0;

class SQLCURSOR cur;
class SQLCURSOR cur1;
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
FILE *ff;
FILE *ff_prot;

char imaf_prot[64];
sprintf(imaf_prot,"mordp%d_%d.lst",nom_ord,getpid());

if((ff_prot = fopen(imaf_prot,"w")) == NULL)
 {
  error_op_nfil(imaf_prot,errno,"");
  return(1);
 }
fprintf(ff_prot,"Протокол хода расчёта\n");
char imaf[64];
sprintf(imaf,"mord%d_%d.lst",nom_ord,getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

fprintf(ff,"%s\n",iceb_t_get_pnk("00",0));

if(nom_ord == 6)
 fprintf(ff,"%32sТипова форма N 409-авт (бюджет)\n","");
if(nom_ord == 6)
 fprintf(ff,"%32sТипова форма N 410-авт (бюджет)\n","");

iceb_t_insfil("mord_start.alx",ff);

fprintf(ff,"\n%15s Меморіальний ордер N%d\n","",nom_ord);

fprintf(ff,"%5s за період з %02d.%02d.%d р. по %02d.%02d.%d р.\n","",dn,mn,gn,dk,mk,gk);

if(nom_ord == 6)
  fprintf(ff,"  Накопичувальна відомість за розрахунками з іншими кредіторами\n");
if(nom_ord == 7)
  fprintf(ff,"  Накопичувальна відомість за розрахунками в порядку планових платежів\n");


//fprintf(ff,"\x1B\x4D"); /*12-знаков*/
//fprintf(ff,"\x0F");  /*Ужатый режим*/

fprintf(ff,"\n\n\
--------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
N з/п|КЕКВ|Назва організації   |Залишок на початок   |Дебет субрахунку %-23.23s|Кредіт субрахунку %-22.22s|Залишок на кінець    |\n\
     |    |(установи) дата, N  |місяця за субрахунком|дата, N документа                       |дата, N документа                       |місяця за субрахунком|\n\
     |    |документа           |%-21.21s|кор. субсчёт - сумма, другі записи:   |кор. субсчёт - сумма, другі записи:   |%-21.21s|\n\
     |    |                    |---------------------| Д, К, сумма                             | Д, К, сумма                             |---------------------|\n\
     |    |                    | Дебет    |  Кредіт  |                                        |                                        |  Дебет   | Кредіт   |\n",
shet_or.ravno(),shet_or.ravno(),shet_or.ravno(),shet_or.ravno());

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

/*
12345 1234 12345678901234567890 1234567890 1234567890 1234567890123456789012345678901234567890 1234567890123456789012345678901234567890 1234567890 1234567890 
*/
int kolstr1=0;

SQL_str row,row1;
short d,m,g;
double suma=0.;
int metka_ps=0;
class iceb_tu_str kontr("");
class iceb_tu_str naim_kontr("");
int nom_zap=0;
int kolstr2=0;
class iceb_tu_str suma_char("");
int kolsimv=20;
double i_start_deb=0.;
double i_start_kre=0.;
double i_obor_deb=0.;
double i_obor_kre=0.;

class SPISOK i_sheta_oplati; /*Список счетов из таблицы Opldok в которую записываются сроки оплат по документам*/
class masiv_din_double i_sheta_oplati_sum; /*Суммы к счетам оплаты*/

/*Список счетов с которыми кореспондирует заданный в меню счёт и проводка в кредит этого счёта*/
class SPISOK i_sheta_prov1_kre; 
class masiv_din_double i_sheta_prov1_kre_sum; /*Суммы*/

/*Список остальных счетов в проводках по этому документа кредитовых*/
class SPISOK i_sheta_prov2_kre;
class masiv_din_double i_sheta_prov2_kre_sum;

/*Список остальных счетов в проводках по этому документа дубитовых*/
class SPISOK i_sheta_prov2_deb;
class masiv_din_double i_sheta_prov2_deb_sum;

int nomer_vsp=0;
class iceb_tu_str nomdok_post("");

while(cur.read_cursor(&row) != 0)
 {
  fprintf(ff_prot,"\n\nДокумент:%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
  strzag(LINES-1,0,kolstr,++kolstr1);

  metka_ps=0;
  if(SRAV(ICEB_MP_MATU,row[0],0) == 0)
   metka_ps=1;
  if(SRAV(ICEB_MP_USLUGI,row[0],0) == 0)
   metka_ps=2;
  if(SRAV(ICEB_MP_UOS,row[0],0) == 0)
   metka_ps=3;
  if(metka_ps == 0)
    continue;

  double start_deb=0.;
  double start_kre=0.;
  
  double obor_deb=0.;
  double obor_kre=0.;
  

  class SPISOK prov1; //Дата|счёт|счёт корреспондент|сумма - проводки по счёту ордеру

  class SPISOK prov2; //Дата|счёт|счёт корреспондент|сумма - проводки по другим счетам
  class SPISOK oplata; //Дата счёт сумма номер документа

  sprintf(strsql,"select datp,sh,shk,kre from Prov where kto='%s' and pod=%s and nomd='%s' and datd='%s'\
and kekv=%s and kre <> 0. order by datp asc",row[0],row[1],row[2],row[3],row[4]);
  
  fprintf(ff_prot,"%s\n",strsql);

  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }

  while(cur1.read_cursor(&row1) != 0)
   {
    fprintf(ff_prot,"%s %s %s %s\n",row1[0],row1[1],row1[2],row1[3]);
    rsdat(&d,&m,&g,row1[0],2);
    
    suma=atof(row1[3]);
    
    sprintf(strsql,"%d.%d.%d|%s|%s|%.2f",d,m,g,row1[1],row1[2],suma);
    
    if(SRAV(shet_or.ravno(),row1[1],1) == 0 )
     {
      fprintf(ff_prot,"shet_or-%s %s %s %s\n",row1[0],row1[1],row1[2],row1[3]);

      if(sravmydat(d,m,g,dn,mn,gn) < 0)
        start_kre+=suma;

      if(sravmydat(d,m,g,dn,mn,gn) >= 0 && sravmydat(d,m,g,dk,mk,gk) <= 0)
       {

        obor_kre+=suma;

        prov1.plus(strsql);

        if((nomer_vsp=i_sheta_prov1_kre.find(row1[2])) < 0)
         i_sheta_prov1_kre.plus(row1[2]);

        i_sheta_prov1_kre_sum.plus(suma,nomer_vsp);
       }
     }
    else
     {
      if(sravmydat(d,m,g,dn,mn,gn) >= 0 && sravmydat(d,m,g,dk,mk,gk) <= 0)
       {
        fprintf(ff_prot,"попало в диапазон дат-%s %s %s %s\n",row1[0],row1[1],row1[2],row1[3]);

        prov2.plus(strsql);

        if((nomer_vsp=i_sheta_prov2_kre.find(row1[2])) < 0)
         i_sheta_prov2_kre.plus(row1[2]);

        i_sheta_prov2_kre_sum.plus(suma,nomer_vsp);

        if((nomer_vsp=i_sheta_prov2_deb.find(row1[1])) < 0)
         i_sheta_prov2_deb.plus(row1[1]);

        i_sheta_prov2_deb_sum.plus(suma,nomer_vsp);
       }      
     }
   }
  
   
  /*Смотрим список оплат*/
  if(metka_ps == 3) /*в учёте основных средств общая нумерация документов не зависящая от подразделения*/
   sprintf(strsql,"select dato,shet,suma,nomp from Opldok where ps=%d and datd='%s' and pd=%d and nomd='%s' \
and kekv=%s  order by dato asc",metka_ps,row[3],0,row[2],row[4]);
  else
   sprintf(strsql,"select dato,shet,suma,nomp from Opldok where ps=%d and datd='%s' and pd=%s and nomd='%s' \
and kekv=%s  order by dato asc",metka_ps,row[3],row[1],row[2],row[4]);
   
  fprintf(ff_prot,"%s\n",strsql);
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }

  fprintf(ff_prot,"kolstr2=%d\n",kolstr2);

  while(cur1.read_cursor(&row1) != 0)
   {
     fprintf(ff_prot,"%s %s %s %s\n",row1[0],row1[1],row1[2],row1[3]);
     rsdat(&d,&m,&g,row1[0],2);
     
     suma=atof(row1[2]);
     sprintf(strsql,"%d.%d.%d|%s|%.2f|%s",d,m,g,row1[1],suma,row1[3]);

     if(sravmydat(d,m,g,dn,mn,gn) >= 0 && sravmydat(d,m,g,dk,mk,gk) <= 0)
      {
       fprintf(ff_prot,"Подходит\n");
       oplata.plus(strsql);     
       obor_deb+=suma;

       if((nomer_vsp=i_sheta_oplati.find(row1[1])) < 0)
        i_sheta_oplati.plus(row1[1]);

       i_sheta_oplati_sum.plus(suma,nomer_vsp);

      }
      
     if(sravmydat(d,m,g,dn,mn,gn) < 0)
      start_deb+=suma;

   }  

//  if(start_deb == start_kre && prov1.kolih() == 0 && prov2.kolih() == 0 && prov2.kolih() == 0)
  if(fabs(start_deb - start_kre) < 0.01 && prov1.kolih() == 0 && prov2.kolih() == 0 && prov2.kolih() == 0)
   continue;
  
  //читаем код контрагента в шапке документа
  if(metka_ps == 1)
   sprintf(strsql,"select kontr,nomon from Dokummat where datd='%s' and sklad=%s and nomd='%s'",
   row[3],row[1],row[2]);
  if(metka_ps == 2)
   sprintf(strsql,"select kontr,nomdp from Usldokum where datd='%s' and podr=%s and nomd='%s'",
   row[3],row[1],row[2]);
  if(metka_ps == 3)
   sprintf(strsql,"select kontr,nomdv from Uosdok where datd='%s' and nomd='%s'",
   row[3],row[2]);
  nomdok_post.new_plus("");
  kontr.new_plus("");
  if(readkey(strsql,&row1,&cur1) == 1)
   {
    kontr.new_plus(row1[0]);
    nomdok_post.new_plus(row1[1]);
   }

  //узнаём наименование контрагента   
  naim_kontr.new_plus("");
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr.ravno());
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_kontr.new_plus(row1[0]);

  if(start_deb != start_kre)
   {
    naim_kontr.plus(", ",nomdok_post.ravno());
   }

  fprintf(ff,"%5d|%*s|%*.*s|",
  ++nom_zap,
  iceb_tu_kolbait(4,row[4]),row[4],
  iceb_tu_kolbait(20,naim_kontr.ravno()),iceb_tu_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno());

  if(start_deb > start_kre)
   fprintf(ff,"%10.2f|%10s|",start_deb-start_kre,"");
  else
   fprintf(ff,"%10s|%10.2f|","",start_kre-start_deb);
    
  fprintf(ff,"%40.2f|%40.2f|",obor_deb,obor_kre);
  
  if(start_deb+obor_deb > start_kre+obor_kre)
   fprintf(ff,"%10.2f|%10s|\n",start_deb+obor_deb - start_kre-obor_kre,"");
  else
   fprintf(ff,"%10s|%10.2f|\n","",start_kre+obor_kre-start_deb-obor_deb);


  i_start_deb+=start_deb;
  i_start_kre+=start_kre;
  i_obor_deb+=obor_deb;
  i_obor_kre+=obor_kre;   
     
  int kolih_deb=oplata.kolih();
  int kolih_kre=prov1.kolih();
  
  int kolih_dr=prov2.kolih();
  int nomer_deb=0;
  int nomer_kre=0;
  int nomer_dr=0;

  char debet_shet[2][64];
  char kredit_shet[2][64];
  char data[2][64];
  char nomdokum[2][64];
  double sum_prov[2];
  int metka_rsdr=0;  
  for(int ii=1;;ii++)
   {
    int metka_kre=0; 
    memset(debet_shet,'\0',sizeof(debet_shet));    
    memset(kredit_shet,'\0',sizeof(kredit_shet));    
    memset(data,'\0',sizeof(data));    
    memset(nomdokum,'\0',sizeof(nomdokum));    
    sum_prov[0]=sum_prov[1]=0.;
    if(nomer_deb < kolih_deb)
     {
      polen(oplata.ravno(nomer_deb),data[0],sizeof(data),1,'|');
      polen(oplata.ravno(nomer_deb),kredit_shet[0],sizeof(kredit_shet[0]),2,'|');
      polen(oplata.ravno(nomer_deb),&sum_prov[0],3,'|');
      polen(oplata.ravno(nomer_deb),nomdokum[0],sizeof(nomdokum[0]),4,'|');
      strncpy(debet_shet[0],shet_or.ravno(),sizeof(debet_shet[0])-1);          
      nomer_deb++;
     }

    if(nomer_kre < kolih_kre)
     {
      polen(prov1.ravno(nomer_kre),data[1],sizeof(data),1,'|');
      polen(prov1.ravno(nomer_kre),kredit_shet[1],sizeof(kredit_shet[1]),3,'|');
      polen(prov1.ravno(nomer_kre),debet_shet[1],sizeof(kredit_shet[1]),2,'|');
      polen(prov1.ravno(nomer_kre),&sum_prov[1],4,'|');

      strncpy(nomdokum[1],row[2],sizeof(nomdokum[1])-1);
      nomer_kre++;
      metka_kre++;
     }
    
    if(metka_kre == 0 && nomer_kre == kolih_kre && nomer_dr < kolih_dr)
     {
      metka_rsdr++;
      if(metka_rsdr > 1)
       {        
        polen(prov2.ravno(nomer_dr),data[1],sizeof(data),1,'|');
        polen(prov2.ravno(nomer_dr),kredit_shet[1],sizeof(kredit_shet[1]),3,'|');
        polen(prov2.ravno(nomer_dr),debet_shet[1],sizeof(debet_shet[1]),2,'|');
        polen(prov2.ravno(nomer_dr),&sum_prov[1],4,'|');

        strncpy(nomdokum[1],row[2],sizeof(nomdokum[1])-1);
        nomer_dr++;
       }
     }

    if(iceb_tu_strlen(naim_kontr.ravno()) > kolsimv*ii)
     fprintf(ff,"     |    |%-*.*s|          |          |",
     iceb_tu_kolbait(20,iceb_tu_adrsimv(kolsimv*ii,naim_kontr.ravno())),
     iceb_tu_kolbait(20,iceb_tu_adrsimv(kolsimv*ii,naim_kontr.ravno())),
     iceb_tu_adrsimv(kolsimv*ii,naim_kontr.ravno()));
    else
     fprintf(ff,"     |    |                    |          |          |");

    suma_char.new_plus("");
    if(sum_prov[0] != 0.)
     suma_char.new_plus(sum_prov[0]);
     
    sprintf(strsql,"%s %s %s %s",data[0],debet_shet[0],kredit_shet[0],nomdokum[0]);
    fprintf(ff,"%-*.*s %10s|",
    iceb_tu_kolbait(29,strsql),iceb_tu_kolbait(29,strsql),strsql,
    suma_char.ravno());
    
    suma_char.new_plus("");
    if(sum_prov[1] != 0.)
     suma_char.new_plus(sum_prov[1]);

    sprintf(strsql,"%s %s %s %s %s",data[1],debet_shet[1],kredit_shet[1],nomdokum[1],nomdok_post.ravno());
     
    if(metka_rsdr != 1)
      fprintf(ff,"%-*.*s %10s|",
    iceb_tu_kolbait(29,strsql),iceb_tu_kolbait(29,strsql),strsql,
      suma_char.ravno());
    else
      fprintf(ff,"%-*s|",iceb_tu_kolbait(40,"Другі записи:"),"Другі записи:");
     
    fprintf(ff,"          |          |\n");
 
    if(nomer_deb == kolih_deb && nomer_kre == kolih_kre && nomer_dr == kolih_dr)
     break;
   }

  fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
   
 }
fprintf(ff,"%*s|",iceb_tu_kolbait(31,"Разом"),"Разом");

if(i_start_deb > i_start_kre)
 fprintf(ff,"%10.2f|%10s|",i_start_deb-i_start_kre,"");
else
 fprintf(ff,"%10s|%10.2f|","",i_start_kre-i_start_deb);
 
fprintf(ff,"%40.2f|%40.2f|",i_obor_deb,i_obor_kre);

if(i_start_deb+i_obor_deb > i_start_kre+i_obor_kre)
 fprintf(ff,"%10.2f|%10s|\n",i_start_deb+i_obor_deb-i_start_kre-i_obor_kre,"");
else
 fprintf(ff,"%10s|%10.2f|\n","",i_start_kre+i_obor_kre-i_start_deb-i_obor_deb);

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------\n");


 
fprintf(ff,"\nУсього до кредіту субрахунків %s: Субсчёт     Сумма\n",shet_or.ravno());
double itogo=0.;                               
for(int ii=0; ii < i_sheta_prov1_kre.kolih(); ii++)
 {
  fprintf(ff,"%34s %-*s %10.2f\n",
  "",
  iceb_tu_kolbait(10,i_sheta_prov1_kre.ravno(ii)),i_sheta_prov1_kre.ravno(ii),
  i_sheta_prov1_kre_sum.ravno(ii));
  itogo+=i_sheta_prov1_kre_sum.ravno(ii);
  
 }

fprintf(ff,"%34s ---------------------\n","");
fprintf(ff,"%34s %-*s %10.2f\n",
"",
iceb_tu_kolbait(10,"Разом"),"Разом",
itogo);


fprintf(ff,"\nУсього до дебету субрахунків %s: Субсчёт     Сумма\n",shet_or.ravno());
itogo=0.;
for(int ii=0; ii < i_sheta_oplati.kolih(); ii++)
 {
  fprintf(ff,"%34s %-*s %10.2f\n",
  "",
  iceb_tu_kolbait(10,i_sheta_oplati.ravno(ii)),i_sheta_oplati.ravno(ii),
  i_sheta_oplati_sum.ravno(ii));
  
  itogo+=i_sheta_oplati_sum.ravno(ii);
  
 }



fprintf(ff,"\nДругі записи: под дебету субрахунків: Субсчёт     Сумма\n");
itogo=0.;                               
for(int ii=0; ii < i_sheta_prov2_deb.kolih(); ii++)
 {
  fprintf(ff,"%34s %-*s %10.2f\n",
  "",
  iceb_tu_kolbait(10,i_sheta_prov2_deb.ravno(ii)),i_sheta_prov2_deb.ravno(ii),
  i_sheta_prov2_deb_sum.ravno(ii));
  
  itogo+=i_sheta_prov2_deb_sum.ravno(ii);
  
 }

fprintf(ff,"%34s ---------------------\n","");
fprintf(ff,"%34s %-*s %10.2f\n",
"",
iceb_tu_kolbait(10,"Разом"),"Разом",
itogo);


fprintf(ff,"\nДругі записи: по кредіту субрахунків: Субсчёт     Сумма\n");
itogo=0.;
for(int ii=0; ii < i_sheta_prov2_kre.kolih(); ii++)
 {
  fprintf(ff,"%34s %-*s %10.2f\n",
  "",
  iceb_tu_kolbait(10,i_sheta_prov2_kre.ravno(ii)),i_sheta_prov2_kre.ravno(ii),
  i_sheta_prov2_kre_sum.ravno(ii));
  
  itogo+=i_sheta_prov2_kre_sum.ravno(ii);
  
 }

fprintf(ff,"%34s ---------------------\n","");
fprintf(ff,"%34s %-*s %10.2f\n",
"",
iceb_tu_kolbait(10,"Разом"),"Разом",
itogo);


fprintf(ff,"\n\n");

iceb_t_insfil("mord_end.alx",ff);

podpis(ff);


fclose(ff);
fclose(ff_prot);

oth->spis_imaf.plus(imaf);
sprintf(strsql,"Меморіальний ордер N%d",nom_ord);
oth->spis_naim.plus(strsql);

oth->spis_imaf.plus(imaf_prot);
oth->spis_naim.plus("Протокол хода розрахунку");

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),0);

return(0);

}

