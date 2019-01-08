/* $Id: saldozr.c,v 5.20 2013/09/26 09:43:40 sasa Exp $ */
/*15.07.2015    26.10.1999      Белых А.И.      saldozr.c
Программа переноса сальдо для "Расчёта зарплаты"
*/
#include <math.h>
#include        "buhg.h"

extern char		*shetb; /*Бюджетные счета начислений*/
extern char		*shetbu; /*Бюджетные счета удержаний*/
extern double          okrg; /*Округление*/
extern short startgodz; /*Стаpтовый год для расчёта сальдо*/

void	saldozr()
{
short		godn=0; /*Стаpтовый год для расчёта сальдо*/
short		godk=0;
SQL_str         row;
long		kolstr,kolstr1;
char		strsql[512];
double		sald,saldi;
time_t		vrem;
long		ttt;
double		saldb;
int		kom;

if(iceb_t_prn() != 0) /*Проверка возможности изменять настройки*/
 return;

clear();

if(iceb_t_poldan("Стартовый год",strsql,"zarnast.alx") == 0)
 {
  godn=(short)atoi(strsql);
 }

if(godn == 0)
 {
  iceb_t_soob(gettext("Не введён стартовый год!"));
  return;
 }

VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(gettext("Заработная плата"));
VV.VVOD_SPISOK_add_ZAG(gettext("Перенос сальдо по работникам"));
sprintf(strsql,"%s:%d",gettext("Стартовый год"),godn);
VV.VVOD_SPISOK_add_ZAG(strsql);

VV.VVOD_SPISOK_add_MD(gettext("С какого года.."));
VV.VVOD_SPISOK_add_MD(gettext("На какой год..."));
VV.VVOD_SPISOK_add_data(godn,5);
VV.VVOD_SPISOK_add_data(godn+1,5);

naz:;
clear();


helstr(LINES-1,0,"F2/+",gettext("расчёт"),
"F5",gettext("блокировка"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

switch(kom)
 {
  case FK2:
  case PLU:
    break;

  case FK5:
    iceb_tl_blok();
    goto naz;

  case FK10:
    return;
  default:
   goto naz;
 }

godn = (short)VV.data_ravno_atoi(0);
godk = (short)VV.data_ravno_atoi(1);
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

if(godk < gt-100 || godk > gt)
 {
  iceb_t_soob(gettext("Не правильно введён год !"));
  return;
 }  

if(iceb_t_pbpds(1,godk) != 0)
 goto naz;

clear();


VVOD DANET(1);

DANET.VVOD_SPISOK_add_ZAG(gettext("Перенос сальдо по работникам для расчёта зарплаты"));
sprintf(strsql,"%s:%d",gettext("Стартовый год"),godn);
DANET.VVOD_SPISOK_add_MD(strsql);

//sprintf(strsql,"Перенести сальдо с %dг. на %dг. ? Вы уверены ?",godn,godk);
sprintf(strsql,"%s %d %s %s %d %s ? %s",
gettext("Перенести сальдо с"),
godn,
gettext("г."),
gettext("на"),
godk,
gettext("г."),
gettext("Вы уверены ?"));
DANET.VVOD_SPISOK_add_MD(strsql);
if(danet(&DANET,2,stdscr) == 2)
  return;


clear();
printw("%s %d\n",gettext("Чистим"),godk);
GDITE();

/*Чистим год куда будут записаны сальдо*/
sprintf(strsql,"delete from Zsal where god=%d",godk);
sql_zapis(strsql,0,0);


sprintf(strsql,"select tabn,fio from Kartb");
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
saldi=0.;
time(&vrem);
while(cur.read_cursor(&row) != 0)
 {
  kolstr1++;
  strzag(LINES-1,0,kolstr,kolstr1);
  
  ttt=atol(row[0]);
  sald=zarsald(1,1,godk,ttt,&saldb);

  if(fabs(sald) < 0.009)
     continue;

  printw("%6s %-*.*s %8.2f\n",
  row[0],
  iceb_tu_kolbait(30,row[1]),
  iceb_tu_kolbait(30,row[1]),
  row[1],
  sald);

  saldi+=sald;

  sprintf(strsql,"insert into Zsal values (%d,%ld,%.2f,%d,%ld,%.2f)",
  godk,ttt,sald,iceb_t_getuid(),vrem,saldb);

  sql_zapis(strsql,0,0);

 }
printw("%s:%.2f\n",gettext("Итого"),saldi);
printw("%s\n",gettext("Сальдо перенесены"));
OSTANOV();

if(godk <= gt)
 {
  sprintf(strsql,"update Alx set str='Стартовый год|%d\n' where fil='zarnast.alx' and str like 'Стартовый год|%d%%'",
  godk,godn);

  if(sql_zapis(strsql,0,0) == 0)
    startgodz=godk;
 }

}
