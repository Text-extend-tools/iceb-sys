/* $Id: okrug.c,v 5.5 2013/05/17 14:56:42 sasa Exp $*/
/*16.05.2010    06.09.1993      Белых А.И.      okrug.c
Подпрограмма округления числа
*/
#include	<math.h>
//#include "util.h"
double          okrug(double sh,double mt)
{
double          sh1=0.;
double		cel,drob;
if(sh == 0.)
 return(0.);
if(mt == 0.)
 return(sh);
 
sh1=sh/mt;
 
drob=modf(sh1,&cel);
//printw("\nsh=%f mt=%f cel=%.2f drob=%.8g\n",sh,mt,cel,drob);
//OSTANOV();
if(fabs(drob) > 0.499 )
 {
/*  printw("**\n");*/
  if(drob > 0)
    cel=cel+1.;
  if(drob < 0)
    cel=cel-1.;
  
 }
/*printw("cel=%.2f\n",cel);*/
sh1=cel*mt;
return(sh1);
}
