/* $Id: clearzar.c,v 5.23 2013/05/17 14:55:56 sasa Exp $ */
/*22.03.2016	07.06.1999	Белых А.И.	clearzar.c
Удаление из зарплаты всех данных за месяц и за год
*/
#include	"buhg.h"

extern short	ddd,mmm,ggg;

void		clearzar()
{
int		kom=0,kom1=0;
short		du,mu,gu;
char		strsql[512];
int		kolud;
int		metkazap=0;
class iceb_tu_str vstr("");
SQL_str		row;
SQLCURSOR cur;
clear(); /*Очистить экран и закрасить фоновым цветом*/



/*проверяем полномочия оператора*/
sprintf(strsql,"select zpb,zpn from icebuser where login='%s'",iceb_tu_getlogin());
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



VVOD  VVOD1(2);
VVOD MENU(3);
MENU.VVOD_SPISOK_add_MD(gettext("Удалить начисления и удержания за месяц"));
MENU.VVOD_SPISOK_add_MD(gettext("Удалить начисления и удержания за год"));
MENU.VVOD_SPISOK_add_MD(gettext("Удалить записи с нулевым значением"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

switch (kom)
 {
  case 3 :
  case -1 :
    return;

  case 0:
    VVOD1.VVOD_delete();  
    helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите месяц и год (м.г)"));
    
    
    if((kom1=vvod1(&vstr,8,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
      return;
    if(kom1 == ESC)
      return;
    if(rsdat1(&mu,&gu,vstr.ravno()) != 0)
     {
      iceb_t_soob(gettext("Не верно введена дата !"));
      return;
     }       
    break; 

  case 1:
    VVOD1.VVOD_delete();  
    helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите год"));
    
    vstr.new_plus("");    
    if((kom1=vvod1(&vstr,5,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
      return;
    if(kom1 == ESC)
      return;

    mu=0;
    gu=vstr.ravno_atoi();
    break; 

  case 2:
    du=1;
    mu=mmm;
    gu=ggg;
    dpm(&du,&mu,&gu,4);

    VVOD1.VVOD_delete();  
    helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите дату до которой нужно удалить"));
    
    sprintf(strsql,"%d.%d",mu,gu);
    vstr.new_plus(strsql);
    if((kom1=vvod1(&vstr,8,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
      return;
    if(kom1 == ESC)
      return;
    if(rsdat1(&mu,&gu,vstr.ravno()) != 0)
     {
      iceb_t_soob(gettext("Не верно введена дата !"));
      return;
     }

    break;
 }

clearstr(LINES-1,0);

VVOD DANET(1);

if(kom == 0)
 {
  if(iceb_t_pbpds(mu,gu) != 0)
   return;


  sprintf(strsql,gettext("Удалить записи за %d.%dг. ? Вы уверены ?"),mu,gu);
  DANET.VVOD_SPISOK_add_MD(strsql);
 }

if(kom == 1)
 {
  for(int ii=0; ii < 12; ii++)
   if(iceb_t_pbpds(ii+1,gu) != 0)
    return;
  sprintf(strsql,gettext("Удалить записи %d год ? Вы уверены ?"),gu);
  DANET.VVOD_SPISOK_add_MD(strsql);
 }
if(kom == 2)
 {
  sprintf(strsql,gettext("Удалить записи с нулевым значением ? Вы уверены ?"));
  DANET.VVOD_SPISOK_add_MD(strsql);
 }

if(danet(&DANET,2,stdscr) == 2)
  return;

if(kom == 0 || kom == 1)
 {
  VVOD PAR(2);
  PAR.VVOD_SPISOK_add_MD(gettext("Введите пароль"));
  if(parolv(&PAR,ddd,mmm,ggg,0) != 0)
    return;
 }
 
clear();
GDITE();
strcpy(strsql,"select VERSION()");
sql_readkey(&bd,strsql,&row,&cur);
if(SRAV(row[0],"3.22",1) == 0)
  metkazap=1;  
printw("VERSION:%s\n",row[0]);
refresh();


if(kom == 0)
  printw("%s %d%s\n",gettext("Удаление записей за"),
  gu,gettext("г."));

if(kom == 1)
  printw("%s %d.%d%s\n",gettext("Удаление записей за"),
  mu,gu,gettext("г."));

if(kom == 2)
  printw("%s\n",gettext("Удаление записей с нулевым значением"));

printw("%s\n",gettext("Удаляем записи начислений и удержаний"));
refresh();
if(kom == 0)
 sprintf(strsql,"delete from Zarp where datz >= '%04d-%02d-01' and \
datz <= '%04d-%02d-31'",gu,mu,gu,mu);
if(kom == 1)
 sprintf(strsql,"delete from Zarp where datz >= '%04d-01-01' and \
datz <= '%04d-12-31'",gu,gu);

if(kom == 2)
 sprintf(strsql,"delete from Zarp where datz < '%04d-%d-01' and suma=0.",gu,mu);

sql_zapis(strsql,1,0);

kolud=sql_rows(&bd);
printw("%s: %d\n",gettext("Количество удаленных записей"),kolud);

if(kolud > 0)
 { 
  printw("%s\n",gettext("Оптимизируем таблицу"));
  refresh();
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table Zarp");
  if(metkazap == 1)
   {
    sql_zapis(strsql,1,0);
   }
  else
    readkey(strsql);
 }

if(kom == 2)
 {
  sprintf(strsql,"%s: %d",gettext("Количество удаленных записей"),kolud);
  iceb_t_soob(strsql);
  return;
 } 

printw("%s\n",gettext("Удаляем записи в вспомогательной таблице"));
refresh();

if(kom == 0)
 sprintf(strsql,"delete from Zarn where god = %d and mes=%d",gu,mu);
if(kom == 1)
 sprintf(strsql,"delete from Zarn where god=%d",gu);

sql_zapis(strsql,1,0);

kolud=sql_rows(&bd);
printw("%s: %d\n",gettext("Количество удаленных записей"),kolud);
if(kolud > 0)
 { 
  printw("%s\n",gettext("Оптимизируем таблицу"));
  refresh();
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table Zarn");
  if(metkazap == 1)
   {
    sql_zapis(strsql,1,0);
   }
  else
    readkey(strsql);
 }

printw(gettext("Удаляем записи начислений на фонд оплаты труда\n"));
refresh();

if(kom == 0)
 sprintf(strsql,"delete from Zarsocz where datz >= '%d-%d-01' and datz <= '%d-%d-31'",
 gu,mu,gu,mu);

if(kom == 1)
 sprintf(strsql,"delete from Zarsocz where datz >= '%d-01-01' and datz <= '%d-12-31'",
 gu,gu);

sql_zapis(strsql,1,0);

kolud=sql_rows(&bd);
printw("%s: %d\n",gettext("Количество удаленных записей"),kolud);
if(kolud > 0)
 { 
  printw("%s\n",gettext("Оптимизируем таблицу"));
  refresh();
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table Zarsocz");
  if(metkazap == 1)
   {
    sql_zapis(strsql,1,0);
   }
  else
    readkey(strsql);
 }

printw("%s\n",gettext("Удаляем записи видов табеля"));
refresh();

if(kom == 0)
 sprintf(strsql,"delete from Ztab where god=%d and mes=%d",gu,mu);
if(kom == 1)
 sprintf(strsql,"delete from Ztab where god=%d",gu);

sql_zapis(strsql,1,0);

kolud=sql_rows(&bd);
printw("%s: %d\n",gettext("Количество удаленных записей"),kolud);
if(kolud > 0)
 { 
  printw("%s\n",gettext("Оптимизируем таблицу"));
  refresh();
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table Ztab");
  if(metkazap == 1)
   {
    sql_zapis(strsql,1,0);
   }
  else
    readkey(strsql);
 }


printw("%s\n",gettext("Удаляем записи протоколов хода расчёта"));
refresh();

if(kom == 0)
 sprintf(strsql,"delete from Zarpr where dr='%04d-%02d-01'",gu,mu);
if(kom == 1)
 sprintf(strsql,"delete from Zarpr where dr >= '%04d-01-01' and dr <= '%04d-12-01'",gu,gu);

sql_zapis(strsql,1,0);

kolud=sql_rows(&bd);
printw("%s: %d\n",gettext("Количество удаленных записей"),kolud);
if(kolud > 0)
 { 
  printw("%s\n",gettext("Оптимизируем таблицу"));
  refresh();
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table Zarpr");
  if(metkazap == 1)
   {
    sql_zapis(strsql,1,0);
   }
  else
    readkey(strsql);
 }



if(kom == 1)
 {
  printw("%s\n",gettext("Удаляем записи сальдо"));
  refresh();
  sprintf(strsql,"delete from Zsal where god=%d",
  gu);

  sql_zapis(strsql,1,0);

  kolud=sql_rows(&bd);
  printw("%s: %d\n",gettext("Количество удаленных записей"),kolud);
  if(kolud > 0)
   { 
    printw("s\n",gettext("Оптимизируем таблицу"));
    refresh();
    memset(strsql,'\0',sizeof(strsql));
    sprintf(strsql,"optimize table Zsal");
    if(metkazap == 1)
     {
      sql_zapis(strsql,1,0);
     }
    else
      readkey(strsql);
   }
 }
}
