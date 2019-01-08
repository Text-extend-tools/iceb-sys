/*$Id:$*/
/*17.05.2017	17.05.2017	Белых А.И.	uosastz818.cpp
Акт списания транспортных средств приказ министерства финансов Украини N818 от 13.09.2016 г.
*/

#include "buhg.h"

int uosastz818(short dd,short md,short gd,const char *nomdok)
{

int metka_u=0; /*0-налоговый 1-бухгалтерский учёт*/
char strsql[512];
char imaf[64];

int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
class iceb_vrvr rv;

rv.ns.plus(4);
rv.begin.plus(0);
rv.end.plus(100);
rv.hag.plus(1);
rv.str.plus(iceb_t_get_pnk("00",1));

rv.ns.plus(7);
rv.begin.plus(33);
rv.end.plus(100);
rv.hag.plus(1);
rv.str.plus(iceb_t_get_edrpou("00"));

rv.ns.plus(21);
rv.begin.plus(133);
rv.end.plus(147);
rv.hag.plus(1);
rv.str.plus(nomdok);

sprintf(strsql,"%02d.%02d.%d",dd,md,gd);
rv.ns.plus(21);
rv.begin.plus(149);
rv.end.plus(163);
rv.hag.plus(1);
rv.str.plus(strsql);

class VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное"));

MENU.VVOD_SPISOK_add_MD(gettext("Налоговый учёт"));
MENU.VVOD_SPISOK_add_MD(gettext("Бухгалтерский учёт"));
while(menu3w(stdscr,&MENU,&metka_u,-1,-1,0) != 0);

if(metka_u == -1)
 return(1);

sprintf(strsql,"select innom,bs,bsby,iz,izby from Uosdok1 where datd='%d.%02d.%02d' and nomd='%s'",gd,md,dd,nomdok);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return(1);
 }

sprintf(imaf,"astz1_%d.lst",getpid());
class iceb_fopen fil;
if(fil.start(imaf,"w") != 0)
 return(1);

iceb_t_vrvr("uosavve818_1.alx",&rv,fil.ff); /*вставка реквизитов в шапку документа*/

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
 }

double bal_st=0.;
double iznos=0.;
class iceb_tu_str naim_os("");
class iceb_tu_str pasport("");
class iceb_tu_str data_vv("");
class iceb_tu_str god_vip("");
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"select naim,pasp,datvv,god from Uosin where innom=%s",row[0]);
  if(readkey(strsql,&row1,&cur1) == 1)
   {
    naim_os.new_plus(row1[0]);
    pasport.new_plus(row1[1]);
    data_vv.new_plus(iceb_tu_datzap(row1[2]));
    god_vip.new_plus(row1[3]);
   }
  else
   {
    naim_os.new_plus("");
    pasport.new_plus("");
    data_vv.new_plus("");
    god_vip.new_plus("");
   }

  if(metka_u == 0)
   {
    bal_st=atof(row[1]);
    iznos=atof(row[3]);
   }
  else
   {
    bal_st=atof(row[2]);
    iznos=atof(row[4]);
   }   

  fprintf(fil.ff,"%-*.*s|%-11s|%-26s|%22.2f|%8.2f|%12s|%-7s|%-*.*s|%16s|\n",
  iceb_tu_kolbait(40,naim_os.ravno()),
  iceb_tu_kolbait(40,naim_os.ravno()),
  naim_os.ravno(),
  row[0],
  data_vv.ravno(),
  bal_st,
  iznos,
  "",
  god_vip.ravno(),  
  iceb_tu_kolbait(14,pasport.ravno()),
  iceb_tu_kolbait(14,pasport.ravno()),
  pasport.ravno(),
  "");    

 }

iceb_t_insfil("uosavve818_2.alx",fil.ff);

fil.end();

class spis_oth oth;

oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Акт списания транспортных средств"));

iceb_t_ustpeh(oth.spis_imaf.ravno(0),3);

iceb_t_rabfil(&oth,"");


return(0);

}
