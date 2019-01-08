/* $Id: glkni.c,v 5.115 2014/07/31 07:09:16 sasa Exp $ */
/*08.03.2016    06.12.1993      Белых А.И.      glkni.c
Программа получения главной книги
*/
#include        <math.h>
#include        <errno.h>
#include        "buhg.h"
#define MAX_KOL_STR 77 //количество сторок которое влезет на лист (64 строки)при уменьшении межстрочного растояния
                       //для ориентации портрет
#define MAX_KOL_STR_L 56 //количество сторок которое влезет на лист (47 строки)при уменьшении межстрочного растояния
                       //для ориентации ланшафт
                       
void shet_gk_p1(short,short,short,short,short,short,int *,int *,struct tm *bf,FILE*);
void shet_ob_p1(short,short,short,short,short,short,int *,int *,struct tm *bf,FILE*);
void shet_gk_l1(short,short,short,short,short,short,int *,int *,struct tm *bf,FILE*);
void shet_ob_l1(short,short,short,short,short,short,int *,int *,struct tm *bf,FILE*);
void gklni_gk_slo1(short dn,short mn,short gn,short dk,short mk,short gk,int *sl,int *kst,struct tm *bf,FILE *ff);

void ras1n(short dn,short mn,short gn,short dk,short mk,short gk,int *sli,int *kst,int *sli1,int *kst1,
short i,const char *ns,const char *nash,class GLKTMP *PROM,double *dns,double *kns,double *dpp,double *kpp,double *di,
double *ki,struct tm *bf,int*,int*,int*,int*,int,FILE *ff,FILE *ff1,FILE *ff_gk_l,FILE *ff_ob_l,FILE *ffcsv);

void ras2n(short dn,short mn,short gn,short dk,short mk,short gk,int *sli,int *kst,int *sli1,int *kst1,
int nomer_sheta,const char *sh,const char *nash,GLKTMP *PROM,double *dns,double *kns,double *dpp,
double *kpp,double *di,double *ki,struct tm *bf,int*,int*,int*,int*,int,
class SPISOK *ns_kontr,
class masiv_din_double *ns_kontr_debs,
class masiv_din_double *ns_kontr_kres,
class masiv_din_double *ns_kontr_debp,
class masiv_din_double *ns_kontr_krep,
FILE *ff,FILE *ff1,FILE *ff_gk_l,FILE *ff_ob_l,FILE *ffcsv);

void gsao1_s1(int *sl,int *kst,struct tm *bf,FILE *ff1);
void  gsap_s1(short dn,short mn,short gn,short dk,short mk,short gk,int *sl,int *kst,struct tm *bf,FILE *ff);
void  balans_f1(int,short,short,short,char *imaf,GLKTMP *PROM);
void  balans_f2(int,short,short,short,char *imaf,GLKTMP *PROM);
void ras2n_tab(short dn,short mn,short gn,short dk,short mk,short gk,int *sli,int *kst,int *sli1,int *kst1,int nomer_sheta,const char *sh,const char *nash,GLKTMP *PROM,double *dns,double *kns,double *dpp,double *kpp,double *di,double *ki,struct tm *bf,int *kollist_gk_l,int *kolstr_gk_l,int *kollist_ob_l,int *kolstr_ob_l,int metka_naim_shet,const char *imatabl,FILE *ff,FILE *ff1,FILE *ff_gk_l,FILE *ff_ob_l,FILE *ffcsv);
extern short	startgodb; /*Стартовый год*/
extern char     *sbshet; //Список бюджетных счетов
extern int kol_strok_na_liste;
extern int kol_strok_na_liste_l;

int glkni(class spis_oth *oth)
{
FILE            *ff,*ff1;
time_t          tmm;
time_t          tmmn;
int             i,kom,sb;
class iceb_tu_str sh("");
char            imaf[64],imaf3[64];
char imaf_csv[64];
static iceb_tu_str sheta("");
static iceb_tu_str dat1("");
static iceb_tu_str dat2("");
static iceb_tu_str kod_subbal("");
SQL_str         row;
char		strsql[512];
class iceb_tu_str nais("");
int		X,Y;
int		vids;
int 		saldo=0;
int             metkabd=0;
double          dns,kns,dpp,kpp;
double          di,ki;
struct  tm      *bf;
short           dn,mn,gn;
short           dk,mk,gk;
int             kst,sli;
int             kst1,sli1;
int metka_sr=0; /*метка способа расчёта 0-с применением списков 1-с применением временной таблицы*/

int kom1=0;

class VVOD MENU(3);
class VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Введите даты для расчёта главной книги"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт..........(,,)..."));
VV.VVOD_SPISOK_add_MD(gettext("Код суббаланса..(,,)."));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(sheta.ravno(),124);
VV.VVOD_SPISOK_add_data(kod_subbal.ravno(),124);

naz:;

clear();
if(sbshet != NULL)
 printw("%s",sbshet);
 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F5",gettext("настройка"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
dat2.new_plus(VV.VVOD_SPISOK_return_data(1));
sheta.new_plus(VV.VVOD_SPISOK_return_data(2));
kod_subbal.new_plus(VV.VVOD_SPISOK_return_data(3));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("buhg5_1.txt");
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case SFK2:
     metka_sr=0;
     break;

  case FK2:
  case PLU:
     metka_sr=1;
     break;


  case FK3:
    MENU.VVOD_delete();

    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка кодов суббалансов"));
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
        vibrek("Plansh",&sheta);
       
        VV.data_plus(2,sheta.ravno());
        break;

      case 1 :
        vibrek("Glksubbal",&kod_subbal);
        VV.data_plus(3,kod_subbal.ravno());
                 
        break;
     }
    goto naz;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  case FK5:
    MENU.VVOD_delete();

    MENU.VVOD_SPISOK_add_MD(gettext("Настройка расчёта формы 1"));
    MENU.VVOD_SPISOK_add_MD(gettext("Настройка расчёта формы 2"));
    MENU.VVOD_SPISOK_add_MD(gettext("Бланк формы 1"));
    MENU.VVOD_SPISOK_add_MD(gettext("Бланк формы 2"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 4:
      case -1:
       goto naz;
    
      case 0 :
        iceb_redfil("glk_balans_n.alx",0);
        break;
      case 1 :
        iceb_redfil("glk_balans_n2.alx",0);
        break;
      case 2 :
        iceb_redfil("glk_balans.alx",0);
        break;
      case 3 :
        iceb_redfil("glk_balans2.alx",0);
        break;
     }
    goto naz;

    

  default:
     goto naz;
 }


if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
 goto naz;

clearstr(LINES-1,0);



MENU.VVOD_delete();


if(sbshet != NULL)
 {
  MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите режим"));

  MENU.VVOD_SPISOK_add_MD(gettext("Расчитать хозрасчёт"));
  MENU.VVOD_SPISOK_add_MD(gettext("Расчитать бюджет"));
  MENU.VVOD_SPISOK_add_MD(gettext("Расчитать сводный баланс"));
  MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

  while(menu3w(stdscr,&MENU,&metkabd,-1,-1,0) != 0);

  if(metkabd == -1 || metkabd == 3)
   return(1);
 }

MENU.VVOD_delete();
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите режим"));

MENU.VVOD_SPISOK_add_MD(gettext("Расчитать по субсчетам"));
MENU.VVOD_SPISOK_add_MD(gettext("Расчитать по счетам"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

sb=0;
while(menu3w(stdscr,&MENU,&sb,-1,-1,0) != 0);
 
if(sb == -1 || sb == 2)
 return(1);

if(sb == 0)
 sb=1; //По субсчетам
else
 sb=0; //По счетам

GDITE();

time(&tmmn);
clear();

printw(gettext("Расчёт главной книги за период с %d.%d.%dг. по %d.%d.%dг.\n"),
dn,mn,gn,dk,mk,gk);

printw("Количество строк на листе в портретной ориентации:%d\n\
Количество строк на листе в ландшафтной ориентации:%d\n",
kol_strok_na_liste,kol_strok_na_liste_l);

int metka_naim_shet=0;

if(iceb_t_poldan("Печатать наименование счёта корреспондента в Главной книге",strsql,"nastrb.alx") == 0)
 if(SRAV("Вкл",strsql,1) == 0)
  metka_naim_shet=1;


if(sbshet != NULL)
 {
  if(metkabd == 0)
   printw("%s\n",gettext("Хозрасчёт"));
  if(metkabd == 1)
   printw("%s\n",gettext("Бюджет"));
   
 }



GLKTMP PROM;
sprintf(imaf,"g%d-%d_%d.lst",mn,mk,getpid());
if(sb == 1)
    sprintf(imaf,"gs%d-%d_%d.lst",mn,mk,getpid());
sprintf(imaf3,"ob%d-%d_%d.lst",mn,mk,getpid());
FILE *ff_gk_l;
FILE *ff_ob_l;

char imaf_gk_l[64];
char imaf_ob_l[64];
sprintf(imaf_gk_l,"l_g%d.lst",getpid());
sprintf(imaf_ob_l,"l_ob%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
if((ff1 = fopen(imaf3,"w")) == NULL)
 {
  error_op_nfil(imaf3,errno,"");
  return(1);
 }

if((ff_gk_l = fopen(imaf_gk_l,"w")) == NULL)
 {
  error_op_nfil(imaf_gk_l,errno,"");
  return(1);
 }

if((ff_ob_l = fopen(imaf_ob_l,"w")) == NULL)
 {
  error_op_nfil(imaf_ob_l,errno,"");
  return(1);
 }

sprintf(imaf_csv,"gkcsv%d.csv",getpid());
class iceb_fopen ffcsv;
if(ffcsv.start(imaf_csv,"w") != 0)
 return(1);

bf=localtime(&tmmn);

fprintf(ffcsv.ff,"|%s|\n",iceb_t_get_pnk("00",0));
fprintf(ffcsv.ff,"|%s %s %02d.%02d.%d %s %s %02d.%02d.%d %s|\n",gettext("Главная книга"),gettext("за период с"),dn,mn,gn,gettext("г."),gettext("по"),dk,mk,gk,gettext("г."));
fprintf(ffcsv.ff,"|%s %d.%d.%d  %s - %d:%d|\n",  gettext("По состоянию на"),  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,  gettext("Время"),  bf->tm_hour,bf->tm_min);

fprintf(ffcsv.ff,"%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
gettext("Счёт"),
gettext("Наименование счёта"),
gettext("Дебет"),
gettext("Кредит"),
gettext("Счёт корреспондент"),
gettext("Дебет"),
gettext("Кредит"),
gettext("Дебет"),
gettext("Кредит"));

short godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;

printw("%s:%d\n",gettext("Стаpтовый год"),godn);

printw(gettext("Формируем массив счетов\n"));
refresh();
int msrv=0;
if(sb == 0)
 msrv=1;
//printw("msrw=%d\n",msrv);
//OSTANOV();
/*************************/
/*Формируем массив счетов*/
/*************************/
SQLCURSOR cur;
SQLCURSOR cur1;
int kolstr=0;
sprintf(strsql,"select ns,vids,saldo from Plansh where stat=0 order by ns asc");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не введён план счетов !"));
  return(1);
 }

int pozz=0;
while(cur.read_cursor(&row) != 0)
 {
  pozz++;

//  printw("%s %s %s\n",row[0],row[1],row[2]);
//  refresh();
//  getch();
  
  if(atoi(row[1]) == 1 && sb == 0)
    continue;
  sh.new_plus(row[0]);
  if(sb == 1 && atoi(row[1]) == 0 && pozz < kolstr-1)
   {
    cur.read_cursor(&row);
    /*Если нет субсчетов запоминаем счет*/
    if(SRAV(sh.ravno(),row[0],1) != 0)
     {
      cur.poz_cursor(pozz-1);
      cur.read_cursor(&row);
     }
    else
     pozz++;
   }
  PROM.masf.plus(row[0]);

  if(atoi(row[2]) == 3)
     PROM.msr.plus(row[0]);
 }
int ks=PROM.masf.kolih();
printw("%s: %d\n",gettext("Количество счетов"),ks);
/*********
printw("%s:",gettext("Счета"));
for(int ish=0; ish < PROM.masf.kolih() ; ish++)
  printw("%s ",PROM.masf.ravno(ish));

printw("\n");
****************/
refresh();

class iceb_t_tmptab tabtmp;
const char *imatmptab={"glkni"};
char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
shet char(32) not null DEFAULT '',\
kod_kontr char(32) not null DEFAULT '',\
debs double(16,2) not null DEFAULT 0.,\
kres double(16,2) not null DEFAULT 0.,\
debp double(16,2) not null DEFAULT 0.,\
krep double(16,2) not null DEFAULT 0.,\
unique(shet,kod_kontr)) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  


int ll1=ks*ks; /*Шахматка для заполнения*/
PROM.ssd.make_class(ks);
PROM.ssk.make_class(ks);
PROM.deb_end.make_class(ks);
PROM.kre_end.make_class(ks);
PROM.sl.make_class(ll1);
PROM.pe.make_class(ll1);

int ksrs=PROM.msr.kolih();
printw("%s:%d\n",gettext("Количество счетов с развернутым сальдо"),ksrs);
/*создаём список контрагентов*/

class SPISOK ns_kontr[ksrs];
class masiv_din_double ns_kontr_debs[ksrs];
class masiv_din_double ns_kontr_kres[ksrs];
class masiv_din_double ns_kontr_debp[ksrs];
class masiv_din_double ns_kontr_krep[ksrs];

/***********************************/
/*Узнаем стартовые сальдо по счетам*/
/***********************************/
printw("%s\n",gettext("Вычисляем стартовые сальдо по счетам"));
refresh();
int nomer_sheta_srs=0;
int nom_k=0;
double deb,kre;
for(i=0; i < ks ; i++)
 {
  sh.new_plus(PROM.masf.ravno(i));
  sprintf(strsql,"select saldo,vids from Plansh where ns='%s'",sh.ravno());
  
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,gettext("Не найден счёт %s в плане счетов !"),sh.ravno());
    iceb_t_soob(strsql);
    continue;
   }

  int ts=0;
  if(atoi(row[0]) == 3)
    ts=1;
  vids=atoi(row[1]);
  
  if(vids == 0) /*Счёт*/
   sprintf(strsql,"select kodkon,deb,kre from Saldo where kkk='%d' and gs=%d \
and ns like '%s%%'",ts,godn,sh.ravno());
  if(vids == 1) /*Субсчёт*/
   sprintf(strsql,"select kodkon,deb,kre from Saldo where kkk='%d' and gs=%d \
and ns='%s'",ts,godn,sh.ravno());

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
      msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

  if(kolstr <= 0)
     continue;

  while(cur.read_cursor(&row) != 0)
   {
    deb=atof(row[1]);
    kre=atof(row[2]);

    if(row[0][0] != '\0')
     {
      
      if(metka_sr == 0)
      if((nomer_sheta_srs=PROM.msr.find(sh.ravno(),msrv)) >= 0)
       {
        if((nom_k=ns_kontr[nomer_sheta_srs].find_r(row[0])) < 0)
         {
          ns_kontr[nomer_sheta_srs].plus(row[0]);
         }
        ns_kontr_debs[nomer_sheta_srs].plus(deb,nom_k);
        ns_kontr_kres[nomer_sheta_srs].plus(kre,nom_k);
        ns_kontr_debp[nomer_sheta_srs].plus(0.,nom_k);
        ns_kontr_krep[nomer_sheta_srs].plus(0.,nom_k);
       }        
      if(metka_sr == 1)
       {
//        printw("%s-%6s %10s %10s\n",sh.ravno(),row[0],row[1],row[2]);
        glkni_zvt(0,sh.ravno(),row[0],deb,kre,imatmptab);
       }
     }
    PROM.ssd.plus(deb,i);
    PROM.ssk.plus(kre,i);
   }
/*  
  printw("%s %.2f %.2f\n",sh,ssd[i],ssk[i]);
  getch();
*/
  if(ts == 0)  /*Счета с не развернутым сальдо*/
   {
    if(PROM.ssd.ravno(i) >= PROM.ssk.ravno(i))
     {
      PROM.ssd.plus(PROM.ssk.ravno(i)*-1,i);
      PROM.ssk.new_plus(0.,i);
     }
    else
     {
      PROM.ssk.plus(PROM.ssd.ravno(i)*-1,i);
      PROM.ssd.new_plus(0.,i);
     }
   }
 }
//OSTANOV();

/******************************************/
/*Просматриваем записи и заполняем массивы*/
/******************************************/

printw("%s\n",gettext("Просматриваем проводки, заполняем массивы"));
GDITE();
sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre from Prov \
where val=0 and datp >= '%d-%02d-%02d' and \
datp <= '%d-%02d-%02d'",godn,1,1,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(2);
 }
if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено проводок !"));
 }
else
 {
  double cel=kolstr;
  double drob=0.;

  drob=modf(cel/2.,&drob);
  if(drob > 0.000001)
   {
    VVOD SOOB(1);
    sprintf(strsql,"%s !!!",gettext("Внимание"));
    SOOB.VVOD_SPISOK_add_MD(strsql);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не парное количество проводок."));
    SOOB.VVOD_SPISOK_add_MD(gettext("Проверте базу данных на логическую целосность."));
    soobshw(&SOOB,stdscr,-1,-1,0,1);
   }
 }
printw("%s:%d\n",gettext("Количество проводок"),kolstr);
refresh();

int mpe=0;
short den,mes,god;
int kolstr1=0;
class iceb_tu_str shk("");
double sm=0.;
int i1=0;
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  rsdat(&den,&mes,&god,row[0],2);
  deb=atof(row[4]);   
  kre=atof(row[5]);
  
  if(sravmydat(den,mes,god,dn,mn,gn)  < 0) /*до периода*/
   {
     mpe=0;
   }

  if(sravmydat(den,mes,god,dn,mn,gn)  >= 0) /*период*/
  if(sravmydat(den,mes,god,dk,mk,gk)  <= 0)
   {
    mpe=1;
   }

  if(row[3][0] != '\0')
   {
    if(metka_sr == 0)
    if((nomer_sheta_srs=PROM.msr.find(row[1],msrv)) >= 0)
     {
        if((nom_k=ns_kontr[nomer_sheta_srs].find_r(row[3])) < 0)
         ns_kontr[nomer_sheta_srs].plus(row[3]);

        if(mpe == 0)
         {
          ns_kontr_debs[nomer_sheta_srs].plus(deb,nom_k);
          ns_kontr_kres[nomer_sheta_srs].plus(kre,nom_k);
          if(nom_k < 0)
           {
            ns_kontr_debp[nomer_sheta_srs].plus(0.,nom_k);
            ns_kontr_krep[nomer_sheta_srs].plus(0.,nom_k);
           }
         }
        else
         {
          if(nom_k < 0)
           {
            ns_kontr_debs[nomer_sheta_srs].plus(0.,nom_k);
            ns_kontr_kres[nomer_sheta_srs].plus(0.,nom_k);
           }
          ns_kontr_debp[nomer_sheta_srs].plus(deb,nom_k);
          ns_kontr_krep[nomer_sheta_srs].plus(kre,nom_k);
         }
       
     }
    if(metka_sr == 1)
     {
      glkni_zvt(mpe,row[1],row[3],deb,kre,imatmptab);
     }
   }
  sh.new_plus("");
  if(fabs(deb) > 0.009)
   {
    sh.new_plus(row[1]);
    shk.new_plus(row[2]);
    sm=deb;
   }
  else
   {
    sh.new_plus(row[2]);
    shk.new_plus(row[1]);
    sm=kre;
   }
  if(sh.ravno()[0] == '\0')
   {
    VVOD SOOB(1);
    sprintf(strsql,"%s %s %s %s %s %s",
    row[0],row[1],row[2],row[3],row[4],row[5]);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не введён счёт !"));
    soobshw(&SOOB,stdscr,-1,-1,0,1);

    continue;
   }
  if(shk.ravno()[0] == '\0')
   {
    VVOD SOOB(1);
    sprintf(strsql,"%s %s %s %s %s %s",
    row[0],row[1],row[2],row[3],row[4],row[5]);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    SOOB.VVOD_SPISOK_add_MD(gettext("Не введён коресподирующий счёт !"));
    soobshw(&SOOB,stdscr,-1,-1,0,1);

    continue;
   }

  if((i=PROM.masf.find(sh.ravno(),msrv)) == -1)
   {
    VVOD SOOB(1);
    sprintf(strsql,"%s %s %s %s %s %s",
    row[0],row[1],row[2],row[3],row[4],row[5]);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    sprintf(strsql,gettext("Не найден счёт %s в массиве счетов !"),sh.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);

    continue;
   }

  if((i1=PROM.masf.find(shk.ravno(),msrv)) == -1)
   {
    VVOD SOOB(1);
    sprintf(strsql,"%s %s %s %s %s %s",
    row[0],row[1],row[2],row[3],row[4],row[5]);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    sprintf(strsql,gettext("Не найден счёт корреспондент %s в масиве счетов !"),shk.ravno());
    SOOB.VVOD_SPISOK_add_MD(strsql);
    SOOB.VVOD_SPISOK_add_MD("rozrax");
    soobshw(&SOOB,stdscr,-1,-1,0,1);

    continue;
   }

  if(mpe == 0)
    PROM.sl.plus(sm,i*ks+i1);
  else
    PROM.pe.plus(sm,i*ks+i1);
  
/*Проверяем есть ли неправильные проводки*/
  if(ksrs != 0)
   if(PROM.msr.find(row[2],msrv) >= 0)
   {

    if(row[3][0] == '\0')
     {

      VVOD SOOB(1);
      sprintf(strsql,"%d.%d.%d %-5s %-5s %-5s %10s %10s",
      den,mes,god,row[1],row[2],row[3],row[4],row[5]);
      SOOB.VVOD_SPISOK_add_MD(strsql);
      SOOB.VVOD_SPISOK_add_MD(gettext("В проводке нет кода контрагента !"));
      SOOB.VVOD_SPISOK_add_MD("rozrax");
      soobshw(&SOOB,stdscr,-1,-1,0,1);

     } 
   }

  /*Проверяем есть ли счёт в списке счетов с развернутым сальдо*/
  if(ksrs != 0)
   if(PROM.msr.find(row[1],msrv) >= 0)
   {
   
    if(row[3][0] == '\0')
     {

      VVOD SOOB(1);
      sprintf(strsql,"%d.%d.%d %-5s %-5s %-5s %10s %10s",
      den,mes,god,row[1],row[2],row[3],row[4],row[5]);
      SOOB.VVOD_SPISOK_add_MD(strsql);
      SOOB.VVOD_SPISOK_add_MD(gettext("В проводке нет кода контрагента !"));
      SOOB.VVOD_SPISOK_add_MD("rozrax");
      soobshw(&SOOB,stdscr,-1,-1,0,1);

     } 


   }



 }
GDITE();



/************************/
/*Распечатка результатов*/
/************************/
/*Так как все проводки встречаются дважды то все делим на 2*/

ks=PROM.masf.kolih();

for(i=0;i<ks*ks;i++)
 {
   if(PROM.sl.ravno(i) != 0.)
    {
     sm=PROM.sl.ravno(i)/2.;
     PROM.sl.new_plus(sm,i);
    }
   if(PROM.pe.ravno(i) != 0.)
    {
     sm=PROM.pe.ravno(i)/2.;
     PROM.pe.new_plus(sm,i);
    }
 }

time(&tmm);
bf=localtime(&tmm);


sli=sli1=0;
kst=kst1=0; //Один перевод строки при назначении режимов печати
int kolstr_gk_l=0;
int kolstr_ob_l=0;
int kollist_gk_l=0;
int kollist_ob_l=0;

fprintf(ff1,"\x1B\x45"); //режим выделенного шрифта
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта
fprintf(ff,"\x1B\x33%c\n",30); /*Уменьшаем межстрочный интервал*/
fprintf(ff1,"\x1B\x33%c\n",30); /*Уменьшаем межстрочный интервал*/

fprintf(ff_gk_l,"\x1B\x33%c\n",30); /*Уменьшаем межстрочный интервал*/
fprintf(ff_ob_l,"\x1B\x33%c\n",30); /*Уменьшаем межстрочный интервал*/

if(kod_subbal.ravno()[0] == '\0')
 {
  fprintf(ff,"%s\n\n",iceb_t_get_pnk("00",0));
  fprintf(ff1,"%s\n\n",iceb_t_get_pnk("00",0));
  fprintf(ff_gk_l,"%s\n\n",iceb_t_get_pnk("00",0));
  fprintf(ff_ob_l,"%s\n\n",iceb_t_get_pnk("00",0));
  kst+=1;
  kst1+=1;
  kolstr_gk_l+=1;
  kolstr_ob_l+=1;
 }
else
 {
  int kolih_kod_subbal=0;
  printcod(ff,"Glksubbal","kod","naik",0,kod_subbal.ravno(),&kolih_kod_subbal);
  fprintf(ff,"\n");

  kst+=kolih_kod_subbal;
  kst1+=kolih_kod_subbal;
  kolstr_gk_l+=kolih_kod_subbal;
  kolstr_ob_l+=kolih_kod_subbal;

  printcod(ff1,"Glksubbal","kod","naik",0,kod_subbal.ravno(),&kolih_kod_subbal);
  fprintf(ff1,"\n");
  printcod(ff_gk_l,"Glksubbal","kod","naik",0,kod_subbal.ravno(),&kolih_kod_subbal);
  fprintf(ff_gk_l,"\n");
  printcod(ff_ob_l,"Glksubbal","kod","naik",0,kod_subbal.ravno(),&kolih_kod_subbal);
  fprintf(ff_ob_l,"\n");
 } 

fprintf(ff,"\x0E"); /*Включение режима удвоенной ширины на одну строку*/

fprintf(ff,"\
                %s\n",
gettext("Главная бухгалтерськая книга"));

fprintf(ff,"\x0E"); /*Включение режима удвоенной ширины на одну строку*/
fprintf(ff,"\
            %s %d.%d.%d%s %s %d.%d.%d%s\n",
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."));
//fprintf(ff,"\x14"); /*Выключение режима удвоенной ширины*/


fprintf(ff1,"\x0E"); /*Включение режима удвоенной ширины на одну строку*/
fprintf(ff1,"\
               %s\n",
gettext("Б а л а н с"));

fprintf(ff1,"\x0E"); /*Включение режима удвоенной ширины на одну строку*/
fprintf(ff1,"\
        %s %d.%d.%d%s %s %d.%d.%d%s\n",
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."));



fprintf(ff_gk_l,"\
%35s %s\n\
%30s %s %d.%d.%d%s %s %d.%d.%d%s\n",
" ",
gettext("Главная бухгалтерськая книга"),
" ",
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."));



fprintf(ff_ob_l,"\
%30s %s\n\
%25s %s %d.%d.%d%s %s %d.%d.%d%s\n",
" ",
gettext("Б а л а н с"),
" ",
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."));

kst+=4;
kst1+=4; //Один перевод строки при назначении режимов печати
kolstr_gk_l+=4;
kolstr_ob_l+=4;

gsap_s1(dn,mn,gn,dk,mk,gk,&sli,&kst,bf,ff);
gsao1_s1(&sli1,&kst1,bf,ff1);

gklni_gk_slo1(dn,mn,gn,dk,mk,gk,&kollist_gk_l,&kolstr_gk_l,bf,ff_gk_l);
gsao1_s1(&kollist_ob_l,&kolstr_ob_l,bf,ff_ob_l);

di=ki=0.;
dns=kns=dpp=kpp=0.;
getyx(stdscr,Y,X);
kolstr1=0;
for(i=0; i < PROM.masf.kolih(); i++)
 {
  strzag(LINES-1,0,PROM.masf.kolih(),++kolstr1);

  sh.new_plus(PROM.masf.ravno(i));

  if(proverka(sheta.ravno(),sh.ravno(),1,0) != 0)
    continue;

  if(sbshet != NULL)
   {
    if(metkabd == 0) //Хозрасчёт
     if(proverka(sbshet,sh.ravno(),1,0) == 0)
       continue;

    if(metkabd == 1) //Бюджет
     if(proverka(sbshet,sh.ravno(),1,0) != 0)
      continue;
   }

  sprintf(strsql,"select saldo,nais,vids,ksb from Plansh where ns='%s'",sh.ravno());
  
  if(readkey(strsql,&row,&cur) != 1)
   {
    sprintf(strsql,gettext("Нет счета %s в плане счетов !"),sh.ravno());
    iceb_t_soob(strsql);
    continue;
   }

  if(proverka(kod_subbal.ravno(),row[3],0,0) != 0)
   continue;
   
  saldo=atoi(row[0]);
  vids=atoi(row[2]);
  nais.new_plus(row[1]);  
  move(Y,X);
  
  printw("%-*s %s\n",
  iceb_tu_kolbait(6,sh.ravno()),
  sh.ravno(),
  nais.ravno());
  
  strzag(LINES-1,0,PROM.masf.kolih(),i);
  
  if(saldo == 3)
   {
    nomer_sheta_srs=PROM.msr.find(sh.ravno(),msrv);

    if(metka_sr == 0)
     ras2n(dn,mn,gn,dk,mk,gk,&sli,&kst,&sli1,&kst1,i,sh.ravno(),nais.ravno(),&PROM,&dns,&kns,&dpp,&kpp,&di,&ki,bf,
     &kollist_gk_l,&kolstr_gk_l,&kollist_ob_l,&kolstr_ob_l,metka_naim_shet,
     &ns_kontr[nomer_sheta_srs],
     &ns_kontr_debs[nomer_sheta_srs],
     &ns_kontr_kres[nomer_sheta_srs],
     &ns_kontr_debp[nomer_sheta_srs],
     &ns_kontr_krep[nomer_sheta_srs],
     ff,ff1,ff_gk_l,ff_ob_l,ffcsv.ff);

   if(metka_sr == 1)
     ras2n_tab(dn,mn,gn,dk,mk,gk,&sli,&kst,&sli1,&kst1,i,sh.ravno(),nais.ravno(),&PROM,&dns,&kns,&dpp,&kpp,&di,&ki,bf,
     &kollist_gk_l,&kolstr_gk_l,&kollist_ob_l,&kolstr_ob_l,metka_naim_shet,
     imatmptab,
     ff,ff1,ff_gk_l,ff_ob_l,ffcsv.ff);
   } 
  else
   {
    ras1n(dn,mn,gn,dk,mk,gk,&sli,&kst,&sli1,&kst1,i,sh.ravno(),nais.ravno(),&PROM,&dns,&kns,&dpp,&kpp,&di,&ki,bf,
    &kollist_gk_l,&kolstr_gk_l,&kollist_ob_l,&kolstr_ob_l,metka_naim_shet,ff,ff1,ff_gk_l,ff_ob_l,ffcsv.ff);
   }
 }
GDITE();

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------------------\n\
%*s%15.2f %15.2f %-5s %15.2f %15.2f %15.2f %15.2f\n",
iceb_tu_kolbait(35,gettext("Итого")),gettext("Итого"),dns,kns," ",dpp,kpp,di,ki);

class iceb_tu_str itogo_char[6];
itogo_char[0].new_plus(dns,2);
itogo_char[1].new_plus(kns,2);
itogo_char[2].new_plus(dpp,2);
itogo_char[3].new_plus(kpp,2);
itogo_char[4].new_plus(di,2);
itogo_char[5].new_plus(ki,2);

fprintf(ffcsv.ff,"\
|%s|%s|%s|%s|%s|%s|%s|%s|\n",
gettext("Итого"),itogo_char[0].ravno_dtczp(),itogo_char[1].ravno_dtczp()," ",itogo_char[2].ravno_dtczp(),itogo_char[3].ravno_dtczp(),itogo_char[4].ravno_dtczp(),itogo_char[5].ravno_dtczp());

fprintf(ff1,"\
-------------------------------------------------------------------------------------------------------------\n\
%-*s %15.2f %15.2f %15.2f %15.2f %15.2f %15.2f\n",
iceb_tu_kolbait(13,gettext("Итого")),gettext("Итого"),dns,kns,dpp,kpp,di,ki);

fprintf(ff,"\n       %s______________________\n",gettext("Главный бухгалтер"));
fprintf(ff1,"\n       %s______________________\n",gettext("Главный бухгалтер"));

podpis(ff);
podpis(ff1);

fprintf(ff_gk_l,"\
----------------------------------------------------------------------------------------------------------------------------\n\
%*s%13.2f %13.2f %-5s %13.2f %13.2f %13.2f %13.2f\n",
iceb_tu_kolbait(35,gettext("Итого")),gettext("Итого"),dns,kns," ",dpp,kpp,di,ki);

fprintf(ff_ob_l,"\
-------------------------------------------------------------------------------------------------------------\n\
%-*s %15.2f %15.2f %15.2f %15.2f %15.2f %15.2f\n",
iceb_tu_kolbait(13,gettext("Итого")),gettext("Итого"),dns,kns,dpp,kpp,di,ki);

fprintf(ff_gk_l,"\n       %s______________________\n",gettext("Главный бухгалтер"));
fprintf(ff_ob_l,"\n       %s______________________\n",gettext("Главный бухгалтер"));

podpis(ff_gk_l);
podpis(ff_ob_l);


sprintf(strsql,"%s %02d.%02d.%d",gettext("Сальдо на"),dn,mn,gn);
printw("\n\
%*s - %18s",iceb_tu_kolbait(20,strsql),strsql,prnbr(dns));

printw(" %18s\n",prnbr(kns));
printw("%-*s - %18s",iceb_tu_kolbait(iceb_tu_strlen(strsql),gettext("Оборот за период")),gettext("Оборот за период"),prnbr(dpp));

printw(" %18s\n",prnbr(kpp));

sprintf(strsql,"%s %02d.%02d.%d",gettext("Сальдо на"),dk,mk,gk);

printw("%*s - %18s",iceb_tu_kolbait(20,strsql),strsql,prnbr(di));
printw(" %18s\n",prnbr(ki));

fprintf(ffcsv.ff,"|%s iceB %s %s %s %s|\n",gettext("Система"),gettext("Версия"),VERSION,gettext("Распечатал"),iceb_t_getfioop());

ffcsv.end();
fclose(ff);
fclose(ff1);
fclose(ff_gk_l);
fclose(ff_ob_l);


printw("%s - %d\n",gettext("Количество листов"),sli);


char imaf_bal_f1[64];
memset(imaf_bal_f1,'\0',sizeof(imaf_bal_f1));
char imaf_bal_f2[64];
memset(imaf_bal_f2,'\0',sizeof(imaf_bal_f2));
if(sb == 1)
 {
  balans_f1(metkabd,dk,mk,gk,imaf_bal_f1,&PROM);
  balans_f2(metkabd,dk,mk,gk,imaf_bal_f2,&PROM);
 }

 
oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Главная книга (портрет)"));

oth->spis_imaf.plus(imaf3);
oth->spis_naim.plus(gettext("Оборотный баланс (портрет)"));


if(imaf_bal_f1[0] != '\0')
 {
  oth->spis_imaf.plus(imaf_bal_f1);
  oth->spis_naim.plus(gettext("Баланс, форма N1"));

  oth->spis_imaf.plus(imaf_bal_f2);
  oth->spis_naim.plus(gettext("Отчёт про финансовые результаты, форма N2"));

 }

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
  iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);


oth->spis_imaf.plus(imaf_gk_l);
oth->spis_naim.plus(gettext("Главная книга (ландшафт)"));
iceb_t_ustpeh(imaf_gk_l,2);

oth->spis_imaf.plus(imaf_ob_l);
oth->spis_naim.plus(gettext("Оборотный баланс (ландшафт)"));
iceb_t_ustpeh(imaf_ob_l,2);

oth->spis_imaf.plus(imaf_csv);
oth->spis_naim.plus(gettext("Главная книга для импорта в электронные таблицы"));


printw_vr(tmmn);
OSTANOV();

return(0);
}

/***********************/
/*Счётчик главной книги для портретной ориентации*/
/**********************/
void shet_gk_p1(short dn,short mn,short gn,short dk,short mk,
short gk,
int *sl,int *kst,
struct tm *bf,
FILE *ff)
{
*kst+=1;
if(*kst > kol_str(kol_strok_na_liste,MAX_KOL_STR))
 {
  fprintf(ff,"\f");
  *kst=0;
  gsap_s1(dn,mn,gn,dk,mk,gk,sl,kst,bf,ff);
  *kst+=1;
 }

}
/***********************/
/*Счётчик главной книги для ландшафтной ориентации*/
/**********************/
void shet_gk_l1(short dn,short mn,short gn,short dk,short mk,
short gk,
int *sl,int *kst,
struct tm *bf,
FILE *ff)
{
*kst+=1;
if(*kst > kol_str_l(kol_strok_na_liste_l,MAX_KOL_STR_L))
 {
  fprintf(ff,"\f");
  *kst=1;
  gklni_gk_slo1(dn,mn,gn,dk,mk,gk,sl,kst,bf,ff);
 }

}
/****************************/
/*счётчик оборотного баланса для портретной ориентации*/
/***************************/
void shet_ob_p1(short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,int *kst,
struct tm *bf,
FILE *ff1)
{
*kst+=1;

if(*kst > kol_str(kol_strok_na_liste,MAX_KOL_STR))
 {
  fprintf(ff1,"\f");

  *kst=0;
  gsao1_s1(sl,kst,bf,ff1);
  *kst+=1;

 }

}

/****************************/
/*счётчик оборотного баланса для ландшафтной ориентации*/
/***************************/
void shet_ob_l1(short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,int *kst,
struct tm *bf,
FILE *ff1)
{
*kst+=1;

if(*kst > kol_str_l(kol_strok_na_liste_l,MAX_KOL_STR_L))
 {
  fprintf(ff1,"\f");

  *kst=0;
  gsao1_s1(sl,kst,bf,ff1);
  *kst+=1;

 }

}



/***************************/
/*Распечатка обычных счетов*/
/***************************/

void ras1n(short dn,short mn,short gn,
short dk,short mk,short gk,
int *sli,int *kst,
int *sli1,int *kst1,
short i,
const char *ns, //Номер счета
const char *nash, //Наіменование
class GLKTMP *PROM,
double *dns,
double *kns,
double *dpp,
double *kpp,
double *di,
double *ki,
struct tm *bf,
int *kollist_gk_l,
int *kolstr_gk_l,
int *kollist_ob_l,
int *kolstr_ob_l,
int metka_naim_shet,
FILE *ff,
FILE *ff1,
FILE *ff_gk_l,
FILE *ff_ob_l,
FILE *ffcsv)
{
short           i1;
class iceb_tu_str shk("");
short		mos; /*Метка оборотов по счетам*/
double          sd,sk,dpe,kpe;
/*
printw("%d %s %s\n",i,ns,nash);
OSTANOV();
*/
mos=0;
sd=sk=0.;
dpe=kpe=0.;
int ks=PROM->masf.kolih();
for(i1=0;i1< ks ;i1++)
 {
  sd+=PROM->sl.ravno(i*ks+i1);
  sk+=PROM->sl.ravno(i+ks*i1);

  dpe+=PROM->pe.ravno(i*ks+i1);
  kpe+=PROM->pe.ravno(i+ks*i1);
  if(fabs(PROM->pe.ravno(i*ks+i1)) > 0.009 || fabs(PROM->pe.ravno(i+ks*i1)) > 0.009)
    mos=1;
 }

if(mos == 0)
 {
  if(fabs(sd) < 0.009 && fabs(sk) < 0.009 && fabs(dpe) < 0.009 && fabs(kpe) < 0.009
  && fabs(PROM->ssd.ravno(i)) < 0.009 && fabs(PROM->ssk.ravno(i)) < 0.009)
    return;

  if(fabs(sd+PROM->ssd.ravno(i)-sk-PROM->ssk.ravno(i)) < 0.009 && fabs(dpe) < 0.009 && fabs(kpe) < 0.009)
   return;
 }

*dpp+=dpe;
*kpp+=kpe;

shet_gk_p1(dn,mn,gn,dk,mk,gk,sli,kst,bf,ff);
shet_ob_p1(dn,mn,gn,dk,mk,gk,sli1,kst1,bf,ff1);

shet_gk_l1(dn,mn,gn,dk,mk,gk,kollist_gk_l,kolstr_gk_l,bf,ff_gk_l);
shet_ob_l1(dn,mn,gn,dk,mk,gk,kollist_ob_l,kolstr_ob_l,bf,ff_ob_l);

double          kii,dii;
class iceb_tu_str suma_char[4];
suma_char[1].new_plus(dpe,2);
suma_char[2].new_plus(kpe,2);

if(PROM->ssd.ravno(i)+sd + dpe >= PROM->ssk.ravno(i)+sk+kpe)
 {
  dii=(PROM->ssd.ravno(i)+sd + dpe)-(PROM->ssk.ravno(i)+sk+kpe);
  suma_char[3].new_plus(dii,2);
  *di+=dii;
  PROM->deb_end.plus(dii,i);
  if(PROM->ssd.ravno(i)+sd >= PROM->ssk.ravno(i)+sk)
   {
    kii=(PROM->ssd.ravno(i)+sd)-(PROM->ssk.ravno(i)+sk);
    suma_char[0].new_plus(kii,2);
    *dns+=kii;

    fprintf(ff,"%-*s %-*.*s %15.2f %15s %-5s %15.2f %15.2f %15.2f\n",
    iceb_tu_kolbait(8,ns),ns,
    iceb_tu_kolbait(25,nash),iceb_tu_kolbait(25,nash),nash,
    kii," "," ",dpe,kpe,dii);

    fprintf(ffcsv,"%s|%s|%s|%s|%s|%s|%s|%s|\n ",
    ns,
    nash,
    suma_char[0].ravno_dtczp()," "," ",suma_char[1].ravno_dtczp(),suma_char[2].ravno_dtczp(),suma_char[3].ravno_dtczp());

    fprintf(ff_gk_l,"%-*s %-*.*s %13.2f %13s %-5s %13.2f %13.2f %13.2f\n",
    iceb_tu_kolbait(8,ns),ns,
    iceb_tu_kolbait(25,nash),iceb_tu_kolbait(25,nash),nash,
    kii," "," ",dpe,kpe,dii);

    fprintf(ff1,"%-*.*s %15.2f %15s %15.2f %15.2f %15.2f\n",
    iceb_tu_kolbait(13,ns),iceb_tu_kolbait(13,ns),ns,
    kii," ",dpe,kpe,dii);


    fprintf(ff_ob_l,"%-*.*s %15.2f %15s %15.2f %15.2f %15.2f\n",
    iceb_tu_kolbait(13,ns),iceb_tu_kolbait(13,ns),ns,
    kii," ",dpe,kpe,dii);

   }
  else
   {
    kii=(PROM->ssk.ravno(i)+sk)-(PROM->ssd.ravno(i)+sd);
    suma_char[0].new_plus(kii,2);
    *kns+=kii;

    fprintf(ff,"%-*s %-*.*s %15s %15.2f %-5s %15.2f %15.2f %15.2f\n",
    iceb_tu_kolbait(8,ns),ns,
    iceb_tu_kolbait(25,nash),iceb_tu_kolbait(25,nash),nash,
    " ",kii," ",dpe,kpe,dii);

    fprintf(ffcsv,"%s|%s|%s|%s|%s|%s|%s|%s|\n",
    ns,
    nash,
    " ",suma_char[0].ravno_dtczp()," ",suma_char[1].ravno_dtczp(),suma_char[2].ravno_dtczp(),suma_char[3].ravno_dtczp());

    fprintf(ff_gk_l,"%-*s %-*.*s %13s %13.2f %-5s %13.2f %13.2f %13.2f\n",
    iceb_tu_kolbait(8,ns),ns,
    iceb_tu_kolbait(25,nash),iceb_tu_kolbait(25,nash),nash,
    " ",kii," ",dpe,kpe,dii);

    fprintf(ff1,"%-*.*s %15s %15.2f %15.2f %15.2f %15.2f\n",
    iceb_tu_kolbait(13,ns),iceb_tu_kolbait(13,ns),ns,
    " ",kii,dpe,kpe,dii);

    fprintf(ff_ob_l,"%-*.*s %15s %15.2f %15.2f %15.2f %15.2f\n",
    iceb_tu_kolbait(13,ns),iceb_tu_kolbait(13,ns),ns,
    " ",kii,dpe,kpe,dii);


   }
 }
else
 {
  dii=(PROM->ssk.ravno(i)+sk+kpe)-(PROM->ssd.ravno(i)+sd + dpe);
  suma_char[3].new_plus(dii,2);
  *ki+=dii;
  PROM->kre_end.plus(dii,i);
  if(PROM->ssd.ravno(i)+sd >= PROM->ssk.ravno(i)+sk)
   {
    kii=(PROM->ssd.ravno(i)+sd)-(PROM->ssk.ravno(i)+sk);
    suma_char[0].new_plus(kii,2);
    *dns+=kii;

    fprintf(ff,"%-*s %-*.*s %15.2f %15s %-5s %15.2f %15.2f %15s %15.2f\n",
    iceb_tu_kolbait(8,ns),ns,
    iceb_tu_kolbait(25,nash),iceb_tu_kolbait(25,nash),nash,
    kii," "," ",dpe,kpe," ",dii);

    fprintf(ffcsv,"%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
    ns,
    nash,
    suma_char[0].ravno_dtczp()," "," ",suma_char[1].ravno_dtczp(),suma_char[2].ravno_dtczp()," ",suma_char[3].ravno_dtczp());

    fprintf(ff_gk_l,"%-*s %-*.*s %13.2f %13s %-5s %13.2f %13.2f %13s %13.2f\n",
    iceb_tu_kolbait(8,ns),ns,
    iceb_tu_kolbait(25,nash),iceb_tu_kolbait(25,nash),nash,
    kii," "," ",dpe,kpe," ",dii);

    fprintf(ff1,"%-*.*s %15.2f %15s %15.2f %15.2f %15s %15.2f\n",
    iceb_tu_kolbait(13,ns),iceb_tu_kolbait(13,ns),ns,
    kii," ",dpe,kpe," ",dii);

   
    fprintf(ff_ob_l,"%-*.*s %15.2f %15s %15.2f %15.2f %15s %15.2f\n",
    iceb_tu_kolbait(13,ns),iceb_tu_kolbait(13,ns),ns,
    kii," ",dpe,kpe," ",dii);
}
  else
   {
    kii=(PROM->ssk.ravno(i)+sk)-(PROM->ssd.ravno(i)+sd);
    suma_char[0].new_plus(kii,2);
    *kns+=kii;

    fprintf(ff,"%-*s %-*.*s %15s %15.2f %-5s %15.2f %15.2f %15s %15.2f\n",
    iceb_tu_kolbait(8,ns),ns,
    iceb_tu_kolbait(25,nash),iceb_tu_kolbait(25,nash),nash,
    " ",kii," ",dpe,kpe," ",dii);

    fprintf(ffcsv,"%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
    ns,
    nash,
    " ",suma_char[0].ravno_dtczp()," ",suma_char[1].ravno_dtczp(),suma_char[2].ravno_dtczp()," ",suma_char[3].ravno_dtczp());

    fprintf(ff_gk_l,"%-*s %-*.*s %13s %13.2f %-5s %13.2f %13.2f %13s %13.2f\n",
    iceb_tu_kolbait(8,ns),ns,
    iceb_tu_kolbait(25,nash),iceb_tu_kolbait(25,nash),nash,
    " ",kii," ",dpe,kpe," ",dii);

    fprintf(ff1,"%-*.*s %15s %15.2f %15.2f %15.2f %15s %15.2f\n",
    iceb_tu_kolbait(13,ns),iceb_tu_kolbait(13,ns),ns,
    " ",kii,dpe,kpe," ",dii);


    fprintf(ff_ob_l,"%-*.*s %15s %15.2f %15.2f %15.2f %15s %15.2f\n",
    iceb_tu_kolbait(13,ns),iceb_tu_kolbait(13,ns),ns,
    " ",kii,dpe,kpe," ",dii);

   }
 }


if(iceb_tu_strlen(nash) > 25)
 {
  shet_gk_p1(dn,mn,gn,dk,mk,gk,sli,kst,bf,ff);
  fprintf(ff,"%-8s %s\n"," ",iceb_tu_adrsimv(25,nash));

  shet_gk_l1(dn,mn,gn,dk,mk,gk,kollist_gk_l,kolstr_gk_l,bf,ff_gk_l);
  fprintf(ff_gk_l,"%-8s %s\n"," ",iceb_tu_adrsimv(25,nash));
 }

char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
class iceb_tu_str str_shet("");

class iceb_tu_str naim_shet("");
/*Распечатываем по счетам*/
for(i1=0; i1<ks ; i1++)
 {

  shk.new_plus(PROM->masf.ravno(i1));
     
  if(PROM->pe.ravno(i*ks+i1) != 0. || PROM->pe.ravno(i+ks*i1) != 0.)
   {
    naim_shet.new_plus("");
    if(metka_naim_shet == 1)
     {
      sprintf(strsql,"select nais from Plansh where ns='%s'",shk.ravno());
      if(readkey(strsql,&row,&cur) == 1)
       naim_shet.new_plus(row[0]);
      
//      str_shet.new_plus(naim_shet.ravno());
//      str_shet.plus(" ",shk.ravno());
      int razmer_naim=71-iceb_tu_strlen(shk.ravno());

      sprintf(strsql,"%.*s %s",
      iceb_tu_kolbait(razmer_naim,naim_shet.ravno()),
      naim_shet.ravno(),
      shk.ravno());
     }
    else
     strcpy(strsql,shk.ravno());
//      str_shet.new_plus(shk.ravno());
     
    shet_gk_p1(dn,mn,gn,dk,mk,gk,sli,kst,bf,ff);

    fprintf(ff,"%*.*s %15.2f %15.2f\n",
    iceb_tu_kolbait(72,strsql),iceb_tu_kolbait(72,strsql),strsql,PROM->pe.ravno(i*ks+i1),PROM->pe.ravno(i+ks*i1));

    suma_char[0].new_plus(PROM->pe.ravno(i*ks+i1));
    suma_char[1].new_plus(PROM->pe.ravno(i+ks*i1));

    fprintf(ffcsv,"|%s|||%s|%s|%s|||||\n",
    naim_shet.ravno(),shk.ravno(),suma_char[0].ravno_dtczp(),suma_char[1].ravno_dtczp());

    shet_gk_l1(dn,mn,gn,dk,mk,gk,kollist_gk_l,kolstr_gk_l,bf,ff_gk_l);

    fprintf(ff_gk_l,"%*.*s %13.2f %13.2f\n",
    iceb_tu_kolbait(68,strsql),iceb_tu_kolbait(68,strsql),strsql,PROM->pe.ravno(i*ks+i1),PROM->pe.ravno(i+ks*i1));

   }
 }

shet_gk_p1(dn,mn,gn,dk,mk,gk,sli,kst,bf,ff);
fprintf(ff,"\
........................................................................................................................................\n");

shet_gk_l1(dn,mn,gn,dk,mk,gk,kollist_gk_l,kolstr_gk_l,bf,ff_gk_l);
fprintf(ff_gk_l,"\
............................................................................................................................\n");

}

/******************************************/
/*Работаем со счетами с развернутым сальдо сформированных с таблицей*/
/******************************************/

void ras2n_tab(short dn,short mn,short gn,
short dk,short mk,short gk,
int *sli,int *kst,
int *sli1,int *kst1,
int nomer_sheta, //Номер счета в массиве счетов
const char *sh,
const char *nash,  //Наименование счета
GLKTMP *PROM,
double *dns,
double *kns,
double *dpp,
double *kpp,
double *di,
double *ki,
struct tm *bf,
int *kollist_gk_l,
int *kolstr_gk_l,
int *kollist_ob_l,
int *kolstr_ob_l,
int metka_naim_shet,
const char *imatabl,
FILE *ff,
FILE *ff1,
FILE *ff_gk_l,
FILE *ff_ob_l,
FILE *ffcsv)
{
double          d1=0.,k1=0.;
double          d3=0.,k3=0.;
int         i1;
short		mos=0; /*Метка оборотов по счетам*/
SQL_str         row;
char		strsql[1024];
double		debs=0.,kres=0.;
double		debp=0.,krep=0.;
double          sd=0.,sk=0.,dpe=0.,kpe=0.;
SQLCURSOR cur;



/*****************
for(int nomer_kontr=0; nomer_kontr < ns_kontr->kolih(); nomer_kontr++)
 {
  debs=ns_kontr_debs->ravno(nomer_kontr);
  kres=ns_kontr_kres->ravno(nomer_kontr);
  debp=ns_kontr_debp->ravno(nomer_kontr);
  krep=ns_kontr_krep->ravno(nomer_kontr);

  if(debs > kres)
   sd+=debs-kres;
  else
   sk+=kres-debs;

  if(debs+debp > kres+krep)
   d1+=debs+debp-kres-krep;  
  else
   k1+=kres+krep-debs-debp;

  dpe+=debp;
  kpe+=krep;
 }
*************************/
sprintf(strsql,"select debs,kres,debp,krep from %s where shet='%s'",imatabl,sh);

if(cur.make_cursor(&bd,strsql) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %10s %10s %6s %s\n",row[0],row[1],row[2],row[3],row[4]);
//  debs=kres=debp=krep=0.;

  debs=atof(row[0]);
  kres=atof(row[1]);
  debp=atof(row[2]);
  krep=atof(row[3]);

  if(debs > kres)
   sd+=debs-kres;
  else
   sk+=kres-debs;

  if(debs+debp > kres+krep)
   d1+=debs+debp-kres-krep;  
  else
   k1+=kres+krep-debs-debp;

  dpe+=debp;
  kpe+=krep;
 }
//printw("sd=%f sk=%f\n",sd,sk);
//OSTANOV();
*dns+=sd;
*kns+=sk;

*dpp+=dpe;
*kpp+=kpe;
/*printw("dpp=%f kpp=%d\n",dpp,kpp);*/

*di+=d1; 
*ki+=k1;
PROM->kre_end.plus(k1,nomer_sheta);
PROM->deb_end.plus(d1,nomer_sheta);

/*Проверяем были ли обороты за период*/

d3=k3=0.;
mos=0;
int ks=PROM->masf.kolih();
for(i1=0; i1<ks;i1++)
 {
  d3+=PROM->pe.ravno(i1+ks*nomer_sheta);
  k3+=PROM->pe.ravno(i1*ks+nomer_sheta);
  if(fabs(PROM->pe.ravno(i1+ks*nomer_sheta)) > 0.009 || fabs(PROM->pe.ravno(i1*ks+nomer_sheta)) > 0.009)
    mos=1;
 }

if(mos == 0)
if(fabs(sd) < 0.009 && fabs(sk) < 0.009 && fabs(dpe) < 0.009
&& fabs(kpe) < 0.009 && fabs(d1) < 0.009 && fabs(k1) < 0.009
&& fabs(d3) < 0.009 && fabs(k3) < 0.009)
  return;

shet_gk_p1(dn,mn,gn,dk,mk,gk,sli,kst,bf,ff);
shet_ob_p1(dn,mn,gn,dk,mk,gk,sli1,kst1,bf,ff1);

shet_gk_l1(dn,mn,gn,dk,mk,gk,kollist_gk_l,kolstr_gk_l,bf,ff_gk_l);
shet_ob_l1(dn,mn,gn,dk,mk,gk,kollist_ob_l,kolstr_ob_l,bf,ff_ob_l);

fprintf(ff,"%-*s %-*.*s %15.2f %15.2f %-5s %15.2f %15.2f %15.2f %15.2f\n",
iceb_tu_kolbait(8,sh),sh,
iceb_tu_kolbait(25,nash),iceb_tu_kolbait(25,nash),nash,
sd,sk," ",dpe,kpe,d1,k1);

class iceb_tu_str suma_char[6];
suma_char[0].new_plus(sd,2);
suma_char[1].new_plus(sk,2);
suma_char[2].new_plus(dpe,2);
suma_char[3].new_plus(kpe,2);
suma_char[4].new_plus(d1,2);
suma_char[5].new_plus(k1,2);

fprintf(ffcsv,"%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
sh,
nash,
suma_char[0].ravno_dtczp(),suma_char[1].ravno_dtczp()," ",suma_char[2].ravno_dtczp(),suma_char[3].ravno_dtczp(),suma_char[4].ravno_dtczp(),suma_char[5].ravno_dtczp());

fprintf(ff_gk_l,"%-*s %-*.*s %13.2f %13.2f %-5s %13.2f %13.2f %13.2f %13.2f\n",
iceb_tu_kolbait(8,sh),sh,
iceb_tu_kolbait(25,nash),iceb_tu_kolbait(25,nash),nash,
sd,sk," ",dpe,kpe,d1,k1);

if(iceb_tu_strlen(nash) > 25)
 {
  shet_gk_p1(dn,mn,gn,dk,mk,gk,sli,kst,bf,ff);
  fprintf(ff,"%-8s %s\n"," ",iceb_tu_adrsimv(25,nash));
  
  shet_gk_l1(dn,mn,gn,dk,mk,gk,kollist_gk_l,kolstr_gk_l,bf,ff_gk_l);
  fprintf(ff_gk_l,"%-8s %s\n"," ",iceb_tu_adrsimv(25,nash));
 }

fprintf(ff1,"%-*s %15.2f %15.2f %15.2f %15.2f %15.2f %15.2f\n",
iceb_tu_kolbait(13,sh),sh, sd, sk, dpe, kpe , d1 , k1);

fprintf(ff_ob_l,"%-*s %15.2f %15.2f %15.2f %15.2f %15.2f %15.2f\n",
iceb_tu_kolbait(13,sh),sh, sd, sk, dpe, kpe , d1 , k1);


class iceb_tu_str naim_shet("");

/*Распечатываем по счетам*/
for(i1=0; i1<ks;i1++)
 {

  d3=PROM->pe.ravno(i1+ks*nomer_sheta);
  k3=PROM->pe.ravno(i1*ks+nomer_sheta);
  if(d3 != 0. || k3 != 0.)
   {
    naim_shet.new_plus("");
    if(metka_naim_shet == 1)
     {
  
      sprintf(strsql,"select nais from Plansh where ns='%s'",PROM->masf.ravno(i1));
      if(readkey(strsql,&row,&cur) == 1)
       naim_shet.new_plus(row[0]);

      int razmer_naim=71-iceb_tu_strlen(PROM->masf.ravno(i1));

      sprintf(strsql,"%.*s %s",
      iceb_tu_kolbait(razmer_naim,naim_shet.ravno()),
      naim_shet.ravno(),
      PROM->masf.ravno(i1));
     }
    else
      strncpy(strsql,PROM->masf.ravno(i1),sizeof(strsql));
     

    shet_gk_p1(dn,mn,gn,dk,mk,gk,sli,kst,bf,ff);

    fprintf(ff,"%*.*s %15.2f %15.2f\n",
    iceb_tu_kolbait(72,strsql),iceb_tu_kolbait(72,strsql),strsql,d3,k3);

    suma_char[0].new_plus(d3,2);
    suma_char[1].new_plus(k3,2);

    fprintf(ffcsv,"|%s|||%s|%s|%s|||||\n",
    naim_shet.ravno(),PROM->masf.ravno(i1),suma_char[0].ravno_dtczp(),suma_char[1].ravno_dtczp());

    shet_gk_l1(dn,mn,gn,dk,mk,gk,kollist_gk_l,kolstr_gk_l,bf,ff_gk_l);
    fprintf(ff_gk_l,"%*.*s %13.2f %13.2f\n",
    iceb_tu_kolbait(68,strsql),iceb_tu_kolbait(68,strsql),strsql,d3,k3);

   }
 }
shet_gk_p1(dn,mn,gn,dk,mk,gk,sli,kst,bf,ff);

fprintf(ff,"\
........................................................................................................................................\n");

shet_gk_l1(dn,mn,gn,dk,mk,gk,kollist_gk_l,kolstr_gk_l,bf,ff_gk_l);
fprintf(ff_gk_l,"\
............................................................................................................................\n");

}
/******************************************/
/*Работаем со счетами с развернутым сальдо сформированных по спискам*/
/******************************************/

void ras2n(short dn,short mn,short gn,
short dk,short mk,short gk,
int *sli,int *kst,
int *sli1,int *kst1,
int nomer_sheta, //Номер счета в массиве счетов
const char *sh,
const char *nash,  //Наименование счета
GLKTMP *PROM,
double *dns,
double *kns,
double *dpp,
double *kpp,
double *di,
double *ki,
struct tm *bf,
int *kollist_gk_l,
int *kolstr_gk_l,
int *kollist_ob_l,
int *kolstr_ob_l,
int metka_naim_shet,
class SPISOK *ns_kontr,
class masiv_din_double *ns_kontr_debs,
class masiv_din_double *ns_kontr_kres,
class masiv_din_double *ns_kontr_debp,
class masiv_din_double *ns_kontr_krep,
FILE *ff,
FILE *ff1,
FILE *ff_gk_l,
FILE *ff_ob_l,
FILE *ffcsv)
{
double          d1=0.,k1=0.;
double          d3=0.,k3=0.;
int         i1;
short		mos=0; /*Метка оборотов по счетам*/
SQL_str         row;
char		strsql[1024];
double		debs=0.,kres=0.;
double		debp=0.,krep=0.;
double          sd=0.,sk=0.,dpe=0.,kpe=0.;
SQLCURSOR cur;


for(int nomer_kontr=0; nomer_kontr < ns_kontr->kolih(); nomer_kontr++)
 {
  debs=ns_kontr_debs->ravno(nomer_kontr);
  kres=ns_kontr_kres->ravno(nomer_kontr);
  debp=ns_kontr_debp->ravno(nomer_kontr);
  krep=ns_kontr_krep->ravno(nomer_kontr);

  if(debs > kres)
   sd+=debs-kres;
  else
   sk+=kres-debs;

  if(debs+debp > kres+krep)
   d1+=debs+debp-kres-krep;  
  else
   k1+=kres+krep-debs-debp;

  dpe+=debp;
  kpe+=krep;
 }


*dns+=sd;
*kns+=sk;

*dpp+=dpe;
*kpp+=kpe;
/*printw("dpp=%f kpp=%d\n",dpp,kpp);*/

*di+=d1; 
*ki+=k1;
PROM->kre_end.plus(k1,nomer_sheta);
PROM->deb_end.plus(d1,nomer_sheta);

/*Проверяем были ли обороты за период*/

d3=k3=0.;
mos=0;
int ks=PROM->masf.kolih();
for(i1=0; i1<ks;i1++)
 {
  d3+=PROM->pe.ravno(i1+ks*nomer_sheta);
  k3+=PROM->pe.ravno(i1*ks+nomer_sheta);
  if(fabs(PROM->pe.ravno(i1+ks*nomer_sheta)) > 0.009 || fabs(PROM->pe.ravno(i1*ks+nomer_sheta)) > 0.009)
    mos=1;
 }

if(mos == 0)
if(fabs(sd) < 0.009 && fabs(sk) < 0.009 && fabs(dpe) < 0.009
&& fabs(kpe) < 0.009 && fabs(d1) < 0.009 && fabs(k1) < 0.009
&& fabs(d3) < 0.009 && fabs(k3) < 0.009)
  return;

shet_gk_p1(dn,mn,gn,dk,mk,gk,sli,kst,bf,ff);
shet_ob_p1(dn,mn,gn,dk,mk,gk,sli1,kst1,bf,ff1);

shet_gk_l1(dn,mn,gn,dk,mk,gk,kollist_gk_l,kolstr_gk_l,bf,ff_gk_l);
shet_ob_l1(dn,mn,gn,dk,mk,gk,kollist_ob_l,kolstr_ob_l,bf,ff_ob_l);

fprintf(ff,"%-*s %-*.*s %15.2f %15.2f %-5s %15.2f %15.2f %15.2f %15.2f\n",
iceb_tu_kolbait(8,sh),sh,
iceb_tu_kolbait(25,nash),iceb_tu_kolbait(25,nash),nash,
sd,sk," ",dpe,kpe,d1,k1);

class iceb_tu_str suma_char[6];
suma_char[0].new_plus(sd,2);
suma_char[1].new_plus(sk,2);
suma_char[2].new_plus(dpe,2);
suma_char[3].new_plus(kpe,2);
suma_char[4].new_plus(d1,2);
suma_char[5].new_plus(k1,2);

fprintf(ffcsv,"%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
sh,
nash,
suma_char[0].ravno_dtczp(),suma_char[1].ravno_dtczp()," ",suma_char[2].ravno_dtczp(),suma_char[3].ravno_dtczp(),suma_char[4].ravno_dtczp(),suma_char[5].ravno_dtczp());

fprintf(ff_gk_l,"%-*s %-*.*s %13.2f %13.2f %-5s %13.2f %13.2f %13.2f %13.2f\n",
iceb_tu_kolbait(8,sh),sh,
iceb_tu_kolbait(25,nash),iceb_tu_kolbait(25,nash),nash,
sd,sk," ",dpe,kpe,d1,k1);

if(iceb_tu_strlen(nash) > 25)
 {
  shet_gk_p1(dn,mn,gn,dk,mk,gk,sli,kst,bf,ff);
  fprintf(ff,"%-8s %s\n"," ",iceb_tu_adrsimv(25,nash));
  
  shet_gk_l1(dn,mn,gn,dk,mk,gk,kollist_gk_l,kolstr_gk_l,bf,ff_gk_l);
  fprintf(ff_gk_l,"%-8s %s\n"," ",iceb_tu_adrsimv(25,nash));
 }

fprintf(ff1,"%-*s %15.2f %15.2f %15.2f %15.2f %15.2f %15.2f\n",
iceb_tu_kolbait(13,sh),sh, sd, sk, dpe, kpe , d1 , k1);

fprintf(ff_ob_l,"%-*s %15.2f %15.2f %15.2f %15.2f %15.2f %15.2f\n",
iceb_tu_kolbait(13,sh),sh, sd, sk, dpe, kpe , d1 , k1);

class iceb_tu_str str_shet("");

class iceb_tu_str naim_shet("");

/*Распечатываем по счетам*/
for(i1=0; i1<ks;i1++)
 {

  d3=PROM->pe.ravno(i1+ks*nomer_sheta);
  k3=PROM->pe.ravno(i1*ks+nomer_sheta);
  if(d3 != 0. || k3 != 0.)
   {
    naim_shet.new_plus("");
    if(metka_naim_shet == 1)
     {
  
      sprintf(strsql,"select nais from Plansh where ns='%s'",PROM->masf.ravno(i1));
      if(readkey(strsql,&row,&cur) == 1)
       naim_shet.new_plus(row[0]);

      str_shet.new_plus(naim_shet.ravno());
      str_shet.plus(" ",PROM->masf.ravno(i1));
     }
    else
      str_shet.new_plus(PROM->masf.ravno(i1));
     

    shet_gk_p1(dn,mn,gn,dk,mk,gk,sli,kst,bf,ff);

    fprintf(ff,"%*.*s %15.2f %15.2f\n",
    iceb_tu_kolbait(72,str_shet.ravno()),iceb_tu_kolbait(72,str_shet.ravno()),str_shet.ravno(),d3,k3);

    suma_char[0].new_plus(d3,2);
    suma_char[1].new_plus(k3,2);

    fprintf(ffcsv,"|%s|||%s|%s|%s|||||\n",
    naim_shet.ravno(),PROM->masf.ravno(i1),suma_char[0].ravno_dtczp(),suma_char[1].ravno_dtczp());

    shet_gk_l1(dn,mn,gn,dk,mk,gk,kollist_gk_l,kolstr_gk_l,bf,ff_gk_l);
    fprintf(ff_gk_l,"%*.*s %13.2f %13.2f\n",
    iceb_tu_kolbait(68,str_shet.ravno()),iceb_tu_kolbait(68,str_shet.ravno()),str_shet.ravno(),d3,k3);

   }
 }
shet_gk_p1(dn,mn,gn,dk,mk,gk,sli,kst,bf,ff);

fprintf(ff,"\
........................................................................................................................................\n");

shet_gk_l1(dn,mn,gn,dk,mk,gk,kollist_gk_l,kolstr_gk_l,bf,ff_gk_l);
fprintf(ff_gk_l,"\
............................................................................................................................\n");

}

/**************************/
/*Шапка оборотного баланса*/
/****************************/

void gsao1_s1(int *sl,int *kst,struct tm *bf,FILE *ff1)
{
 fprintf(ff1,"\
%s %d.%d.%d  %s - %d:%02d",
 gettext("По состоянию на"),
 bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
 gettext("Время"),
 bf->tm_hour,bf->tm_min);
 *sl+=1;
 fprintf(ff1,"%*s%d\n",iceb_tu_kolbait(55,gettext("Лист N")),gettext("Лист N"),*sl);

 fprintf(ff1,"\
--------------------------------------------------------------------------------------------------------------\n");

fprintf(ff1,gettext("             |          C а л ь д о          |         О б о р о т           |         С а л ь д о           |\n"));
fprintf(ff1,gettext("    Счёт     -------------------------------------------------------------------------------------------------\n"));
fprintf(ff1,gettext("             |    Дебет      |    Кредит     |    Дебет      |     Кредит    |    Дебет      |    Кредит     |\n"));

  fprintf(ff1,"\
--------------------------------------------------------------------------------------------------------------\n");
*kst+=6;
}
/***********************/
/*Шапка главной книги портретной ориентации*/
/**********************/
void  gsap_s1(short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,int *kst,struct tm *bf,FILE *ff)
{

  fprintf(ff,"\
%s %d.%d.%d  %s - %d:%d",
  gettext("По состоянию на"),
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
  gettext("Время"),
  bf->tm_hour,bf->tm_min);
  *sl+=1;
  fprintf(ff,"%*s%d\n",iceb_tu_kolbait(70,gettext("Лист N")),gettext("Лист N"),*sl);

  fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff,gettext("        |      Наименование       |   Сальдо до %02d.%02d.%4dг.      |Счёт |     Оборот за период          |   Сальдо на %02d.%02d.%4dг.     |\n"),
  dn,mn,gn,dk,mk,gk);
  fprintf(ff,gettext("  Счёт  |         счета           --------------------------------|коре-|---------------------------------------------------------------\n"));
  fprintf(ff,gettext("        |                         |    Дебет      |    Кредит     |спон.|    Дебет      |     Кредит    |    Дебет      |    Кредит    |\n"));

  fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------------------\n");
*kst+=6;
}


/***********************/
/*Шапка главной книги ландшафтной ориентации*/
/**********************/
void  gklni_gk_slo1(short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,int *kst,struct tm *bf,FILE *ff)
{

  fprintf(ff,"\
%s %d.%d.%d  %s - %d:%d",
  gettext("По состоянию на"),
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
  gettext("Время"),
  bf->tm_hour,bf->tm_min);
  *sl+=1;
  fprintf(ff,"%*s%d\n",iceb_tu_kolbait(70,gettext("Лист N")),gettext("Лист N"),*sl);

  fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff,gettext("        |      Наименование       |   Сальдо до %02d.%02d.%4dг.  |Счёт |     Оборот за период      |   Сальдо на %02d.%02d.%4dг. |\n"),  dn,mn,gn,dk,mk,gk);
  fprintf(ff,gettext("  Счёт  |         счета           ----------------------------|коре-|-------------------------------------------------------\n"));
  fprintf(ff,gettext("        |                         |    Дебет    |    Кредит   |спон.|    Дебет    |     Кредит  |    Дебет    |    Кредит  |\n"));

  fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------\n");
*kst+=6;
}
/***********************************************/
/* запись в промежуточную таблицу*/
/********************************************/
int glkni_zvt(int per,const char *shet,const char *kod_kontr,double deb,double kre,const char *imatabl)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;



sprintf(strsql,"select debs,kres,debp,krep from %s where shet='%s' and kod_kontr='%s'",imatabl,shet,kod_kontr);

if(readkey(strsql,&row,&cur) == 0)
 {
  if(per == 0)
   sprintf(strsql,"insert into %s values ('%s','%s',%.2f,%.2f,0.,0.)",imatabl,shet,kod_kontr,deb,kre);
  if(per == 1)
   sprintf(strsql,"insert into %s values ('%s','%s',0.,0.,%.2f,%.2f)",imatabl,shet,kod_kontr,deb,kre);
 }
else
 {

  double debz=0.;
  double krez=0.;

  if(per == 0)
   {
  
    debz=deb+atof(row[0]);
    krez=kre+atof(row[1]);

    sprintf(strsql,"update %s \
set \
debs=%.2f,\
kres=%.2f \
where shet='%s'and kod_kontr='%s'",imatabl,debz,krez,shet,kod_kontr);
   }

  if(per == 1)
   {
    debz=deb+atof(row[2]);
    krez=kre+atof(row[3]);
   sprintf(strsql,"update %s \
set \
debp=%.2f,\
krep=%.2f \
where shet='%s'and kod_kontr='%s'",imatabl,debz,krez,shet,kod_kontr);
   }
 }

if(sql_zapis(strsql,1,0) != 0)
 return(1);

return(0);
}
