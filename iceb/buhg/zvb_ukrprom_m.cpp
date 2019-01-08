/*$Id: zvb_ukrprom_m.c,v 5.12 2013/08/13 05:50:04 sasa Exp $*/
/*30.06.2013	04.07.2005	Белых А.И.	zvb_ukrprom_m.c
Меню для начала выгрузки сумм для зачисления на карт-счета Укрпромбанка
*/
#include "buhg.h"
#ifdef	HAVE_UNISTD_H
#include        <unistd.h>
#endif

void zvb_ukrprom_rn(class iceb_tu_str *kodor,class iceb_tu_str *ima_mah,class iceb_tu_str *ima_sd,class iceb_tu_str *parol);
void zvb_ukrprom(short prn,short knah,short mv,short gv,struct dirnupoi *poisk,SQLCURSOR *cur,const char *kodor,const char *ima_mah,const char *ima_sd,const char *parol,const char *nom_fil);


void	zvb_ukrprom_m(short prn,short knah,short mv,
short gv,
struct dirnupoi *poisk,
SQLCURSOR *cur)
{
int kom=0;
class iceb_tu_str kodor("");
class iceb_tu_str ima_mah("");
class iceb_tu_str ima_sd("");
class iceb_tu_str parol("");
char nom_fil[5];

strcpy(nom_fil,"1");

zvb_ukrprom_rn(&kodor,&ima_mah,&ima_sd,&parol);

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Укрпромбанк"));

VV.VVOD_SPISOK_add_data(nom_fil,sizeof(nom_fil));
VV.VVOD_SPISOK_add_data(kodor.ravno(),16);
VV.VVOD_SPISOK_add_data(ima_mah.ravno(),32);
VV.VVOD_SPISOK_add_data(ima_sd.ravno(),32);
VV.VVOD_SPISOK_add_data(parol.ravno(),32);

VV.VVOD_SPISOK_add_MD(gettext("Номер файла за текущий день....................."));
VV.VVOD_SPISOK_add_MD(gettext("Код организации................................."));
VV.VVOD_SPISOK_add_MD(gettext("Имя машины на которую будет передаваться файл..."));
VV.VVOD_SPISOK_add_MD(gettext("Имя сетевого диска.............................."));
VV.VVOD_SPISOK_add_MD(gettext("Пароль для доступа к сетевоми диску............."));


naz:;
clear();

helstr(LINES-1,0,\
//"F1",gettext("помощь"),
"F2",gettext("расчитать"),
//"F3",gettext("реквизиты"),
//"F4",gettext("очистить"),
"F5",gettext("настройка"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

strcpy(nom_fil,VV.VVOD_SPISOK_return_data(0));
kodor.new_plus(VV.VVOD_SPISOK_return_data(1));
ima_mah.new_plus(VV.VVOD_SPISOK_return_data(2));
ima_sd.new_plus(VV.VVOD_SPISOK_return_data(3));
parol.new_plus(VV.VVOD_SPISOK_return_data(4));

switch (kom)
 {
  case ESC:
  case FK10:
     return;
     break;

  case FK1:
    GDITE();
//    prosf(strsql);
    goto naz;

  case FK2:
    break;

  case FK5:
    iceb_redfil("zvb_ukrprom.alx",0);
    zvb_ukrprom_rn(&kodor,&ima_mah,&ima_sd,&parol);
    VV.data_plus(1,kodor.ravno());
    VV.data_plus(2,ima_mah.ravno());
    VV.data_plus(3,ima_sd.ravno());
    VV.data_plus(4,parol.ravno());
    goto naz;
}


zvb_ukrprom(prn,knah,mv,gv,poisk,cur,kodor.ravno(),ima_mah.ravno(),ima_sd.ravno(),parol.ravno(),nom_fil);


}
/**************************/
/*чтение настроек*/
/*************************/

void zvb_ukrprom_rn(class iceb_tu_str *kodor,
class iceb_tu_str *ima_mah,
class iceb_tu_str *ima_sd,
class iceb_tu_str *parol)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];


sprintf(strsql,"select str from Alx where fil='zvb_ukrprom.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zvb_ukrprom.alx");
  iceb_t_soob(strsql);
  return;
 }



class iceb_tu_str naim_str("");

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  polen(row_alx[0],&naim_str,1,'|');
  if(SRAV(naim_str.ravno(),"Имя машины",0) == 0)
   {
    polen(row_alx[0],ima_mah,2,'|');
    continue;   
   }
  if(SRAV(naim_str.ravno(),"Имя сетевого диска",0) == 0)
   {
    polen(row_alx[0],ima_sd,2,'|');
    continue;   
   }
  if(SRAV(naim_str.ravno(),"Пароль",0) == 0)
   {
    polen(row_alx[0],parol,2,'|');
    continue;   
   }
  if(SRAV(naim_str.ravno(),"Код организации",0) == 0)
   {
    polen(row_alx[0],kodor,2,'|');
    continue;   
   }
 }

}
