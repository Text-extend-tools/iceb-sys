/*$Id: vvodkr.c,v 5.15 2013/09/26 09:43:44 sasa Exp $*/
/*05.11.2007	25.10.2002	Белых А.И.	vvodkr.c
Вод командировочных расходов в документ
*/
#include <ctype.h>
#include	"buhg.h"

extern double	okrcn;  /*Округление цены*/
extern double	okrg1;  /*Округление суммы*/

void vvodkr(short dd,short md,short gd,const char *nomdok)
{
int		kvoz=0;
class iceb_tu_str kodkr("");
class iceb_tu_str naim("");

VVOD VVOD1(2);

helstr(LINES-1,0,"Enter",gettext("просмотр кодов"),
"F10",gettext("выход"),"  ",
gettext(" + перед числом - поиск по числу в наименовании"),NULL);

VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код или наименование командировочного расхода"));

if((kvoz=vvod1(&kodkr,40,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
   return;
if(kvoz == ESC)
  return;

GDITE();

if(kodkr.getdlinna() <= 1 || isdigit(kodkr.ravno()[0]) == 0)
 {
  clear();
  if(polen(kodkr.ravno(),&naim,2,'+') != 0)
    naim.new_plus(kodkr.ravno());

  if(dirkkr(&kodkr,&naim,1) != 0)
   return;
 }
vvodkr1(dd,md,gd,nomdok,kodkr.ravno(),"",0,0);
}
