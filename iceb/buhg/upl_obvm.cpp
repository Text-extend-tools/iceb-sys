/*$Id: upl_obvm.c,v 5.11 2013/05/17 14:56:14 sasa Exp $*/
/*21.11.2011	27.06.2005	Белых А.И.	upl_obvm.c
Меню для расчёта оборотной ведомости движения топлива по автомобилям и водителям
*/
#include        "buhg.h"

int upl_obv(short dn,short mn,short gn,short dk,short mk,short gk,const char *kodavt,const char *kodvod,class spis_oth*);


int upl_obvm(class spis_oth *oth)
{
short	dn,mn,gn;
short	dk,mk,gk;
int     kom=0;
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
static class iceb_tu_str kodavt("");
static class iceb_tu_str kodvod("");
int		kom1=0;
class iceb_tu_str kodn("");
class iceb_tu_str naimn("");

VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Распечатать оборотную ведомость движения топлива"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала............."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца.............."));//1
VV.VVOD_SPISOK_add_MD(gettext("Код автомобиля....(,,).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Код водителя......(,,).."));//3

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(kodavt.ravno(),128);
VV.VVOD_SPISOK_add_data(kodvod.ravno(),128);

naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,0,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
kodavt.new_plus(VV.data_ravno(2));
kodvod.new_plus(VV.data_ravno(3));

switch (kom)
 {

  case FK10:
  case ESC:
   return(1);

  case FK1:
    GDITE();
//    prosf(strsql);
    goto naz;

  case FK2:
  case PLU:
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка водителей"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка автомобилей"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 2:
      case -1:
        break;


      case 0:
        clear();
        naimn.new_plus("");
        if(dirout(&kodn,&naimn,1,2) == 0)
          VV.VVOD_SPISOK_zapis_data(3,kodn.ravno());
        break;

      case 1:
        clear();
        naimn.new_plus("");
        if(diravt0(&kodn,&naimn,1) == 0)
          VV.VVOD_SPISOK_zapis_data(2,kodn.ravno());
        break;

     }
    goto naz;


  default:
    goto naz;
 }

if(rsdatp(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(0),&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(1)) != 0)
  goto naz;

return(upl_obv(dn,mn,gn,dk,mk,gk,kodavt.ravno(),kodvod.ravno(),oth));

}
