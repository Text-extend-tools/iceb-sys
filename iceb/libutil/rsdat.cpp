/*$Id: rsdat.c,v 5.11 2012-10-30 09:24:34 sasa Exp $*/
/*16.04.2017    11.11.1993      Белых А.И.      rsdat.c
Подпрограмма расшифровки символьной строки с датой
Если вернули 0 - дата введена правильно
             1 - день не правильно
             2 - месяц не правильно
             3 - год не правильно
             (отменено)4 - введена будущая дата больше чем год вперед
*/
#include        <time.h>
#include	"util.h"

int rsdat(short *d,short *m,short *g,const char *stroka,int mr)
/*
    		mr =  0-автоматическое определение даты
                      1 дд.мм.гггг
                      2 гггг-мм-дд
                      3 дд месяц гггг
                      */
{
class iceb_tu_str bros("");
struct  tm      *bf;
time_t          tmm;
int		i;
char *str=new char[strlen(stroka)+1];
*d=0;
*m=0;
*g=0;
strcpy(str,stroka);

if(mr == 0 || mr == 1)
 {
  for(i=0; str[i] != '\0'; i++)
   {
    if(str[i] == ',')
      str[i]='.';
   }      
  if(polen(str,&bros,1,'.') == 0)
   {
    *d=(short)bros.ravno_atoi();

    polen(str,m,2,'.');

    polen(str,g,3,'.');

   }
  else
   {
    if(strlen(str) == 8) /*дата без разделительныйх символов*/
     {
      char tmp[16];

      memset(tmp,'\0',sizeof(tmp));
      strncpy(tmp,str,2);
      *d=atoi(tmp);

      memset(tmp,'\0',sizeof(tmp));
      strncpy(tmp,&str[2],2);
      *m=atoi(tmp);

      memset(tmp,'\0',sizeof(tmp));
      strncpy(tmp,&str[4],4);
      *g=atoi(tmp);
     }
   }
 }
if(mr == 0 || mr == 2)
if(polen(str,&bros,1,'-') == 0)
 {
  *g=(short)bros.ravno_atoi();

  polen(str,m,2,'-');

  polen(str,d,3,'-');

 }

if(mr == 0 || mr == 3)
if(polen(str,&bros,1,' ') == 0 || polen(str,&bros,1,'-') == 0)
 {
  *d=(short)bros.ravno_atoi();

  if(strstrm(str,gettext("янв")) != 0)
   *m=1;

  if(strstrm(str,gettext("фев")) != 0)
   *m=2;

  if(strstrm(str,gettext("мар")) != 0)
   *m=3;

  if(strstrm(str,gettext("апр")) != 0)
   *m=4;

  if(strstrm(str,gettext("мая")) != 0)
   *m=5;

  if(strstrm(str,gettext("июн")) != 0)
   *m=6;

  if(strstrm(str,gettext("июл")) != 0)
   *m=7;

  if(strstrm(str,gettext("авг")) != 0)
   *m=8;

  if(strstrm(str,gettext("сен")) != 0)
   *m=9;

  if(strstrm(str,gettext("окт")) != 0)
   *m=10;

  if(strstrm(str,gettext("ноя")) != 0)
   *m=11;

  if(strstrm(str,gettext("дек")) != 0)
   *m=12;
  if(*m == 0)
   {
    if(strstrm(str,"янв") != 0)
     *m=1;

    if(strstrm(str,"фев") != 0)
     *m=2;

    if(strstrm(str,"мар") != 0)
     *m=3;

    if(strstrm(str,"апр") != 0)
     *m=4;

    if(strstrm(str,"мая") != 0 || strstrm(str,"май") != 0)
     *m=5;

    if(strstrm(str,"июн") != 0)
     *m=6;

    if(strstrm(str,"июл") != 0)
     *m=7;

    if(strstrm(str,"авг") != 0)
     *m=8;

    if(strstrm(str,"сен") != 0)
     *m=9;

    if(strstrm(str,"окт") != 0)
     *m=10;

    if(strstrm(str,"ноя") != 0)
     *m=11;

    if(strstrm(str,"дек") != 0)
     *m=12;
   }   
  if(polen(str,&bros,3,' ') == 0  || polen(str,&bros,3,'-') == 0 )
    *g=(short)bros.ravno_atoi();
 }

delete [] str;

if(*d > 31 || *d < 1)
 return(1);

if((*m == 4 || *m == 6 || *m == 9 || *m == 11) && *d > 30)
 return(1);

if(*m == 2)
 {
  int metka_vis_god=0;
//   if(*g % 4 == 0 && *g % 100 != 0 || *g % 400 == 0)
   if(*g % 4 == 0 && *g % 100 != 0)
      metka_vis_god=1;

   if( *g % 400 == 0)
     metka_vis_god=1;

    if(metka_vis_god ==1)
     {
      if(*d > 29) /*Год високосный*/
        return(1);
     } 
    else
      if(*d > 28) /*Год не високосный*/
        return(1);
 }   

if(*m > 12 || *m < 1)
 return(2);

time(&tmm);
bf=localtime(&tmm);

if(*g < 1900 || *g > bf->tm_year+1900+1000)
 return(3);

//god=bf->tm_year+1900+1;
//mes=bf->tm_mon+1;
//if(SRAV1(god,mes,(short)bf->tm_mday,*g,*m,*d) > 0)
// return(4);

return(0);
}
