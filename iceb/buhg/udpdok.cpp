/* $Id: udpdok.c,v 5.17 2013/09/26 09:43:41 sasa Exp $ */
/*15.11.2012	10.01.1998	Белых А.И.	udpdok.c
Удаление платежных документов за год.
*/
#include        "buhg.h"


void		udpdok()
{
class iceb_tu_str bros("");
short		kom;
short		god;
char		strsql[512];
SQL_str		row;
SQLCURSOR cur;




clear();


/*проверяем полномочия оператора*/
sprintf(strsql,"select pdb,pdn from icebuser where login='%s'",iceb_tu_getlogin());
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


VVOD VVOD1(2);

helstr(LINES-1,0,"F10",gettext("выход"),NULL);

VVOD1.VVOD_SPISOK_add_ZAG(gettext("Удаление платёжных документов за год"));
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите год"));
     
if((kom=vvod1(&bros,5,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
   return;
if(kom == ESC)
   return;

god=bros.ravno_atoi();

clearstr(LINES-1,0);

VVOD DANET(1);

sprintf(strsql,gettext("Удалить документы за %d год ? Вы уверены ?"),god);
DANET.VVOD_SPISOK_add_MD(strsql);

if(danet(&DANET,2,stdscr) == 2)
  return;

strsql[0]='\0';
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
VVOD1.VVOD_delete();
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите пароль."));
vparol(&VVOD1,strsql,20,stdscr,-1,-1,1);
if((int)ATOFM(strsql) != gt+mt+dt)
 {
  iceb_t_soob(gettext("Не верно введён пароль !"));
  return;
 }


clear();

printw(gettext("Удаление платёжных документов за %d год\n"),god);

GDITE();

int		metkazap=0;


strcpy(strsql,"select VERSION()");

sql_readkey(&bd,strsql,&row,&cur);

if(SRAV(row[0],"3.22",1) == 0)
  metkazap=1;  

printw("VERSION:%s\n",row[0]);
refresh();

printw("%s\n",gettext("Удаление платёжных поручений"));
refresh();

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"delete from Pltp where datd >= '%d-01-01' and \
datd <= '%d-12-31'",god,god);

sql_zapis(strsql,0,0);

printw(gettext("Оптимизируем таблицу\n"));
refresh();

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table Pltp");

if(metkazap == 1)
  sql_zapis(strsql,0,0);
else
  sql_readkey(&bd,strsql);

printw("%s\n",gettext("Удаление комментариев к платёжным поручениям"));
refresh();

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"delete from Pltpz where datd >= '%d-01-01' and \
datd <= '%d-12-31'",god,god);

sql_zapis(strsql,0,0);

printw("%s\n",gettext("Оптимизируем таблицу"));
refresh();

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table Pltpz");

if(metkazap == 1)
  sql_zapis(strsql,0,0);
else
  sql_readkey(&bd,strsql);

printw("%s\n",gettext("Удаление платёжных требований"));
refresh();

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"delete from Pltt where datd >= '%d-01-01' and \
datd <= '%d-12-31'",god,god);

sql_zapis(strsql,0,0);

printw("%s\n",gettext("Оптимизируем таблицу"));
refresh();
memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table Pltt");

if(metkazap == 1)
  sql_zapis(strsql,0,0);
else
  sql_readkey(&bd,strsql);

printw("%s\n",gettext("Удаление комментариев к платёжным требованиям"));
refresh();

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"delete from Plttz where datd >= '%d-01-01' and \
datd <= '%d-12-31'",god,god);

sql_zapis(strsql,0,0);

printw("%s\n",gettext("Оптимизируем таблицу"));
refresh();
memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table Plttz");

if(metkazap == 1)
  sql_zapis(strsql,0,0);
else
  sql_readkey(&bd,strsql);

printw("%s\n",gettext("Удаление подтверждающих записей к платёжкам/требованиям"));
refresh();

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"delete from Pzpd where datd >= '%d-01-01' and \
datd <= '%d-12-31'",god,god);

sql_zapis(strsql,0,0);

printw("%s\n",gettext("Оптимизируем таблицу"));
refresh();
memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table Pzpd");

if(metkazap == 1)
  sql_zapis(strsql,0,0);
else
  sql_readkey(&bd,strsql);

printw("%s\n",gettext("Удаление объявок"));
refresh();

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"delete from Obqvka where datd >= '%d-01-01' and \
datd <= '%d-12-31'",god,god);

sql_zapis(strsql,0,0);

printw("%s\n",gettext("Оптимизируем таблицу"));
refresh();
memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table Obqvka");

if(metkazap == 1)
 sql_zapis(strsql,0,0);
else
  sql_readkey(&bd,strsql);


printw("%s\n",gettext("Удаление завершено"));

OSTANOV();
}
