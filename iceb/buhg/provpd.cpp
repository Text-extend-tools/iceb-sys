/* $Id: provpd.c,v 5.19 2013/09/26 09:43:37 sasa Exp $ */
/*24.05.2013	20.05.1998	Белых А.И.	provpd.c
Проверка выполнены ли все проводки по платежному поручению
Смотрим на какую сумму подтверждена платежка 
если меньше чем в документе то документ не подтвержден
если сумма проводок равна подтвержденной сумме то проводки
выполнены
*/
#include        <math.h>
#include        "buhg.h"
#include        "dok4.h"

extern class REC rec;

void provpd(const char *tabl)
{
double		sum,sump,sumpp;
class iceb_tu_str shet("");
short		pr,podt;
short		dp,mp,gp;
long		kolstr;
SQL_str         row;
char		strsql[512];
SQLCURSOR cur;  

pr=1;
sump=sumpzpd(tabl,&dp,&mp,&gp,&sumpp);

if(fabs(sump -(rec.sumd+rec.uslb)) > 0.0099)
 {
  podt=0;
 }
else
 podt=1;

if(SRAV(tabl,"Pltp",0) == 0)
  sprintf(strsql,"%s","avtprodok.alx");

if(SRAV(tabl,"Pltt",0) == 0)
  sprintf(strsql,"%s","avtprodokt.alx");

if(iceb_t_poldan(rec.nsh.ravno(),&shet,strsql) != 0)
 {
  VVOD SOOB(1);
  sprintf(strsql,gettext("Не найден бухгалтерский счёт для счёта %s"),rec.nsh.ravno());
  SOOB.VVOD_SPISOK_add_MD(strsql);
  SOOB.VVOD_SPISOK_add_MD(gettext("Проводки не проверялись !"));
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  goto vp;
 } 

/*Суммиреум выполненные проводки*/
if(SRAV(tabl,"Pltp",0) == 0)
 sprintf(strsql,"select sh,kre from Prov \
where kto='%s' and pod=%d and nomd='%s' and datd='%d-%02d-%02d'",
 ICEB_MP_PPOR,0,rec.nomdk.ravno(),rec.gd,rec.md,rec.dd);

if(SRAV(tabl,"Pltt",0) == 0)
 sprintf(strsql,"select sh,kre from Prov \
where kto='%s' and pod=%d and nomd='%s' and datd='%d-%02d-%02d'",
 ICEB_MP_PTRE,0,rec.nomdk.ravno(),rec.gd,rec.md,rec.dd);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  pr=1;
  goto vp;
 }

sum=0.;
pr=0;
while(cur.read_cursor(&row) != 0)
 {

  pr++;
  
  if(SRAV(shet.ravno(),row[0],0) == 0 )
    sum+=atof(row[1]);

 }


if(pr== 0)
 {
  pr=1;
  goto vp;
 }

pr=0;
rec.prov=0;
if(fabs(sum-sump) > 0.00999)
 {
  sprintf(strsql,gettext("Не выполнены все проводки ! %.2f != %.2f"),sum,sump);
  iceb_t_soob(strsql);
  rec.prov=1;
  pr=1;
 }

vp:;

sprintf(strsql,"update %s \
set \
prov=%d,\
podt=%d \
where datd='%d-%02d-%02d' and nomd='%s'",
tabl,pr,podt,rec.gd,rec.md,rec.dd,rec.nomdk.ravno());

if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_DBACCESS_DENIED_ERROR)
  { 
   msql_error(&bd,__FUNCTION__,strsql);
  }
}
