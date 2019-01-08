/*$Id: iceb_t_mpods.c,v 5.4 2013-04-07 10:22:54 sasa Exp $*/
/*04.03.2013	22.04.2008	Белых А.И.	iceb_t_mpodsf.c

Получение по номеру подсистемы её символьной метки или
получение по метке подсистемы её номера
применяется только для проводок

*/
#include <string.h>
#include <libintl.h>
#include "bezcurses.h"

void iceb_t_mpods(int nomer_ps,char *kto)
{

if(nomer_ps == 1) /*Материальный учёт*/
  strcpy(kto,ICEB_MP_MATU);

if(nomer_ps == 2) /*зароботная плата*/
  strcpy(kto,ICEB_MP_ZARP);

if(nomer_ps == 3) /*учёт основных средств*/
  strcpy(kto,ICEB_MP_UOS);

if(nomer_ps == 4) /*платёжные поручения*/
  strcpy(kto,ICEB_MP_PPOR);

if(nomer_ps == 5) /*платёжные требования*/
  strcpy(kto,ICEB_MP_PTRE);


if(nomer_ps == 6) /*учёт услуг*/
  strcpy(kto,ICEB_MP_USLUGI);

if(nomer_ps == 7) /*учёт кассовых ордеров*/
  strcpy(kto,ICEB_MP_KASA);

if(nomer_ps == 8) /*учёт командировочных расходов*/
  strcpy(kto,ICEB_MP_UKR);

}
/******************************************/
/*получение по метке подсистемы её номера*/
/****************************************/
int iceb_t_mpods(char *kto)
{
if(kto[0] == '\0')
 return(0);
 
if(SRAV(ICEB_MP_MATU,kto,0) == 0)
  return(1);

if(SRAV(ICEB_MP_ZARP,kto,0) == 0)
  return(2);

if(SRAV(ICEB_MP_UOS,kto,0) == 0)
  return(3);


if(SRAV(ICEB_MP_PPOR,kto,0) == 0)
  return(4);

if(SRAV(ICEB_MP_PTRE,kto,0) == 0)
  return(5);

if(SRAV(ICEB_MP_USLUGI,kto,0) == 0)
  return(6);

if(SRAV(ICEB_MP_KASA,kto,0) == 0)
  return(7);

if(SRAV(ICEB_MP_UKR,kto,0) == 0)
  return(8);

return(0);   
}
