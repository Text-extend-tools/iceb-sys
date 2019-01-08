/*$Id: rpshet_ss.c,v 5.10 2013/09/26 09:43:40 sasa Exp $*/
/*27.11.2009	10.02.2006	Белых А.И.	rpshet_ss.c
Расчёт по счёту со свёрнутым сальдо
*/
#include "buhg.h"

extern short	startgodb; /*Стартовый год*/

void rpshet_ss(const char *shet,
short dn,short mn,short gn,
short dk,short mk,short gk,
double *deb_kre) //Массив [6]
{
for(int i=0; i < 6; i++)
  deb_kre[i]=0;

short godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;

/*Узнаем начальное сальдо по счёту*/
char strsql[512];
sprintf(strsql,"select ns,deb,kre from Saldo where kkk='0' and gs=%d and \
ns='%s'",godn,shet);

SQL_str row;
int kolstr=0;
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  deb_kre[0]+=atof(row[1]);
  deb_kre[1]+=atof(row[2]);
 }

short d=1;
short m=1;
short g=godn;
double  debetsh=0.,kreditsh=0.;

while(sravmydat(d,m,g,dk,mk,gk) <= 0)
 {
  debetsh=kreditsh=0.;
  rozden(shet,d,m,g,&debetsh,&kreditsh,1);
  if(sravmydat(d,m,g,dn,mn,gn) < 0)
   {
    deb_kre[0]+=debetsh;
    deb_kre[1]+=kreditsh;
   }
  else
   {
    deb_kre[2]+=debetsh;
    deb_kre[3]+=kreditsh;
   }
  dpm(&d,&m,&g,1);
 }

//Сворачиваем начальное сальдо
if(deb_kre[0] > deb_kre[1])
 {
  deb_kre[0]-=deb_kre[1];
  deb_kre[1]=0.;
 }
else
 {
  deb_kre[1]-=deb_kre[0];
  deb_kre[0]=0.;
 }

//Вычисляем конечное сальдо развёрнутое
deb_kre[4]+=deb_kre[0]+deb_kre[2];
deb_kre[5]+=deb_kre[1]+deb_kre[3];

//Сворачиваем конечное сальдо
if(deb_kre[4] > deb_kre[5])
 {
  deb_kre[4]-=deb_kre[5];
  deb_kre[5]=0.;
 }
else
 {
  deb_kre[5]-=deb_kre[4];
  deb_kre[4]=0.;
 }

}

