/* $Id: muodk.c,v 5.10 2013/09/26 09:43:36 sasa Exp $ */
/*16.05.2018    08.02.1994      Белых А.И.      muodk.c
Получение пустографок для отчёта кладовщиков
*/

#include        <errno.h>
#include        "buhg.h"

void            muodt_shet(short,short,short,short,short,short,short *,short *,int,const char*,class iceb_tu_str *str,FILE*,FILE*);
void            muodk_itskl(int,double,double,double,double,double,double,double,double,class iceb_tu_str *str,FILE*,FILE*);
void            muodt_itsh(const char*,double,double,double,double,double,double,double,double,class iceb_tu_str *str,FILE*,FILE*);
void		muodk_sap(short,short,short,int,FILE*,int,const char*,FILE*);

void itkg_uz(char sh[],int skl,double k1,double k2,double k3,double k4,short met,short dn,short mn,short gn,short dk,short mk,short gk,
int *sl,int *kst,char *naim_skl,FILE *ffuz,FILE *ffuzcp);


void dvtmcf1_sap_uz(short dn,short mn,short gn,short dk,short mk,short gk,int *nom_list_uz,int *kolstr_uz,int skl,char nai[],FILE *ffuz,FILE *ffuzcp);

extern int kol_strok_na_liste;

int muodk(class spis_oth *oth)
{
char            bros[512];
class iceb_tu_str str("");
char strsql[1024];
extern char	*maskmat; /*Массив кодов отмеченных материалов*/
struct  tm      *bf;
time_t          tmm;
time_t          tmmn;
short           kst,sli;
char imaf[64];
char imafkl2[64];
short           dn,mn,gn;
short           dk,mk,gk;
char            nomn[128];
int             kom,kom1;
double          k1,s1,k2,s2,k3,s3,k4,s4;
double          kk1,ss1,kk2,ss2,kk3,ss3,kk4,ss4;
static class iceb_tu_str dat1("");
FILE            *ff=NULL;
FILE		*ffkl2=NULL;
class iceb_tu_str shb(""),shbm("");
int		skl,skl1;
class iceb_tu_str naim("");
static class iceb_tu_str sklz("");
static class iceb_tu_str shbz("");
static class iceb_tu_str kodmz("");
static class iceb_tu_str kgrmz("");
static class iceb_tu_str ndsc("");
short		kgrm,kgrmzp;
int		nk;
class iceb_tu_str ei("");
int		i1;
double		cena=0.,nds=0.;
char		mnds=0;
double		ost1,ost2;
double		itg1,itg2,itg3,itg4,itg5,itg6,itg7,itg8;
int		kgr,kodm;
long		kolstr,kolstr1,kolstr2;
SQL_str         row,rowtmp,row1;
class iceb_tu_str naiskl("");
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

VVOD VV(0);
VVOD MENU(3);

  VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка заготовки для отчёта"));
  VV.VVOD_SPISOK_add_ZAG(gettext("Введите реквизиты для получения распечатки"));

  VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
  VV.VVOD_SPISOK_add_data(shbz.ravno(),112);
  VV.VVOD_SPISOK_add_data(sklz.ravno(),112);
  VV.VVOD_SPISOK_add_data(kgrmz.ravno(),112);
  VV.VVOD_SPISOK_add_data(112);
  VV.VVOD_SPISOK_add_data(3);

  VV.VVOD_SPISOK_add_MD(gettext("Дата остатка..........."));
  VV.VVOD_SPISOK_add_MD(gettext("Счёт..(,,)............."));
  VV.VVOD_SPISOK_add_MD(gettext("Склад..(,,)............"));
  VV.VVOD_SPISOK_add_MD(gettext("Группа материалла.(,,)."));
  VV.VVOD_SPISOK_add_MD(gettext("Код материалла (,,/*).."));
  sprintf(bros,gettext("НДС (0,%2.f,+)..........."),pnds);
  VV.VVOD_SPISOK_add_MD(bros);


naz:;
clear();

move(LINES-6,0);
printw(gettext("\
Если НДС не ввели - все карточки.\n\
Если НДС = 0      - карточки с нулевым НДС.\n\
Если НДС = %2.f     - карточки с %2.f%% НДС.\n\
Если НДС = +      - карточки с НДС в том числе.\n"),pnds,pnds);


  helstr(LINES-1,0,"F1",gettext("помощь"),
  "F2/+",gettext("расчитать"),
  "F3",gettext("реквизиты"),
  "F4",gettext("очистить"),
  "F10",gettext("выход"),NULL);


kom=VV.vvod(0,1,1,-1,-1);

  dat1.new_plus(VV.data_ravno(0));
  shbz.new_plus(VV.VVOD_SPISOK_return_data(1));
  sklz.new_plus(VV.VVOD_SPISOK_return_data(2));
  kgrmz.new_plus(VV.VVOD_SPISOK_return_data(3));
  kodmz.new_plus(VV.VVOD_SPISOK_return_data(4));
  ndsc.new_plus(VV.VVOD_SPISOK_return_data(5));

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
    iceb_t_pdoc("matu5_7.txt");
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
          VV.data_z_plus(2,kods.ravno());
         }
        goto naz;

      case 1 :
        vibrek("Grup",&kgrmz,&naim);
        VV.VVOD_SPISOK_zapis_data(3,kgrmz.ravno());
        goto naz;

      case 2 :

        if(dirmatr(&kgr,&kodm,&naims,1,0,"",0,0) == 0)
         {
          VV.data_z_plus(4,kodm);
         }

        clear();
        goto naz;

      case 3 :
        vibrek("Plansh",&shbz);
        VV.VVOD_SPISOK_zapis_data(1,shbz.ravno());
        goto naz;
     }

  case FK4:
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
    break;
 }

if(rsdat(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(0),1) != 0)
 { 
    iceb_t_soob(gettext("Не верно введена дата !"));
    goto naz;
 } 
dk=31; mk=mn; gk=gn;
dpm(&dk,&mk,&gk,5); 

MENU.VVOD_delete();
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите вид сортировки"));
MENU.VVOD_SPISOK_add_MD(gettext("Сортировать счет-склад-наименование материалла"));
MENU.VVOD_SPISOK_add_MD(gettext("Сортировать счет-склад-группа-наименование материалла"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
sort=0;
while(menu3w(stdscr,&MENU,&sort,-1,-1,0) != 0);
if(sort == -1 || sort == 2 )
  goto naz;

time(&tmmn);


clear();
refresh();

GDITE();


sprintf(strsql,"select * from Kart");
SQLCURSOR cur,curtmp;
SQLCURSOR cur1;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
if(kolstr == 0)
 {
  iceb_t_soob(gettext("Нет записей по заданным реквизитам поиска !"));
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
  sprintf(strsql,"select kodgr,naimat from Material where kodm=%s",row[2]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
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




time(&tmm);
bf=localtime(&tmm);

  sprintf(imafkl2,"zagg%d.lst",getpid());

  if((ffkl2 = fopen(imafkl2,"w")) == NULL)
   {
    error_op_nfil(imafkl2,errno,"");
    return(1);
   }



fprintf(ff,"%s\n\n\
				%s %d.%d.%d%s %s %d.%d.%d%s\n\
					       %s %d.%d.%d%s  %s:%02d:%02d\n",
iceb_t_get_pnk("00",0),
gettext("Оборотная ведомость движения товарно-материальных ценностей за период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

  char zagol[4][512];
  memset(zagol,'\0',sizeof(zagol));
  SQL_str row_alx;
  class SQLCURSOR cur_alx;

  sprintf(strsql,"select str from Alx where fil='matzag.alx' order by ns asc");
  if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return(1);
   }

  if(kolstr == 0)
   {
    sprintf(strsql,"Не найдены настройки matzag.alx");
    iceb_t_soob(strsql);
   }
  else
   {  
    i1=0;
    while(cur_alx.read_cursor(&row_alx) != 0)
     {
      strcpy(zagol[i1++],row_alx[0]);
      zagol[i1-1][strlen(zagol[i1-1])-1]='\0';
      if(i1 == 4)
       break;
     }             
   }

  fprintf(ffkl2,"%-133s %s\n%135s%s\n\
  				%s %d.%d.%d%s %s %d.%d.%d%s %10s%s\n\
  					       %s %d.%d.%d%s  %s - %d:%d%54s%s\n",
  iceb_t_get_pnk("00",0),zagol[0]," ",zagol[1],
  gettext("Оборотная ведомость движения товарно-материальных ценностей за период с"),
  dn,mn,gn,
  gettext("г."),
  gettext("до"),
  dk,mk,gk,
  gettext("г."),
  " ",
  zagol[2],
  gettext("По состоянию на"),
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
  gettext("г."),
  gettext("Время"),
  bf->tm_hour,bf->tm_min," ",zagol[3]);
 
kst=4;

if(shbz.ravno()[0] != '\0')
 {
  fprintf(ff,gettext("По счету учета %s\n"),shbz.ravno());
  fprintf(ffkl2,gettext("По счету учета %s\n"),shbz.ravno());
  kst++;
 }
if(sklz.ravno()[0] != '\0')
 {
  fprintf(ff,gettext("По складах %s\n"),sklz.ravno());
  fprintf(ffkl2,gettext("По складах %s\n"),sklz.ravno());
  kst++;

 }
if(kgrmz.ravno()[0] != '\0')
 {
  fprintf(ff,gettext("По группе материалла %s\n"),kgrmz.ravno());
  fprintf(ffkl2,gettext("По группе материалла %s\n"),kgrmz.ravno());
  kst++;
 }

if(kodmz.ravno()[0] != '\0')
 {
  fprintf(ff,gettext("По материалу %s\n"),kodmz.ravno());
  fprintf(ffkl2,gettext("По материалу %s\n"),kodmz.ravno());
  kst++;
 }
if(ndsc.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("НДС"),ndsc.ravno());
  fprintf(ffkl2,"%s:%s\n",gettext("НДС"),ndsc.ravno());
  kst++;
 }

skl=0;
sli=1;
itg1=itg2=itg3=itg4=itg5=itg6=itg7=itg8=0.;
k1=s1=k2=s2=k3=s3=k4=s4=0.;
kk1=ss1=kk2=ss2=kk3=ss3=kk4=ss4=0.;
kgrmzp=0;
str.new_plus("");
for(int nom=0; nom < 240; nom++)
 str.plus(".");
mvsh=kolstr1=0;
class ostatokt ost;
while(curtmp.read_cursor(&rowtmp) != 0)
 {
//  printw("%s %s %s %s %s\n",rowtmp[0],rowtmp[0],rowtmp[1] ,rowtmp[2] ,rowtmp[3]);
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

  
  if(ost.ostm[0] == 0. && ost.ostm[1] == 0.)
    continue;

  ei.new_plus(rowtmp[6]);
  cena=atof(rowtmp[7]);

  nds=atof(rowtmp[8]);
  strncpy(bros,rowtmp[9],sizeof(bros)-1);

  if(atoi(bros) == 0)
   mnds=' ';
  if(atoi(bros) == 1)
   mnds='+';
      
  if(kgrmzp != kgrm)
   {
    memset(itgr_svcr,'\0',sizeof(itgr_svcr));
    itg1=itg2=itg3=itg4=itg5=itg6=itg7=itg8=0.;
    kgrmzp=kgrm;
   }

  if(SRAV(shb.ravno(),shbm.ravno(),1) != 0)
   {
    if(shbm.ravno()[0] != '\0')
     {
	muodk_itskl(skl,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,&str,ff,ffkl2);
	skl=0;
	muodt_itsh(shbm.ravno(),k1,k2,k3,k4,s1,s2,s3,s4,&str,ff,ffkl2);
	kst+=4;

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
      if(ffkl2 != NULL)
         fprintf(ffkl2,"\f");
      kst=0;
      sli++;
     }


    fprintf(ff,"\n%s %s \"%s\"\n",gettext("Счёт"),shb.ravno(),naimshet.ravno());
    kst+=2; 
    muodt_shet(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl.ravno(),&str,ff,ffkl2);

    mvsh=1;

    shbm.new_plus(shb.ravno());
   }

  if(skl != skl1)
   {
    if(skl != 0)
     {
	muodk_itskl(skl,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,&str,ff,ffkl2);
        kst+=2;
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

    printw("%s: %d %s\n",gettext("Склад"),skl1,naiskl.ravno());
    refresh();

    
    if(kst != 0)
    if(kst > kol_strok_na_liste - 10)
     {
      fprintf(ff,"\f");
      if(ffkl2 != NULL)
         fprintf(ffkl2,"\f");
      kst=0;
      sli++;
     }
    mvsh=1;
    skl=skl1;

   }
  if(mvsh == 1)
   {
      muodk_sap(dn,mn,gn,sli,ff,skl,naiskl.ravno(),ffkl2);
      kst+=6;
    mvsh=0;
   }

  
  sprintf(nomn,"%s.%d.%d",shb.ravno(),kgrm,nk);


    ost1=ost.ostm[0];
    ost2=ost.ostmc[0];
    char ost3[64],ost4[64];
    memset(ost3,'\0',sizeof(ost3));
    memset(ost4,'\0',sizeof(ost4));
    if(ost.ostm[1] != 0.)
       sprintf(ost3,"%10.10g",ost.ostm[1]);
    if(ost.ostmc[1] != 0.)
       sprintf(ost4,"%12.2f",ost.ostmc[1]);

    /*Остаток по документам*/
     
    ostdok(dn,mn,gn,dk,mk,gk,skl1,nk,&ost);

    fprintf(ff,"\
%5d %-*.*s %-*s %-*s %c%2.2g %7.7g %7.7g %15.2f|               |               |               |               |               |               |               |            |\n%s\n",
    kodm,
    iceb_tu_kolbait(40,naim.ravno()),iceb_tu_kolbait(40,naim.ravno()),naim.ravno(),
    iceb_tu_kolbait(20,nomn),nomn,
    iceb_tu_kolbait(10,ei.ravno()),ei.ravno(),
    mnds,nds,ost1,ost.ostm[0],ost2,str.ravno());

    muodt_shet(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl.ravno(),&str,ff,ffkl2);

    fprintf(ffkl2,"\
%5d %-*.*s %-*s %-*s %c%2.2g %10.10g %10.10g %12.2f %10s %12s|       |       |       |       |       |       |       |       |       |       |         |       |\n%s\n",
    kodm,
    iceb_tu_kolbait(40,naim.ravno()),iceb_tu_kolbait(40,naim.ravno()),naim.ravno(),
    iceb_tu_kolbait(20,nomn),nomn,
    iceb_tu_kolbait(10,ei.ravno()),ei.ravno(),
    mnds,nds,cena,ost1,ost2,ost3,ost4,str.ravno());
     
  
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

muodk_itskl(skl,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,&str,ff,ffkl2);
muodt_itsh(shbm.ravno(),k1,k2,k3,k4,s1,s2,s3,s4,&str,ff,ffkl2);


podpis(ffkl2);

fclose(ffkl2);


podpis(ff);
fclose(ff);




printw_vr(tmmn);


OSTANOV();
 

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Заготовка для отчёта со стартовыми остатками"));
oth->spis_imaf.plus(imafkl2);
oth->spis_naim.plus(gettext("Заготовка для отчёта со стартовыми остатками и приходами"));


for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),1);
return(0);

}



/*******/
/*Счетчик для кладовщика*/
/*******/
void muodt_shet(short dn,short mn,short gn,short dk,short mk,
short gk,
short *sl,  //Счетчик листов
short *kst, //Счетчик строк
int skl,const char *nai,
class iceb_tu_str *str,
FILE *ff,FILE *ffkl2)
{

*kst+=2;

if(*kst >= kol_strok_na_liste-1)
 {
  fprintf(ff,"\f");
  fprintf(ffkl2,"\f");

  *sl+=1;
  muodk_sap(dn,mn,gn,*sl,ff,skl,nai,ffkl2);
  *kst=6;
  str->new_plus("");
  for(int nom=0; nom < 240; nom++)
   str->plus(".");

 }

}

/*****************/
/*Итого по складу для кладовщика*/
/*****************/
void muodk_itskl(int skl,double kk1,double kk2,double kk3,
double kk4,double ss1,double ss2,double ss3,double ss4,class iceb_tu_str *str,
FILE *ff,FILE *ffkl2)
{
char		bros[512];

sprintf(bros,"%s %d",gettext("Итого по складу"),skl);

str->new_plus("");
for(int nom=0; nom < 240; nom++)
 str->plus("-");

fprintf(ff,"%-*s %15.2f\n%s\n",iceb_tu_kolbait(91,bros),bros,ss1,str->ravno());

fprintf(ffkl2,"%-*s %10.10g %12.2f %10.10g %12.2f\n%s\n",iceb_tu_kolbait(86,bros),bros,kk1,ss1,kk2,ss2,str->ravno());

str->new_plus("");
for(int nom=0; nom < 240; nom++)
 str->plus(".");
}

/****************/
/*Итого по счёту для кладовщика*/
/****************/
void muodt_itsh(const char *sh,double k1,double k2,double k3,double k4,
double s1,double s2,double s3,double s4,class iceb_tu_str *str,FILE *ff,FILE *ffkl2)
{
char		bros[512];

sprintf(bros,"%s %s",gettext("Итого по счёту"),sh);

str->new_plus("");
for(int nom=0; nom < 240; nom++)
 str->plus("-");
fprintf(ff,"%-*s %15.2f\n%s\n",
iceb_tu_kolbait(91,bros),bros,
s1,str->ravno());

fprintf(ffkl2,"%-*s %10.10g %12.2f %10.10g %12.2f\n%s\n",
iceb_tu_kolbait(86,bros),bros,k1,s1,k2,s2,str->ravno());

str->new_plus("");
for(int nom=0; nom < 240; nom++)
 str->plus(".");
}


/*******/
/*Шапка для кладовщика*/
/*******/
void muodk_sap(short dn,short mn,short gn,int sli,FILE *ff,
int skl,const char *nai,FILE *ffkl2)
{
char		str[512];
char		bros[512];

memset(str,'\0',sizeof(str));

memset(str,'-',240);
sprintf(bros,"%s: %d %s",gettext("Склад"),skl,nai);

fprintf(ff,"%-*s %s%d\n%s\n",iceb_tu_kolbait(196,bros),bros,gettext("Лист N"),sli,str);

fprintf(ff,gettext("     |             Наименование               |  Номенклатурный    | Единица  |   | Остаток на  %02d.%02d.%4dг.      |               |               |               |               |               |               |               |            |\n"),
dn,mn,gn),
fprintf(ff,gettext(" Код |        товарно-материальных            |       номер        | измерения|НДС|-------------------------------|               |               |               |               |               |               |               |            |\n"));
fprintf(ff,gettext("     |             ценностей                  |                    |          |   |   Количество  |    Сумма      |               |               |               |               |               |               |               |            |\n"));
fprintf(ff,"%s\n",str);

memset(str,'\0',sizeof(str));
memset(str,'-',240);

fprintf(ffkl2,"%-*s %s%d\n%s\n",iceb_tu_kolbait(196,bros),bros,gettext("Лист N"),sli,str);

fprintf(ffkl2,gettext("\
     |             Наименование               |   Номенклатурный   | Единица  |   |          |Остаток на %02d.%02d.%4dг.|       Приход          |               |               |               |               |               |                 |\n"),
dn,mn,gn),
fprintf(ffkl2,gettext("\
 Код |        товарно-материальных            |       номер        | измерения|НДС|   Цена   |-----------------------|-----------------------|---------------|---------------|---------------|---------------|---------------|-----------------|\n"));
fprintf(ffkl2,gettext("\
     |             ценностей                  |                    |          |   |          |Количество|   Сумма    |Количество|   Сумма    |       |       |       |       |       |       |       |       |       |       |         |       |\n"));

fprintf(ffkl2,"%s\n",str);

}
