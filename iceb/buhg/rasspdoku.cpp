/* $Id: rasspdoku.c,v 5.46 2013/09/26 09:43:39 sasa Exp $ */
/*19.06.2015    21.06.2000      Белых А.И.      rasspdoku.c
Получение списка документов услуг
*/
#include        <errno.h>
#include        "buhg.h"

class rasspdoku_poi
 {
  public:
   class iceb_tu_str dat1;
   class iceb_tu_str dat2;
   class iceb_tu_str kopz;
   class iceb_tu_str kodoz;
   class iceb_tu_str sklz;
   class iceb_tu_str prra;
   class iceb_tu_str grupk;

   int srt; /*метка сортировки*/
   int vidd; /*метка вида документов*/
  rasspdoku_poi()
   {
    srt=0;
    vidd=0;
    clear();
   }

  void clear()
   {
    dat1.new_plus("");
    dat2.new_plus("");
    kopz.new_plus("");
    kodoz.new_plus("");
    sklz.new_plus("");
    prra.new_plus("");
    grupk.new_plus("");
   }
 };
 


void rasnaku(int,short,short,short,const char*,double*,double*,double*,double*,double*,short,double,int,
class masiv_din_int *spku,
class masiv_din_int *spmetka,
class masiv_din_double *spkol,
class masiv_din_double *spsuma_bez_nds,
FILE*);

void rasspdoku_rp(class rasspdoku_poi *poi,FILE *ff);

extern double   okrg1;
extern double	okrcn;

int rasspdoku(class spis_oth *oth)
{
static class rasspdoku_poi poi;
class iceb_tu_str bros("");
struct  tm      *bf;
FILE            *ff,*ff2;
time_t		tmm;
char		imaf[64],imaf2[64];
short           dn,mn,gn,dk,mk,gk;
short 	        d,m,g;
class iceb_tu_str naior("");
int           kom,kom1;
double          suma,sumav,sumabn;
double		sp,spbn;
double		bb,bb1;
double		itog,itogsk,itognds;
class iceb_tu_str kop(""); /*код операции*/
class iceb_tu_str kor(""); /*код контрагента*/
class iceb_tu_str nn(""),nnn("");  /*номер накладной*/
class iceb_tu_str telefon("");
int		tipzz;
unsigned int    ndpp;
int vt=0;
class iceb_tu_str dataopl("");
double		sumkor;
long		kolstr,kolstr1;
SQL_str         row,row1;
char		strsql[1024];
int		skl;
int		lnds;
VVOD MENU(3);
VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка списка документов"));
VV.VVOD_SPISOK_add_ZAG(gettext("Введите нужные реквизиты"));
VV.VVOD_SPISOK_add_ZAG(gettext("По не введенным реквизитам поиск не выполняется"));

VV.VVOD_SPISOK_add_data(poi.dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(poi.dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(poi.kopz.ravno(),128);
VV.VVOD_SPISOK_add_data(poi.kodoz.ravno(),128);
VV.VVOD_SPISOK_add_data(poi.sklz.ravno(),128);
VV.VVOD_SPISOK_add_data(poi.prra.ravno(),2);
VV.VVOD_SPISOK_add_data(poi.grupk.ravno(),128);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала..........."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца............"));
VV.VVOD_SPISOK_add_MD(gettext("Код операции.(,,)....."));
VV.VVOD_SPISOK_add_MD(gettext("Код контрагента.(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Код под-зделения (,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Приход/расход. (+/-).."));
VV.VVOD_SPISOK_add_MD(gettext("Код группы конт.(,,).."));

naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("поиск"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

poi.dat1.new_plus(VV.data_ravno(0));
poi.dat2.new_plus(VV.data_ravno(1));

poi.kopz.new_plus(VV.data_ravno(2));
poi.kodoz.new_plus(VV.data_ravno(3));
poi.sklz.new_plus(VV.data_ravno(4));
poi.prra.new_plus(VV.data_ravno(5));
poi.grupk.new_plus(VV.data_ravno(6));
class iceb_tu_str naim("");

switch (kom)
 {

  case FK10:
  case ESC:
   return(1);

  case FK1:
    GDITE();

    iceb_t_pdoc("usl4_2.txt");

    clear();
    goto naz;

  case FK2:
  case PLU:
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
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
        vibrek("Uslpodr",&poi.sklz);
        VV.VVOD_SPISOK_zapis_data(4,poi.sklz.ravno());
        goto naz;

      case 1 :
        vibrek("Kontragent",&poi.kodoz);
        VV.VVOD_SPISOK_zapis_data(3,poi.kodoz.ravno());
        goto naz;

      case 2 :
        if(diruslop1(1,&poi.kopz,&naim) == 0)
          VV.data_z_plus(2,poi.kopz.ravno());
        goto naz;

      case 3 :
        if(diruslop2(1,&poi.kopz,&naim) == 0)
          VV.data_z_plus(2,poi.kopz.ravno());
        goto naz;

      case 4 :
        vibrek("Gkont",&poi.grupk);
        VV.VVOD_SPISOK_zapis_data(6,poi.grupk.ravno());
        goto naz;

     }
    goto naz;

  case FK4:
    VV.VVOD_clear_data();    
    goto naz;

  default:
    goto naz;
 }

if(rsdatp(&dn,&mn,&gn,poi.dat1.ravno(),&dk,&mk,&gk,poi.dat2.ravno()) != 0)
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

poi.srt=0;
while(menu3w(stdscr,&MENU,&poi.srt,-1,-1,0) != 0);
if(poi.srt == -1 || poi.srt == 2)
  return(1);

MENU.VVOD_delete();

MENU.VVOD_SPISOK_add_MD(gettext("Только подтвержденные документы"));
MENU.VVOD_SPISOK_add_MD(gettext("Только не подтвержденные документы"));
MENU.VVOD_SPISOK_add_MD(gettext("Все документы"));
MENU.VVOD_SPISOK_add_MD(gettext("Только оплаченные документы"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

poi.vidd=0;
while(menu3w(stdscr,&MENU,&poi.vidd,-1,-1,0) != 0);
if(poi.vidd == -1 || poi.vidd == 4)
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
sprintf(strsql,"select * from Usldokum where datd >= '%04d-%02d-%02d' \
and datd <= '%04d-%02d-%02d'",gn,mn,dn,gk,mk,dk);
SQLCURSOR cur,cur1;
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

SQL_str rowtmp;
class SQLCURSOR curtmp;

class iceb_t_tmptab tabtmp;
const char *imatmptab={"rassdokuw"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
podr int not null,\
datd DATE NOT NULL,\
ndpp int not null,\
nomd char(24) not null,\
kontr char(24) not null,\
kodop char(24) not null,\
nomnn char(24) not null,\
nds smallint not null,\
datop char(16) NOT NULL,\
sumkor double(16,6) not null,\
pn char(24) not null,\
nomdokp char(32) not null,\
tipz smallint not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  
class masiv_din_int spku; /*список кодов услуг*/
class masiv_din_int spmetka; /*список меток*/
class masiv_din_double spkol;/*список количества*/
class masiv_din_double spsuma_bez_nds;  /*список cумм без НДС*/
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,kolstr1++);

  if(tipzz != 0)
   if(tipzz != atoi(row[0]))
       continue;

  if(poi.vidd == 0)
   if(atoi(row[12]) != 1 )
       continue;

  if(poi.vidd == 1)
   if(atoi(row[12]) != 0 )
       continue;

  if(poi.vidd == 3)
   if(atoi(row[27]) != 0 )
       continue;


  if(proverka(poi.kodoz.ravno(),row[3],0,0) != 0)
     continue;
  if(proverka(poi.kopz.ravno(),row[6],0,0) != 0)
     continue;
     
  if(proverka(poi.sklz.ravno(),row[2],0,0) != 0)
     continue;

  if(poi.grupk.ravno()[0] != '\0')
   {
    if(SRAV(row[3],"00-",1) == 0)
     sprintf(strsql,"select grup from Kontragent where kodkon='%s'","00");
    else
     sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[3]);
    if(readkey(strsql,&row1,&cur1) == 1)
     {
      if(proverka(poi.grupk.ravno(),row1[0],0,0) != 0)
       continue;
     }
   }

  /* Период*/

  if(polen(row[4],&bros,2,'-') != 0)
   if(polen(row[4],&bros,2,'+') != 0)
    bros.new_plus(row[4]);
  ndpp=(unsigned int)bros.ravno_atoi();
  
  rsdat(&d,&m,&g,row[1],2);

  sprintf(strsql,"insert into %s values (%s,'%04d-%02d-%02d',%d,'%s','%s','%s','%s',%s,'%s',%s,%s,'%s',%s)",
  imatmptab,
  row[2],g,m,d,ndpp,row[4],row[3],row[6],row[5],row[7],row[9],row[11],row[26],row[16],row[0]);

  sql_zapis(strsql,1,0);    

 }

if(poi.srt == 0)
  sprintf(strsql,"select * from %s order by datd,ndpp,nomd,kontr,kodop asc",imatmptab);
if(poi.srt == 1)
  sprintf(strsql,"select * from %s order by podr,ndpp,kontr,kodop asc",imatmptab);

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

iceb_t_zagolov(gettext("Распечатка списка документов"),dn,mn,gn,dk,mk,gk,ff);
iceb_t_zagolov(gettext("Распечатка списка документов"),dn,mn,gn,dk,mk,gk,ff2);

rasspdoku_rp(&poi,ff);
rasspdoku_rp(&poi,ff2);


fprintf(ff,"\
%s %d.%d.%d%s %s - %d:%d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("  Дата    |Дата опла.|Ном.док. |Нал. накл.|  Сумма  |Сум.кop.|  НДС   |Сумма с НДС|Контр.|  Наименование контрагента    |Код оп.|   Телефон   |\n"));

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------\n");
itog=itogsk=itognds=0.;
sp=spbn=0.;
kolstr1=0;
SQLCURSOR curr;
double nds=0.;
double itogo_nds=0.;
float pnds=0.;
int tipz=0;
short dop,mop,gop;

class iceb_tu_str nomdokp("");
while(curtmp.read_cursor(&rowtmp) != 0)
 {
  strzag(LINES-1,0,kolstrtmp,kolstr1++);

  skl=atoi(rowtmp[0]);
  rsdat(&d,&m,&g,rowtmp[1],2);
  nn.new_plus(rowtmp[3]);
  kor.new_plus(rowtmp[4]);
  kop.new_plus(rowtmp[5]);
  nnn.new_plus(rowtmp[6]);
  lnds=atoi(rowtmp[7]);
  dataopl.new_plus(rowtmp[8]);
  sumkor=atof(rowtmp[9]);
  pnds=atof(rowtmp[10]);
  nomdokp.new_plus(rowtmp[11]);
  tipz=atoi(rowtmp[12]);
    

  if(kor.ravno()[0] == '0' && kor.ravno()[1] == '0' && polen(kor.ravno(),&bros,1,'-') == 0)
   kor.new_plus(bros.ravno());
      
  sprintf(strsql,"select naikon,telef from Kontragent where kodkon='%s'",kor.ravno());
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    beep();
    printw("%s %s\n",gettext("Не найден контрагент"),kor.ravno());
    fprintf(ff,"%s - %s\n",gettext("Не найден контрагент"),kor.ravno());
    OSTANOV();
    naior.new_plus("");
    telefon.new_plus("");
   }
  else
   {
    naior.new_plus(row1[0]);
    telefon.new_plus(row1[1]);
   }



  /*Узнаем сумму записей по накладной*/
  
  fprintf(ff2,"\n%d.%d.%d %s(%s) %s %s\n",d,m,g,nn.ravno(),nomdokp.ravno(),kor.ravno(),naior.ravno());

  rasnaku(skl,d,m,g,nn.ravno(),&suma,&sumabn,&sumav,&sp,&spbn,vt,sumkor,lnds,&spku,&spmetka,&spkol,&spsuma_bez_nds,ff2);
  itogsk+=sumkor;
  bb=suma+sumkor+sumabn+(suma+sumkor)*pnds/100.;
  bb=okrug(bb,okrg1);
  bb1=suma+sumabn;
  itog+=bb1;
  itognds+=bb;
  nds=bb-sumkor-bb1;
  itogo_nds+=nds;

  printw("%02d.%02d.%d %-*s %-*.*s %-*s %8.2f %8.8g %10.2f\n",
  d,m,g,
  iceb_tu_kolbait(5,kor.ravno()),kor.ravno(),
  iceb_tu_kolbait(20,naior.ravno()),iceb_tu_kolbait(20,naior.ravno()),naior.ravno(),
  iceb_tu_kolbait(9,nn.ravno()),nn.ravno(),
  bb1,sumkor,bb);
  refresh();

  if(rsdat(&dop,&mop,&gop,dataopl.ravno(),2) == 0)
   {
    sprintf(strsql,"%02d.%02d.%d",dop,mop,gop);
    dataopl.new_plus(strsql);    
   }
  else
   dataopl.new_plus("");
        
  fprintf(ff,"%02d.%02d.%d %*.*s %-*s %-*s %9.2f %8.8g %8.2f %11.2f %-*s %-*.*s %-*s %s\n",
  d,m,g,
  iceb_tu_kolbait(10,dataopl.ravno()),iceb_tu_kolbait(10,dataopl.ravno()),dataopl.ravno(),
  iceb_tu_kolbait(9,nn.ravno()),nn.ravno(),
  iceb_tu_kolbait(10,nnn.ravno()),nnn.ravno(),
  bb1,sumkor,nds,bb,
  iceb_tu_kolbait(6,kor.ravno()),kor.ravno(),
  iceb_tu_kolbait(30,naior.ravno()),iceb_tu_kolbait(30,naior.ravno()),naior.ravno(),
  iceb_tu_kolbait(7,kop.ravno()),kop.ravno(),
  telefon.ravno());

  if(tipz == 1)  
   {
    sprintf(strsql,"select vido from Usloper1 where kod='%s'",kop.ravno());
    if(readkey(strsql,&row1,&cur1) == 1)
     if(atoi(row1[0]) == 0)    
      if(nomdokp.getdlinna() > 1)
        fprintf(ff,"%10s %10s %-*s\n","","",iceb_tu_kolbait(9,nomdokp.ravno()),nomdokp.ravno());
   }

 }

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------\n\
%*s: %10.2f %8.8g %8.2f %11.2f\n",
iceb_tu_kolbait(40,gettext("Итого")),gettext("Итого"),itog,itogsk,itogo_nds,itognds);

printw("%*s: %8.2f %8.8g %10.2f\n",
iceb_tu_kolbait(45,gettext("Итого")),gettext("Итого"),itog,itogsk,itognds);

fprintf(ff2,"%s:\n\
-----------------------------------------------------------------------------------------------\n",
gettext("Общий итог"));

fprintf(ff2,"%*s: %10.2f %10s %10.2f\n",
iceb_tu_kolbait(60,gettext("Итого")),gettext("Итого"),itog," ",sp+spbn);

bb=sp+spbn+sp*pnds/100.;
bb=okrug(bb,okrg1);
fprintf(ff2,"%*s %10.2f %10s %10.2f\n",
iceb_tu_kolbait(61,gettext("Итого с НДС:")),gettext("Итого с НДС:"),itognds," ",bb);

podpis(ff);
podpis(ff2);

fclose(ff);
fclose(ff2);


/*разпечатываем свод по услугам*/
class iceb_fopen fil;
char imaf_sv[64];
sprintf(imaf_sv,"svod%d.lst",getpid());
if(fil.start(imaf_sv,"w") != 0)
 return(1);

double itogi_sv[2];
memset(itogi_sv,'\0',sizeof(itogi_sv));

iceb_t_zagolov(gettext("Свод по услугам"),dn,mn,gn,dk,mk,gk,fil.ff);

fprintf(fil.ff,"\
-------------------------------------------------------------\n");
fprintf(fil.ff,gettext("\
 Код |  Наименование услуги         |Количество |Сум.без НДС|\n"));
fprintf(fil.ff,"\
-------------------------------------------------------------\n");
for(int nom=0; nom < spku.kolih(); nom++)
 {

  if(spmetka.ravno(nom) == 0)
    sprintf(strsql,"select naimat from Material where kodm=%d",spku.ravno(nom));
  else  
    sprintf(strsql,"select naius from Uslugi where kodus=%d",spku.ravno(nom));

  if(readkey(strsql,&row,&cur) == 1)
   naim.new_plus(row[0]);
  else
   naim.new_plus("");

   
  fprintf(fil.ff,"%5d|%-*.*s|%11.2f|%11.2f|\n",
  spku.ravno(nom),
  iceb_tu_kolbait(30,naim.ravno()),
  iceb_tu_kolbait(30,naim.ravno()),
  naim.ravno(),
  spkol.ravno(nom),
  spsuma_bez_nds.ravno(nom));

  itogi_sv[0]+=spkol.ravno(nom);
  itogi_sv[1]+=spsuma_bez_nds.ravno(nom);
  
 }
fprintf(fil.ff,"\
-------------------------------------------------------------\n");
fprintf(fil.ff,"%*s|%11.2f|%11.2f|\n",
iceb_tu_kolbait(36,gettext("Итого")),
gettext("Итого"),
itogi_sv[0],
itogi_sv[1]);

podpis(fil.ff);
fil.end();

printw_vr(tmm);

OSTANOV();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Список документoв"));

oth->spis_imaf.plus(imaf2);
oth->spis_naim.plus(gettext("Содержимое документов"));

oth->spis_imaf.plus(imaf_sv);
oth->spis_naim.plus(gettext("Свод по услугам"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);

return(0);
}

/*********************************************************************/

void rasnaku(int skl,short d,short m,short g,const char *nn,
double *suma,double *sumabn,double *sumav,double *sp,double *spbn,
short vt,double sumkor,int lnds,
class masiv_din_int *spku,
class masiv_din_int *spmetka,
class masiv_din_double *spkol,
class masiv_din_double *spsuma_bez_nds,
FILE *ff)
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
double		kolih,cena;
int		kodzap,tipz;
short		metka;

float pnds=iceb_t_pnds();

/*Узнаем сумму всех записей*/
*suma=*sumabn=*sumav=0.;
sumap=sumapbn=0.;

sprintf(strsql,"select tp,metka,kodzap,kolih,cena,ei,shetu,nz from Usldokum1 \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s' order by kodzap asc",
g,m,d,skl,nn);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
  return;

fprintf(ff,"-----------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext(" N |       Наименование           |Ед. |Количество|  Ц е н а | С у м м а|Количество|Сумма подт|\n"));
fprintf(ff,gettext("   |         услуг                |изм.|          |          |          |подтвержд.|верждена  |\n"));
fprintf(ff,"-----------------------------------------------------------------------------------------------\n");
SQLCURSOR curr;
ks=0;
itog[0]=itog[1]=0.;
int nom_vsp=0;
while(cur.read_cursor(&row) != 0)
 {
  tipz=atoi(row[0]);
  metka=atoi(row[1]);  
  kodzap=atoi(row[2]);
  kolih=atof(row[3]);
  cena=atof(row[4]);
  cena=okrug(cena,okrcn);
  ei.new_plus(row[5]);
  bb=cena*kolih;
  bb=okrug(bb,okrg1);

  if(metka == 0)
   {
    /*Узнаем наименование материалла*/
    sprintf(strsql,"select naimat from Material where kodm=%s",row[2]);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      beep();
      fprintf(ff,"%s %s !\n",gettext("Не найден код материалла"),row[2]);
      printw("%s %s !\n",gettext("Не найден код материалла"),row[2]);
      OSTANOV();
      continue;
     }
   }

  if(metka == 1)
   {
    /*Читаем наименование услуги*/
    sprintf(strsql,"select naius from Uslugi where kodus=%s",row[2]);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
      {
       beep();
       printw("%s %s !\n",gettext("Не найден код услуги"),row[2]);
       fprintf(ff,"%s %s !\n",gettext("Не найден код услуги"),row[2]);
       OSTANOV();
       continue;
      }
   }
  naim.new_plus(row1[0]);
  
  if(lnds > 0)
    *sumabn=*sumabn+bb;
  else
    *suma=*suma+bb;
  kol=readkolu(d,m,g,skl,nn,tipz,metka,kodzap,atoi(row[7]));
    
  bb=cena*kolih;
  bb=okrug(bb,okrg1);

  bb1=cena*kol;
  bb1=okrug(bb1,okrg1);

  ks++;
  fprintf(ff,"%3d %-*.*s %-*.*s %10.10g %10.10g %10.2f %10.10g %10.2f\n",
  ks,
  iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
  iceb_tu_kolbait(4,ei.ravno()),iceb_tu_kolbait(4,ei.ravno()),ei.ravno(),
  kolih,cena,bb,kol,bb1);

  if(lnds > 0)
    sumapbn+=bb1;
  else
    sumap+=bb1;

  if((nom_vsp=spku->find(atoi(row[2]))) < 0)
   {
    spku->plus(atoi(row[2]),nom_vsp);
    spmetka->plus(atoi(row[1]),nom_vsp);
   }
  spkol->plus(atof(row[3]),nom_vsp);
  spsuma_bez_nds->plus(atof(row[4])*atof(row[3]),nom_vsp);

 }


itog[0]=*suma+*sumabn;
itog[1]=sumap+sumapbn;
fprintf(ff,"%*s: %10.2f %10s %10.2f\n",
iceb_tu_kolbait(60,gettext("Итого")),
gettext("Итого"),itog[0]," ",itog[1]);

if(sumkor != 0)
 {
  bb=sumkor*100./itog[0];
  bb=okrug(bb,0.01);

  if(sumkor < 0)
      sprintf(strsql,"%s %.6g%% :",gettext("Снижка"),bb*(-1));
  if(sumkor > 0)
      sprintf(strsql,"%s %.6g%% :",gettext("Надбавка"),bb);

   fprintf(ff,"%*s %10.2f            %10.2f\n",iceb_tu_kolbait(61,strsql),strsql,sumkor,sumkor);

  if(sumkor < 0)
      sprintf(strsql,gettext("Итого со снижкой :"));
  if(sumkor > 0)
      sprintf(strsql,gettext("Итого с надбавкой :"));

   fprintf(ff,"%*s %10.2f            %10.2f\n",iceb_tu_kolbait(61,strsql),strsql,sumkor+itog[0],sumkor+itog[1]);

 }

bb=*suma+sumkor+*sumabn+( *suma + sumkor)*pnds/100.;
bb=okrug(bb,okrg1);
bb1=sumap+sumkor+sumapbn+(sumap+sumkor)*pnds/100.;
bb1=okrug(bb1,okrg1);

fprintf(ff,"%*s: %10.2f %10s %10.2f\n",
iceb_tu_kolbait(60,gettext("НДС")),gettext("НДС"),bb-sumkor-itog[0]," ",bb1-sumkor-itog[1]);

fprintf(ff,"%*s %10.2f %10s %10.2f\n",
iceb_tu_kolbait(61,gettext("Итого с НДС:")),gettext("Итого с НДС:"),bb," ",bb1);
*sp=*sp+sumap;
*spbn=*spbn+sumapbn;

}
/***************************************/
/*Реквизиты поиска*/
/*************************************/
void rasspdoku_rp(class rasspdoku_poi *poi,FILE *ff)
{

if(poi->prra.ravno_pr() == 1)
  fprintf(ff,gettext("ТОЛЬКО ПРИХОДЫ\n"));

if(poi->prra.ravno_pr() == 2)
  fprintf(ff,gettext("ТОЛЬКО РАСХОДЫ\n"));

if(poi->vidd == 0)
  fprintf(ff,gettext("ТОЛЬКО ПОДТВЕРЖДЕННЫЕ ДОКУМЕНТЫ\n"));

if(poi->vidd == 1)
  fprintf(ff,gettext("ТОЛЬКО НЕ ПОДТВЕРЖДЕННЫЕ ДОКУМЕНТЫ\n"));

if(poi->vidd == 2)
  fprintf(ff,gettext("ВСЕ ДОКУМЕНТЫ\n"));
if(poi->vidd == 3)
  fprintf(ff,gettext("ТОЛЬКО ОПЛАЧЕННЫЕ ДОКУМЕНТЫ\n"));

if(poi->kopz.ravno()[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Код операции"),poi->kopz.ravno());

if(poi->kodoz.ravno()[0] != '\0')
  fprintf(ff,gettext("Код контрагента - %s\n"),poi->kodoz.ravno());

if(poi->sklz.ravno()[0] != '\0')
  fprintf(ff,gettext("Склад - %s\n"),poi->sklz.ravno());

if(poi->grupk.ravno()[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Код группы контрагента"),poi->grupk.ravno());



}
