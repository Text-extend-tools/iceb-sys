/*$Id: menuvnp.c,v 5.11 2013/11/05 10:50:49 sasa Exp $*/
/*01.10.2013	15.04.2009	Белых А.И.	menuvnp.c
Меню для выбора с каким файлом наcтройки проводок работать
*/
#include "buhg.h"


void menuvnp(int metka,/*1-материальный учёт 2-учёт услуг*/
int tipz) /*0 всё 1-приходные 2-расходные*/
{
int kom=0,kom1=0;
class VVOD MENU(3);
char strsql[512];
int metka_nsnp=0; /*метка наличия файла старой настройки проводок*/

if(metka == 1)
 {
  sprintf(strsql,"select ns from Alx where fil='avtpro.alx' limit 1");
  if(readkey(strsql) <= 0)
   metka_nsnp=1;
 }

if(metka == 2)
 {
  sprintf(strsql,"select ns from Alx where fil='avtprouslp.alx' limit 1");
  if(readkey(strsql) <= 0)
   metka_nsnp=1;
 }

if(metka != 3 && metka_nsnp == 0)
 {

  MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное и нажмите Enter"));

  MENU.VVOD_SPISOK_add_MD(gettext("Настройка проводок в таблице"));//0
  MENU.VVOD_SPISOK_add_MD(gettext("Настройка списков проводок"));//1
  MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2

  while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
  if(kom == 2)
   return;
 }
else
 kom=1;
  
if(tipz == 1)
 {
  if(metka == 1)
   {
    if(kom == 0 && metka_nsnp == 0)
      iceb_redfil("avtpro.alx",0);
    else
      iceb_redfil("avtpromu1.alx",0);
    return;
   }    
  if(metka == 2)
   {
    if(kom == 0 && metka_nsnp == 0)
      iceb_redfil("avtprouslp.alx",0);
    else
      iceb_redfil("avtprousl1.alx",0);
    return;
   }    
  if(metka == 3)
   {
    iceb_redfil("avtprouos1.alx",0);
   }    
  return;
 }
if(tipz == 2)
 {
  if(metka == 1)
   {
    if(kom == 0 && metka_nsnp == 0)
      iceb_redfil("avtpro.alx",0);
    else
      iceb_redfil("avtpromu2.alx",0);
    return;
   }    
  if(metka == 2)
   {
    if(kom == 0 && metka_nsnp == 0)
      iceb_redfil("avtprouslr.alx",0);
    else
      iceb_redfil("avtprousl2.alx",0);
    return;
   }    
  if(metka == 3)
   {
    iceb_redfil("avtprouos2.alx",0);
   }    
  return;
 }

switch (kom)
 {
  case 2:
  case -1:
    return;

  case 0:
    if(metka == 1)
     {
      iceb_redfil("avtpro.alx",0);
      return;
     }    

    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное и нажмите Enter"));

    MENU.VVOD_SPISOK_add_MD(gettext("Настpойка выполнения пpоводок для приходных документов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Настpойка выполнения пpоводок для расходных документов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
    switch(kom1)
     {
      case 2:
      case -1:
        return;

      case 0:
        iceb_redfil("avtprouslp.alx",0);
        break;
      case 1:
        iceb_redfil("avtprouslr.alx",0);
        break;
     }
    

    break;

  case 1:
      MENU.VVOD_delete();
      MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное и нажмите Enter"));

      MENU.VVOD_SPISOK_add_MD(gettext("Настpойка выполнения пpоводок для приходных документов"));
      MENU.VVOD_SPISOK_add_MD(gettext("Настpойка выполнения пpоводок для расходных документов"));
      MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

      kom1=0;
      while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
      switch(kom1)
       {
        case 2:
        case -1:
          return;

        case 0:
          if(metka == 1)
            iceb_redfil("avtpromu1.alx",0);
          if(metka == 2)
            iceb_redfil("avtprousl1.alx",0);
          if(metka == 3)
            iceb_redfil("avtprouos1.alx",0);
          break;
        case 1:
          if(metka == 1)
            iceb_redfil("avtpromu2.alx",0);
          if(metka == 2)
            iceb_redfil("avtprousl2.alx",0);
          if(metka == 3)
            iceb_redfil("avtprouos2.alx",0);
          break;
       }
      
    break;
 }


}
