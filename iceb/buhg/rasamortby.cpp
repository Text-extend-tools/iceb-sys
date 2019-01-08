/* $Id: rasamortby.c,v 5.20 2013/09/26 09:43:38 sasa Exp $ */
/*15.07.2015    06.12.2000      Белых А.И.      rasamortby.c
Расчёт амортизаци по организации для бухгалтерского учёта
*/
#include        "buhg.h"
#include "poiinpd.h"


void    rasamortby()
{
time_t          tmm;
class iceb_tu_str dr("");
short           m,g;
long		kolstr,kolstr1;
SQL_str         row;
char		strsql[512];
long		in;
int		kom;

clear();
m=g=0;
VVOD VVOD1(2);

VVOD1.VVOD_SPISOK_add_ZAG(gettext("Расчёт амортизации по организации для бухгалтерского учёта"));
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите дату расчёта (м.г)"));

naz:;

helstr(LINES-1,0," F10 ",gettext("выход"),NULL);


if((kom=vvod1(&dr,8,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
  return;
if(kom == ESC)
  return;

if(rsdat1(&m,&g,dr.ravno()) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz;
 }

if(iceb_t_pbpds(m,g) != 0)
  goto naz;

clear();
printw("%s %d.%d%s\n",gettext("Расчёт за"),m,g,
gettext("г."));

refresh();

time(&tmm);

sprintf(strsql,"select innom from Uosin order by innom asc");
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;
  
 }
 
unlink("amort.lst");
kolstr1=0;
class poiinpdw_data rekin;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  in=atol(row[0]);
  poiinpd(in,m,g,&rekin);
  
  amortby(in,m,g,0,rekin.hnaby.ravno(),NULL);
 }

printw_vr(tmm);


OSTANOV();
}
