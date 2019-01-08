/* $Id: rppvou.c,v 5.18 2013/09/26 09:43:40 sasa Exp $ */
/*18.11.2011    06.09.2000      Белых А.И.      rppvou.c
Реестр проводок по виду операции (Учет услуг)
*/
#include        <errno.h>
#include        "buhg.h"

int rppvou1(short dn,short mn,short gn,short dk,short mk,short gk,const char *podr,const char *kontr,const char *kodop,const char *shet,const char *prihr,class spis_oth *oth);

extern short    ddd,mmm,ggg;

int rppvou(class spis_oth *oth)
{
short           dn,mn,gn;
short           dk,mk,gk;
int           kom,kom1;
static class iceb_tu_str dat1(""),dat2("");
static class iceb_tu_str kontr("");
static class iceb_tu_str kodop("");
static class iceb_tu_str shetz("");
static class iceb_tu_str prr("");
static class iceb_tu_str podr("");
VVOD VV(0);
VVOD MENU(3);
VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(podr.ravno(),128);
VV.VVOD_SPISOK_add_data(kontr.ravno(),128);
VV.VVOD_SPISOK_add_data(prr.ravno(),2);
VV.VVOD_SPISOK_add_data(kodop.ravno(),128);
VV.VVOD_SPISOK_add_data(shetz.ravno(),128);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала............"));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца............."));
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения.(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Код контрагента.(,,)..."));
VV.VVOD_SPISOK_add_MD(gettext("Приход/расход. .(+/-).."));
VV.VVOD_SPISOK_add_MD(gettext("Код операции....(,,)..."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт.....(,,).........."));

VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт реестра проводок."));
VV.VVOD_SPISOK_add_ZAG(gettext("Введите реквизиты поиска."));


naz:;

clear();


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
podr.new_plus(VV.data_ravno(2));
kontr.new_plus(VV.data_ravno(3));
prr.new_plus(VV.data_ravno(4));
kodop.new_plus(VV.data_ravno(5));
shetz.new_plus(VV.data_ravno(6));

class iceb_tu_str naim("");

switch (kom)
 {
  case ESC:
  case FK10:
     clear();
     return(1);
     break;

  case FK1:
    GDITE();
    iceb_t_pdoc("usl4_3.txt");
    clear();
    goto naz;

  case FK2:
  case PLU:
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций приходов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций расходов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
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
        vibrek("Uslpodr",&podr,&naim);
        VV.VVOD_SPISOK_zapis_data(2,podr.ravno());
        goto naz;

      case 1 :
        vibrek("Kontragent",&kontr,&naim);
        VV.VVOD_SPISOK_zapis_data(3,kontr.ravno());
        goto naz;

      case 2 :
        if(diruslop1(1,&kodop,&naim) == 0)
          VV.data_z_plus(5,kodop.ravno());
        goto naz;

      case 3 :
        if(diruslop2(1,&kodop,&naim) == 0)
          VV.data_z_plus(5,kodop.ravno());
        goto naz;

      case 4 :
        vibrek("Plansh",&shetz,&naim);
        VV.VVOD_SPISOK_zapis_data(6,shetz.ravno());
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

/*Готовим файл с отсортированными записями*/

clear();
refresh();

return(rppvou1(dn,mn,gn,dk,mk,gk,podr.ravno(),kontr.ravno(),kodop.ravno(),shetz.ravno(),prr.ravno(),oth));

}
