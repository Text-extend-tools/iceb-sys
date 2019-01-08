/* $Id: uosvamot.c,v 5.91 2014/04/30 06:12:26 sasa Exp $ */
/*29.12.2016    12.07.1996      Белых А.И.      uosvamot.c
Получение ведомости амортизационных отчислений для налогового учёта
*/
#include        <errno.h>
#include        "buhg.h"
#include "poiinpd.h"
#include "bsizt.h"

class uosvamot_poi
 {
  public:
   class iceb_tu_str datan;
   class iceb_tu_str datak;
   class iceb_tu_str hnaz;
   class iceb_tu_str hzt;
   class iceb_tu_str podd;
   class iceb_tu_str godd;
   class iceb_tu_str invnom;
   class iceb_tu_str matot;
   class iceb_tu_str sost;
   class iceb_tu_str shetup;
   class iceb_tu_str hau;
  uosvamot_poi()
   {
    clear();
   }
  void clear()
   {
    datan.new_plus("");
    datak.new_plus("");
    hnaz.new_plus("");
    hzt.new_plus("");
    podd.new_plus("");
    godd.new_plus("");
    invnom.new_plus("");
    matot.new_plus("");
    sost.new_plus("");
    shetup.new_plus("");
    hau.new_plus("");
   }
 };
 


extern void shvam(short mn,short gn,short mk,short gk,int *sl,int *kst,int mr,FILE *ff,struct tm *bf);
extern void itamh(short mn,short gn,short mk,short gk,double itg[],const char *kod,const char *nai,short mr,double *maotph,FILE *ff,FILE *ffi,int metkaitog,int *sli,int *kst,struct tm *bf);
extern void itam(short mn,short gn,short mk,short gk,double itg[],const char *nai,short mr,double *maotp,FILE *ff,FILE *ffi,int *sli,int *kst,struct tm *bf);

extern void     itamo(short,short,double*,short,double*,FILE*,FILE*);

void shvam(short mn,short gn,short mk,short gk,int *sl,int *kst,int mr,FILE *ff,struct tm *bf);
void itamh(short mn,short gn,short mk,short gk,double itg[],char *kod,char *nai,short mr,double *maotph,FILE *ff,FILE *ffi,int metkaitog,int *sli,int *kst,struct tm *bf);
void itam(short mn,short gn,short mk,short gk,double itg[],char nai[],short mr,double *maotp,FILE *ff,FILE *ffi,int *sli,int *kst,struct tm *bf);
void uosvamot_nf(int innom,short mn,short mk,short gr,double *amort_innom,double *itogo_raoz,const char *naios,double god_proc_am,double popkf,FILE *raoz);


void            strms(short,short,short,FILE*);
void            strpod(short,short,short,FILE*);
void uosvamot_prp(class uosvamot_poi *poi,int *kol_str,FILE *ff);
int uosvamot1(class spis_oth *oth,class uosvamot_poi *poi);

extern float    nemi; /*Необлагаемый минимум*/
extern short    metkabo; //Если равна 1-бюджетная организация
extern int      kol_strok_na_liste;
extern int      kol_strok_na_liste_l;
extern class iceb_tu_str spis_print_a3;
extern short metkabo;  //Если 1 то организация бюджетная

int uosvamot(class spis_oth *oth)
{
const char *imaf_nast_gr={"uosvamot_gr.alx"};
const char *imaf_nast_hz={"uosvamot_gr.alx"};

static class uosvamot_poi poi;
struct  tm      *bf;
double          maotp[12];
double          maotph[12];
double          maotpo[12];
FILE            *ff,*ffi;
class iceb_tu_str bros("");
double          maot[12];
time_t          tmm;
int           kom,kom1;
short           d,mn,gn,mk,gk,mp,gp;
int             pod,pod1;
char            imaf[64],imafi[64];
int           sli=0,kst=0;
int           sli_bp=0,kst_bp=0;
int           slii=0,ksti=0;
int           slii_bp=0,ksti_bp=0;
int            innom,in1;
int             i;
class bsizw_data bal_st;
double          god=0,mes=0;
short           mvr=0; /*Метка выдачи результата*/
const int RAZMER_MAS=6;
double          itg[RAZMER_MAS];
double          itg1[RAZMER_MAS];
double          itg2[RAZMER_MAS];
class iceb_tu_str naimhz("");
class iceb_tu_str naipod("");
double          ostt=0.;
short           mpg; /*0-не было первой группы 1-была*/
class iceb_tu_str hna(""),hnaz1("");
SQLCURSOR 	curr;
class iceb_tu_str filnast("");
short		mr; /*Метка распечатки*/
class iceb_tu_str naios(""); /*Наименование основного средства*/
class iceb_tu_str bros1("");
class iceb_tu_str invn("");
float		kof;
short		m,g;
long		kolstr,kolstr1;
SQL_str         row,row1;
char		strsql[1024];
double		sao;
int		metka23=0; //1-по группе 2-по каждому
int		kolos=0; //Количество основных средств попавших в отчёт
float		ppkf=1.;   /*Поправочный коэффициент*/

short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка ведомости амортотчислений налогового учёта"));

VV.VVOD_SPISOK_add_data(poi.datan.ravno(),3);
VV.VVOD_SPISOK_add_data(poi.datak.ravno(),3);
if(poi.godd.getdlinna() <= 1)
  poi.godd.new_plus(gt);
VV.VVOD_SPISOK_add_data(poi.godd.ravno(),5);
VV.VVOD_SPISOK_add_data(poi.hzt.ravno(),128);
VV.VVOD_SPISOK_add_data(poi.podd.ravno(),128);
VV.VVOD_SPISOK_add_data(poi.hnaz.ravno(),128);
VV.VVOD_SPISOK_add_data(poi.invnom.ravno(),128);
VV.VVOD_SPISOK_add_data(poi.matot.ravno(),128);
VV.VVOD_SPISOK_add_data(poi.sost.ravno(),128);
VV.VVOD_SPISOK_add_data(poi.shetup.ravno(),128);
VV.VVOD_SPISOK_add_data(poi.hau.ravno(),128);

VV.VVOD_SPISOK_add_MD(gettext("Месяц начала.......(м)..."));//0
VV.VVOD_SPISOK_add_MD(gettext("Месяц конца........(м)..."));//1
VV.VVOD_SPISOK_add_MD(gettext("Год......................"));//2
VV.VVOD_SPISOK_add_MD(gettext("Шифр затрат.........(,,)."));//3
VV.VVOD_SPISOK_add_MD(gettext("Подразделение.......(,,)."));//4
VV.VVOD_SPISOK_add_MD(gettext("Группа..............(,,)."));//5
VV.VVOD_SPISOK_add_MD(gettext("Инв-ный номер.......(,,)."));//6
VV.VVOD_SPISOK_add_MD(gettext("Мат.отве-нный.......(,,)."));//7
VV.VVOD_SPISOK_add_MD(gettext("Состояние объекта.(+/-).."));//8
VV.VVOD_SPISOK_add_MD(gettext("Счёт учёта.........(,,).."));//9
VV.VVOD_SPISOK_add_MD(gettext("Шифр анал.учёта....(,,).."));//10

naz:;

clear();

helstr(LINES-1,0,"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F5",gettext("настройки"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

poi.datan.new_plus(VV.data_ravno(0));
poi.datak.new_plus(VV.data_ravno(1));
poi.godd.new_plus(VV.data_ravno(2));
poi.hzt.new_plus(VV.data_ravno(3));

poi.podd.new_plus(VV.data_ravno(4));

poi.hnaz.new_plus(VV.data_ravno(5));
poi.invnom.new_plus(VV.data_ravno(6));
poi.matot.new_plus(VV.data_ravno(7)); 
poi.sost.new_plus(VV.data_ravno(8));
poi.shetup.new_plus(VV.data_ravno(9));
poi.hau.new_plus(VV.data_ravno(10));
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
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка шифров затрат"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр материально-ответственных"));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка шифров аналитического учёта"));//4
    MENU.VVOD_SPISOK_add_MD(gettext("Выбор набора групп"));//5
    MENU.VVOD_SPISOK_add_MD(gettext("Выбор набора шифров затрат"));//6
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//7

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 7:
      case -1:
       goto naz;

      case 0 :
        vibrek("Uosgrup",&poi.hnaz,&naim_v);
        VV.VVOD_SPISOK_zapis_data(5,poi.hnaz.ravno());
        goto naz;

      case 1 :
        if(diruospod(1,&kod_v,&naim_v,0) == 0)
         VV.data_z_plus(4,kod_v.ravno());
        goto naz;

      case 2 :
        vibrek("Uoshz",&poi.hzt,&naim_v);
        VV.data_z_plus(3,poi.hzt.ravno());
        goto naz;

      case 3 :
        if(dirmatot(1,&kod_v,&naim_v,0) == 0)
         VV.data_z_plus(7,kod_v.ravno());
        goto naz;

      case 4 :
        vibrek("Uoshau",&kod_v,&naim_v);
        VV.data_z_plus(10,kod_v.ravno());
        goto naz;

      case 5 :
        if(iceb_t_nabor(&naim_v,imaf_nast_gr) == 0)
          VV.data_z_plus(5,naim_v.ravno());
        goto naz;
      case 6 :
        if(iceb_t_nabor(&naim_v,imaf_nast_hz) == 0)
          VV.data_z_plus(3,naim_v.ravno());
        goto naz;

     }
    goto naz;
    

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();
    goto naz;

  case FK5: /*Настройки*/
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Настройка наборов групп"));
    MENU.VVOD_SPISOK_add_MD(gettext("Настройка наборов шифров затрат"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 2 :
      case -1:
       goto naz;

      case 0 :
        iceb_redfil(imaf_nast_gr,0);
        goto naz;

      case 1 :
        iceb_redfil(imaf_nast_hz,0);
        goto naz;


     }
    goto naz;
    

  default:
    goto naz;
 }


//sprintf(bros,"%s.%s",VV.VVOD_SPISOK_return_data(0),VV.VVOD_SPISOK_return_data(2));
bros.new_plus(VV.VVOD_SPISOK_return_data(0));
bros.plus(".",VV.VVOD_SPISOK_return_data(2));
if(rsdat1(&mn,&gn,bros.ravno()) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата начала !"));
  goto naz;
 }

if(VV.VVOD_SPISOK_return_data(1)[0] != '\0')
 {
  //sprintf(bros,"%s.%s",VV.VVOD_SPISOK_return_data(1),VV.VVOD_SPISOK_return_data(2));
  bros.new_plus(VV.VVOD_SPISOK_return_data(1));
  bros.plus(".",VV.VVOD_SPISOK_return_data(2));
  if(rsdat1(&mk,&gk,bros.ravno()) != 0)
   {
    iceb_t_soob(gettext("Не верно введена дата конца !"));
    goto naz;
   }
 }
else
 {
  mk=mn; gk=gn;
 }

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
if(sravmydat(1,mn,gn,1,4,2011) >= 0) //В целом по группе было только до этой даты
 {
  return(uosvamot1(oth,&poi));
 }

time(&tmm);
bf=localtime(&tmm);

sprintf(strsql,"select ta from Uosgrup where ta=1 limit 1");

if(readkey(strsql) >= 1)
  metka23=1; /*Есть группы с амортизацией в целом по группе*/
else
 metka23=2; /*Все группы с пообъектной амортизацией*/
 

clear();

GDITE();



int kol_mes_v_per=period(1,mn,gn,1,mk,gk,1);

if(kol_mes_v_per <=3)
 mr=1;
else
 mr=0;


d=1;
mp=mn; gp=gn;
dpm(&d,&mp,&gp,2);


printw("%s %d.%d%s %s %d.%d%s (%d.%d)\n",
gettext("Период с"),
mn,gn,
gettext("г."),
gettext("до"),
mk,gk,
gettext("г."),
mp,gp);
refresh();

//sprintf(strsql,"select innom,mes,god,podr,hzt,hna,suma from Uosamor where god = %d and mes >= %d and mes <= %d order by hna,podr,innom,god,mes asc",gn,mn,mk);
sprintf(strsql,"select innom,da,podr,hzt,hna,suma from Uosamor where da >= '%04d-%02d-01' and '%04d-%02d-31' order by hna,podr,innom,da asc",gn,mn,gk,mk);
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
short pdm=0;
dpm(&pdm,&mk,&gk,5);
FILE *ff_prot=NULL;
char imaf_prot[56];

sprintf(imaf_prot,"uospra%d.lst",getpid());
if((ff_prot = fopen(imaf_prot,"w")) == NULL)
 {
  error_op_nfil(imaf_prot,errno,"");
  return(1);
 }
/*****************
int orient=1;
if(proverka(spis_print_a3.ravno(),getenv("PRINTER"),0,1) == 0)
  orient=0; //Если принтер А4 то ориентация портрет
*****************/
zagolov(gettext("Ведомость амортизационных отчислений"),1,mn,gn,pdm,mk,gk,iceb_t_get_pnk("00",0),ff_prot);

sprintf(imaf,"vao%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }


zagolov(gettext("Ведомость амортизационных отчислений"),1,mn,gn,pdm,mk,gk,iceb_t_get_pnk("00",0),ff);

fprintf(ff,"%s\n",gettext("Налоговый учёт"));
kst=6;

uosvamot_prp(&poi,&kst,ff);

sprintf(imafi,"vaoi%d.lst",getpid());

if((ffi = fopen(imafi,"w")) == NULL)
 {
  error_op_nfil(imafi,errno,"");
  return(1);
 }


zagolov(gettext("Ведомость амортизационных отчислений"),1,mn,gn,pdm,mk,gk,iceb_t_get_pnk("00",0),ffi);

fprintf(ffi,"%s\n",gettext("Налоговый учёт"));

int kkkk=0;
uosvamot_prp(&poi,&kkkk,ffi);
uosshap(mn,gn,mk,gk,bf,&slii,mr,ffi,&ksti);


/*****************отчёты без подразделений*******************/

FILE *ff_bp;
char imaf_bp[64];

sprintf(imaf_bp,"vaobp%d.lst",getpid());
if((ff_bp = fopen(imaf_bp,"w")) == NULL)
   {
    error_op_nfil(imaf_bp,errno,"");
    return(1);
   }


zagolov(gettext("Ведомость амортизационных отчислений"),1,mn,gn,pdm,mk,gk,iceb_t_get_pnk("00",0),ff_bp);

fprintf(ff_bp,"%s\n",gettext("Налоговый учёт"));
kst_bp=6;

uosvamot_prp(&poi,&kst_bp,ff_bp);



FILE *ffi_bp;
char imafi_bp[64];
sprintf(imafi_bp,"vaoibp%d.lst",getpid());

if((ffi_bp = fopen(imafi_bp,"w")) == NULL)
 {
  error_op_nfil(imafi_bp,errno,"");
  return(1);
 }

zagolov(gettext("Ведомость амортизационных отчислений"),1,mn,gn,pdm,mk,gk,iceb_t_get_pnk("00",0),ffi_bp);

fprintf(ffi_bp,"%s\n",gettext("Налоговый учёт"));

kkkk=0;
uosvamot_prp(&poi,&kkkk,ffi_bp);
uosshap(mn,gn,mk,gk,bf,&slii_bp,mr,ffi_bp,&ksti_bp);


innom=0;
for(i=0; i< 12; i++)
    maot[i]=maotp[i]=maotph[i]=maotpo[i]=0.;
mpg=0;

for(i=0;i<RAZMER_MAS;i++)
  itg[i]=itg1[i]=itg2[i]=0.;
naipod.new_plus(iceb_t_get_pnk("00",0));
pod1=0;
kolstr1=in1=mvr=0;
class poiinpdw_data rekin;
filnast.new_plus("uosnast.alx");

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(proverka(poi.invnom.ravno(),row[0],0,0) != 0)
    continue;


  if(proverka(poi.hzt.ravno(),row[3],0,0) != 0)
    continue;

  if(proverka(poi.podd.ravno(),row[2],0,0) != 0)
    continue;

  if(proverka(poi.hnaz.ravno(),row[4],0,0) != 0)
    continue;

  innom=atol(row[0]);

  if(metka23 == 1)  /*есть группы с амортизацией в целом по группе*/
   if(uosprovgr(row[4]) == 1) /*амортизация в целом по группе*/
     if(innom > 0)
      continue;

  if(metka23 == 2 ) /*все группы с пообъектной амортизацией*/
    if(uosprovgr(row[4]) == 0) /*амортизация в группе пообъектная*/
     if(innom < 0)
      continue;
     
  if(poi.matot.getdlinna() > 1)
   {
    int pdd=0;
    int kol=0;
    poiin(innom,1,mn,gn,&pdd,&kol);
    if(proverka(poi.matot.ravno(),kol,0,0) != 0)
      continue;
   }

  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4],row[5]);
   
  rsdat(&d,&m,&g,row[1],2);
  
  
 
//  m=atoi(row[1]);
//  g=atoi(row[2]);
  pod=atoi(row[2]);
  hna.new_plus(row[4]);
  sao=atof(row[5]);

  if(mpg == 0)  
   if(SRAV(hna.ravno(),"1",0) == 0)
    mpg=1;

  if(SRAV(hnaz1.ravno(),hna.ravno(),0) != 0)
   {

    if(hnaz1.ravno()[0] != '\0')
     {
      if(mvr > 0)
       {
        rasmes(mn,mk,maot,ostt,hnaz1.ravno(),ff);
        rasmes(mn,mk,maot,ostt,hnaz1.ravno(),ff_bp);
     
        if(uosprovgr(hnaz1.ravno()) == 0)
         {
          itam(mn,gn,mk,gk,itg1,naipod.ravno(),mr,maotp,ff,ffi,&sli,&kst,bf);
          pod1=0;
         }
        mvr=0;
       }
      itamh(mn,gn,mk,gk,itg2,hnaz1.ravno(),naimhz.ravno(),mr,maotph,ff,ffi,0,&sli,&kst,bf);
      itamh(mn,gn,mk,gk,itg2,hnaz1.ravno(),naimhz.ravno(),mr,maotph,ff_bp,ffi_bp,0,&sli_bp,&kst_bp,bf);
      for(i=0;i<RAZMER_MAS;i++)
	itg1[i]=itg2[i]=0.;

      fprintf(ff,"\f");
      fprintf(ff_bp,"\f");
      kst=0;
     }

    /*Читаем группу налогового учёта*/
    sprintf(strsql,"select naik from Uosgrup where kod='%s'",hna.ravno());
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      beep();
      printw("%s %s !\n",gettext("Не найдена группа"),hna.ravno());
      OSTANOV();
      naimhz.new_plus("");
     }
    else
     naimhz.new_plus(row1[0]);
          
    printw("%s: %s %s\n",gettext("Группа"),hna.ravno(),naimhz.ravno());

    shvam(mn,gn,mk,gk,&sli,&kst,mr,ff,bf);
    shvam(mn,gn,mk,gk,&sli_bp,&kst_bp,mr,ff_bp,bf);

    fprintf(ff,"%s: %s %s\n",gettext("Группа"),hna.ravno(),naimhz.ravno());
    fprintf(ff_bp,"%s: %s %s\n",gettext("Группа"),hna.ravno(),naimhz.ravno());

    uosshap(mn,gn,mk,gk,bf,&sli,mr,ff,&kst);
    uosshap(mn,gn,mk,gk,bf,&sli_bp,mr,ff_bp,&kst_bp);

    for(i=0; i< 12; i++)
     maotp[i]=maotph[i]=0.;

    hnaz1.new_plus(hna.ravno());
   }

  if(pod1 != pod)
   {
    if(pod1 != 0 && mvr > 0)
     {
        rasmes(mn,mk,maot,ostt,hnaz1.ravno(),ff);
        rasmes(mn,mk,maot,ostt,hnaz1.ravno(),ff_bp);
        mvr=0;
      if(uosprovgr(hnaz1.ravno()) == 0)
       {
         itam(mn,gn,mk,gk,itg1,naipod.ravno(),mr,maotp,ff,ffi,&sli,&kst,bf);
       }
     }
    for(i=0;i< RAZMER_MAS;i++)
      itg1[i]=0.;
    /*Читаем наименование подразделения*/
    if(innom > 0)
     {
      sprintf(strsql,"select naik from Uospod where kod=%d",pod);
      if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
       {
        beep();
        printw("%s %d !\n",gettext("Не найден код подразделения"),pod);
        OSTANOV();
        naipod.new_plus("");
       }
      else
       naipod.new_plus(row1[0]);
     }
    else
      naipod.new_plus(iceb_t_get_pnk("00",0));

    printw("%s %d %s\n",gettext("Подразделение"),pod,naipod.ravno());

    shvam(mn,gn,mk,gk,&sli,&kst,mr,ff,bf);
    fprintf(ff,"%s %d %s\n",gettext("Подразделение"),pod,naipod.ravno());

    for(i=0; i< 12; i++)
      maotp[i]=0.;

    pod1=pod;
   }

    if(innom > 0)
     {
      /*Узнаем поправочный коэффициент счёт учёта состояние объекта в расчётном месяце*/
      if(poiinpd(innom,m,g,&rekin) != 0)
       {

        shvam(mn,gn,mk,gk,&sli,&kst,mr,ff,bf);
        fprintf(ff,"%d инвентарный номер не числится %d.%dг. !!!\n",innom,m,g);

        shvam(mn,gn,mk,gk,&sli_bp,&kst_bp,mr,ff_bp,bf);
        fprintf(ff_bp,"%d инвентарный номер не числится %d.%dг. !!!\n",innom,m,g);

        sprintf(strsql,"%d инвентарный номер не числится %d.%dг. !!!",innom,m,g);
        iceb_t_soob(strsql);
       }
      ppkf=rekin.popkf;
      if(proverka(poi.shetup.ravno(),rekin.shetu.ravno(),0,0) != 0)
       {
        if(ff_prot != NULL)
         fprintf(ff_prot,"не подходит счёт учёта %s != %s\n",poi.shetup.ravno(),rekin.shetu.ravno());
        continue;
       }
      
      if(poi.sost.ravno_pr() == 1)
       if(rekin.soso == 1 || rekin.soso == 2)
        {
         if(ff_prot != NULL)
          fprintf(ff_prot,"не подходит состояние объекта! +");
      
         continue;
        }
      if(poi.sost.ravno_pr() == 2)
       if(rekin.soso == 0 || rekin.soso == 3)
        {
         if(ff_prot != NULL)
          fprintf(ff_prot,"не подходит состояние объекта! -");

         continue;
        }         
      printw("%s %s\n",poi.hau.ravno(),rekin.hau.ravno());
      refresh();      
      if(proverka(poi.hau.ravno(),rekin.hau.ravno(),0,0) != 0)
       continue;
     }


  if(in1 != innom)
   {
    if(mvr > 0 && in1 != 0)
     {
      rasmes(mn,mk,maot,ostt,hnaz1.ravno(),ff);
      rasmes(mn,mk,maot,ostt,hnaz1.ravno(),ff_bp);
      mvr=0;
     }
    in1=innom;

    for(i=0; i< 12; i++)
     maot[i]=0.;


    if(innom > 0)
     {

      /*Читаем наименование объекта*/
      sprintf(strsql,"select naim from Uosin where innom=%d",innom);
      if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
       {
        sprintf(strsql,"%s %d !",gettext("Не найден инвентарный номер"),innom);
        iceb_t_soob(strsql);
        naios.new_plus("");
       }
      else
       naios.new_plus(row1[0]);

     }
    else
     {
      //sprintf(bros,"Поправочный коэффициент для группы %d",innom*-1);
      bros.new_plus("Поправочный коэффициент для группы ");
      bros.plus(innom*-1);
      if(iceb_t_poldan(bros.ravno(),&bros1,filnast.ravno()) != 0)
       {
        VVOD SOOB(1);
        sprintf(strsql,"Не найден %s !!!",bros.ravno());
        SOOB.VVOD_SPISOK_add_MD(strsql);
        sprintf(strsql,"%s %s",gettext("В файле"),filnast.ravno());
        SOOB.VVOD_SPISOK_add_MD(strsql);
        soobshw(&SOOB,stdscr,-1,-1,0,1);
       }
      ppkf=bros1.ravno_atof(); 
      if(ff_prot != NULL)
        fprintf(ff_prot,"Поправочный коэффициент для группы %d=%f",innom*-1,ppkf);
     } 

    /*Читаем коэффициент*/

    if(innom < 0)
     rekin.hna.new_plus(innom*(-1));


    sprintf(strsql,"select naik,kof from Uosgrup where kod='%s'",rekin.hna.ravno());

    kof=0;
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      printw("%s %s !\n",gettext("Не найдено группу"),rekin.hna.ravno());
      OSTANOV();
      continue;
     }
    else
     {
      kof=atof(row1[1]);
      if(innom < 0)
       naios.new_plus(row1[0]);
     }

    /*На начало периода*/
    //bs=iz=bs1=iz1=0.;
    bal_st.clear();    
    if(innom > 0)
     {
      bsiz(innom,pod1,1,mn,gn,&bal_st,NULL);
      
      bal_st.bs+=bal_st.sbs;
      bal_st.iz+=bal_st.iz1+bal_st.siz;
     }
    if(innom < 0)
     {
      //sprintf(bros,"Стартовая балансовая стоимость для группы OH%d",innom*-1);
      bros.new_plus("Стартовая балансовая стоимость для группы ");
      bros.plus(innom*-1);
      
      if(iceb_t_poldan(bros.ravno(),&bros1,filnast.ravno()) != 0)
         {
          VVOD SOOB(1);
          sprintf(strsql,"Не найдена %s",bros.ravno());
          SOOB.VVOD_SPISOK_add_MD(strsql);
          SOOB.VVOD_SPISOK_add_MD(filnast.ravno());
          soobshw(&SOOB,stdscr,-1,-1,0,1);
         }
      double startbs=bros1.ravno_atof();
      if(ff_prot != NULL)
        fprintf(ff_prot,"Стартовая балансовая стоимость для группы %d=%.2f",innom*-1,bal_st.bs);

//      sprintf(bros,"Стартовый износ для группы %d",innom*-1);
      bros.new_plus("Стартовый износ для группы ");
      bros.plus(innom*-1);
      if(iceb_t_poldan(bros.ravno(),&bros1,filnast.ravno()) != 0)
         {
          sprintf(strsql,"Не найден %s",bros.ravno());
          iceb_t_soob(strsql);
         }
       double startiz=bros1.ravno_atof();
       printw("Группа 2 bs=%f iz=%f\n",bal_st.bs,bal_st.iz);
       if(ff_prot != NULL)
         fprintf(ff_prot,"Стартовый износ для группы %d=%.2f",innom*-1,bal_st.iz);

      bsiz23(rekin.hna.ravno(),1,mn,gn,&bal_st.bs,&bal_st.iz,&bal_st.iz1,0);
      bal_st.iz+=bal_st.iz1+startiz;
      bal_st.bs+=startbs;
     }
    double lik_st=uosgetls(innom,1,mn,gn,0);

    if(uosprovar(rekin.hna.ravno(),0) == 1)
     {
      ostt=bal_st.bs-bal_st.iz;
      if(metkabo == 0)
       god=(bal_st.bs-bal_st.iz)*kof/100.;
      if(metkabo == 1)
        god=(bal_st.bs-lik_st)*kof/100.;
     }
    else
     {
      ostt=bal_st.bs-bal_st.iz;
      god=(bal_st.bs-lik_st)*kof/100.;
     }
  
    mes=0.;
    if(god != 0.)
      mes=god/12.;
    itg[0]+=bal_st.bs;
    itg[1]+=bal_st.iz;
    itg[2]+=ostt;
    itg[3]+=lik_st;  
    itg[4]+=god;
    itg[5]+=mes;

    itg1[0]+=bal_st.bs;
    itg1[1]+=bal_st.iz;
    itg1[2]+=ostt;
    itg1[3]+=lik_st;
    itg1[4]+=god;
    itg1[5]+=mes;

    itg2[0]+=bal_st.bs;
    itg2[1]+=bal_st.iz;
    itg2[2]+=ostt;
    itg2[3]+=lik_st;
    itg2[4]+=god;
    itg2[5]+=mes;

    if(innom > 0)
     invn.new_plus(innom);
    else
     {
//      sprintf(invn,"%d %s",innom*(-1),gettext("группа"));
      invn.new_plus(innom*-1);
      invn.plus(" ",gettext("группа"));
     }     

    shvam(mn,gn,mk,gk,&sli,&kst,mr,ff,bf);
    shvam(mn,gn,mk,gk,&sli_bp,&kst_bp,mr,ff_bp,bf);

    fprintf(ff,"%-8s|%-*.*s|%-*.*s|%-*s|",
    invn.ravno(),
    iceb_tu_kolbait(40,naios.ravno()),iceb_tu_kolbait(40,naios.ravno()),naios.ravno(),
    iceb_tu_kolbait(10,rekin.nomz.ravno()),iceb_tu_kolbait(10,rekin.nomz.ravno()),rekin.nomz.ravno(),
    iceb_tu_kolbait(5,rekin.hna.ravno()),rekin.hna.ravno());
  
    fprintf(ff_bp,"%-8s|%-*.*s|%-*.*s|%-*s|",
    invn.ravno(),
    iceb_tu_kolbait(40,naios.ravno()),iceb_tu_kolbait(40,naios.ravno()),naios.ravno(),
    iceb_tu_kolbait(10,rekin.nomz.ravno()),iceb_tu_kolbait(10,rekin.nomz.ravno()),rekin.nomz.ravno(),
    iceb_tu_kolbait(5,rekin.hna.ravno()),rekin.hna.ravno());
  
    if(mr == 1)
     {
      fprintf(ff,"%5.2f|%10.2f|%10.2f|%10.2f|%10.2f|%12.2f|%10.2f|%5.2f|",kof,bal_st.bs,bal_st.iz,ostt,lik_st,god,mes,ppkf);
      fprintf(ff_bp,"%5.2f|%10.2f|%10.2f|%10.2f|%10.2f|%12.2f|%10.2f|%5.2f|",kof,bal_st.bs,bal_st.iz,ostt,lik_st,god,mes,ppkf);
     }
    kolos++;

    mvr++;

/*
    printw("*************mr=%d\n",mr);
    OSTANOV();
*/    
   }

  maot[m-1]=sao;
  maotp[m-1]=maotp[m-1]+sao;
  maotph[m-1]=maotph[m-1]+sao;
  maotpo[m-1]=maotpo[m-1]+sao;

 }

rasmes(mn,mk,maot,ostt,hnaz1.ravno(),ff);
rasmes(mn,mk,maot,ostt,hnaz1.ravno(),ff_bp);

if(uosprovgr(hnaz1.ravno()) == 0)
   itam(mn,gn,mk,gk,itg1,naipod.ravno(),mr,maotp,ff,ffi,&sli,&kst,bf);

itamh(mn,gn,mk,gk,itg2,hnaz1.ravno(),naimhz.ravno(),mr,maotph,ff,ffi,0,&sli,&kst,bf);
itamo(mn,mk,itg,mr,maotpo,ff,ffi);

itamh(mn,gn,mk,gk,itg2,hnaz1.ravno(),naimhz.ravno(),mr,maotph,ff_bp,ffi_bp,0,&sli_bp,&kst_bp,bf);
itamo(mn,mk,itg,mr,maotpo,ff_bp,ffi_bp);

if(mpg == 1)
 {
  fprintf(ff,gettext("* Остаточная стоимость меньше (равняется) 100 необлагаемых минимумов.\n"));
  fprintf(ff_bp,gettext("* Остаточная стоимость меньше (равняется) 100 необлагаемых минимумов.\n"));
 }

printw("\n%s:%d\n",gettext("Количество основных средств"),kolos);

fprintf(ff,"\n%s:%d\n",gettext("Количество основных средств"),kolos);
fprintf(ffi,"\n%s:%d\n",gettext("Количество основных средств"),kolos);
fprintf(ff_bp,"\n%s:%d\n",gettext("Количество основных средств"),kolos);
fprintf(ffi_bp,"\n%s:%d\n",gettext("Количество основных средств"),kolos);

podpis(ff);
fclose(ff);

podpis(ffi);
fclose(ffi);

podpis(ff_bp);
fclose(ff_bp);

podpis(ffi_bp);
fclose(ffi_bp);
if(ff_prot != NULL)
 {
  podpis(ff_prot);
  fclose(ff_prot);
 }
printw_vr(tmm);

OSTANOV();


oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Распечатка ведомости амортотчислений"));
oth->spis_imaf.plus(imaf_bp);
oth->spis_naim.plus(gettext("Ведомоть амортотчислений без распределения по подразделениям"));
oth->spis_imaf.plus(imafi);
oth->spis_naim.plus(gettext("Общие итоги"));
oth->spis_imaf.plus(imafi_bp);
oth->spis_naim.plus(gettext("Общие итоги без распределения по подразделениям"));
if(ff_prot != NULL)
 {
  oth->spis_imaf.plus(imaf_prot);
  oth->spis_naim.plus("Протокол хода расчёта");
 }

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);
 
return(0);

}
/*****************/
/*Шапка документа*/
/*****************/
void shvam(short mn,short gn,short mk,short gk,int *sl,
int *kst,int mr,FILE *ff,struct tm *bf)
{

*kst+=1;
if(*kst <= kol_strok_na_liste)
    return;
    
fprintf(ff,"\f");

*kst=1;

uosshap(mn,gn,mk,gk,bf,sl,mr,ff,kst);

}
/**********************/
/*строка подчеркивания*/
/**********************/

void strpod(short mn,short mk,short dlu,FILE *ff)
{
short           i;
class iceb_tu_str bros("");

for(i=0; i< dlu ; i++)
 bros.plus("-");
for(i=mn-1; i<mk ;i++)
  fprintf(ff,"%s",bros.ravno());
if(mn != mk)
  fprintf(ff,"%s",bros.ravno());
fprintf(ff,"\n");
}

/****************/
/*Строка месяцев*/
/****************/
void strms(short mt, //0-с номером месяца 1 - без
short mn,short mk,FILE *ff)
{
short           i;

if(mt == 0)
 {
  for(i=mn-1; i< mk ; i++)
    fprintf(ff,"    %-6d|",i+1);

  if(mn != mk)
   fprintf(ff,"%-*s|",iceb_tu_kolbait(10,gettext("Итого")),gettext("Итого"));
 }
if(mt == 1)
 {
  for(i=mn-1; i< mk ; i++)
   fprintf(ff,"%10s|"," ");

  if(mn != mk)
    fprintf(ff,"%10s|"," ");
 }
fprintf(ff,"\n");
}

/*******************************/
/*Выдача итога по подразделению*/
/*******************************/
void itam(short mn,short gn,short mk,short gk,double itg[],const char *nai,short mr,
double *maotp,FILE *ff,FILE *ffi,int *sli,int *kst,
struct tm *bf)
{
short           i;
double          vs;
char		bros[512];

shvam(mn,gn,mk,gk,sli,kst,mr,ff,bf);
fprintf(ff,"\
-------------------------------------------------------------------");
fprintf(ffi,"\
-------------------------------------------------------------------");
if(mr == 1)
 {
  fprintf(ff,"\
--------------------------------------------------------------------------------");
  fprintf(ffi,"\
--------------------------------------------------------------------------------");
 }

strpod(mn,mk,11,ff);
strpod(mn,mk,11,ffi);

memset(bros,'\0',sizeof(bros));
sprintf(bros,"%s %s",gettext("Итого по подразделению"),nai);

shvam(mn,gn,mk,gk,sli,kst,mr,ff,bf);

fprintf(ff,"%-*.*s|",iceb_tu_kolbait(66,bros),iceb_tu_kolbait(66,bros),bros);

fprintf(ffi,"%-*.*s|",iceb_tu_kolbait(66,bros),iceb_tu_kolbait(66,bros),bros);

if(mr == 1)
 {
  fprintf(ff,"%5s|%10.2f|%10.2f|%10.2f|%10.2f|%12.2f|%10.2f|%5s|",
" ",itg[0],itg[1],itg[2],itg[3],itg[4],itg[5]," ");
  fprintf(ffi,"%5s|%10.2f|%10.2f|%10.2f|%10.2f|%12.2f|%10.2f|%5s|",
" ",itg[0],itg[1],itg[2],itg[3],itg[4],itg[5]," ");
 }
vs=0.;
for(i=mn-1 ;i<mk ;i++)
 {
  vs+=maotp[i];
  fprintf(ff,"%10.2f|",maotp[i]);
  fprintf(ffi,"%10.2f|",maotp[i]);
 }
if(mn != mk)
 {
  fprintf(ff,"%10.2f|",vs);
  fprintf(ffi,"%10.2f|",vs);
  printw("%s: %.2f\n",gettext("Итого по подразделению"),vs);
 }
else
  printw("%s: %.2f\n",gettext("Итого по подразделению"),maotp[mn-1]);

fprintf(ff,"\n");
fprintf(ffi,"\n");

}
/*******************************/
/*Выдача итога по шифру нормы амортотчислений или группе*/
/*******************************/
void itamh(short mn,short gn,short mk,short gk,double itg[],
const char *kod,
const char *nai,
short mr,
double *maotph,FILE *ff,FILE *ffi,
int metkaitog,  //0-по группе 1-по шифру
int *sli,int *kst,
struct tm *bf)
{
short           i;
double          vs;
char		bros[512];

shvam(mn,gn,mk,gk,sli,kst,mr,ff,bf);
fprintf(ff,"\
-------------------------------------------------------------------");
fprintf(ffi,"\
-------------------------------------------------------------------");

if(mr == 1)
 {
  fprintf(ff,"\
--------------------------------------------------------------------------------");
  fprintf(ffi,"\
--------------------------------------------------------------------------------");
 }
strpod(mn,mk,11,ff);
strpod(mn,mk,11,ffi);
if(metkaitog == 0)
  sprintf(bros,"%s %s %s",gettext("Итого по группе"),kod,nai);
if(metkaitog == 1)
  sprintf(bros,"%s %s %s",gettext("Итого по шифру"),kod,nai);

shvam(mn,gn,mk,gk,sli,kst,mr,ff,bf);

fprintf(ff,"%-*.*s|",iceb_tu_kolbait(66,bros),iceb_tu_kolbait(66,bros),bros);

fprintf(ffi,"%-*.*s|",iceb_tu_kolbait(66,bros),iceb_tu_kolbait(66,bros),bros);


if(mr == 1)
 {
  fprintf(ff,"%5s|%10.2f|%10.2f|%10.2f|%10.2f|%12.2f|%10.2f|%5s|",
" ",itg[0],itg[1],itg[2],itg[3],itg[4],itg[5]," ");
  fprintf(ffi,"%5s|%10.2f|%10.2f|%10.2f|%10.2f|%12.2f|%10.2f|%5s|",
" ",itg[0],itg[1],itg[2],itg[3],itg[4],itg[5]," ");
 }
vs=0.;
for(i=mn-1 ;i<mk ;i++)
 {
  vs+=maotph[i];
  fprintf(ff,"%10.2f|",maotph[i]);
  fprintf(ffi,"%10.2f|",maotph[i]);
 }

if(mn != mk)
 {
  fprintf(ff,"%10.2f|",vs);
  fprintf(ffi,"%10.2f|",vs);
  printw("%s: %.2f\n",bros,vs);
 }
else
  printw("%s: %.2f\n",bros,maotph[mn-1]);

fprintf(ff,"\n");
fprintf(ffi,"\n");

}
/*******************************/
/*Выдача итога по организации*/
/*******************************/
void itamo(short mn,short mk,double itg[],short mr,
double *maotpo,FILE *ff,FILE *ffi)
{
short           i;
double          vs;
char		bros[512];

fprintf(ff,"\
-------------------------------------------------------------------");
fprintf(ffi,"\
-------------------------------------------------------------------");
if(mr == 1)
 {
  fprintf(ff,"\
--------------------------------------------------------------------------------");
  fprintf(ffi,"\
--------------------------------------------------------------------------------");
 }
strpod(mn,mk,11,ff);
strpod(mn,mk,11,ffi);

sprintf(bros,gettext("Общий итог"));


fprintf(ff,"%-*.*s|",iceb_tu_kolbait(66,bros),iceb_tu_kolbait(66,bros),bros);

fprintf(ffi,"%-*.*s|",iceb_tu_kolbait(66,bros),iceb_tu_kolbait(66,bros),bros);

if(mr == 1)
 {
  fprintf(ff,"%5s|%10.2f|%10.2f|%10.2f|%10.2f|%12.2f|%10.2f|%5s|",
" ",itg[0],itg[1],itg[2],itg[3],itg[4],itg[5]," ");
  fprintf(ffi,"%5s|%10.2f|%10.2f|%10.2f|%10.2f|%12.2f|%10.2f|%5s|",
" ",itg[0],itg[1],itg[2],itg[3],itg[4],itg[5]," ");
 }
vs=0.;
for(i=mn-1 ;i<mk ;i++)
 {
  vs+=maotpo[i];
  fprintf(ff,"%10.2f|",maotpo[i]);
  fprintf(ffi,"%10.2f|",maotpo[i]);
 }

if(mn != mk)
 {
  fprintf(ff,"%10.2f|",vs);
  fprintf(ffi,"%10.2f|",vs);
  printw("%s: %.2f\n",bros,vs);
 }
else
  printw("%s: %.2f\n",bros,maotpo[mn-1]);

fprintf(ff,"\n");
fprintf(ffi,"\n");

}

/***********************/
/*Выдача строки месяцев*/
/***********************/
void rasmes(short mn,short mk,double *maot,double ostt,
const char *hnaz,FILE *ff)
{
short           i;
double          vs;
vs=0.;

for(i=mn-1 ;i<mk ;i++)
 {
  vs+=maot[i];
  fprintf(ff,"%10.2f|",maot[i]);
 }
if(mn != mk)
   fprintf(ff,"%10.2f|",vs);
if(ostt <= 100*nemi && ostt > 0.001 && SRAV(hnaz,"1",0) == 0)
   fprintf(ff,"*");
fprintf(ff,"\n");
}

/*************************************************/
/*Шапка*/
/*************************************************/

void uosshap(short mn,short gn,short mk,short gk,struct tm *bf,
int *sl,short mr,FILE *ffi,int *kst)
{
*sl+=1;
*kst+=5;

fprintf(ffi,"\
%s %d.%d.%d %s %s %d:%d %90s%s N%d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min,
"",gettext("Лист"),
*sl);

  fprintf(ffi,"\
-------------------------------------------------------------------");
  if(mr == 1)
    fprintf(ffi,"\
--------------------------------------------------------------------------------");

  strpod(mn,mk,11,ffi);

  fprintf(ffi,gettext("\
Инв-рный|        Н а и м е н о в а н и е         | Номерной |Шифр |"));

  if(mr == 1)
    fprintf(ffi,gettext("\
Норма|Балансовая|  Износ   |Остаточная|Ликвидаци.|Годов. сумма|Мес. сумма|Коэфи|"));

  strms(0,mn,mk,ffi);

  fprintf(ffi,gettext("\
 номер  |             объекта                    |   знак   |н.ам.|"));

  if(mr == 1)
    fprintf(ffi,gettext("\
а.отч|стоимость |          |бал. с-сть|стоимость |амортотчисл.|амортотчи.|циент|"));

  strms(1,mn,mk,ffi);

  fprintf(ffi,"\
-------------------------------------------------------------------");
  if(mr == 1)
    fprintf(ffi,"\
--------------------------------------------------------------------------------");
  strpod(mn,mk,11,ffi);
}
/***************************/
/****************************/
void uosvamot_prp(class uosvamot_poi *poi,int *kol_str,FILE *ff)
{

if(poi->hnaz.getdlinna() > 1)
 {
  fprintf(ff,"%s: %s\n",gettext("Група"),poi->hnaz.ravno());
  *kol_str+=1;
 }
if(poi->podd.getdlinna() > 1)
 {
  fprintf(ff,"%s: %s\n",gettext("Подразделение"),poi->podd.ravno());
  *kol_str+=1;
 }
if(poi->hzt.getdlinna() > 1)
 {
  *kol_str+=1;
  fprintf(ff,"%s: %s\n",gettext("Шифр затрат"),poi->hzt.ravno());
 }
if(poi->invnom.getdlinna() > 1)
 {
  *kol_str+=1;
  fprintf(ff,"%s: %s\n",gettext("Инвентарный номер"),poi->invnom.ravno());
 }
if(poi->shetup.getdlinna() > 1)
 {
  *kol_str+=1;
  fprintf(ff,"%s: %s\n",gettext("Счёт"),poi->shetup.ravno());
 }
if(poi->sost.getdlinna() > 1)
 {
  *kol_str+=1;
  fprintf(ff,"%s: %s\n",gettext("Состояние объекта"),poi->sost.ravno());
 }
if(poi->hau.getdlinna() > 1)
 {
  *kol_str+=1;
  fprintf(ff,"%s: %s\n",gettext("Шифр аналитического учёта"),poi->hau.ravno());
 }

}
/******************************************/
/*шапка для нового отчёта*/
/*******************************/
void uosvamot1_sh(short mn,short mk,short god,FILE *ffi)
{

fprintf(ffi,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------");

strpod(mn,mk,11,ffi);

fprintf(ffi,gettext("\
Инв-рный|        Н а и м е н о в а н и е         | Номерной |Шифр |"));

fprintf(ffi,gettext("\
Норма|Балансовая  |   Износ    | Остаточная |Ликвидаци.|Годов.сумм|Мес. сумма|Коэфи|"));

strms(0,mn,mk,ffi);

fprintf(ffi,gettext("\
 номер  |             объекта                    |   знак   |н.ам.|"));

fprintf(ffi,gettext("\
а.отч|  стоимость |            |бал.стои-сть|стоимость |амортотчис|амортотчи.|циент|"));

strms(1,mn,mk,ffi);

fprintf(ffi,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------");

strpod(mn,mk,11,ffi);



}
/*******************************************/
/*распечатка по месяцам для нового отчёта*/
/*****************************************/
void rasmes1(short mn,short mk,double *maot,
int metka_ob, /*0-обнулять 1-нет*/
FILE *ff)
{
short           i;
double          vs;
vs=0.;

for(i=mn-1 ;i<mk ;i++)
 {
  vs+=maot[i];
  fprintf(ff,"%10.2f|",maot[i]);
  if(metka_ob == 0)
    maot[i]=0.;
 }
if(mn != mk)
   fprintf(ff,"%10.2f|",vs);
fprintf(ff,"\n");
}
/***********************************************/
/*Распечтатка итога по группе или шифру затрат*/
/************************************************/

void uosvamot1_itog(int metka,const char *kod,short mn,short mk,double *bsiz,double *amort,FILE *ff)
{
char strsql[512];
class iceb_tu_str strit("");
SQL_str row;
class SQLCURSOR cur;

if(metka == 1) /*по группе*/
 {
  strit.new_plus(gettext("Итого по группе"));
  strit.plus(" ",kod);
  sprintf(strsql,"select naik from Uosgrup where kod='%s'",kod);
  if(readkey(strsql,&row,&cur) == 1)
   strit.plus(" ",row[0]);
   
 }
if(metka == 2) /*по шифру затрат*/
 {
  strit.new_plus(gettext("Итого по шифру затрат"));
  strit.plus(" ",kod);
  sprintf(strsql,"select naik from Uoshz where kod='%s'",kod);
  if(readkey(strsql,&row,&cur) == 1)
   strit.plus(" ",row[0]);
 }
if(metka == 3) /*общий иток*/
 {
  strit.new_plus(gettext("Общий итог"));
 }
fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------");
strpod(mn,mk,11,ff);

fprintf(ff,"%*.*s|",
iceb_tu_kolbait(72,strit.ravno()),
iceb_tu_kolbait(72,strit.ravno()),
strit.ravno());

fprintf(ff,"%12.2f|%12.2f|%12.2f|%10.2f|%10.2f|%10.2f|     |",bsiz[0],bsiz[1],bsiz[2],bsiz[3],bsiz[4],bsiz[5]);

bsiz[0]=bsiz[1]=bsiz[2]=bsiz[3]=bsiz[4]=bsiz[5]=0.;

rasmes1(mn,mk,amort,0,ff);
if(metka == 2)
 fprintf(ff,"\n");
}
/***********************************************/
/*Распечтатка общих итогов*/
/************************************************/

void uosvamot1_itogo(int metka,const char *kod,short mn,short mk,double *bsiz,double *amort,FILE *ff)
{
char strsql[512];
class iceb_tu_str strit("");
SQL_str row;
class SQLCURSOR cur;

if(metka == 1) /*по группе*/
 {
  strit.new_plus(gettext("Итого по группе"));
  strit.plus(" ",kod);
  sprintf(strsql,"select naik from Uosgrup where kod='%s'",kod);
  if(readkey(strsql,&row,&cur) == 1)
   strit.plus(" ",row[0]);
   
 }
if(metka == 2) /*по шифру затрат*/
 {
  strit.new_plus(gettext("Итого по шифру затрат"));
  strit.plus(" ",kod);
  sprintf(strsql,"select naik from Uoshz where kod='%s'",kod);
  if(readkey(strsql,&row,&cur) == 1)
   strit.plus(" ",row[0]);
 }
if(metka == 3) /*общий иток*/
 {
  strit.new_plus(gettext("Общий итог"));
 }
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------");
strpod(mn,mk,11,ff);

fprintf(ff,"%*.*s|",
iceb_tu_kolbait(50,strit.ravno()),
iceb_tu_kolbait(50,strit.ravno()),
strit.ravno());

fprintf(ff,"%12.2f|%12.2f|%12.2f|%10.2f|%10.2f|%10.2f|",bsiz[0],bsiz[1],bsiz[2],bsiz[3],bsiz[4],bsiz[5]);

rasmes1(mn,mk,amort,1,ff);

if(metka == 1)
 fprintf(ff,"\n");
}
/****************************************************/
/*Новый отчёт*/
/****************************************************/
int uosvamot1(class spis_oth *oth,class uosvamot_poi *poi)
{
char strsql[512];
int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
short mn=0,gn=0;
short mk=0,gk=0;

sprintf(strsql,"%s.%s",poi->datan.ravno(),poi->godd.ravno());
if(rsdat1(&mn,&gn,strsql) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата начала !"));
  return(1);
 }

if(poi->datak.getdlinna() > 1)
 {
  sprintf(strsql,"%s.%s",poi->datak.ravno(),poi->godd.ravno());
  if(rsdat1(&mk,&gk,strsql) != 0)
   {
    iceb_t_soob(gettext("Не верно введена дата конца !"));
    return(1);
   }
 }
else
 {
  mk=mn; gk=gn;
 }

//int kolmes=period(1,mn,gn,1,mk,gk,1);

sprintf(strsql,"select innom,da,podr,hzt,hna,suma from Uosamor where da >= '%04d-%02d-01'\
 and da <= '%04d-%02d-31' order by hna asc,hzt asc, innom asc",gn,mn,gn,mk);
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


FILE *ff_innom;
char imaf_innom[64];
sprintf(imaf_innom,"amotnu1%d.lst",getpid());

if((ff_innom = fopen(imaf_innom,"w")) == NULL)
 {
  error_op_nfil(imaf_innom,errno,"");
  return(1);
 }

FILE *ff_it;
char imaf_it[64];
sprintf(imaf_it,"amotnui1%d.lst",getpid());

if((ff_it = fopen(imaf_it,"w")) == NULL)
 {
  error_op_nfil(imaf_it,errno,"");
  return(1);
 }

class iceb_fopen raoz;
char imaf_raoz[64];
sprintf(imaf_raoz,"raoz%d.lst",getpid());
if(raoz.start(imaf_raoz,"w") != 0)
 return(1);
class iceb_vrvr rv;

rv.ns.plus(4);
rv.begin.plus(0);
rv.end.plus(100);
rv.hag.plus(1);
rv.str.plus(iceb_t_get_pnk("00",0));

rv.ns.plus(5);
rv.begin.plus(31);
rv.end.plus(100);
rv.hag.plus(1);
rv.str.plus(iceb_t_get_edrpou("00"));

sprintf(strsql,"%02d.%d по %02d.%d р.",mn,gn,mk,gk);
rv.ns.plus(7);
rv.begin.plus(45);
rv.end.plus(100);
rv.hag.plus(1);
rv.str.plus(strsql);

iceb_t_vrvr("uosvamot_ra_h.alx",&rv,raoz.ff); /*печать заголовка документа*/
int kol=0;
uosvamot_h(1,&kol,raoz.ff); /*печать шапки документа*/
 
short dkm=0;
dpm(&dkm,&mk,&gk,5);

zagolov(gettext("Ведомость амортизационных отчислений"),1,mn,gn,dkm,mk,gk,iceb_t_get_pnk("00",0),ff_innom);
fprintf(ff_innom,"%s\n",gettext("Налоговый учёт"));

int kol_str=0;
uosvamot_prp(poi,&kol_str,ff_innom);

uosvamot1_sh(mn,mk,gn,ff_innom);

zagolov(gettext("Ведомость амортизационных отчислений"),1,mn,gn,dkm,mk,gk,iceb_t_get_pnk("00",0),ff_it);
fprintf(ff_it,"%s\n",gettext("Налоговый учёт"));

kol_str=0;
uosvamot_prp(poi,&kol_str,ff_it);

fprintf(ff_it,"\
---------------------------------------------------------------------------------------------------------------------------");
strpod(mn,mk,11,ff_it);

fprintf(ff_it,gettext("\
                Наименование итога                | Балансовая |   Износ    | Остаточная |Ликвидаци.|Годов.сумм|Мес. сумма|"));
strms(0,mn,mk,ff_it);

fprintf(ff_it,gettext("\
                                                  | стоимость  |            |  стоимость | стоимость|амортизац.|амортизац.|"));


strms(1,mn,mk,ff_it);

fprintf(ff_it,"\
---------------------------------------------------------------------------------------------------------------------------");
strpod(mn,mk,11,ff_it);


class iceb_tu_str hzt("");
class iceb_tu_str hna("");
class iceb_tu_str naim("");
int innom=0;
int innomz=0;
double itogo_hzt_bsiz[6];
double itogo_hna_bsiz[6];
double itogo_all_bsiz[6];

double itogo_hzt_amort[12];
double itogo_hna_amort[12];
double itogo_all_amort[12];
double amort_innom[12];

memset(itogo_hzt_amort,'\0',sizeof(itogo_hzt_amort));
memset(itogo_hna_amort,'\0',sizeof(itogo_hna_amort));
memset(itogo_all_amort,'\0',sizeof(itogo_hna_amort));
memset(itogo_hzt_bsiz,'\0',sizeof(itogo_hzt_bsiz));
memset(itogo_hna_bsiz,'\0',sizeof(itogo_hna_bsiz));
memset(itogo_all_bsiz,'\0',sizeof(itogo_hna_bsiz));
memset(amort_innom,'\0',sizeof(amort_innom));

class poiinpdw_data rekin;
class bsizw_data bal_st;

double bal_stoim=0.;
double bal_iznos=0.;
double ost_bal_st=0.;
double god_proc_am=0.;
double sgod_amort=0.;
double smes_amort=0.;
class iceb_tu_str naios("");
double suma_amort=0.;
int metka_vs_hna=0; /*метка вывода строки*/
int metka_vs_hzt=0;
int metka_vs_innom=0;
int kolstr1=0;
double itogo_raoz[3];
memset(itogo_raoz,'\0',sizeof(itogo_raoz));

while(cur.read_cursor(&row) != 0)
 {
/************
  printw("%s %s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4],row[5]);
  refresh();
**************/  
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(poi->invnom.ravno(),row[0],0,0) != 0)
   continue;
  if(proverka(poi->podd.ravno(),row[2],0,0) != 0)
   continue;
  if(proverka(poi->hzt.ravno(),row[3],0,0) != 0)
   continue;
  if(proverka(poi->hnaz.ravno(),row[4],0,0) != 0)
   continue;
   
  innom=atoi(row[0]);
  if(SRAV(hna.ravno(),row[4],0) != 0)
   {
    if(hna.ravno()[0] != '\0' && metka_vs_hna > 0)
     {
      if(metka_vs_innom > 0)
       {
        uosvamot_nf(innomz,mn,mk,gk,amort_innom,itogo_raoz,naios.ravno(),god_proc_am,rekin.popkf,raoz.ff);
        rasmes1(mn,mk,amort_innom,0,ff_innom);
        metka_vs_innom=0;
       }
      if(metka_vs_hzt > 0)
       {
        uosvamot1_itogo(2,hzt.ravno(),mn,mk,itogo_hzt_bsiz,itogo_hzt_amort,ff_it);
        uosvamot1_itog(2,hzt.ravno(),mn,mk,itogo_hzt_bsiz,itogo_hzt_amort,ff_innom);
        metka_vs_hzt=0;
       }     
      uosvamot1_itogo(1,hna.ravno(),mn,mk,itogo_hna_bsiz,itogo_hna_amort,ff_it);
      uosvamot1_itog(1,hna.ravno(),mn,mk,itogo_hna_bsiz,itogo_hna_amort,ff_innom);
      metka_vs_hna=0;
     }
    naim.new_plus("");
    god_proc_am=0.;    
    sprintf(strsql,"select naik,kof from Uosgrup where kod='%s'",row[4]);
    if(readkey(strsql,&row1,&cur1) == 1)
     {
      naim.new_plus(row1[0]);
      god_proc_am=atof(row1[1]);
     }
    fprintf(ff_innom,"\n%s:%s %s\n",gettext("Группа"),row[4],naim.ravno());

    hna.new_plus(row[4]);
   }

  if(SRAV(hzt.ravno(),row[3],0) != 0)
   {
    if(hzt.ravno()[0] != '\0' && metka_vs_hzt > 0)
     {
      if(metka_vs_innom > 0)
       {
        uosvamot_nf(innomz,mn,mk,gk,amort_innom,itogo_raoz,naios.ravno(),god_proc_am,rekin.popkf,raoz.ff);
        rasmes1(mn,mk,amort_innom,0,ff_innom);
        metka_vs_innom=0;
       }
      uosvamot1_itogo(2,hzt.ravno(),mn,mk,itogo_hzt_bsiz,itogo_hzt_amort,ff_it);
      uosvamot1_itog(2,hzt.ravno(),mn,mk,itogo_hzt_bsiz,itogo_hzt_amort,ff_innom);
      metka_vs_hzt=0;
     }    
    naim.new_plus("");
    sprintf(strsql,"select naik from Uoshz where kod='%s'",row[3]);
    if(readkey(strsql,&row1,&cur1) == 1)
     naim.new_plus(row1[0]);

    fprintf(ff_innom,"\n%s:%s %s\n",gettext("Шифр затрат"),row[3],naim.ravno());

    hzt.new_plus(row[3]);
   }
//  short mes_amort=atoi(row[1]);
  short mes_amort=0;
  short da=0,ga=0;
  rsdat(&da,&mes_amort,&ga,row[1],2);
  if(poiinpd(atoi(row[0]),mes_amort,gn,&rekin) != 0)
   {
    fprintf(ff_innom,"%d инвентарный номер не числится %d.%dг. !!!\n",innom,mes_amort,gn);


    sprintf(strsql,"%d инвентарный номер не числится %d.%dг. !!!",innom,mes_amort,gn);
    iceb_t_soob(strsql);
    continue;
   }

  if(proverka(poi->shetup.ravno(),rekin.shetu.ravno(),0,0) != 0)
   {
    continue;
   }
  
  if(poi->sost.ravno_pr() == 1)
   if(rekin.soso == 1 || rekin.soso == 2)
    {
     continue;
    }
  if(poi->sost.ravno_pr() == 2)
   if(rekin.soso == 0 || rekin.soso == 3)
    {
     continue;
    }         
  if(proverka(poi->hau.ravno(),rekin.hau.ravno(),0,0) != 0)
   continue;


  if(innomz != innom)
   {
    if(innomz != 0 && metka_vs_innom > 0 )
     {
    
      uosvamot_nf(innomz,mn,mk,gk,amort_innom,itogo_raoz,naios.ravno(),god_proc_am,rekin.popkf,raoz.ff);
      rasmes1(mn,mk,amort_innom,0,ff_innom);
      metka_vs_innom=0;
     }
    bal_st.clear();
    bsiz(innom,atoi(row[2]),1,mn,gn,&bal_st,NULL);
    
    bal_stoim=bal_st.bs+bal_st.sbs;
    bal_iznos=bal_st.iz+bal_st.iz1+bal_st.siz;
    ost_bal_st=bal_stoim-bal_iznos;
    
    double lik_st=uosgetls(innom,1,mn,gn,0);

    sgod_amort=(bal_stoim-lik_st)*god_proc_am/100.*rekin.popkf;
    smes_amort=sgod_amort/12.;

    itogo_hna_bsiz[0]+=bal_stoim;
    itogo_hna_bsiz[1]+=bal_iznos;
    itogo_hna_bsiz[2]+=ost_bal_st;
    itogo_hna_bsiz[3]+=lik_st;
    itogo_hna_bsiz[4]+=sgod_amort;
    itogo_hna_bsiz[5]+=smes_amort;

    itogo_hzt_bsiz[0]+=bal_stoim;
    itogo_hzt_bsiz[1]+=bal_iznos;
    itogo_hzt_bsiz[2]+=ost_bal_st;
    itogo_hzt_bsiz[3]+=lik_st;
    itogo_hzt_bsiz[4]+=sgod_amort;
    itogo_hzt_bsiz[5]+=smes_amort;

    itogo_all_bsiz[0]+=bal_stoim;
    itogo_all_bsiz[1]+=bal_iznos;
    itogo_all_bsiz[2]+=ost_bal_st;
    itogo_all_bsiz[3]+=lik_st;
    itogo_all_bsiz[4]+=sgod_amort;
    itogo_all_bsiz[5]+=smes_amort;
        
    naios.new_plus("");
    sprintf(strsql,"select naim from Uosin where innom=%d",innom);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      sprintf(strsql,"%s %d !",gettext("Не найден инвентарный номер"),innom);
      iceb_t_soob(strsql);
     }
    else
     naios.new_plus(row1[0]);

    fprintf(ff_innom,"%-8s|%-*.*s|%-*.*s|%-*s|",
    row[0],
    iceb_tu_kolbait(40,naios.ravno()),iceb_tu_kolbait(40,naios.ravno()),naios.ravno(),
    iceb_tu_kolbait(10,rekin.nomz.ravno()),iceb_tu_kolbait(10,rekin.nomz.ravno()),rekin.nomz.ravno(),
    iceb_tu_kolbait(5,rekin.hna.ravno()),rekin.hna.ravno());

    fprintf(ff_innom,"%5.2f|%12.2f|%12.2f|%12.2f|%10.2f|%10.2f|%10.2f|%5.2f|",god_proc_am,bal_stoim,bal_iznos,ost_bal_st,lik_st,sgod_amort,smes_amort,rekin.popkf);
    

    innomz=innom;
   }  
  suma_amort=atof(row[5]);

  itogo_hzt_amort[mes_amort-1]+=suma_amort;
  itogo_hna_amort[mes_amort-1]+=suma_amort;
  itogo_all_amort[mes_amort-1]+=suma_amort;
  amort_innom[mes_amort-1]+=suma_amort;

  metka_vs_hna++;  
  metka_vs_hzt++;  
  metka_vs_innom++;  
 }

if(metka_vs_innom > 0)
 {
  uosvamot_nf(innom,mn,mk,gk,amort_innom,itogo_raoz,naios.ravno(),god_proc_am,rekin.popkf,raoz.ff);
  rasmes1(mn,mk,amort_innom,0,ff_innom);
 }
 

if(metka_vs_hzt > 0)
 {
  uosvamot1_itogo(2,hzt.ravno(),mn,mk,itogo_hzt_bsiz,itogo_hzt_amort,ff_it);
  uosvamot1_itog(2,hzt.ravno(),mn,mk,itogo_hzt_bsiz,itogo_hzt_amort,ff_innom);
 }

if(metka_vs_hna > 0)
 {
  uosvamot1_itogo(1,hna.ravno(),mn,mk,itogo_hna_bsiz,itogo_hna_amort,ff_it);
  uosvamot1_itog(1,hna.ravno(),mn,mk,itogo_hna_bsiz,itogo_hna_amort,ff_innom);
 }

double iamort=0.;

for(int nom=0; nom < 12; nom++)
  iamort+=itogo_all_amort[nom];

clear();
move(LINES/2-2,0);

printw("\
%*s:%15s\n",
iceb_tu_kolbait(25,gettext("Балансовая стоимость")),
gettext("Балансовая стоимость"),
prnbr(itogo_all_bsiz[0]));

printw("\
%*s:%15s\n",
iceb_tu_kolbait(25,gettext("Износ")),
gettext("Износ"),
prnbr(itogo_all_bsiz[1]));

printw("\
%*s:%15s\n",
iceb_tu_kolbait(25,gettext("Остаточная стоимость")),
gettext("Остаточная стоимость"),
prnbr(itogo_all_bsiz[2]));

printw("\
%*s:%15s\n",
iceb_tu_kolbait(25,gettext("Амортизация")),
gettext("Амортизация"),
prnbr(iamort));

uosvamot1_itogo(3,"",mn,mk,itogo_all_bsiz,itogo_all_amort,ff_it);
uosvamot1_itog(3,"",mn,mk,itogo_all_bsiz,itogo_all_amort,ff_innom);


fprintf(raoz.ff,"%*s %12.2f %12.2f %12.2f\n",
iceb_tu_kolbait(101,"Всього"),"Всього",
itogo_raoz[0],
itogo_raoz[1],
itogo_raoz[2]);



podpis(ff_innom);
fclose(ff_innom);

podpis(ff_it);
fclose(ff_it);

fprintf(raoz.ff,"ICEB_LST_END\n");
iceb_t_vrvr("uosvamot_ra_end.alx",NULL,raoz.ff); /*печать концовки документа*/

podpis(raoz.ff);
raoz.end();

oth->spis_imaf.plus(imaf_innom);
oth->spis_naim.plus(gettext("Распечатка ведомости амортотчислений"));
oth->spis_imaf.plus(imaf_it);
oth->spis_naim.plus(gettext("Общие итоги"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);

oth->spis_imaf.plus(imaf_raoz);
oth->spis_naim.plus(gettext("Расчёт амортизации основных средств"));

iceb_t_ustpeh(imaf_raoz,3,&kolstr);
iceb_t_rnl(imaf_raoz,kolstr,"",&uosvamot_h);

OSTANOV();

return(0);
}
/*******************************************/
/* */
/**********************************/
void uosvamot_nf(int innom,short mn,short mk,short gr,double *amort_innom,double *itogo_raoz,const char *naios,double god_proc_am,double popkf,FILE *raoz)
{

/*
-----------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
Субрахунок|Інвентарний|     Назва об’єкта            |Вартість яка|Річна сума |Кількість місяців     | Сума зносу |Сума нарахо-|Сума зносу  |Субрахунок|Примітка|\n\
          |   номер   |                              |амортизуєть-|амортизації|корисного використання| на початок |ваної аморт-|на кінець   |  витрат  |        |\n\
          |           |                              |     ся     |           |(експлуанації) у пері-|  періоду   |изації за   |періоду     |          |        |\n\
          |           |                              |            |           |оді                   |            |  період    |(гр.7+гр.8) |          |        |\n\
-----------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
    1     |     2     |              3               |     4      |     5     |          6           |      7     |      8     |     9      |    10    |   11   |\n\
-----------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

1234567890 12345678901 123456789012345678901234567890 123456789012 12345678901 1234567890123456789012 123456789012 123456789012 123456789012
*/
/*проверяем числится ли на конец периода*/

class bsizw_data bal_st;
short d=1;
short m=mk;
short g=gr;
dpm(&d,&m,&g,3); /*увеличиваем на месяц нужна стоимость на начало следующего месяца*/
int podr=0;
int kodotl=0;
int metka_bs=0; /*0-балансовая стоимость и износ взяты на конец периода 1- на начало*/

/*проверяем числится ли на конец периода*/
if(metkabo == 1) /*бюджетная организация если нет на конец периода то амортизация не считалась*/
 {
  if(poiin(innom,1,m,g,&podr,&kodotl) != 0) /*амортизация считается раз в году на 12-й месяц*/
   return;
 }
else
 {
  /*если на конец периода нет то нужно смотреть балансовую стоимость на начало периода*/
  if(poiin(innom,1,m,g,&podr,&kodotl) != 0)
   {
    m=mn;
    g=gr;
    metka_bs=1;
   }
 }



bsiz(innom,0,1,m,g,&bal_st,NULL);




double bal_stoim=bal_st.bs+bal_st.sbs;
double bal_iznos=bal_st.iz+bal_st.iz1+bal_st.siz;

double lik_st=uosgetls(innom,1,mk,gr,0);

double sgod_amort=(bal_stoim-lik_st)*god_proc_am/100.*popkf;

int kolmes=mk-mn+1;

double amort_za_period=0.;

for(int i=mn-1 ;i<mk ;i++)
 {
  amort_za_period+=amort_innom[i];
 }


if(metka_bs == 0)/*балансовая стоимость и износ взяты на конец периода*/
 {
  bal_iznos-=amort_za_period;
 }


double iznos_end=bal_iznos+amort_za_period;

fprintf(raoz,"%10s %-11d %-*.*s %12.2f %11.2f %22d %12.2f %12.2f %12.2f\n",
"",
innom,
iceb_tu_kolbait(30,naios),iceb_tu_kolbait(30,naios),naios,
bal_stoim,
sgod_amort,
kolmes,
bal_iznos,
amort_za_period,
iznos_end);

if(iceb_tu_strlen(naios) > 30)
 fprintf(raoz,"%10s %11s %s\n","","",iceb_tu_adrsimv(30,naios));

itogo_raoz[0]+=bal_iznos;
itogo_raoz[1]+=amort_za_period;
itogo_raoz[2]+=iznos_end;
}
