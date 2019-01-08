/*$Id: raspitog.c,v 5.6 2013-04-07 10:22:43 sasa Exp $*/
/*05.03.2013	06.01.2005	Белых А.И.	raspitog.c
Распечатка итога по проводкам
*/
#include "buhg.h"

void raspitog(double dbt,double krt,int metka,FILE *ff1)
{
double	brr;

fprintf(ff1,"\
----------------------------------------------------------------------------------------------------------------------------------------------\n");

if(metka == 0)
  fprintf(ff1,"%*s:",iceb_tu_kolbait(48,gettext("Итого")),gettext("Итого"));
if(metka == 1)
  fprintf(ff1,"%*s:",iceb_tu_kolbait(48,gettext("Итого за день")),gettext("Итого за день"));
if(metka == 2)
  fprintf(ff1,"%*s:",iceb_tu_kolbait(48,gettext("Итого за месяц")),gettext("Итого за месяц"));
if(metka == 3)
  fprintf(ff1,"%*s:",iceb_tu_kolbait(48,gettext("Итого по комментарию")),gettext("Итого по комментарию"));
if(metka == 4)
  fprintf(ff1,"%*s:",iceb_tu_kolbait(48,gettext("Итого по счёту")),gettext("Итого по счёту"));

fprintf(ff1,"%13s",prnbr(dbt));
fprintf(ff1," %10s\n",prnbr(krt));

if(dbt >= krt)
 {
  brr=dbt-krt;
  fprintf(ff1,"\
%52s%10s\n"," ",prnbr(brr));
 }
else
 {
  brr=krt-dbt;
  fprintf(ff1,"\
%52s%10s %10s\n"," "," ",prnbr(brr));
 }
fprintf(ff1,"\n");

}
