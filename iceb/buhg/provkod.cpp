/*$Id: provkod.c,v 1.11 2013/05/17 14:56:08 sasa Exp $*/
/*20.03.2007	25.07.2001	Белых А.И.	provkod.c
Проверка наличия кода в массиве кодов в которых в первой ячейке записан размер массива
Возвращаем номер ячейки в котором находится код
Если не нашли возвращаем -1
*/
#include	<util.h>


int provkod(short *masiv,short kod)
{

if(masiv == NULL)
  return(-1);

for(int i1=1; i1 <= masiv[0] ; i1++)
 if(kod == masiv[i1])
   return(i1-1);

return(-1);

}

int provkod(short *masiv,char *kod)
{
return(provkod(masiv,atoi(kod)));
}
