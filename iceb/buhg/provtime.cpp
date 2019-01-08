/* $Id: provtime.c,v 5.22 2013/05/17 14:56:08 sasa Exp $ */
/*15.07.2015	02.06.2001	Белых А.И.	provtime.c
Просмотр проводок по времени их записи в базу
*/
#include        "buhg.h"


void	provtime()
{
int		kom1=0;
struct  tm      *bf;
time_t          tmm;
char		strsql[512];

VVOD MENU(3);

MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите период за который хотите просмотреть проводки"));
MENU.VVOD_SPISOK_add_ZAG(gettext("Поиск будет выполняться по времени записи проводок"));

MENU.VVOD_SPISOK_add_MD(gettext("За последних две минуты"));
MENU.VVOD_SPISOK_add_MD(gettext("За последних десять минут"));
MENU.VVOD_SPISOK_add_MD(gettext("За последний час"));
MENU.VVOD_SPISOK_add_MD(gettext("За текущий день"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

if(kom1 == 4 || kom1 == -1)
 return;

time(&tmm);
//    printw("\ntmm=%d\n",tmm);

bf=localtime(&tmm);
if(kom1 == 0)
  tmm-=2*60;
if(kom1 == 1)
  tmm-=10*60;
if(kom1 == 2)
  tmm-=60*60;
if(kom1 == 3)
 {
  short    dt,mt,gt;
  poltekdat(&dt,&mt,&gt);
  bf->tm_mday=dt;
  bf->tm_mon=mt-1;
  bf->tm_year=gt-1900;
  bf->tm_hour=(int)0;
  bf->tm_min=0;
  bf->tm_sec=0;
  tmm=0;

  tmm=mktime(bf);
 }
class poiprov_rek poi;

VVOD DANET(1);
DANET.VVOD_SPISOK_add_MD(gettext("Только проводки сделанные вами ?"));

if(danet(&DANET,2,stdscr) == 1)
 {
  sprintf(strsql,"select * from Prov where vrem >= %ld \
  and ktoi=%d order by vrem asc",tmm,iceb_t_getuid());
 }    
else
 sprintf(strsql,"select * from Prov where vrem >= %ld \
  order by vrem asc",tmm);
poi.zapros.new_plus(strsql);      

dirprov(&poi);

}
