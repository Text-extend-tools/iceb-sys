/* $Id: rasnaln.c,v 5.59 2014/07/31 07:09:17 sasa Exp $ */
/*07.12.2018    22.01.1994      Белых А.И.      rasnaln.c
Распечатка налоговых накладных в Материальном учёте
*/
#include        <errno.h>
#include	"buhg.h"

int rasnaln9(short dd,short md,short gd,int skl,const char *nomdok,const char *imaf,short tipnn);


extern double   okrg1; /*Округление*/
extern double	okrcn;  /*Округление цены*/
extern short srtnk;

int rasnaln(short dd,short md,short gd,int skl,const char *nomdok,
const char *imaf,short tipnn) //Тип документа
{
/**************
if(sravmydat(dd,md,gd,16,03,2017) < 0)
  return(rasnaln8(dd,md,gd,skl,nomdok,imaf,tipnn));
**************/
  return(rasnaln9(dd,md,gd,skl,nomdok,imaf,tipnn));
return(1);
}
/******************************************************************/
/*Новая налоговая накладная 9*/
/******************************************************************/
int rasnaln9(short dd,short md,short gd,int skl,const char *nomdok,
const char *imaf,short tipnn) //Тип документа
{
SQL_str         row,row1,rowtmp;
int kolstr=0;
char		strsql[1024];
FILE            *f1=NULL;
double          itogo;
double          bb;
double		sumt; /*Сумма по возвратной таре*/
int		vt=0;
double		sumkor;
class iceb_tu_str bros("");
class iceb_tu_str kpos("");
class iceb_tu_str frop("");
class iceb_tu_str uspr("");
class iceb_tu_str osnov("");
short		tipz=0;
class iceb_tu_str nomnn(""); /*Номер налоговой накладной*/
class iceb_tu_str naim("");
short		kgrm;
double		cena,kolih;
short		vtar;
class iceb_tu_str ei("");
short		metz;
short		dnn,mnn,gnn;
class iceb_tu_str kodop("");
class iceb_tu_str imaf_nast("matnast.alx");

sumt=0.;
vt=0;
iceb_t_poldan("Код группы возвратная тара",&vt,imaf_nast.ravno());

class iceb_tu_str kodkontr00("00");

SQLCURSOR cur,cur1,curtmp;

/*Читаем шапку документа*/
sprintf(strsql,"select * from Dokummat where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",gd,md,dd,skl,nomdok);

if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s-%s=%d %s=%s %s=%d %s!",
  __FUNCTION__,  
  gettext("Год"),
  gd,
  gettext("Документ"),
  nomdok,
  gettext("Склад"),
  skl,
  gettext("Не найден документ"));  
  
  iceb_t_soob(strsql);
  
  return(1);
 }
float pnds=atof(row[13]);

kpos.new_plus(row[3]);
tipz=atoi(row[0]);
nomnn.new_plus(row[5]);
kodop.new_plus(row[6]);
rsdat(&dnn,&mnn,&gnn,row[12],2);

kodkontr00.new_plus(row[15]);

if(tipz == 2)
 if(SRAV(kodkontr00.ravno(),"00",0) == 0)
  kodkontr00.new_plus(iceb_t_getk00(0,kodop.ravno()));

sprintf(strsql,"select nomerz,sodz from Dokummat2 where god=%d and nomd='%s' and sklad=%d",gd,nomdok,skl);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

sumkor=0.;

int lnds=0;
if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  metz=atoi(row[0]);
  if(metz == 3)
    osnov.new_plus(row[1]);
  if(metz == 7)
    uspr.new_plus(row[1]);
  if(metz == 8)
    frop.new_plus(row[1]);

  if(metz == 9)
   {
    continue;
   }

  if(metz == 11)
   {   
    lnds=atoi(row[1]);
    continue;
   }
   
  if(metz == 13)
   {
    sumkor=ATOFM(row[1]);
    sumkor=okrug(sumkor,0.01);
   }
 }


if(kodkontr00.getdlinna() <= 1)
 kodkontr00.new_plus("00");

class iceb_tu_str kod_vid_deqt("");
if(lnds == 4)
 kod_vid_deqt.new_plus("7");
 
//Печатаем шапку документа
if(iceb_t_nn9_start(imaf,dd,md,gd,dnn,mnn,gnn,nomnn.ravno(),kpos.ravno(),tipz,kodkontr00.ravno(),&f1) != 0)
 return(1);


iceb_t_insfil("nn9_hap.alx",f1);

/*Готовим отсортированный файл*/

class iceb_t_tmptab tabtmp;
const char *imatmptab={__FUNCTION__};

if(sortdokmuw(dd,md,gd,skl,nomdok,imatmptab,&tabtmp,NULL) != 0)
 return(1);


sprintf(strsql,"select * from %s",imatmptab);

if(srtnk == 1)
  sprintf(strsql,"select * from %s order by kgrm asc,naim asc",imatmptab);

int kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора!"),strsql);
  return(1);
 }
itogo=0.;
int metka_data_opl=0;
class iceb_tu_str kodtov("");
int nomer_str=0;
int metka_imp_tov=0;
class iceb_tu_str kdstv("");

while(curtmp.read_cursor(&rowtmp) != 0)
 {

  kgrm=atoi(rowtmp[0]);
  naim.new_plus(rowtmp[1]);
  cena=atof(rowtmp[4]);
  cena=okrug(cena,okrcn);
  ei.new_plus(rowtmp[6]);
  vtar=atoi(rowtmp[7]);
  kolih=atof(rowtmp[9]);
  kodtov.new_plus(rowtmp[12]);
  metka_imp_tov=atoi(rowtmp[13]);
  kdstv.new_plus(rowtmp[14]);
  
  bb=cena*kolih;
  bb=okrug(bb,okrg1);

  if(kgrm == vt || vtar == 1)
   {
    sumt+=bb;
    continue;
   }
  metka_data_opl++;


  /*Разпечатка строки*/
  iceb_t_nn9_str(++nomer_str,0,naim.ravno(),ei.ravno(),kolih,cena,bb,kodtov.ravno(),metka_imp_tov,"",kdstv.ravno(),f1);

  itogo+=bb;


 }
/*Распечатываем услуги*/
sprintf(strsql,"select * from Dokummat3 where \
datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",gd,md,dd,skl,nomdok);
/*printw("\nstrsql=%s\n",strsql);*/

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
class iceb_tu_str naiusl("");

if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  kolih=atof(row[4]);
  cena=atof(row[5]);
  cena=okrug(cena,okrcn);

  if(kolih > 0)
    bb=cena*kolih;
  else
    bb=cena;

  bb=okrug(bb,okrg1);

  naiusl.new_plus("");
  if(atoi(row[10]) != 0)
   {
    sprintf(strsql,"select naius from Uslugi where kodus=%s",row[10]);
    if(readkey(strsql,&row1,&cur1) == 1)
     naiusl.new_plus(row1[0]);
   }  
  if(naiusl.ravno()[0] == '\0')
   naiusl.new_plus(row[7]);
  else
   {
    naiusl.plus(" ",row[7]);
   }

  /*Разпечатка строки*/
  iceb_t_nn9_str(++nomer_str,0,naiusl.ravno(),ei.ravno(),kolih,cena,bb,"",0,"","",f1);

  itogo+=bb;
 }
fprintf(f1,"ICEB_LST_END\n");
//Печатаем концовку налоговой накладной
iceb_t_nn9_end(tipz, tipnn,"matnast.alx",kodop.ravno(),kpos.ravno(),f1);

double suma_dok=0;
double suma_nds=0;

/*НДС*/
if(lnds == 0) //20% НДС
 {
  suma_nds=(itogo+sumkor)*pnds/100.;
  suma_nds=okrug(suma_nds,okrg1);

 }

if(lnds == 4) //7% НДС
 {
  suma_nds=(itogo+sumkor)*pnds/100.;
  suma_nds=okrug(suma_nds,okrg1);

 }


/*Загальна сума з ПДВ*/
suma_dok=itogo+sumkor+suma_nds;
/*записываем в шапку суммы*/
iceb_t_nn9_start_s(imaf,lnds,suma_dok,suma_nds,sumt);

iceb_t_ustpeh(imaf,3,&kolstr);
iceb_t_rnl(imaf,kolstr,"",&iceb_t_nn9_hap);
return(0);
}
