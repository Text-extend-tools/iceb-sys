/*$Id: skoplmu.c,v 5.6 2013/05/17 14:56:11 sasa Exp $*/
/*15.10.2009	12.10.2009	Белых А.И.	skoplmu.c
Получение суммы к оплате для документа материального учёта
*/
#include "buhg.h"
extern double okrg1;

double skoplmu(short dd,short md,short gd, //Дата документа
const char *nomdok, //номер документа
int sklad)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
double suma=0.;
double sumkor=0.;
short vt=0;
int tipz=0;
double kolihmat=0.;
double sumabn=0.;
int lnds=0;
float pnds=0.;
  
/*читаем шапку документа*/
sprintf(strsql,"select tip,pn from Dokummat where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
gd,md,dd,sklad,nomdok);
if(readkey(strsql,&row,&cur) == 0)
 {
  sprintf(strsql,"%s! %d.%d.%d %d %s",gettext("Не найдена шапка документа"),dd,md,gd,sklad,nomdok);
  iceb_t_soob(strsql);
  return(0.);
 }
pnds=atof(row[1]);
tipz=atoi(row[0]);

memset(strsql,'\0',sizeof(strsql));
if(iceb_t_poldan("Код группы возвратная тара",strsql,"matnast.alx") == 0)
 vt=atoi(strsql);

sumzap1(dd,md,gd,nomdok,sklad,&suma,&sumabn,lnds,vt,&sumkor,tipz,&kolihmat);
double suma_dok=suma+sumkor+sumabn+(suma+sumkor)*pnds/100.;
suma_dok=okrug(suma_dok,okrg1);
return(suma_dok);
}
