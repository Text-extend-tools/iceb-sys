/*$Id:$*/
/*21.04.2018	21.04.2018	Белых А.И.	rasdokkr.cpp
Вывод количество мест 
*/
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

void rasdokkr(double kol,double kolt,double kratz,FILE *ff)
{
int		dop=0;
double		ikolt=0.;
char		bros[512];

if(kolt != 0.)
 {
  modf(kolt,&ikolt);
  dop=(int)(kol-ikolt*kratz);
  if(dop > 0)
   sprintf(bros,"%.f/%d",ikolt,dop);
  else
   sprintf(bros,"%.f",ikolt);

  fprintf(ff," %-4s %4.4g\n",bros,kratz);
//  *kolt=0.;
 }
else
  fprintf(ff,"\n");
  
}
