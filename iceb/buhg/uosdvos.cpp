/* $Id: uosdvos.c,v 5.59 2013/11/05 10:50:49 sasa Exp $ */
/*16.07.2015    15.08.1996      Белых А.И.      uosdvos.c
Получение ведомости движения основных средств
*/
#include        <errno.h>
#include        "buhg.h"
#include "poiinpd.h"

#define         KOLSTSH  6  //Количество строк в шапке 

void gsapp(short,short,short,short,short,short,short *,short *,int,const char*,short,FILE*,FILE*);
void shsss(short,short,short,short,short,short,short *,short *,int,const char*,short,FILE*,FILE*);
void itsh(const char*,short,short,short,short,double,double,double,double,double,double,double,double,short,short*,FILE*,FILE*);
void uosdvos_shet_uf(short dn,short mn,short gn,short dk,short mk,short gk,int *sl,int *kst,int skl,const char *nai,short mt,FILE *ff_nal,FILE *ff_buh);
void uosdvos_suf(short dn,short mn,short gn,short dk,short mk,short gk,int *sl,int *kst,int skl,const char *nai,short mt,FILE *ff_nal,FILE *ff_buh);
void uosdvos_itog(const char *kod,short k1,short k2,short k3,short k4,double s1,double s2,double s3,double s4,double s1by,double s2by,double s3by,double s4by,short met,int *kst,FILE *ff_nal,FILE *ff_buh);
int uosdvos_poi(int mt,int in,short dn,short mn,short gn,short dk,short mk,short gk,int metkasort,const char *naimos,const char *podrz,const char *kodolz,const char *hnaz,const char *hnabyz,const char *shetupoi,const char *hauz,const char *imatmptab,FILE *ffprot);


extern short	startgoduos; /*Стартовый год*/
extern int kol_strok_na_liste;

int uosdvos(short mt, //0-ведомость движения по подразделению 1- по мат.ответственным
class spis_oth *oth)
{
FILE            *ff,*ffby;
time_t          tmm;
static class iceb_tu_str dat1(""),dat2("");
int           kom,kom1;
static class iceb_tu_str podrz(""); /*Код подразделения*/
static class iceb_tu_str kodolz(""); /*Коды ответственных лиц*/
static class iceb_tu_str hnaz(""); //Группы налогового учёта
static class iceb_tu_str hnabyz(""); //Группы бухгалтерского учёта
static class iceb_tu_str shetupoi(""); //Счёт учета
static class iceb_tu_str hauz("");     //Шифр аналитического учета
static class iceb_tu_str inventnom("");
short           dn,mn,gn;
short           dk,mk,gk;
char            imaf[64],imafby[64];
short           k1,k2,k3,k4,kk1,kk2,kk3,kk4;
short           k11,k21,k31,k41;
double		s1=0.,s2=0.,s3=0;
double		ss1,ss2,ss3; /* Итоги по подр*/
double          s4,ss4;
double		s1by=0.,s2by=0.,s3by=0.;
double		ss1by=0.,ss2by=0.,ss3by=0.; /* Итоги по подр*/
double          s4by=0.,ss4by=0.;
double          s11,s21,s31,s41;
double          s11by=0.,s21by=0.,s31by=0.,s41by=0.;
int            in;
short           kst=0,sli;
class iceb_tu_str naimos; /*Наименование подразделения*/
long		kolstr,kolstr1;
SQL_str         row,row1;
char		strsql[1024];
int		pod=0,podz=0;
short		mnp;   /* 0-старое подразделение 1-новое*/
class iceb_tu_str naishet("");
int		vids=0;
/*Переменные данные по инвентарному номеру*/

class iceb_tu_str shetu(""); /*Счёт учета*/
class iceb_tu_str shetuz(""); 
class iceb_tu_str nomz("");  /*Номерной знак*/
class iceb_tu_str naipod("");
int		klst=0;
SQLCURSOR curr;
VVOD MENU(3);
VVOD VV(0);
if(mt == 0)
 VV.VVOD_SPISOK_add_ZAG(gettext("Движение основных средств по подразделениям"));
if(mt == 1)
 VV.VVOD_SPISOK_add_ZAG(gettext("Движение основных средств по мат.-ответственным"));

VV.VVOD_SPISOK_add_ZAG(gettext("Введите данные"));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(hnaz.ravno(),128);
VV.VVOD_SPISOK_add_data(hnabyz.ravno(),128);
VV.VVOD_SPISOK_add_data(podrz.ravno(),128);
VV.VVOD_SPISOK_add_data(kodolz.ravno(),128);
VV.VVOD_SPISOK_add_data(shetupoi.ravno(),128);
VV.VVOD_SPISOK_add_data(hauz.ravno(),128);
VV.VVOD_SPISOK_add_data(inventnom.ravno(),128);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала...(д.м.г)..."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца....(д.м.г)..."));
VV.VVOD_SPISOK_add_MD(gettext("Группа нал.учета..(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Группа бух.учета..(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Подразделение.....(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Мат.ответственный.(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт учета........(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Шифр анал. учета..(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Инвентарный номер.(,,).."));

naz:;

clear();

helstr(LINES-1,0,"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));

hnaz.new_plus(VV.data_ravno(2));
hnabyz.new_plus(VV.data_ravno(3));

podrz.new_plus(VV.data_ravno(4));
kodolz.new_plus(VV.data_ravno(5));
shetupoi.new_plus(VV.data_ravno(6));
hauz.new_plus(VV.data_ravno(7));
inventnom.new_plus(VV.data_ravno(8));
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
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп налогового учёта"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп бухгалтерского учёта"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка мат.-ответственны"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка шифров аналитического учета"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 5 :
      case -1:
       goto naz;

      case 0 :
        vibrek("Uosgrup",&hnaz,&naim_v);
        VV.VVOD_SPISOK_zapis_data(2,hnaz.ravno());
        goto naz;

      case 1 :
        vibrek("Uosgrup1",&hnabyz,&naim_v);
        VV.VVOD_SPISOK_zapis_data(3,hnabyz.ravno());
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
        vibrek("Uoshau",&hauz,&naim_v);
        VV.VVOD_SPISOK_zapis_data(7,hauz.ravno());
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

MENU.VVOD_delete();
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите вид сортировки"));

MENU.VVOD_SPISOK_add_MD(gettext("Сортировать по счетам учета"));
MENU.VVOD_SPISOK_add_MD(gettext("Сортировать по шифрам производственных затрат налогового учёта"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

int metkasort=0; 
while(menu3w(stdscr,&MENU,&metkasort,-1,-1,0) != 0);

if(metkasort == -1 || metkasort == 2 )
  return(1);
  
clear();
refresh();

vids=2;
if(metkasort == 1)
 vids=3;      

printw("Период %d.%d.%d > %d.%d.%d\n",dn,mn,gn,dk,mk,gk);
printw("%s\n",gettext("Сортируем записи"));

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
class iceb_t_tmptab tabtmp;
const char *imatmptab={"uosdvos"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));


sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
shu char(24) not null,\
podmat int not null,\
innom int not null,\
naim char(112) not null,\
nomz char(56) not null,\
hzt char(32) not null,\
unique(shu,podmat,innom)) ENGINE = MYISAM",imatmptab);
//unique(shu,pod,innom,kmo)) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  

char imaf_prot[64];
sprintf(imaf_prot,"prot%d.lst",getpid());
class iceb_fopen ff_prot;
if(ff_prot.start(imaf_prot,"w") != 0)
 return(1);

kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);  
  if(proverka(inventnom.ravno(),row[0],0,0) != 0)
      continue;
    
  in=atol(row[0]);
  fprintf(ff_prot.ff,"Инвентарный номер-%d\n",in);
  /*Смотрим где оно было на начало периода*/
  uosdvos_poi(mt,in,dn,mn,gn,dk,mk,gk,metkasort,row[1],podrz.ravno(),kodolz.ravno(),hnaz.ravno(),hnabyz.ravno(),shetupoi.ravno(),hauz.ravno(),imatmptab,ff_prot.ff);
  

 }

if(metkasort == 0) /*по счёту учёта*/
    sprintf(strsql,"select * from %s order by shu asc,podmat asc,innom asc",imatmptab);
if(metkasort == 1) /*по группе*/
    sprintf(strsql,"select * from %s order by hzt asc,podmat asc,innom asc",imatmptab);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(mt == 0)
 {
  sprintf(imafby,"vdpbu%d.lst",getpid());
  sprintf(imaf,"vdpod%d.lst",getpid());
 }
if(mt == 1)
 {
  sprintf(imafby,"vdobu%d.lst",getpid());
  sprintf(imaf,"vdotl%d.lst",getpid());
 }
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }


if((ffby = fopen(imafby,"w")) == NULL)
 {
  error_op_nfil(imafby,errno,"");
  return(1);
 }



//файлы для узкой формы распечатки 
char imaf_buh[64];
char imaf_nal[64];
FILE *ff_buh;
FILE *ff_nal;
int sl_uz=0; //Счётчик листов для узкой форми
int kolstrok_uz=0; //Счётчик строк для узкой форми
sprintf(imaf_buh,"vdbuh%d.lst",getpid());
sprintf(imaf_nal,"vdnal%d.lst",getpid());

if((ff_buh = fopen(imaf_buh,"w")) == NULL)
 {
  error_op_nfil(imaf_buh,errno,"");
  return(1);
 }



if((ff_nal = fopen(imaf_nal,"w")) == NULL)
 {
  error_op_nfil(imaf_nal,errno,"");
  return(1);
 }


time(&tmm);

sprintf(strsql,"%s. (%s.)",
gettext("Оборотная ведомость движения основных средств"),
gettext("Налоговый учёт"));
iceb_t_zagolov(strsql,dn,mn,gn,dk,mk,gk,ff);
iceb_t_zagolov(strsql,dn,mn,gn,dk,mk,gk,ff_nal);

sprintf(strsql,"%s. (%s.)",
gettext("Оборотная ведомость движения основных средств"),
gettext("Бухгалтерский учёт"));
iceb_t_zagolov(strsql,dn,mn,gn,dk,mk,gk,ffby);
iceb_t_zagolov(strsql,dn,mn,gn,dk,mk,gk,ff_buh);

kolstrok_uz=5;
sl_uz=1;
sli=1;
kst=5;

if(podrz.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Подразделение"),podrz.ravno());
  printcod(ff,"Uospod","kod","naik",0,podrz.ravno(),&klst);
  fprintf(ffby,"%s: %s\n",gettext("Подразделение"),podrz.ravno());
  printcod(ffby,"Uospod","kod","naik",0,podrz.ravno(),&klst);

  fprintf(ff_buh,"%s: %s\n",gettext("Подразделение"),podrz.ravno());
  printcod(ff_buh,"Uospod","kod","naik",0,podrz.ravno(),&klst);
  fprintf(ff_nal,"%s: %s\n",gettext("Подразделение"),podrz.ravno());
  printcod(ff_nal,"Uospod","kod","naik",0,podrz.ravno(),&klst);

  kolstrok_uz+=klst+1;
  kst+=klst+1;
 }
if(hnaz.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Группа налогового учёта"),hnaz.ravno());
  printcod(ff,"Uosgrup","kod","naik",1,hnaz.ravno(),&klst);

  fprintf(ffby,"%s: %s\n",gettext("Группа налогового учёта"),hnaz.ravno());
  printcod(ffby,"Uosgrup","kod","naik",1,hnaz.ravno(),&klst);

  fprintf(ff_nal,"%s: %s\n",gettext("Группа налогового учёта"),hnaz.ravno());
  printcod(ff_nal,"Uosgrup","kod","naik",1,hnaz.ravno(),&klst);

  fprintf(ff_buh,"%s: %s\n",gettext("Группа налогового учёта"),hnaz.ravno());
  printcod(ff_buh,"Uosgrup","kod","naik",1,hnaz.ravno(),&klst);

  kolstrok_uz+=klst+1;
  kst+=klst+1;
 }

if(hnabyz.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Группа бухгалтерского учёта"),hnabyz.ravno());
  printcod(ff,"Uosgrup1","kod","naik",1,hnabyz.ravno(),&klst);

  fprintf(ffby,"%s: %s\n",gettext("Группа бухгалтерского учёта"),hnabyz.ravno());
  printcod(ffby,"Uosgrup1","kod","naik",1,hnabyz.ravno(),&klst);

  fprintf(ff_nal,"%s: %s\n",gettext("Группа бухгалтерского учёта"),hnabyz.ravno());
  printcod(ff_nal,"Uosgrup1","kod","naik",1,hnabyz.ravno(),&klst);

  fprintf(ff_buh,"%s: %s\n",gettext("Группа бухгалтерского учёта"),hnabyz.ravno());
  printcod(ff_buh,"Uosgrup1","kod","naik",1,hnabyz.ravno(),&klst);

  kolstrok_uz+=klst+1;
  kst+=klst+1;
 }
if(kodolz.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Мат.-ответственный"),kodolz.ravno());
  printcod(ff,"Uosol","kod","naik",1,kodolz.ravno(),&klst);
  fprintf(ffby,"%s: %s\n",gettext("Мат.-ответственный"),kodolz.ravno());
  printcod(ffby,"Uosol","kod","naik",0,kodolz.ravno(),&klst);

  fprintf(ff_nal,"%s: %s\n",gettext("Мат.-ответственный"),kodolz.ravno());
  printcod(ff_nal,"Uosol","kod","naik",1,kodolz.ravno(),&klst);
  fprintf(ff_buh,"%s: %s\n",gettext("Мат.-ответственный"),kodolz.ravno());
  printcod(ff_buh,"Uosol","kod","naik",0,kodolz.ravno(),&klst);

  kolstrok_uz+=klst+1;
  kst+=klst+1;
 }
if(shetupoi.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Счёт"),shetupoi.ravno());
  fprintf(ffby,"%s: %s\n",gettext("Счёт"),shetupoi.ravno());

  fprintf(ff_nal,"%s: %s\n",gettext("Счёт"),shetupoi.ravno());
  fprintf(ff_buh,"%s: %s\n",gettext("Счёт"),shetupoi.ravno());

  kolstrok_uz++;
  kst++;
 }
if(hauz.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Шифр аналитического учета"),hauz.ravno());
  printcod(ff,"Uoshau","kod","naik",0,hauz.ravno(),&klst);
  fprintf(ffby,"%s: %s\n",gettext("Шифр аналитического учета"),hauz.ravno());
  printcod(ffby,"Uoshau","kod","naik",0,hauz.ravno(),&klst);

  fprintf(ff_nal,"%s: %s\n",gettext("Шифр аналитического учета"),hauz.ravno());
  printcod(ff_nal,"Uoshau","kod","naik",0,hauz.ravno(),&klst);
  fprintf(ff_buh,"%s: %s\n",gettext("Шифр аналитического учета"),hauz.ravno());
  printcod(ff_buh,"Uoshau","kod","naik",0,hauz.ravno(),&klst);

  kolstrok_uz+=klst+1;
  kst+=klst+1;
 }

uosdvos_suf(dn,mn,gn,dk,mk,gk,&sl_uz,&kolstrok_uz,pod,naipod.ravno(),mt,ff_nal,ff_buh);

k1=k2=k3=k4=kk1=kk2=kk3=kk4=0;
k11=k21=k31=k41=0;
ss1=s2=ss2=s3=ss3=s4=ss4=0.;
s11=s21=s31=s41=0.;
mnp=in=0;
podz=-1;
class ostuos_data ost_in;
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  
  fprintf(ff_prot.ff,"%s %s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4],row[5]);
  
  pod=atoi(row[1]);

  if(metkasort == 0)
    shetu.new_plus(row[0]);
  if(metkasort == 1)
    shetu.new_plus(row[5]);
  
  in=atoi(row[2]);
  naimos.new_plus(row[3]);
  nomz.new_plus(row[4]);
  
  if(shetu.ravno()[0] == '?')
   {
    sprintf(strsql,"Для инвентарного номера %d не введён счёт учёта!",in);
    iceb_t_soob(strsql);
   }
  

  if(podz != pod)
   {
    mnp=1;
    if(podz != -1)
     {
      sprintf(strsql,"%d",podz);
      itsh(strsql,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,ss1by,ss2by,ss3by,ss4by,mt,&kst,ff,ffby);
      uosdvos_itog(strsql,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,ss1by,ss2by,ss3by,ss4by,mt,&kolstrok_uz,ff_nal,ff_buh);
      ss1=ss2=ss3=ss4=0.;
      ss1by=ss2by=ss3by=ss4by=0.;
      kk1=kk2=kk3=kk4=0;
     }

    if(mt == 0)
     {
      /*Читаем наименование подразделения*/
      sprintf(strsql,"select naik from Uospod where kod=%d",pod);
      if(readkey(strsql,&row1,&curr) != 1)
       {
        beep();
        printw("Не найден код подразделения %d !\n",pod);
        fprintf(ff,"Не найден код подразделения %d !\n",pod);
        naipod.new_plus("");
       }
      else
        naipod.new_plus(row1[0]);
     }

    if(mt == 1)
     {
      /*Читаем мат.-ответственного*/
      sprintf(strsql,"select naik from Uosol where kod=%d",pod);
      if(readkey(strsql,&row1,&curr) != 1)
       {
        beep();
        printw("Не найден мат.-ответственный %d !\n",pod);
        fprintf(ff,"Не найден мат.-ответственный %d !\n",pod);
        naipod.new_plus("");
       }
      else
        naipod.new_plus(row1[0]);
     }

    podz=pod;
   }

  if(SRAV(shetu.ravno(),shetuz.ravno(),0) != 0)
   {
    if(shetuz.ravno()[0] != '\0')
     {
      itsh(shetuz.ravno(),k1,k2,k3,k4,s1,s2,s3,s4,s1by,s2by,s3by,s4by,vids,&kst,ff,ffby);
      uosdvos_itog(shetuz.ravno(),k1,k2,k3,k4,s1,s2,s3,s4,s1by,s2by,s3by,s4by,vids,&kolstrok_uz,ff_nal,ff_buh);
      s1=s2=s3=s4=0.;
      s1by=s2by=s3by=s4by=0.;
      k1=k2=k3=k4=0;
     }
    if(metkasort == 0)
     {
      if(shetu.ravno()[0] != '?')
       { 
        sprintf(strsql,"select nais from Plansh where ns='%s'",shetu.ravno());
        if(readkey(strsql,&row1,&curr) != 1)
         {
          VVOD SOOB(1);
          sprintf(strsql,gettext("Не найден счёт %s в плане счетов !"),shetu.ravno());
          SOOB.VVOD_SPISOK_add_MD(strsql);
          soobshw(&SOOB,stdscr,-1,-1,0,1);

          naishet.new_plus("");
          fprintf(ff,gettext("Не найден счёт %s в плане счетов !"),shetu.ravno());
          fprintf(ff,"\n");
         }
        else
          naishet.new_plus(row1[0]);
       }
      fprintf(ff,"%s: %s %s\n",gettext("Счёт"),shetu.ravno(),naishet.ravno());
      fprintf(ffby,"%s: %s %s\n",gettext("Счёт"),shetu.ravno(),naishet.ravno());
      kst++;
     }
    if(metkasort == 1)
     {
      sprintf(strsql,"select naik from Uoshz where kod='%s'",shetu.ravno());
      if(readkey(strsql,&row1,&curr) != 1)
       {
        sprintf(strsql,"%s %s !",gettext("Не найден шифр производственных затрат"),shetu.ravno());
        iceb_t_soob(strsql);
        naishet.new_plus("");
        fprintf(ff,"%s %s !\n",gettext("Не найден шифр производственных затрат"),shetu.ravno());
       }
      else
        naishet.new_plus(row1[0]);
      fprintf(ff,"%s: %s %s\n",gettext("Шифр затрат"),shetu.ravno(),naishet.ravno());
      fprintf(ffby,"%s: %s %s\n",gettext("Шифр затрат"),shetu.ravno(),naishet.ravno());
      kst++;
     }

    shetuz.new_plus(shetu.ravno());
   }

  fprintf(ff_prot.ff,"in=%d %d.%d.%d > %d.%d.%d pod=%d mt=%d\n",in,dn,mn,gn,dk,mk,gk,pod,mt);
  ostuos(in,dn,mn,gn,dk,mk,gk,pod,mt,&ost_in,ff_prot.ff);
  
  fprintf(ff_prot.ff,"\
ostgcby[0]=%.2f\n\
ostgcby[1]=%.2f\n\
ostgcby[2]=%.2f\n\
ostgcby[3]=%.2f\n",
  ost_in.ostgcby[0],
  ost_in.ostgcby[1],
  ost_in.ostgcby[2],
  ost_in.ostgcby[3]);

  fprintf(ff_prot.ff,"\n\
ostmcby[0]=%.2f\n\
ostmcby[1]=%.2f\n\
ostmcby[2]=%.2f\n\
ostmcby[3]=%.2f\n",
  ost_in.ostmcby[0],
  ost_in.ostmcby[1],
  ost_in.ostmcby[2],
  ost_in.ostmcby[3]);
  
  if(ost_in.ostm[0] != 0 || ost_in.ostm[1] != 0 || ost_in.ostm[3] != 0)
   {
    if(mnp == 1)
     {
      if(kst > kol_strok_na_liste-KOLSTSH-5)
       {
        fprintf(ff,"\f");
        fprintf(ffby,"\f");
//        fprintf(ff_nal,"\f");
//        fprintf(ff_buh,"\f");
        sli++;
//        sl_uz++;
        kst=0;
//        kolstrok_uz=0; //Сразу за выводом шапки идет запуск счётчика
       }
      else
       {
        gsapp(dn,mn,gn,dk,mk,gk,&sli,&kst,pod,naipod.ravno(),mt,ff,ffby);
//        uosdvos_shet_uf(dn,mn,gn,dk,mk,gk,&sl_uz,&kolstrok_uz,pod,naipod.ravno(),mt,ff_nal,ff_buh);
        fprintf(ff,"\n");
        fprintf(ffby,"\n");
//        fprintf(ff_nal,"\n");
//        fprintf(ff_buh,"\n");
       }               
      uosdvos_shet_uf(dn,mn,gn,dk,mk,gk,&sl_uz,&kolstrok_uz,pod,naipod.ravno(),mt,ff_nal,ff_buh);
      if(mt == 0)
       {
        printw("\n%s:%d %s\n",gettext("Поразделение"),podz,naipod.ravno());
        fprintf(ff_buh,"%s:%d %s\n",gettext("Подразделение"),podz,naipod.ravno());
        fprintf(ff_nal,"%s:%d %s\n",gettext("Подразделение"),podz,naipod.ravno());
       }
      if(mt == 1)
       {
        printw("\n%s:%d %s\n",gettext("Мат.-ответственный"),podz,naipod.ravno());
        fprintf(ff_buh,"%s:%d %s\n",gettext("Материально ответственный"),podz,naipod.ravno());
        fprintf(ff_nal,"%s:%d %s\n",gettext("Материально ответственный"),podz,naipod.ravno());
       }             

      shsss(dn,mn,gn,dk,mk,gk,&sli,&kst,pod,naipod.ravno(),mt,ff,ffby);
      mnp=0;
     }
    gsapp(dn,mn,gn,dk,mk,gk,&sli,&kst,pod,naipod.ravno(),mt,ff,ffby);
    uosdvos_shet_uf(dn,mn,gn,dk,mk,gk,&sl_uz,&kolstrok_uz,pod,naipod.ravno(),mt,ff_nal,ff_buh);

    fprintf(ff,"\
%11d %-*s %-*.*s %15d %15.2f %15d %15.2f %15d %15.2f %15d %15.2f\n",
in,
iceb_tu_kolbait(40,naimos.ravno()),naimos.ravno(),
iceb_tu_kolbait(10,nomz.ravno()),iceb_tu_kolbait(10,nomz.ravno()),nomz.ravno(),
ost_in.ostm[0],ost_in.ostmc[0],ost_in.ostm[1],ost_in.ostmc[1],ost_in.ostm[2],ost_in.ostmc[2],ost_in.ostm[3],ost_in.ostmc[3]);

    fprintf(ffby,"\
%11d %-*s %-*.*s %15d %15.2f %15d %15.2f %15d %15.2f \
%15d %15.2f\n",
in,
iceb_tu_kolbait(40,naimos.ravno()),naimos.ravno(),
iceb_tu_kolbait(10,nomz.ravno()),iceb_tu_kolbait(10,nomz.ravno()),nomz.ravno(),
ost_in.ostm[0],ost_in.ostmcby[0],ost_in.ostm[1],ost_in.ostmcby[1],ost_in.ostm[2],ost_in.ostmcby[2],ost_in.ostm[3],ost_in.ostmcby[3]);

    fprintf(ff_nal,"\
%11d %-*.*s %-*.*s %8d %12.2f %8d %12.2f %8d %12.2f %8d %12.2f\n",
in,
iceb_tu_kolbait(30,naimos.ravno()),iceb_tu_kolbait(30,naimos.ravno()),naimos.ravno(),
iceb_tu_kolbait(10,nomz.ravno()),iceb_tu_kolbait(10,nomz.ravno()),nomz.ravno(),
ost_in.ostm[0],ost_in.ostmc[0],ost_in.ostm[1],ost_in.ostmc[1],ost_in.ostm[2],ost_in.ostmc[2],ost_in.ostm[3],ost_in.ostmc[3]);

    fprintf(ff_buh,"\
%11d %-*.*s %-*.*s %8d %12.2f %8d %12.2f %8d %12.2f %8d %12.2f\n",
in,
iceb_tu_kolbait(30,naimos.ravno()),iceb_tu_kolbait(30,naimos.ravno()),naimos.ravno(),
iceb_tu_kolbait(10,nomz.ravno()),iceb_tu_kolbait(10,nomz.ravno()),nomz.ravno(),
ost_in.ostm[0],ost_in.ostmcby[0],ost_in.ostm[1],ost_in.ostmcby[1],ost_in.ostm[2],ost_in.ostmcby[2],ost_in.ostm[3],ost_in.ostmcby[3]);
    if(iceb_tu_strlen(naimos.ravno()) > 30)
     {
      uosdvos_shet_uf(dn,mn,gn,dk,mk,gk,&sl_uz,&kolstrok_uz,pod,naipod.ravno(),mt,ff_nal,ff_buh);
      fprintf(ff_nal,"%11s %s\n","",iceb_tu_adrsimv(30,naimos.ravno()));     
      fprintf(ff_buh,"%11s %s\n","",iceb_tu_adrsimv(30,naimos.ravno()));     
     }
    k1+=ost_in.ostm[0];   kk1+=ost_in.ostm[0];   k11+=ost_in.ostm[0];
    s1+=ost_in.ostmc[0];  ss1+=ost_in.ostmc[0];  s11+=ost_in.ostmc[0];
    s1by+=ost_in.ostmcby[0];  ss1by+=ost_in.ostmcby[0];  s11by+=ost_in.ostmcby[0];

    k2+=ost_in.ostm[1];   kk2+=ost_in.ostm[1];   k21+=ost_in.ostm[1];
    s2+=ost_in.ostmc[1];  ss2+=ost_in.ostmc[1];  s21+=ost_in.ostmc[1];
    s2by+=ost_in.ostmcby[1];  ss2by+=ost_in.ostmcby[1];  s21by+=ost_in.ostmcby[1];

    k3+=ost_in.ostm[2];   kk3+=ost_in.ostm[2];   k31+=ost_in.ostm[2];
    s3+=ost_in.ostmc[2];  ss3+=ost_in.ostmc[2];  s31+=ost_in.ostmc[2];
    s3by+=ost_in.ostmcby[2];  ss3by+=ost_in.ostmcby[2];  s31by+=ost_in.ostmcby[2];

    k4+=ost_in.ostm[3];   kk4+=ost_in.ostm[3];   k41+=ost_in.ostm[3];
    s4+=ost_in.ostmc[3];  ss4+=ost_in.ostmc[3];  s41+=ost_in.ostmc[3];
    s4by+=ost_in.ostmcby[3];  ss4by+=ost_in.ostmcby[3];  s41by+=ost_in.ostmcby[3];
   }
 }


sprintf(strsql,"%d",podz);
itsh(strsql,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,ss1by,ss2by,ss3by,ss4by,mt,&kst,ff,ffby);
uosdvos_itog(strsql,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,ss1by,ss2by,ss3by,ss4by,mt,&kolstrok_uz,ff_nal,ff_buh);

itsh(shetuz.ravno(),k1,k2,k3,k4,s1,s2,s3,s4,s1by,s2by,s3by,s4by,vids,&kst,ff,ffby);
uosdvos_itog(shetuz.ravno(),k1,k2,k3,k4,s1,s2,s3,s4,s1by,s2by,s3by,s4by,vids,&kolstrok_uz,ff_nal,ff_buh);

/*Итог по организации*/
if(podrz.ravno()[0] == '\0')
 {
  itsh("0",k11,k21,k31,k41,s11,s21,s31,s41,s11by,s21by,s31by,s41by,4,&kst,ff,ffby);
  uosdvos_itog("0",k11,k21,k31,k41,s11,s21,s31,s41,s11by,s21by,s31by,s41by,4,&kolstrok_uz,ff_nal,ff_buh);
 }
else
 {
  itsh(strsql,k11,k21,k31,k41,s11,s21,s31,s41,s11by,s21by,s31by,s41by,mt,&kst,ff,ffby);
  uosdvos_itog(strsql,k11,k21,k31,k41,s11,s21,s31,s41,s11by,s21by,s31by,s41by,mt,&kolstrok_uz,ff_nal,ff_buh);
  /*Итог по органищации представляем как по подразделению*/
 }
podpis(ff);
podpis(ffby);
podpis(ff_nal);
podpis(ff_buh);
podpis(ff_prot.ff);

ff_prot.end();
fclose(ff);
fclose(ffby);
fclose(ff_nal);
fclose(ff_buh);

printw_vr(tmm);

OSTANOV();

oth->spis_imaf.plus(imaf_nal);

sprintf(strsql,"%s (%s A4)",
gettext("Оборотная ведомость движения основных средств"),
gettext("Налоговый учёт"));
oth->spis_naim.plus(strsql);

oth->spis_imaf.plus(imaf_buh);

sprintf(strsql,"%s (%s A4)",
gettext("Оборотная ведомость движения основных средств"),
gettext("Бухгалтерский учёт"));
oth->spis_naim.plus(strsql);

oth->spis_imaf.plus(imaf);

sprintf(strsql,"%s (%s A3)",
gettext("Оборотная ведомость движения основных средств"),
gettext("Налоговый учёт"));
oth->spis_naim.plus(strsql);

oth->spis_imaf.plus(imafby);

sprintf(strsql,"%s (%s A3)",
gettext("Оборотная ведомость движения основных средств"),
gettext("Бухгалтерский учёт"));
oth->spis_naim.plus(strsql);

oth->spis_imaf.plus(imaf_prot);
oth->spis_naim.plus(gettext("Протокол хода расчёта"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),0);

return(0);
}

/*******/
/*Счетчик*/
/*******/
void gsapp(short dn,short mn,short gn,
short dk,short mk,short gk,
short *sl,  //Счетчик листов
short *kst,  //Счетчик строк
int skl,const char *nai,
short mt,  //0-по подразделению 1-по материально-ответств
FILE *ff,FILE *ffby)
{

*kst=*kst+1;

if(*kst > kol_strok_na_liste)
 {
  fprintf(ff,"\f");
  fprintf(ffby,"\f");
  *sl+=1;
  *kst=0;
  shsss(dn,mn,gn,dk,mk,gk,sl,kst,skl,nai,mt,ff,ffby);
 }

}

/****************/
/*Итого по шифру для движения*/
/****************/
void            itsh(const char *kod,short k1,short k2,short k3,short k4,
double s1,double s2,double s3,double s4, //Суммы по налоговому учету
double s1by,double s2by,double s3by,double s4by, //Суммы по бух. учету
short met,  //0-по подразделению 1-по материально-ответственному 2 по счёту 3 по шифру 4 общий
short *kst,FILE *ff,FILE *ffby)
{
char		bros[512];
if(met == 4)
  sprintf(bros,gettext("Общий итог"));

if(met == 1)
  sprintf(bros,"%s %s",gettext("Итого по мат.ответсвенному"),kod);

if(met == 0)
 {
  sprintf(bros,"%s %s",gettext("Итого по подразделению"),kod);
 }

if(met == 2)
 {
  sprintf(bros,"%s %s",gettext("Итого по счёту"),kod);
 }

if(met == 3)
 {
  sprintf(bros,"%s %s",gettext("Итого по шифру"),kod);
 }

if(k1+s1+k2+s2+k3+s3+k4+s4 > 0.0001 || met == 4 || met == 1)
 {
  fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
%11s %-*s %10s %15d %15.2f %15d %15.2f %15d %15.2f \
%15d %15.2f\n"," ",iceb_tu_kolbait(40,bros),bros," ",
  k1,s1,k2,s2,k3,s3,k4,s4);
 }

if(k1+s1by+k2+s2by+k3+s3by+k4+s4by > 0.0001 || met == 4 || met == 1)
 {
  fprintf(ffby,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
%11s %-*s %10s %15d %15.2f %15d %15.2f %15d %15.2f \
%15d %15.2f\n"," ",iceb_tu_kolbait(40,bros),bros," ",
  k1,s1by,k2,s2by,k3,s3by,k4,s4by);
 }

*kst=*kst+2;

printw("%s:\n\
%-*s %10d %10.2f %10.2f\n\
%-*s %10d %10.2f %10.2f\n\
%-*s %10d %10.2f %10.2f\n\
%-*s %10d %10.2f %10.2f\n\n",
bros,
iceb_tu_kolbait(10,gettext("Остаток")),gettext("Остаток"),
k1,s1,s1by,
iceb_tu_kolbait(10,gettext("Приход")),gettext("Приход"),
k2,s2,s2by,
iceb_tu_kolbait(10,gettext("Расход")),gettext("Расход"),
k3,s3,s3by,
iceb_tu_kolbait(10,gettext("Остаток")),gettext("Остаток"),
k4,s4,s4by);
refresh();
}

/****************/
/*Итоги для узкой форми*/
/****************/
void uosdvos_itog(const char *kod,short k1,short k2,short k3,short k4,
double s1,double s2,double s3,double s4, //Суммы по налоговому учету
double s1by,double s2by,double s3by,double s4by, //Суммы по бух. учету
short met,  //0-по подразделению 1-по материально-ответственному 2 по счёту 3 по шифру 4 общий
int *kst,FILE *ff_nal,FILE *ff_buh)
{
char		bros[512];
if(met == 4)
  sprintf(bros,gettext("Общий итог"));

if(met == 1)
  sprintf(bros,"%s %s",gettext("Итого по мат.ответсвенному"),kod);

if(met == 0)
 {
  sprintf(bros,"%s %s",gettext("Итого по подразделению"),kod);
 }

if(met == 2)
 {
  sprintf(bros,"%s %s",gettext("Итого по счёту"),kod);
 }

if(met == 3)
 {
  sprintf(bros,"%s %s",gettext("Итого по шифру"),kod);
 }


if(k1+s1+k2+s2+k3+s3+k4+s4 > 0.0001 || met == 4 || met == 1)
 {
  fprintf(ff_nal,"\
----------------------------------------------------------------------------------------------------------------------------------------------\n");
  fprintf(ff_nal,"\
%11s %-*s %10s %8d %12.2f %8d %12.2f %8d %12.2f \
%8d %12.2f\n"," ",iceb_tu_kolbait(30,bros),bros," ",
  k1,s1,k2,s2,k3,s3,k4,s4);
 }

if(k1+s1by+k2+s2by+k3+s3by+k4+s4by > 0.0001 || met == 4 || met == 1)
 {
  fprintf(ff_buh,"\
----------------------------------------------------------------------------------------------------------------------------------------------\n");
  fprintf(ff_buh,"\
%11s %-*s %10s %8d %12.2f %8d %12.2f %8d %12.2f \
%8d %12.2f\n"," ",iceb_tu_kolbait(30,bros),bros," ",
  k1,s1by,k2,s2by,k3,s3by,k4,s4by);
 }

*kst+=2;

}

/*******/
/*Шапка*/
/*******/
void shsss(short dn,short mn,short gn,
short dk,short mk,short gk,
short *sl,  //Счетчик листов
short *kst, //Счетчик строк
int skl,const char *nai,
short mt,  //0-по подразделению 1-по материально-ответств
FILE *ff,FILE *ffby)
{
*kst+=KOLSTSH;
char		bros[512];

if(mt == 0)
  sprintf(bros,"%s: %d %s",gettext("Подразделение"),skl,nai);

if(mt == 1)
  sprintf(bros,"%s: %d %s",gettext("Мат.-ответственный"),skl,nai);

fprintf(ff,"\
%-*s %s%d\n\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n",
iceb_tu_kolbait(115,bros),bros,gettext("Лист N"),*sl);

fprintf(ff,gettext("\
Инвентарный|             Наименование               |          | Остаток на  %02d.%02d.%4dг.      |     Приход за период          |     Расход за период          |  Остаток на %02d.%02d.%4dг.      |\n"),
dn,mn,gn,dk,mk,gk);
fprintf(ff,gettext("\
   номер   |          основных  средств             |  Номер   |-------------------------------|------------------------------------------------------------------------------------------------\n"));
fprintf(ff,gettext("\
	   |                                        |          |   Количество  |  Сумма        |    Количество |     Сумма     |    Количество |     Сумма     |   Количество  |    Сумма      |\n"));
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ffby,"\
%-*s %s%d\n\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n",
iceb_tu_kolbait(115,bros),bros,gettext("Лист N"),*sl);

fprintf(ffby,gettext("\
Инвентарный|             Наименование               |          | Остаток на  %02d.%02d.%4dг.      |     Приход за период          |     Расход за период          |  Остаток на %02d.%02d.%4dг.      |\n"),
dn,mn,gn,dk,mk,gk);
fprintf(ffby,gettext("\
   номер   |          основных  средств             |  Номер   |-------------------------------|------------------------------------------------------------------------------------------------\n"));
fprintf(ffby,gettext("\
	   |                                        |          |   Количество  |  Сумма        |    Количество |     Сумма     |    Количество |     Сумма     |   Количество  |    Сумма      |\n"));
fprintf(ffby,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

}


/*********************************/
/*Шапка для узкой форми*/
/**********************************/
void uosdvos_suf(short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,  //Счетчик листов
int *kst, //Счетчик строк
int skl,const char *nai,
short mt,  //0-по подразделению 1-по материально-ответств
FILE *ff_nal,FILE *ff_buh)
{
//*sl+=1; //На одном и томже листе может быть выведено несколько шапок
*kst+=KOLSTSH;

char		bros[512];
memset(bros,'\0',sizeof(bros));

if(mt == 0 && skl != 0)
  sprintf(bros,"%s: %d %s",gettext("Подразделение"),skl,nai);

if(mt == 1 && skl != 0)
  sprintf(bros,"%s: %d %s",gettext("Мат.-ответственный"),skl,nai);

fprintf(ff_nal,"\
%-*s %s%d\n\
----------------------------------------------------------------------------------------------------------------------------------------------\n",
iceb_tu_kolbait(115,bros),bros,
gettext("Лист N"),
*sl);

fprintf(ff_nal,gettext("\
Инвентарный|          Наименование        |          |Остаток на %02d.%02d.%4d|   Приход за период  |  Расход за период   |Остаток на %02d.%02d.%4d|\n"),
dn,mn,gn,dk,mk,gk);
fprintf(ff_nal,gettext("\
   номер   |       основных  средств      |  Номер   |---------------------|---------------------|---------------------|----------------------\n"));
fprintf(ff_nal,gettext("\
           |                              |          |Количест|  Сумма     |Количест|    Сумма   |Количест|   Сумма    |Количест|  Сумма     |\n"));
fprintf(ff_nal,"\
----------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff_buh,"\
%-*s %s%d\n\
----------------------------------------------------------------------------------------------------------------------------------------------\n",
iceb_tu_kolbait(115,bros),bros,
gettext("Лист N"),
*sl);

fprintf(ff_buh,gettext("\
Инвентарный|          Наименование        |          |Остаток на %02d.%02d.%4d|   Приход за период  |  Расход за период   |Остаток на %02d.%02d.%4d|\n"),
dn,mn,gn,dk,mk,gk);
fprintf(ff_buh,gettext("\
   номер   |       основных  средств      |  Номер   |---------------------|---------------------|---------------------|----------------------\n"));
fprintf(ff_buh,gettext("\
	   |                              |          |Количест|  Сумма     |Количест|    Сумма   |Количест|   Сумма    |Количест|  Сумма     |\n"));
fprintf(ff_buh,"\
----------------------------------------------------------------------------------------------------------------------------------------------\n");
/*
12345678901 123456789012345678901234567890 1234567890 12345678 123456789012 12345678 123456789012 12345678 123456789012 12345678 123456789012 
*/



}
/*******/
/*Счетчик для узкой форми*/
/*******/
void  uosdvos_shet_uf(short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,  //Счетчик листов
int *kst,  //Счетчик строк
int skl,const char *nai,
short mt,  //0-по подразделению 1-по материально-ответств
FILE *ff_nal,FILE *ff_buh)
{

*kst+=1;
if(*kst <= kol_strok_na_liste)
 return;

fprintf(ff_nal,"\f");
fprintf(ff_buh,"\f");
*sl+=1;
*kst=1;
uosdvos_suf(dn,mn,gn,dk,mk,gk,sl,kst,skl,nai,mt,ff_nal,ff_buh);

}
/****************************/
/****************************/
int uosdvos_rek(int in,short dn,short mn,short gn,
int mt,
int metkasort,
const char *naimos,
const char *podrz,
const char *kodolz,
const char *hnaz,
const char *hnabyz,
const char *shetupoi,
const char *hauz,
int matotn,
int podrn,
const char *imatmptab)
{
/*Переменные данные по инвентарному номеру*/
class poiinpdw_data rekin;
char strsql[512];

poiinpd(in,mn,gn,&rekin);

if(rekin.shetu.ravno()[0] == '\0')
  rekin.shetu.new_plus("???");    

if(proverka(hnaz,rekin.hna.ravno(),0,0) != 0)
 return(1);

if(proverka(hnabyz,rekin.hnaby.ravno(),0,0) != 0)
 return(1);

if(proverka(shetupoi,rekin.shetu.ravno(),1,0) != 0)
 return(1);

if(proverka(hauz,rekin.hau.ravno(),0,0) != 0)
 return(1);

if(mt == 0)
  sprintf(strsql,"insert into %s values ('%s',%d,%d,'%s','%s','%s')",
  imatmptab,
  rekin.shetu.ravno(),podrn,in,iceb_tu_sqlfiltr(naimos),rekin.nomz.ravno(),rekin.hzt.ravno());
if(mt == 1)
  sprintf(strsql,"insert into %s values ('%s',%d,%d,'%s','%s','%s')",
  imatmptab,
  rekin.shetu.ravno(),matotn,in,iceb_tu_sqlfiltr(naimos),rekin.nomz.ravno(),rekin.hzt.ravno());

sql_zapis(strsql,1,1);    

return(0);
}

/*************************************/
/*Если вернули 0 всё впорядке
               1 не подходит
               2 ещё не пришло или уже списано
               */
int uosdvos_poi(int mt,int in,
short dn,short mn,short gn,
short dk,short mk,short gk,
int metkasort,
const char *naimos,
const char *podrz,
const char *kodolz,
const char *hnaz,
const char *hnabyz,
const char *shetupoi,
const char *hauz,
const char *imatmptab,
FILE *ffprot)
{
SQL_str row,row1;
class SQLCURSOR cur,cur1;
char strsql[512];
int kolstr=0;
int podrn=0;
int matotn=0;

/*Узнаём где находится на начало периода*/
if(poiin(in,dn,mn,gn,&podrn,&matotn) == 0)
 {
  if(proverka(podrz,podrn,0,0) == 0)
   {
    if(proverka(kodolz,matotn,0,0) == 0)
     {
      fprintf(ffprot,"%s-На начало периода %d.%d.%d\n",__FUNCTION__,dn,mn,gn);
      uosdvos_rek(in,dn,mn,gn,mt,metkasort,naimos,podrz,kodolz,hnaz,hnabyz,shetupoi,hauz,matotn,podrn,imatmptab);
     }
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
  if(proverka(podrz,row[1],0,0) != 0)
   continue;
  podrn=atoi(row[1]);
  rsdat(&d,&m,&g,row[0],2);
  /**************Читаем документ*/
  sprintf(strsql,"select kodol from Uosdok where datd='%s' and nomd='%s'",row[0],row[2]);

  //printw("%s\n",strsql);

  if(readkey(strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"Не найден документ N%s от %s !",row[2],row[0]);
    iceb_t_soob(strsql);
    continue;
   }
  else
   {
    matotn=atoi(row1[0]);
   }
  if(proverka(kodolz,matotn,0,0) != 0)
   continue;
  fprintf(ffprot,"%s-Период %d.%d.%d\n",__FUNCTION__,d,m,g);
  uosdvos_rek(in,d,m,g,mt,metkasort,naimos,podrz,kodolz,hnaz,hnabyz,shetupoi,hauz,matotn,podrn,imatmptab);
 }
return(0);
}

