/* $Id: viknu.c,v 5.13 2013/08/13 05:49:59 sasa Exp $ */
/*07.09.2014	21.09.2000	Белых А.И.	viknu.c
Ввод и корректировка начисления или удержания
*/
#include        "buhg.h"

extern short	mmm,ggg;

void		viknu()
{
int		kom;
short		prn=0;
class iceb_tu_str kod("");
class iceb_tu_str naim("");
short		knah;
char		strsql[512];
SQL_str		row;

clear();

VVOD MENU(3);
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка начисления"));
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка удержания"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное"));

kom=0;
while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

switch (kom)
 {
  case 2 :
  case -1 :
   return;

  case 0:
   prn=1;
   break;  

  case 1:
   prn=2;
   break;  
 }
clear();

helstr(LINES-1,0,
"Enter",gettext("просмотр кодов"),
"F10",gettext("выход"),NULL);

if(prn == 1)
 {
  VVOD VVOD1(2);

  kod.new_plus("");
  VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код начисления"));

  if((kom=vvod1(&kod,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
    return;
  if(kom == ESC)
    return;
  GDITE();
  if(kod.getdlinna() <= 1)
   {
    if(dirnach(1,&kod,&naim) != 0)
     return;

   }


 }

if(prn == 2)
 {
  VVOD VVOD1(2);

  VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код удержания"));
  
  kod.new_plus("");
  if((kom=vvod1(&kod,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
    return;
  if(kom == ESC)
    return;
  if(kod.getdlinna() <= 1)
   {
    if(diruder(1,&kod,&naim) != 0)
     return;
   }

 }
GDITE();
class SQLCURSOR curr;

/*Проверяем код*/
if(prn == 1)
 sprintf(strsql,"select kod from Nash where kod=%s",kod.ravno());
if(prn == 2)
 sprintf(strsql,"select kod from Uder where kod=%s",kod.ravno());
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  VVOD SOOB(1);
  if(prn == 1)
   sprintf(strsql,"%s %s !",gettext("Не найден код начисления"),kod.ravno());  
  if(prn == 2)
   sprintf(strsql,"%s %s !",gettext("Не найден код удержания"),kod.ravno());  
  SOOB.VVOD_SPISOK_add_MD(strsql);
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return;
 } 
knah=kod.ravno_atoi();

dirnu(prn,knah,mmm,ggg);

}

