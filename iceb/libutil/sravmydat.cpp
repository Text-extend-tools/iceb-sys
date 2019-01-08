/*$Id: sravmydat.c,v 5.10 2013/05/17 14:56:42 sasa Exp $*/
/*30.03.2009	06.05.2001	Белых А.И.	sravmydat.c
Сравнение двух дат
Если даты равны возвращаем      =  0
Если первая дата меньше         <  -1
Если первая дата больше         >  +1
*/
#include "util.h"

int	sravmydat(short d1,short m1,short g1,short d2,short m2,short g2)
{

if(g1 == g2 && m1 == m2 && d1 == d2)
  return(0);
  
if(g1 > g2)
  return(1);

if(g1 < g2)
  return(-1);

//Годы равны
if(m1 > m2)
 return(2);
if(m1 < m2)
 return(-2);

//Месяцы равны

if(d1 > d2)
 return(3);
if(d1 < d2)
 return(-3);

return(0);

}
/*********************************/
int sravmydat(const char *data1,const char *data2)
{
short d1,m1,g1,d2,m2,g2;
rsdat(&d1,&m1,&g1,data1,1);
rsdat(&d2,&m2,&g2,data2,1);

return(sravmydat(d1,m1,g1,d2,m2,g2));

}
/*********************************/
int sravmydat(const char *data1,const char *data2,int metka)
{
short d1,m1,g1,d2,m2,g2;
rsdat(&d1,&m1,&g1,data1,metka);
rsdat(&d2,&m2,&g2,data2,metka);

return(sravmydat(d1,m1,g1,d2,m2,g2));

}
