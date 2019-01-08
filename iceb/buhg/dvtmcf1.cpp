/* $Id: dvtmcf1.c,v 5.100 2013/09/26 09:43:32 sasa Exp $ */
/*06.12.2018    08.02.1994      Белых А.И.      dvtmcf1.c
Движение товаро-материальных ценностей по складам
С разбивкой по счетам и складам
С сортировкой по названию наименования
*/

#include        <math.h>
#include        <errno.h>
#include        "buhg.h"

void            gsapp(short,short,short,short,short,short,short *,short *,int,const char*,FILE*);
void            itgrup(int,double,double,double,double,double,double,double,double,FILE*);
void            itskl(int,double,double,double,double,double,double,double,double,FILE*,FILE*,FILE*);
void            itsh(const char*,double,double,double,double,double,double,double,double,FILE*,FILE*,FILE*);
void            itkg(const char[],int,double,double,double,double,short,FILE*);
void		shdtmcf1(short,short,short,short,short,short,int,FILE*,int,const char*);
void dvtmcf1_sh_uz(short dn,short mn,short gn,short dk,short mk,short gk,int *sl,int *kst,int skl,const char *nai,FILE *ffuz,FILE *ffuzcp);

void  itgrup_uz(int kodgr,double kk1,double kk2,double kk3,double kk4,double ss1,double ss2,double ss3,double ss4,FILE *ffuz,FILE *ffuzcp,
short dn,short mn,short gn,short dk,short mk,short gk,int *sl,int *kst,int skl,const char *nai,double *itgr_svcr);

void  itskl_uz(int skl,double kk1,double kk2,double kk3,double kk4,double ss1,double ss2,double ss3,double ss4,short dn,short mn,short gn,
short dk,short mk,short gk,int *sl,int *kst,double *itsk_svcr,FILE *ffuz,FILE *ffuzcp);

void itkg_uz(const char *sh,int skl,double k1,double k2,double k3,double k4,short met,short dn,short mn,short gn,short dk,short mk,short gk,
int *sl,int *kst,const char *naim_skl,FILE *ffuz,FILE *ffuzcp);

void itsh_uz(const char *sh,double k1,double k2,double k3,double k4,double s1,double s2,double s3,double s4,short dn,short mn,short gn,
short dk,short mk,short gk,int *sl,int *kst,int skl,const char *naim_skl,double *itsh_svcr,FILE *ffuz,FILE *ffuzcp);

void dvtmcf1_sap_uz(short dn,short mn,short gn,short dk,short mk,short gk,int *nom_list_uz,int *kolstr_uz,int skl,const char *nai,FILE *ffuz,FILE *ffuzcp);

extern int kol_strok_na_liste;

int dvtmcf1(class spis_oth *oth) //0-нормально 1-для кладовщика*/
{
char            bros[1024];
char            str[1024];
char strsql[1024];
extern char	*maskmat; /*Массив кодов отмеченных материалов*/
time_t          tmmn;
short           kst,sli;
char            imaf[64];
char imafuz[64],imafuzcp[64];
char		imafitog[64];
char		imafitog1[64];
short           dn,mn,gn;
short           dk,mk,gk;
char            nomn[128];
int             kom,kom1;
double          k1,s1,k2,s2,k3,s3,k4,s4;
double          kk1,ss1,kk2,ss2,kk3,ss3,kk4,ss4;
FILE            *ff=NULL;
FILE		*ffuz=NULL;
FILE		*ffuzcp=NULL;
FILE		*ffitog=NULL;
FILE		*ffitog1=NULL;
class iceb_tu_str shb(""),shbm("");
int		skl,skl1;
class iceb_tu_str naim("");
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
static class iceb_tu_str sklz("");
static class iceb_tu_str shbz("");
static class iceb_tu_str kodmz("");
static class iceb_tu_str kgrmz("");
static class iceb_tu_str ndsc("");
short		kgrm,kgrmzp;
int		nk;
class iceb_tu_str ei("");
double		cena=0.,cena_real=0.,nds=0.;
char		mnds=0;
double		ost1,ost2,ost3,ost4;
short		ostkg;
double		kg0,kg1,kg2,kg3;
double		kgs0,kgs1,kgs2,kgs3;
double		itg1,itg2,itg3,itg4,itg5,itg6,itg7,itg8;
int		kgr,kodm;
long		kolstr,kolstr1,kolstr2;
SQL_str         row,rowtmp,row1;
class iceb_tu_str naiskl("");
double		fas;
short		mvsh; /*0-не выдавать шапку 1-выдавать*/
int		sort;
class iceb_tu_str naimshet("");
double itgr_svcr[4]; /*сумма в ценах реализации по группе*/
double itsh_svcr[4]; /*сумма в ценах реализации по счёту*/
double itsk_svcr[4]; /*сумма в ценах реализации по складу*/
double itog_svcr[4]; /*сумма в ценах реализации общая*/

double itogo_kol[4]; /*Общий итог*/
double itogo_sum[4];

memset(itogo_kol,'\0',sizeof(itogo_kol));
memset(itogo_sum,'\0',sizeof(itogo_sum));
memset(itgr_svcr,'\0',sizeof(itgr_svcr));
memset(itsh_svcr,'\0',sizeof(itsh_svcr));
memset(itog_svcr,'\0',sizeof(itog_svcr));
memset(itsk_svcr,'\0',sizeof(itsk_svcr));

float pnds=iceb_t_pnds();

class VVOD VV(0);
class VVOD MENU(3);

VV.VVOD_SPISOK_add_ZAG(gettext("Движение тов.мат. ценностей с сортировкой по наименованию (форма 1)"));
VV.VVOD_SPISOK_add_ZAG(gettext("Введите реквизиты для получения распечатки"));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(shbz.ravno(),112);
VV.VVOD_SPISOK_add_data(sklz.ravno(),112);
VV.VVOD_SPISOK_add_data(kgrmz.ravno(),112);
VV.VVOD_SPISOK_add_data(kodmz.ravno(),112);
VV.VVOD_SPISOK_add_data(ndsc.ravno(),5);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала...(д.м.г)..."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца....(д.м.г)..."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт..(,,).............."));
VV.VVOD_SPISOK_add_MD(gettext("Склад..(,,)............."));
VV.VVOD_SPISOK_add_MD(gettext("Группа материалов.(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Код материалла (,,/*)..."));
sprintf(bros,gettext("НДС (0,%2.f,+)............"),pnds);
VV.VVOD_SPISOK_add_MD(bros);

  

naz:;
clear();

move(LINES-6,0);
printw(gettext("\
Если НДС не ввели - все карточки\n\
Если НДС = 0      - карточки с нулевым НДС\n\
Если НДС = %2.f     - карточки с %2.f%% НДС\n\
Если НДС = +      - карточки с НДС в том числе\n"),pnds,pnds);


  helstr(LINES-1,0,"F1",gettext("помощь"),
  "F2/+",gettext("расчитать"),
  "F3",gettext("реквизиты"),
  "F4",gettext("очистить"),
  "F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
  shbz.new_plus(VV.VVOD_SPISOK_return_data(2));
  sklz.new_plus(VV.VVOD_SPISOK_return_data(3));
  kgrmz.new_plus(VV.VVOD_SPISOK_return_data(4));
  kodmz.new_plus(VV.VVOD_SPISOK_return_data(5));
  ndsc.new_plus(VV.VVOD_SPISOK_return_data(6));

class iceb_tu_str kods("");
class iceb_tu_str naims("");
switch (kom)
 {
  case ESC:
  case FK10:
     return(1);
     break;

  case FK1:
    GDITE();
    iceb_t_pdoc("matu5_4.txt");
    goto naz;

  case PLU:
  case FK2:
     break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка складов"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка материалов"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));//3
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
        if(dirsklad(1,&kods,&naims) == 0)
         {        
            VV.data_z_plus(3,kods.ravno());
         }
        goto naz;

      case 1 :
        vibrek("Grup",&kgrmz,&naim);
        VV.VVOD_SPISOK_zapis_data(4,kgrmz.ravno());
        goto naz;

      case 2 :

        if(dirmatr(&kgr,&kodm,&naims,1,0,"",0,0) == 0)
         {
          VV.data_z_plus(5,kodm);
         }

        clear();
        goto naz;

      case 3 :
        vibrek("Plansh",&shbz);
        VV.VVOD_SPISOK_zapis_data(2,shbz.ravno());
        goto naz;
     }

  case FK4:
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
    break;
 }

  if(kodmz.ravno()[0] == '*' && maskmat == NULL)
   { 
    iceb_t_soob(gettext("Не сделан список материалов !"));
    goto naz;
   } 

  if(rsdatp(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(0),&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(1)) != 0)
    goto naz;


MENU.VVOD_delete();
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите вид сортировки"));
MENU.VVOD_SPISOK_add_MD(gettext("Сортировать счет-склад-наименование материалла")); //0
MENU.VVOD_SPISOK_add_MD(gettext("Сортировать счет-склад-группа-наименование материалла"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Сортировать Склад-счет-наименование материалла"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//3
sort=0;
while(menu3w(stdscr,&MENU,&sort,-1,-1,0) != 0);
if(sort == -1 || sort == 3 )
  goto naz;

time(&tmmn);


clear();
refresh();

GDITE();

ostkg=0;
iceb_t_poldan("Отчет в килограммах",bros,"matnast.alx");
if(SRAV(bros,"Вкл",1) == 0)
 ostkg=1;


sprintf(str,"select * from Kart");
SQLCURSOR cur,curtmp;
SQLCURSOR cur1;
if((kolstr=cur.make_cursor(&bd,str)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),str);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной карточки !"));
  return(1);
 }  

class iceb_t_tmptab tabtmp;
const char *imatmptab={"dvtmcf1"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
sh char(24) not null,\
skl int not null,\
kgm int not null,\
naim char(112) not null,\
km int not null,\
nk int not null,\
ei char(24) not null,\
cena double(15,6) not null,\
nds float(2) not null,\
mnds smallint not null,\
fas float(2) not null,\
cenap double(15,6) not null) ENGINE = MYISAM",
imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 return(-1);

printw("%s\n%s %d.%d.%d %s %d.%d.%d\n",
gettext("Сортируем записи"),
gettext("Период с"),
dn,mn,gn,
gettext("до"),
dk,mk,gk);

refresh();
kgrm=0;
kolstr2=kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {

  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(sklz.ravno(),row[0],0,0) != 0)
   continue;

  if(proverka(shbz.ravno(),row[5],1,0) != 0)
   continue;

  kgrm=0;
  naim.new_plus("");
  /*Читаем код группы материалла*/
  sprintf(str,"select kodgr,naimat from Material where kodm=%s",row[2]);
  if(sql_readkey(&bd,str,&row1,&cur1) != 1)
   {
    beep();
    printw("%s %s !\n",gettext("Не найден код материалла"),row[2]);
    OSTANOV();
    continue;
   }
  else
   {
    kgrm=atoi(row1[0]);
    naim.new_plus(row1[1]);
   }

  if(proverka(kgrmz.ravno(),row1[0],0,0) != 0)
   continue;

  if(provnds(ndsc.ravno(),row) != 0)
    continue;

  if(kodmz.ravno()[0] != '\0')
   {
    if(kodmz.ravno()[0] == '*' && maskmat != NULL)
     {
      if(proverka(maskmat,row[2],0,0) != 0)
         continue;
     }
    else
      if(proverka(kodmz.ravno(),row[2],0,0) != 0)
         continue;
   }
  kolstr2++;
  sprintf(strsql,"insert into %s values ('%s',%s,%d,'%s',%s,%s,'%s',%.10g,%.5g,%s,%s,%.10g)",
  imatmptab,
  row[5],row[0],kgrm,naim.ravno_filtr(),row[2],row[1],row[4],atof(row[6]),atof(row[9]),
  row[3],row[10],atof(row[7]));
  sql_zapis(strsql,1,0);    
 }
GDITE();



if(sort == 0) //Сортировать счет-склад-наименование материалла.
 {
  sprintf(strsql,"select * from %s order by sh asc,skl asc,naim asc",imatmptab);
 }
if(sort == 1) //Сортировать счет-склад-группа-наименование материалла.
 {
  sprintf(strsql,"select * from %s order by sh asc,skl asc,kgm asc,naim asc",imatmptab);
  
 }

if(sort == 2) //Сортировать склад-счет-наименование материалла.
 {
  sprintf(strsql,"select * from %s order by skl asc,sh asc,naim asc",imatmptab);
  
 }

int kolstrtmp=0;


if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
printw(gettext("Делаем отчёт\n"));
refresh();

sprintf(imaf,"zag%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

char imaf_csv[64];
sprintf(imaf_csv,"dtmc%d.csv",getpid());
class iceb_fopen fil_csv;
if(fil_csv.start(imaf_csv,"w") != 0)
 return(1);

int kolstr_uz=0;
int nom_list_uz=0;


sprintf(imafitog,"zai%d.lst",getpid());

if((ffitog = fopen(imafitog,"w")) == NULL)
 {
  error_op_nfil(imafitog,errno,"");
  return(1);
 }
iceb_t_zagolov(gettext("Свод движения материальных ценностей"),dn,mn,gn,dk,mk,gk,ffitog);

sprintf(imafitog1,"zair%d.lst",getpid());

if((ffitog1 = fopen(imafitog1,"w")) == NULL)
 {
  error_op_nfil(imafitog1,errno,"");
  return(1);
 }
iceb_t_zagolov(gettext("Свод движения материальных ценностей"),dn,mn,gn,dk,mk,gk,ffitog1);

sprintf(imafuz,"zaguz%d.lst",getpid());
if((ffuz = fopen(imafuz,"w")) == NULL)
 {
  error_op_nfil(imafuz,errno,"");
  return(1);
 }

iceb_t_zagolov(gettext("Оборотная ведомость движения товарно-материальных ценностей за период с"),dn,mn,gn,dk,mk,gk,ffuz);
kolstr_uz=5;

sprintf(imafuzcp,"zaguzc%d.lst",getpid());
if((ffuzcp = fopen(imafuzcp,"w")) == NULL)
 {
  error_op_nfil(imafuzcp,errno,"");
  return(1);
 }


iceb_t_zagolov(gettext("Оборотная ведомость движения товарно-материальных ценностей в ценах реализации"),dn,mn,gn,dk,mk,gk,ffuzcp);


iceb_t_zagolov(gettext("Оборотная ведомость движения товарно-материальных ценностей за период с"),dn,mn,gn,dk,mk,gk,ff);
iceb_t_zagolov(gettext("Оборотная ведомость движения товарно-материальных ценностей за период с"),dn,mn,gn,dk,mk,gk,fil_csv.ff);
kst=4;

if(shbz.ravno()[0] != '\0')
 {
  fprintf(ff,gettext("По счету учета %s\n"),shbz.ravno());
  fprintf(fil_csv.ff,gettext("По счету учета %s\n"),shbz.ravno());
  fprintf(ffuz,gettext("По счету учета %s\n"),shbz.ravno());
  fprintf(ffuzcp,gettext("По счету учета %s\n"),shbz.ravno());
  kolstr_uz++;
  kst++;
 }
if(sklz.ravno()[0] != '\0')
 {
  fprintf(ff,gettext("По складах %s\n"),sklz.ravno());
  fprintf(fil_csv.ff,gettext("По складах %s\n"),sklz.ravno());
  fprintf(ffuz,gettext("По складах %s\n"),sklz.ravno());
  fprintf(ffuzcp,gettext("По складах %s\n"),sklz.ravno());
  kolstr_uz++;
  kst++;

 }
if(kgrmz.ravno()[0] != '\0')
 {
  fprintf(ff,gettext("По группе материалла %s\n"),kgrmz.ravno());
  fprintf(fil_csv.ff,gettext("По группе материалла %s\n"),kgrmz.ravno());
  fprintf(ffuz,gettext("По группе материалла %s\n"),kgrmz.ravno());
  fprintf(ffuzcp,gettext("По группе материалла %s\n"),kgrmz.ravno());
   kolstr_uz++;
  kst++;
 }

if(kodmz.ravno()[0] != '\0')
 {
  fprintf(ff,gettext("По материалу %s\n"),kodmz.ravno());
  fprintf(fil_csv.ff,gettext("По материалу %s\n"),kodmz.ravno());
  fprintf(ffuz,gettext("По материалу %s\n"),kodmz.ravno());
  fprintf(ffuzcp,gettext("По материалу %s\n"),kodmz.ravno());
  kolstr_uz++;
  kst++;
 }
if(ndsc.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("НДС"),ndsc.ravno());
  fprintf(fil_csv.ff,"%s:%s\n",gettext("НДС"),ndsc.ravno());
  fprintf(ffuz,"%s:%s\n",gettext("НДС"),ndsc.ravno());
  fprintf(ffuzcp,"%s:%s\n",gettext("НДС"),ndsc.ravno());
  kolstr_uz++;
  kst++;
 }

skl=0;
sli=1;
itg1=itg2=itg3=itg4=itg5=itg6=itg7=itg8=0.;
k1=s1=k2=s2=k3=s3=k4=s4=0.;
kk1=ss1=kk2=ss2=kk3=ss3=kk4=ss4=0.;
kg0=kg1=kg2=kg3=0.;
kgs0=kgs1=kgs2=kgs3=0.;
kgrmzp=0;
memset(str,'\0',sizeof(str));
memset(str,'.',233);
mvsh=kolstr1=0;
short dp,mp,gp;
short metka_sp_uz=0; //метка первого вывода шапки узкой форми
class ostatokt ost;
while(curtmp.read_cursor(&rowtmp) != 0)
 {
  strzag(LINES-1,0,kolstrtmp,++kolstr1);
  shb.new_plus(rowtmp[0]);
  skl1=atoi(rowtmp[1]);
  kgrm=atoi(rowtmp[2]);
  naim.new_plus(rowtmp[3]);
  kodm=atoi(rowtmp[4]);
  nk=atoi(rowtmp[5]);
  /*Остаток по карточкам*/
  ostkar(dn,mn,gn,dk,mk,gk,skl1,nk,&ost);

  if(ost.ostm[3] < -0.0000009)
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Отрицательный остаток !"));
    
    sprintf(bros,"%s:%.10g %s:%d.%d.%d",gettext("Остаток"),ost.ostm[3],
    gettext("На дату"),dk,mk,gk);

    SOOB.VVOD_SPISOK_add_MD(bros);
    
    sprintf(bros,"%s:%d %s:%d",gettext("Склад"),skl1,
    gettext("Номер карточки"),nk);

    SOOB.VVOD_SPISOK_add_MD(bros);

    soobshw(&SOOB,stdscr,-1,-1,0,1);
   }

  if(ost.ostm[0] == 0. && ost.ostm[1] == 0. && ost.ostm[3] == 0. && fabs(ost.ostmc[3]) < 0.000009)
     continue;
  

  ei.new_plus(rowtmp[6]);
  cena=atof(rowtmp[7]);
  cena_real=atof(rowtmp[11]);

  nds=atof(rowtmp[8]);
  strncpy(bros,rowtmp[9],sizeof(bros)-1);

  if(atoi(bros) == 0)
   mnds=' ';
  if(atoi(bros) == 1)
   mnds='+';
      
  if(kgrmzp != kgrm)
   {
    if(kgrmzp != 0)
     {
      if(sort == 1)
      if(itg1 != 0. || itg2 != 0. || itg3 != 0. || itg4 != 0.)
       {
        itgrup(kgrmzp,itg1,itg2,itg3,itg4,itg5,itg6,itg7,itg8,ff);
        itgrup_uz(kgrmzp,itg1,itg2,itg3,itg4,itg5,itg6,itg7,itg8,ffuz,ffuzcp,dn,mn,gn,dk,mk,gk,&nom_list_uz,&kolstr_uz,skl,naiskl.ravno(),itgr_svcr);
        kst+=2;
       }
     }
    memset(itgr_svcr,'\0',sizeof(itgr_svcr));
    itg1=itg2=itg3=itg4=itg5=itg6=itg7=itg8=0.;
    kgrmzp=kgrm;
   }

  if(SRAV(shb.ravno(),shbm.ravno(),1) != 0)
   {
    if(shbm.ravno()[0] != '\0')
     {
        if(sort == 1)
        if(itg1 != 0. || itg2 != 0. || itg3 != 0. || itg4 != 0.)
         {
          itgrup(kgrmzp,itg1,itg2,itg3,itg4,itg5,itg6,itg7,itg8,ff);
          itgrup_uz(kgrmzp,itg1,itg2,itg3,itg4,itg5,itg6,itg7,itg8,ffuz,ffuzcp,dn,mn,gn,dk,mk,gk,&nom_list_uz,&kolstr_uz,skl,naiskl.ravno(),itgr_svcr);
          memset(itgr_svcr,'\0',sizeof(itgr_svcr));
          itg1=itg2=itg3=itg4=itg5=itg6=itg7=itg8=0.;
          kst+=2;
         }
	itskl(skl,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,ff,ffitog,ffitog1);

        itskl_uz(skl,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,dn,mn,gn,dk,mk,gk,&nom_list_uz,&kolstr_uz,itsk_svcr,ffuz,ffuzcp);
        kst+=2;
        if(ostkg == 1 && fabs((kk1+kk2+kk3+kk4) - (kg0+kg1+kg2+kg3)) > 0.0001 &&
        kg0+kg1+kg2+kg3 > 0.0001)
         { 
          itkg(shbm.ravno(),skl,kg0,kg1,kg2,kg3,1,ff);
          itkg_uz(shbm.ravno(),skl,kg0,kg1,kg2,kg3,1,dn,mn,gn,dk,mk,gk,&nom_list_uz,&kolstr_uz,naiskl.ravno(),ffuz,ffuzcp);
          kst+=2;
          kg0=kg1=kg2=kg3=0.;
         }
	itsh(shbm.ravno(),k1,k2,k3,k4,s1,s2,s3,s4,ff,ffitog,ffitog1);

        itsh_uz(shbm.ravno(),k1,k2,k3,k4,s1,s2,s3,s4,dn,mn,gn,dk,mk,gk,&nom_list_uz,&kolstr_uz,skl,naiskl.ravno(),itsh_svcr,ffuz,ffuzcp);
        kst+=2;
        if(ostkg == 1 && fabs((k1+k2+k3+k4) - (kgs0+kgs1+kgs2+kgs3)) > 0.0001 &&
         kgs0+kgs1+kgs2+kgs3 > 0.0001)
         { 
          itkg(shbm.ravno(),skl,kgs0,kgs1,kgs2,kgs3,0,ff);
          itkg_uz(shbm.ravno(),skl,kgs0,kgs1,kgs2,kgs3,0,dn,mn,gn,dk,mk,gk,&nom_list_uz,&kolstr_uz,naiskl.ravno(),ffuz,ffuzcp);
          kst+=2;
          kgs0=kgs1=kgs2=kgs3=0.;
         }
	skl=0;


     }
    k1=s1=k2=s2=k3=s3=k4=s4=0.;
    memset(itsh_svcr,'\0',sizeof(itsh_svcr));
    /*Читаем наименование счета*/
    naimshet.new_plus("");
    sprintf(bros,"select nais from Plansh where ns='%s'",shb.ravno());
    if(sql_readkey(&bd,bros,&row,&cur1) != 1)
     {
      beep();
      printw(gettext("Не найден счёт %s в плане счетов !"),shb.ravno());
      printw("\n");
      fprintf(ff,gettext("Не найден счёт %s в плане счетов !"),shb.ravno());
      fprintf(ff,"\n");
      OSTANOV();
     }
    else
      naimshet.new_plus(row[0]);       

    printw("\n%s %s \"%s\"\n",gettext("Счёт"),shb.ravno(),naimshet.ravno());
    refresh();

    if(kst > kol_strok_na_liste - 10)
     {
      fprintf(ff,"\f");
      kst=0;
      sli++;
     }


    fprintf(ff,"\n%s %s \"%s\"\n",gettext("Счёт"),shb.ravno(),naimshet.ravno());

    dvtmcf1_sh_uz(dn,mn,gn,dk,mk,gk,&nom_list_uz,&kolstr_uz,skl,naiskl.ravno(),ffuz,ffuzcp);
    fprintf(ffuz,"\n");
    fprintf(ffuzcp,"\n");

    dvtmcf1_sh_uz(dn,mn,gn,dk,mk,gk,&nom_list_uz,&kolstr_uz,skl,naiskl.ravno(),ffuz,ffuzcp);
    fprintf(ffuz,"%s %s \"%s\"\n",gettext("Счёт"),shb.ravno(),naimshet.ravno());
    fprintf(ffuzcp,"%s %s \"%s\"\n",gettext("Счёт"),shb.ravno(),naimshet.ravno());

    if(ffitog  != NULL)
      fprintf(ffitog,"\n%s %s \"%s\"\n",gettext("Счёт"),shb.ravno(),naimshet.ravno());
    if(ffitog1 != NULL)
      fprintf(ffitog1,"\n%s %s \"%s\"\n",gettext("Счёт"),shb.ravno(),naimshet.ravno());
    kst+=2; 
    gsapp(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl.ravno(),ff);

    mvsh=1;

    shbm.new_plus(shb.ravno());
   }

  if(skl != skl1)
   {
    if(skl != 0)
     {
        if(sort == 1)
        if(itg1 != 0. || itg2 != 0. || itg3 != 0. || itg4 != 0.)
         {
          itgrup(kgrmzp,itg1,itg2,itg3,itg4,itg5,itg6,itg7,itg8,ff);
          itgrup_uz(kgrmzp,itg1,itg2,itg3,itg4,itg5,itg6,itg7,itg8,ffuz,ffuzcp,dn,mn,gn,dk,mk,gk,&nom_list_uz,&kolstr_uz,skl,naiskl.ravno(),itgr_svcr);
          memset(itgr_svcr,'\0',sizeof(itgr_svcr));
          itg1=itg2=itg3=itg4=itg5=itg6=itg7=itg8=0.;
          kst+=2;
         }

	itskl(skl,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,ff,ffitog,ffitog1);

        itskl_uz(skl,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,dn,mn,gn,dk,mk,gk,&nom_list_uz,&kolstr_uz,itsk_svcr,ffuz,ffuzcp);
        kst+=2;
        if(ostkg == 1 && fabs((kk1+kk2+kk3+kk4) - (kg0+kg1+kg2+kg3)) > 0.0001 &&
        kg0+kg1+kg2+kg3 > 0.0001)
         { 
          itkg(shbm.ravno(),skl,kg0,kg1,kg2,kg3,1,ff);
          itkg_uz(shbm.ravno(),skl,kg0,kg1,kg2,kg3,1,dn,mn,gn,dk,mk,gk,&nom_list_uz,&kolstr_uz,naiskl.ravno(),ffuz,ffuzcp);
          kst+=2;
          kg0=kg1=kg2=kg3=0.;
         }
     }
    kk1=ss1=kk2=ss2=kk3=ss3=kk4=ss4=0.;
    memset(itsk_svcr,'\0',sizeof(itsk_svcr));
    /*Читаем наименование склада*/
    naiskl.new_plus("");
    sprintf(bros,"select naik from Sklad where kod='%d'",skl1);
    if(sql_readkey(&bd,bros,&row,&cur1) != 1)
     {
      beep();
      printw(gettext("Не найден склад %d в списке складов"),skl1);
      printw("\n");
      fprintf(ff,gettext("Не найден склад %d в списке складов"),skl1);
      fprintf(ff,"\n");
      OSTANOV();
     }
    else
      naiskl.new_plus(row[0]);

    if(metka_sp_uz == 0)
     { 
      metka_sp_uz++;
      dvtmcf1_sap_uz(dn,mn,gn,dk,mk,gk,&nom_list_uz,&kolstr_uz,skl1,naiskl.ravno(),ffuz,ffuzcp); //узкая распечтака*/
     }
    else
     {
      dvtmcf1_sh_uz(dn,mn,gn,dk,mk,gk,&nom_list_uz,&kolstr_uz,skl,naiskl.ravno(),ffuz,ffuzcp);

      fprintf(ffuz,"%s: %d %s -------------------------------------------------------\n",gettext("Склад"),skl1,naiskl.ravno());
      fprintf(ffuzcp,"%s: %d %s -------------------------------------------------------\n",gettext("Склад"),skl1,naiskl.ravno());
     }
    printw("%s: %d %s\n",gettext("Склад"),skl1,naiskl.ravno());
    refresh();

    
    if(kst != 0)
    if(kst > kol_strok_na_liste - 10)
     {
      fprintf(ff,"\f");
      kst=0;
      sli++;
     }
    mvsh=1;
    skl=skl1;

   }
  if(mvsh == 1)
   {
      shdtmcf1(dn,mn,gn,dk,mk,gk,sli,ff,skl,naiskl.ravno());
      kst+=6; 
    mvsh=0;
   }

  
  sprintf(nomn,"%s.%d.%d",shb.ravno(),kgrm,nk);


    gsapp(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl.ravno(),ff);

    fprintf(ff,"\
%5d %-*.*s %-*s %-*s %c%2.2g %14.10g %15.10g %15.2f %15.10g %15.2f %15.10g %15.2f \
%15.10g %15.2f\n",kodm,
    iceb_tu_kolbait(40,naim.ravno()),iceb_tu_kolbait(40,naim.ravno()),naim.ravno(),
    iceb_tu_kolbait(13,nomn),nomn,
    iceb_tu_kolbait(10,ei.ravno()),ei.ravno(),
    mnds,nds,cena,
    ost.ostm[0],ost.ostmc[0],ost.ostm[1],ost.ostmc[1],ost.ostm[2],ost.ostmc[2],ost.ostm[3],ost.ostmc[3]);

    fprintf(fil_csv.ff,"\
%d|%s|%s|%s|%c%.2g|%.10g|%.10g|%.2f|%.10g|%.2f|%.10g|%.2f|\
%.10g|%.2f\n",kodm,
    naim.ravno(),
    nomn,
    ei.ravno(),
    mnds,nds,
    cena,
    ost.ostm[0],ost.ostmc[0],ost.ostm[1],ost.ostmc[1],ost.ostm[2],ost.ostmc[2],ost.ostm[3],ost.ostmc[3]);
    
    dp=mp=gp=0;    
    //Узнаём дату последнего прихода в карточку
    sprintf(bros,"select datdp from Zkart where nomd != '000' and sklad=%d and nomk=%d and tipz=1 order by datdp desc limit 1",
    skl1,nk);
    
    if(readkey(bros,&row,&cur) > 0)
     rsdat(&dp,&mp,&gp,row[0],2);
     
    dvtmcf1_sh_uz(dn,mn,gn,dk,mk,gk,&nom_list_uz,&kolstr_uz,skl,naiskl.ravno(),ffuz,ffuzcp);

    fprintf(ffuz,"\
%5d %-*.*s %-*s %-*s %c%2.2g %10s %8.8g %8.2f %8.8g %8.2f %8.8g %8.2f \
%8.8g %8.2f",
    kodm,
    iceb_tu_kolbait(23,naim.ravno()),iceb_tu_kolbait(23,naim.ravno()),naim.ravno(),
    iceb_tu_kolbait(13,nomn),nomn,
    iceb_tu_kolbait(3,ei.ravno()),ei.ravno(),
    mnds,nds,prcn(cena),
    ost.ostm[0],ost.ostmc[0],ost.ostm[1],ost.ostmc[1],ost.ostm[2],ost.ostmc[2],ost.ostm[3],ost.ostmc[3]);

    fprintf(ffuzcp,"\
%5d %-*.*s %-*s %-*s %c%2.2g %10s %8.8g %8.2f %8.8g %8.2f %8.8g %8.2f \
%8.8g %8.2f",
    kodm,
    iceb_tu_kolbait(23,naim.ravno()),iceb_tu_kolbait(23,naim.ravno()),naim.ravno(),
    iceb_tu_kolbait(13,nomn),nomn,
    iceb_tu_kolbait(3,ei.ravno()),ei.ravno(),
    mnds,nds,prcn(cena_real),
    ost.ostm[0],ost.ostmc_real[0],ost.ostm[1],ost.ostmc_real[1],ost.ostm[2],ost.ostmc_real[2],ost.ostm[3],ost.ostmc_real[3]);
    
    if(dp != 0)
     {
      fprintf(ffuz," %02d.%02d.%d",dp,mp,gp);
      fprintf(ffuzcp," %02d.%02d.%d",dp,mp,gp);
     }
    fprintf(ffuz,"\n");
    fprintf(ffuzcp,"\n");
    
    if(iceb_tu_strlen(naim.ravno()) > 23)
     {
      dvtmcf1_sh_uz(dn,mn,gn,dk,mk,gk,&nom_list_uz,&kolstr_uz,skl,naiskl.ravno(),ffuz,ffuzcp);
      fprintf(ffuz,"%5s %s\n","",iceb_tu_adrsimv(23,naim.ravno()));
      fprintf(ffuzcp,"%5s %s\n","",iceb_tu_adrsimv(23,naim.ravno()));
     }
     if(ostkg == 1)
      {
       ost1=ost2=ost3=ost4=0.;
       fas=0.;
       fas=atof(rowtmp[10]);
       if(fas > 0.0000001)
        { 
         ost1=ost.ostm[0]*fas;
         ost2=ost.ostm[1]*fas;
         ost3=ost.ostm[2]*fas;
         ost4=ost.ostm[3]*fas;
         kg0+=ost1;
         kg1+=ost2;
         kg2+=ost3;
         kg3+=ost4;
         kgs0+=ost1;
         kgs1+=ost2;
         kgs2+=ost3;
         kgs3+=ost4;

         gsapp(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl.ravno(),ff);

         fprintf(ff,"\
%5s %-40.40s %-13s %-10s %3s %14s %15.10g %15s %15.10g %15s %15.10g %15s \
%15.10g %15s\n"," "," "," "," "," "," ",ost1," ",ost2," ",ost3," ",ost4," ");
         fprintf(fil_csv.ff,"\
%s|%s|%s|%s|%s|%s|%.10g|%s|%.10g|%s|%.10g|%s|\
%.10g|%s\n"," "," "," "," "," "," ",ost1," ",ost2," ",ost3," ",ost4," ");

         dvtmcf1_sh_uz(dn,mn,gn,dk,mk,gk,&nom_list_uz,&kolstr_uz,skl,naiskl.ravno(),ffuz,ffuzcp);

         fprintf(ffuz,"\
%5s %-23.23s %-13s %-3s %3s %10s %8.8g %8s %8.8g %8s %8.8g %8s \
%8.8g %8s\n"," "," "," "," "," "," ",ost1," ",ost2," ",ost3," ",ost4," ");

         fprintf(ffuzcp,"\
%5s %-23.23s %-13s %-3s %3s %10s %8.8g %8s %8.8g %8s %8.8g %8s \
%8.8g %8s\n"," "," "," "," "," "," ",ost1," ",ost2," ",ost3," ",ost4," ");

        } 
       else
         if(SRAV(ei.ravno(),"Кг",0) == 0 || SRAV(ei.ravno(),"кг",0) == 0 ||
          SRAV(ei.ravno(),"КГ",0) == 0)
          {
           kg0+=ost.ostm[0];
           kg1+=ost.ostm[1];
           kg2+=ost.ostm[2];
           kg3+=ost.ostm[3];
           kgs0+=ost.ostm[0];
           kgs1+=ost.ostm[1];
           kgs2+=ost.ostm[2];
           kgs3+=ost.ostm[3];
          }             
      }     

  k1+=ost.ostm[0];   kk1+=ost.ostm[0]; itg1+=ost.ostm[0];
  s1+=ost.ostmc[0];  ss1+=ost.ostmc[0]; itg5+=ost.ostmc[0];

  k2+=ost.ostm[1];   kk2+=ost.ostm[1];  itg2+=ost.ostm[1];
  s2+=ost.ostmc[1];  ss2+=ost.ostmc[1]; itg6+=ost.ostmc[1];

  k3+=ost.ostm[2];   kk3+=ost.ostm[2];    itg3+=ost.ostm[2];
  s3+=ost.ostmc[2];  ss3+=ost.ostmc[2];   itg7+=ost.ostmc[2];

  k4+=ost.ostm[3];   kk4+=ost.ostm[3];    itg4+=ost.ostm[3];
  s4+=ost.ostmc[3];  ss4+=ost.ostmc[3];   itg8+=ost.ostmc[3];

  itogo_kol[0]+=ost.ostm[0];
  itogo_kol[1]+=ost.ostm[1];
  itogo_kol[2]+=ost.ostm[2];
  itogo_kol[3]+=ost.ostm[3];

  itogo_sum[0]+=ost.ostmc[0];
  itogo_sum[1]+=ost.ostmc[1];
  itogo_sum[2]+=ost.ostmc[2];
  itogo_sum[3]+=ost.ostmc[3];

  /*итоги в ценах реализации*/
  itgr_svcr[0]+=ost.ostmc_real[0];
  itgr_svcr[1]+=ost.ostmc_real[1];
  itgr_svcr[2]+=ost.ostmc_real[2];
  itgr_svcr[3]+=ost.ostmc_real[3];

  itsh_svcr[0]+=ost.ostmc_real[0];
  itsh_svcr[1]+=ost.ostmc_real[1];
  itsh_svcr[2]+=ost.ostmc_real[2];
  itsh_svcr[3]+=ost.ostmc_real[3];

  itog_svcr[0]+=ost.ostmc_real[0];
  itog_svcr[1]+=ost.ostmc_real[1];
  itog_svcr[2]+=ost.ostmc_real[2];
  itog_svcr[3]+=ost.ostmc_real[3];

  itsk_svcr[0]+=ost.ostmc_real[0];
  itsk_svcr[1]+=ost.ostmc_real[1];
  itsk_svcr[2]+=ost.ostmc_real[2];
  itsk_svcr[3]+=ost.ostmc_real[3];
 }

if(sort == 1)
 {
  itgrup(kgrmzp,itg1,itg2,itg3,itg4,itg5,itg6,itg7,itg8,ff);
  itgrup_uz(kgrmzp,itg1,itg2,itg3,itg4,itg5,itg6,itg7,itg8,ffuz,ffuzcp,dn,mn,gn,dk,mk,gk,&nom_list_uz,&kolstr_uz,skl,naiskl.ravno(),itgr_svcr);
 }
itskl(skl,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,ff,ffitog,ffitog1);

itskl_uz(skl,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,dn,mn,gn,dk,mk,gk,&nom_list_uz,&kolstr_uz,itsk_svcr,ffuz,ffuzcp);

if(ostkg == 1 && fabs((kk1+kk2+kk3+kk4) - (kg0+kg1+kg2+kg3)) > 0.0001 &&
kg0+kg1+kg2+kg3 > 0.0001)
 {
  itkg(shbm.ravno(),skl,kg0,kg1,kg2,kg3,1,ff);
  itkg_uz(shbm.ravno(),skl,kg0,kg1,kg2,kg3,1,dn,mn,gn,dk,mk,gk,&nom_list_uz,&kolstr_uz,naiskl.ravno(),ffuz,ffuzcp);
 }

itsh(shbm.ravno(),k1,k2,k3,k4,s1,s2,s3,s4,ff,ffitog,ffitog1);
itsh_uz(shbm.ravno(),k1,k2,k3,k4,s1,s2,s3,s4,dn,mn,gn,dk,mk,gk,&nom_list_uz,&kolstr_uz,skl,naiskl.ravno(),itsh_svcr,ffuz,ffuzcp);

if(ostkg == 1 && fabs((k1+k2+k3+k4) - (kgs0+kgs1+kgs2+kgs3)) > 0.0001 &&
kgs0+kgs1+kgs2+kgs3 > 0.0001)
 {   
  itkg(shbm.ravno(),skl,kgs0,kgs1,kgs2,kgs3,0,ff);
  itkg_uz(shbm.ravno(),skl,kgs0,kgs1,kgs2,kgs3,0,dn,mn,gn,dk,mk,gk,&nom_list_uz,&kolstr_uz,naiskl.ravno(),ffuz,ffuzcp);
 }

fprintf(ff,"\n\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
%5s %-*.*s %-13s %-10s %14s %-4s %14.10g %15.2f %15.10g %15.2f %15.10g %15.2f \
%15.10g %15.2f\n",
" ",
iceb_tu_kolbait(40,gettext("Общий итог")),
iceb_tu_kolbait(40,gettext("Общий итог")),
gettext("Общий итог"),
" "," "," "," ",
itogo_kol[0],
itogo_sum[0],
itogo_kol[1],
itogo_sum[1],
itogo_kol[2],
itogo_sum[2],
itogo_kol[3],
itogo_sum[3]);


dvtmcf1_sh_uz(dn,mn,gn,dk,mk,gk,&nom_list_uz,&kolstr_uz,skl,naiskl.ravno(),ffuz,ffuzcp);
fprintf(ffuz,"\n\
--------------------------------------------------------------------------------------------------------------------------------------------------\n");

dvtmcf1_sh_uz(dn,mn,gn,dk,mk,gk,&nom_list_uz,&kolstr_uz,skl,naiskl.ravno(),ffuz,ffuzcp);
fprintf(ffuz,"\
%*s %-13s %-3s %3s %10s %8.8g %8.2f %8.8g %8.2f %8.8g %8.2f \
%8.8g %8.2f\n",iceb_tu_kolbait(28,gettext("Общий итог")),gettext("Общий итог")," "," "," "," ",
  itogo_kol[0],
  itogo_sum[0],
  itogo_kol[1],
  itogo_sum[1],
  itogo_kol[2],
  itogo_sum[2],
  itogo_kol[3],
  itogo_sum[3]);

fprintf(ffuzcp,"\n\
--------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ffuzcp,"\
%*s %-13s %-3s %3s %10s %8.8g %8.2f %8.8g %8.2f %8.8g %8.2f \
%8.8g %8.2f\n",iceb_tu_kolbait(28,gettext("Общий итог")),gettext("Общий итог")," "," "," "," ",
itogo_kol[0],
itog_svcr[0],
itogo_kol[1],
itog_svcr[1],
itogo_kol[2],
itog_svcr[2],
itogo_kol[3],
itog_svcr[3]);


podpis(ffitog);
fclose(ffitog);

podpis(ffitog1);
fclose(ffitog1);

podpis(ffuz);
fclose(ffuz);

podpis(ffuzcp);
fclose(ffuzcp);


podpis(ff);
fclose(ff);

fil_csv.end();


printw_vr(tmmn);


OSTANOV();
 

  
oth->spis_imaf.plus(imaf);

sprintf(bros,"%s (A3)",gettext("Оборотная ведомость движения материальных ценностей"));
oth->spis_naim.plus(bros);

oth->spis_imaf.plus(imafuz);
sprintf(bros,"%s (A4)",gettext("Оборотная ведомость движения материальных ценностей в ценах учёта"));
oth->spis_naim.plus(bros);

oth->spis_imaf.plus(imafuzcp);
sprintf(bros,"%s (A4)",gettext("Оборотная ведомость движения материальных ценностей в ценах реализации"));
oth->spis_naim.plus(bros);

oth->spis_imaf.plus(imafitog);
oth->spis_naim.plus(gettext("Итоги по складам и счетам"));

oth->spis_imaf.plus(imafitog1);
oth->spis_naim.plus(gettext("Остатки по складам и счетам"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),1);

oth->spis_imaf.plus(imaf_csv);
oth->spis_naim.plus(gettext("Оборотная ведомость для загрузки в электронные таблицы"));

return(0);

}

/*******/
/*Счетчик*/
/*******/
void gsapp(short dn,short mn,short gn,short dk,short mk,
short gk,
short *sl, //Счетчик листов
short *kst, //Счетчик строк
int skl,const char *nai,
FILE *ff)
{

*kst+=1;
/*
printw("gsapp kst=%d sl=%d\n",*kst,*sl);
refresh();
*/
if(*kst >= kol_strok_na_liste)
 {
  fprintf(ff,"\f");

  *sl+=1;
  shdtmcf1(dn,mn,gn,dk,mk,gk,*sl,ff,skl,nai);
  *kst=6;  
 }

}

/*****************/
/*Итого по складу*/
/*****************/
void            itskl(int skl,double kk1,double kk2,double kk3,
double kk4,double ss1,double ss2,double ss3,double ss4,
FILE *ff,FILE *ffitog,FILE *ffitog1)
{
char		bros[512];
class iceb_tu_str naim_skl("");
SQL_str row;
SQLCURSOR cur;


sprintf(bros,"select naik from Sklad where kod='%d'",skl);
if(sql_readkey(&bd,bros,&row,&cur) == 1)
  naim_skl.new_plus(row[0]);
  
sprintf(bros,"%s %d",gettext("Итого по складу"),skl);


fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
%5s %-*.*s %-13s %-10s %14s %-4s %14.10g %15.2f %15.10g %15.2f %15.10g %15.2f \
%15.10g %15.2f\n",
" ",
iceb_tu_kolbait(40,bros),iceb_tu_kolbait(40,bros),bros,
" "," "," "," ",
kk1,ss1,kk2,ss2,kk3,ss3,kk4,ss4);

printw("%s %d %s:\n\
%s %15.10g %15.2f\n\
%s %15.10g %15.2f\n\
%s %15.10g %15.2f\n\
%s %15.10g %15.2f\n\n",
gettext("Итого по складу:"),
skl,naim_skl.ravno(),
gettext("Остаток:"),
kk1,ss1,
gettext("Приход :"),
kk2,ss2,
gettext("Расход :"),
kk3,ss3,
gettext("Остаток:"),
kk4,ss4);


if(ffitog != NULL)
  fprintf(ffitog,"%s %d %s:\n\
%s %15.10g %15.2f\n\
%s %15.10g %15.2f\n\
%s %15.10g %15.2f\n\
%s %15.10g %15.2f\n\n",
gettext("Итого по складу:"),
skl,naim_skl.ravno(),
gettext("Остаток:"),
kk1,ss1,
gettext("Приход :"),
kk2,ss2,
gettext("Расход :"),
kk3,ss3,
gettext("Остаток:"),
kk4,ss4);

if(ffitog1 != NULL)
  fprintf(ffitog1,"%s %d %s:\n\
%s %15.10g %15.2f\n\n",
gettext("Итого по складу:"),
skl,naim_skl.ravno(),
gettext("Остаток:"),
kk4,ss4);

}

/****************/
/*Итого по счёту*/
/****************/
void itsh(const char *sh,double k1,double k2,double k3,double k4,
double s1,double s2,double s3,double s4,
FILE *ff,FILE *ffitog,FILE *ffitog1)
{
char		bros[512];

sprintf(bros,"%s %s",gettext("Итого по счёту"),sh);


fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
%5s %-*.*s %-13s %-10s %14s %-4s %14.10g %15.2f %15.10g %15.2f %15.10g %15.2f \
%15.10g %15.2f\n",
" ",
iceb_tu_kolbait(40,bros),iceb_tu_kolbait(40,bros),bros,
" "," "," "," ",
k1,s1,k2,s2,k3,s3,k4,s4);

printw("%s %s:\n\
%s %15.10g %15.2f\n\
%s %15.10g %15.2f\n\
%s %15.10g %15.2f\n\
%s %15.10g %15.2f\n\n",
gettext("Итого по счёту:"),
sh,
gettext("Остаток:"),
k1,s1,
gettext("Приход :"),
k2,s2,
gettext("Расход :"),
k3,s3,
gettext("Остаток:"),
k4,s4);

if(ffitog != NULL)
  fprintf(ffitog,"%s %s:\n\
%s %15.10g %15.2f\n\
%s %15.10g %15.2f\n\
%s %15.10g %15.2f\n\
%s %15.10g %15.2f\n\n",
gettext("Итого по счёту:"),
sh,
gettext("Остаток:"),
k1,s1,
gettext("Приход :"),
k2,s2,
gettext("Расход :"),
k3,s3,
gettext("Остаток:"),
k4,s4);

if(ffitog1 != NULL)
  fprintf(ffitog1,"%s %s:\n\
%s %15.10g %15.2f\n\n",
gettext("Итого по счёту:"),
sh,
gettext("Остаток:"),
k4,s4);


}
/****************/
/*Итого в килограммах по счёту и по складу*/
/****************/
void            itkg(const char *sh,int skl,double k1,double k2,double k3,
double k4,
short met, //0-по счету 1-по складу*/
FILE *ff)
{
char		bros[512];
if(met == 1)
  sprintf(bros,"%s %d %s",gettext("Итого по складу"),
  skl,
  gettext("Кг."));

if(met == 0)
  sprintf(bros,"%s %s %s",
  gettext("Итого по счёту"),
  sh,
  gettext("Кг."));

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
%5s %-*.*s %-13s %-10s %4s %14s %15.10g %15s %15.10g %15s %15.10g %15s \
%15.10g %15s\n",
" ",
iceb_tu_kolbait(40,bros),iceb_tu_kolbait(40,bros),bros,
" "," "," "," ",
k1," ",k2," ",k3," ",k4," ");

}

/*******/
/*Шапка*/
/*******/
void shdtmcf1(short dn,short mn,short gn,short dk,short mk,
short gk,int sli,FILE *ff,int skl,const char *nai)
{
char		bros[512];

sprintf(bros,"%s: %d %s",gettext("Склад"),skl,nai);
fprintf(ff,"%-*s %s%d\n",iceb_tu_kolbait(196,bros),bros,gettext("Лист N"),sli);
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("     |             Наименование               |  Номенкла-  | Единица  |   |              | Остаток на  %02d.%02d.%4dг.      |     Приход   за период        |    Расход  за период          |  Остаток на %02d.%02d.%4dг.      |\n"),dn,mn,gn,dk,mk,gk);
fprintf(ff,gettext(" Код |        товарно-материальных            |турний номер | измерения|НДС|   Ц е н а    |-------------------------------|------------------------------------------------------------------------------------------------\n"));
fprintf(ff,gettext("     |             ценностей                  |             |          |   |              |   Количество  |  Сумма        |    Количество |     Сумма     |    Количество |     Сумма     |   Количество  |    Сумма      |\n"));


fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

}

/*****************/
/*Итого по группе*/
/*****************/
void            itgrup(int kodgr,double kk1,double kk2,double kk3,
double kk4,double ss1,double ss2,double ss3,double ss4,
FILE *ff)
{
char		bros[512];
class iceb_tu_str naigr("");
SQL_str         row;
SQLCURSOR       cur;

/*Читаем наименование группы*/
sprintf(bros,"select naik from Grup where kod=%d",kodgr);
if(sql_readkey(&bd,bros,&row,&cur) != 1)
 {
  beep();
  printw("%s %d !",gettext("Не нашли группу"),kodgr);
  OSTANOV();
 }
else
 naigr.new_plus(row[0]);

sprintf(bros,"%s %d %s:",gettext("Итого по группе"),kodgr,naigr.ravno());


fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
%*s %14.10g %15.2f %15.10g %15.2f %15.10g %15.2f \
%15.10g %15.2f\n",iceb_tu_kolbait(90,bros),bros,
kk1,ss1,kk2,ss2,kk3,ss3,kk4,ss4);

}
/*************************************/
/*Шапка для узкой форми*/
/*************************************/
void dvtmcf1_sap_uz(short dn,short mn,short gn,short dk,short mk,
short gk,int *nom_list_uz,int *kolstr_uz,int skl,const char *nai,
FILE *ffuz,FILE *ffuzcp) //узкая распечтака*/
{
char bros[512];

*nom_list_uz+=1;
*kolstr_uz+=6;

sprintf(bros,"%s: %d %s %s %02d.%02d.%04d%s => %02d.%02d.%04d%s",
gettext("Склад"),skl,nai,
gettext("Период с"),
dn,mn,gn,
gettext("г."),
dk,mk,gk,
gettext("г."));
  
fprintf(ffuz,"%-*s %s%d\n",iceb_tu_kolbait(120,bros),bros,gettext("Лист N"),*nom_list_uz);

fprintf(ffuz,"\
--------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ffuz,gettext("\
     |     Наименование      |  Номенкла-  |Ед.|   |          |     Остаток     |      Приход     |      Расход     |     Остаток     |  Дата    |\n\
 Код |товарно-материальных   |турный номер |изм|НДС| Ц е н а  |------------------------------------------------------------------------последнего|\n\
     |     ценностей         |             |   |   |          |Колич.  |  Сумма |Колич.  | Сумма  |Колич.  | Сумма  |Колич.  | Сумма  | получения|\n"));

fprintf(ffuz,"\
--------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ffuzcp,"%-*s %s%d\n",iceb_tu_kolbait(120,bros),bros,gettext("Лист N"),*nom_list_uz);

fprintf(ffuzcp,"\
--------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ffuzcp,gettext("\
     |     Наименование      |  Номенкла-  |Ед.|   |          |     Остаток     |      Приход     |      Расход     |     Остаток     |  Дата    |\n\
 Код |товарно-материальных   |турный номер |изм|НДС| Ц е н а  |------------------------------------------------------------------------последнего|\n\
     |     ценностей         |             |   |   |          |Колич.  |  Сумма |Колич.  | Сумма  |Колич.  | Сумма  |Колич.  | Сумма  | получения|\n"));

fprintf(ffuzcp,"\
--------------------------------------------------------------------------------------------------------------------------------------------------\n");
/*
1234 12345678901234567890123 1234567890123 123 123 1234567890 12345678 12345678 12345678 12345678 12345678 12345678 12345678 12345678 1234567890
*/

}


/*******/
/*Счетчик для узкой форми*/
/*******/
void dvtmcf1_sh_uz(short dn,short mn,short gn,short dk,short mk,
short gk,
int *sl, //Счетчик листов
int *kst, //Счетчик строк
int skl,const char *nai,
FILE *ffuz,FILE *ffuzcp) //узкая распечтака
{

*kst+=1;
/*
printw("gsapp kst=%d sl=%d\n",*kst,*sl);
refresh();
*/
if(*kst > kol_strok_na_liste)
 {
  fprintf(ffuz,"\f");
  fprintf(ffuzcp,"\f");
  *kst=0;
  dvtmcf1_sap_uz(dn,mn,gn,dk,mk,gk,sl,kst,skl,nai,ffuz,ffuzcp); //узкая распечтака*/
  *kst+=1;  //1 шапка плюс следующая строка
  
 }

}

/*****************/
/*Итого по группе*/
/*****************/
void  itgrup_uz(int kodgr,double kk1,double kk2,double kk3,
double kk4,double ss1,double ss2,double ss3,double ss4,
FILE *ffuz,  //узкая распечтака
FILE *ffuzcp,  //узкая распечтака
short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,
int *kst,
int skl,
const char *nai,
double *itgr_svcr)
{
char		bros[512];
class iceb_tu_str naigr("");
SQL_str         row;
SQLCURSOR       cur;

/*Читаем наименование группы*/
sprintf(bros,"select naik from Grup where kod=%d",kodgr);
if(sql_readkey(&bd,bros,&row,&cur) != 1)
 {
  beep();
  printw("%s %d !",gettext("Не нашли группу"),kodgr);
  OSTANOV();
 }
else
 naigr.new_plus(row[0]);

sprintf(bros,"%s %d %s:",gettext("Итого по группе"),kodgr,naigr.ravno());


dvtmcf1_sh_uz(dn,mn,gn,dk,mk,gk,sl,kst,skl,nai,ffuz,ffuzcp);
fprintf(ffuz,"\
--------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ffuzcp,"\
--------------------------------------------------------------------------------------------------------------------------------------------------\n");

dvtmcf1_sh_uz(dn,mn,gn,dk,mk,gk,sl,kst,skl,nai,ffuz,ffuzcp);

fprintf(ffuz,"\
%*s %8.8g %8.2f %8.8g %8.2f %8.8g %8.2f \
%8.8g %8.2f\n",iceb_tu_kolbait(62,bros),bros,
kk1,ss1,kk2,ss2,kk3,ss3,kk4,ss4);

fprintf(ffuzcp,"\
%*s %8.8g %8.2f %8.8g %8.2f %8.8g %8.2f \
%8.8g %8.2f\n",iceb_tu_kolbait(62,bros),bros,
kk1,itgr_svcr[0],kk2,itgr_svcr[1],kk3,itgr_svcr[2],kk4,itgr_svcr[3]);
}
/*****************/
/*Итого по складу узкая форма*/
/*****************/
void  itskl_uz(int skl,double kk1,double kk2,double kk3,
double kk4,double ss1,double ss2,double ss3,double ss4,
short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,
int *kst,
double *itsk_svcr,
FILE *ffuz,FILE *ffuzcp) //узкая распечатка
{
char		bros[512];
class iceb_tu_str naim_skl("");
SQL_str row;
SQLCURSOR cur;


sprintf(bros,"select naik from Sklad where kod='%d'",skl);
if(sql_readkey(&bd,bros,&row,&cur) == 1)
  naim_skl.new_plus(row[0]);
  
sprintf(bros,"%s %d",gettext("Итого по складу"),skl);


dvtmcf1_sh_uz(dn,mn,gn,dk,mk,gk,sl,kst,skl,naim_skl.ravno(),ffuz,ffuzcp);

fprintf(ffuz,"\
--------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ffuzcp,"\
--------------------------------------------------------------------------------------------------------------------------------------------------\n");

dvtmcf1_sh_uz(dn,mn,gn,dk,mk,gk,sl,kst,skl,naim_skl.ravno(),ffuz,ffuzcp);

fprintf(ffuz,"\
%*s %-13s %-3s %3s %10s %8.8g %8.2f %8.8g %8.2f %8.8g %8.2f \
%8.8g %8.2f\n",iceb_tu_kolbait(29,bros),bros," "," "," "," ",
kk1,ss1,kk2,ss2,kk3,ss3,kk4,ss4);

fprintf(ffuzcp,"\
%*s %-13s %-3s %3s %10s %8.8g %8.2f %8.8g %8.2f %8.8g %8.2f \
%8.8g %8.2f\n",iceb_tu_kolbait(29,bros),bros," "," "," "," ",
kk1,itsk_svcr[0],kk2,itsk_svcr[1],kk3,itsk_svcr[2],kk4,itsk_svcr[3]);



}
/****************/
/*Итого в килограммах по счёту и по складу узкая форма*/
/****************/
void itkg_uz(const char *sh,int skl,double k1,double k2,double k3,
double k4,
short met, //0-по счету 1-по складу*/
short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,
int *kst,
const char *naim_skl,
FILE *ffuz,FILE *ffuzcp)  //узкая распечатка
{
char		bros[512];
if(met == 1)
  sprintf(bros,"%s %d %s",gettext("Итого по складу"),
  skl,
  gettext("Кг."));

if(met == 0)
  sprintf(bros,"%s %s %s",
  gettext("Итого по счёту"),
  sh,
  gettext("Кг."));


dvtmcf1_sh_uz(dn,mn,gn,dk,mk,gk,sl,kst,skl,naim_skl,ffuz,ffuzcp);
fprintf(ffuz,"\
---------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ffuzcp,"\
---------------------------------------------------------------------------------------------------------------------------------------------------\n");

dvtmcf1_sh_uz(dn,mn,gn,dk,mk,gk,sl,kst,skl,naim_skl,ffuz,ffuzcp);

fprintf(ffuz,"\
%*s %-13s %-3s %3s %10s %8.8g %8s %8.8g %8s %8.8g %8s \
%8.8g %8s\n",iceb_tu_kolbait(29,bros),bros," "," "," "," ",
k1," ",k2," ",k3," ",k4," ");
fprintf(ffuzcp,"\
%*s %-13s %-3s %3s %10s %8.8g %8s %8.8g %8s %8.8g %8s \
%8.8g %8s\n",iceb_tu_kolbait(29,bros),bros," "," "," "," ",
k1," ",k2," ",k3," ",k4," ");

}
/****************/
/*Итого по счёту для узкой форми*/
/****************/
void itsh_uz(const char *sh,double k1,double k2,double k3,double k4,
double s1,double s2,double s3,double s4,
short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,
int *kst,
int skl,
const char *naim_skl,
double *itsh_svcr,
FILE *ffuz,FILE *ffuzcp) //узкая распечтака
{
char		bros[512];

sprintf(bros,"%s %s",gettext("Итого по счёту"),sh);

dvtmcf1_sh_uz(dn,mn,gn,dk,mk,gk,sl,kst,skl,naim_skl,ffuz,ffuzcp);

fprintf(ffuz,"\
--------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ffuzcp,"\
--------------------------------------------------------------------------------------------------------------------------------------------------\n");

dvtmcf1_sh_uz(dn,mn,gn,dk,mk,gk,sl,kst,skl,naim_skl,ffuz,ffuzcp);

fprintf(ffuz,"\
%*s %-13s %-3s %3s %10s %8.8g %8.2f %8.8g %8.2f %8.8g %8.2f \
%8.8g %8.2f\n",iceb_tu_kolbait(29,bros),bros," "," "," "," ",
k1,s1,k2,s2,k3,s3,k4,s4);

fprintf(ffuzcp,"\
%*s %-13s %-3s %3s %10s %8.8g %8.2f %8.8g %8.2f %8.8g %8.2f \
%8.8g %8.2f\n",iceb_tu_kolbait(29,bros),bros," "," "," "," ",
k1,itsh_svcr[0],k2,itsh_svcr[1],k3,itsh_svcr[2],k4,itsh_svcr[3]);


}


