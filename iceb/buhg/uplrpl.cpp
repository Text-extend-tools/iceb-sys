/*$Id: uplrpl.c,v 5.67 2013/08/13 05:49:58 sasa Exp $*/
/*12.07.2016	26.04.2003	Белых А.И.	uplrpl.c
Реестр путевых листов
*/
#include        <errno.h>
#include        "buhg.h"

int uplrpl_r(short dn,short mn,short gn,short dk,short mk,short gk,const char *kodavt,const char *kodvod,const char *podr,class spis_oth *oth);
int uplrpla_r(short dn,short mn,short gn,short dk,short mk,short gk,const char *kodavt,const char *kodvod,const char *podr,class spis_oth *oth);
int uplovdt(short dn,short mn,short gn,short dk,short mk,short gk,const char *kodavt,const char *kodvod,const char *podr,class spis_oth *oth);

void uplrpl_nad(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *kodvod,
const char *naimvod,
const char *,
const char *naimavt,
const char *rnavt,
FILE *ff);
void uplrpl_sapi(FILE *ff);

double per_top(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *kodvod,
const char *kodavt,double,int,int,FILE*,FILE*);


int uplrpl(int metkar, //0-реестр путевых листов под водителям 1-оборотная ведомость движения топлива 2-реестр путевых листов по автомобилям
class spis_oth *oth)
{
short	dn,mn,gn;
short	dk,mk,gk;
int     kom=0;
char	strsql[512];
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
static class iceb_tu_str kodavt("");
static class iceb_tu_str kodvod("");
static class iceb_tu_str podr("");
int		kom1=0;
class iceb_tu_str kodn("");
class iceb_tu_str naimn("");
SQLCURSOR curr;
VVOD MENU(3);
VVOD VV(0);
if(metkar == 0)
 VV.VVOD_SPISOK_add_ZAG(gettext("Распечатать реестр путевых листов по водителям"));
if(metkar == 1)
 VV.VVOD_SPISOK_add_ZAG(gettext("Распечатать оборотную ведомость движения топлива по автомобилям"));
if(metkar == 2)
 VV.VVOD_SPISOK_add_ZAG(gettext("Распечатать реестр путевых листов по автомобилям"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала............."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца.............."));//1
VV.VVOD_SPISOK_add_MD(gettext("Код автомобиля....(,,).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Код водителя......(,,).."));//3
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения.(,,).."));//4

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(kodavt.ravno(),128);
VV.VVOD_SPISOK_add_data(kodvod.ravno(),128);
VV.VVOD_SPISOK_add_data(podr.ravno(),128);

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
podr.new_plus(VV.data_ravno(4));

switch (kom)
 {

  case FK10:
  case ESC:
   return(1);

  case FK1:
    GDITE();
    if(metkar == 0)
     sprintf(strsql,"%s","upl4_1.txt");
    if(metkar == 1)
     sprintf(strsql,"%s","upl4_2.txt");
    iceb_t_pdoc(strsql);
    goto naz;

  case FK2:
  case PLU:
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка водителей"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка автомобилей"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 3:
      case -1:
        break;

      case 0:
        if(vibrek("Uplpodr",&kodn) == 0)
          VV.VVOD_SPISOK_zapis_data(4,kodn.ravno());
        break;

      case 1:
        clear();
        naimn.new_plus("");
        if(dirout(&kodn,&naimn,1,2) == 0)
          VV.VVOD_SPISOK_zapis_data(3,kodn.ravno());
        break;

      case 2:
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

if(metkar == 0)
  return(uplrpl_r(dn,mn,gn,dk,mk,gk,kodavt.ravno(),kodvod.ravno(),podr.ravno(),oth));
if(metkar == 1)
  return(uplovdt(dn,mn,gn,dk,mk,gk,kodavt.ravno(),kodvod.ravno(),podr.ravno(),oth));
if(metkar == 2)
  return(uplrpla_r(dn,mn,gn,dk,mk,gk,kodavt.ravno(),kodvod.ravno(),podr.ravno(),oth));
  
return(1); 
}
