/*$Id: upl_plc.c,v 5.9 2012-11-29 09:48:57 sasa Exp $*/
/*30.09.2007	08.09.2005	Белых А.И.	upl_plc.c
Проверка логической целосности бызы для подсистемы учёт путевых листов
*/
#include <errno.h>
#include "buhg.h"


void upl_plc()
{
char strsql[1024];
int kolstr;
SQL_str row,row1;
SQLCURSOR cur,cur1;
char imaf[64];
FILE *ff;

clear();


/*проверяем полномочия оператора*/
sprintf(strsql,"select plb,pln from icebuser where login='%s'",iceb_tu_getlogin());
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



sprintf(imaf,"upl_plc%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }


printw("%s\n",gettext("Проверка логической целосности базы данных"));
printw("%s Upldok1\n",gettext("Проверяем записи в таблице"));

fprintf(ff,"%s\n",gettext("Проверка логической целосности базы данных"));
fprintf(ff,"%s Upldok1\n",gettext("Проверяем записи в таблице"));

sprintf(strsql,"select datd,kp,nomd from Upldok1");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
 }

printw("%s %d\n",gettext("Количество записей в таблице"),kolstr);
fprintf(ff,"%s %d\n",gettext("Количество записей в таблице"),kolstr);

int koloh=0;
int kolstr1=0;

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  sprintf(strsql,"select kp from Upldok where datd='%s' and kp=%s and nomd='%s'",row[0],row[1],row[2]);

  if(readkey(strsql,&row1,&cur1) != 1)
   {
    printw("%s %-3s %s\n",row[0],row[1],row[2]);
    fprintf(ff,"%s %-3s %s\n",row[0],row[1],row[2]);

    koloh++;

    sprintf(strsql,"delete from Upldok1 where datd='%s' and kp=%s and nomd='%s'",row[0],row[1],row[2]);
    sql_zapis(strsql,1,0);
   }
  
 }

printw("\n%s Upldok2a\n",gettext("Проверяем записи в таблице"));

fprintf(ff,"\n%s Upldok2a\n",gettext("Проверяем записи в таблице"));

sprintf(strsql,"select datd,kp,nomd from Upldok2a");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
 }
printw("%s %d\n",gettext("Количество записей в таблице"),kolstr);
fprintf(ff,"%s %d\n",gettext("Количество записей в таблице"),kolstr);

kolstr1=0;
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  sprintf(strsql,"select kp from Upldok where datd='%s' and kp=%s and nomd='%s'",
  row[0],row[1],row[2]);

  if(readkey(strsql,&row1,&cur1) != 1)
   {
    printw("%s %-3s %s\n",row[0],row[1],row[2]);
    fprintf(ff,"%s %-3s %s\n",row[0],row[1],row[2]);
    koloh++;

    sprintf(strsql,"delete from Upldok2a where datd='%s' and kp=%s and nomd='%s'",
    row[0],row[1],row[2]);
    sql_zapis(strsql,1,0);
   }
  
 }

printw("\n%s Upldok3\n",gettext("Проверяем записи в таблице"));

fprintf(ff,"\n%s Upldok3\n",gettext("Проверяем записи в таблице"));

sprintf(strsql,"select datd,kp,nomd from Upldok3");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
 }
printw("%s %d\n",gettext("Количество записей в таблице"),kolstr);
fprintf(ff,"%s %d\n",gettext("Количество записей в таблице"),kolstr);

kolstr1=0;
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  sprintf(strsql,"select kp from Upldok where datd='%s' and kp=%s and nomd='%s'",row[0],row[1],row[2]);

  if(readkey(strsql,&row1,&cur1) != 1)
   {
    printw("%s %-3s %s\n",row[0],row[1],row[2]);
    fprintf(ff,"%s %-3s %s\n",row[0],row[1],row[2]);
    koloh++;

    sprintf(strsql,"delete from Upldok3 where datd='%s' and kp=%s and nomd='%s'",
    row[0],row[1],row[2]);
    sql_zapis(strsql,1,0);
   }
  
 }

fclose(ff);

if(koloh == 0)
 {
  iceb_t_soob(gettext("Порядок, ошибок не обнаружено !"));
  unlink(imaf);
 }
else
 {
  VVOD SOOB(1);
  sprintf(strsql,"%s: %d",gettext("Количество исправленых ошибок"),koloh);

  SOOB.VVOD_SPISOK_add_MD(strsql);
  sprintf(strsql,"%s %s",gettext("Распечатку выгружено в файл"),imaf);
  SOOB.VVOD_SPISOK_add_MD(strsql);
  soobshw(&SOOB,stdscr,-1,-1,0,1);

  class spis_oth oth;
  oth.spis_imaf.plus(imaf);
  oth.spis_naim.plus(gettext("Протокол найденных ошибок"));
  iceb_t_rabfil(&oth,"");
 }




}
