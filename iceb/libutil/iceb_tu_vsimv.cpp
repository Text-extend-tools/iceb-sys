/*$Id: iceb_tu_vsimv.c,v 5.6 2013/05/17 14:56:41 sasa Exp $*/
/*03.06.2011	11.11.2009	Белых А.И.	iceb_tu_vsimv.c
Вставить символ в строку ввода
*/
#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include "util.h"

void iceb_tu_vsimv(char *stroka,int dlinastr, //Максимальная длинна строки в символах
int poz, /*Позиция символа в строке куда надо вставить*/
const char *kodsimv) /*Цепочка символов или один символ которую надо вставить*/
{
if(iceb_tu_strlen(stroka)+iceb_tu_strlen(kodsimv) > dlinastr)
 return;
 
if(poz >= dlinastr-iceb_tu_strlen(kodsimv)-1)
 return;

char strtmp[sizeof(wchar_t)*dlinastr];
memset(strtmp,'\0',sizeof(strtmp));
int kolbait=iceb_tu_kolbait(poz,stroka);
strncpy(strtmp,stroka,kolbait);
strcat(strtmp,kodsimv);
strcat(strtmp,&stroka[kolbait]);

strcpy(stroka,strtmp);
//printw("\n%s\n",stroka);

}
