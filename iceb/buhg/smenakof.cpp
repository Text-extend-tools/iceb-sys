/* $Id: smenakof.c,v 5.15 2013/09/26 09:43:40 sasa Exp $ */
/*08.06.2013	30.06.2000	Белых А.И.	smenakof.c
Смена поправочного коєффициента
*/
#include        "buhg.h"
#include "poiinpd.h"

void		smenakof(int metkauh) //0-налоговый учет 1-бухгалтеский учет
{
long		kolstr,kolstr1;
SQL_str         row;
char		strsql[512];
int		kom;
short		mn,gn;
class iceb_tu_str grupa("");
long		in;
float		popkfu;
time_t		vrem;

VVOD VV(0);

if(metkauh == 0)
 VV.VVOD_SPISOK_add_ZAG(gettext("Смена поправочных коэффициентов в карточках для налогового учёта."));
if(metkauh == 1)
 VV.VVOD_SPISOK_add_ZAG(gettext("Смена поправочных коэффициентов в карточках для бухгалтерского учёта."));

VV.VVOD_SPISOK_add_MD(gettext("Дата установки коэффициента.(м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Группа............................."));
if(metkauh == 0)
 VV.VVOD_SPISOK_add_MD(gettext("Коэффициент налогового учёта......."));
if(metkauh == 1)
 VV.VVOD_SPISOK_add_MD(gettext("Коэффициент бухгалтерского учёта..."));

VV.VVOD_SPISOK_add_data(8);
VV.VVOD_SPISOK_add_data(10);
VV.VVOD_SPISOK_add_data(10);


naz:;

clear(); 

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("uos5_9.txt");
   goto naz;

  case FK10:
  case ESC:
   return;

  case FK2:
  case PLU:
   break;

  default:
    goto naz;

 }

if(rsdat1(&mn,&gn,VV.VVOD_SPISOK_return_data(0)) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  goto naz;
 }
if(VV.VVOD_SPISOK_return_data(1)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введена группа !"));
  goto naz;
 }
grupa.new_plus(VV.VVOD_SPISOK_return_data(1));

popkfu=ATOFM(VV.VVOD_SPISOK_return_data(2));
if(popkfu == 0. )
 {
  iceb_t_soob(gettext("Поправочный коэффициент не может равняться нолю !"));
  goto naz;

 }

GDITE();

sprintf(strsql,"select innom,naim from Uosin");
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
kolstr1=0;
time(&vrem);
class poiinpdw_data rekin;
while(cur.read_cursor(&row) != 0)
 {
  kolstr1++;
  strzag(LINES-1,0,kolstr,kolstr1);
  in=atol(row[0]);
  if(poiinpd(in,mn,gn,&rekin) != 0)
    continue;  

  if(metkauh == 0)
  if(SRAV(grupa.ravno(),rekin.hna.ravno(),0) != 0)
    continue;

  if(metkauh == 1)
  if(SRAV(grupa.ravno(),rekin.hnaby.ravno(),0) != 0)
    continue;

  printw("%ld %s\n",row[0],row[1]);    

  sprintf(strsql,"replace into Uosinp \
values (%ld,%d,%d,'%s','%s','%s','%s',%.6g,%d,'%s',%d,%ld,'%s',%.10g)",
  in,mn,gn,rekin.shetu.ravno(),rekin.hzt.ravno(),rekin.hau.ravno(),rekin.hna.ravno(),rekin.popkf,rekin.soso,rekin.nomz.ravno(),iceb_t_getuid(),vrem,rekin.hnaby.ravno(),
  rekin.popkfby);
 
  sql_zapis(strsql,1,0);
      
 }
OSTANOV();
}
