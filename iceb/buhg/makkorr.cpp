/* $Id: makkorr.c,v 5.6 2013/05/17 14:56:04 sasa Exp $ */
/*02.04.2001	21.01.1999	Белых А.И.	makkorr.c
Вычисление сумы корректировки документа
Возвращаем сумму корректировки
*/
#include	"buhg.h"

extern double	okrg1;

double		makkorr(double sum, //Сумма по документа
double *maskor)  //Массив с диапазонами и процентами корректировки
{
short		i;
double		sumk;
short		ns;

sumk=0.;

ns=(short)maskor[0];

if(maskor[0] == 1)
 {

  if(maskor[2] > 0.)
    sumk=sum*maskor[2]/100.;
  if(maskor[2] < 0.)
    sumk=sum*maskor[2]/(100+ maskor[2]*-1);
  sumk=okrug(sumk,okrg1);
/************
  printw("\n00makkorr-Вернули %.2f Процент %.2f С суммы:%.2f\n",
  sumk,maskor[2],sum);
  OSTANOV();
***********/
  return(sumk);  
 }

for(i=1; i < ns+1 ;i++)
 {
/*
  printw("ns=%d i=%d sum=%.2f maskor[i]=%.2f\n",ns,i,sum,maskor[i]);
*/
  if(i == ns)
   {
    if(maskor[i*2] > 0.)
      sumk=sum*maskor[i*2]/100.;
    if(maskor[i*2] < 0.)
      sumk=sum*maskor[i*2]/(100.+maskor[i*2]*-1);
    sumk=okrug(sumk,okrg1);
/*
    printw("\n1Вернули %.2f Процент %.2f\n",sumk,maskor[2*i]);
    OSTANOV();
*/  
    return(sumk);
    
   }

  if(sum >= maskor[i] && sum < maskor[i+1])
   {
    if(maskor[i+ns] > 0.)
      sumk=sum*maskor[i+ns]/100.;
    if(maskor[i+ns] < 0.)
      sumk=sum*maskor[i+ns]/(100.+maskor[i+ns]*-1);
    sumk=okrug(sumk,okrg1);
/*  
    printw("\n2Вернули %.2f Процент %.2f i=%d sum=%.2f ns=%d\n\
элемент массива %d\n",
    sumk,maskor[i+ns],i,sum,ns,i+ns);
    OSTANOV();
*/

    return(sumk);
   }
 }
return(sumk);
}
