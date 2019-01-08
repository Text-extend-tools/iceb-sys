/* $Id: uosvamotby.c,v 5.67 2013/11/05 10:50:49 sasa Exp $ */
/*22.11.2016    06.12.2000      Белых А.И.      uosvamotby.c
Получение ведомости амортизационных отчислений для бухучета
*/
#include        <errno.h>
#include        "buhg.h"
#include "poiinpd.h"
#include "bsizt.h"

void strms(short,short,short,FILE*);
void strpod(short,short,short,FILE*);
void uosshapbu(short mn,short gn,short mk,short gk,struct tm *bf,short *sl,short mr,FILE *ffi,short *kst);
void shvambu(short mn,short gn,short mk,short gk,short *sl,short *kst,int orient,short mr,FILE *ff,struct tm *bf);
void itamobu(short mn,short mk,double itg[],short mr,double maotpo[12],FILE *ff,FILE *ffi);
void itambu(short mn,short gn,short mk,short gk,double itg[],const char *nai,int orient,short mr,double maotp[12],FILE *ff,FILE *ffi,short *sli,short *kst,struct tm *bf);
void itamhbu(short mn,short gn,short mk,short gk,double itg[],const char *kod,const char *nai,int orient,short mr,double maotph[12],FILE *ff,FILE *ffi,int metkaitog,short *sli,short *kst,struct tm *bf);

extern int      kol_strok_na_liste; //Количество строк на листе в портретной ориентации
extern int      kol_strok_na_liste_l; //Количество строк на листе в ландшафтной ориентации
extern class iceb_tu_str spis_print_a3;

int uosvamotby(class spis_oth *oth)
{
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
short           sli=0,kst=0;
short           slii=0;
int         in,in1;
int             i;
class bsizw_data bal_st;
double          god=0,mes=0;
short           mvr=0; /*Метка выдачи результата*/
const int RAZMER_MAS=6;
double          itg[RAZMER_MAS];
double          itg1[RAZMER_MAS];
double          itg2[RAZMER_MAS];
static class iceb_tu_str dat1(""),dat2("");
class iceb_tu_str naimhz("");
class iceb_tu_str naipod("");
double          ostt=0;
class iceb_tu_str hna(""),hnaz1("");
class iceb_tu_str hzt1(""),hzt2(""); /*Шифр затрат*/
static class iceb_tu_str hnaz("");
static class iceb_tu_str hzt("");
static class iceb_tu_str podd("");
static class iceb_tu_str godd("");
static class iceb_tu_str invnom("");
static class iceb_tu_str shetu_poi("");
static class iceb_tu_str kod_mat_ot("");
static class iceb_tu_str hau("");

short		mr; /*Метка распечатки*/
class iceb_tu_str naios(""); /*Наименование основного средства*/
float		kof;
short		m=0,g=0;
int		kolstr,kolstr1;
SQL_str         row,row1;
char		strsql[1024];

double		sao;
SQLCURSOR curr;
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
class iceb_tu_str gvve(""); //Год ввода в эксплуатацию

VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка ведомости амортотчислений для бух.учета"));
VV.VVOD_SPISOK_add_data(dat1.ravno(),3);
VV.VVOD_SPISOK_add_data(dat2.ravno(),3);
if(godd.ravno()[0] == '\0')
  godd.new_plus(gt);
VV.VVOD_SPISOK_add_data(godd.ravno(),5);
VV.VVOD_SPISOK_add_data(hzt.ravno(),128);
VV.VVOD_SPISOK_add_data(podd.ravno(),128);
VV.VVOD_SPISOK_add_data(hnaz.ravno(),128);
VV.VVOD_SPISOK_add_data(invnom.ravno(),128);
VV.VVOD_SPISOK_add_data(shetu_poi.ravno(),128);
VV.VVOD_SPISOK_add_data(kod_mat_ot.ravno(),128);
VV.VVOD_SPISOK_add_data(hau.ravno(),128);

VV.VVOD_SPISOK_add_MD(gettext("Месяц начала.(м)..."));//0
VV.VVOD_SPISOK_add_MD(gettext("Месяц конца..(м)..."));//1
VV.VVOD_SPISOK_add_MD(gettext("Год................"));//2
VV.VVOD_SPISOK_add_MD(gettext("Шифр затрат...(,,)."));//3
VV.VVOD_SPISOK_add_MD(gettext("Подразделение.(,,)."));//4
VV.VVOD_SPISOK_add_MD(gettext("Группа........(,,)."));//5
VV.VVOD_SPISOK_add_MD(gettext("Инв-ный номер.(,,)."));//6
VV.VVOD_SPISOK_add_MD(gettext("Счёт учёта....(,,)."));//7
VV.VVOD_SPISOK_add_MD(gettext("Код мат.ответ.(,,)."));//8
VV.VVOD_SPISOK_add_MD(gettext("Шифр ан.учёта.(,,)."));//9

naz:;

clear();

helstr(LINES-1,0,"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
godd.new_plus(VV.data_ravno(2));
hzt.new_plus(VV.data_ravno(3));

podd.new_plus(VV.data_ravno(4));

hnaz.new_plus(VV.data_ravno(5));
invnom.new_plus(VV.data_ravno(6));
shetu_poi.new_plus(VV.data_ravno(7));
kod_mat_ot.new_plus(VV.data_ravno(8));
hau.new_plus(VV.data_ravno(9));

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
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка шифров затрат"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка шифров аналитического учёта"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 4 :
      case -1:
       goto naz;

      case 0 :
        vibrek("Uosgrup1",&hnaz,&naim_v);
        VV.data_z_plus(5,hnaz.ravno());
        goto naz;
         
      case 1 :
        if(diruospod(1,&kod_v,&naim_v,0) == 0)
         VV.data_z_plus(4,kod_v.ravno());
        goto naz;

      case 2 :
        vibrek("Uoshz",&hzt,&naim_v);
        VV.data_z_plus(3,hzt.ravno());
        goto naz;

      case 3 :
        vibrek("Uoshau",&kod_v,&naim_v);
        VV.data_z_plus(9,kod_v.ravno());
        goto naz;


     }
    goto naz;
    

  case FK4: /*Очистить меню*/

    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
 }

bros.new_plus(VV.VVOD_SPISOK_return_data(0));
bros.plus(".",VV.VVOD_SPISOK_return_data(2));
if(rsdat1(&mn,&gn,bros.ravno()) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата начала !"));
  goto naz;
 }
if(VV.VVOD_SPISOK_return_data(1)[0] != '\0')
 {
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

time(&tmm);
bf=localtime(&tmm);

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

sprintf(strsql,"select innom,da,podr,hzt,hna,suma from Uosamor1 where da >= '%04d-%02d-01' and da <= '%04d-%02d-31' order by hzt,podr,innom,da asc",gn,mn,gk,mk);
class SQLCURSOR cur;
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


sprintf(imaf,"vaoby%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

int orient=1;

if(kol_mes_v_per > 3)
 {
  orient=0; /*Если принтер А4 то ориентация портрет*/
 }

zagolov(gettext("Ведомость амортизационных отчислений"),1,mn,gn,pdm,mk,gk,iceb_t_get_pnk("00",0),ff);

fprintf(ff,"%s\n",gettext("Бухгалтерский учёт"));
kst=6;

if(hnaz.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Група"),hnaz.ravno());
  kst++;
 }
if(podd.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Подразделение"),podd.ravno());
  kst++;
 }
if(hzt.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Шифр затрат"),hzt.ravno());
  kst++;
 }
if(invnom.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Инвентарный номер"),invnom.ravno());
  kst++;
 }
if(shetu_poi.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Счёт"),shetu_poi.ravno());
  kst++;
 }
if(kod_mat_ot.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Код материально-ответсвенного"),kod_mat_ot.ravno());
  kst++;
 }
if(hau.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Шифр аналитического учёта"),hau.ravno());
  kst++;
 }
sprintf(imafi,"vaobyi%d.lst",getpid());

if((ffi = fopen(imafi,"w")) == NULL)
 {
  error_op_nfil(imafi,errno,"");
  return(1);
 }



zagolov(gettext("Ведомость амортизационных отчислений"),1,mn,gn,pdm,mk,gk,iceb_t_get_pnk("00",0),ffi);


fprintf(ffi,"%s\n",gettext("Бухгалтерский учёт"));

if(hnaz.ravno()[0] != '\0')
 {
  fprintf(ffi,"%s: %s\n",gettext("Група"),hnaz.ravno());
 }
if(podd.ravno()[0] != '\0')
 {
  fprintf(ffi,"%s: %s\n",gettext("Подразделение"),podd.ravno());
 }
if(hzt.ravno()[0] != '\0')
 {
  fprintf(ffi,"%s: %s\n",gettext("Шифр затрат"),hzt.ravno());
 }
if(invnom.ravno()[0] != '\0')
 {
  fprintf(ffi,"%s: %s\n",gettext("Инвентарный номер"),invnom.ravno());
 }
if(shetu_poi.ravno()[0] != '\0')
 {
  fprintf(ffi,"%s: %s\n",gettext("Счёт"),shetu_poi.ravno());
 }

if(hau.getdlinna() > 1)
 fprintf(ffi,"%s:%s\n",gettext("Шифр аналитического учёта"),hau.ravno());
short ksti=0;
uosshapbu(mn,gn,mk,gk,bf,&slii,mr,ffi,&ksti);

in=0;
for(i=0; i< 12; i++)
    maot[i]=maotp[i]=maotph[i]=maotpo[i]=0.;
for(i=0;i<RAZMER_MAS;i++)
  itg[i]=itg1[i]=itg2[i]=0.;


naipod.new_plus(iceb_t_get_pnk("00",0));
pod1=-100;
kolstr1=in1=mvr=0;
int vrabu; //Вариант расчёта амотизации бухгалтерского учёта
class poiinpdw_data rekin;
double amort_za_period=0.;
double iznos_end=0.;
double itogo_raoz[3];
memset(itogo_raoz,'\0',sizeof(itogo_raoz));

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(proverka(invnom.ravno(),row[0],0,0) != 0)
    continue;

  if(proverka(hzt.ravno(),row[3],0,0) != 0)
    continue;


  if(proverka(podd.ravno(),row[2],0,0) != 0)
    continue;

  if(proverka(hnaz.ravno(),row[4],0,0) != 0)
    continue;

  in=atol(row[0]);
  rsdat(&d,&m,&g,row[1],2);
  pod=atoi(row[2]);
  hzt1.new_plus(row[3]);
  hna.new_plus(row[4]);
  sao=atof(row[5]);

  if(SRAV(hzt2.ravno(),hzt1.ravno(),0) != 0)
   {

    if(hzt2.ravno()[0] != '\0')
     {
      if(mvr > 0)
       {
        rasmes(mn,mk,maot,ostt,hnaz1.ravno(),ff);
        mvr=0;
       }
      itambu(mn,gn,mk,gk,itg1,naipod.ravno(),orient,mr,maotp,ff,ffi,&sli,&kst,bf);
      itamhbu(mn,gn,mk,gk,itg2,hzt2.ravno(),naimhz.ravno(),orient,mr,maotph,ff,ffi,1,&sli,&kst,bf);
      for(i=0;i<RAZMER_MAS;i++)
	itg1[i]=itg2[i]=0.;
      fprintf(ff,"\f");
      kst=0;
      pod1=0;
     }

    /*Читаем шифр производственных затрат*/
    sprintf(strsql,"select naik from Uoshz where kod='%s'",hzt1.ravno());
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      beep();
      printw("%s %s !",gettext("Не найден шифр производственных затрат"),hzt1.ravno());
      OSTANOV();
      naimhz.new_plus("");
     }
    else
     {
      naimhz.new_plus(row1[0]);
     }          
    printw("%s: %s %s\n",gettext("Шифр производственных затрат"),hzt1.ravno(),naimhz.ravno());
//    refresh(); 

    shvambu(mn,gn,mk,gk,&sli,&kst,orient,mr,ff,bf);

    fprintf(ff,"%s: %s %s\n",gettext("Шифр производственных затрат"),hzt1.ravno(),naimhz.ravno());
    uosshapbu(mn,gn,mk,gk,bf,&sli,mr,ff,&kst);

    for(i=0; i< 12; i++)
     maotp[i]=maotph[i]=0.;

    hzt2.new_plus(hzt1.ravno());
   }
  if(pod1 != pod)
   {
    if(mvr > 0 )
     {
      rasmes(mn,mk,maot,ostt,hnaz1.ravno(),ff);
      mvr=0;

      itambu(mn,gn,mk,gk,itg1,naipod.ravno(),orient,mr,maotp,ff,ffi,&sli,&kst,bf);
      for(i=0;i<RAZMER_MAS;i++)
	itg1[i]=0.;
     }
   
    /*Читаем наименование подразделения*/
    naipod.new_plus("");
    if(in > 0)
     {
      sprintf(strsql,"select naik from Uospod where kod=%d",pod);
      if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
       {
        beep();
        printw("%s %d !\n",gettext("Не найден код подразделения"),pod);
        OSTANOV();
       }
      else
       naipod.new_plus(row1[0]);
     }
    else
      naipod.new_plus(iceb_t_get_pnk("00",0));

    printw("%s %d %s\n",gettext("Подразделение"),pod,naipod.ravno());
//    refresh();

    shvambu(mn,gn,mk,gk,&sli,&kst,orient,mr,ff,bf);
    fprintf(ff,"%s %d %s\n",gettext("Подразделение"),pod,naipod.ravno());

    shvambu(mn,gn,mk,gk,&sli,&kst,orient,mr,ff,bf);
    fprintf(ff,"------------------------------------\n");

    for(i=0; i< 12; i++)
     maotp[i]=0.;

    pod1=pod;
   }
  /*Узнаем поправочный коэффициент счёт учёта в данном месяце*/
  if(poiinpd(in,m,g,&rekin) != 0)
   {
    beep();
    printw("%d инвентарный номер не числится %d.%dг. !!!\n",in,m,g);
    refresh();

    shvambu(mn,gn,mk,gk,&sli,&kst,orient,mr,ff,bf);
    fprintf(ff,"%d инвентарный номер не числится %d.%dг. !!!\n",in,m,g);

   }

  if(proverka(shetu_poi.ravno(),rekin.shetu.ravno(),0,0) != 0)
    continue;
  if(proverka(hau.ravno(),rekin.hau.ravno(),0,0) != 0)
   continue;
  if(in1 != in)
   {
//    printw("mvr=%d in1=%d\n",mvr,in1);
//    refresh();
    if(kod_mat_ot.ravno()[0] != '\0')
     {
      int podr=0;
      int kod_ot=0;
      poiin(in,1,mn,gn,&podr,&kod_ot);
      if(proverka(kod_mat_ot.ravno(),kod_ot,0,0) != 0)
        continue;
     }

    if(mvr > 0 && in1 != 0)
     {
      rasmes(mn,mk,maot,ostt,hnaz1.ravno(),ff);
      mvr=0;
     }
    in1=in;

    for(i=0; i< 12; i++)
     maot[i]=0.;


    /*Читаем наименование объекта*/
    sprintf(strsql,"select naim,datvv from Uosin where innom=%d",in);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      beep();
      printw("%s %d !\n",
      gettext("Не найден инвентарный номер"),in);
      OSTANOV();
      naios.new_plus("");
     }
    else
     {
      naios.new_plus(row1[0]);

      if(row1[1][0] != '\0')
       {
        short dv,mv,gv;
        rsdat(&dv,&mv,&gv,row1[1],2);
        gvve.new_plus(gv);
       }
     }

    /*Читаем коэффициент*/
    sprintf(strsql,"select naik,kof from Uosgrup1 where kod='%s'",rekin.hnaby.ravno());

    kof=0;
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      beep();
      printw("%d %s %s !\n",in,gettext("Не найдено группу"),rekin.hnaby.ravno());
//      printw("В таблице Uosgrup1 !\n");
      OSTANOV();
      continue;
     }
    else
     {
      kof=atof(row1[1]);
     }

    /*На начало периода*/

    bsiz(in,pod1,1,mn,gn,&bal_st,NULL);
    bal_st.bsby+=bal_st.sbsby;
    bal_st.izby+=bal_st.iz1by+bal_st.sizby;
    vrabu=uosprovar(rekin.hnaby.ravno(),1);
    ostt=bal_st.bsby-bal_st.izby;

    double lik_st=uosgetls(in,1,mn,gn,1);

    if(vrabu == 1)
       god=(bal_st.bsby-bal_st.izby)*kof/100.;
    if(vrabu == 0)
       god=(bal_st.bsby-lik_st)*kof/100.;

    mes=0.;
    if(god != 0.)
      mes=god/12.;


    itg[0]+=bal_st.bsby;
    itg[1]+=bal_st.izby;
    itg[2]+=ostt;
    itg[3]+=lik_st;    
    itg[4]+=god;
    itg[5]+=mes;

    itg1[0]+=bal_st.bsby;
    itg1[1]+=bal_st.izby;
    itg1[2]+=ostt;
    itg1[3]+=lik_st;
    itg1[4]+=god;
    itg1[5]+=mes;

    itg2[0]+=bal_st.bsby;
    itg2[1]+=bal_st.izby;
    itg2[2]+=ostt;
    itg2[3]+=lik_st;
    itg2[4]+=god;
    itg2[5]+=mes;


    shvambu(mn,gn,mk,gk,&sli,&kst,orient,mr,ff,bf);

    fprintf(ff,"%-8d|%-*.*s|%4s|%-*.*s|%-*s|",
    in,
    iceb_tu_kolbait(40,naios.ravno()),iceb_tu_kolbait(40,naios.ravno()),naios.ravno(),
    gvve.ravno(),
    iceb_tu_kolbait(10,rekin.nomz.ravno()),iceb_tu_kolbait(10,rekin.nomz.ravno()),rekin.nomz.ravno(),
    iceb_tu_kolbait(5,rekin.hnaby.ravno()),rekin.hnaby.ravno());

    if(mr == 1)
      fprintf(ff,"%5.2f|%10.2f|%10.2f|%10.2f|%10.2f|%12.2f|%10.2f|%5.2f|",kof,bal_st.bsby,bal_st.izby,ostt,lik_st,god,mes,rekin.popkfby);

    mvr++;
    hnaz1.new_plus(hna.ravno());


    amort_za_period=0.;
    for(int i=mn-1 ;i<mk ;i++)
     {
      amort_za_period+=maot[i];
     }
    iznos_end=bal_st.izby+amort_za_period;
    
    fprintf(raoz.ff,"%10s %-11s %-*.*s %12.2f %11.2f %22d %12.2f %12.2f %12.2f\n",
    "",
    row[0],
    iceb_tu_kolbait(30,naios.ravno()),iceb_tu_kolbait(30,naios.ravno()),naios.ravno(),
    bal_st.bsby,
    ostt,
    kol_mes_v_per,
    bal_st.izby,
    amort_za_period,
    iznos_end);
 
    if(iceb_tu_strlen(naios.ravno()) > 30)
     fprintf(raoz.ff,"%10s %11s %s\n","","",iceb_tu_adrsimv(30,naios.ravno()));
 
    itogo_raoz[0]+=bal_st.izby;
    itogo_raoz[1]+=amort_za_period;
    itogo_raoz[2]+=iznos_end;


   }

  maot[m-1]=sao;
  maotp[m-1]=maotp[m-1]+sao;
  maotph[m-1]=maotph[m-1]+sao;
  maotpo[m-1]=maotpo[m-1]+sao;
 }

rasmes(mn,mk,maot,ostt,hnaz1.ravno(),ff);

itambu(mn,gn,mk,gk,itg1,naipod.ravno(),orient,mr,maotp,ff,ffi,&sli,&kst,bf);
itamhbu(mn,gn,mk,gk,itg2,hzt2.ravno(),naimhz.ravno(),orient,mr,maotph,ff,ffi,1,&sli,&kst,bf);
itamobu(mn,mk,itg,mr,maotpo,ff,ffi);

podpis(ff);
fclose(ff);

podpis(ffi);
fclose(ffi);

fprintf(raoz.ff,"%*s %12.2f %12.2f %12.2f\n",
iceb_tu_kolbait(101,"Всього"),"Всього",
itogo_raoz[0],
itogo_raoz[1],
itogo_raoz[2]);
fprintf(raoz.ff,"ICEB_LST_END\n");
iceb_t_vrvr("uosvamot_ra_end.alx",NULL,raoz.ff); /*печать концовки документа*/


podpis(raoz.ff);
raoz.end();

printw_vr(tmm);



OSTANOV();
oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Распечатка ведомости амортотчислений для бух.учета"));
oth->spis_imaf.plus(imafi);
oth->spis_naim.plus(gettext("Общие итоги"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);

oth->spis_imaf.plus(imaf_raoz);
oth->spis_naim.plus(gettext("Расчёт амортизации основных средств"));

iceb_t_ustpeh(imaf_raoz,3,&kolstr);
iceb_t_rnl(imaf_raoz,kolstr,"",&uosvamot_h);
 
return(0);

}
/*************************************************/
/*Шапка*/
/*************************************************/

void	uosshapbu(short mn,short gn,short mk,short gk,struct tm *bf,
short *sl,short mr,FILE *ffi,short *kst)
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
------------------------------------------------------------------------");
if(mr == 1)
 fprintf(ffi,"\
--------------------------------------------------------------------------------");

strpod(mn,mk,11,ffi);

fprintf(ffi,gettext("\
Инв-рный|        Н а и м е н о в а н и е         |Рік | Номерной |Шифр |"));

if(mr == 1)
  fprintf(ffi,gettext("Норма|Балансовая|   Износ  |Остаточная|Ликвидаци.|Годов. сумма|Мес. сумма|Коэфи|"));

strms(0,mn,mk,ffi);
fprintf(ffi,gettext("\
 номер  |             объекта                    |    |   знак   |н.ам.|"));
if(mr == 1)
  fprintf(ffi,gettext("а.отч|стоимость |          |бал. с-сть|стоимость |амортотчисл.|амортотчи.|циент|"));

strms(1,mn,mk,ffi);

fprintf(ffi,"\
------------------------------------------------------------------------");
if(mr == 1)
  fprintf(ffi,"\
--------------------------------------------------------------------------------");
strpod(mn,mk,11,ffi);
}
/*****************/
/*Шапка документа*/
/*****************/
void shvambu(short mn,short gn,short mk,short gk,short *sl,
short *kst,int orient,short mr,FILE *ff,struct tm *bf)
{

*kst+=1;
if(orient == 0) /*портрет*/
 {
  if(*kst <= kol_strok_na_liste)
    return;
 }
else /*ландшафт*/
  if(*kst <= kol_strok_na_liste_l)
   return;
  
fprintf(ff,"\f");

*kst=1;

uosshapbu(mn,gn,mk,gk,bf,sl,mr,ff,kst);

}
/*******************************/
/*Выдача итога по подразделению*/
/*******************************/
void itambu(short mn,short gn,short mk,short gk,double itg[],const char *nai,int orient,short mr,
double maotp[12],FILE *ff,FILE *ffi,short *sli,short *kst,
struct tm *bf)

{
short           i;
double          vs;
char		bros[512];

shvambu(mn,gn,mk,gk,sli,kst,orient,mr,ff,bf);
fprintf(ff,"\
------------------------------------------------------------------------");
fprintf(ffi,"\
------------------------------------------------------------------------");
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

shvambu(mn,gn,mk,gk,sli,kst,orient,mr,ff,bf);

fprintf(ff,"%-*.*s|",iceb_tu_kolbait(71,bros),iceb_tu_kolbait(71,bros),bros);
fprintf(ffi,"%-*.*s|",iceb_tu_kolbait(71,bros),iceb_tu_kolbait(71,bros),bros);


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
void itamhbu(short mn,short gn,short mk,short gk,double itg[],
const char *kod,
const char *nai,
int orient,
short mr,
double maotph[12],FILE *ff,FILE *ffi,
int metkaitog,  //0-по группе 1-по шифру
short *sli,short *kst,
struct tm *bf)
{
short           i;
double          vs;
char		bros[512];

shvambu(mn,gn,mk,gk,sli,kst,orient,mr,ff,bf);
fprintf(ff,"\
------------------------------------------------------------------------");
fprintf(ffi,"\
------------------------------------------------------------------------");

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

shvambu(mn,gn,mk,gk,sli,kst,orient,mr,ff,bf);
fprintf(ff,"%-*.*s|",iceb_tu_kolbait(71,bros),iceb_tu_kolbait(71,bros),bros);
fprintf(ffi,"%-*.*s|",iceb_tu_kolbait(71,bros),iceb_tu_kolbait(71,bros),bros);

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

if(metkaitog == 1)
 fprintf(ffi,"\n");
}
/*******************************/
/*Выдача итога по организации*/
/*******************************/
void itamobu(short mn,short mk,double itg[],short mr,
double maotpo[12],FILE *ff,FILE *ffi)
{
short           i;
double          vs;
char		bros[512];

fprintf(ff,"\
------------------------------------------------------------------------");
fprintf(ffi,"\
------------------------------------------------------------------------");
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

fprintf(ff,"%-*.*s|",iceb_tu_kolbait(71,bros),iceb_tu_kolbait(71,bros),bros);
fprintf(ffi,"%-*.*s|",iceb_tu_kolbait(71,bros),iceb_tu_kolbait(71,bros),bros);


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
