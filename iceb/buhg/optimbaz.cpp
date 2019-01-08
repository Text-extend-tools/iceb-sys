/* $Id: optimbaz.c,v 5.16 2013-01-02 10:38:36 sasa Exp $ */
/*11.12.2012	24.03.1999	Белих А.И.	optimbaz.c
Оптимизация базы данных
*/
#include	"buhg.h"

extern char *imabaz;

void		optimbaz()
{
char		strsql[512];
SQL_str		row,row1;
SQLCURSOR       cur,cur1;
int		metkazap=0;

clear();

printw(gettext("Оптимизация базы данных делается после удаления большого\nколичества записей из базы данных. Выполнение оптимизации после удаления\nзаписей позволяет уменьшить размер базы.\n"));


VVOD DANET(1);

DANET.VVOD_SPISOK_add_MD(gettext("Оптимизировать ? Вы уверены ?"));
if(danet(&DANET,2,stdscr) == 2)
  return;
clear();
strcpy(strsql,"select VERSION()");
sql_readkey(&bd,strsql,&row,&cur);
if(SRAV(row[0],"3.22",1) == 0)
  metkazap=1;  
printw("VERSION:%s\n",row[0]);

sprintf(strsql,"SHOW TABLES FROM %s",imabaz);
int kolstr=0;
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

  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table %s",row[0]);
  if(metkazap == 1)
   {
    printw(gettext("Оптимизация таблицы %s !\n"),row[0]);
    refresh();
    if(sql_zap(&bd,strsql) < 0)
     printw("%s\n",mysql_error(&bd));
   }
  else
   {
    if(cur1.make_cursor(&bd,strsql) < 0)
     {
      msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
      continue;
     }

    while(cur1.read_cursor(&row1) != 0)
     {
      printw("%-20s %s %s %s\n",row1[0],row1[1],row1[2],row1[3]);
      refresh();
   
     }
   }

 }
printw("\n%s:%d\n",gettext("Количество таблиц"),kolstr);

printw(gettext("\nОптимизация базы закончена\n"));
OSTANOV();
}
