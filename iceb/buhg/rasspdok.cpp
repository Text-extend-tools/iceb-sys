/* $Id: rasspdok.c,v 5.52 2013/09/26 09:43:39 sasa Exp $ */
/*23.04.2018    29.05.1997      Белых А.И.      rasspdok.c
Получение списка документов в материальном учете
*/
#include        <errno.h>
#include        "buhg.h"
#define         PL      50  /*Количество строк на первом листе*/
#define         VL      55  /*Количество строк на втором листе*/

void rasnak(int,short,short,short,const char*,double*,double*,double*,double*,short,double*,int,FILE*,FILE*,double*,float);

extern double   okrg1;
extern double	okrcn;

int rasspdok(class spis_oth *oth)
{
class iceb_tu_str bros("");
int		srt; 
struct  tm      *bf;
FILE            *ff,*ff2,*ffnp;
time_t		tmm;
char		imaf[64],imaf2[64],imafnp[64];
static class iceb_tu_str dat1(""),dat2("");
short           dn,mn,gn,dk,mk,gk;
short 	        d,m,g;
static class iceb_tu_str kopz("");
static class iceb_tu_str kodoz("");
static class iceb_tu_str sklz("");
static class iceb_tu_str prra("");
static class iceb_tu_str grupk(""); 
class iceb_tu_str naior("");
int           kom,kom1;
short           i;
double          suma,sumabn;
double		sp,spbn;
double		bb,bb1;
double		itog,itogsk,itognds;
class iceb_tu_str kop(""); /*код операции*/
class iceb_tu_str kor(""),kor1(""); /*код контрагента*/
class iceb_tu_str nn(""),nnn("");  /*номер накладной*/
class iceb_tu_str telefon("");

int		tipzz;
unsigned int    ndpp;
int		vidd;
int vt=0;
class iceb_tu_str otsr("");
double		sumkor;
long		kolstr,kolstr1;
SQL_str         row,row1,rowtmp;
char		strsql[1024];
int		skl;
int		lnds;
double		sumsnds=0.;
double		isumsnds=0.;
short		shek;
double		sumndspr=0.;
double		ndssum=0.;

VVOD MENU(3);
VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка списка документов"));
VV.VVOD_SPISOK_add_ZAG(gettext("Введите нужные реквизиты"));
VV.VVOD_SPISOK_add_ZAG(gettext("По не введенным реквизитам поиск не выполняется"));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(kopz.ravno(),128);
VV.VVOD_SPISOK_add_data(kodoz.ravno(),128);
VV.VVOD_SPISOK_add_data(sklz.ravno(),128);
VV.VVOD_SPISOK_add_data(prra.ravno(),2);
VV.VVOD_SPISOK_add_data(grupk.ravno(),128);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала..(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца...(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Код операции..(,,)...."));
VV.VVOD_SPISOK_add_MD(gettext("Код контрагента..(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Склад (,,)............"));
VV.VVOD_SPISOK_add_MD(gettext("Приход/расход. (+/-).."));
VV.VVOD_SPISOK_add_MD(gettext("Код группы контр.(,,)."));

naz:;

clear();


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("поиск"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));

kopz.new_plus(VV.data_ravno(2));
kodoz.new_plus(VV.data_ravno(3));
sklz.new_plus(VV.data_ravno(4));
prra.new_plus(VV.data_ravno(5));
grupk.new_plus(VV.data_ravno(6));
class iceb_tu_str kods("");
class iceb_tu_str naims("");
switch (kom)
 {

  case FK10:
  case ESC:
   return(1);

  case FK1:
    GDITE();
    iceb_t_pdoc("matu5_8.txt");
    clear();
    goto naz;

  case FK2:
  case PLU:
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка складов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций приходов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций расходов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп"));
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
        if(dirsklad(1,&kods,&naims) == 0)
         VV.data_z_plus(4,kods.ravno());
        goto naz;

      case 1 :
        vibrek("Kontragent",&kodoz,&naims);
        VV.VVOD_SPISOK_zapis_data(3,kodoz.ravno());
        goto naz;

      case 2 :
//        vibrek("Prihod",&kopz,&naims);
       if(dirprihod(1,&kods,&naims) == 0)
         VV.data_z_plus(2,kods.ravno());
        goto naz;

      case 3 :
        if(dirrashod(1,&kopz,&naims) == 0)
          VV.data_z_plus(2,kopz.ravno());
        goto naz;

      case 4 :
        vibrek("Gkont",&grupk,&naims);
        VV.VVOD_SPISOK_zapis_data(6,grupk.ravno());
        goto naz;

     }
    goto naz;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
 }

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
  goto naz;

tipzz=0;
if(VV.VVOD_SPISOK_return_data(5)[0] == '+')
  tipzz=1;
if(VV.VVOD_SPISOK_return_data(5)[0] == '-')
  tipzz=2;
  

MENU.VVOD_delete();

MENU.VVOD_SPISOK_add_MD(gettext("Сортировка по датам"));
MENU.VVOD_SPISOK_add_MD(gettext("Сортировка по номерам документов"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

srt=0;
while(menu3w(stdscr,&MENU,&srt,-1,-1,0) != 0);
if(srt == -1 || srt == 2)
  return(1);

MENU.VVOD_delete();

MENU.VVOD_SPISOK_add_MD(gettext("Только подтвержденные документы"));
MENU.VVOD_SPISOK_add_MD(gettext("Только не подтвержденные документы"));
MENU.VVOD_SPISOK_add_MD(gettext("Все документы"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

vidd=0;
while(menu3w(stdscr,&MENU,&vidd,-1,-1,0) != 0);
if(vidd == -1 || vidd == 3)
  return(1);

clear(); /*Очистить экран*/
GDITE();
time(&tmm);
bf=localtime(&tmm);

iceb_t_poldan("Код группы возвратная тара",&vt,"matnast.alx");

move(0,0);
printw("%s %d.%d.%d%s %s %d.%d.%d%s\n",
gettext("Распечатка документов за период c"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."));
refresh();

/*Готовим файл для сортировки*/
sprintf(strsql,"select * from Dokummat where datd >= '%d-%02d-%02d' \
and datd <= '%d-%02d-%02d'",gn,mn,dn,gk,mk,dk);
class SQLCURSOR cur,curtmp;
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
const char *imatmptab={"rasspdok"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
skl int not null,\
datd DATE not null,\
ndpp int not null,\
nomdok char(32) not null,\
kontr char(32) not null,\
kodop char(32) not null,\
nomnn char(32) not null,\
pn float(5,2) not null,\
datnnal char(16) not null,\
nomdokp char(32) not null,\
tipz smallint not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  

char data_nal_nak[32];
short dnn,mnn,gnn;
SQLCURSOR cur1;

kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  kolstr1++;
  strzag(LINES-1,0,kolstr,kolstr1);
/*
  printw("%d.%d.%d %d %s %s\n",mk1.d,mk1.m,mk1.g,mk1.tp,mk1.kop,mk1.kor);
*/

  if(tipzz != 0)
   if(tipzz != atoi(row[0]))
       continue;

  if(vidd == 0)
   if(atoi(row[7]) != 1 )
       continue;

  if(vidd == 1)
   if(atoi(row[7]) != 0 )
       continue;

  if(proverka(kodoz.ravno(),row[3],0,0) != 0)
     continue;

  if(proverka(kopz.ravno(),row[6],0,0) != 0)
     continue;

  if(proverka(sklz.ravno(),row[2],0,0) != 0)
     continue;

  if(grupk.ravno()[0] != '\0')
   {
    if(SRAV(row[3],"00-",1) == 0) 
      sprintf(strsql,"select grup from Kontragent where kodkon='%s'","00");
    else
      sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[3]);
    if(readkey(strsql,&row1,&cur1) == 1)
     {
      if(proverka(grupk.ravno(),row1[0],0,0) != 0)
       continue;
    
     }
   }

  /* Период*/

  if(polen(row[4],&bros,2,'-') != 0)
   if(polen(row[4],&bros,2,'+') != 0)
    bros.new_plus(row[4]);
  ndpp=(unsigned int)bros.ravno_atoi();
  
  rsdat(&d,&m,&g,row[1],2);

  rsdat(&dnn,&mnn,&gnn,row[12],2);
  memset(data_nal_nak,'\0',sizeof(data_nal_nak));
  if(dnn > 0)
   sprintf(data_nal_nak,"%02d.%02d.%d",dnn,mnn,gnn);

  sprintf(strsql,"insert into %s values (%d,'%04d-%02d-%02d',%d,'%s','%s','%s','%s',%s,'%s','%s',%s)",
  imatmptab,
  atoi(row[2]),g,m,d,ndpp,row[4],row[3],row[6],row[5],row[13],data_nal_nak,row[11],row[0]);

  sql_zapis(strsql,1,0);    

 }

if(srt == 0)
  sprintf(strsql,"select * from %s order by datd,ndpp,kontr,kodop asc",imatmptab);
if(srt == 1)
  sprintf(strsql,"select * from %s order by ndpp,kontr,kodop asc",imatmptab);

int kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

sprintf(imaf,"sp%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

sprintf(imaf2,"spn%d.lst",getpid());
if((ff2 = fopen(imaf2,"w")) == NULL)
 {
  error_op_nfil(imaf2,errno,"");
  return(1);
 }

sprintf(imafnp,"spnp%d.lst",getpid());
if((ffnp = fopen(imafnp,"w")) == NULL)
 {
  error_op_nfil(imafnp,errno,"");
  return(1);
 }

fprintf(ff,"%s\n%s %d.%d.%d%s %s %d.%d.%d%s\n",
iceb_t_get_pnk("00",0),
gettext("Распечатка документов за период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."));

fprintf(ff2,"%s\n%s %d.%d.%d%s %s %d.%d.%d%s\n",
iceb_t_get_pnk("00",0),
gettext("Распечатка документов за период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."));

fprintf(ffnp,"%s\n%s %d.%d.%d%s %s %d.%d.%d%s\n",
iceb_t_get_pnk("00",0),
gettext("Распечатка документов за период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."));

fprintf(ffnp,"\n%s !\n",gettext("Только не подтвержденные записи в документах"));

if(tipzz == 1)
 {
  fprintf(ff,"%s\n",gettext("ТОЛЬКО ПРИХОДЫ"));
  fprintf(ff2,"%s\n",gettext("ТОЛЬКО ПРИХОДЫ"));
  fprintf(ffnp,"%s\n",gettext("ТОЛЬКО ПРИХОДЫ"));
 }
if(tipzz == 2)
 {
  fprintf(ff,"%s\n",gettext("ТОЛЬКО РАСХОДЫ"));
  fprintf(ff2,"%s\n",gettext("ТОЛЬКО РАСХОДЫ"));
  fprintf(ffnp,"%s\n",gettext("ТОЛЬКО РАСХОДЫ"));
 }
if(vidd == 0)
 {
  fprintf(ff,"%s\n",gettext("ТОЛЬКО ПОДТВЕРЖДЕННЫЕ ДОКУМЕНТЫ"));
  fprintf(ff2,"%s\n",gettext("ТОЛЬКО ПОДТВЕРЖДЕННЫЕ ДОКУМЕНТЫ"));
  fprintf(ffnp,"%s\n",gettext("ТОЛЬКО ПОДТВЕРЖДЕННЫЕ ДОКУМЕНТЫ"));
 }
if(vidd == 1)
 {
  fprintf(ff,"%s\n",gettext("ТОЛЬКО НЕ ПОДТВЕРЖДЕННЫЕ ДОКУМЕНТЫ"));
  fprintf(ff2,"%s\n",gettext("ТОЛЬКО НЕ ПОДТВЕРЖДЕННЫЕ ДОКУМЕНТЫ"));
  fprintf(ffnp,"%s\n",gettext("ТОЛЬКО НЕ ПОДТВЕРЖДЕННЫЕ ДОКУМЕНТЫ"));
 }
if(vidd == 3)
 {
  fprintf(ff,"%s\n",gettext("ВСЕ ДОКУМЕНТЫ"));
  fprintf(ff2,"%s\n",gettext("ВСЕ ДОКУМЕНТЫ"));
  fprintf(ffnp,"%s\n",gettext("ВСЕ ДОКУМЕНТЫ"));
 }

if(kopz.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Код операции"),kopz.ravno());
  fprintf(ff2,"%s:%s\n",gettext("Код операции"),kopz.ravno());
  fprintf(ffnp,"%s:%s\n",gettext("Код операции"),kopz.ravno());
 }
if(kodoz.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Код контрагента"),kodoz.ravno());
  fprintf(ff2,"%s:%s\n",gettext("Код контрагента"),kodoz.ravno());
  fprintf(ffnp,"%s:%s\n",gettext("Код контрагента"),kodoz.ravno());
 }

if(sklz.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Склад"),sklz.ravno());
  fprintf(ff2,"%s:%s\n",gettext("Склад"),sklz.ravno());
  fprintf(ffnp,"%s:%s\n",gettext("Склад"),sklz.ravno());
 }

if(grupk.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Код группы контрагента"),grupk.ravno());
  fprintf(ff2,"%s:%s\n",gettext("Код группы контрагента"),grupk.ravno());
  fprintf(ffnp,"%s:%s\n",gettext("Код группы контрагента"),grupk.ravno());
 }

fprintf(ff,"\
%s %d.%d.%d%s %s - %d:%d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("  Дата    |Дата отср.|Ном.док. |Нал. накл.|Дата н.нак|  Сумма  |Сум.кop.|Сумма с НДС|  По кассе |Контр.|  Наименование контрагента    |Код оп.|   Телефон   |\n"));

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
itog=itogsk=itognds=0.;
sp=spbn=0.;
kolstr1=0;
float pnds=0.;
class iceb_tu_str nomdokp("");
int tipz=0;
while(curtmp.read_cursor(&rowtmp) != 0)
 {
//  printw("%s %s %s\n",rowtmp[0],rowtmp[1],rowtmp[2]);
//  refresh();
  strzag(LINES-1,0,kolstrtmp,kolstr1++);
  skl=atoi(rowtmp[0]);
  rsdat(&d,&m,&g,rowtmp[1],2);
  nn.new_plus(rowtmp[3]);
  kor.new_plus(rowtmp[4]);
  kop.new_plus(rowtmp[5]);
  nnn.new_plus(rowtmp[6]);
  pnds=(float)atof(rowtmp[7]);
  strncpy(data_nal_nak,rowtmp[8],sizeof(data_nal_nak)-1);
  nomdokp.new_plus(rowtmp[9]);
  tipz=atoi(rowtmp[10]);
  telefon.new_plus("");      
  kor1.new_plus("");
  if(kor.ravno()[0] == '0' && kor.ravno()[1] == '0' && polen(kor.ravno(),&kor1,2,'-') == 0)
   {
    sprintf(strsql,"select naik from Sklad where kod='%s'",kor1.ravno());
   }
  else
    sprintf(strsql,"select naikon,telef from Kontragent where kodkon='%s'",kor.ravno());

  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    beep();
    if(kor1.ravno()[0] == '\0')
     {
      printw("%s %s\n",gettext("Не найден контрагент"),kor.ravno());
      fprintf(ff,"%s - %s\n",gettext("Не найден контрагент"),kor.ravno());
     }
    else
     {
      printw(gettext("Не найден склад %d в списке складов !"),kor1.ravno_atoi());
      printw("\n");
      fprintf(ff,gettext("Не найден склад %d в списке складов !"),kor1.ravno_atoi());
      fprintf(ff,"\n");
     }
    naior.new_plus("");
    
    OSTANOV();
   }
  else
   {
    naior.new_plus(row1[0]);
    if(kor1.ravno()[0] == '\0')
      telefon.new_plus(row1[1]);
   }
 
  /*Узнаем дату отсрочки платежа*/

  sprintf(strsql,"select sodz from Dokummat2 \
  where god=%d and nomd='%s' and sklad=%d and nomerz=9",
  g,nn.ravno(),skl);

  if((i=sql_readkey(&bd,strsql,&row1,&cur1)) == 1)
    otsr.new_plus(row1[0]);
  else
    otsr.new_plus("");

  /*Узнаем НДС документа*/
  sprintf(strsql,"select sodz from Dokummat2 \
  where god=%d and nomd='%s' and sklad=%d and nomerz=11",
  g,nn.ravno(),skl);
  lnds=0;
  if((i=sql_readkey(&bd,strsql,&row1,&cur1)) == 1)
     lnds=atoi(row1[0]);

  /*Узнаем Был ли распечатан кассовый чек*/
  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=5",
  g,nn.ravno(),skl);
  shek=0;
  if((i=sql_readkey(&bd,strsql,&row1,&cur1)) == 1)
     shek=atoi(row1[0]);

  sumndspr=0.;

  //Читаем введенную сумму НДС для приходного документа
  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=6",
  g,nn.ravno(),skl);
  if((i=sql_readkey(&bd,strsql,&row1,&cur1)) == 1)
     sumndspr=atoi(row1[0]);

  /*Узнаем сумму записей по накладной*/
  
  fprintf(ff2,"\n%d.%d.%d %s(%s) %s %s\n",d,m,g,nn.ravno(),nomdokp.ravno(),kor.ravno(),naior.ravno());

  fprintf(ffnp,"\n%d.%d.%d %s(%s) %s %s\n",d,m,g,nn.ravno(),nomdokp.ravno(),kor.ravno(),naior.ravno());

  rasnak(skl,d,m,g,nn.ravno(),&suma,&sumabn,&sp,&spbn,vt,&sumkor,lnds,ff2,ffnp,&sumsnds,pnds);

  if(lnds == 0)
   {
    if(sumndspr != 0.)
     ndssum=sumndspr;
    else
     ndssum=(suma+sumkor)*pnds/100.;
    bb=suma+sumkor+sumabn+ndssum;
   }
  else
    bb=suma+sumkor+sumabn;

  bb=okrug(bb,okrg1);
  bb1=suma+sumabn;
  itog+=bb1;
  itognds+=bb;

  if(shek != 0)
   {
    isumsnds+=sumsnds;
   }
  else
   sumsnds=0.;  
  itogsk+=sumkor;

  printw("%02d.%02d.%d %-*s %-*.*s %-*s %9.2f %9.8g %10.2f %9.2f\n",
  d,m,g,
  iceb_tu_kolbait(7,kor.ravno()),kor.ravno(),
  iceb_tu_kolbait(15,naior.ravno()),iceb_tu_kolbait(15,naior.ravno()),naior.ravno(),
  iceb_tu_kolbait(9,nn.ravno()),nn.ravno(),
  bb1,sumkor,bb,sumsnds);
  
   
  fprintf(ff,"%02d.%02d.%d %*.*s %-*s %-*s %10s %9.2f %8.8g %11.2f %11.2f %-*s %-*.*s %-*s %s\n",
  d,m,g,
  iceb_tu_kolbait(10,otsr.ravno()),iceb_tu_kolbait(10,otsr.ravno()),otsr.ravno(),
  iceb_tu_kolbait(9,nn.ravno()),nn.ravno(),
  iceb_tu_kolbait(10,nnn.ravno()),nnn.ravno(),
  data_nal_nak,bb1,sumkor,bb,sumsnds,
  iceb_tu_kolbait(6,kor.ravno()),kor.ravno(),
  iceb_tu_kolbait(30,naior.ravno()),iceb_tu_kolbait(30,naior.ravno()),naior.ravno(),
  iceb_tu_kolbait(7,kop.ravno()),kop.ravno(),
  telefon.ravno());

  if(tipz == 1)  
   {
    sprintf(strsql,"select vido from Prihod where kod='%s'",kop.ravno());
    if(readkey(strsql,&row1,&cur1) == 1)
     if(atoi(row1[0]) == 0)    
      if(nomdokp.getdlinna() > 1)
        fprintf(ff,"%10s %10s %-*s\n","","",iceb_tu_kolbait(9,nomdokp.ravno()),nomdokp.ravno());
   }
 }

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"%*s: %10.2f %8.8g %11.2f %11.2f\n",
iceb_tu_kolbait(51,gettext("Итого")),gettext("Итого"),itog,itogsk,itognds,isumsnds);

printw("%*s: %9.2f %9.8g %10.2f %9.2f\n",
iceb_tu_kolbait(42,gettext("Итого")),gettext("Итого"),itog,itogsk,itognds,isumsnds);

podpis(ff);


fprintf(ff2,"%s:\n\
-----------------------------------------------------------------------------------------------\n",
gettext("Общий итог"));

fprintf(ff2,"%*s: %10.2f %10s %10.2f\n",
iceb_tu_kolbait(60,gettext("Итого")),gettext("Итого"),itog," ",sp+spbn);

bb=sp+spbn+sp*pnds/100.;
bb=okrug(bb,okrg1);
fprintf(ff2,"%*s: %10.2f %10s %10.2f\n",
iceb_tu_kolbait(60,gettext("Итого с НДС")),gettext("Итого с НДС"),itognds," ",bb);

podpis(ff2);
podpis(ffnp);


fclose(ff);
fclose(ff2);
fclose(ffnp);

printw_vr(tmm);

OSTANOV();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Список документов"));

oth->spis_imaf.plus(imaf2);
oth->spis_naim.plus(gettext("Содержимое документов"));
oth->spis_imaf.plus(imafnp);
oth->spis_naim.plus(gettext("Неподтверждённые записи в документах"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);
 
return(0);
}

/*********************************************************************/

void rasnak(int skl,short d,short m,short g,const char *nn,
double *suma,double *sumabn,double *sp,double *spbn,
short vt,double *sumkor,int lnds,FILE *ff,FILE *ffnp,double *sumsnds,float pnds)
{
long		kolstr;
SQL_str         row,row1;
char		strsql[1024];
double		bb,bb1;
short		ks;
double		kol;
double		itog[2];
double		sumap,sumapbn;
class iceb_tu_str naim("");
class iceb_tu_str ei("");
short		vtr;
double		kolih,cena;
int		nk;
short		kgrm;
short		mnds; /*1-НДС в том числе*/
char		nomn[64]; /*Номенклатурный номер*/
class iceb_tu_str shu("");
double		sumands=0.;
double		sumandspod=0.;
double		sumandspr=0.; //Сумма НДС для приходного документа введенная вручную
SQLCURSOR cur;
SQLCURSOR cur1;


/*Узнаем сумму скидки*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=13",
g,nn,skl);

*sumkor=0.;
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
  *sumkor=atof(row[0]);

//Узнаем сумму НДС
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=6",
g,nn,skl);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
  sumandspr=atof(row[0]);


/*Узнаем сумму всех записей*/
*sumsnds=*suma=*sumabn=0.;
sumap=sumapbn=0.;

sprintf(strsql,"select kodm,kolih,cena,voztar,ei,nomkar,mnds \
from Dokummat1 where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' \
order by kodm asc",g,m,d,skl,nn);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  return;
 }

fprintf(ff,"-----------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext(" N |       Наименование продукции |Ед. |Количество|  Ц е н а | С у м м а|Количество|Сума под-|\n"));
fprintf(ff,gettext("   |              (товару)        |изм.|          |          |          |подтвержд.|тверждена |\n"));
fprintf(ff,"-----------------------------------------------------------------------------------------------\n");

fprintf(ffnp,"-----------------------------------------------------------------------------------------------\n");
fprintf(ffnp,gettext(" N |       Наименование продукции |Ед. |Количество|  Ц е н а | С у м м а|Количество|Сумма под-|\n"));
fprintf(ffnp,gettext("   |              (товару)        |изм.|          |          |          |подтвержд.|тверждена |\n"));
fprintf(ffnp,"-----------------------------------------------------------------------------------------------\n");

ks=0;
memset(nomn,'\0',sizeof(nomn));
itog[0]=itog[1]=0.;
while(cur.read_cursor(&row) != 0)
 {
/*   printw("*%d %d %s\n",mk2.g,mk2.skl,mk2.nn);*/
  kolih=atof(row[1]);
  cena=atof(row[2]);
//Для вложенного налога
  if(lnds == 0)
    bb=cena+cena*pnds/100.;
  else
    bb=cena;

  bb=okrug(bb,okrg1);
 
  bb=bb*kolih;

  *sumsnds+=okrug(bb,okrg1);

  cena=okrug(cena,okrcn);
  vtr=atoi(row[3]);
  ei.new_plus(row[4]);
  nk=atoi(row[5]);
  mnds=atoi(row[6]);  
  bb=cena*kolih;
  bb=okrug(bb,okrg1);

  /*Узнаем код группы*/
  sprintf(strsql,"select naimat,kodgr from Material where kodm=%s",
  row[0]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    beep();
    printw("%s %s !\n",gettext("Не найден код материалла"),row[0]);
    OSTANOV();
    naim.new_plus("");
    kgrm=0;
   }
  else
   {
    naim.new_plus(row1[0]);
    kgrm=atoi(row1[1]);
   }  
  if(vtr == 1 || kgrm == vt || lnds > 0 || mnds == 1)
    *sumabn=*sumabn+bb;
  else
    *suma=*suma+bb;

  kol=readkolk(skl,nk,d,m,g,nn);  
    
  bb=cena*kolih;
  bb=okrug(bb,okrg1);

  bb1=cena*kol;
  bb1=okrug(bb1,okrg1);

  ks++;
  /*Узнаем счёт учета для номенклатурного номера*/
  memset(nomn,'\0',sizeof(nomn));
  if(nk != 0)
   {
    sprintf(strsql,"select shetu from Kart where sklad=%d and nomk=%d",
    skl,nk);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      beep();
      printw("Не найдена карточка %d на складе %d !\n",nk,skl);
      OSTANOV();
      shu.new_plus("");
     }
    else
     {
      shu.new_plus(row1[0]);
     }
    sprintf(nomn,"%d.%s.%s.%d",skl,shu.ravno(),row[0],nk);
   }

  fprintf(ff,"%3d %-*.*s %-*.*s %10.10g %10.10g %10.2f %10.10g %10.2f %s\n",
  ks,
  iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
  iceb_tu_kolbait(4,ei.ravno()),iceb_tu_kolbait(4,ei.ravno()),ei.ravno(),
  kolih,cena,bb,kol,bb1,nomn);

  if(kol == 0.)
   fprintf(ffnp,"%3d %-*.*s %-*.*s %10.10g %10.10g %10.2f %10.10g %10.2f %s\n",
   ks,
   iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
   iceb_tu_kolbait(4,ei.ravno()),iceb_tu_kolbait(4,ei.ravno()),ei.ravno(),
   kolih,cena,bb,kol,bb1,nomn);

  if(vtr == 1 || kgrm == vt || lnds > 0 || mnds == 1)
    sumapbn+=bb1;
  else
    sumap+=bb1;

 }

/*Читаем услуги*/

sprintf(strsql,"select * from Dokummat3 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s'",g,m,d,skl,nn);
/*printw("\nstrsql=%s\n",strsql);*/
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {

  kolih=atof(row[4]);
  cena=atof(row[5]);
  cena=okrug(cena,okrcn);

//Для вложенного налога
  bb=cena+cena*pnds/100.;
  bb=okrug(bb,okrg1);
  bb=bb*kolih;
  *sumsnds+=okrug(bb,okrg1);
  
  if(kolih > 0)
    bb=kolih*cena;
  else
    bb=cena;
  bb=okrug(bb,okrg1);
  *suma=*suma+okrug(bb,okrg1);

  ks++;
  fprintf(ff,"%3d %-*.*s %-*.*s %10.10g %10s",
  ks,
  iceb_tu_kolbait(30,row[7]),iceb_tu_kolbait(30,row[7]),row[7],
  iceb_tu_kolbait(4,row[3]),iceb_tu_kolbait(4,row[3]),row[3],
  kolih,prcn(cena));
  fprintf(ff," %10s\n",prcn(bb));

 }


itog[0]=*suma+*sumabn;
itog[1]=sumap+sumapbn;
fprintf(ff,"%*s: %10.2f %10s %10.2f\n",
iceb_tu_kolbait(60,gettext("Итого")),gettext("Итого"),itog[0]," ",itog[1]);

if(*sumkor != 0)
 {
  bb=*sumkor*100./itog[0];
  bb=okrug(bb,0.01);
  if(*sumkor < 0)
      sprintf(strsql,"%s %.6g%% :",gettext("Снижка"),bb*(-1));
  if(*sumkor > 0)
      sprintf(strsql,"%s %.6g%% :",gettext("Надбавка"),bb);

   fprintf(ff,"%*s %10.2f\n",iceb_tu_kolbait(61,strsql),strsql,*sumkor);

  if(*sumkor < 0)
      sprintf(strsql,gettext("Итого со снижкой :"));
  if(*sumkor > 0)
      sprintf(strsql,gettext("Итого с надбавкой :"));

   fprintf(ff,"%*s %10.2f\n",iceb_tu_kolbait(61,strsql),strsql,*sumkor+itog[0]);

 }
sumandspod=sumands=0.;
if(lnds == 0)
 {
  if(sumandspr != 0.)
   sumands=sumandspr;
  else 
   sumands=( *suma + *sumkor)*pnds/100.;
 }

bb=*suma+*sumkor+*sumabn+sumands;
bb=okrug(bb,okrg1);
if(lnds == 0)
  sumandspod=sumap*pnds/100.;

bb1=sumap+sumapbn+sumandspod;
bb1=okrug(bb1,okrg1);

fprintf(ff,"%*s: %10.2f %10s %10.2f\n",
iceb_tu_kolbait(60,gettext("НДС")),gettext("НДС"),sumands," ",sumandspod);

fprintf(ff,"%*s: %10.2f %10s %10.2f\n",
iceb_tu_kolbait(60,gettext("Итого с НДС")),gettext("Итого с НДС"),bb," ",bb1);
*sp=*sp+sumap;
*spbn=*spbn+sumapbn;

}
