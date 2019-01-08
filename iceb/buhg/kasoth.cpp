/* $Id: kasoth.c,v 5.25 2013/09/26 09:43:34 sasa Exp $ */
/*09.01.2018	07.10.2000	Белых А.И.	kasoth.c
Распечатка списка ордеров за любой период времени
*/
#include        <errno.h>
#include        "buhg.h"

int kasothr(const char *dat_start,const char *dat_end,const char *kassa,const char *kodop,const char *shet,const char *vpr,const char *kod_kontr,class spis_oth *oth);
int kasothr1(short,short,short,short,short,short,const char[],const char[],const char[],const char[],const char*,class spis_oth*);


int kasoth(int metka, /*0-распечатка списка документов 1-распечатка свода по видам операций*/
class spis_oth *oth)
{
static class iceb_tu_str dat1(""); /*дата и время начала*/
static class iceb_tu_str dat2(""); /*дата и время конца*/

static class iceb_tu_str shet("");
static class iceb_tu_str kassa("");
static class iceb_tu_str kodop("");
static class iceb_tu_str vpr("");
static class iceb_tu_str kontr("");
int		kom,kom1;
short		dn,mn,gn,dk,mk,gk;
class iceb_tu_str naim("");

VVOD MENU(3);
VVOD VV(0);
if(metka == 0)
 VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка списка кассовых ордеров"));
if(metka == 1)
 VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка свода по операциям"));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(kassa.ravno(),128);
VV.VVOD_SPISOK_add_data(kodop.ravno(),128);
VV.VVOD_SPISOK_add_data(shet.ravno(),128);
VV.VVOD_SPISOK_add_data(vpr.ravno(),2);
VV.VVOD_SPISOK_add_data(kontr.ravno(),128);


VV.VVOD_SPISOK_add_MD(gettext("Дата начала..(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца...(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Код кассы....(,,)....."));
VV.VVOD_SPISOK_add_MD(gettext("Код операции.(,,)....."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт.........(,,)....."));
VV.VVOD_SPISOK_add_MD(gettext("Приход/расход.(+/-)..."));
VV.VVOD_SPISOK_add_MD(gettext("Код контрагента (,,).."));

naz:;
clear(); 

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
kassa.new_plus(VV.data_ravno(2));
kodop.new_plus(VV.data_ravno(3));
shet.new_plus(VV.data_ravno(4));
vpr.new_plus(VV.data_ravno(5));
kontr.new_plus(VV.data_ravno(6));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("kas4_1.txt");
   goto naz;

  case FK10:
  case ESC:
   return(1);

  case FK5:
  case FK2:
  case PLU:
   break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка касс"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций приходов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций расходов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));
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
        naim.new_plus("");
        if(dirkas(1,&kassa,&naim) == 0)
          VV.VVOD_SPISOK_zapis_data(2,kassa.ravno());
        goto naz;

      case 1 :
        naim.new_plus("");
        if(dirkasop(1,1,&kodop,&naim) == 0)
          VV.data_z_plus(3,kodop.ravno());
        goto naz;

      case 2 :
        naim.new_plus("");
        if(dirkasop(2,1,&kodop,&naim) == 0)
          VV.data_z_plus(3,kodop.ravno());
        goto naz;
      
      case 3 :
        vibrek("Plansh",&shet);      
        VV.VVOD_SPISOK_zapis_data(4,shet.ravno());
        goto naz;

      case 4 :
        vibrek("Kontragent",&kontr);      
        VV.VVOD_SPISOK_zapis_data(6,kontr.ravno());
        goto naz;
      
     }

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
    goto naz;
 }

if(rsdatp(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(0),&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(1)) != 0)
 goto naz;

clear();

if(metka == 0)
 return(kasothr(dat1.ravno(),dat2.ravno(),kassa.ravno(),kodop.ravno(),shet.ravno(),vpr.ravno(),kontr.ravno(),oth));

if(metka == 1)
 return(kasothr1(dn,mn,gn,dk,mk,gk,kassa.ravno(),kodop.ravno(),shet.ravno(),vpr.ravno(),kontr.ravno(),oth));

return(1);

}
