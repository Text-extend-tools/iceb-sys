/*$Id: zarkvrt1.c,v 5.4 2013/09/26 09:43:45 sasa Exp $*/
/*17.05.2018	29.04.2011	Белых А.И.	zarkvrt.c
Расчёт формы 1ДФ
*/
#include <errno.h>
#include "buhg.h"
#include "sprkvrt1.h"

class zarkvrt_mas /*для подоходного налога*/
 {
  public:
  class SPISOK mesgod_koddoh;  /*месяц год в рамках квартала|код дохода */
                              /*Для начислений и обязательных удержаний это дата начисления. Для выплат это дата в счёт которой выплачена сумма*/
  class masiv_din_double dohod_nah;
  class masiv_din_double dohod_vip;
  class masiv_din_double podoh_nah;
  class masiv_din_double podoh_vip;  
 };

class zarkvrt_vs /*для военного сбора*/
 {
  public:
  class SPISOK mesgod;  /*месяц год - дата в счёт которой выплачены суммы*/
  class masiv_din_double dohod_nah; /*пока не используется. понадобится если нужно будет налог показывать по каждому работнику*/
  class masiv_din_double dohod_vip;
  class masiv_din_double vs_nah;    /*пока не используется*/
  class masiv_din_double vs_vip;  
 };

/*данные за месяц в счёт которого была сделана выплата*/
class zarkvrt_mes_vipl
 {
  public:
   class masiv_din_int koddoh;
   class masiv_din_double suma_nah_kod;
  
   double suma_nah; /*общая сумма начисленй*/
   double suma_obud; /*Cумма обязательных удержаний*/

   double suma_nah_vs; /*Сумма начислений входящих в расчёт для военного сбора*/
   
   double suma_podoh_nah; /*Начисленная сумма подоходного налога*/
   zarkvrt_mes_vipl()
    {
     clear();
    }
   void clear()
    {
     suma_nah=0.;
     suma_obud=0.;
     suma_podoh_nah=0.;
     suma_nah_vs=0.;
    }
 };
 

class zarkvrt_poi
 {
  public:
   class iceb_tu_str kvrt;
   class iceb_tu_str datan;
   class iceb_tu_str datak;
   class iceb_tu_str tabnom;
   class iceb_tu_str podr;
   zarkvrt_poi()
    {
     clear();
    }

   void clear()
    {
     kvrt.new_plus("");
     datan.new_plus("");
     datak.new_plus("");
     tabnom.new_plus("");
     podr.new_plus("");
    }


 };

void  zarsdf1df(class sprkvrt1_nast *nast);
void zarkvrt1_zp(short m,short g,int kod_doh,class zarkvrt_mas *mas,double dohod_nah,double dohod_vip,double podoh_nah,double podoh_vip,FILE *ff_prot);
void zarkvrt1_vs(short m,short g,class zarkvrt_vs *vsbor,double dohod_nah,double dohod_vip,double vs_nah,double vs_vip,FILE *ff_prot);
int zarkvrt_gk(short dn,short mn,short gn,short dk,short mk,short gk,const char *shkas_shkom,const char *prist_k_tabnom,int kod_doh,const char *imatmptab,FILE *ff_prot);
double zarkvrt1_esv(int tabn,short mn,short gn,short dk,short mk,short gk,const char *gosstrah,int metka);
int zarkvrt1_kdfpn(short mr,short gr,class zarkvrt_mas *mas,FILE *ff_prot);
void zarkvrt1_mes(int tabn,short m,short g,double suma_viplati,short mv,short gv,class zarkvrt_mas *mas,class sprkvrt1_nast *nast,double *koef_vip_nah,double *suma_k_viplate,class zarkvrt_vs *vsbor,FILE *ff_prot);
void zarkvrt_pr(class zarkvrt_mas *mas,class sprkvrt1_nast *nast,FILE *ff_prot);
void zarkvrt1_kr(short mn,short gn,short mk,short gk,const char *imatmptab,class sprkvrt1_nast *nast);


extern class iceb_tu_str kodpn_all; /*Все коды подоходного налога*/
extern short *obud; /*Обязательные удержания*/
extern short *knvr;/*Коды начислений не входящие в расчёт подоходного налога*/
extern short kodvn; /*код военного налога*/
extern short kodvs_b; /*код военного сбора с больничного*/
extern class iceb_tu_str kodvn_nvr; /*Коды не входящие в расчёт военного сбора*/
extern class iceb_tu_str kod_zv_gr; /*коды званий гражданских*/
int kod_doh_tek=0.; 

int zarkvrt1(class spis_oth *oth)
{
int kom1=0;
char strsql[2048];
static class zarkvrt_poi rp;
class iceb_tu_str kod("");
class iceb_tu_str naim("");
int tbn=0;
class iceb_tu_str fio("");
const int kod_doh_for_atestov=185; /*Код дохода для атестованых*/
const int kod_doh_for_neatestov=101; /*код дохода для неатестованых*/

VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт справки за квартал (форма N1ДФ)"));


VV.VVOD_SPISOK_add_data(rp.kvrt.ravno(),2);
VV.VVOD_SPISOK_add_data(rp.datan.ravno(),11);
VV.VVOD_SPISOK_add_data(rp.datak.ravno(),11);
VV.VVOD_SPISOK_add_data(rp.tabnom.ravno(),128);
VV.VVOD_SPISOK_add_data(rp.podr.ravno(),128);

VV.VVOD_SPISOK_add_MD(gettext("Квартал (1,2,3,4)........."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата начала...(д.м.г)....."));//1
VV.VVOD_SPISOK_add_MD(gettext("Дата конца....(д.м.г)....."));//2
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер..(,,)....."));//3
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения..(,,)..."));//4

naz:;
clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("таб.номер"),
"F4",gettext("очистить"),
"F5",gettext("настройка"),
"F10",gettext("выход"),NULL);


int kom=VV.vvod(0,1,1,-1,-1);

rp.kvrt.new_plus(VV.data_ravno(0));
rp.datan.new_plus(VV.data_ravno(1));
rp.datak.new_plus(VV.data_ravno(2));
rp.tabnom.new_plus(VV.data_ravno(3));


switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp4_7_1_1.txt");
   goto naz;

  case FK2:
  case PLU:
    break;
      
  case FK3: //Просмотр списка табельных номеров
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка табельных номеров"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 2 :
      case -1:
       goto naz;

      case 0:
       tbn=0;
       fio.new_plus("");
       if(dirtab(&tbn,&fio,0,1) == 0)
        {
         VV.data_z_plus(3,tbn);
        }    
       break;
      case 1:
       kod.new_plus("");
       naim.new_plus("");
       if(dirzarpodr(2,&kod,&naim) == 0)
        VV.data_z_plus(4,kod.ravno());
       break;
     }
    goto naz;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;

  case FK5 :  
    iceb_redfil("zarsdf1df.alx",0);
    goto naz;

  case FK10:
  case ESC:
    return(1);

  default:
    goto naz;
 }
short dn=0,mn=0,gn=0;
short dk=0,mk=0,gk=0;

if(rsdatp(&dn,&mn,&gn,rp.datan.ravno(),&dk,&mk,&gk,rp.datak.ravno()) != 0)
 goto naz;

if(rp.kvrt.ravno_atoi() < 1 && rp.kvrt.ravno_atoi() > 4 )
 {
  iceb_t_soob(gettext("Не правильно ввели номер квартала!"));
  return(1);
 }

short dkk=0; /*последний день месяца*/
dpm(&dkk,&mk,&gk,5);


time_t time_n=time(NULL);
char imaf_1df[64];
char imaf_1df_fam[64];
char imaf_prot[64];
char imaf_dbf[64];
char imaf_dbf_tmp[64];

char imaf_xml[64];
int kolstr=0;
int kolstr1=0;
class iceb_tu_str inn("");
SQL_str row,row1;
class SQLCURSOR cur,cur1;
short d,m,g;
class iceb_tu_str datanr(""); /*дата начала работы*/
class iceb_tu_str datakr(""); /*дата конца работы*/
class iceb_tu_str datanr133(""); /*дата начала работы для записи*/
class iceb_tu_str datakr133(""); /*дата конца работы для записи*/

int kolih_rab=0;
int kolih_dog_pod=0;
int tabnom=0;
int nomer_v_sp=0;
int metka_dog_podr=0;
int kod_doh=kod_doh_for_neatestov;
int kod_doh_for_podoh=0; /*Код дохода для подоходного налога*/

double suma_vs_tn=0.; /*Cумма военного сбора начисленная по табельному номеру*/
double suma_nah_fvs_tn=0.; /*Cумма начислений с которых брался военный сбор по табельному номеру*/

double suma_vs_vip_tn=0.; /*Cумма военного сбора выплаченная по табельному номеру*/
double suma_nah_fvs_vip_tn=0.; /*Cумма сумма выплаченная начислений с которых брался военный сбор по табельному номеру*/

double suma_vs=0.; /*Cумма военного сбора начисленная итого*/
double suma_nah_fvs=0.; /*Cумма начислений с которых брался военный сбор итого*/

double suma_vs_vip=0.; /*Cумма военного сбора выплаченная*/
double suma_nah_fvs_vip=0.; /*Cумма сумма выплаченная начислений с которых брался военный сбор итого*/


//читаем список кодов доходов 
class sprkvrt1_nast nast;
zarsdf1df(&nast);




sprintf(strsql,"select distinct tabn from Zarp where datz >= '%04d-%02d-%02d' and \
datz <= '%04d-%02d-%02d' and suma != 0. order by tabn asc",gn,mn,1,gk,mk,dk);

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

class iceb_fopen fil_prot;

sprintf(imaf_prot,"zkvrt%d.lst",getpid());

if(fil_prot.start(imaf_prot,"w") != 0)
 return(1);

class iceb_t_tmptab tabtmp;
const char *imatmptab={"zarkvrt"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
inn char(16) not null default '',\
pd int not null default 0,\
ndoh double(10,2) not null default 0.,\
vdoh double(10,2) not null default 0.,\
npod double(8,2) not null default 0.,\
vpod double(8,2) not null default 0.,\
datap DATE NOT NULL DEFAULT '0000-00-00',\
datau DATE NOT NULL DEFAULT '0000-00-00',\
kl char(8) NOT NULL DEFAULT '',\
fio char(128) NOT NULL DEFAULT '',\
tn char(32) NOT NULL DEFAULT '',\
unique (inn,pd,kl)) ENGINE = MYISAM",
imatmptab);

/*********************
 0 inn   - индивидуальный налоговый номер
 1 pd    - признак дохода
 2 ndoh  - начисленный доход
 3 vdoh  - выплаченый доход
 4 npod  - начисленный подоходный налог
 5 vpod  - выплаченный подоходный налог
 6 datap - дата приёма на работу
 7 datau - дата увольнения с работы
 8 kl    - код льготы
 9 fio   - фамилия / наименование контрагента
10 tn    - табельный номер/код контрагента
***********************/

if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 return(-1);
clear();
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(rp.tabnom.ravno(),row[0],0,0) != 0)
   continue;
  
  tabnom=atoi(row[0]);
  sprintf(strsql,"select fio,datn,datk,sovm,inn,podr,zvan from Kartb where tabn=%s",row[0]);
  if(readkey(strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s %s!",gettext("Не найдена карточка для табельного номера"),row[0]);
    iceb_t_soob(strsql);
    continue;
   }  
  
  if(proverka(rp.podr.ravno(),row1[5],0,0) != 0)
   continue;    

  
  if(kod_zv_gr.getdlinna() <= 1 || row1[6] == NULL)
   {
    kod_doh_tek=kod_doh_for_neatestov;
   }
  else
   {
//    if(atoi(row1[6]) == kodzv_nat || atoi(row1[6]) == 0)
    if(proverka(kod_zv_gr.ravno(),row1[6],0,1) == 0 || atoi(row1[6]) == 0)
     kod_doh_tek=kod_doh_for_neatestov;
    else
     kod_doh_tek=kod_doh_for_atestov;
   }

  kod_doh=kod_doh_tek;
     
  suma_vs_tn=0.;  /*Cумма военного сбора начисленная по табельному номеру*/
  suma_nah_fvs_tn=0.; /*Cумма начислений с которых брался военный сбор по табельному номеру*/

  suma_vs_vip_tn=0.; /*Cумма военного сбора выплаченная по табельному номеру*/
  suma_nah_fvs_vip_tn=0.; /*Cумма сумма выплаченная начислений с которых брался военный сбор по табельному номеру*/

  fio.new_plus(row1[0]);
  inn.new_plus(row1[4]); 
  printw("%6s %s\n",row[0],fio.ravno());
  fprintf(fil_prot.ff,"\n%s %s\n---------------------------------\nДата приёма на работу:%s Дата увольнения:%s\n",row[0],fio.ravno(),row1[1],row1[2]);
  
  if(inn.getdlinna() <= 1)
   {
    fprintf(fil_prot.ff,"Не введён идентификационной номер!!!!!!!\n");
    continue;
   }

  datanr.new_plus("");
  datakr.new_plus("");
  datanr133.new_plus("");
  datakr133.new_plus("");

  rsdat(&d,&m,&g,row1[1],2);
  if(sravmydat(d,m,g,1,mn,gn) >= 0 && sravmydat(d,m,g,31,mk,gk) <= 0)
   datanr.new_plus(row1[1]);     

  rsdat(&d,&m,&g,row1[2],2);
  if(sravmydat(d,m,g,1,mn,gn) >= 0 && sravmydat(d,m,g,31,mk,gk) <= 0)
   datakr.new_plus(row1[2]);     
  
  datanr133.new_plus(datanr.ravno());
  datakr133.new_plus(datakr.ravno());
    
  metka_dog_podr=zarprtn(mn,gn,atoi(row[0]),3);  

  
   
  /*Смотрим все записи  в диапазоне дат День начала обязательно первый день месяца начала */
  sprintf(strsql,"select datz,prn,knah,suma,godn,mesn from Zarp where tabn=%d and datz >= '%04d-%02d-%02d' and \
datz <= '%04d-%02d-%02d' and suma <> 0. order by prn asc,datz asc",tabnom,gn,mn,1,gk,mk,dk);

  if(cur1.make_cursor(&bd,strsql) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue; 
   }


  class SPISOK mesn_godn; /*список дат в счёт которых были выплаты|месяц когда выплачено*/
  class masiv_din_double suma_viplat; /*Сумма выплат к списку дат*/
  class zarkvrt_mas mas;
  class zarkvrt_vs vsbor;
  kod_doh_for_podoh=0; /*Код дохода для подоходного налога*/
  int knah=0;
  double suma=0.;
  
  /*определяем начисленный доход и подоходный налог и все выплаты*/
  while(cur1.read_cursor(&row1) != 0)
   {
    rsdat(&d,&m,&g,row1[0],2);
    knah=atoi(row1[2]);
    suma=atof(row1[3]);
//    fprintf(fil_prot.ff,"%s %s %s %s %s %s\n",row1[0],row1[1],row1[2],row1[3],row1[4],row1[5]);
    if(atoi(row1[1]) == 1) /*начисления*/    
     {
      //коды которые не входят в 1ДФ
      if(provkod(nast.kodnvf8dr,knah) >= 0)
          continue; 

      if(sravmydat(d,m,g,1,mn,gn) >= 0 && sravmydat(d,m,g,31,mk,gk) <= 0) /*Дата строго в рамках квартала*/
       {
        kod_doh=kod_doh_tek;
        for(int nom=0; nom < nast.kodi_nah.kolih(); nom++)
         {
          if(proverka(nast.kodi_nah.ravno(nom),knah,0,1) == 0)
            kod_doh=nast.kodi_pd.ravno(nom);         
         }          
        zarkvrt1_zp(m,g,kod_doh,&mas,suma,0.,0.,0.,fil_prot.ff);
        zarkvrt1_vs(m,g,&vsbor,suma,0.,0.,0.,fil_prot.ff);
        
        if(proverka(kodvn_nvr.ravno(),knah,0,1) != 0)
         {
          suma_nah_fvs+=suma;
          suma_nah_fvs_tn+=suma;
          
         }
       }

     }
    else  /*Удержания*/
     {
      /*Выплаты не входящие в 1ДФ*/
      if(provkod(nast.kodud,knah) >= 0)
         continue;

      if(sravmydat(d,m,g,1,mn,gn) >= 0 && sravmydat(d,m,g,31,mk,gk) <= 0) /*строго квартал*/
       {
        if(proverka(kodpn_all.ravno(),row1[2],0,0) == 0) /*начисленный подоходный налог */
         {
          /*нужно определить код дохода для подоходного налога - если в массиве уже есть 101 то на него если его вообще нет то на первый*/                         
          if(kod_doh_for_podoh == 0)
            kod_doh_for_podoh=zarkvrt1_kdfpn(m,g,&mas,fil_prot.ff);
          zarkvrt1_zp(m,g,kod_doh_for_podoh,&mas,0.,0.,suma*-1,0.,fil_prot.ff);
          continue;
         }

        if(kodvn == knah || kodvs_b == knah)
         {
          zarkvrt1_vs(m,g,&vsbor,0.,0.,suma*-1,0.,fil_prot.ff);
          suma_vs+=suma;
          suma_vs_tn+=suma;
         }
       }            

      if(provkod(obud,knah) < 0) /*если это не обязательное удержание то это выплата*/
       {
        short godn=atoi(row1[4]);
        short mesn=atoi(row1[5]); 

        if(sravmydat(d,m,g,dn,mn,gn)  < 0)  /*если выплаты до даты начала и в счёт предыдущего периода то пропускаем*/
         if(sravmydat(1,mesn,godn,1,mn,gn) < 0)
          {
           fprintf(fil_prot.ff,"Выплата в счёт предыдущего квартала!\n");
           continue;
          }

        if(dkk > dk) /*если день конца не равен последнему дню месяца значит захватываем следующий месяц за кварталом*/
        if(sravmydat(d,m,g,1,mk,gk) >= 0 && sravmydat(d,m,g,dk,mk,gk)  <= 0)  /*если выплаты от конца квартала и до даты конца и в счёт сдедующего квартала то пропускаем*/
         if(sravmydat(1,mesn,godn,1,mk,gk) >= 0)
          {
           fprintf(fil_prot.ff,"Выплата в счёт следующего квартала! %d.%d\n",mesn,godn);
           continue;
          }

        fprintf(fil_prot.ff,"%02d.%02d.%04d -> %02d.%04d Сумма выплаты:%.2f\n",d,m,g,mesn,godn,suma);        

        sprintf(strsql,"%02d.%04d|%02d.%04d",mesn,godn,m,g);
        if((nomer_v_sp=mesn_godn.find(strsql)) < 0)
         mesn_godn.plus(strsql);

        suma_viplat.plus(suma,nomer_v_sp);
        
       }
     }      
      
       
   
   }
  /************************************
    в настоящий момент мы имеем начисленный доход по кодам доходов и начисленный подоходный 
    собранный на одном коде дохода. Распределяем начисленный подоходный по кодам доходов
    также имеем массив сумм выплат в счёт каких месяцев эти выплаты
    ************************************/
  zarkvrt_pr(&mas,&nast,NULL);

  /*читаем начисления на зарплату за квартал*/
  double suma_esv_nah_kvrt=zarkvrt1_esv(tabnom,mn,gn,dk,mk,gk,nast.gosstrah.ravno(),0);
  double suma_esv_vip=0.;

  /*Для определения переплаты создаем три массива*/  
  class SPISOK mes_god_nah; /*даты в счёт которых были выплаты*/
  class masiv_din_double suma_kvip_nah; /*начисленная в этой дате к выплате сумма*/
  class masiv_din_double suma_vip; /*cумма всех выплат в счёт даты*/
      
  /*определяем выплаченный доход и подоходный налог*/
  for(int nom=0; nom < mesn_godn.kolih(); nom++)
   {
    double suma_v=suma_viplat.ravno(nom);
    if(suma_v != 0.)
     suma_v*=-1;    
    rsdat1(&m,&g,mesn_godn.ravno(nom)); /*Дата в счёт которой выплачено*/
     
    short mes_vip=0;
    short god_vip=0;        
    polen(mesn_godn.ravno(nom),strsql,sizeof(strsql),2,'|');

    rsdat1(&mes_vip,&god_vip,strsql); /*дата когда выплачено*/

    double koef_v=0.;
    double suma_k_viplate_nah=0.;
    zarkvrt1_mes(tabnom,m,g,suma_v,mes_vip,god_vip,&mas,&nast,&koef_v,&suma_k_viplate_nah,&vsbor,fil_prot.ff);

    if(nast.gosstrah.getdlinna() > 1) /*Если коды фондов на зарплату есть в настройке*/
     {
      double suma_esv_nah_mes=zarkvrt1_esv(tabnom,m,g,0,0,0,nast.gosstrah.ravno(),1); /*сумма начислений на заралату в месяце в счёт которого выплачено*/
      suma_esv_vip+=suma_esv_nah_mes*koef_v;    
     }

    sprintf(strsql,"%02d.%04d",m,g);
    int nomer_mes=0;
    if((nomer_mes=mes_god_nah.find(strsql)) < 0)
     {
      mes_god_nah.plus(strsql);
      suma_kvip_nah.plus(suma_k_viplate_nah,nomer_mes); //начисления записываем только один раз так как они повторяются если в счёт месяца было сделано несколько выплат
     }
   suma_vip.plus(suma_v,nomer_mes);              

   }
  suma_vs_vip_tn+=vsbor.vs_vip.suma();
  suma_nah_fvs_vip_tn+=vsbor.dohod_vip.suma();  

  suma_vs_vip+=vsbor.vs_vip.suma();
  suma_nah_fvs_vip+=vsbor.dohod_vip.suma();  

  fprintf(fil_prot.ff,"\
-------------------------------------------------------------\n");
  fprintf(fil_prot.ff,"\
 Дата  |Начислено |Выплачено | Разница  |Коэффициент выплаты|\n");
  fprintf(fil_prot.ff,"\
-------------------------------------------------------------\n");
  /*проверяем есть ли переплаты*/
  for(int nomer=0; nomer < mes_god_nah.kolih(); nomer++)
   {

     fprintf(fil_prot.ff,"%s %10.2f %10.2f %10.2f %f\n",
     mes_god_nah.ravno(nomer),suma_kvip_nah.ravno(nomer),suma_vip.ravno(nomer),
     suma_kvip_nah.ravno(nomer)-suma_vip.ravno(nomer),suma_kvip_nah.ravno(nomer)/suma_vip.ravno(nomer));

     if(suma_kvip_nah.ravno(nomer) - suma_vip.ravno(nomer) < -0.01)
      {
       sprintf(strsql,"%d %s\n%s %s %s\n%.2f > %.2f", 
       tabnom,fio.ravno(),
       gettext("Сумма выплат в счёт"),mes_god_nah.ravno(nomer),gettext("больше суммы к выплате начисленной в этом месяце"),
       suma_vip.ravno(nomer),suma_kvip_nah.ravno(nomer));
       iceb_t_soob(strsql);       
      }
   }
  if(suma_esv_nah_kvrt != 0. || suma_esv_vip != 0.)
   {
    zarkvrt1_zp(mn,gn,133,&mas,suma_esv_nah_kvrt,suma_esv_vip,0.,0.,fil_prot.ff);
   }

  if(mas.mesgod_koddoh.kolih() == 0)
   {
    fprintf(fil_prot.ff,"Не найдено ни начислений ни удержаний!\n");
    continue;
   }

  if(metka_dog_podr == 0)
   kolih_rab++;
  else
   kolih_dog_pod++;
  
  class SPISOK mes_god_kod_lgoti; /*месяц год*/
  class SPISOK sp_kod_lgoti; /*Список кодов льгот по датам*/
  class iceb_tu_str kod_lgoti("");

   /*Записываем в промежуточную таблицу*/
  for(int nom=0; nom < mas.mesgod_koddoh.kolih(); nom++)
   {
    kod_lgoti.new_plus("");
    polen(mas.mesgod_koddoh.ravno(nom),&kod_doh,2,'|');

    if(kod_doh == kod_doh_tek)
     {
      rsdat1(&m,&g,mas.mesgod_koddoh.ravno(nom));
      
      sprintf(strsql,"%02d.%04d",m,g);
      if((nomer_v_sp=mes_god_kod_lgoti.find(strsql)) < 0)
       {
        mes_god_kod_lgoti.plus(strsql);
        sprintf(strsql,"select lgot from Zarn where tabn=%d and god=%d and mes=%d",tabnom,g,m);

        if(readkey(strsql,&row1,&cur1) == 1)
         {
          if(atoi(row1[0]) != 0) /*вместо кода льготы может быть мусор*/
           {
            sprintf(strsql,"%02d",atoi(row1[0]));
            kod_lgoti.new_plus(strsql);
           }
         }        
        sp_kod_lgoti.plus(kod_lgoti.ravno());
        
       }
      else
       {
        kod_lgoti.new_plus(sp_kod_lgoti.ravno(nomer_v_sp));
       }
     }
    
    /*Запись во временную таблицу подоходного налога*/   
    if(kod_doh != 133)
     {
      zarkvrt_zapvtabl(imatmptab,inn.ravno(),kod_doh,kod_lgoti.ravno(),
      mas.dohod_nah.ravno(nom),
      mas.dohod_vip.ravno(nom),
      mas.podoh_nah.ravno(nom),
      mas.podoh_vip.ravno(nom),
      datanr.ravno(),datakr.ravno(),fio.ravno_filtr(),row[0],fil_prot.ff);

      datanr.new_plus("0000-00-00");
      datakr.new_plus("0000-00-00");
     }
    else
     {
      zarkvrt_zapvtabl(imatmptab,inn.ravno(),kod_doh,kod_lgoti.ravno(),
      mas.dohod_nah.ravno(nom),
      mas.dohod_vip.ravno(nom),
      mas.podoh_nah.ravno(nom),
      mas.podoh_vip.ravno(nom),
      datanr133.ravno(),datakr133.ravno(),fio.ravno_filtr(),row[0],fil_prot.ff);

      datanr133.new_plus("0000-00-00");
      datakr133.new_plus("0000-00-00");
     }
   }      

  /*Запись во временную таблицу военного сбора*/
  zarkvrt_zapvtabl(imatmptab,inn.ravno(),0,"VS",
  suma_nah_fvs_tn,
  suma_nah_fvs_vip_tn,
  suma_vs_tn*-1,
  suma_vs_vip_tn,
  "0000-00-00","0000-00-00",fio.ravno_filtr(),row[0],fil_prot.ff);
/*****************     
  printw("%s-%.2f %.2f %.2f %.2f\n",__FUNCTION__,
  suma_nah_fvs_tn,
  suma_nah_fvs_vip_tn,
  suma_vs_tn,
  suma_vs_vip_tnk);
  OSTANOV();
*****************/
 }

/*Смотрим настройки поиска данных в проводках*/

kod_doh=0;
printw("%s:%d\n",gettext("Количество настроек на поиск проводок"),nast.kodi_pd_prov.kolih());

if(rp.tabnom.getdlinna() <= 1)
for(int kkk=0; kkk < nast.kodi_pd_prov.kolih(); kkk++)
 {
  kod_doh=nast.kodi_pd_prov.ravno(kkk);
  printw("%d:%s\n",kod_doh,nast.sheta.ravno(kkk));
  refresh();  
  zarkvrt_gk(1,mn,gn,31,mk,gk,nast.sheta.ravno(kkk),ICEB_ZAR_PKTN,kod_doh,imatmptab,fil_prot.ff);
  
 }
/*Проверяем подсистему Учёт командировочных расходов*/
zarkvrt1_kr(mn,gn,mk,gk,imatmptab,&nast);



/*формируем отчёт*/
sprintf(strsql,"select * from %s order by inn asc,pd asc",imatmptab);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи в промежуточной таблице!"));
  return(1);
 }

class iceb_tu_str naim_kontr("");
class iceb_tu_str kod_edrpou("");
class iceb_tu_str telefon("");
sprintf(strsql,"select naikon,kod,telef from Kontragent where kodkon='00'");
if(readkey(strsql,&row1,&cur1) == 1)
 {
  naim_kontr.new_plus(row1[0]);
  kod_edrpou.new_plus(row1[1]);
  telefon.new_plus(row1[2]);
 }
sprintf(imaf_1df,"f1df%d.lst",getpid());
class iceb_fopen fil_1df;
if(fil_1df.start(imaf_1df,"w") != 0)
 return(1);

sprintf(imaf_1df_fam,"f1dff%d.lst",getpid());
class iceb_fopen fil_1df_fam;
if(fil_1df_fam.start(imaf_1df_fam,"w") != 0)
 return(1);

sprintf(imaf_dbf_tmp,"f1df%d.tmp",getpid());
class iceb_fopen fil_dbf_tmp;
if(fil_dbf_tmp.start(imaf_dbf_tmp,"w") != 0)
 return(1);

class iceb_fopen fil_csv;
const char *imaf_csv={"f1df.csv"};

if(fil_csv.start(imaf_csv,"w") != 0) 
 return(1);

int shet_str_1df=0; /*счётчик записей в файле*/
int kolih_str_1df=0; /*Количество записей в файле*/
int nomer_xml_str=0;
//Заголовок распечаток
zagf1df(kolih_rab,kolih_dog_pod,rp.kvrt.ravno_atoi(),kod_edrpou.ravno(),nast.TYP,&shet_str_1df,0,dn,mn,gn,dk,mk,gk,0,fil_1df.ff,fil_1df_fam.ff);
spkvd(&shet_str_1df,fil_1df.ff);

//заголовок xml файла
FILE *ff_xml=NULL;
memset(imaf_xml,'\0',sizeof(imaf_xml));
f1df_xml_zag(rp.kvrt.ravno_atoi(),gn,kod_edrpou.ravno(),nast.TYP,0,kolih_rab,kolih_dog_pod,naim_kontr.ravno(),imaf_xml,&ff_xml);

fprintf(fil_csv.ff,"KR|%d\n",kolih_rab);
fprintf(fil_csv.ff,"KRDP|%d\n",kolih_dog_pod);

double nah_dohod=0.;
double vip_dohod=0.;
double nah_podoh=0.;
double vip_podoh=0.;

double itogo_nah_dohod=0.;
double itogo_vip_dohod=0.;
double itogo_nah_podoh=0.;
double itogo_vip_podoh=0.;
int kol_list=0;
int nomer_str=0;
short dp,mp,gp;
short du,mu,gu;
char datnr_lst[32];
char datnr_dbf[32];
char datkr_lst[32];
char datkr_dbf[32];
char datnr_xml[32];
char datkr_xml[32];

while(cur.read_cursor(&row) != 0)
 {


  nah_dohod=atof(row[2]);
  vip_dohod=atof(row[3]);
  nah_podoh=atof(row[4]);
  vip_podoh=atof(row[5]);  

  if(SRAV(row[8],"VS",0) == 0) /*запись военного сбора*/
   {
    fprintf(fil_csv.ff,"VS|%s|%.2f|%.2f|%.2f|%.2f|\n",
    row[0],
    nah_dohod,
    vip_dohod,
    nah_podoh,
    vip_podoh);
    continue;
   }

  itogo_nah_dohod+=nah_dohod;
  itogo_vip_dohod+=vip_dohod;
  itogo_nah_podoh+=nah_podoh;
  itogo_vip_podoh+=vip_podoh;

  memset(datnr_lst,'\0',sizeof(datnr_lst));  
  memset(datkr_lst,'\0',sizeof(datkr_lst));  
  memset(datnr_dbf,'\0',sizeof(datnr_dbf));  
  memset(datkr_dbf,'\0',sizeof(datkr_dbf));  
  memset(datnr_xml,'\0',sizeof(datnr_xml));  
  memset(datkr_xml,'\0',sizeof(datkr_xml));  

  rsdat(&dp,&mp,&gp,row[6],2);
  rsdat(&du,&mu,&gu,row[7],2);
  if(dp != 0)
   {
    sprintf(datnr_lst,"%02d.%02d.%04d",dp,mp,gp);
    sprintf(datnr_dbf,"%04d%02d%02d",gp,mp,dp);
    sprintf(datnr_xml,"%02d%02d%04d",dp,mp,gp);
   }
  if(du != 0)
   {
    sprintf(datkr_lst,"%02d.%02d.%04d",du,mu,gu);
    sprintf(datkr_dbf,"%04d%02d%02d",gu,mu,du);
    sprintf(datkr_xml,"%02d%02d%04d",du,mu,gu);
   }
 
  kolih_str_1df++;
  
  fprintf(fil_1df.ff,"%-5d|%-10s|%10.2f|%10.2f|%10.2f|%10.2f|  %-4s|%-10s|%-10s|%-10s|%6s|\n",
  kolih_str_1df,
  row[0],
  nah_dohod,
  vip_dohod,
  nah_podoh,
  vip_podoh,  
  row[1],datnr_lst,datkr_lst,row[8]," ");

  fprintf(fil_1df_fam.ff,"%-5d|%-10s|%10.2f|%10.2f|%10.2f|%10.2f|  %-4s|%-10s|%-10s|%-10s|%6s|%s %s\n",
  kolih_str_1df,
  row[0],
  nah_dohod,
  vip_dohod,
  nah_podoh,
  vip_podoh,  
  row[1],datnr_lst,datkr_lst,row[8]," ",row[10],row[9]);
  
  zapf8dr(kolih_str_1df,rp.kvrt.ravno_atoi(),gn,kod_edrpou.ravno(),nast.TYP,row[0],nah_dohod,vip_dohod,nah_podoh,vip_podoh,row[1],datnr_dbf,datkr_dbf,atoi(row[8]),0,fil_dbf_tmp.ff);

  f1df_xml_str(&nomer_xml_str,row[0],nah_dohod,vip_dohod,nah_podoh,vip_podoh,row[1],datnr_xml,datkr_xml,row[8],"0",ff_xml);

  fprintf(fil_csv.ff,"PN|%s|%.2f|%.2f|%.2f|%.2f|%s|%s|%s|%s|%s|\n",
  row[0],
  nah_dohod,
  vip_dohod,
  nah_podoh,
  vip_podoh,  
  row[1],datnr_lst,datkr_lst,row[8]," ");

 }

suma_vs*=-1;

printw("\
%*s:%10.2f\n\
%*s:%10.2f\n\
%*s:%10.2f\n\
%*s:%10.2f\n\
%*s:%10.2f\n\
%*s:%10.2f\n\
%*s:%10.2f\n\
%*s:%10.2f\n",
iceb_tu_kolbait(40,gettext("Начисленный доход")),gettext("Начисленный доход"),itogo_nah_dohod,
iceb_tu_kolbait(40,gettext("Выплаченный доход")),gettext("Выплаченный доход"),itogo_vip_dohod,
iceb_tu_kolbait(40,gettext("Начисленный подоходный")),gettext("Начисленный подоходный"),itogo_nah_podoh,
iceb_tu_kolbait(40,gettext("Выплаченный подоходный")),gettext("Выплаченный подоходный"),itogo_vip_podoh,
iceb_tu_kolbait(40,gettext("Начисленный доход для военного сбора")),gettext("Начисленный доход для военного сбора"),suma_nah_fvs,
iceb_tu_kolbait(40,gettext("Начисленный военный сбор")),gettext("Начисленный военный сбор"),suma_vs,
iceb_tu_kolbait(40,gettext("Выплаченный доход для военного сбора")),gettext("Выплаченный доход для военного сбора"),suma_nah_fvs_vip,
iceb_tu_kolbait(40,gettext("Выплаченный военный сбор")),gettext("Выплаченный военный сбор"),suma_vs_vip);




int kol_fiz_lic=kolih_rab+kolih_dog_pod;

//Распечатка концовки файлов
koncf8dr(rp.kvrt.ravno_atoi(),gn,kod_edrpou.ravno(),nast.TYP,itogo_vip_dohod,itogo_nah_dohod,itogo_vip_podoh,itogo_nah_podoh,
itogo_vip_dohod,itogo_nah_dohod,itogo_vip_podoh,itogo_nah_podoh,kol_fiz_lic,kol_list,&nomer_str,2,suma_nah_fvs,suma_nah_fvs_vip,suma_vs,suma_vs_vip,fil_1df.ff,fil_1df_fam.ff,fil_dbf_tmp.ff);


f1df_xml_kon(itogo_nah_dohod,itogo_vip_dohod,itogo_nah_podoh,itogo_vip_podoh,nomer_xml_str,kol_fiz_lic,kol_list,naim_kontr.ravno(),telefon.ravno(),nast.TYP,suma_nah_fvs,suma_nah_fvs_vip,suma_vs,suma_vs_vip,ff_xml);
 
fputc(26,fil_dbf_tmp.ff);
fil_dbf_tmp.end();



fil_prot.end();
fil_1df.end();
fil_1df_fam.end();
fil_csv.end();

int nomer_rasp=1;
sprintf(imaf_dbf,"da0000%02d.%d",nomer_rasp,rp.kvrt.ravno_atoi());
if(iceb_t_poldan("Имя DBF файла",strsql,"zarsdf1df.alx") == 0)
  sprintf(imaf_dbf,"%s%02d.%d",strsql,nomer_rasp,rp.kvrt.ravno_atoi());

creatheaddbf(imaf_dbf,kolih_str_1df);

/*Сливаем два файла*/
iceb_t_cat(imaf_dbf,imaf_dbf_tmp);

unlink(imaf_dbf_tmp);



oth->spis_imaf.plus(imaf_1df);
oth->spis_naim.plus(gettext("Расчёт формы 1ДФ"));

oth->spis_imaf.plus(imaf_1df_fam);

sprintf(strsql,"%s+%s",gettext("Расчёт формы 1ДФ"),gettext("Фамилия"));

oth->spis_naim.plus(strsql);

oth->spis_imaf.plus(imaf_prot);
oth->spis_naim.plus(gettext("Протокол расчёта"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),1);
 
oth->spis_imaf.plus(imaf_dbf);
oth->spis_naim.plus(gettext("Документ в формате dbf"));

oth->spis_imaf.plus(imaf_xml);
oth->spis_naim.plus(gettext("Документ в формате xml"));

oth->spis_imaf.plus(imaf_csv);
oth->spis_naim.plus(gettext("Документ в формате csv"));




printw_vr(time_n);
OSTANOV();

return(0);

}
/****************************/
/*запись в массивы данных о военном сборе*/
/******************************/

void zarkvrt1_vs(short m,short g,
class zarkvrt_vs *vsbor,
double dohod_nah,
double dohod_vip,
double vs_nah,
double vs_vip,
FILE *ff_prot)
{
int nomer_v_sp=0;
char strsql[512];
sprintf(strsql,"%02d.%04d",m,g);

if((nomer_v_sp=vsbor->mesgod.find(strsql)) < 0)
 vsbor->mesgod.plus(strsql);

vsbor->dohod_nah.plus(dohod_nah,nomer_v_sp);
vsbor->dohod_vip.plus(dohod_vip,nomer_v_sp);
vsbor->vs_nah.plus(vs_nah,nomer_v_sp);
vsbor->vs_vip.plus(vs_vip,nomer_v_sp);

}

/************************************/
/*запись в массивы данных о подоходном налоге*/
/***********************************/
void zarkvrt1_zp(short m,short g,
int kod_doh,
class zarkvrt_mas *mas,
double dohod_nah,
double dohod_vip,
double podoh_nah,
double podoh_vip,
FILE *ff_prot)
{
int nomer_v_sp=0;
char strsql[512];
sprintf(strsql,"%02d.%04d|%d",m,g,kod_doh);

if((nomer_v_sp=mas->mesgod_koddoh.find(strsql)) < 0)
 mas->mesgod_koddoh.plus(strsql);
/*****************
fprintf(ff_prot,"%s-дата:%d.%d|%d\n",__FUNCTION__,m,g,kod_doh);
if(nomer_v_sp < 0)
 fprintf(ff_prot,"\
Сумма до записи    %10.2f %10.2f %8.2f %8.2f %d\n",0.,0.,0.,0.,nomer_v_sp);
else
 fprintf(ff_prot,"\
 Сумма до записи    %10.2f %10.2f %8.2f %8.2f %d\n",
 mas->dohod_nah.ravno(nomer_v_sp),
 mas->dohod_vip.ravno(nomer_v_sp),
 mas->podoh_nah.ravno(nomer_v_sp),
 mas->podoh_vip.ravno(nomer_v_sp),
 nomer_v_sp);
*********************/

mas->dohod_nah.plus(dohod_nah,nomer_v_sp);
mas->dohod_vip.plus(dohod_vip,nomer_v_sp);
mas->podoh_nah.plus(podoh_nah,nomer_v_sp);
mas->podoh_vip.plus(podoh_vip,nomer_v_sp);

 
/************** 
if(nomer_v_sp < 0)
 nomer_v_sp=mas->mesgod_koddoh.kolih()-1;
fprintf(ff_prot,"\
Записываем         %10.2f %10.2f %8.2f %8.2f\n\
Сумма после записи %10.2f %10.2f %8.2f %8.2f %d\n",
dohod_nah,
dohod_vip,
podoh_nah,
podoh_vip,
mas->dohod_nah.ravno(nomer_v_sp),
mas->dohod_vip.ravno(nomer_v_sp),
mas->podoh_nah.ravno(nomer_v_sp),
mas->podoh_vip.ravno(nomer_v_sp),
nomer_v_sp);
***************/
}
/**********************************************************************/
/*определяем данные за месяц в счёт которого была сделана выплата*/
/********************************************************************/
void zarkvrt1_mes(int tabn,
short m,short g, /*месяц в счёт которого выплачено*/
double suma_viplati,
short mv,short gv, /*месяц в котором сделана выплата*/
class zarkvrt_mas *mas,
class sprkvrt1_nast *nast,
double *koef_viplat, /*Коэффициент выплат начислеий*/
double *suma_k_viplate_nah, /*Сумма к выплате начисленная в заданном месяце*/
class zarkvrt_vs *vsbor,
FILE *ff_prot)
{
class zarkvrt_mes_vipl mes_data;
char strsql[512];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
double suma=0.;
double suma_vs_nah=0.; /*начисленная сумма военного сбора*/

*suma_k_viplate_nah=0.;
*koef_viplat=0.;

sprintf(strsql,"select prn,knah,suma from Zarp where tabn=%d and datz >= '%04d-%02d-01' and \
datz <= '%04d-%02d-31' and suma <> 0.",tabn,g,m,g,m);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 return;
int nomer=0; 
/***************
if(obud != NULL)
 {
  fprintf(ff_prot,"\n%s-obud=",__FUNCTION__);
  for(int nom=1 ; nom <= obud[0];nom++)
   fprintf(ff_prot," %d",obud[nom]);
  fprintf(ff_prot,"\n");
 }
*****************/
class iceb_tu_str koddoh("");
int knah=0;
while(cur.read_cursor(&row) != 0)
 {
  suma=atof(row[2]);
  knah=atoi(row[1]);

  if(atoi(row[0]) == 1)
   {
    mes_data.suma_nah+=suma; /*берём все начиления для правильного определения суммы к выплате*/

    //коды которые не входят в 1ДФ
    if(provkod(nast->kodnvf8dr,knah) >= 0)
        continue; 

    if(proverka(kodvn_nvr.ravno(),knah,0,1) != 0) /*определяем сумму с которой берётся военный сбор*/
     mes_data.suma_nah_vs+=suma;
     
    koddoh.new_plus(kod_doh_tek);
    for(nomer=0; nomer < nast->kodi_nah.kolih(); nomer++)
     {
      if(proverka(nast->kodi_nah.ravno(nomer),row[1],0,1) == 0)
       {
        koddoh.new_plus(nast->kodi_pd.ravno(nomer));
        break;
       }
     }
    if((nomer=mes_data.koddoh.find(koddoh.ravno_atoi())) < 0)
      mes_data.koddoh.plus(koddoh.ravno_atoi());     
    mes_data.suma_nah_kod.plus(suma,nomer);   
   }
  else
   {
    /*Выплаты не входящие в 1ДФ*/
    if(provkod(nast->kodud,knah) >= 0)
       continue;
    if(provkod(obud,knah) >= 0)
     {
      mes_data.suma_obud+=suma;
//      fprintf(ff_prot,"%s-Обязательное удержание-%d %.2f\n",__FUNCTION__,knah,suma);
     }
    if(proverka(kodpn_all.ravno(),knah,0,0) == 0)
      mes_data.suma_podoh_nah+=suma;

    if(kodvn == knah || kodvs_b == knah)
     suma_vs_nah+=suma;
   }
 }
suma_vs_nah*=-1;
mes_data.suma_podoh_nah*=-1;
int nomer_vsp=0;

/*определяем сумму по кодам дохода с которых берётся подоходный налог*/
/*расчитывем эту сумму для того чтобы подоходный гарантированно правильно разпределился*/
double suma_kod_s_podoh=0;
for(int nomer=0; nomer < mes_data.koddoh.kolih(); nomer++)
 {
  if((nomer_vsp=nast->kodi_pd.find(mes_data.koddoh.ravno(nomer))) >= 0)
   if(nast->metka_podoh.ravno(nomer_vsp) == 1) /*налог не берётся*/
      continue;
  suma_kod_s_podoh+=mes_data.suma_nah_kod.ravno(nomer);
 }

double suma_kvid=mes_data.suma_nah+mes_data.suma_obud;
*suma_k_viplate_nah=suma_kvid;

double koef_v=0.;
if(suma_viplati != 0. && suma_kvid != 0.)
 koef_v=suma_viplati/suma_kvid;

*koef_viplat=koef_v;

fprintf(ff_prot,"\n\
%s-%d.%d -> %d.%d\n\
Сумма выплаты               :%.2f\n\
Сумма всех начислений       :%.2f\n\
Сумма обязательных удержаний:%.2f\n\
Cумма к выдаче              :%.2f\n\
Коэффициент выплаты %.2f/%.2f=%f\n\
Сумма военного сбора        :%.2f\n\
Сумма подоходного           :%.2f\n",
__FUNCTION__,
mv,gv,m,g,
suma_viplati,
mes_data.suma_nah,
mes_data.suma_obud,
suma_kvid,
suma_viplati,suma_kvid,koef_v,
suma_vs_nah,
mes_data.suma_podoh_nah);

/*подоходный налог*/
double suma_vip=0.;
for(int nomer=0; nomer < mes_data.koddoh.kolih(); nomer++)
 {

  /*расчитываем выплаченый доход по кодам доходов*/
  suma_vip=mes_data.suma_nah_kod.ravno(nomer)*koef_v;
  fprintf(ff_prot,"%s-Cумма выплаты по коду %d -> %.2f*%f=%.2f\n",__FUNCTION__,mes_data.koddoh.ravno(nomer),mes_data.suma_nah_kod.ravno(nomer),koef_v,suma_vip);


  fprintf(ff_prot,"%s-Выплаченый доход по коду %d -> %.2f*%f=%.2f\n",__FUNCTION__,mes_data.koddoh.ravno(nomer),mes_data.suma_nah_kod.ravno(nomer),koef_v,suma_vip);
   
  zarkvrt1_zp(m,g,mes_data.koddoh.ravno(nomer),mas,0.,suma_vip,0.,0.,ff_prot);

  if((nomer_vsp=nast->kodi_pd.find(mes_data.koddoh.ravno(nomer))) >= 0)
   if(nast->metka_podoh.ravno(nomer_vsp) == 1) /*налог не берётся*/
      continue;

  double koef_vip=0.;
  if(suma_kod_s_podoh != 0.)
    koef_vip=suma_vip/suma_kod_s_podoh;

  fprintf(ff_prot,"%s-Коэффициент выплаты по коду %d -> %.2f/%.2f=%f\n",__FUNCTION__,mes_data.koddoh.ravno(nomer),suma_vip,suma_kod_s_podoh,koef_vip);

  
  double podoh_vip=mes_data.suma_podoh_nah*koef_vip;

  fprintf(ff_prot,"%s-Сумма выплаты подоходного по коду %d -> %.2f*%.2f=%f\n",__FUNCTION__,mes_data.koddoh.ravno(nomer),mes_data.suma_podoh_nah,koef_vip,podoh_vip);  

  zarkvrt1_zp(m,g,mes_data.koddoh.ravno(nomer),mas,0.,0.,0.,podoh_vip,ff_prot);



 }

fprintf(ff_prot,"\n%s-Cумма с которой вычисляется военный сбор %.2f\n",__FUNCTION__,mes_data.suma_nah_vs);

/*военный сбор*/
suma_vip=mes_data.suma_nah_vs*koef_v;
zarkvrt1_vs(m,g,vsbor,0.,suma_vip,0.,0.,ff_prot);

fprintf(ff_prot,"%s-Cумма выплаченного дохода для военного сбора %.2f*%f=%.2f\n",__FUNCTION__,mes_data.suma_nah_vs,koef_v,suma_vip);

double suma_vs_vip=suma_vs_nah*koef_v;
zarkvrt1_vs(m,g,vsbor,0.,0.,0.,suma_vs_vip,ff_prot);

fprintf(ff_prot,"%s-Cумма выплаченного военного сбора %.2f*%f=%.2f\n",__FUNCTION__,suma_vs_nah,koef_v,suma_vs_vip);


}
/*********************************/
/*чтение начислений на зарплату*/
/*******************************/
double zarkvrt1_esv(int tabn,
short mn,short gn, /*месяц начала*/
short dk,short mk,short gk, /*Дата конца для получения отчёта за квартал*/
const char *gosstrah,
int metka) //0-за квартал 1-за месяц
{
SQL_str row;
SQLCURSOR cur;
int kolstr;
char strsql[1024];
if(gosstrah[0] == '\0')
 return(0.);

if(metka == 0)
 {
  short denk=0,mesk=0,godk=0;
  short dkm=0; /*последний день в месяце конца*/

  dpm(&dkm,&mk,&gk,5); /*получаем последний день в месяце конца*/
  mesk=mk;
  godk=gk;
  if(dkm > dk) /*расчёт с захватом выплат в следующем квартале*/
   {
    dpm(&denk,&mesk,&godk,4); /*уменьшаем на месяц*/
    dpm(&denk,&mesk,&godk,5); /*получаем последний день месяца*/
   } 
  else
   {
    denk=dkm;
   }
  sprintf(strsql,"select sumap from Zarsocz where datz >= '%04d-%02d-01' \
and datz <=  '%04d-%02d-%02d' and tabn=%d ",gn,mn,godk,mesk,denk,tabn);
 }

if(metka == 1)
  sprintf(strsql,"select sumap from Zarsocz where datz >= '%04d-%02d-01' \
and datz <=  '%04d-%02d-31' and tabn=%d ",gn,mn,gn,mn,tabn);
 
char bros[512];

int kol_pol=pole2(gosstrah,',');
if(kol_pol == 0)
 {
  sprintf(bros," and kodz=%s",gosstrah);
  strcat(strsql,bros);
 }
else
 {
  int kod=0;
  for(int i=0; i < kol_pol; i++)
   {
    polen(gosstrah,&kod,i+1,',');
    if(i == 0)
      sprintf(bros," and (kodz=%d",kod);
    else
      sprintf(bros," or kodz=%d",kod);
    strcat(strsql,bros);
   }
  strcat(strsql,")");
 }


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(0.);
 }
double suma=0.;

if(kolstr != 0)
 while(cur.read_cursor(&row) != 0)
  suma+=atof(row[0]);  
 
return(suma);
}
/****************************************/
/*определение кода дохода для подоходного налога*/
/**************************************************/

int zarkvrt1_kdfpn(short mr,short gr,class zarkvrt_mas *mas,FILE *ff_prot)
{
int kod_doh=0;
char bros[512];
short m,g;
int kolih=mas->mesgod_koddoh.kolih();

if(kolih == 0) /*нет начислений*/
 return(kod_doh_tek);

/*проверяем есть ли начисление по 101 коду дохода - если есть возвращаем 101*/
for(int nom=0; nom < kolih; nom++)
 {
  polen(mas->mesgod_koddoh.ravno(nom),bros,sizeof(bros),1,'|');
  rsdat1(&m,&g,bros);
  if(sravmydat(1,mr,gr,1,m,g) == 0)
   {
    polen(mas->mesgod_koddoh.ravno(nom),&kod_doh,2,'|');
    if(kod_doh == kod_doh_tek)
      return(kod_doh);    
   }  
 }

/*берём первый в списке*/
for(int nom=0; nom < kolih; nom++)
 {
  polen(mas->mesgod_koddoh.ravno(nom),bros,sizeof(bros),1,'|');
  rsdat1(&m,&g,bros);
  if(sravmydat(1,mr,gr,1,m,g) == 0)
   {
    polen(mas->mesgod_koddoh.ravno(nom),&kod_doh,2,'|');
    return(kod_doh);    
   }  
 }
/*если по месяцу нет записей возвращаем 101*/
return(kod_doh_tek);
}

/***********************************************/
/*Пропорциональное распределение начисленного подоходного налога по кодам доходов*/
/*****************************************************/
void zarkvrt_pr(class zarkvrt_mas *mas,class sprkvrt1_nast *nast,FILE *ff_prot)
{
class SPISOK mes_god;/*Список дат по которым начислен подоходный налог*/
class masiv_din_double suma_podoh; /*Суммы подоходного налога по датам*/
class iceb_tu_str data("");
int nomer_v_sp=0;
class iceb_tu_str koddoh("");
if(ff_prot != NULL)
  fprintf(ff_prot,"%s-Пропорциональное распределение подоходного налога по кодам доходов\n",__FUNCTION__);
/*составляем список дат которые имеют начисленный подоходный и их суммы*/
for(int nomer=0; nomer < mas->mesgod_koddoh.kolih();nomer++)
 {
  if(mas->podoh_nah.ravno(nomer) == 0.)
   continue;
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s-%.2f\n",__FUNCTION__,mas->podoh_nah.ravno(nomer));
  
  polen(mas->mesgod_koddoh.ravno(nomer),&data,1,'|');

  if((nomer_v_sp=mes_god.find(data.ravno())) < 0)
   mes_god.plus(data.ravno());

  suma_podoh.plus(mas->podoh_nah.ravno(nomer),nomer_v_sp);
  mas->podoh_nah.new_plus(0.,nomer); //обнуляем значение
 }



class iceb_tu_str data1("");
/*распределяем подоходный*/
double suma_nah=0.;
for(int nomer1=0; nomer1 < mes_god.kolih(); nomer1++)
 {
  suma_nah=0.;
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s-%s %.2f\n",__FUNCTION__,mes_god.ravno(nomer1),suma_podoh.ravno(nomer1));
  
  data.new_plus(mes_god.ravno(nomer1));
  /*Узнаём общюю сумму начислений с которой начислялся подоходный налог*/
  for(int nomer2=0; nomer2 < mas->mesgod_koddoh.kolih();nomer2++)
   {
    
    polen(mas->mesgod_koddoh.ravno(nomer2),&data1,1,'|');
    polen(mas->mesgod_koddoh.ravno(nomer2),&koddoh,2,'|');
    if(ff_prot != NULL)
     fprintf(ff_prot,"%s-%s %s %s\n",__FUNCTION__,data.ravno(),data1.ravno(),koddoh.ravno());
    if(SRAV(data.ravno(),data1.ravno(),0) != 0)
     continue;

//    if(SRAV(koddoh.ravno(),"101",0) != 0) /*проверяем берётся ли для этого кода дохода подоходный налог*/
    if(koddoh.ravno_atoi() != kod_doh_tek) /*проверяем берётся ли для этого кода дохода подоходный налог*/
     {
      int nomer_kod=0;
      if((nomer_kod=nast->kodi_pd.find(koddoh.ravno_atoi())) >= 0)
       {
        if(nast->metka_podoh.ravno(nomer_kod) == 1)/*не берётся*/
         {
          if(ff_prot != NULL)
           fprintf(ff_prot,"%s-С кода дохода %d подоходный не берётся\n",__FUNCTION__,koddoh.ravno_atoi());
          continue;

         }
       }     
     }     
    suma_nah+=mas->dohod_nah.ravno(nomer2);
   }
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s-Общая сумма начислений с которых брался подоходный=%.2f\n",__FUNCTION__,suma_nah);
  if(suma_nah == 0.)
   continue;
  /*Распределяем сумму подоходного налога по кодам доходов*/
  for(int nomer2=0; nomer2 < mas->mesgod_koddoh.kolih();nomer2++)
   {
    polen(mas->mesgod_koddoh.ravno(nomer2),&data1,1,'|');
    polen(mas->mesgod_koddoh.ravno(nomer2),&koddoh,2,'|');

    if(SRAV(data.ravno(),data1.ravno(),0) != 0)
     continue;
//    if(SRAV(koddoh.ravno(),"101",0) != 0) /*проверяем берётся ли для этого кода дохода подоходный налог*/
    if(koddoh.ravno_atoi(),kod_doh_tek) /*проверяем берётся ли для этого кода дохода подоходный налог*/
     {
      int nomer_kod=0;
      if((nomer_kod=nast->kodi_pd.find(koddoh.ravno_atoi())) >= 0)
       {
        if(nast->metka_podoh.ravno(nomer_kod) == 1)/*не берётся*/
         continue;
       }     
     }     

    double kof=0.;

    if(suma_nah != 0.)
     kof=mas->dohod_nah.ravno(nomer2)/suma_nah;
    if(ff_prot != NULL)
     fprintf(ff_prot,"%s-Коэффициент для кода дохода %d -> %.2f/%.2f=%f\n",__FUNCTION__,koddoh.ravno_atoi(),mas->dohod_nah.ravno(nomer2),suma_nah,kof);
    double suma_r_podoh=suma_podoh.ravno(nomer1)*kof;

    if(ff_prot != NULL)
     fprintf(ff_prot,"%s-Cумма подоходного для кода дохода %d -> %.2f*%f=%.2f\n",__FUNCTION__,koddoh.ravno_atoi(),suma_podoh.ravno(nomer1),kof,suma_r_podoh);
    mas->podoh_nah.new_plus(suma_r_podoh,nomer2);    
   }
  
 }
}
