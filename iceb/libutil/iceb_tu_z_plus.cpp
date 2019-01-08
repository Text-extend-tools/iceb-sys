/*$Id: iceb_tu_z_plus.c,v 5.5 2013/05/17 14:56:41 sasa Exp $*/
/*13.02.2007	13.02.2007	Белых А.И.	iceb_tu_z_plus.c
Подпрограмма складывания двух строк, если в строке чтото есть то строка добавляется с запятой.
*/
#include <string.h>

void iceb_tu_z_plus(char *stroka_in,int dlinna,char *stroka_plus)
{
int dlinna_dan=0;
//printw("iceb_tu_z_plus\n");

if((dlinna_dan=strlen(stroka_in)) == 0)
 {
  strncpy(stroka_in,stroka_plus,dlinna-1);
  return;
 }

int dlinna_dob=dlinna-1-dlinna_dan-1; //для запятой

if(dlinna_dob > 0)
 {
  strcat(stroka_in,",");
  strcat(stroka_in,stroka_plus);
 }
}
