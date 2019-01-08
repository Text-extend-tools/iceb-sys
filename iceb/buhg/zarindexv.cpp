/*$Id: zarindexv.c,v 5.8 2013/05/17 14:56:17 sasa Exp $*/
/*16.04.2017	30.01.2010	Белых А.И.	zarindexv.c
Расчёт индексации на не вовремя выплаченную зарплату
*/
#include        <errno.h>
#include        "buhg.h"
#include "read_nast_ind.h"

double zarindexv_kof(short mes,short god);


extern class index_ua tabl_ind; /*Таблица для индексации зарплаты*/
extern double   okrg; /*Округление*/
extern struct ZAR	zar;

void zarindexv(int tn, //Табельный номер*/
short mp,short gp,int podr,const char *nah_only)
{
char strsql[1024];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
class iceb_tu_str shet("");

sprintf(strsql,"\n%s-Расчёт индексации на не вовремя выплаченную зарплату\n\
------------------------------------------------------------------\n",__FUNCTION__); 
zar_pr_ins(strsql);

if(tabl_ind.kninvz <= 0)
 {
  sprintf(strsql,"В файле настройки zarindexv.alx не введён код индексации!\n");
  zar_pr_ins(strsql);
  return;
 }

if(tabl_ind.kodpzns <= 0)
 {
  sprintf(strsql,"В файле настройки zarnast.alx не введён код перечисления на карт-счёт!\n");
  zar_pr_ins(strsql);
 }

if(tabl_ind.kodpzvk <= 0)
 {
  sprintf(strsql,"В файле настройки zarnast.alx не введён код получения зарплаты в кассе!\n");
  zar_pr_ins(strsql);
 }
if(tabl_ind.kodpzvk <= 0 && tabl_ind.kodpzns <= 0)
 return;

if(proverka(nah_only,tabl_ind.kninvz,0,0) != 0)
 {
  sprintf(strsql,"Код индексации %d на невовремя выплаченную зарплату исключён из расчёта\n",tabl_ind.kninvz);
  zar_pr_ins(strsql);
  return;
 }

/*Проверяем есть ли индексация в списке начислений*/
sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='1' and knah=%d",tn,tabl_ind.kninvz);
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"Не введён код индексации %d в список начислений!\n",tabl_ind.kninvz);
  zar_pr_ins(strsql);
  return;
 }
shet.new_plus(row[0]);

/*Определяем есть ли выплаты в счёт предыдущих периодов */
sprintf(strsql,"select knah,suma,godn,mesn from Zarp where datz >= '%04d-%02d-01' \
and datz <= '%04d-%02d-31' and tabn=%d and prn='2' and (knah=%d or knah=%d) and suma <> 0.",
gp,mp,gp,mp,tn,tabl_ind.kodpzns,tabl_ind.kodpzvk);

sprintf(strsql,"%s\n",strsql);
zar_pr_ins(strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
  return;

sprintf(strsql,"Таблица коэффициентов:%d\n",tabl_ind.koefv.kolih());
zar_pr_ins(strsql);
for(int nom=0; nom < tabl_ind.koefv.kolih(); nom++)
   {
    sprintf(strsql,"%02d.%d|%f\n",tabl_ind.miv.ravno(nom),tabl_ind.giv.ravno(nom),tabl_ind.koefv.ravno(nom));
    zar_pr_ins(strsql);
    
   }
 
short dpr=1,mpr=mp,gpr=gp; /*Дата зарплаты котороя выплачена в срок*/
sprintf(strsql,"Дата зарплаты которая выплачена в срок %d.%d\n",mpr,gpr);
zar_pr_ins(strsql);
 
dpm(&dpr,&mpr,&gpr,4); /*уменьшаем на месяц*/
short d,m,g;

class SPISOK mes_god;
class masiv_din_int mesv;
class masiv_din_int godv;
class masiv_din_double sumav;
int nomer_dat=0;

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  zar_pr_ins(strsql);
  m=atoi(row[3]);
  g=atoi(row[2]);  

  if(sravmydat(1,m,g,1,mpr,gpr) >= 0) 
   continue;

  sprintf(strsql,"подходит\n");
  zar_pr_ins(strsql);

  sprintf(strsql,"%02d.%04d",m,g);
  printw("*%02d.%04d\n",m,g);
  
  if((nomer_dat=mes_god.find(strsql)) < 0)
   {
    mes_god.plus(strsql);
    mesv.plus(m,-1);    
    godv.plus(g,-1);
   }
  sumav.plus(atof(row[1])*-1,nomer_dat);
 }

double koef=0.;
double ikoef=1;
double sum=0.;
class ZARP     zp;
d=1;
dpm(&d,&mp,&gp,5);
zp.dz=d;
zp.nomz=0;
zp.podr=podr;
short mi=0;
short gi=0;

for(int nomer=0 ; nomer < mes_god.kolih(); nomer++)
 {
  sprintf(strsql,"Дата в счёт которой выплачено %s сумма %.2f\n",mes_god.ravno(nomer),sumav.ravno(nomer));
  zar_pr_ins(strsql);
  d=1;
  mi=m=mesv.ravno(nomer);
  gi=g=godv.ravno(nomer);
  dpm(&d,&m,&g,3);  
  ikoef=1.;
  while(sravmydat(1,m,g,1,mpr,gpr) <= 0)  
   {
    koef=zarindexv_kof(m,g);
    ikoef*=koef;

    sprintf(strsql,"Дата %d.%d коэффициент за месяц %f Итоговый коэффициент %f\n",m,g,koef,ikoef);
    zar_pr_ins(strsql);

    dpm(&d,&m,&g,3);
   }

  if(ikoef == 0.)
   {
    sprintf(strsql,"Итоговый коэффициент равен нолю!\n");
    zar_pr_ins(strsql);
    continue;
   }
  sum=sumav.ravno(nomer)*(ikoef*100-100.)/100.;
  sum=okrug(sum,okrg);
  
  sprintf(strsql,"%.2f*(%f*100.-100.)/100.=%.2f\n",sumav.ravno(nomer),ikoef,sum);
  zar_pr_ins(strsql);
  if(sum <= 0.)
   {
    sprintf(strsql,"Cумма меньше или равна нолю!\n");
    zar_pr_ins(strsql);
    continue;      
   }  
  zp.mesn=mi; zp.godn=gi;

  strsql[0]='\0';
  strcpy(zp.shet,shet.ravno());
  zapzarp(zp.dz,mp,gp,tn,1,tabl_ind.kninvz,sum,shet.ravno(),mi,gi,0,0,strsql,podr,"",zp);
   
 }

}

/****************************/
/*Получени коэффициента за месяц*/
/********************************/

double zarindexv_kof(short mes,short god)
{

//if(ff_prot != NULL)
// sprintf(strsql,"Ищем коэффициент для %d.%d\n",mes,god);
 
for(int nom=0; nom < tabl_ind.koefv.kolih(); nom++)
 {
   
  if(mes == tabl_ind.miv.ravno(nom) && god == tabl_ind.giv.ravno(nom))
   {
//    if(ff_prot != NULL)
//     sprintf(strsql,"Нашли %d.%d %f\n",tabl_ind.miv.ravno(nom),tabl_ind.giv.ravno(nom),tabl_ind.koefv.ravno(nom));
    return(tabl_ind.koefv.ravno(nom)/100.);
   }
 }
return(0.);
}
