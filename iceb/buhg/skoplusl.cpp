/*$Id: skoplusl.c,v 5.5 2013/05/17 14:56:11 sasa Exp $*/
/*12.10.2009	12.10.2009	Белых А.И.	skoplusl.c
Сумма к оплате по документа учёта услуг
*/
#include "buhg.h"
extern double okrg1;

double skoplusl(short dd,short md,short gd, //Дата документа
const char *nomdok, //номер документа
int podr)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
double suma=0.;
double sumabn=0.;
int lnds=0;
int tipz=0;
float pnds=0.;
sprintf(strsql,"select tp,nds,pn from Usldokum where datd='%04d-%02d-%02d' and podr=%d and nomd='%s'",
gd,md,dd,podr,nomdok);
if(readkey(strsql,&row,&cur) <= 0)
 return(0.);
tipz=atoi(row[0]);
lnds=atoi(row[1]);
pnds=atof(row[2]);
 
sumzap1u(dd,md,gd,nomdok,podr,&suma,&sumabn,lnds,tipz);

double nds=suma*pnds/100.;
double suma_dok=suma+sumabn+nds;
suma_dok=okrug(suma_dok,okrg1);

return(suma_dok);


}
