/*$Id: iceb_tu_vstav.c,v 5.5 2013/10/04 07:25:14 sasa Exp $*/
/*29.09.2013	15.09.2011	Белых А.И.	iceb_tu_vstav.c
Вставка строки в заданном диапазоне
*/
#include <glib.h>
#include "util.h"


/************************************/
void iceb_tu_vstav(char *stroka, /*строка в которую вставляем*/
int znah,                     /*строка которую вставляем*/
int poz1,                        /*позиция символа в строке откуда начинаем вставлять нумерация начинается с 0*/
int poz2,                        /*позиция символа в строке по которую вставляем нумерация начинается с 0*/
int hag,                         /*шаг вставки*/
int kolbait_stroka)              /*размер stroka в байтах*/
{
char strtmp[64];
sprintf(strtmp,"%d",znah);

iceb_tu_vstav(stroka,strtmp,poz1,poz2,hag,kolbait_stroka);

}

/*************************************/
void iceb_tu_vstav(char *stroka, /*строка в которую вставляем*/
const char *strin,                     /*строка которую вставляем*/
int poz1,                        /*позиция символа в строке откуда начинаем вставлять нумерация начинается с 0*/
int poz2,                        /*позиция символа в строке по которую вставляем нумерация начинается с 0*/
int hag,                         /*шаг вставки*/
int kolbait_stroka)              /*размер stroka в байтах*/
{
char strtmp[64];
int kol_simv_strin=iceb_tu_strlen(strin);
int nom_strin=0;

int metka_utf=0; 
if(g_utf8_validate(strin,-1,NULL) == TRUE) /*текст в utf8 кодировке*/
  metka_utf=1;

for(int nom=poz1; nom <= poz2 && nom_strin < kol_simv_strin; nom+=hag)
 {
  int kols=0;
  if(metka_utf == 1)
   kols=iceb_tu_kolbait(1,iceb_tu_adrsimv(nom_strin,strin));
  else 
   kols=1;
  sprintf(strtmp,"%.*s",kols,iceb_tu_adrsimv(nom_strin,strin));
  nom_strin++;
  iceb_tu_zvstrp(stroka,nom,strtmp,kolbait_stroka);  
 }

}
/**********************************/
void iceb_tu_vstav(class iceb_tu_str *stroka, /*строка в которую вставляем*/
int znah,                     /*строка которую вставляем*/
int poz1,                        /*позиция символа в строке откуда начинаем вставлять нумерация начинается с 0*/
int poz2,                        /*позиция символа в строке по которую вставляем нумерация начинается с 0*/
int hag)                         /*шаг вставки*/
{

char *bros=NULL;

unsigned kolbait=strlen(stroka->ravno())+1;
if(kolbait < poz2*sizeof(wchar_t)+1)
 kolbait=poz2*sizeof(wchar_t)+1;

bros=new char[kolbait];

strcpy(bros,stroka->ravno()); 

iceb_tu_vstav(bros,znah,poz1,poz2,hag,kolbait);
stroka->new_plus(bros);
delete [] bros;
}
/**********************************/
void iceb_tu_vstav(class iceb_tu_str *stroka, /*строка в которую вставляем*/
const char *znah,                     /*строка которую вставляем*/
int poz1,                        /*позиция символа в строке откуда начинаем вставлять нумерация начинается с 0*/
int poz2,                        /*позиция символа в строке по которую вставляем нумерация начинается с 0*/
int hag)                         /*шаг вставки*/
{

char *bros=NULL;

unsigned kolbait=strlen(stroka->ravno())+8+strlen(znah); /*при вставке многобайтных символов строка должна иметь больший размер*/
if(kolbait < poz2*sizeof(wchar_t)+8)
 {
  kolbait=poz2*sizeof(wchar_t)+1;
 }

//char bold_start[8];
//char bold_end[8];
//sprintf(bold_start,"\x1B\x45"); /*Включение режима выделенной печати*/
//sprintf(bold_end,"\x1B\x46"); /*Выключение режима выделенной печати*/
//sprintf(bold_start,"%c",15); /*Включение режима выделенной печати*/
//sprintf(bold_end,"%c",18); /*Выключение режима выделенной печати*/

bros=new char[kolbait];

strcpy(bros,stroka->ravno()); 

//iceb_tu_vsimv(bros,strlen(bros),poz1-1,bold_start);
//iceb_tu_vsimv(bros,strlen(bros),poz2+1,bold_end);

iceb_tu_vstav(bros,znah,poz1,poz2,hag,kolbait);

stroka->new_plus(bros);

delete [] bros;
}
