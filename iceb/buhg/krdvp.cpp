/*$Id: krdvp.c,v 5.5 2013/05/17 14:56:04 sasa Exp $*/
/*05.10.2003	05.10.2003	Белых А.И.	krdvp.c
Вычисление количества рабочих дней в периоде
*/
#include  "buhg.h"


int krdvp(short dn,short mn,short gn,short dk,short mk,short gk)
{

if(dn == 0 || mn == 0 || gn == 0 || dk == 0 || mk == 0 || gk == 0)
 return(0);
short d=dn;
short m=mn;
short g=gn;
int kolrd=0;

while(sravmydat(d,m,g,dk,mk,gk) <= 0)
 {
  if(vixod(d,m,g) != 0)
   kolrd++;  
  dpm(&d,&m,&g,1);
 }
return(kolrd);
}
