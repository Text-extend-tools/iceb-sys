/* $Id: prvkp.c,v 5.7 2013/05/17 14:56:08 sasa Exp $ */
/*15.07.2015	07.04.1998	Белых А.И.	prvkp.c
Проверка возможности корректировки или удаления проводки
Если вернули 0- работать можно
*/
#include        "buhg.h"


int prvkp(const char *kto)
{
extern short    zkhp;  /*0-запрещено корректировать проводки из других подсистем 1-разрешено*/

if(zkhp != 0)
 return(0);
 
if(SRAV(kto,ICEB_MP_MATU,0) == 0)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("С этой проводкой можно работать только из программы"));
  SOOB.VVOD_SPISOK_add_MD(gettext("           \"Материальный учет\" !"));
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(1);
 } 
if(SRAV(kto,ICEB_MP_UOS,0) == 0)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("С этой проводкой можно работать только из программы"));
  SOOB.VVOD_SPISOK_add_MD(gettext("          \"Учет основных средств\" !"));
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(2);
 } 
if(SRAV(kto,ICEB_MP_ZARP,0) == 0)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("С этой проводкой можно работать только из программы"));
  SOOB.VVOD_SPISOK_add_MD(gettext("            \"Заработная плата\" !"));
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(3);
 } 

if(SRAV(kto,ICEB_MP_PPOR,0) == 0 || \
SRAV(kto,ICEB_MP_PTRE,0) == 0)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("С этой проводкой можно работать только из программы"));
  SOOB.VVOD_SPISOK_add_MD(gettext("            \"Платежные документы\" !"));
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(4);
 } 

if(SRAV(kto,ICEB_MP_KASA,0) == 0)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("С этой проводкой можно работать только из программы"));
  SOOB.VVOD_SPISOK_add_MD(gettext("            \"Учет кассовых ордеров\" !"));
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(5);
 } 

if(SRAV(kto,ICEB_MP_USLUGI,0) == 0)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("С этой проводкой можно работать только из программы"));
  SOOB.VVOD_SPISOK_add_MD(gettext("                \"Учет услуг\" !"));
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(6);
 } 

if(SRAV(kto,ICEB_MP_UKR,0) == 0)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("С этой проводкой можно работать только из программы"));
  SOOB.VVOD_SPISOK_add_MD(gettext("        \"Учет командировочных расходов\" !"));
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(6);
 } 

return(0);

}
