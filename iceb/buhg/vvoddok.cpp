/* $Id: vvoddok.c,v 5.18 2013/08/13 05:50:00 sasa Exp $ */
/*01.02.2017    14.04.1997      Белых А.И.      vvoddok.c
Ввод накладных для материального учета
*/
#include	"buhg.h"

extern short           mdd;  /*0-обычный документ 1-двойной*/
short    blokpid; /*Кто заблокировал*/

void            vvoddok()
{
static class iceb_tu_str dat1("");
int           mpr; /*1- приход 2 -расход*/
int		tipz=0;
class iceb_tu_str kprr("");
class iceb_tu_str naimpr("");
short		dd,md,gd;
class iceb_tu_str nomdok("");
int		skl;
int		lnds;
double		sumkor;
if(dat1.ravno()[0] == '\0')
 {
  dat1.plus_tek_dat();
 }
blokpid=0;

clear();
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(gettext("Введение нового документа"));
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите какой вид документа вы будете вводить"));

MENU.VVOD_SPISOK_add_MD(gettext("Расходный документ"));
MENU.VVOD_SPISOK_add_MD(gettext("Приходный документ"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));


mpr=0;
while(menu3w(stdscr,&MENU,&mpr,-1,-1,0) != 0);

if(mpr == 2 || mpr == -1)
 return;

if(mpr == 0)
 {
  tipz=2;
 }
if(mpr == 1) 
 {
  tipz=1;
 }

/***************************************/
/*Вибираем конкретный приход или расход*/
/***************************************/
clear();

kprr.new_plus(""); 
naimpr.new_plus("");
mdd=0;
if(tipz == 1)
 {
  if(dirprihod(1,&kprr,&naimpr) != 0)
   return;
 }
else
 {
  if(dirrashod(1,&kprr,&naimpr) != 0)
   return;
 }
lnds=0;
sumkor=0.;

VVOD VV(0);

sozvdnd(tipz,&VV);

VV.VVOD_SPISOK_zapis_data(1,dat1.ravno());

if(vdnd(&VV,&dat1,tipz,&kprr,0,lnds,sumkor,0,0) != 0)
 return;

rsdat(&dd,&md,&gd,VV.VVOD_SPISOK_return_data(1),0);
skl=atoi(VV.VVOD_SPISOK_return_data(2));
nomdok.new_plus(VV.VVOD_SPISOK_return_data(3));
/*
printw("Переходим в накладную.\n");
refresh();
*/
dirdok(dd,md,gd,tipz,skl,&nomdok);

}
