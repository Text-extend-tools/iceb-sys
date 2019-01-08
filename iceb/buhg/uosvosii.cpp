/* $Id: uosvosii.c,v 5.85 2013/09/26 09:43:42 sasa Exp $ */
/*23.01.2016	19.11.1999	Белых А.И.	uosvosii.c
Получение ведомости износа и остаточной стоимости и износа
*/
#include        <math.h>
#include        <errno.h>
#include        "buhg.h"
#include "poiinpd.h"

class uosvosii_poi
 {
  public:
   class iceb_tu_str datan;
   class iceb_tu_str datak;
   class iceb_tu_str podrz; /*Код подразделения*/
   class iceb_tu_str kodolz; /*Коды ответственных лиц*/
   class iceb_tu_str hnaz;
   class iceb_tu_str hnabyz;
   class iceb_tu_str hauz;
   class iceb_tu_str shetupoi;
   class iceb_tu_str innomp;
   class iceb_tu_str hztz;
   class iceb_tu_str sosob;
   
   int viduh;
   int metr;
   int metka_sort;
   int metkaost;
  uosvosii_poi()
   {
    viduh=0;
    metr=0;
    metka_sort=0;
    metkaost=0;
    clear();
   }
  void clear()
   {
    datan.new_plus("");
    datak.new_plus("");
    podrz.new_plus("");
    kodolz.new_plus("");
    hnaz.new_plus("");
    hnabyz.new_plus("");
    hauz.new_plus("");
    shetupoi.new_plus("");
    innomp.new_plus("");
    hztz.new_plus("");
    sosob.new_plus("");
   }
 };
 
extern int      kol_strok_na_liste_l; //Количество строк на стандартном листе в ландшафтной ориентации
#define         KOLSTSH  6  /*Количество строк в шапке*/

void gsapphna(short,short,short,short,short,short,short *,short *,int,const char*,short,FILE*,FILE*);
void haphna(short,short,short,short,short,short,short,short*,int,const char*,short,FILE*,FILE*,FILE*,FILE*);

void uosvosii_start(short dn,short mn,short gn,short dk,short mk,short gk,short *kst,class uosvosii_poi *poi,FILE *ff);

int uosvosii_poi(int in,const char *naim,short dn,short mn,short gn,short dk,short mk,short gk,class uosvosii_poi *poi,const char *imatmptab);
void itogi(const char *sh,double *str_suma1,double *str_suma2,double *str_suma3,short mt,short *kst,FILE *ff,FILE *ffhpz,FILE *ff_f,FILE *ff_fi,FILE *ff_fis);

extern short	startgoduos; /*Стартовый год*/

int uosvosii(int viduh, //0-налоговый учет 1-бухгалтерский
class spis_oth *oth)
{
static class uosvosii_poi poi;
class iceb_tu_str naimos("");
time_t          tmm;
class iceb_tu_str bros("");
int           kom,kom1;
short           dn,mn,gn;
short           dk,mk,gk;
short		dp,mp,gp;
char            imaf[64],imafhpz[64];;
int		kolstr,kolstr1;
SQL_str         row,row1;
char		strsql[1024];
int		pod=0,podz=0;
int		in;
double		osts;
double		bsn,izn;
double		pr1=0.,pr2=0.,pr11=0.,pr22=0.;
class iceb_tu_str hna1("");
class iceb_tu_str naipod("");
short		sli,kst;
class iceb_tu_str naigrup("");
short		mvsh; /*0-шапку не печатать, если больше печатать*/
short		mvsi; /*0-шифр не печатать*/
//int		podrp=0;
int 		kolos=0; //Количество основных средств попавших в отчёт
SQLCURSOR curr;
class iceb_tu_str hna("");   /*Шифр нормы амортотчислений*/
class iceb_tu_str nomz("");  /*Номерной знак*/

poi.viduh=viduh;

VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_data(poi.datan.ravno(),11);
VV.VVOD_SPISOK_add_data(poi.datak.ravno(),11);
VV.VVOD_SPISOK_add_data(poi.hnaz.ravno(),128);
VV.VVOD_SPISOK_add_data(poi.hnabyz.ravno(),128);
VV.VVOD_SPISOK_add_data(poi.podrz.ravno(),128);
VV.VVOD_SPISOK_add_data(poi.kodolz.ravno(),128);
VV.VVOD_SPISOK_add_data(poi.hauz.ravno(),128);
VV.VVOD_SPISOK_add_data(poi.shetupoi.ravno(),128);
VV.VVOD_SPISOK_add_data(poi.innomp.ravno(),128);
VV.VVOD_SPISOK_add_data(poi.hztz.ravno(),128);
VV.VVOD_SPISOK_add_data(poi.sosob.ravno(),2);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала...(д.м.г)..."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца....(д.м.г)..."));//1
VV.VVOD_SPISOK_add_MD(gettext("Группа нал.учета..(,,).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Группа бух.учета..(,,).."));//3
VV.VVOD_SPISOK_add_MD(gettext("Подразделение (,,)......"));//4
VV.VVOD_SPISOK_add_MD(gettext("Мат.ответственный (,,).."));//5
VV.VVOD_SPISOK_add_MD(gettext("Шифр анал. учета..(,,).."));//6
VV.VVOD_SPISOK_add_MD(gettext("Счёт учета........(,,).."));//7
VV.VVOD_SPISOK_add_MD(gettext("Инвентарный номер.(,,).."));//8
VV.VVOD_SPISOK_add_MD(gettext("Шифр затрат.......(,,).."));//9
VV.VVOD_SPISOK_add_MD(gettext("Состояние объекта.(+/-)."));//10

if(viduh == 0)
 VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт ведомости остаточной стоимости и износа (налоговый учет)"));
if(viduh == 1)
 VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт ведомости остаточной стоимости и износа (бухгалтерский учет)"));
VV.VVOD_SPISOK_add_ZAG(gettext("Введите данные"));

naz:;

clear();

helstr(LINES-1,0,"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

poi.datan.new_plus(VV.data_ravno(0));
poi.datak.new_plus(VV.data_ravno(1));

poi.hnaz.new_plus(VV.data_ravno(2));

poi.hnabyz.new_plus(VV.data_ravno(3));

poi.podrz.new_plus(VV.data_ravno(4));
poi.kodolz.new_plus(VV.data_ravno(5));
poi.hauz.new_plus(VV.data_ravno(6));
poi.shetupoi.new_plus(VV.data_ravno(7));
poi.innomp.new_plus(VV.data_ravno(8));
poi.hztz.new_plus(VV.data_ravno(9));
poi.sosob.new_plus(VV.data_ravno(10));
class iceb_tu_str kod_v("");
class iceb_tu_str naim_v("");
switch(kom)
 {
  case FK10:
  case ESC:
   return(1);
  case FK2:
  case PLU:
   break;

  case FK3:  /*Просмотр реквизитов*/
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп налогового учёта"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп бухгалтерского учёта"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка мат.-ответственны"));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка шифров аналитического учета"));//4
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка шифров затрат"));//5
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
        vibrek("Uosgrup",&poi.hnaz,&naim_v);
        VV.VVOD_SPISOK_zapis_data(2,poi.hnaz.ravno());
        goto naz;

      case 1 :
        vibrek("Uosgrup1",&poi.hnabyz,&naim_v);
        VV.VVOD_SPISOK_zapis_data(3,poi.hnabyz.ravno());
        goto naz;

      case 2 :
        if(diruospod(1,&kod_v,&naim_v,0) == 0)
         VV.data_z_plus(4,kod_v.ravno());
        goto naz;

      case 3 :
        if(dirmatot(1,&kod_v,&naim_v,0) == 0)
         VV.data_z_plus(5,kod_v.ravno());
        goto naz;

      case 4 :
        vibrek("Uoshau",&poi.hauz,&naim_v);
        VV.VVOD_SPISOK_zapis_data(6,poi.hauz.ravno());
        goto naz;

      case 5 :
        vibrek("Uoshz",&poi.hztz,&naim_v);
        VV.VVOD_SPISOK_zapis_data(9,poi.hztz.ravno());
        goto naz;
     }
    goto naz;
    

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
 }


if(rsdatp(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(0),&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(1)) != 0)
    goto naz;

dp=dk;
mp=mk;
gp=gk;
dpm(&dp,&mp,&gp,1);

MENU.VVOD_delete();
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное и нажмите Enter"));

MENU.VVOD_SPISOK_add_MD(gettext("Расчитать по подразделениям"));
MENU.VVOD_SPISOK_add_MD(gettext("Расчитать по мат.-ответственным"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
poi.metr=0;
while(menu3w(stdscr,&MENU,&poi.metr,-1,-1,0) != 0);
if(poi.metr == -1 || poi.metr == 2)
 return(1);

MENU.VVOD_delete();
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное и нажмите Enter"));

MENU.VVOD_SPISOK_add_MD(gettext("Все основные средства"));
MENU.VVOD_SPISOK_add_MD(gettext("Основные средства с нулевой остаточной стоимостью"));
MENU.VVOD_SPISOK_add_MD(gettext("Основные средства с ненулевой остаточной стоимостью"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

poi.metkaost=0;

while(menu3w(stdscr,&MENU,&poi.metkaost,-1,-1,0) != 0);

if(poi.metkaost == -1 || poi.metkaost == 3)
 return(1);

MENU.VVOD_delete();
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное и нажмите Enter"));
MENU.VVOD_SPISOK_add_MD(gettext("Сортировка по группам"));
MENU.VVOD_SPISOK_add_MD(gettext("Сортировать по счетам учёта"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
while(menu3w(stdscr,&MENU,&poi.metka_sort,-1,-1,0) != 0);
if(poi.metka_sort == -1 || poi.metka_sort == 2)
 return(1);
 
clear();
refresh();
printw("%s %d.%d.%d%s > %d.%d.%d%s\n",
gettext("Период с"),
dn,mn,gn,
gettext("г."),
dk,mk,gk,
gettext("г."));
printw("%s\n",gettext("Сортируем"));
time(&tmm);

GDITE(); 
sprintf(strsql,"select innom,naim from Uosin");

SQLCURSOR cur;
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
char imaf_prot[64];
sprintf(imaf_prot,"uospr%d.lst",getpid());
if(fil_prot.start(imaf_prot,"w") != 0)
 return(1);


class iceb_t_tmptab tabtmp;
const char *imatmptab={"uosvosii"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

if(poi.metr == 0) //По подразделениям
  sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
shu char(24) not null,\
kmo int not null,\
innom int not null,\
naim char(112) not null,\
pod int not null,\
nomz char(56) not null,\
hna char(32) not null,\
hnaby char(32) not null,\
unique(pod,innom)) ENGINE = MYISAM",imatmptab);
else 
  sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
shu char(24) not null,\
kmo int not null,\
innom int not null,\
naim char(112) not null,\
pod int not null,\
nomz char(56) not null,\
hna char(32) not null,\
hnaby char(32) not null,\
unique(kmo,innom)) ENGINE = MYISAM",imatmptab);

if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  

kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);  
  if(proverka(poi.innomp.ravno(),row[0],0,0) != 0)
    continue;
    
  in=atoi(row[0]);

  /*Смотрим обязательно на две даты, так как основное средство может в периоде уйти или прийти*/
   uosvosii_poi(in,row[1],dn,mn,gn,dk,mk,gk,&poi,imatmptab);

 }

GDITE(); 
if(poi.metr == 0) //По подразделениям
 {
  if(poi.metka_sort == 0)
   {
    if(viduh == 0) //налоговый учёт
      sprintf(strsql,"select * from %s order by hna asc,pod asc,innom asc",imatmptab);
    if(viduh == 1) //бух. учёт
      sprintf(strsql,"select * from %s order by hnaby asc,pod asc,innom asc",imatmptab);
   }
  if(poi.metka_sort == 1)
      sprintf(strsql,"select * from %s order by shu asc,pod asc,innom asc",imatmptab);
 }
if(poi.metr == 1) //По материально-ответственным
 {
  if(poi.metka_sort == 0)
   {
    if(viduh == 0) //налоговый учёт
      sprintf(strsql,"select * from %s order by hna asc,kmo asc,innom asc",imatmptab);
    if(viduh == 1) //бух.учёт
      sprintf(strsql,"select * from %s order by hnaby asc,kmo asc,innom asc",imatmptab);
   }
  if(poi.metka_sort == 1)
      sprintf(strsql,"select * from %s order by shu asc,kmo asc,innom asc",imatmptab);

 }

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(poi.metr == 0)
 sprintf(imaf,"iznosp%d.lst",getpid());
if(poi.metr == 1)
 sprintf(imaf,"iznosm%d.lst",getpid());

class iceb_fopen fil_ff;
if(fil_ff.start(imaf,"w") != 0)
 return(1);

char imaf_f[64];

if(poi.metr == 0)
 sprintf(imaf_f,"viiosp%d.lst",getpid());
if(poi.metr == 1)
 sprintf(imaf_f,"viiosm%d.lst",getpid());
class iceb_fopen fil_f;
if(fil_f.start(imaf_f,"w") != 0)
 return(1);

char imaf_fi[64];

if(poi.metr == 0)
 sprintf(imaf_fi,"viiospi%d.lst",getpid());
if(poi.metr == 1)
 sprintf(imaf_fi,"viiosmi%d.lst",getpid());
class iceb_fopen fil_fi;
if(fil_fi.start(imaf_fi,"w") != 0)
 return(1);
 

//Отчёт без итогов по подразделению, только по шифрам затрат
char imaf_fis[64];

if(poi.metr == 0)
 sprintf(imaf_fis,"visn%d.lst",getpid());
if(poi.metr == 1)
 sprintf(imaf_fis,"visb%d.lst",getpid());
class iceb_fopen fil_fis;
if(fil_fis.start(imaf_fis,"w") != 0)
 return(1);


if(poi.metr == 0)
 sprintf(imafhpz,"iznospi%d.lst",getpid());
if(poi.metr == 1)
 sprintf(imafhpz,"iznosmi%d.lst",getpid());
class iceb_fopen fil_ffhpz;
if(fil_ffhpz.start(imafhpz,"w") != 0)
 return(1);

uosvosii_start(dn,mn,gn,dk,mk,gk,&kst,&poi,fil_ff.ff);
uosvosii_start(dn,mn,gn,dk,mk,gk,&kst,&poi,fil_ffhpz.ff);
uosvosii_start(dn,mn,gn,dk,mk,gk,&kst,&poi,fil_f.ff);
uosvosii_start(dn,mn,gn,dk,mk,gk,&kst,&poi,fil_fi.ff);
kst=0;
uosvosii_start(dn,mn,gn,dk,mk,gk,&kst,&poi,fil_fis.ff);

 
short kstt=0;
haphna(dn,mn,gn,dk,mk,gk,1,&kstt,0,"",poi.metr,fil_ffhpz.ff,NULL,fil_fi.ff,fil_fis.ff);

mvsi=mvsh=in=0;
sli=1;
podz=kolstr1=0;
double iznos_per[2];
memset(&iznos_per,'\0',sizeof(iznos_per));

#define STR1 8
#define STR2 10
#define STR3 11
double str_suma1[STR1]; /*строка с суммами для вывода*/
double ithz_suma1[STR1];
double itpd_suma1[STR1];
double itsh_suma1[STR1];
double itall_suma1[STR1];

double str_suma2[STR2]; /*строка с суммами для вывода*/
double ithz_suma2[STR2];
double itpd_suma2[STR2];
double itsh_suma2[STR2];
double itall_suma2[STR2];

double ithz_suma3[STR3];
double itpd_suma3[STR3];
double itsh_suma3[STR3];
double itall_suma3[STR3];

memset(str_suma1,'\0',sizeof(str_suma1));
memset(ithz_suma1,'\0',sizeof(ithz_suma1));
memset(itpd_suma1,'\0',sizeof(itpd_suma1));
memset(itsh_suma1,'\0',sizeof(itsh_suma1));
memset(itall_suma1,'\0',sizeof(itall_suma1));

memset(str_suma2,'\0',sizeof(str_suma2));
memset(ithz_suma2,'\0',sizeof(ithz_suma2));
memset(itpd_suma2,'\0',sizeof(itpd_suma2));
memset(itsh_suma2,'\0',sizeof(itsh_suma2));
memset(itall_suma2,'\0',sizeof(itall_suma2));

memset(ithz_suma3,'\0',sizeof(ithz_suma3));
memset(itpd_suma3,'\0',sizeof(itpd_suma3));
memset(itsh_suma3,'\0',sizeof(itsh_suma3));
memset(itall_suma3,'\0',sizeof(itall_suma3));
int kodmo=0;
int kodot=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  printw("%s %s %s %s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7]);
  if(poi.metka_sort == 0)
   {
    if(viduh == 0) //налоговый учёт
     hna.new_plus(row[6]);
    if(viduh == 1) //бух.учёт
     hna.new_plus(row[7]);
   }
  if(poi.metka_sort == 1)
    hna.new_plus(row[0]);

  if(poi.metr == 0) //По подразделениям
   {
//    podrp=pod=atoi(row[4]);
    kodot=atoi(row[4]);
   }
  if(poi.metr == 1) //По материально-ответственным
   {
    kodot=atoi(row[1]);
//    pod=atoi(row[1]);
//    podrp=atoi(row[4]);
   }

  pod=atoi(row[4]);
  kodmo=atoi(row[1]);
  in=atoi(row[2]);
  naimos.new_plus(row[3]);
  nomz.new_plus(row[5]);
  memset(iznos_per,'\0',sizeof(iznos_per));

  double bs_prih_za_per=0.;
  double bs_rash_za_per=0.;
  
  class bsiz_data data_per;

  if(poi.metr == 0) //По подразделениям
    bsizp(in,pod,0,dn,mn,gn,dk,mk,gk,&data_per,fil_prot.ff);
  else
    bsizp(in,0,kodmo,dn,mn,gn,dk,mk,gk,&data_per,fil_prot.ff);
//  data_per.bu.class_printw();  
//  OSTANOV();  
  if(viduh == 0)
   {
    pr1=data_per.nu.bs_nah_per; //Балансовая стоимость на начало периода
    pr2=data_per.nu.izmen_iznosa_nah_per+data_per.nu.amort_nah_per;  //Износ на начало периода
      
    pr11=data_per.nu.bs_kon_per; //Балансовая стоимость на конец периода
    
    pr22=data_per.nu.izmen_iznosa_kon_per+data_per.nu.amort_kon_per;  //Износ на конец периода
    
    iznos_per[0]+=data_per.nu.amort_raznica;     //Амортизация за период
    iznos_per[1]+=data_per.nu.izmen_iznosa_raznica; //Изменение износа за период
    bs_prih_za_per=data_per.nu.bs_prih_za_per;
    bs_rash_za_per=data_per.nu.bs_rash_za_per;
   }
  
  if(viduh == 1)
   {
    pr1=data_per.bu.bs_nah_per; //Балансовая стоимость на начало периода
    pr2=data_per.bu.izmen_iznosa_nah_per+data_per.bu.amort_nah_per;  //Износ на начало периода
    
    pr11=data_per.bu.bs_kon_per; //Балансовая стоимость на конец периода
    
    pr22=data_per.bu.izmen_iznosa_kon_per+data_per.bu.amort_kon_per;  //Износ на конец периода
    
    iznos_per[0]+=data_per.bu.amort_raznica;     //Амортизация за период
    iznos_per[1]+=data_per.bu.izmen_iznosa_raznica; //Изменение износа за период
    bs_prih_za_per=data_per.bu.bs_prih_za_per;
    bs_rash_za_per=data_per.bu.bs_rash_za_per;

   }
    
    
  
  if(fabs(pr1) < 0.01 && fabs(pr11) < 0.01) //Значит уже продан или списан или передан в другое подразделение
   {
    continue;
   }

  if(poi.metr == 0 && poi.kodolz.ravno()[0] != '\0') /*По подразделениям*/
   {
    //Может быть движение внутри подразделения по разным материально-ответственным лицам
    //в этом случае балансовая стоимость не равна нулю
    //Проверяем числился ли у этого мат. ответственного
    int pp=0,kk=0;
    if(poiin(in,dk,mk,gk,&pp,&kk) != 0)
      continue;
    sprintf(strsql,"%d",kk);
    if(proverka(poi.kodolz.ravno(),strsql,0,0) != 0)
      continue;
   }
  if(podz != kodot)
   {
    if(podz != 0)
     {
      bros.new_plus(podz);
      itogi(bros.ravno(),itpd_suma1,itpd_suma2,itpd_suma3,0,&kst,fil_ff.ff,fil_ffhpz.ff,fil_f.ff,fil_fi.ff,fil_fis.ff);
      memset(itpd_suma1,'\0',sizeof(itpd_suma1));
      memset(itpd_suma2,'\0',sizeof(itpd_suma2));
      memset(itpd_suma3,'\0',sizeof(itpd_suma3));
     }

    if(poi.metr == 0)
     {
      /*Читаем наименование подразделения*/
      sprintf(strsql,"select naik from Uospod where kod=%d",kodot);
      if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
       {
        beep();
        printw("%s %d !\n",gettext("Не найден код подразделения"),kodot);
        fprintf(fil_ff.ff,"%s %d !\n",gettext("Не найден код подразделения"),kodot);
        OSTANOV();
        naipod.new_plus("");
       }
      else
        naipod.new_plus(row1[0]);
     }

    if(poi.metr == 1)
     {
      /*Читаем мат.-ответственного*/
      sprintf(strsql,"select naik from Uosol where kod=%d",kodmo);
      if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
       {
        beep();
        printw("%s %d !\n",gettext("Не найден мат.-ответственный"),kodmo);
        fprintf(fil_ff.ff,"%s %d !\n",gettext("Не найден мат.-ответственный"),kodmo);
        OSTANOV();
        naipod.new_plus("");
       }
      else
        naipod.new_plus(row1[0]);
     }

    mvsh++;
             
    podz=kodot;
    if(poi.metr == 0)
     printw("\n%s - %d/%s\n",gettext("Подразделение"),podz,naipod.ravno());
    if(poi.metr == 1)
     printw("\n%s - %d/%s\n",gettext("Мат.-ответственный"),podz,naipod.ravno());
   }

  if(SRAV(hna1.ravno(),hna.ravno(),0) != 0)
   {
    if(hna1.ravno()[0] != '\0')
     {
      if(poi.metka_sort == 0)
       {
        itogi(hna1.ravno(),ithz_suma1,ithz_suma2,ithz_suma3,1,&kst,fil_ff.ff,fil_ffhpz.ff,fil_f.ff,fil_fi.ff,fil_fis.ff);
        memset(ithz_suma1,'\0',sizeof(ithz_suma1));
        memset(ithz_suma2,'\0',sizeof(ithz_suma2));
        memset(ithz_suma3,'\0',sizeof(ithz_suma3));

       }

      if(poi.metka_sort == 1)
       {
        itogi(hna1.ravno(),itsh_suma1,itsh_suma2,itsh_suma3,3,&kst,fil_ff.ff,fil_ffhpz.ff,fil_f.ff,fil_fi.ff,fil_fis.ff);
        memset(itsh_suma1,'\0',sizeof(itsh_suma1));
        memset(itsh_suma2,'\0',sizeof(itsh_suma2));
        memset(itsh_suma3,'\0',sizeof(itsh_suma3));
       }
     }
    
    if(poi.metka_sort == 0)
     {
      /*Уэнаем наименование шифра нормы аморт. отчисления*/
      if(SRAV(hna.ravno(),"???",0) != 0)
       {
        if(viduh == 0)
          sprintf(strsql,"select naik from Uosgrup where kod='%s'",hna.ravno());
        if(viduh == 1)
          sprintf(strsql,"select naik from Uosgrup1 where kod='%s'",hna.ravno());

        if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
         {
          beep();
          printw("%s %s !\n",gettext("Не найдено группу"),hna.ravno());
          fprintf(fil_ff.ff,"%s %s !\n",gettext("Не найдено группу"),hna.ravno());
          OSTANOV();
          naigrup.new_plus("");
         }
        else
          naigrup.new_plus(row1[0]);
       }
      else
       naigrup.new_plus(gettext("Не введена в карточку группа"));
      printw("\n%s %s \"%s\"\n",gettext("Группа"),hna.ravno(),naigrup.ravno());
     }
    if(poi.metka_sort == 1)
     {
      sprintf(strsql,"select nais from Plansh where ns='%s'",hna.ravno());
      if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
        naigrup.new_plus(row1[0]);
      else
        naigrup.new_plus("");
     
      printw("\n%s %s \"%s\"\n",gettext("Счёт учёта"),hna.ravno(),naigrup.ravno());
     }
    mvsi++;


    hna1.new_plus(hna.ravno());

   }

  if(mvsh > 0 || mvsi > 0) 
   {  
    if(kst > kol_strok_na_liste_l-KOLSTSH-5)
     {
      fprintf(fil_ff.ff,"\f");
      fprintf(fil_f.ff,"\f");
      sli++;
      kst=0;
     }
    if(mvsi > 0)
     {
      if(poi.metka_sort == 0)
       {
        fprintf(fil_ff.ff,"\n%s %s \"%s\"\n",gettext("Группа"),hna.ravno(),naigrup.ravno());
        fprintf(fil_f.ff,"\n%s %s \"%s\"\n",gettext("Группа"),hna.ravno(),naigrup.ravno());
       }
      if(poi.metka_sort == 1)
       {
        fprintf(fil_ff.ff,"\n%s %s \"%s\"\n",gettext("Счёт учёта"),hna.ravno(),naigrup.ravno());
        fprintf(fil_f.ff,"\n%s %s \"%s\"\n",gettext("Счёт учёта"),hna.ravno(),naigrup.ravno());
       }
      mvsi=0;
      kst+=2;
     } 
    haphna(dn,mn,gn,dk,mk,gk,sli,&kst,kodot,naipod.ravno(),poi.metr,fil_ff.ff,fil_f.ff,NULL,NULL);
    mvsh=0;
   }
  izn=pr2;
  bsn=pr1;
  osts=bsn-izn;

  if(osts >  0.009 && poi.metkaost == 1)
   continue;

  if(osts ==  0. && poi.metkaost == 2)
   continue;


  double izpr=pr22-pr2;


  if(fabs(bsn+izn+osts+pr11-pr1+pr22-pr2+bsn+pr11-pr1+izn+pr22+pr2+(bsn+pr11-pr1)+(izn+pr22-pr2)) > 0.009)
   {
    gsapphna(dn,mn,gn,dk,mk,gk,&sli,&kst,kodot,naipod.ravno(),poi.metr,fil_ff.ff,fil_f.ff);
    
    memset(str_suma1,'\0',sizeof(str_suma1));
    str_suma1[0]=bsn;
    str_suma1[1]=izn;
    str_suma1[2]=osts;
    str_suma1[3]=pr11-pr1;
    str_suma1[4]=izpr;
    str_suma1[5]=bsn+pr11-pr1;
    str_suma1[6]=izn+pr22-pr2;
//    str_suma1[7]=(bsn+pr11-pr1)-(izn+pr22-pr2);
    str_suma1[7]=str_suma1[5]-str_suma1[6];
    for(int nom=0; nom < STR1; nom++)
     {
      ithz_suma1[nom]+=str_suma1[nom];
      itpd_suma1[nom]+=str_suma1[nom];
      itsh_suma1[nom]+=str_suma1[nom];
      itall_suma1[nom]+=str_suma1[nom];
     }

    

    fprintf(fil_ff.ff,"\
%11d %-*.*s %-*.*s %15.2f %12.2f %12.2f %12.2f %12.2f %15.2f %12.2f %12.2f\n",
    in,
    iceb_tu_kolbait(30,naimos.ravno()),iceb_tu_kolbait(30,naimos.ravno()),naimos.ravno(),
    iceb_tu_kolbait(10,nomz.ravno()),iceb_tu_kolbait(10,nomz.ravno()),nomz.ravno(),
    str_suma1[0],
    str_suma1[1],
    str_suma1[2],
    str_suma1[3],
    str_suma1[4],
    str_suma1[5],
    str_suma1[6],
    str_suma1[7]);


    memset(str_suma2,'\0',sizeof(str_suma2));
    str_suma2[0]=bsn;
    str_suma2[1]=izn;
    str_suma2[2]=osts;
    str_suma2[3]=bs_prih_za_per;
    str_suma2[4]=bs_rash_za_per;
    str_suma2[5]=iznos_per[0];
    str_suma2[6]=iznos_per[1];
    str_suma2[7]=bsn+pr11-pr1;
    str_suma2[8]=izn+pr22-pr2;
    str_suma2[9]=(bsn+pr11-pr1)-(izn+pr22-pr2);
    for(int nom=0; nom < STR2; nom++)
     {
      ithz_suma2[nom]+=str_suma2[nom];
      itpd_suma2[nom]+=str_suma2[nom];
      itsh_suma2[nom]+=str_suma2[nom];
      itall_suma2[nom]+=str_suma2[nom];
     }

    
    int nom1=0;
    for(int nom=0; nom < STR2; nom++)
     {
      if(nom1 == 7)
       {
        itall_suma3[nom1]+=bs_prih_za_per-bs_rash_za_per;
        ithz_suma3[nom1]+=bs_prih_za_per-bs_rash_za_per;
        itsh_suma3[nom1]+=bs_prih_za_per-bs_rash_za_per;
        itpd_suma3[nom1++]+=bs_prih_za_per-bs_rash_za_per;
        nom--;
       }
      else
       {
        itall_suma3[nom1]+=str_suma2[nom];
        ithz_suma3[nom1]+=str_suma2[nom];
        itsh_suma3[nom1]+=str_suma2[nom];
        itpd_suma3[nom1++]+=str_suma2[nom];
       }
     }

    fprintf(fil_f.ff,"\
%11d %-*.*s %-*.*s %15.2f %12.2f %12.2f %12.2f %12.2f %10.2f %10.2f %15.2f %12.2f %12.2f\n",
    in,
    iceb_tu_kolbait(30,naimos.ravno()),iceb_tu_kolbait(30,naimos.ravno()),naimos.ravno(),
    iceb_tu_kolbait(10,nomz.ravno()),iceb_tu_kolbait(10,nomz.ravno()),nomz.ravno(),
    str_suma2[0],
    str_suma2[1],
    str_suma2[2],
    str_suma2[3],
    str_suma2[4],
    str_suma2[5],
    str_suma2[6],
    str_suma2[7],
    str_suma2[8],
    str_suma2[9]);

    

    if(iceb_tu_strlen(naimos.ravno()) > 30)
     {
      gsapphna(dn,mn,gn,dk,mk,gk,&sli,&kst,kodot,naipod.ravno(),poi.metr,fil_ff.ff,fil_f.ff);
      fprintf(fil_ff.ff,"%11s %s\n","",iceb_tu_adrsimv(30,naimos.ravno()));
      fprintf(fil_f.ff,"%11s %s\n","",iceb_tu_adrsimv(30,naimos.ravno()));
     }
    kolos++;
   }
 }

GDITE();


bros.new_plus(kodot);
itogi(bros.ravno(),itpd_suma1,itpd_suma2,itpd_suma3,0,&kst,fil_ff.ff,fil_ffhpz.ff,fil_f.ff,fil_fi.ff,fil_fis.ff);

if(poi.metka_sort == 0)
 itogi(hna1.ravno(),ithz_suma1,ithz_suma2,ithz_suma3,1,&kst,fil_ff.ff,fil_ffhpz.ff,fil_f.ff,fil_fi.ff,fil_fis.ff);

if(poi.metka_sort == 1)
 itogi(hna1.ravno(),itsh_suma1,itsh_suma2,itsh_suma3,3,&kst,fil_ff.ff,fil_ffhpz.ff,fil_f.ff,fil_fi.ff,fil_fis.ff);

/*Итог Общий*/
itogi("",itall_suma1,itall_suma2,itall_suma3,2,&kst,fil_ff.ff,fil_ffhpz.ff,fil_f.ff,fil_fi.ff,fil_fis.ff);


printw("\n%s:%d\n",gettext("Количество основных средств"),kolos);
fprintf(fil_ff.ff,"\n%s:%d\n",gettext("Количество основных средств"),kolos);
fprintf(fil_f.ff,"\n%s:%d\n",gettext("Количество основных средств"),kolos);
fprintf(fil_fi.ff,"\n%s:%d\n",gettext("Количество основных средств"),kolos);
fprintf(fil_fis.ff,"\n%s:%d\n",gettext("Количество основных средств"),kolos);
fprintf(fil_ffhpz.ff,"\n%s:%d\n",gettext("Количество основных средств"),kolos);

podpis(fil_ff.ff);
podpis(fil_f.ff);
podpis(fil_fi.ff);
podpis(fil_fis.ff);
podpis(fil_ffhpz.ff);
podpis(fil_prot.ff);

fil_ff.end();
fil_f.end();
fil_fi.end();
fil_fis.end();
fil_ffhpz.end();
fil_prot.end();


OSTANOV();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Ведомость износа и остаточной стоимости основных средств"));

oth->spis_imaf.plus(imafhpz);
oth->spis_naim.plus(gettext("Общие итоги"));

oth->spis_imaf.plus(imaf_f);
oth->spis_naim.plus(gettext("Ведомость с расшифровкой изменения балансовой стоимости и износа"));
oth->spis_imaf.plus(imaf_fi);
oth->spis_naim.plus(gettext("Общие итоги по ведомости с расшифровкой"));
oth->spis_imaf.plus(imaf_fis);
oth->spis_naim.plus(gettext("Общие итоги по ведомости с расшифровкой+остаточ.бал.стои"));
oth->spis_imaf.plus(imaf_prot);
oth->spis_naim.plus(gettext("Протокол хода расчёта"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),0);

printw_vr(tmm);

return(0);

}
/****************************************************/

void itogi(const char *sh,
double *str_suma1,
double *str_suma2,
double *str_suma3,
short mt,  //0-по подразделению 1- по группе 2-общий итог 3-по счету учёта
short *kst,
FILE *ff,FILE *ffhpz,FILE *ff_f,FILE *ff_fi,FILE *ff_fis)
{
char		bros[512];
char		str[312];

memset(str,'\0',sizeof(str));
memset(str,'-',164);

if(mt == 2)
 {
  sprintf(bros,gettext("Общий итог"));
 }

if(mt == 1)
 {
  if(sh[0] == '\0')
   return;
  sprintf(bros,"%s %s",gettext("Итого по группе"),sh);
  fprintf(ffhpz,"%s\n\
%11s %-*s %10s %15.2f %12.2f %12.2f %12.2f %12.2f \
%15.2f %12.2f %12.2f\n",str," ",iceb_tu_kolbait(30,bros),bros," ",
  str_suma1[0],
  str_suma1[1],
  str_suma1[2],
  str_suma1[3],
  str_suma1[4],
  str_suma1[5],
  str_suma1[6],
  str_suma1[7]);
 }

double suma=0.;
for(int nom=0; nom < STR1; nom++)
   suma+=str_suma1[nom];

if(mt == 0)
 {
  if(suma < 0.0001)
   return;
  sprintf(bros,"%s %s",gettext("Итого по подразделению"),sh);
 }

if(mt == 3)
 {
  if(suma < 0.0001)
   return;
  sprintf(bros,"%s %s",gettext("Итого по счёту"),sh);
 }

fprintf(ff,"%s\n\
%11s %-*s %10s %15.2f %12.2f %12.2f %12.2f %12.2f \
%15.2f %12.2f %12.2f\n",str," ",iceb_tu_kolbait(30,bros),bros," ",
str_suma1[0],
str_suma1[1],
str_suma1[2],
str_suma1[3],
str_suma1[4],
str_suma1[5],
str_suma1[6],
str_suma1[7]);

fprintf(ff_f,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff_f,"\
%11s %-*s %10s %15.2f %12.2f %12.2f %12.2f %12.2f %10.2f %10.2f\
 %15.2f %12.2f %12.2f\n"," ",iceb_tu_kolbait(30,bros),bros," ",
str_suma2[0],
str_suma2[1],
str_suma2[2],
str_suma2[3],
str_suma2[4],
str_suma2[5],
str_suma2[6],
str_suma2[7],
str_suma2[8],
str_suma2[9]);


fprintf(ff_fi,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff_fi,"\
%11s %-*s %10s %15.2f %12.2f %12.2f %12.2f %12.2f %10.2f %10.2f\
 %15.2f %12.2f %12.2f\n"," ",iceb_tu_kolbait(30,bros),bros," ",
str_suma2[0],
str_suma2[1],
str_suma2[2],
str_suma2[3],
str_suma2[4],
str_suma2[5],
str_suma2[6],
str_suma2[7],
str_suma2[8],
str_suma2[9]);

if(mt != 0)
 {
  fprintf(ff_fis,"\
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
  fprintf(ff_fis,"\
%11s %-*s %10s %15.2f %12.2f %12.2f %12.2f %12.2f %10.2f %10.2f %10.2f \
%15.2f %12.2f %12.2f\n"," ",iceb_tu_kolbait(30,bros),bros," ",
  str_suma2[0],
  str_suma2[1],
  str_suma2[2],
  str_suma2[3],
  str_suma2[4],
  str_suma2[5],
  str_suma2[6],
  str_suma2[7],
  str_suma2[8],
  str_suma2[9],
  str_suma2[10]);
 }
 
if(mt == 2 || mt == 3)
  fprintf(ffhpz,"%s\n\
%11s %-*s %10s %15.2f %12.2f %12.2f %12.2f %12.2f \
%15.2f %12.2f %12.2f\n",str," ",iceb_tu_kolbait(30,bros),bros," ",
  str_suma1[0],
  str_suma1[1],
  str_suma1[2],
  str_suma1[3],
  str_suma1[4],
  str_suma1[5],
  str_suma1[6],
  str_suma1[7]);

*kst=*kst+2;

printw("\n%s:\n\
%s %15.2f %15.2f %15.2f\n\
%s %15.2f %15.2f\n\
%s %15.2f %15.2f %15.2f\n",
bros,
gettext("Баланс. ст./Износ/Ост.ст.:"),
str_suma1[0],
str_suma1[1],
str_suma1[2],
gettext("Измение бал. ст./Износ   :"),
str_suma1[3],
str_suma1[4],
gettext("Баланс. ст./Износ/Ост.ст.:"),
str_suma1[5],
str_suma1[6],
str_suma1[7]);


}

/*******/
/*Счетчик строк*/
/*******/
void gsapphna(short dn,short mn,short gn,
short dk,short mk,short gk,
short *sl,  //Счетчик листов
short *kst, //Счетчик строк
int skl,const char *nai,
short mt,   //0-по подразделению 1-по материально-ответств
FILE *ff,FILE *ff_f)
{

*kst=*kst+1;

if(*kst <= kol_strok_na_liste_l)
 return;

*sl=*sl+1;
*kst=1;
fprintf(ff,"\f");
fprintf(ff_f,"\f");

haphna(dn,mn,gn,dk,mk,gk,*sl,kst,skl,nai,mt,ff,ff_f,NULL,NULL);


}

/********/
/*Шапка*/
/*******/

void haphna(short dn,short mn,short gn,
short dk,short mk,short gk,
short sl, //Счетчик листов
short *kst,  //Счетчик строк
int skl,const char *nai,
short mt,  //0-по подразделению 1-по материально-ответств
FILE *ff,
FILE *ff_f,
FILE *ff_fi,FILE *ff_fis)
{
char		str[312];
char		bros[312];

if(mt == 0)
 sprintf(bros,"%s %d %s",gettext("Подразделение"),skl,nai);

if(mt == 1)
 sprintf(bros,"%s %d %s",gettext("Мат.-ответственный"),skl,nai);

if(mt == 2)
 sprintf(bros,"%s",nai);

memset(str,'\0',sizeof(str));
memset(str,'-',164);

fprintf(ff,"\
%-*s %s N%d\n\%s\n",
iceb_tu_kolbait(150,bros),bros,
gettext("Лист"),
sl,str);

fprintf(ff,gettext("\
Инвентарный|     Наименование             |          |  Балансовая   |   Износ    | Остаточная | Изменение  |    Износ   |   Балансова   |    Износ   | Остаточная |\n"));
fprintf(ff,gettext("\
  номер    |                              |  Номер   | стоимость на  |    на      |стоимость на| балансовой |    за      | стоимость  на |    на      |стоимость на|\n"));
fprintf(ff,gettext("\
           |                              |          | %02d.%02d.%dг.  |%02d.%02d.%dг.|%02d.%02d.%dг.| стоимости  |   период   |  %02d.%02d.%dг. |%02d.%02d.%dг.|%02d.%02d.%dг.|\n"),
dn,mn,gn,dn,mn,gn,dn,mn,gn,dk,mk,gk,dk,mk,gk,dk,mk,gk);

fprintf(ff,"%s\n",str);

if(ff_f != NULL)
 {
  fprintf(ff_f,"\
%-*s %s N%d\n",
  iceb_tu_kolbait(150,bros),bros,
  gettext("Лист"),
  sl);

  fprintf(ff_f,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff_f,"\
Инвентарный|     Наименование             |          |  Балансовая   |   Износ    | Остаточная | Приход     |  Расход    |Амортизац.|Изменение |   Балансовая  |   Износ    | Остаточная |\n");
  fprintf(ff_f,"\
  номер    |                              |  Номер   | стоимасть на  |    на      |стоимость на| балансовой | балансовой |   за     |износа за | стоимость  на |    на      |стоимость на|\n");
  fprintf(ff_f,"\
           |                              |          | %02d.%02d.%dг.  |%02d.%02d.%dг.|%02d.%02d.%dг.| стоимости  | стоимости  | период   |  период  |  %02d.%02d.%dг. |%02d.%02d.%dг.|%02d.%02d.%dг.|\n",
  dn,mn,gn,dn,mn,gn,dn,mn,gn,dk,mk,gk,dk,mk,gk,dk,mk,gk);


  fprintf(ff_f,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
 }

if(ff_fi != NULL)
 {
  fprintf(ff_fi,"\
%-*s %s N%d\n",
  iceb_tu_kolbait(150,bros),bros,
  gettext("Лист"),
  sl);

  fprintf(ff_fi,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff_fi,"\
Инвентарный|     Наименование             |          |  Балансовая   |   Износ    | Остаточная | Приход     |  Расход    |Амортизац.|Изменение |   Балансовая  |   Износ    | Остаточная |\n");
  fprintf(ff_fi,"\
  номер    |                              |  Номер   | стоимость на  |    на      |стоимость на| балансовой | балансовой |   за     |износа за | стоимость  на |    на      |стоимость на|\n");
  fprintf(ff_fi,"\
           |                              |          | %02d.%02d.%dг.  |%02d.%02d.%dг.|%02d.%02d.%dг.| стоимости  | стоимости  | период   |  период  |  %02d.%02d.%dг. |%02d.%02d.%dг.|%02d.%02d.%dг.|\n",
  dn,mn,gn,dn,mn,gn,dn,mn,gn,dk,mk,gk,dk,mk,gk,dk,mk,gk);


  fprintf(ff_fi,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
 }

if(ff_fis != NULL)
 {
  fprintf(ff_fis,"\
%-*s %s N%d\n",
  iceb_tu_kolbait(150,bros),bros,
  gettext("Лист"),
  sl);

  fprintf(ff_fis,"\
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff_fis,"\
Инвентарный|     Наименование             |          |  Балансовая   |   Износ    | Остаточная | Приход     |  Расход    |Амортизац.|Изменение |Изменение |   Балансовая  |   Износ    | Остаточная |\n");
  fprintf(ff_fis,"\
  номер    |                              |  Номер   | стоимость на  |    на      |стоимость на| балансовой | балансовой |   за     |износа за |остаточной| стоимость  на |    на      |стоимость на|\n");
  fprintf(ff_fis,"\
	   |                              |          | %02d.%02d.%dг.  |%02d.%02d.%dг.|%02d.%02d.%dг.| стоимости  | стоимости  | период   |  период  |стоимости |  %02d.%02d.%dг. |%02d.%02d.%dг.|%02d.%02d.%dг.|\n",
  dn,mn,gn,dn,mn,gn,dn,mn,gn,dk,mk,gk,dk,mk,gk,dk,mk,gk);


  fprintf(ff_fis,"\
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
 }
 
*kst=*kst+KOLSTSH;

}
/**********************************/
/*Начало отчётов*/
/***********************************/

void uosvosii_start(short dn,short mn,short gn,
short dk,short mk,short gk,
short *kst,
class uosvosii_poi *poi,
FILE *ff)
{

*kst=5;

iceb_t_zagolov(gettext("Ведомость износа и остаточной стоимости основных средств"),dn,mn,gn,dk,mk,gk,ff);

*kst+=1;

if(poi->viduh == 0)
 {
  fprintf(ff,"%s.\n",gettext("Налоговый учёт"));
 }
if(poi->viduh == 1)
 {
  fprintf(ff,"%s.\n",gettext("Бухгалтерский учёт"));
 }

int klst=0;
if(poi->podrz.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Подразделение"),poi->podrz.ravno());
   printcod(ff,"Uospod","kod","naik",0,poi->podrz.ravno(),&klst);
  *kst+=klst+1;
 }

if(poi->hnaz.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Группа налогового учёта"),poi->hnaz.ravno());
  printcod(ff,"Uosgrup","kod","naik",1,poi->hnaz.ravno(),&klst);
  *kst+=klst+1;
 }
if(poi->hnabyz.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Группа бухгалтерского учёта"),poi->hnabyz.ravno());
  printcod(ff,"Uosgrup1","kod","naik",1,poi->hnabyz.ravno(),&klst);
  *kst+=klst+1;
 }
if(poi->kodolz.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Мат.-ответственный"),poi->kodolz.ravno());
  printcod(ff,"Uosol","kod","naik",1,poi->kodolz.ravno(),&klst);
  *kst+=klst+1;
 }
if(poi->hauz.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Шифр аналитического учета"),poi->hauz.ravno());
  printcod(ff,"Uoshau","kod","naik",0,poi->hauz.ravno(),&klst);
  *kst+=klst+1;
 }
if(poi->shetupoi.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Счёт"),poi->shetupoi.ravno());
  *kst+=1;
 }

if(poi->innomp.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Инвентарный номер"),poi->innomp.ravno());
  *kst+=1;

 }
 
if(poi->hztz.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Шифр затрат"),poi->hztz.ravno());
  *kst+=1;
 }

if(poi->sosob.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Состояние объекта"),poi->sosob.ravno());
  *kst+=1;
 }

if(poi->metkaost == 1)
 {
  fprintf(ff,"%s\n",gettext("Основные средства с нулевою остаточной стоимостью"));
  *kst+=1;
 }
if(poi->metkaost == 2)
 {
  fprintf(ff,"%s\n",gettext("Основные средства с ненулевой остаточной стоимостью"));
  *kst+=1;
 }
*kst+=1;
if(poi->metr == 0)
 fprintf(ff,"%s\n",gettext("По подразделениям"));
if(poi->metr == 1)
 fprintf(ff,"%s\n",gettext("По материально-ответственным"));

*kst+=1;
if(poi->metka_sort == 0)
 fprintf(ff,"%s\n",gettext("Сортировка по группам"));
if(poi->metka_sort == 1)
 fprintf(ff,"%s\n",gettext("Сортировать по счетам учёта"));

}

/****************************/
/*проверка реквизитов поиска*/
/*****************************/
int uosvosii_rek(int in,
const char *naim,
short mn,short gn,
short mk,short gk,
int poddz,
int kodotl,
class uosvosii_poi *poi,
const char *imatmptab)
{
class poiinpdw_data rekin;

if(poiinpd(in,mn,gn,&rekin) == 3) /*Небыло на начало периода*/
 {
  poiinpd(in,mk,gk,&rekin);

 } 

if(poi->viduh == 0) //Пропускаем все основные средства если для них амортизазия считается в целом по группе
 if(uosprovgr(rekin.hna.ravno()) == 1)
    return(1);

if(poi->viduh == 0 ) //Налоговый учёт
 {
  if(rekin.soso == 0 || rekin.soso == 3) //амортизация считается
   if(poi->sosob.ravno()[0] == '-')
    return(1);
  if(rekin.soso == 1 || rekin.soso == 2) //амортизация не считаестя
   if(poi->sosob.ravno()[0] == '+')
    return(1);
  
 }
if(poi->viduh == 1 ) //Бух.учёт
 {
  if(rekin.soso == 0 || rekin.soso == 2) //амортизация считается
   if(poi->sosob.ravno()[0] == '-')
    return(1);
  if(rekin.soso == 1 || rekin.soso == 3) //амортизация не считаестя
   if(poi->sosob.ravno()[0] == '+')
    return(1);
  
 }
if(proverka(poi->shetupoi.ravno(),rekin.shetu.ravno(),1,0) != 0)
  return(1);

if(proverka(poi->hztz.ravno(),rekin.hzt.ravno(),0,0) != 0)
  return(1);

if(proverka(poi->hnaz.ravno(),rekin.hna.ravno(),0,0) != 0)
  return(1);

if(proverka(poi->hnabyz.ravno(),rekin.hnaby.ravno(),0,0) != 0)
  return(1);

if(proverka(poi->hauz.ravno(),rekin.hau.ravno(),0,0) != 0)
  return(1);
char strsql[1024];
sprintf(strsql,"insert into %s values ('%s',%d,%d,'%s',%d,'%s','%s','%s')",
imatmptab,
rekin.shetu.ravno(),kodotl,in,iceb_tu_sqlfiltr(naim),poddz,rekin.nomz.ravno(),rekin.hna.ravno(),rekin.hnaby.ravno());
//printw("%s\n",strsql);
sql_zapis(strsql,1,1);

return(0);
}
/*********************************/
/*определение где и с какими реквизитами числится основное средство на указанную дату*/
/*************************************/
int uosvosii_poi(int in,
const char *naim,
short dn,short mn,short gn,
short dk,short mk,short gk,
class uosvosii_poi *poi,
const char *imatmptab)
{
SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kolstr=0;
char strsql[512];
int poddz=0;
int kodotl=0;

if(poiin(in,dn,mn,gn,&poddz,&kodotl) == 0) 
 {
  if(proverka(poi->podrz.ravno(),poddz,0,0) == 0)
   if(proverka(poi->kodolz.ravno(),kodotl,0,0) == 0)
    {
     uosvosii_rek(in,naim,mn,gn,mk,gk,poddz,kodotl,poi,imatmptab);
    }
 }

/*просмотриваем период -может быть куча внутренних перемещений*/

sprintf(strsql,"select datd,podr,nomd from Uosdok1 where \
datd >= '%04d-%d-%d' and datd <= '%04d-%d-%d' and innom=%d and podt=1 \
order by datd asc, tipz desc",gn,mn,dn,gk,mk,dk,in);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
short d,m,g;
while(cur.read_cursor(&row) != 0)
 {
  if(proverka(poi->podrz.ravno(),row[1],0,0) != 0)
   continue;
  poddz=atoi(row[1]);
  rsdat(&d,&m,&g,row[0],2);
  /**************Читаем документ*/
  sprintf(strsql,"select kodol from Uosdok where datd='%s' and \
  nomd='%s'",row[0],row[2]);

  //printw("%s\n",strsql);

  if(readkey(strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"Не найден документ N%s от %s !",row[2],row[0]);
    iceb_t_soob(strsql);
    continue;
   }
  kodotl=atoi(row1[0]);
  if(proverka(poi->kodolz.ravno(),row1[0],0,0) != 0)
   continue;

  uosvosii_rek(in,naim,mn,gn,mk,gk,poddz,kodotl,poi,imatmptab);
 }




return(0);
}
