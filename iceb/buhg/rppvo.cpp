/* $Id: rppvo.c,v 5.27 2013/09/26 09:43:40 sasa Exp $ */
/*16.04.2017    14.02.1994      Белых А.И.      rppvo.c
Реестр проводок по виду операции (материальный учет)
*/
#include        <errno.h>
#include        "buhg.h"

short           vidopr(short);
int  rppvo1(VVOD*,class spis_oth*);

extern short    ddd,mmm,ggg;

void            rppvo()
{
char		bros[512];
short           dn,mn,gn;
short           dk,mk,gk;
int           kom,kom1;
static class iceb_tu_str dat1(""),dat2("");
static class iceb_tu_str sklz("");
static class iceb_tu_str nabor("");
static class iceb_tu_str korz("");
static class iceb_tu_str kodop("");
static class iceb_tu_str shetz("");
static class iceb_tu_str prr("");
static class iceb_tu_str kodgr("");
VVOD MENU(3);
VVOD VV(0);

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(sklz.ravno(),128);
VV.VVOD_SPISOK_add_data(korz.ravno(),128);
VV.VVOD_SPISOK_add_data(prr.ravno(),2);
VV.VVOD_SPISOK_add_data(kodop.ravno(),128);
VV.VVOD_SPISOK_add_data(shetz.ravno(),128);
VV.VVOD_SPISOK_add_data(nabor.ravno(),16);
VV.VVOD_SPISOK_add_data(kodgr.ravno(),128);

VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт реестра проводок"));
VV.VVOD_SPISOK_add_ZAG(gettext("Введите реквизиты поиска"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала............"));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца............."));//1
VV.VVOD_SPISOK_add_MD(gettext("Склад............(,,).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Код контрагента..(,,).."));//3
VV.VVOD_SPISOK_add_MD(gettext("Приход/расход. .(+/-).."));//4
VV.VVOD_SPISOK_add_MD(gettext("Код операции.....(,,).."));//5
VV.VVOD_SPISOK_add_MD(gettext("Счёт......(,,)........."));//6
VV.VVOD_SPISOK_add_MD(gettext("Набор операций........."));//7
VV.VVOD_SPISOK_add_MD(gettext("Группа контрагента....."));//8



naz:;

clear();


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
dat2.new_plus(VV.VVOD_SPISOK_return_data(1));
sklz.new_plus(VV.data_ravno(2));
korz.new_plus(VV.data_ravno(3));
prr.new_plus(VV.data_ravno(4));
kodop.new_plus(VV.data_ravno(5));
shetz.new_plus(VV.data_ravno(6));
nabor.new_plus(VV.data_ravno(7));
kodgr.new_plus(VV.data_ravno(8));

class iceb_tu_str kods("");
class iceb_tu_str naims("");

switch (kom)
 {
  case ESC:
  case FK10:
     clear();
     return;
     break;

  case FK1:
    GDITE();
    iceb_t_pdoc("matu5_9.txt");
    clear();
    goto naz;

  case FK2:
  case PLU:
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка складов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций приходов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций расходов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр наборов операций"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп контрагентов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 7 :
      case -1:
       goto naz;

      case 0 :
        if(dirsklad(1,&kods,&naims) == 0)
         VV.data_z_plus(2,kods.ravno());
        goto naz;

      case 1 :
        vibrek("Kontragent",&korz,&naims);
        VV.VVOD_SPISOK_zapis_data(3,korz.ravno());
        goto naz;

      case 2 :
       if(dirprihod(1,&kods,&naims) == 0)
         VV.data_z_plus(5,kods.ravno());
       goto naz;

      case 3 :
        if(dirrashod(1,&kodop,&naims) == 0)
          VV.data_z_plus(5,kodop.ravno());
        goto naz;

      case 4 :
        vibrek("Plansh",&shetz,&naims);
        VV.VVOD_SPISOK_zapis_data(6,shetz.ravno());
        goto naz;
               
      case 5 :
        iceb_redfil("nabopr.alx",0);
        goto naz;

      case 6 :
        vibrek("Gkont",&kodgr,&naims);
        VV.VVOD_SPISOK_zapis_data(8,kodgr.ravno());
        goto naz;
     }
    goto naz;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
    break;
 }

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
 goto naz;

if(VV.VVOD_SPISOK_return_data(7)[0] != '\0')
 {
  class iceb_tu_str nabor("");
   if(iceb_t_poldan(VV.VVOD_SPISOK_return_data(7),&nabor,"nabopr.alx") != 0)
    {
     sprintf(bros,gettext("Не найден набор операций %s !"),VV.VVOD_SPISOK_return_data(7));
     iceb_t_soob(bros);
     goto naz;
    }
   else
    VV.data_plus(4,nabor.ravno());
 }

clear();
refresh();

class spis_oth oth;

if(rppvo1(&VV,&oth) == 0)
 iceb_t_rabfil(&oth,"");

}
