/*$Id: rasamortnu.c,v 5.4 2013/09/26 09:43:38 sasa Exp $*/
/*02.02.2012	02.02.2012	Белых А.И.	rasamortnu.c
Расчёт амортизации для налогового учёта
с 01.04.2011 изменились правила расчёта амортизации
*/
#include "buhg.h"

extern short metkabo; //Если 1 то организация бюджетная

void rasamortnu()
{
char strsql[512];
int kom=0;
short m=0,g=0;
class iceb_tu_str dr("");
VVOD VV(0);
//VVOD DANET(1);

naz:;

if(metkabo == 0) //не бюджетная организация
 {
naz1:;

  helstr(LINES-1,0," F10 ",gettext("выход"),NULL);

  VVOD VVOD1(2);

  dr.new_plus("");
  
  VVOD1.VVOD_SPISOK_add_ZAG(gettext("Расчёт амортизации по организации"));
  VVOD1.VVOD_SPISOK_add_MD(gettext("Введите дату расчёта (м.г)"));

  if((kom=vvod1(&dr,8,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
    return;
  if(kom == ESC)
    return;

  if(rsdat1(&m,&g,dr.ravno()) != 0)
   {
    iceb_t_soob(gettext("Не верно введена дата !"));
    goto naz1;
   }
 } 

if(metkabo == 1) //бюджетная организация
 {
naz2:;

  helstr(LINES-1,0," F10 ",gettext("выход"),NULL);

  VVOD VVOD1(2);

  dr.new_plus("");
  
  VVOD1.VVOD_SPISOK_add_ZAG(gettext("Расчёт амортизации по организации"));
  VVOD1.VVOD_SPISOK_add_MD("Введите год расчёта");

  if((kom=vvod1(&dr,8,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
    return;
  if(kom == ESC)
    return;
  g=dr.ravno_atoi();
  if(g < 2000)
   {
    iceb_t_soob(gettext("Не верно введена дата !"));
    goto naz2;
   }
 } 

if(iceb_t_pbpds(m,g) != 0)
  goto naz;

time_t tmm=time(NULL);

class SQLCURSOR cur;
SQL_str row;
int kolstr=0;
sprintf(strsql,"select innom from Uosin order by innom asc");
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
 
int kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  amort(atoi(row[0]),m,g,0,0,"",NULL);
 }

printw_vr(tmm);


OSTANOV();


}
