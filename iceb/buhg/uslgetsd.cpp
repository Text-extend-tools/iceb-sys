/*$Id:$*/
/*08.07.2015	20.06.2015	Белых А.И.	uslgetsd.c
Получить сумму по документу к оплате
*/
#include "buhg.h"

extern double	okrg1;  /*Округление суммы*/

double uslgetsd(const char *datdok,const char *podr,const char *nomdok)
{

long kolstr=0;
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
double sumzap=0.;

double sumabn=0.;
double suma=0.;
int lnds=0;
double suma_nds_vv=0.; /*сумма НДС введённая вручную*/
short d=0,m=0,g=0;
double suma_opl=0.;
double proc_nds=20.;
double sumkor=0.;

rsdat(&d,&m,&g,datdok,1);

/*читаем метку ндс*/
if(podr[0] != '\0')
 sprintf(strsql,"select nds,sumkor,sumnds,pn from Usldokum where datd='%04d-%02d-%02d' and podr=%s and nomd='%s'",g,m,d,podr,nomdok);
else
 sprintf(strsql,"select nds,sumkor,sumnds,pn from Usldokum where datd='%04d-%02d-%02d' and nomd='%s'",g,m,d,nomdok);
if(readkey(strsql,&row,&cur) == 1)
 {
  lnds=atoi(row[0]);
  sumkor=atof(row[1]);
  suma_nds_vv=atof(row[2]);
  proc_nds=atof(row[3]);
 }


/*Узнаем сумму всех записей*/
sprintf(strsql,"select cena,kolih from Usldokum1 where datd='%d-%d-%d' and \
nomd='%s'",g,m,d,nomdok);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return(0.);
 }

while(cur.read_cursor(&row) != 0)
 {
  
  sumzap=atof(row[0])*atof(row[1]);
  sumzap=okrug(sumzap,okrg1);

  if(lnds > 0)
    sumabn+=sumzap;
  else
    suma+=sumzap;

 }
double suma_nds=0.;

if(lnds == 0 && suma_nds_vv == 0.)
  suma_nds=(suma+sumkor)*proc_nds/100.;

suma_opl=suma+sumkor+suma_nds+sumabn;

return(suma_opl);

}
