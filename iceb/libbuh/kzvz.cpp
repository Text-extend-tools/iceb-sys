/*$Id: kzvz.c,v 5.7 2013-04-07 10:22:54 sasa Exp $*/
/*27.03.2013	25.11.2003 	Белых А.И.	kzvz.c
Расшифровка кто записал и время записи
*/
#include <ctype.h>
#include	"buhl.h"



void kzvz(int ktozap,time_t vrem,VVOD *VV)
{
struct  tm      *bf;
char		strsql[512];
SQL_str row;
class SQLCURSOR cur;


if(ktozap != 0) /*это номер оператора*/
 {
  
  sprintf(strsql,"select fio from icebuser where un=%d",ktozap);
  if(readkey(strsql,&row,&cur) == 1)
   sprintf(strsql,"%s %d %s",gettext("Записал"),ktozap,row[0]);
  else
   sprintf(strsql,gettext("Не известный логин %d !"),ktozap);
  VV->VVOD_SPISOK_add_ZAG(strsql);

 }



if(vrem != 0)
 {
  bf=localtime(&vrem);
  sprintf(strsql,"%s %d.%d.%d%s %s:%02d:%02d:%02d",
  gettext("Дата записи"),
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
  gettext("г."),
  gettext("Время"),
  bf->tm_hour,bf->tm_min,bf->tm_sec);
  VV->VVOD_SPISOK_add_ZAG(strsql);
 }

}

/***********************************/

void kzvz(const char *ktozap,const char *vrem,VVOD *VV)
{

if(isdigit(ktozap[0]) == 0) /*Значит символ не число*/
 {
  char strsql[512];
  SQL_str row;
  class SQLCURSOR cur;

  sprintf(strsql,"select fio from icebuser where log='%s'",ktozap);
  if(readkey(strsql,&row,&cur) == 1)
   sprintf(strsql,"%s %s %s",gettext("Записал"),ktozap,row[0]);
  else
   sprintf(strsql,gettext("Не известный логин %s !"),ktozap);
  VV->VVOD_SPISOK_add_ZAG(strsql);

 }
else 
 {
  if(ktozap != NULL)
   kzvz(atoi(ktozap),atol(vrem),VV);
  else
   kzvz(0,atol(vrem),VV);
 }
}



