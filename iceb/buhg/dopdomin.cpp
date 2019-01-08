/* $Id: dopdomin.c,v 5.16 2013/09/26 09:43:32 sasa Exp $ */
/*30.05.2016	22.08.2000	Белых А.И.	dopdomin.c
Расчёт доплаты до минимальной зарплаты
*/
#include        <errno.h>
#include        "buhg.h"

extern double   okrg;
extern short	koddopmin; /*Код доплаты до минимальной зарплаты*/
extern short    *kodnvmin; /*Коды не входящие в расчёт доплаты до минимальной зарплаты*/
extern short    *kodotrs;  /*Коды отдельного рачета доплаты до минимальной зарплаты*/

void dopdomin(int tabn,short mp,short gp,int podr,
double koefpv,const char *nah_only)
{
double		sum,suma;
double		doplata;
int		i1;
class iceb_tu_str shet("");
short		d;
char		strsql[512];
long		kolstr;
SQL_str         row;
int		knah;
float dnei=0.,hasov=0.;
class ZARP     zp;
SQLCURSOR cur;
short dnr=0,mnr=0,gnr=0;
short dkr=0,mkr=0,gkr=0;

redkal(mp,gp,&dnei,&hasov);
/*
printw("\ndopdomin-%ld %d %d %d %f koddopmin=%d\n",
tabn,mp,gp,podr,koefpv,koddopmin);
OSTANOV();
*/
class zar_read_tn1h nastr;
zar_read_tn1(1,mp,gp,&nastr);

sprintf(strsql,"\nРасчёт начисления до минимальной зарплаты:%.2f\n\
--------------------------------------------------------\n",nastr.minzar); 
zar_pr_ins(strsql);

if(koddopmin == 0)
 return;
if(proverka(nah_only,koddopmin,0,0) != 0)
 {
  sprintf(strsql,"Код начисления %d исключён из расчёта\n",koddopmin);
  zar_pr_ins(strsql);
  return;
 }

/*для совметсителей не применяется для первого и поледнего месяца работы тоже*/
sprintf(strsql,"select sovm,datn,datk from Kartb where tabn=%d",tabn);
if(readkey(strsql,&row,&cur) != 1)
  return;

if(atoi(row[0]) == 1)
 {
  sprintf(strsql,"%s-Для совместителей расчёт не выполняется\n",__FUNCTION__);
  zar_pr_ins(strsql);
  return;  
 }

rsdat(&dnr,&mnr,&gnr,row[1],2);
if(sravmydat(1,mp,gp,1,mnr,gnr) == 0)
 {
  sprintf(strsql,"%s-Для месяца начала работы расчёт не выполняется\n",__FUNCTION__);
  zar_pr_ins(strsql);
  return;  
 }

rsdat(&dkr,&mkr,&gkr,row[2],2);
if(sravmydat(1,mp,gp,1,mkr,gkr) == 0)
 {
  sprintf(strsql,"%s-Для месяца конца работы расчёт не выполняется\n",__FUNCTION__);
  zar_pr_ins(strsql);
  return;  
 }
/*Проверяем есть ли код доплаты в списке*/
sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='1' and \
knah=%d",tabn,koddopmin); 
if(readkey(strsql,&row,&cur) < 1)
  return;
shet.new_plus(row[0]);

sprintf(strsql,"select knah,suma from Zarp where datz>='%d-%d-01' and \
datz <= '%d-%d-31' and tabn=%d and prn='1' and knah != %d and \
godn=%d and mesn=%d and suma <> 0.",
gp,mp,gp,mp,tabn,koddopmin,gp,mp);

//printw("%s\n",strsql);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

//printw("kolstr=%d\n",kolstr);
//OSTANOV();
if(kolstr == 0)
  return;


/*Вычисляем начисленную сумму*/
dpm(&d,&mp,&gp,5);
suma=0.;
while(cur.read_cursor(&row) != 0)
 {
  knah=atoi(row[0]);
  sum=atof(row[1]);
  /*Проверяем на коды не входящие в расчёт*/
  if(kodnvmin != NULL)
   {
    for(i1=1; i1 <= kodnvmin[0]; i1++)
     if(knah == kodnvmin[i1])
      break;
    if(i1 <= kodnvmin[0])
     continue;
   }  

  /*Проверяем на коды отдельного расчёта доплаты до минимальной зарплаты*/
  if(kodotrs != NULL)
   {
    for(i1=1; i1 <= kodotrs[0]; i1++)
     if(knah == kodotrs[i1])
      break;
    if(i1 <= kodotrs[0])
      continue;
   }  
  suma+=sum;
 }

sprintf(strsql,"Сумма которая берется для расчёта:%.2f\n",suma);
zar_pr_ins(strsql);

if(koefpv > 1.)
 {
  sprintf(strsql,"Устанавливаем koefvp=1.\n");
  zar_pr_ins(strsql);
  koefpv=1.;
 } 

//Узнаем количество отработанных дней
float	ddd=0.;
sprintf(strsql,"select dnei from Ztab where god=%d and mes=%d and \
tabn=%d and kodt=1",gp,mp,tabn);
if(readkey(strsql,&row,&cur) >= 1)
  ddd=atof(row[0]);     

//doplata=nastr.minzar*koefpv-suma;
doplata=nastr.minzar/dnei*ddd-suma;
sprintf(strsql,"%.2f/%.2f*%.2f-%.2f=%.2f\n",nastr.minzar,dnei,ddd,suma,doplata);
zar_pr_ins(strsql);

if(doplata < 0.01)
  doplata=0.;

doplata=okrug(doplata,okrg);


zp.dz=d;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;
strcpy(zp.shet,shet.ravno());
zapzarp(d,mp,gp,tabn,1,koddopmin,doplata,shet.ravno(),mp,gp,0,0,"",podr,"",zp); 

}
