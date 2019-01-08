/*$Id: srav1.c,v 5.5 2013/05/17 14:56:42 sasa Exp $*/
/*05.11.2007    13.10.1987     Белых А.И.   "SRAV1.C"
Программа сравнения двух дат.
Если даты равны возвращаем      -  0
Если дата меньше                - -1
Если дата больше                - +1
Передается максимальный срок. K111,K222,K333 - срок
который проверяется.
*/
#include	"util.h"

#define         I       0       /*даты равны*/
#define         I1      -1      /*дата меньше*/
#define         I2      1       /*дата больше*/
#define         POL     50      /*половина века*/

int  SRAV1(short DYEAR,short DMONTH,short DDAY,short K333,short K222,short K111)
//short           DYEAR,DMONTH,DDAY;/*дата которую проверяем*/
//short           K333,K222,K111;  /*контрольная дата*/
{
/*printf("%d.%d.%d    %d.%d.%d \n",DYEAR,DMONTH,DDAY,K333,K222,K111);*/

if(DYEAR == 0 && DMONTH == 0 && DDAY == 0)
  return(0);
if(K333 == 0 && K222 == 0 && K111 == 0)
  return(0);
  
if(DYEAR == 0 && DMONTH > 0 && K333 <= POL)
  return(I2);
if(DYEAR == 0 && DMONTH > 0 && K333 > POL)
  return(I1);


if(DYEAR < K333 )
  return(I2);
if(DYEAR > K333 )
  return(I1);

if(DYEAR == K333 )
 {
  if(DMONTH < K222)
    return(I2);

  if(DMONTH > K222 )
    return(I1);

  if(DMONTH == K222)
   {
    if(DDAY < K111)
     return(I2);
    if(DDAY > K111)
     return(I1);
    if(DDAY == K111)
     return(I);
   }
 }
return(0); //Сюда не должно подать
}
