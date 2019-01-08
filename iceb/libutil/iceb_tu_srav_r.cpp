/*$Id: iceb_tu_srav_r.c,v 5.5 2013/05/17 14:56:41 sasa Exp $*/
/*21.02.2007	21.02.2007	Белых А.И.	iceb_tu_srav_r.c
Сравнение двух строк без учёта регистра букв
Если вернуди 0-строки равны
             1-не равны
*/
#include "util.h"


int iceb_tu_srav_r(const char *stroka1,const char *stroka2,int metka) //0-полное сравнение 1-первых символов
{

char obr1[strlen(stroka1)+1];
char obr2[strlen(stroka2)+1];

strcpy(obr1,stroka1);
strcpy(obr2,stroka2);

//Переводим в нижний регистр
iceb_tu_tolower_str(obr1);
iceb_tu_tolower_str(obr2);

return(SRAV(obr1,obr2,metka));



}
