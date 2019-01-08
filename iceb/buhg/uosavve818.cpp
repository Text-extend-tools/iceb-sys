/*$Id:$*/
/*18.05.2017	18.05.2017	Белых А.И.	uosavve818.cpp
Акт списания транспортных средств приказ министерства финансов Украини N818 от 13.09.2016 г.
*/

#include "buhg.h"

int uosavve818(short dd,short md,short gd,const char *nomdok)
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
rv.begin.plus(81);
rv.end.plus(105);
rv.hag.plus(1);
rv.str.plus(nomdok);

sprintf(strsql,"%02d.%02d.%d",dd,md,gd);
rv.ns.plus(21);
rv.begin.plus(107);
rv.end.plus(124);
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

sprintf(imaf,"avve%d.lst",getpid());
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
int itogo_kol=0;
double itogo_suma[2];
memset(itogo_suma,'\0',sizeof(itogo_suma));

class iceb_tu_str pasport("");
class iceb_tu_str god_vip("");
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"select pasp,god from Uosin where innom=%s",row[0]);
  if(readkey(strsql,&row1,&cur1) == 1)
   {
    pasport.new_plus(row1[0]);
    god_vip.new_plus(row1[1]);
   }
  else
   {
    pasport.new_plus("");
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

  fprintf(fil.ff,"|%16s|%9s|%12.2f|%12.2f|%12.2f|%12.2f|%12s|%12s|%-9s|%-*.*s|\n",
  row[0],
  "1",
  bal_st,
  bal_st,
  iznos,
  iznos,
  "",
  "",
  god_vip.ravno(),
  iceb_tu_kolbait(8,pasport.ravno()),
  iceb_tu_kolbait(8,pasport.ravno()),
  pasport.ravno());
            
  itogo_kol++;
  itogo_suma[0]+=bal_st;
  itogo_suma[1]+=iznos;
 }

fprintf(fil.ff,"\
-----------------------------------------------------------------------------------------------------------------------------\n");

fprintf(fil.ff,"%*.*s|%9d|%12.2f|%12.2f|%12.2f|%12.2f|\n\n",
iceb_tu_kolbait(17,"Всього"),
iceb_tu_kolbait(17,"Всього"),
"Всього",
itogo_kol,
itogo_suma[0],
itogo_suma[0],
itogo_suma[1],
itogo_suma[1]);

iceb_t_insfil("uosavve818_2.alx",fil.ff);

fil.end();

class spis_oth oth;

oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Акт введения в эксплуатацию"));

iceb_t_ustpeh(oth.spis_imaf.ravno(0),3);

iceb_t_rabfil(&oth,"");


return(0);

}
