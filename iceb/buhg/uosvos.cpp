/* $Id: uosvos.c,v 5.28 2013/09/26 09:43:42 sasa Exp $ */
/*20.06.2013	19.11.1999	Белых А.И.	uosvos.c
Получение ведомости остаточной стоимости
*/
#include        <math.h>
#include        <errno.h>
#include        "buhg.h"
#include "poiinpd.h"

extern int      kol_strok_na_liste; //Количество строк на стандартном листе
#define         KOLSTSH  6  /*Количество строк в шапке*/

void            uosvos_ithz(const char*,double,double,short,int*,double*,class bsiz_per *data_hz,FILE*,FILE*);
void            uosvos_gsap(short,short,short,int *,int *,int,const char*,short,FILE*);
void            uosvos_hhap(short,short,short,int,int*,int,const char*,short,FILE*);
void uosvos_porgup(char *kodgr,short dk,short mk,short gk,double*,FILE *ff,FILE*);
void uosvos_pogurp_s(short dk,short mk,short gk,short sli,int *kst,const char*,double*,FILE *ff,FILE*);

void uosvosii_start(short dk,short mk,short gk,int viduh,int metr,int metka_sort,
int *kst,const char *podrz,const char *hnaz,const char *hnabyz,const char *kodolz,const char *hauz,const char *shetupoi,const char *innomp,const char *hztz,const char*,int,FILE *ff);

extern short	startgoduos; /*Стартовый год*/

int uosvos(int viduh, //0-налоговый учет 1-бухгалтерский
class spis_oth *oth)
{
class iceb_tu_str naimos("");
time_t          tmm;
FILE		*ff,*ffhpz;
class iceb_tu_str bros("");
static class iceb_tu_str dat1("");
int           kom,kom1;
static class iceb_tu_str podrz(""); /*Код подразделения*/
static class iceb_tu_str kodolz(""); /*Коды ответственных лиц*/
static class iceb_tu_str hnaz("");
static class iceb_tu_str hnabyz("");
static class iceb_tu_str hauz("");
static class iceb_tu_str shetupoi("");
static class iceb_tu_str innomp("");
static class iceb_tu_str hztz("");
static class iceb_tu_str sosob("");
short           dk,mk,gk;
char            imaf[64],imafhpz[64];;
long		kolstr,kolstr1,kolstr3;
SQL_str         row,row1;
char		strsql[1024];
int		pod=0,podz=0;
long		in;
int		metr=0; /*0-по подразделениям 1-по мат.-ответственным*/
double          bs1=0.;
double		s1=0.;
double          ss1=0.;
double		s11=0.;
double		bs11=0.;
double          bss1;
double		osts;
double		bsn=0.,izn=0.;
double		pr1=0.,pr2=0.;
class iceb_tu_str hna1("");
class iceb_tu_str naipod("");
int		sli,kst;
class iceb_tu_str naigrup("");
short		mvsh; /*0-шапку не печатать, если больше печатать*/
short		mvsi; /*0-шифр не печатать*/
int             poddz,kodotl;
int		podrp=0;
int 		kolos=0; //Количество основных средств попавших в отчёт
SQLCURSOR curr;
int             metka_sort=0; //0-сортировка по группам 1-по счетам учёта

VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(hnaz.ravno(),128);
VV.VVOD_SPISOK_add_data(hnabyz.ravno(),128);
VV.VVOD_SPISOK_add_data(podrz.ravno(),128);
VV.VVOD_SPISOK_add_data(kodolz.ravno(),128);
VV.VVOD_SPISOK_add_data(hauz.ravno(),128);
VV.VVOD_SPISOK_add_data(shetupoi.ravno(),128);
VV.VVOD_SPISOK_add_data(innomp.ravno(),128);
VV.VVOD_SPISOK_add_data(hztz.ravno(),128);
VV.VVOD_SPISOK_add_data(sosob.ravno(),2);

VV.VVOD_SPISOK_add_MD(gettext("Дата остатка..(д.м.г)..."));//0
VV.VVOD_SPISOK_add_MD(gettext("Группа нал.учета..(,,).."));//1
VV.VVOD_SPISOK_add_MD(gettext("Группа бух.учета..(,,).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Подразделение (,,)......"));//3
VV.VVOD_SPISOK_add_MD(gettext("Мат.ответственный (,,).."));//4
VV.VVOD_SPISOK_add_MD(gettext("Шифр анал. учета..(,,).."));//5
VV.VVOD_SPISOK_add_MD(gettext("Счёт учета........(,,).."));//6
VV.VVOD_SPISOK_add_MD(gettext("Инвентарный номер.(,,).."));//7
VV.VVOD_SPISOK_add_MD(gettext("Шифр затрат.......(,,).."));//8
VV.VVOD_SPISOK_add_MD(gettext("Состояние объекта.(+/-)."));//9

if(viduh == 0)
 VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка ведомости остаточной стоимости (налоговый учет)"));
if(viduh == 1)
 VV.VVOD_SPISOK_add_ZAG(gettext("Рапечатка ведомости остаточной стоимости (бухгалтерский учет)"));
VV.VVOD_SPISOK_add_ZAG(gettext("Введите данные"));

naz:;

clear();

helstr(LINES-1,0,"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));

hnaz.new_plus(VV.data_ravno(1));

hnabyz.new_plus(VV.data_ravno(2));

podrz.new_plus(VV.data_ravno(3));
kodolz.new_plus(VV.data_ravno(4));
hauz.new_plus(VV.data_ravno(5));
shetupoi.new_plus(VV.data_ravno(6));
innomp.new_plus(VV.data_ravno(7));
hztz.new_plus(VV.data_ravno(8));
sosob.new_plus(VV.data_ravno(9));
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
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//6

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 6:
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
        VV.VVOD_SPISOK_zapis_data(6,hauz.ravno());
        goto naz;

      case 5 :
        vibrek("Uoshz",&hztz,&naim_v);
        VV.VVOD_SPISOK_zapis_data(9,hztz.ravno());
        goto naz;

     }
    goto naz;
    

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
 }


if(rsdat(&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(0),1) != 0)
 {
  iceb_t_soob(gettext("Неправильно введена дата остатка !"));
  goto naz;
 }
 

MENU.VVOD_delete();
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное и нажмите Enter"));

MENU.VVOD_SPISOK_add_MD(gettext("Расчитать по подразделениям"));
MENU.VVOD_SPISOK_add_MD(gettext("Расчитать по мат.-ответственным"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
metr=0;
while(menu3w(stdscr,&MENU,&metr,-1,-1,0) != 0);
if(metr == -1 || metr == 2)
 return(1);

MENU.VVOD_delete();
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное и нажмите Enter"));

MENU.VVOD_SPISOK_add_MD(gettext("Все основные средства"));
MENU.VVOD_SPISOK_add_MD(gettext("Основные средства с нулевой остаточной стоимостью"));
MENU.VVOD_SPISOK_add_MD(gettext("Основные средства с ненулевой остаточной стоимостью"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

int metkaost=0;

while(menu3w(stdscr,&MENU,&metkaost,-1,-1,0) != 0);

if(metkaost == -1 || metkaost == 3)
 return(1);

MENU.VVOD_delete();
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное и нажмите Enter"));
MENU.VVOD_SPISOK_add_MD(gettext("Сортировка по группам"));
MENU.VVOD_SPISOK_add_MD(gettext("Сортировать по счетам учёта"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
while(menu3w(stdscr,&MENU,&metka_sort,-1,-1,0) != 0);
if(metka_sort == -1 || metka_sort == 2)
 return(1);
 
clear();
refresh();
time(&tmm);
printw(" %d.%d.%d%s\n",
dk,mk,gk,
gettext("г."));
printw("%s\n",gettext("Сортируем"));

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
const char *imatmptab={"uosvos"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
shu char(24) not null,\
kmo int not null,\
innom int not null,\
naim char(112) not null,\
pod int not null,\
nomz char(56) not null,\
hna char(32) not null,\
hnaby char(32) not null,\
unique(kmo,innom,pod)) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  
class poiinpdw_data rekin;
kolstr3=kolstr1=0;
class SQLCURSOR cur1;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s\n",row[0],row[1]);
  strzag(LINES-1,0,kolstr,++kolstr1);  
  if(proverka(innomp.ravno(),row[0],0,0) != 0)
    continue;
    
  in=atol(row[0]);

  if(poiin(in,dk,mk,gk,&poddz,&kodotl) != 0) 
     continue; //Произошел расход или на эту дату небыло прихода
  
  if(proverka(podrz.ravno(),poddz,0,0) != 0)
    continue;

  if(proverka(kodolz.ravno(),kodotl,0,0) != 0)
    continue;

  poiinpd(in,mk,gk,&rekin);

  if(viduh == 0 ) //Пропускаем все основные средства если для них амортизазия считается в целом по группе
   if(uosprovgr(rekin.hna.ravno()) == 1)
    continue;

  if(viduh == 0 ) //Налоговый учёт
   {
    if(rekin.soso == 0 || rekin.soso == 3) //амортизация считается
     if(sosob.ravno()[0] == '-')
      continue;
    if(rekin.soso == 1 || rekin.soso == 2) //амортизация не считаетcя
     if(sosob.ravno()[0] == '+')
      continue;
    
   }
  if(viduh == 1 ) //Бух.учёт
   {
    if(rekin.soso == 0 || rekin.soso == 2) //амортизация считается
     if(sosob.ravno()[0] == '-')
      continue;
    if(rekin.soso == 1 || rekin.soso == 3) //амортизация не считаестя
     if(sosob.ravno()[0] == '+')
      continue;
    
   }
  if(proverka(shetupoi.ravno(),rekin.shetu.ravno(),0,0) != 0)
    continue;

  if(proverka(hztz.ravno(),rekin.hzt.ravno(),0,0) != 0)
    continue;

  if(proverka(hnaz.ravno(),rekin.hna.ravno(),0,0) != 0)
    continue;

  if(proverka(hnabyz.ravno(),rekin.hnaby.ravno(),0,0) != 0)
    continue;

  if(proverka(hauz.ravno(),rekin.hau.ravno(),0,0) != 0)
    continue;

  kolstr3++;


  sprintf(strsql,"insert into %s values ('%s',%d,%d,'%s',%d,'%s','%s','%s')",
  imatmptab,
  rekin.shetu.ravno(),kodotl,atoi(row[0]),iceb_tu_sqlfiltr(row[1]),poddz,rekin.nomz.ravno(),rekin.hna.ravno(),rekin.hnaby.ravno());

  sql_zapis(strsql,1,1);    

 }
GDITE(); 

if(metr == 0) //По подразделениям
 {
  if(metka_sort == 0)
   {
    if(viduh == 0) //налоговый учёт
      sprintf(strsql,"select * from %s order by hna asc,pod asc,innom asc",imatmptab);
    if(viduh == 1) //бух. учёт
      sprintf(strsql,"select * from %s order by hnaby asc,pod asc,innom asc",imatmptab);
   }
  if(metka_sort == 1)
      sprintf(strsql,"select * from %s order by shu asc,pod asc,innom asc",imatmptab);
 }
if(metr == 1) //По материально-ответственным
 {
  if(metka_sort == 0)
   {
    if(viduh == 0) //налоговый учёт
      sprintf(strsql,"select * from %s order by hna asc,kmo asc,innom asc",imatmptab);
    if(viduh == 1) //бух.учёт
      sprintf(strsql,"select * from %s order by hnaby asc,kmo asc,innom asc",imatmptab);
   }
  if(metka_sort == 1)
      sprintf(strsql,"select * from %s order by shu asc,kmo asc,innom asc",imatmptab);

 }

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

sprintf(imaf,"vos%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

sprintf(imafhpz,"vosi%d.lst",getpid());

if((ffhpz = fopen(imafhpz,"w")) == NULL)
 {
  error_op_nfil(imafhpz,errno,"");
  return(1);
 }


char imaf_br[64];

sprintf(imaf_br,"vosbr%d.lst",getpid());
FILE *ff_br;

if((ff_br = fopen(imaf_br,"w")) == NULL)
 {
  error_op_nfil(imaf_br,errno,"");
  return(1);
 }


uosvosii_start(dk,mk,gk,viduh,metr,metka_sort,&kst,podrz.ravno(),
hnaz.ravno(),hnabyz.ravno(),kodolz.ravno(),hauz.ravno(),shetupoi.ravno(),innomp.ravno(),hztz.ravno(),sosob.ravno(),metkaost,ff);

uosvosii_start(dk,mk,gk,viduh,metr,metka_sort,&kst,podrz.ravno(),
hnaz.ravno(),hnabyz.ravno(),kodolz.ravno(),hauz.ravno(),shetupoi.ravno(),innomp.ravno(),hztz.ravno(),sosob.ravno(),metkaost,ffhpz);


int kolstr_br=0;
int kol_list_br=1;

uosvosii_start(dk,mk,gk,viduh,metr,metka_sort,&kolstr_br,podrz.ravno(),
hnaz.ravno(),hnabyz.ravno(),kodolz.ravno(),hauz.ravno(),shetupoi.ravno(),innomp.ravno(),hztz.ravno(),sosob.ravno(),metkaost,ff_br);

 
int kstt=0;
uosvos_hhap(dk,mk,gk,1,&kstt,0,"",2,ffhpz);

uosvos_hhap(dk,mk,gk,kol_list_br,&kolstr_br,0,"",2,ff_br);

bs1=bss1=0.;
s1=ss1=0.;
bs11=s11=0.;
mvsi=mvsh=in=0;
sli=1;
podz=kolstr1=0;
class iceb_tu_str hna("");
double iznos_per[2];
double i_iznos_per[2];
double iznos_per_pod[2];
double iznos_per_hna[2];
memset(&i_iznos_per,'\0',sizeof(i_iznos_per));
memset(&iznos_per,'\0',sizeof(iznos_per));
memset(&iznos_per_pod,'\0',sizeof(iznos_per_pod));
memset(&iznos_per_hna,'\0',sizeof(iznos_per_hna));

class bsiz_per data_podr; //Итоги по подразделению
class bsiz_per data_hz;   //Итоги по шифру затрат
class bsiz_per data_oi;   //Общий итог по всему отчёту
class bsiz_per data_shet; //Итог по счёту
class iceb_tu_str nomz("");
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(metka_sort == 0)
   {
    if(viduh == 0) //налоговый учёт
     hna.new_plus(row[6]);
    if(viduh == 1) //бух.учёт
     hna.new_plus(row[7]);
   }
  if(metka_sort == 1)
    hna.new_plus(row[0]);

  if(metr == 0) //По подразделениям
   {
    podrp=pod=atoi(row[4]);
   }
  if(metr == 1) //По материально-ответственным
   {
    pod=atoi(row[1]);
    podrp=atoi(row[4]);
   }
  in=atoi(row[2]);
  naimos.new_plus(row[3]);
  nomz.new_plus(row[5]);

  memset(iznos_per,'\0',sizeof(iznos_per));

  
  class bsiz_data data_per;
  
  bsizp(in,podrp,0,dk,mk,gk,dk,mk,gk,&data_per,NULL);
  
  pr1=pr2=0.;  
  if(viduh == 0)
   {
    pr1=data_per.nu.bs_nah_per; //Балансовая стоимость на начало периода
    pr2=data_per.nu.izmen_iznosa_nah_per+data_per.nu.amort_nah_per;  //Износ на начало периода
      
   }
  
  if(viduh == 1)
   {
    pr1=data_per.bu.bs_nah_per; //Балансовая стоимость на начало периода
    pr2=data_per.bu.izmen_iznosa_nah_per+data_per.bu.amort_nah_per;  //Износ на начало периода
    
   }
    
  
    
  if(fabs(pr1) < 0.01 ) //Значит уже продан или списан или передан в другое подразделение
   {
/***********
    if(in == 7001090)
     { 
      printw("in=%d %d.%d.%d\n",in,dk,mk,gk);
      printw("pr1=%.2f pr2=%.2f\n",pr1,pr2);
      OSTANOV();
     }
*************/
    continue;
   }


  if(podz != pod)
   {
    if(podz != 0)
     {
      bros.new_plus(podz);
      uosvos_ithz(bros.ravno(),bss1,ss1,0,&kst,iznos_per_pod,&data_podr,ff,ffhpz);
      memset(&iznos_per_pod,'\0',sizeof(iznos_per_pod));
      bss1=0.;
      ss1=0.;
     }

    if(metr == 0)
     {
      /*Читаем наименование подразделения*/
      sprintf(strsql,"select naik from Uospod where kod=%d",pod);
      if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
       {
        beep();
        printw("%s %d !\n",gettext("Не найден код подразделения"),pod);
        fprintf(ff,"%s %d !\n",gettext("Не найден код подразделения"),pod);
        OSTANOV();
        naipod.new_plus("");
       }
      else
        naipod.new_plus(row1[0]);
     }

    if(metr == 1)
     {
      /*Читаем мат.-ответственного*/
      sprintf(strsql,"select naik from Uosol where kod=%d",pod);
      if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
       {
        beep();
        printw("%s %d !\n",gettext("Не найден мат.-ответственный"),pod);
        fprintf(ff,"%s %d !\n",gettext("Не найден мат.-ответственный"),pod);
        OSTANOV();
        naipod.new_plus("");
       }
      else
        naipod.new_plus(row1[0]);
     }

    mvsh++;
             
    podz=pod;
    if(metr == 0)
     printw("\n%s - %d/%s\n",gettext("Подразделение"),podz,naipod.ravno());
    if(metr == 1)
     printw("\n%s - %d/%s\n",gettext("Мат.-ответственный"),podz,naipod.ravno());
   }

  if(SRAV(hna1.ravno(),hna.ravno(),0) != 0)
   {
    if(hna1.ravno()[0] != '\0')
     {
      if(metka_sort == 0)
       uosvos_ithz(hna1.ravno(),bs1,s1,1,&kst,iznos_per_hna,&data_hz,ff,ffhpz);
      if(metka_sort == 1)
       uosvos_ithz(hna1.ravno(),bs1,s1,3,&kst,iznos_per_hna,&data_shet,ff,ffhpz);
      memset(&iznos_per_hna,'\0',sizeof(iznos_per_hna));
      bs1=s1=0.;
     }
    
    if(metka_sort == 0)
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
          fprintf(ff,"%s %s !\n",gettext("Не найдено группу"),hna.ravno());
          OSTANOV();
          naigrup.new_plus("");
         }
        else
          naigrup.new_plus(row1[0]);
       }
      else
       naigrup.new_plus(gettext("Не введена в карточку группа"));
      printw("\n%s %s \"%s\"\n",
      gettext("Группа"),hna.ravno(),naigrup.ravno());
     }
    if(metka_sort == 1)
     {
      sprintf(strsql,"select nais from Plansh where ns='%s'",hna.ravno());
      if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
        naigrup.new_plus(row1[0]);
      else
        naigrup.new_plus("");
     
      printw("\n%s %s \"%s\"\n",
      gettext("Счёт учёта"),hna.ravno(),naigrup.ravno());
     }
    mvsi++;


    hna1.new_plus(hna.ravno());

   }
  if(mvsh > 0 || mvsi > 0) 
   {  
    if(kst > kol_strok_na_liste-KOLSTSH-5)
     {
      fprintf(ff,"\f");
      sli++;
      kst=0;
     }
    if(mvsi > 0)
     {
      if(metka_sort == 0)
       {
        fprintf(ff,"\n%s %s \"%s\"\n",gettext("Группа"),hna.ravno(),naigrup.ravno());
       }
      if(metka_sort == 1)
       {
        fprintf(ff,"\n%s %s \"%s\"\n",
        gettext("Счёт учёта"),hna.ravno(),naigrup.ravno());
       }
      mvsi=0;
      kst+=2;
     } 
    uosvos_hhap(dk,mk,gk,sli,&kst,pod,naipod.ravno(),metr,ff);
    mvsh=0;
   }

  if(viduh == 0)
   {
    data_podr.plus(&data_per.nu);
    data_oi.plus(&data_per.nu);
    data_hz.plus(&data_per.nu);
    data_shet.plus(&data_per.nu);
   }
  if(viduh == 1)
   {
    data_podr.plus(&data_per.bu);
    data_oi.plus(&data_per.bu);
    data_hz.plus(&data_per.bu);
    data_shet.plus(&data_per.bu);
   }
  izn=pr2;
  bsn=pr1;
  osts=bsn-izn;

  if(osts >  0.009 && metkaost == 1)
   continue;

  if(osts ==  0. && metkaost == 2)
   continue;

  bss1+=bsn;
  ss1+=izn;
  
  bs1+=bsn;
  s1+=izn;
  
  bs11+=bsn;
  s11+=izn;

  if(fabs(bsn+izn+osts) > 0.009)
   {

    uosvos_gsap(dk,mk,gk,&sli,&kst,pod,naipod.ravno(),metr,ff);
    fprintf(ff,"%11ld %-*.*s %-*.*s %12.2f %12.2f %12.2f\n",
    in,
    iceb_tu_kolbait(30,naimos.ravno()),iceb_tu_kolbait(30,naimos.ravno()),naimos.ravno(),
    iceb_tu_kolbait(10,nomz.ravno()),iceb_tu_kolbait(10,nomz.ravno()),nomz.ravno(),
    bsn,izn,osts);

    uosvos_gsap(dk,mk,gk,&kol_list_br,&kolstr_br,0,"",2,ff_br);
    
    fprintf(ff_br,"%11ld %-*.*s %-*.*s %12.2f %12.2f %12.2f\n",
    in,
    iceb_tu_kolbait(30,naimos.ravno()),iceb_tu_kolbait(30,naimos.ravno()),naimos.ravno(),
    iceb_tu_kolbait(10,nomz.ravno()),iceb_tu_kolbait(10,nomz.ravno()),nomz.ravno(),
    bsn,izn,osts);

    if(iceb_tu_strlen(naimos.ravno()) > 30)
     {
    
      uosvos_gsap(dk,mk,gk,&sli,&kst,pod,naipod.ravno(),metr,ff);
      fprintf(ff,"%11s %s\n","",iceb_tu_adrsimv(30,naimos.ravno()));
  
      uosvos_gsap(dk,mk,gk,&kol_list_br,&kolstr_br,0,"",2,ff_br);
      fprintf(ff_br,"%11s %s\n","",iceb_tu_adrsimv(30,naimos.ravno()));
     }
    kolos++;
   }
 }

GDITE();


bros.new_plus(pod);
uosvos_ithz(bros.ravno(),bss1,ss1,0,&kst,iznos_per_pod,&data_podr,ff,ffhpz);

if(metka_sort == 0)
  uosvos_ithz(hna1.ravno(),bs1,s1,1,&kst,iznos_per_hna,&data_hz,ff,ffhpz);
if(metka_sort == 1)
  uosvos_ithz(hna1.ravno(),bs1,s1,3,&kst,iznos_per_hna,&data_shet,ff,ffhpz);

/*Итог Общий*/
uosvos_ithz(" ",bs11,s11,2,&kst,i_iznos_per,&data_oi,ff,ffhpz);
uosvos_ithz(" ",bs11,s11,2,&kst,i_iznos_per,&data_oi,ff_br,NULL);


if(viduh == 0) //распечатка по группам с общегрупповой амортизацией
 {
  double suma_pogr[3];
  memset(suma_pogr,'\0',sizeof(suma_pogr));

  uosvos_pogurp_s(dk,mk,gk,sli,&kst,hnaz.ravno(),suma_pogr,ff,ffhpz);

  suma_pogr[0]+=bs11;
  suma_pogr[1]+=s11;
  suma_pogr[2]+=bs11-s11;
  
  char str[200];
  memset(str,'\0',sizeof(str));
  memset(str,'-',93);

  fprintf(ff,"%s\n",str);
  fprintf(ff,"\
%*s %12.2f %12.2f %12.2f\n",
  iceb_tu_kolbait(53,gettext("Общий итог")),gettext("Общий итог"),
  suma_pogr[0],
  suma_pogr[1],
  suma_pogr[2]);


  fprintf(ffhpz,"%s\n",str);
  fprintf(ffhpz,"\
%*s %12.2f %12.2f %12.2f\n",
  iceb_tu_kolbait(53,gettext("Общий итог")),gettext("Общий итог"),
  suma_pogr[0],
  suma_pogr[1],
  suma_pogr[2]);

 }





printw("\n%s:%d\n",gettext("Количество основных средств"),kolos);
fprintf(ff,"\n%s:%d\n",gettext("Количество основных средств"),kolos);
fprintf(ffhpz,"\n%s:%d\n",gettext("Количество основных средств"),kolos);

podpis(ff);
podpis(ffhpz);
podpis(ff_br);

fclose(ff);
fclose(ff_br);
fclose(ffhpz);

printw_vr(tmm);




oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Ведомость остаточной стоимости с сортировкой"));
oth->spis_imaf.plus(imafhpz);
oth->spis_naim.plus(gettext("Общие итоги"));
oth->spis_imaf.plus(imaf_br);
oth->spis_naim.plus(gettext("Ведомость остаточной стоимости без сортировки"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),1);

OSTANOV();

return(0);

}
/****************/
/*Итого по шифру и подразделению для износа*/
/****************/
void uosvos_ithz(const char *sh,double k1,double s1,
short mt,  //0-по подразделению 1- по шифру 2-общий итог 3-по счету учёта
int *kst,
double *iznos_per,
class bsiz_per *data_i,
FILE *ff,FILE *ffhpz)
{
class iceb_tu_str bros("");
char		str[300];

memset(str,'\0',sizeof(str));
memset(str,'-',93);

if(mt == 2)
 {
  bros.new_plus(gettext("Общий итог"));
 }

if(mt == 1)
 {
  if(sh[0] == '\0')
   return;
//  sprintf(bros,"%s %s",gettext("Итого по шифру"),sh);
  bros.new_plus(gettext("Итого по шифру"));
  bros.plus(" ",sh);
  if(ffhpz != NULL)
   fprintf(ffhpz,"%s\n\
%11s %-*s %10s %12.2f %12.2f %12.2f\n",
str," ",
iceb_tu_kolbait(30,bros.ravno()),bros.ravno(),
" ",k1,s1,k1-s1);
 }

if(mt == 0)
 {
  if(k1+s1+(k1-s1) < 0.0001)
   return;
//  sprintf(bros,"%s %s",gettext("Итого по подразделению"),sh);
  bros.new_plus(gettext("Итого по подразделению"));
  bros.plus(" ",sh);
  
 }

if(mt == 3)
 {
  if(k1+s1+(k1-s1) < 0.0001)
   return;
//  sprintf(bros,"%s %s",gettext("Итого по счёту"),sh);
  bros.new_plus(gettext("Итого по счёту"));
  bros.plus(" ",sh);
 }

fprintf(ff,"%s\n\
%11s %-*s %10s %12.2f %12.2f %12.2f\n",str," ",iceb_tu_kolbait(30,bros.ravno()),bros.ravno()," ",k1,s1,k1-s1);

 
if(mt == 2 || mt == 3)
 if(ffhpz != NULL)
  fprintf(ffhpz,"%s\n\
%11s %-*s %10s %12.2f %12.2f %12.2f\n",str," ",iceb_tu_kolbait(30,bros.ravno()),bros.ravno()," ",k1,s1,k1-s1);

*kst=*kst+2;

printw("\n%s:\n\
%s %12.2f %12.2f %12.2f\n",
bros.ravno(),
gettext("Баланс. ст./Износ/Ост.ст.:"),
k1,s1,k1-s1);

data_i->clear_data(); //обнуляем массив

}

/*******/
/*Счетчик строк*/
/*******/
void uosvos_gsap(short dk,short mk,short gk,
int *sl,  //Счетчик листов
int *kst, //Счетчик строк
int skl,const char *nai,
short mt,   //0-по подразделению 1-по материально-ответств
FILE *ff)
{

*kst=*kst+1;

if(*kst <= kol_strok_na_liste)
 return;

*sl+=1;
*kst=1;
fprintf(ff,"\f");

uosvos_hhap(dk,mk,gk,*sl,kst,skl,nai,mt,ff);


}

/********/
/*Шапка*/
/*******/

void uosvos_hhap(short dk,short mk,short gk,
int sl, //Счетчик листов
int *kst,  //Счетчик строк
int skl,
const char *nai,
short mt,  //0-по подразделению 1-по материально-ответств 
FILE *ff)
{
char		str[300];
char		bros[512];

if(mt == 0)
 sprintf(bros,"%s %d %s",gettext("Подразделение"),skl,nai);

if(mt == 1)
 sprintf(bros,"%s %d %s",gettext("Мат.-ответственный"),skl,nai);

if(mt == 2)
 sprintf(bros,"%s",nai);

memset(str,'\0',sizeof(str));
memset(str,'-',93);

fprintf(ff,"\
%-*s %s N%d\n\%s\n",
iceb_tu_kolbait(71,bros),bros,
gettext("Лист"),
sl,str);

fprintf(ff,gettext("\
Инвентарный|     Наименование             |          | Балансовая |   Износ    | Остаточная |\n\
  номер    |                              |  Номер   |стоимость на|    на      |стоимость на|\n\
	   |                              |          |%02d.%02d.%dг.|%02d.%02d.%dг.|%02d.%02d.%dг.|\n"),
dk,mk,gk,dk,mk,gk,dk,mk,gk);

fprintf(ff,"%s\n",str);

 
*kst+=KOLSTSH;

}
/********************************/
/*Распечатка в целом по группе по списку групп*/
/*******************************/

void uosvos_pogurp_s(short dk,short mk,short gk,
short sli,
int *kst,
const char *hnaz,
double *suma_pogr,
FILE *ff,FILE *ffhpz)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select kod from Uosgrup where ta=1");
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
  return;  
fprintf(ff,"\n\n");
uosvos_hhap(dk,mk,gk,sli,kst,0,"Группы с амортизацией в целом по группе",2,ff);

fprintf(ffhpz,"\n\n");
uosvos_hhap(dk,mk,gk,sli,kst,0,"Группы с амортизацией в целом по группе",2,ffhpz);

while(cur.read_cursor(&row) != 0)
 {
  if(proverka(hnaz,row[0],0,0) != 0)
   continue;
  uosvos_porgup(row[0],dk,mk,gk,suma_pogr,ff,ffhpz);
 }

char str[200];
memset(str,'\0',sizeof(str));
memset(str,'-',93);

fprintf(ff,"%s\n",str);
fprintf(ff,"\
%*s %12.2f %12.2f %12.2f\n",
iceb_tu_kolbait(53,gettext("Итого")),gettext("Итого"),
suma_pogr[0],
suma_pogr[1],
suma_pogr[2]);

fprintf(ffhpz,"%s\n",str);
fprintf(ffhpz,"\
%*s %12.2f %12.2f %12.2f\n",
iceb_tu_kolbait(53,gettext("Итого")),gettext("Итого"),
suma_pogr[0],
suma_pogr[1],
suma_pogr[2]);
 


}
/****************/
/*Распечатка одной группы налогового учёта для которой амортизация считается в целом по группе*/
/******************************************/

void uosvos_porgup(char *kodgr,
short dk,short mk,short gk,
double *suma_pogr,
FILE *ff,FILE *ffhpz)
{
class iceb_tu_str naigrup("");
SQL_str row;
SQLCURSOR cur;
char strsql[512];


sprintf(strsql,"select naik from Uosgrup where kod='%s'",kodgr);
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 naigrup.new_plus(row[0]);

  
class bsiz_data data;
bsiz23p(kodgr,dk,mk,gk,dk,mk,gk,&data);

fprintf(ff,"\
%11s %-*.*s %-10.10s %12.2f %12.2f %12.2f\n",
kodgr,
iceb_tu_kolbait(30,naigrup.ravno()),iceb_tu_kolbait(30,naigrup.ravno()),naigrup.ravno(),
"",
data.nu.bs_nah_per,
data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per,
data.nu.bs_nah_per-(data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per));




fprintf(ffhpz,"\
%11s %-*.*s %-10.10s %12.2f %12.2f %12.2f\n",
kodgr,
iceb_tu_kolbait(30,naigrup.ravno()),iceb_tu_kolbait(30,naigrup.ravno()),naigrup.ravno(),
"",
data.nu.bs_nah_per,
data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per,
data.nu.bs_nah_per-(data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per));


suma_pogr[0]+=data.nu.bs_nah_per;
suma_pogr[1]+=data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per;
suma_pogr[2]+=data.nu.bs_nah_per-(data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per);

}
/**********************************/
/*Начало отчётов*/
/***********************************/

void uosvosii_start(short dk,short mk,short gk,
int viduh,
int metr,
int metka_sort,
int *kst,
const char *podrz,
const char *hnaz,
const char *hnabyz,
const char *kodolz,
const char *hauz,
const char *shetupoi,
const char *innomp,
const char *hztz,
const char *sosob,
int metkaost,
FILE *ff)
{

*kst=5;

iceb_t_zagolov(gettext("Ведомость остаточной стоимости основных средств"),0,0,0,dk,mk,gk,ff);

*kst+=1;

if(viduh == 0)
 {
  fprintf(ff,"%s.\n",gettext("Налоговый учёт"));
 }
if(viduh == 1)
 {
  fprintf(ff,"%s.\n",gettext("Бухгалтерский учёт"));
 }

int klst=0;
if(podrz[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Подразделение"),podrz);
  printcod(ff,"Uospod","kod","naik",0,podrz,&klst);
  *kst+=klst+1;
 }

if(hnaz[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Группа налогового учёта"),hnaz);
  printcod(ff,"Uosgrup","kod","naik",1,hnaz,&klst);
  *kst+=klst+1;
 }
if(hnabyz[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Группа бухгалтерского учёта"),hnabyz);
  printcod(ff,"Uosgrup1","kod","naik",1,hnabyz,&klst);
  *kst+=klst+1;
 }
if(kodolz[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Мат.-ответственный"),kodolz);
  printcod(ff,"Uosol","kod","naik",1,kodolz,&klst);
  *kst+=klst+1;
 }
if(hauz[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Шифр аналитического учета"),hauz);
  printcod(ff,"Uoshau","kod","naik",0,hauz,&klst);
  *kst+=klst+1;
 }
if(shetupoi[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Счёт"),shetupoi);
  *kst+=1;
 }

if(innomp[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Инвентарный номер"),innomp);
  *kst+=1;
 }

if(hztz[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Шифр затрат"),hztz);
  *kst+=1;
 }

if(sosob[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Состояние объекта"),sosob);
  *kst+=1;
 }

if(metkaost == 1)
 {
  fprintf(ff,"%s\n",gettext("Основные средства с нулевою остаточной стоимостью"));
  *kst+=1;
 }
if(metkaost == 2)
 {
  fprintf(ff,"%s\n",gettext("Основные средства с ненулевой остаточной стоимостью"));
  *kst+=1;
 }
*kst+=1;
if(metr == 0)
 fprintf(ff,"%s\n",gettext("По подразделениям"));
if(metr == 1)
 fprintf(ff,"%s\n",gettext("По материально-ответственным"));

*kst+=1;
if(metka_sort == 0)
 fprintf(ff,"%s\n",gettext("Сортировка по группам"));
if(metka_sort == 1)
 fprintf(ff,"%s\n",gettext("Сортировать по счетам учёта"));

}
