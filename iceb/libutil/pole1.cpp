/*$Id: pole1.c,v 5.15 2013/09/26 09:44:09 sasa Exp $*/
/*22.07.2013    11.11.1996      Белых А.И.      pole1.c
Поиск в массиве заданного образца
Если вернули "0" значит образец найден
если вернули "1" - нет
В переменной voz возвращаем номер поля в котором нашли образец
*/
#include	"util.h"

short pole1(const char *mas, /*массив в котором ищем образец*/
const char *obr, //образец
char raz, /*разделитель полей*/
short met, /*0-полное сравнение 1-совпадение начала 2-полное с преобразованием 3-совпадение начала с преобразованием 4-поиск образца в любом месте строки с перобрзованием*/
int *voz) /*возвращаемый номер поля в котором найден образец*/
{
short           i,i1;
char            obr1[strlen(mas)+1]; /*вся входная строка может быть образцом*/
short		metkat=met;
char		mast[strlen(mas)+1];
char		obrt[strlen(obr)+1];


*voz=0;

if(mas == NULL)
  return(1);
  
strcpy(obrt,obr);
strcpy(mast,mas);

if(met == 2 || met == 3)
 {
  /*Преобразовываем в маленькие*/
  iceb_tu_tolower_str(mast);
  iceb_tu_tolower_str(obrt);
  
  if(met == 2)
   metkat=0;


  if(met == 3)
   metkat=1;

 }


memset(obr1,'\0',sizeof(obr1));
i1=0;
for(i=0; mast[i] != '\0' && mast[i] != '\n';i++)
 {
  if(mast[i] != raz)
   {
    obr1[i1]=mast[i];
    i1++;
   }
  else
   {

    obr1[i1]='\0';
    if(met == 4)
     {
      if(strstrm(obrt,obr1) == 1) /*Перед сравнением переводит всё в один регистр*/
       return(0);
     }
    else
     {
      if(SRAV(obrt,obr1,metkat) == 0)
       return(0);
     }
    *voz=*voz+1;
    i1=0;
   }
 }
/*Последнее поле*/
obr1[i1]='\0';
if(met == 4)
 {
  if(strstrm(obrt,obr1) == 1) /*Перед сравнением переводит всё в один регистр*/
   return(0);
 }
else
 {
  if(SRAV(obrt,obr1,metkat) == 0)
     return(0);
 }

return(1);
}
