/*$Id: ukrudgod.c,v 5.3 2013/09/26 09:43:41 sasa Exp $*/
/*15.11.2012	15.11.2012	Белых А.И.	ukrudgod.c
Удаление документов за год
*/

#include "buhg.h"

void ukrudgod()
{
int		kom1=0;
short		god;
char		strsql[512];
class iceb_tu_str vstr("");
SQLCURSOR cur;
SQL_str		row;
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
clear();

/*проверяем полномочия оператора*/
sprintf(strsql,"select krb,krn from icebuser where login='%s'",iceb_tu_getlogin());
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %s %s icebuser",gettext("Не найден логин"),iceb_tu_getlogin(),gettext("в таблице"));
  iceb_t_soob(strsql);
  return;
 }
if(atoi(row[0]) != 0 || atoi(row[1]) != 0)
 {
  iceb_t_soob(gettext("У вас нет полномочий для этой операции!"));
  return;
 }

VVOD SOOBN(1);
SOOBN.VVOD_SPISOK_add_MD(gettext("       	         В н и м а н и е !!!"));
SOOBN.VVOD_SPISOK_add_MD(gettext("                 Режим удаления документов за год."));
SOOBN.VVOD_SPISOK_add_MD(gettext("Удаление документов за прошедшие годы выполняется с целью уменьшения"));
SOOBN.VVOD_SPISOK_add_MD(gettext("                       объёма базы данных."));
soobsh(&SOOBN,0,-1,1);

helstr(LINES-1,0,"F10",gettext("выход"),NULL);

memset(strsql,'\0',sizeof(strsql));

VVOD VVOD1(2);

VVOD1.VVOD_SPISOK_add_MD(gettext("Введите год"));

if((kom1=vvod1(&vstr,5,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
  return;
if(kom1 == ESC)
  return;


god=vstr.ravno_atoi();
if(god < 1900 || god > 3000)
 {
  iceb_t_soob(gettext("Не верно введён год !"));
  return;
 }

VVOD PAR(2);
PAR.VVOD_SPISOK_add_MD(gettext("Введите пароль"));
if(parolv(&PAR,dt,mt,gt,0) != 0)
  return;

clear();
GDITE();

int metkazap=0;
strcpy(strsql,"select VERSION()");
sql_readkey(&bd,strsql,&row,&cur);
if(SRAV(row[0],"3.22",1) == 0)
  metkazap=1;  

printw("VERSION:%s\n",row[0]);
printw("%s %d\n",gettext("Удаляем документы за год"),god);
refresh();

printw("%s Ukrdok1\n",gettext("Удаляем записи в таблице"));
refresh();


sprintf(strsql,"delete from Ukrdok1 where datd >= '%04d-01-01' and datd <= '%04d-12-31'",god,god);

if(sql_zapis(strsql,1,0) != 0)
 return;


printw("%s Ukrdok\n",gettext("Удаляем записи в таблице"));
refresh();

sprintf(strsql,"delete from Ukrdok where god=%d\n",god);
sql_zapis(strsql,1,0);

printw("%s Ukrdok1\n",gettext("Оптимизация таблицы"));
refresh();

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table Ukrdok1");
if(metkazap == 1)
 {
  sql_zapis(strsql,1,0);
 }
else
  readkey(strsql);


printw("%s Ukrdok\n",gettext("Оптимизация таблицы"));
refresh();

sprintf(strsql,"optimize table Ukrdok");
if(metkazap == 1)
 {
  sql_zapis(strsql,1,0);
 }
else
  readkey(strsql);

OSTANOV();

}
