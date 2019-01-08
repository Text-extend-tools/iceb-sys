/* $Id: rasdok2.c,v 5.38 2013/08/13 05:49:52 sasa Exp $ */
/*15.07.2015	11.03.1999	Белых А.И.	rasdok2.c
Распечатка накладной в две колонки
*/
#include        <errno.h>
#include        "buhg.h"


void		vztr1(short,short,short,int,const char*,double*,FILE*,short);
void		dvekol(char[]);

extern double	okrcn;  /*Округление цены*/
extern double   okrg1; /*Округление*/
extern short	vtara; /*Код группы возвратная тара*/
extern short    srtnk; /*0-не включена сортировка 1-включена*/

void rasdok2(short dd,short md,short gd,int skl,const char *nomdok,short tipnn) //Тип накладной
{
struct  tm      *bf;
time_t          tmm;
short		mnt; /*Метка наличия тары*/
short           mvnpp=0; /*0-внешняя 1 -внутреняя 2-изменение стоимости*/
class iceb_tu_str nmo1("");
char		imaf[64];
FILE		*f1;
double		bb,bb1,bb2,bb3,bb4;
double		kol,krat,cena;
class iceb_tu_str str("");
class iceb_tu_str bros("");
class iceb_tu_str naim("");
double		itogo,itogo1;
double		mest;
double		sumkor;
double		sumt;
short		kgrm;
short		kolz;
SQL_str         row,rowtmp;
char		strsql[1024];
short		tipz;
class iceb_tu_str kpos("");
class iceb_tu_str kop("");
class iceb_tu_str naimpr("");
short		vtar;
SQLCURSOR cur,curtmp;

/*
printw("Печать двойного документа.\n");
OSTANOV();
*/
/*Читаем шапку накладной*/

sprintf(strsql,"select tip,kontr,kodop,pn from Dokummat \
where datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",
gd,md,dd,skl,nomdok);

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  VVOD SOOB(1);

  sprintf(strsql,"%s-%s",__FUNCTION__,gettext("Не найден документ !")),
  SOOB.VVOD_SPISOK_add_MD(strsql);

  sprintf(strsql,"%s=%d.%d.%d %s=%s %s=%d",
  gettext("Дата"),
  dd,md,gd,
  gettext("Номер документа"),
  nomdok,
  gettext("Код склада"),
  skl);
  SOOB.VVOD_SPISOK_add_MD(strsql);

  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return;
 }

tipz=atoi(row[0]);
kpos.new_plus(row[1]);
kop.new_plus(row[2]);
float pnds=atof(row[3]);
/*Читаем наименование операции*/
if(tipz == 1)
 bros.new_plus("Prihod");
if(tipz == 2)
 bros.new_plus("Rashod");

sprintf(strsql,"select naik,vido from %s where kod='%s'",bros.ravno(),kop.ravno());
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  beep();
  sprintf(strsql,"%s %s!",gettext("Не найден код операции"),kop.ravno());
  iceb_t_soob(strsql);
 }
else
 {
  mvnpp=atoi(row[1]);
  naimpr.new_plus(row[0]);
 }



/*Читаем реквизиты организации чужие*/
/*При внутреннем перемещении не читать*/


if(kpos.ravno_atoi() != 0 || kpos.ravno()[0] != '0')
 {

  sprintf(strsql,"select * from Kontragent where kodkon='%s'",kpos.ravno());
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kpos.ravno());
    iceb_t_soob(strsql);
    return;
   }
  else
   {
    nmo1.new_plus(row[1]);
   }
 }

class iceb_t_tmptab tabtmp;
const char *imatmptab={__FUNCTION__};

if(sortdokmuw(dd,md,gd,skl,nomdok,imatmptab,&tabtmp,NULL) != 0)
 return;


sprintf(strsql,"select * from %s",imatmptab);

if(srtnk == 1)
  sprintf(strsql,"select * from %s order by kgrm asc,naim asc",imatmptab);

int kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора!"),strsql);
  return;
 }

//sortnak(dd,md,gd,skl,nomdok);

sprintf(imaf,"sn%d.lst",getpid());
if((f1 = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }



fprintf(f1,"%s\n\
                  %s %s\n\
                     %s %d.%d.%d%s\n",
iceb_t_get_pnk("00",0),gettext("С ч е т  фактура"),nomdok,
gettext("от"),
dd,md,gd,
gettext("г."));


fprintf(f1,"%.73s\n",nmo1.ravno());


/*Читаем доверенность*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gd,nomdok,skl,1);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  polen(row[0],&bros,1,'#');
  fprintf(f1," %s",bros.ravno());
  fprintf(f1,"\n");
 }

/*Читаем льготы по ндс*/
int lnds=0;
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gd,nomdok,skl,11);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 lnds=atoi(row[0]);

if(lnds == 0)
 {
  fprintf(f1,"\
-------------------------------------------------------------------------\n");

  fprintf(f1,gettext("\
N |    Наименование продукции    |Коли- |Ц е н а|Ц е н а| С у м м а|Кол.|\n"));
  fprintf(f1,gettext("\
  |         (товару)             |чество| з НДС |без НДС| без НДС  |мест|\n"));

  fprintf(f1,"\
-------------------------------------------------------------------------\n");
 }
else
 {
  fprintf(f1,"\
-------------------------------------------------------------------------\n");

  fprintf(f1,gettext("\
N |        Наименование продукции        |Коли- |Ц е н а| С у м м а|Кол.|\n"));
  fprintf(f1,gettext("\
  |             (товару)                 |чество|без НДС| без НДС  |мест|\n"));

  fprintf(f1,"\
-------------------------------------------------------------------------\n");

 }
itogo=itogo1=0.;
kolz=0;
kol=0.;
mnt=0;
while(curtmp.read_cursor(&rowtmp) != 0)
 {

  kgrm=atoi(rowtmp[0]);
  naim.new_plus(rowtmp[1]);
  cena=atof(rowtmp[4]);
  cena=okrug(cena,okrcn);
  krat=atoi(rowtmp[5]);
  vtar=atoi(rowtmp[7]);
  kol=atof(rowtmp[9]);

  if(vtar == 1)
   {
    mnt++;
    continue;
   } 

  if(vtara != 0 && kgrm == vtara)
   {
    mnt++;
    continue;
   } 
  mest=0;
  if(krat > 0.001)
   {
    mest=kol/krat;
    mest=okrug(mest,0.1);
   }

/*  printw("%d kol=%f krat=%f %f\n",kolz,kol,krat,mest);*/

  bb=cena*kol;
  bb=okrug(bb,okrg1);
  bb1=cena+(cena*pnds/100.);
  bb1=okrug(bb1,okrg1);

  itogo+=bb;

  kolz++;
  if(lnds == 0)
   fprintf(f1,"%2d %-*.*s %6.6g %7.2f %7s",
   kolz,
   iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
   kol,bb1,prcn(cena));
  else
   fprintf(f1,"%2d %-*.*s %6.6g %7s",
   kolz,
   iceb_tu_kolbait(38,naim.ravno()),iceb_tu_kolbait(38,naim.ravno()),naim.ravno(),
   kol,prcn(cena));

  fprintf(f1," %10s",prcn(bb));
  if(mest != 0)
     fprintf(f1," %4.4g\n",mest);
  else
     fprintf(f1,"\n");


 }


fprintf(f1,"\
-------------------------------------------------------------------------\n");
fprintf(f1,"\
%*s: %10s\n",iceb_tu_kolbait(55,gettext("Итого")),gettext("Итого"),prcn(itogo));

if(mvnpp == 0) /*Внешнее перемещение*/
 {

  /*Читаем сумму корректировки*/
  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
  gd,nomdok,skl,13);
  sumkor=0.;

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   {
    sumkor=atof(row[0]);
    sumkor=okrug(sumkor,0.01);
   }


  bb=bb1=bb2=bb3=bb4=0.;

  if(sumkor != 0)
   {
    if(sumkor > 0.)
      bb=sumkor*100./itogo;
    if(sumkor < 0.)
      bb=100*sumkor/(itogo-sumkor*-1);
    bb=okrug(bb,0.01);

    if(sumkor < 0)
      sprintf(strsql,"%s %.6g%% :",gettext("Снижка"),bb*(-1));
    if(sumkor > 0)
      sprintf(strsql,"%s %.6g%% :",gettext("Надбавка"),bb);

    fprintf(f1,"%*s %10.2f/%.f\n",iceb_tu_kolbait(56,strsql),strsql,sumkor,sumkor+sumkor*pnds/100.);

    if(sumkor < 0)
      sprintf(strsql,gettext("Итого со снижкой :"));
    if(sumkor > 0)
      sprintf(strsql,gettext("Итого с надбавкой :"));

    fprintf(f1,"%*s %10.2f\n",iceb_tu_kolbait(56,strsql),strsql,sumkor+itogo);

   }

  bb=0.;

  if(tipnn == 0 )
   {
    sprintf(strsql,"%s %.2f%% :",gettext("НДС"),pnds);
    bb=(bb1+bb3+itogo+sumkor)*pnds/100.;
    bb=okrug(bb,0.01);
    fprintf(f1,"%*s %10.2f\n",iceb_tu_kolbait(56,strsql),strsql,bb);
   }

  sumt=0.;
  if(mnt != 0)
     vztr1(dd,md,gd,skl,nomdok,&sumt,f1,vtara);
        
  itogo1=(itogo+sumkor)+bb1+bb3+bb+sumt;
  strcpy(strsql,gettext("Итого к оплате:"));
  if(tipz == 1)
    strcpy(strsql,gettext("Итого:"));
  fprintf(f1,"%*s %10.2f\n",iceb_tu_kolbait(56,strsql),strsql,itogo1);
 }
else
 { 
  sumt=0.;
  if(mnt != 0)
     vztr1(dd,md,gd,skl,nomdok,&sumt,f1,vtara);
  itogo1+=sumt;
 }

preobr(itogo1,&str,0);

fprintf(f1,"%s:%s\n",gettext("Сумма прописью"),str.ravno());
fprintf(f1,"\n\%s\n",gettext("\
Отпустил_____________ Принял_______________"));

time(&tmm);
bf=localtime(&tmm);


fprintf(f1,"%s: %s\n",gettext("Документ распечатал"),iceb_t_getfioop());
fprintf(f1,"\
%s %d.%d.%d%s %s - %d:%d\n",gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);
podpis(f1);
fclose(f1);

dvekol(imaf);

//class iceb_tu_str imaf_nast;
class iceb_tu_str printer("lpr -r");

//imafn("nastsys.alx",&imaf_nast);
iceb_t_poldan("Системный принтер",&printer,"nastsys.alx");


pehf(imaf,1,printer.ravno());

}

/*****************/
/*Распечатка тары*/
/*****************/

void vztr1(short dd,short md,short gd,int skl,const char *nomdok,
double *sumt,FILE *f1,short vt)
{
double		itogo,bb;
SQL_str         row,row1;
char		strsql[512];
long		kolstr;
double		kolih,cena;
int		kgrm,vtr;
class iceb_tu_str naim("");
int		i;

sprintf(strsql,"select * from Dokummat1 where datd='%d-%02d-%02d' and \
sklad=%d and nomd='%s' order by kodm asc",
gd,md,dd,skl,nomdok);
SQLCURSOR cur;
SQLCURSOR cur1;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  printw("%s\n",sql_error(&bd));
  OSTANOV();
 }

if(kolstr <= 0)
 { 
  return;
 } 

itogo=0.;
i=0;
while(cur.read_cursor(&row) != 0)
 {

  /*Читаем наименование материалла*/
  sprintf(strsql,"select naimat,kodgr from Material where kodm=%s",row[4]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
    {
     naim.new_plus("");
     beep();
     printw("%s-Не нашли наименование материалла по коду %s\n",__FUNCTION__,row[4]);
     OSTANOV();
    }
  else
    naim.new_plus(row1[0]);

  kgrm=atoi(row1[1]);
  vtr=atoi(row[8]);

  if(vtr == 0. && (vtara != 0 && kgrm != vtara))
    continue;
  if(vtr == 0 && vtara == 0 )
    continue;

  kolih=atof(row[5]);
  cena=atof(row[6]);
  cena=okrug(cena,okrcn);
  
  bb=cena*kolih;
  bb=okrug(bb,okrg1);
  i++;
  if(i == 1)
   fprintf(f1,"%s:\n",gettext("Оборотная тара"));

  fprintf(f1,"%2d %-*.*s %6.6g %7s %7s",
  i,
  iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
  kolih," ",prcn(cena));
  fprintf(f1," %10s\n",prcn(bb));

  itogo+=bb;

 }


*sumt=itogo;

}

/******************************/
/*Преобразование в две колонки*/
/******************************/
void		dvekol(char imaf[])
{
FILE		*ff,*f1;
char		imaf1[24];
char		strm[1024];

if((ff = fopen(imaf,"r")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

sprintf(imaf1,"sn%d.tmp",getpid());
if((f1 = fopen(imaf1,"w")) == NULL)
 {
  error_op_nfil(imaf1,errno,"");
  return;
 }



while(FGETSS(strm,sizeof(strm),ff) != 0)
 {
  fprintf(f1," %-*s %s\n",iceb_tu_kolbait(80,strm),strm,strm);
 }


fclose(f1);
fclose(ff);
iceb_t_cp(imaf1,imaf,0);
unlink(imaf1);
iceb_t_ustpeh(imaf,1);
}
