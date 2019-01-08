/*$Id: skopluos.c,v 5.4 2013/05/17 14:56:11 sasa Exp $*/
/*12.10.2009	12.10.2009	Белых А.И.	skopluos.c
Получение суммы к оплате для документа учёта основных средств
*/
#include        "buhg.h"
extern double okrg1;

double skopluos(short dd,short md,short gd, //Дата документа
const char *nomdok) //номер документа
{
char strsql[512];
int lnds=0;
int tipz=0;
double suma=0.;
double sumabn=0.;
SQL_str row;
class SQLCURSOR cur;
double pnds=0.;
/*узнаём НДС*/

sprintf(strsql,"select tipz,nds,pn from Uosdok where datd='%04d-%02d-%02d' and nomd='%s'",
gd,md,dd,nomdok);
if(readkey(strsql,&row,&cur) <= 0)
 return(0.);
tipz=atoi(row[0]);
lnds=atoi(row[1]);
pnds=atof(row[2]);
 
sumzapuos(dd,md,gd,nomdok,&suma,&sumabn,lnds,tipz);



double nds=suma*pnds/100.;
double suma_dok=suma+sumabn+nds;
suma_dok=okrug(suma_dok,okrg1);
return(suma_dok);
}
