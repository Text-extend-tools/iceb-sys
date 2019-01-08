/* $Id: rppvokr.c,v 5.18 2013/09/26 09:43:40 sasa Exp $ */
/*20.11.2011    28.10.2002      Белых А.И.      rppvokr.c
Реестр проводок по виду операции (Учет командировочных расходов)
*/
#include        <errno.h>
#include        "buhg.h"

int rppvokr1(short,short,short,short,short,short,const char*,const char*,const char*,class spis_oth *oth);


int rppvokr(class spis_oth *oth)
{
short           dn,mn,gn;
short           dk,mk,gk;
int           kom,kom1;
static class iceb_tu_str dat1(""),dat2("");
static class iceb_tu_str kontr("");
static class iceb_tu_str kodop("");
static class iceb_tu_str shetz("");

VVOD VV(0);
VVOD MENU(3);
VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(kontr.ravno(),128);
VV.VVOD_SPISOK_add_data(kodop.ravno(),128);
VV.VVOD_SPISOK_add_data(shetz.ravno(),128);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала..(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца...(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Код контрагента.(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Вид кома-ировки.(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт.....(,,)........."));

VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт реестра проводок"));
VV.VVOD_SPISOK_add_ZAG(gettext("Введите реквизиты поиска"));

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
kontr.new_plus(VV.data_ravno(2));
kodop.new_plus(VV.data_ravno(3));
shetz.new_plus(VV.data_ravno(4));

switch (kom)
 {
  case ESC:
  case FK10:
     clear();
     return(1);
     break;

  case FK1:
    GDITE();
    iceb_t_pdoc("ukr4_1.txt");
    clear();
    goto naz;

  case FK2:
  case PLU:
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка видов командировок"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 3 :
      case -1:
       goto naz;

      case 0 :
        vibrek("Kontragent",&kontr);
        VV.VVOD_SPISOK_zapis_data(2,kontr.ravno());
        goto naz;

      case 1 :
        vibrek("Plansh",&shetz);
        VV.VVOD_SPISOK_zapis_data(4,shetz.ravno());
        goto naz;
               
      case 2 :
        vibrek("Ukrvkr",&kodop);
        VV.VVOD_SPISOK_zapis_data(3,kodop.ravno());
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

return(rppvokr1(dn,mn,gn,dk,mk,gk,kontr.ravno(),kodop.ravno(),shetz.ravno(),oth));

}
