/* $Id: uosrp.c,v 5.22 2013/09/26 09:43:42 sasa Exp $ */
/*14.01.2010   14.02.1994      Белых А.И.      uosrp.c
Реестр проводок по виду операции
*/
#include        <errno.h>
#include        "buhg.h"

int uosrp1(VVOD*,class spis_oth *oth);

extern  short   ddd,mmm,ggg;

void            uosrp()
{
char		bros[512];
short           dn,mn,gn;
short           dk,mk,gk;
int           kom,kom1;
static class iceb_tu_str dat1(""),dat2("");
static class iceb_tu_str podr("");
static class iceb_tu_str kontr("");
static class iceb_tu_str kodop("");
static class iceb_tu_str shet("");
static class iceb_tu_str nabor("");
static class iceb_tu_str prr("");
VVOD MENU(3);
VVOD VV(0);

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(podr.ravno(),128);
VV.VVOD_SPISOK_add_data(kontr.ravno(),128);
VV.VVOD_SPISOK_add_data(prr.ravno(),2);
VV.VVOD_SPISOK_add_data(kodop.ravno(),128);
VV.VVOD_SPISOK_add_data(shet.ravno(),128);
VV.VVOD_SPISOK_add_data(nabor.ravno(),32);

VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт реестра проводок"));
VV.VVOD_SPISOK_add_ZAG(gettext("Введите нужные реквизиты"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала..........."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца............"));
VV.VVOD_SPISOK_add_MD(gettext("Подразделение...(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Код контрагента.(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Приход/расход..(+/-).."));
VV.VVOD_SPISOK_add_MD(gettext("Код операции....(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт..(,,)............"));
VV.VVOD_SPISOK_add_MD(gettext("Набор операций........"));

naz:;

clear();

helstr(LINES-1,0,"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
dat2.new_plus(VV.VVOD_SPISOK_return_data(1));
podr.new_plus(VV.data_ravno(2));
kontr.new_plus(VV.data_ravno(3));
prr.new_plus(VV.data_ravno(4));
kodop.new_plus(VV.data_ravno(5));
shet.new_plus(VV.data_ravno(6));
nabor.new_plus(VV.data_ravno(7));

class iceb_tu_str kods("");
class iceb_tu_str naim("");

switch (kom)
 {
  case ESC:
  case FK10:
     clear();
     return;
     break;

  case PLU:
  case FK2:
     break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций приходов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций расходов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр наборов операций"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 5 :
      case -1:
       goto naz;

      case 0 :
        if(diruospod(1,&kods,&naim,0) == 0)
         VV.data_z_plus(2,kods.ravno());
        goto naz;

      case 1 :
        vibrek("Kontragent",&kontr,&naim);
        VV.VVOD_SPISOK_zapis_data(3,kontr.ravno()); 
        goto naz;

      case 2 :
        vibrek("Uospri",&kodop,&naim);
        VV.VVOD_SPISOK_zapis_data(5,kodop.ravno()); 
        goto naz;

      case 3 :
//        vibrek("Uosras",&kodop,&naim);
        if(diruosras(1,&kodop,&naim) == 0)
          VV.data_z_plus(5,kodop.ravno()); 
        goto naz;

      case 4 :

        clear();
        iceb_redfil("uosnabopr.alx",0);

        goto naz;
     }
  case FK4: //Очистить
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
 }

if(rsdatp(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(0),&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(1)) != 0)
 goto naz;

if(VV.VVOD_SPISOK_return_data(7)[0] != '\0')
 {
  class iceb_tu_str nabor("");
   if(iceb_t_poldan(VV.VVOD_SPISOK_return_data(7),&nabor,"uosnabopr.alx") != 0)
    {
     sprintf(bros,gettext("Не найден набор операций %s !"),VV.VVOD_SPISOK_return_data(5));
     iceb_t_soob(bros);
     goto naz;
    }
   else
    VV.data_plus(5,nabor.ravno());
 }
class spis_oth oth;

if(uosrp1(&VV,&oth) == 0)
 iceb_t_rabfil(&oth,"");

}
