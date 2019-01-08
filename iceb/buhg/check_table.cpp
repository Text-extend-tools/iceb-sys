/*$Id: check_table.c,v 5.1 2012-11-29 09:48:57 sasa Exp $*/
/*13.07.2015	06.10.2012	Белых А.И.	check_table.c
Проверка физической целосности таблиц
*/
#include <errno.h>
#include "buhg.h"

int check_table(class spis_oth *oth,const char *namebase)
{
char		strsql[512];
SQL_str		row,row1;
SQLCURSOR       cur,cur1;
FILE *ff_prot;
char imaf_prot[64];

clear();

printw("%s %s\n",gettext("Проверка физической целосности таблиц базы данных"),namebase);


VVOD DANET(1);

DANET.VVOD_SPISOK_add_MD(gettext("Проверить физическую целосноть таблиц ? Вы уверены ?"));
if(danet(&DANET,2,stdscr) == 2)
  return(1);

clear();

strcpy(strsql,"select VERSION()");
sql_readkey(&bd,strsql,&row,&cur);

printw("VERSION:%s\n",row[0]);

sprintf(strsql,"SHOW TABLES FROM %s",namebase);
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }
sprintf(imaf_prot,"check%d.lst",getpid());

if((ff_prot = fopen(imaf_prot,"w")) == NULL)
 {
  error_op_nfil(imaf_prot,errno,"");
  return(1);
 }

iceb_t_zagolov(gettext("Проверка физической целосности таблиц базы данных"),ff_prot);
fprintf(ff_prot,"\n");
int kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"CHECK TABLE %s",row[0]);

  if(cur1.make_cursor(&bd,strsql) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    continue;
   }

  while(cur1.read_cursor(&row1) != 0)
   {
    printw("%-30s %s %s %s\n",row1[0],row1[1],row1[2],row1[3]);
    refresh();
    fprintf(ff_prot,"%-20s %s %s %s\n",row1[0],row1[1],row1[2],row1[3]);
 
   }
 }

printw("\n%s:%d\n",gettext("Количество таблиц"),kolstr);
fprintf(ff_prot,"\n%s:%d\n",gettext("Количество таблиц"),kolstr);

podpis(ff_prot);
fclose(ff_prot);

OSTANOV();

oth->spis_imaf.plus(imaf_prot);
oth->spis_naim.plus(gettext("Протокол проверки"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
  iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);


return(0);
}
