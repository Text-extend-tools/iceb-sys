/*$Id: vedprz1.c,v 5.32 2013/09/26 09:43:43 sasa Exp $*/
/*28.03.2018	18.01.2004	Белых А.И.	vedprz1.c
Новая форма ведомости производственных затрат
*/
#include        <errno.h>
#include        "buhg.h"

void veprz1r(short,short,short,short,short,short,struct tm *,FILE *,SPISOK *,SPISOK *,masiv_din_double *,const char*);
void veprz1rn(short,short,short,short,short,short,struct tm *,FILE *,SPISOK *,SPISOK *,
masiv_din_double *,
masiv_din_double *,
masiv_din_double *,
masiv_din_double *,
masiv_din_double *,const char*);
void vedprz1_sap(const char *podr,const char *grupap,const char *sheta,const char *prih_rash,FILE *ff);

extern short mmm,ggg;
extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/

int vedprz1(class spis_oth *oth)
{
char		strsql[512];
static class iceb_tu_str podrz("");
static class iceb_tu_str grupap("");
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
static class iceb_tu_str sheta("");
static class iceb_tu_str prih_rash("");
static class iceb_tu_str tabnom("");
int itn=0;
short metka_prih_rash=0;

int kom=0,kom1=0;
class iceb_tu_str naim("");

VVOD MENU(3);

if(dat1.ravno()[0] == '\0')
 {
  sprintf(strsql,"%d.%d",mmm,ggg);
  dat1.new_plus(strsql);
 }
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт ведомости производственных затрат"));

VV.VVOD_SPISOK_add_data(dat1.ravno(),8);
VV.VVOD_SPISOK_add_data(dat2.ravno(),8);
VV.VVOD_SPISOK_add_data(podrz.ravno(),128);
VV.VVOD_SPISOK_add_data(grupap.ravno(),128);
VV.VVOD_SPISOK_add_data(sheta.ravno(),128);
VV.VVOD_SPISOK_add_data(prih_rash.ravno(),2);
VV.VVOD_SPISOK_add_data(tabnom.ravno(),128);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала..(м.г)......"));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца...(м.г)......"));
VV.VVOD_SPISOK_add_MD(gettext("Подразделение......(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Код группы под-ния.(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт...............(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Начисл./удержан. .(+/-)."));
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер....(,,)."));

naz:;
clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
podrz.new_plus(VV.data_ravno(2));
grupap.new_plus(VV.data_ravno(3));
sheta.new_plus(VV.data_ravno(4));
prih_rash.new_plus(VV.data_ravno(5));
tabnom.new_plus(VV.data_ravno(6));

switch (kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp4_5_1.txt");
   goto naz;
  case ESC:
  case FK10:
     return(1);
     break;

  case PLU:
  case FK2:
     break;

  case FK3:

    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка cчетов"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка табельных номеров"));//3
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
       if(dirzarpodr(2,&podrz,&naim) == 0)
          VV.data_z_plus(2,podrz.ravno());
       break;

      case 1 :
       if(vibrek("Zargrupp",&grupap,&naim) == 0)
          VV.VVOD_SPISOK_zapis_data(3,grupap.ravno());

      case 2:
       if(vibrek("Plansh",&sheta,&naim) == 0)
          VV.VVOD_SPISOK_zapis_data(4,sheta.ravno());

      case 3:
       itn=0;
       
       if(dirtab(&itn,&naim,0,1) == 0)
          VV.data_z_plus(6,itn);
       
       break;
     }
    goto naz;
    
  case FK4:
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
    break;
 }
short dn=1,mn=0,gn=0;
short dk=31,mk=0,gk=0;

if(prih_rash.ravno()[0] == '+')
 metka_prih_rash=1;
if(prih_rash.ravno()[0] == '-')
 metka_prih_rash=2;
 
if(rsdat1(&mn,&gn,dat1.ravno()) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата начала !"));
  goto naz;
 }

if(dat2.ravno()[0] != '\0')
 {
  if(rsdat1(&mk,&gk,dat2.ravno()) != 0)
   {
    iceb_t_soob(gettext("Не верно введена дата конца !"));
    goto naz;
   }

 }
else
 {
  mk=mn;
  gk=gn;
 }
dpm(&dk,&mk,&gk,5);

GDITE();
class iceb_tu_str bros("");
SQL_str   row,row1;
SQLCURSOR cur;
SQLCURSOR cur1;

class iceb_tu_str glavbuh("");

iceb_t_poldan("Табельный номер бухгалтера",&bros,"zarnast.alx");

if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros.ravno());
  if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
   {
    beep();
    printw("Не знайдено табельного номера %s головного бухгалтера!\n",bros.ravno());
    OSTANOV();
   }
  else
   {
    glavbuh.new_plus(row[0]);
   }
 }

struct tm *bf;
time_t vremn;
time(&vremn);
bf=localtime(&vremn);


//Определяем список счетов для начислений и удержаний
class SPISOK sp_shet_nah; //Список всех счетов начислений в порядке возростания
class SPISOK sp_shet_uder; //Список всех счетов удержаний в порядке возростания
class SPISOK shet_nah;     //Счёт+начисление
class SPISOK shet_uder;    //Счёт + удержание

class masiv_din_double shet_nah_suma; //Суммы для Счет+начисление
class masiv_din_double shet_nah_suma_os; //Суммы для Счет+начисление основная зарплата
class masiv_din_double shet_nah_suma_do; //Суммы для Счет+начисление дополнительная зарплата
class masiv_din_double shet_nah_suma_dr; //Суммы для Счет+начисление другая зарплата
class masiv_din_double shet_nah_suma_fz; //Суммы для Счет+начисление входящие в фонд зарплаты
class masiv_din_double shet_uder_suma; //Суммы для Счет+удержание

class masiv_din_double shet_nah_suma_bt; //Суммы для Счет+начисление бюджет
class masiv_din_double shet_nah_suma_os_bt; //Суммы для Счет+начисление основная зарплата бюджет
class masiv_din_double shet_nah_suma_do_bt; //Суммы для Счет+начисление дополнительная зарплата бюджет
class masiv_din_double shet_nah_suma_dr_bt; //Суммы для Счет+начисление другая зарплата  бюджет
class masiv_din_double shet_nah_suma_fz_bt; //Суммы для Счет+начисление входящие в фонд зарплаты  бюджет
class masiv_din_double shet_uder_suma_bt; //Суммы для Счет+удержание  бюджет

class masiv_din_double shet_nah_suma_ht; //Суммы для Счет+начисление хозрасчёт
class masiv_din_double shet_nah_suma_os_ht; //Суммы для Счет+начисление основная зарплата хозрасчёт
class masiv_din_double shet_nah_suma_do_ht; //Суммы для Счет+начисление дополнительная зарплата хозрасчёт
class masiv_din_double shet_nah_suma_dr_ht; //Суммы для Счет+начисление другая зарплата хозрасчёт
class masiv_din_double shet_nah_suma_fz_ht; //Суммы для Счет+начисление входящие в фонд зарплаты хозрасчёт
class masiv_din_double shet_uder_suma_ht; //Суммы для Счет+удержание  хозрасчёт

int       kol_shet_nah=0;
int       kol_shet_uder=0;


//Определяем список счетов для начислений
sprintf(strsql,"select distinct shet from Zarp where datz >= '%d-%d-%d' and \
datz <= '%d-%d-%d' and prn='1' and suma <> 0. order by shet asc",gn,mn,dn,gk,mk,dk);

if((kol_shet_nah=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kol_shet_nah == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }

while(cur.read_cursor(&row) != 0)
  sp_shet_nah.plus(row[0]);

//Определяем список счетов для удержаний
sprintf(strsql,"select distinct shet from Zarp where datz >= '%d-%d-%d' and \
datz <= '%d-%d-%d' and prn='2' and suma <> 0. order by shet asc",gn,mn,dn,gk,mk,dk);

if((kol_shet_uder=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }


while(cur.read_cursor(&row) != 0)
  sp_shet_uder.plus(row[0]);


sprintf(strsql,"select prn,knah,suma,shet,podr,tabn from Zarp where datz >= '%d-%d-%d' and \
datz <= '%d-%d-%d' and suma <> 0.",gn,mn,dn,gk,mk,dk);
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

//printw("%s\n",strsql);

int kolstr1=0;
int nomer=0;
double suma=0.;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,kolstr1++);
    
  if(proverka(podrz.ravno(),row[4],0,0) != 0)
   continue;

  if(proverka(tabnom.ravno(),row[5],0,0) != 0)
   continue;

  if(grupap.ravno()[0] != '\0')
   {
    sprintf(strsql,"select grup from Podr where kod=%s",row[4]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     if(proverka(grupap.ravno(),row1[0],0,0) != 0)
      continue;
   }
  
//  printw("%s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4]);
  
  suma=atof(row[2]);
  if(atoi(row[0]) ==  1) //Начисление
   {
    sprintf(strsql,"%s|%s",row[3],row[1]);
    if((nomer = shet_nah.find(strsql)) < 0)
      shet_nah.plus(strsql);

    shet_nah_suma.plus(suma,nomer);

    if(proverka(shetb,row[3],0,0) == 0)
     {
      shet_nah_suma_bt.plus(suma,nomer);
      shet_nah_suma_ht.plus(0.,nomer);
     }
    else
     {
      shet_nah_suma_ht.plus(suma,nomer);
      shet_nah_suma_bt.plus(0.,nomer);
     }
    
    //читаем начисление
    short vn=0;
    short fz=0;
    
    sprintf(strsql,"select vidn,fz from Nash where kod=%s",row[1]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     {
      vn=atoi(row1[0]);
      fz=atoi(row1[1]);
     }     

    if(vn == 0)
      shet_nah_suma_os.plus(suma,nomer);
    else
      shet_nah_suma_os.plus(0.,nomer);
    
    if(vn == 1)
      shet_nah_suma_do.plus(suma,nomer);
    else
      shet_nah_suma_do.plus(0.,nomer);

    if(vn == 2)
      shet_nah_suma_dr.plus(suma,nomer);
    else
      shet_nah_suma_dr.plus(0.,nomer);

    if(fz == 0)
      shet_nah_suma_fz.plus(suma,nomer);
    else
      shet_nah_suma_fz.plus(0.,nomer);

    if(proverka(shetb,row[3],0,0) == 0)
     {
      if(vn == 0)
        shet_nah_suma_os_bt.plus(suma,nomer);
      else
        shet_nah_suma_os_bt.plus(0.,nomer);
      shet_nah_suma_os_ht.plus(0.,nomer);
      
      if(vn == 1)
        shet_nah_suma_do_bt.plus(suma,nomer);
      else
        shet_nah_suma_do_bt.plus(0.,nomer);
      shet_nah_suma_do_ht.plus(0.,nomer);

      if(vn == 2)
        shet_nah_suma_dr_bt.plus(suma,nomer);
      else
        shet_nah_suma_dr_bt.plus(0.,nomer);
      shet_nah_suma_dr_ht.plus(0.,nomer);

      if(fz == 0)
        shet_nah_suma_fz_bt.plus(suma,nomer);
      else
        shet_nah_suma_fz_bt.plus(0.,nomer);
      shet_nah_suma_fz_ht.plus(0.,nomer);
      

     }
    else
     {
      if(vn == 0)
        shet_nah_suma_os_ht.plus(suma,nomer);
      else
        shet_nah_suma_os_ht.plus(0.,nomer);
      shet_nah_suma_os_bt.plus(0.,nomer);
      
      if(vn == 1)
        shet_nah_suma_do_ht.plus(suma,nomer);
      else
        shet_nah_suma_do_ht.plus(0.,nomer);
      shet_nah_suma_do_bt.plus(0.,nomer);

      if(vn == 2)
        shet_nah_suma_dr_ht.plus(suma,nomer);
      else
        shet_nah_suma_dr_ht.plus(0.,nomer);
      shet_nah_suma_dr_bt.plus(0.,nomer);

      if(fz == 0)
        shet_nah_suma_fz_ht.plus(suma,nomer);
      else
        shet_nah_suma_fz_ht.plus(0.,nomer);
      shet_nah_suma_fz_bt.plus(0.,nomer);

     }          
   }   
  else  //Удержание
   {
    sprintf(strsql,"%s|%s",row[3],row[1]);
    if((nomer = shet_uder.find(strsql)) < 0)
      shet_uder.plus(strsql);
    shet_uder_suma.plus(suma,nomer);

    if(proverka(shetbu,row[3],0,0) == 0)
     {
      shet_uder_suma_bt.plus(suma,nomer);
      shet_uder_suma_ht.plus(0.,nomer);
     }
    else
     {
      shet_uder_suma_ht.plus(suma,nomer);
      shet_uder_suma_bt.plus(0.,nomer);
     }
   }

 }
GDITE();
FILE *ffbt=NULL;
FILE *ffht=NULL;
char imafbt[64];
char imafht[64];

if(shetb != NULL)
 { 
  sprintf(imafbt,"vpzbt%d.lst",getpid());
  sprintf(imafht,"vpzht%d.lst",getpid());

  if((ffbt = fopen(imafbt,"w")) == NULL)
   {
    error_op_nfil(imafbt,errno,"");
    return(1);
   }

  iceb_t_zagolov(gettext("Расчёт ведомости производственных затрат"),dn,mn,gn,dk,mk,gk,ffbt);
  fprintf(ffbt,"%s\n",gettext("Бюджет"));
  if((ffht = fopen(imafht,"w")) == NULL)
   {
    error_op_nfil(imafht,errno,"");
    return(1);
   }

  iceb_t_zagolov(gettext("Расчёт ведомости производственных затрат"),dn,mn,gn,dk,mk,gk,ffht);
  fprintf(ffht,"%s\n",gettext("Хозрасчёт"));
 }
 
char imaf[64];
sprintf(imaf,"vpz%d.lst",getpid());
FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
 
iceb_t_zagolov(gettext("Расчёт ведомости производственных затрат"),dn,mn,gn,dk,mk,gk,ff);

vedprz1_sap(podrz.ravno(),grupap.ravno(),sheta.ravno(),prih_rash.ravno(),ff);


if(metka_prih_rash == 0 || metka_prih_rash == 1)
  veprz1rn(dn,mn,gn,dk,mk,gk,bf,ff,&sp_shet_nah,&shet_nah,&shet_nah_suma,&shet_nah_suma_os,&shet_nah_suma_do,
  &shet_nah_suma_dr,&shet_nah_suma_fz,sheta.ravno());

if(metka_prih_rash == 0 || metka_prih_rash == 2)
 {
  fprintf(ff,"\n%s\n",gettext("Удержания"));
  veprz1r(dn,mn,gn,dk,mk,gk,bf,ff,&sp_shet_uder,&shet_uder,&shet_uder_suma,sheta.ravno());
 }

fprintf(ff,"%s____________________%s\n",gettext("Главный бухгалтер"),
glavbuh.ravno());

podpis(ff);
fclose(ff);


if(ffbt != NULL)
 {
 
   vedprz1_sap(podrz.ravno(),grupap.ravno(),sheta.ravno(),prih_rash.ravno(),ffbt);

  if(metka_prih_rash == 0 || metka_prih_rash == 1)
    veprz1rn(dn,mn,gn,dk,mk,gk,bf,ffbt,&sp_shet_nah,&shet_nah,&shet_nah_suma_bt,
    &shet_nah_suma_os_bt,&shet_nah_suma_do_bt,&shet_nah_suma_dr_bt,&shet_nah_suma_fz_bt,sheta.ravno());

  if(metka_prih_rash == 0 || metka_prih_rash == 2)
   {
    fprintf(ffbt,"\n%s\n",gettext("Удержания"));
    veprz1r(dn,mn,gn,dk,mk,gk,bf,ffbt,&sp_shet_uder,&shet_uder,&shet_uder_suma_bt,sheta.ravno());
   }

  fprintf(ffbt,"%s____________________%s\n",gettext("Главный бухгалтер"),
  glavbuh.ravno());
  podpis(ffbt);
  fclose(ffbt);

 }

if(ffht != NULL)
 {
 
   vedprz1_sap(podrz.ravno(),grupap.ravno(),sheta.ravno(),prih_rash.ravno(),ffht);

  if(metka_prih_rash == 0 || metka_prih_rash == 1)
    veprz1rn(dn,mn,gn,dk,mk,gk,bf,ffht,&sp_shet_nah,&shet_nah,&shet_nah_suma_ht,
    &shet_nah_suma_os_ht,&shet_nah_suma_do_ht,&shet_nah_suma_dr_ht,&shet_nah_suma_fz_ht,sheta.ravno());

  if(metka_prih_rash == 0 || metka_prih_rash == 2)
   {
    fprintf(ffht,"\n%s\n",gettext("Удержания"));
    veprz1r(dn,mn,gn,dk,mk,gk,bf,ffht,&sp_shet_uder,&shet_uder,&shet_uder_suma_ht,sheta.ravno());
   }
  fprintf(ffht,"%s____________________%s\n",gettext("Главный бухгалтер"),
  glavbuh.ravno());
  podpis(ffht);
  fclose(ffht);

 }


oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт ведомости производственных затрат"));
if(ffbt != NULL)
 {
  oth->spis_imaf.plus(imafbt);
  
  sprintf(strsql,"%s (%s)",
  gettext("Расчёт ведомости производственных затрат"),
  gettext("Бюджет"));
  oth->spis_naim.plus(strsql);
  
  oth->spis_imaf.plus(imafht);

  sprintf(strsql,"%s (%s)",
  gettext("Расчёт ведомости производственных затрат"),
  gettext("Хозрасчёт"));
  oth->spis_naim.plus(strsql);

 }
for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);
return(0);
}

/************************************/
/*Распечатка удержаний    */
/*************************************/

void veprz1r(short dn,short mn,short gn,
short dk,short mk,short gk,
struct tm *bf,
FILE *ff,
SPISOK *sp_shet_nah,
SPISOK *shet_nah,
class masiv_din_double *shet_nah_suma,const char *sheta)
{
SQL_str row;
SQLCURSOR curr;
char strsql[512];

class iceb_tu_str shet("");
class iceb_tu_str shet1("");
class iceb_tu_str knah("");
class iceb_tu_str naimshet("");
class iceb_tu_str naimnah("");
double itogo=0.;
double itogos=0.;

int kol_par=shet_nah->kolih();
//printw("kol_par=%d\n",kol_par);
double suma=0.;

int nomerp=0;
int kol_shet_nah=sp_shet_nah->kolih();


fprintf(ff,"\
------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Счёт  |     Наименование счета       |Код|  Наименование удержания      | Сумма    |\n"));

fprintf(ff,"\
------------------------------------------------------------------------------------\n");


for(int nomersh=0; nomersh < kol_shet_nah; nomersh++)
 {
  shet.new_plus(sp_shet_nah->ravno(nomersh));

  if(proverka(sheta,shet.ravno(),0,0) != 0)
   continue;

  nomerp=0;
  itogos=0.;
  for(int nomer=0; nomer < kol_par; nomer++)
   {
    
    polen(shet_nah->ravno(nomer),&shet1,1,'|');
//    printw("%s\n",shet1);
    if(SRAV(shet.ravno(),shet1.ravno(),0) != 0)
      continue;
    polen(shet_nah->ravno(nomer),&knah,2,'|');

    //Читаем наименование начисления

    sprintf(strsql,"select naik from Uder where kod=%s",knah.ravno());

    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
      naimnah.new_plus(row[0]);
    else
      naimnah.new_plus("");
    
    suma=shet_nah_suma->ravno(nomer);
    if(suma == 0.)
     continue;
    itogo+=suma;    
    itogos+=suma;    
    if(nomerp == 0)
     {
      //Читаем наименование счета
      sprintf(strsql,"select nais from Plansh where ns='%s'",shet1.ravno());                

      if(sql_readkey(&bd,strsql,&row,&curr) == 1)
       naimshet.new_plus(row[0]);
      else
       naimshet.new_plus("");

      fprintf(ff,"%-*s %-*.*s %3s %-*.*s %10.2f\n",
      iceb_tu_kolbait(6,shet1.ravno()),shet1.ravno(),
      iceb_tu_kolbait(30,naimshet.ravno()),iceb_tu_kolbait(30,naimshet.ravno()),naimshet.ravno(),
      knah.ravno(),
      iceb_tu_kolbait(30,naimnah.ravno()),iceb_tu_kolbait(30,naimnah.ravno()),naimnah.ravno(),
      suma);
     }
    else
     {
      fprintf(ff,"%6s %-30.30s %3s %-*.*s %10.2f\n",
      " "," ",knah.ravno(),
      iceb_tu_kolbait(30,naimnah.ravno()),iceb_tu_kolbait(30,naimnah.ravno()),naimnah.ravno(),
      suma);

     }
    nomerp++;
   }
  if(nomerp > 0)
   {
    fprintf(ff,"%*s%10.2f\n",iceb_tu_kolbait(73,gettext("Итого")),gettext("Итого"),itogos);
    fprintf(ff,"\
. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . \n");
   }
 }
fprintf(ff,"\
------------------------------------------------------------------------------------\n");

fprintf(ff,"%*s%10.2f\n",iceb_tu_kolbait(73,gettext("Итого")),gettext("Итого"),itogo);


}

/************************************/
/*Распечатка начислений    */
/*************************************/

void veprz1rn(short dn,short mn,short gn,
short dk,short mk,short gk,
struct tm *bf,
FILE *ff,
SPISOK *sp_shet_nah,
SPISOK *shet_nah,
class masiv_din_double *shet_nah_suma,
class masiv_din_double *shet_nah_suma_os,
class masiv_din_double *shet_nah_suma_do,
class masiv_din_double *shet_nah_suma_dr,
class masiv_din_double *shet_nah_suma_fz,
const char *sheta)
{
SQL_str row;
SQLCURSOR curr;
char strsql[512];

class iceb_tu_str shet("");
class iceb_tu_str shet1("");
class iceb_tu_str knah("");
class iceb_tu_str naimshet("");
class iceb_tu_str naimnah("");
double itogo=0.;
double itogo_os=0.;
double itogo_do=0.;
double itogo_dr=0.;
double itogo_fz=0.;
double itogos=0.;
double itogos_os=0.;
double itogos_do=0.;
double itogos_dr=0.;
double itogos_fz=0.;

int kol_par=shet_nah->kolih();
//printw("kol_par=%d\n",kol_par);
double suma=0.;
double suma_os=0.;
double suma_do=0.;
double suma_dr=0.;
double suma_fz=0.;

int nomerp=0;
int kol_shet_nah=sp_shet_nah->kolih();


fprintf(ff,"\
----------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Счёт  |Наименование счета  |Код|Наи-ние начисления  | Сумма    |Основная|Доп-ная |Другая  |Фонд з/п|\n"));

fprintf(ff,"\
----------------------------------------------------------------------------------------------------\n");


for(int nomersh=0; nomersh < kol_shet_nah; nomersh++)
 {
  shet.new_plus(sp_shet_nah->ravno(nomersh));

  if(proverka(sheta,shet.ravno(),0,0) != 0)
   continue;

  nomerp=0;
  itogos=0.;
  itogos_os=0.;
  itogos_do=0.;
  itogos_dr=0.;
  itogos_fz=0.;
  for(int nomer=0; nomer < kol_par; nomer++)
   {
    
    polen(shet_nah->ravno(nomer),&shet1,1,'|');
//    printw("%s\n",shet1);
    if(SRAV(shet.ravno(),shet1.ravno(),0) != 0)
      continue;
    polen(shet_nah->ravno(nomer),&knah,2,'|');

    //Читаем наименование начисления

    sprintf(strsql,"select naik from Nash where kod=%s",knah.ravno());


    if(readkey(strsql,&row,&curr) == 1)
      naimnah.new_plus(row[0]);
    else
      naimnah.new_plus("");    
    suma=shet_nah_suma->ravno(nomer);
    if(suma == 0.)
     continue;
    suma_os=shet_nah_suma_os->ravno(nomer);
    suma_do=shet_nah_suma_do->ravno(nomer);
    suma_dr=shet_nah_suma_dr->ravno(nomer);
    suma_fz=shet_nah_suma_fz->ravno(nomer);

    itogo+=suma;    
    itogo_os+=suma_os;    
    itogo_do+=suma_do;    
    itogo_dr+=suma_dr;    
    itogo_fz+=suma_fz;    

    itogos+=suma;    
    itogos_os+=suma_os;    
    itogos_do+=suma_do;    
    itogos_dr+=suma_dr;    
    itogos_fz+=suma_fz;    
    if(nomerp == 0)
     {
      //Читаем наименование счета
      sprintf(strsql,"select nais from Plansh where ns='%s'",shet1.ravno());

      if(sql_readkey(&bd,strsql,&row,&curr) == 1)
       naimshet.new_plus(row[0]);
      else
       naimshet.new_plus("");

      fprintf(ff,"%-*s %-*.*s %3s %-*.*s %10.2f %8.2f %8.2f %8.2f %8.2f\n",
      iceb_tu_kolbait(6,shet1.ravno()),shet1.ravno(),
      iceb_tu_kolbait(20,naimshet.ravno()),iceb_tu_kolbait(20,naimshet.ravno()),naimshet.ravno(),
      knah.ravno(),
      iceb_tu_kolbait(20,naimnah.ravno()),iceb_tu_kolbait(20,naimnah.ravno()),naimnah.ravno(),
      suma,suma_os,suma_do,suma_dr,suma_fz);
     }
    else
     {
      fprintf(ff,"%6s %-20.20s %3s %-*.*s %10.2f %8.2f %8.2f %8.2f %8.2f\n",
      " "," ",knah.ravno(),
      iceb_tu_kolbait(20,naimnah.ravno()),iceb_tu_kolbait(20,naimnah.ravno()),naimnah.ravno(),
      suma,suma_os,suma_do,suma_dr,suma_fz);

     }
    nomerp++;
   }
  if(nomerp > 0)
   {
    fprintf(ff,"%*s%10.2f %8.2f %8.2f %8.2f %8.2f\n",
    iceb_tu_kolbait(53,gettext("Итого")),gettext("Итого"),
    itogos,
    itogos_os,
    itogos_do,
    itogos_dr,
    itogos_fz);

    fprintf(ff,"\
. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . \n");
   }
 }
fprintf(ff,"\
----------------------------------------------------------------------------------------------------\n");

fprintf(ff,"%*s%10.2f %8.2f %8.2f %8.2f %8.2f\n",
iceb_tu_kolbait(53,gettext("Итого")),gettext("Итого"),
itogo,
itogo_os,
itogo_do,
itogo_dr,
itogo_fz);



}
/****************************/
/*Печать реквизитов поиска*/
/*****************************/
void vedprz1_sap(const char *podr,const char *grupap,const char *sheta,const char *prih_rash,FILE *ff)
{
if(podr[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Подразделение"),podr);

if(grupap[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Группа"),grupap);

if(sheta[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Счёт"),sheta);

if(prih_rash[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Начисления/удержания"),prih_rash);

}
