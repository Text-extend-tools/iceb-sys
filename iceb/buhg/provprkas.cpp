/* $Id: provprkas.c,v 5.12 2013/09/26 09:43:38 sasa Exp $ */
/*09.11.2008	06.10.2000	Белых А.И.	provprkas.c
Проверка выполнены ли проводки для кассового ордера
*/
#include        <math.h>
#include        "buhg.h"


void provprkas(const char *kassa,short tipz,const char *nomd,short dd,short md,short gd,const char *kodop)
{
char		strsql[512];
int		kolstr;
SQL_str		row;
SQLCURSOR curr;
double		sumd,sumpod;
double		sum;

VVOD SOOB(1);

SQLCURSOR cur;

/*Проверяем код операции*/

if(tipz == 1)
 sprintf(strsql,"select metkapr from Kasop1 where kod='%s'",kodop);
if(tipz == 2)
 sprintf(strsql,"select metkapr from Kasop2 where kod='%s'",kodop);

if(sql_readkey(&bd,strsql,&row,&curr) == 1)
  if(row[0][0] == '0')
    goto vper;
    
if(tipz == 1)
 sprintf(strsql,"select deb \
from Prov where kto='%s' and pod=%s and nomd='%s' and oper='%s' and \
datd='%d-%02d-%02d' and deb != 0. and tz=%d",
 ICEB_MP_KASA,kassa,nomd,kodop,gd,md,dd,tipz);

if(tipz == 2)
 sprintf(strsql,"select kre \
from Prov where kto='%s' and pod=%s and nomd='%s' and oper='%s' and \
datd='%d-%02d-%02d' and kre != 0. and tz=%d",
 ICEB_MP_KASA,kassa,nomd,kodop,gd,md,dd,tipz);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
  goto vper1;  

sumkasorue(kassa,tipz,nomd,gd,&sumd,&sumpod);
sum=0.;
while(cur.read_cursor(&row) != 0)
 {
  sum+=atof(row[0]);
 }


if(fabs(sum-sumpod) > 0.009)
 {

  SOOB.VVOD_SPISOK_add_MD(gettext("Не сделаны все проводки !"));  
  sprintf(strsql,"%-*s: %8.2f",
  iceb_tu_kolbait(30,gettext("Подтвержденная сумма")),
  gettext("Подтвержденная сумма"),sumpod);  
  SOOB.VVOD_SPISOK_add_MD(strsql);

  sprintf(strsql,"%-*s: %8.2f",
  iceb_tu_kolbait(30,gettext("Сумма выполненых проводок")),
  gettext("Сумма выполненых проводок"),
  sum);  

  SOOB.VVOD_SPISOK_add_MD(strsql);
  soobshw(&SOOB,stdscr,-1,-1,0,1);

vper1:;

  sprintf(strsql,"update Kasord set \
prov=0 where kassa=%s and tp=%d and nomd='%s' and god=%d",
  kassa,tipz,nomd,gd);
 }
else
 {

vper:;

  sprintf(strsql,"update Kasord set \
prov=1 where kassa=%s and tp=%d and nomd='%s' and god=%d",
  kassa,tipz,nomd,gd);
 }
if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_DBACCESS_DENIED_ERROR)
  {
   msql_error(&bd,__FUNCTION__,strsql);
  }
}
