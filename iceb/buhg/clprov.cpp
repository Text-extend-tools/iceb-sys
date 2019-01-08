/* $Id: clprov.c,v 5.17 2012-11-29 09:48:57 sasa Exp $ */
/*27.09.2014    10.12.1992      Белых А.И.      clprov.c
Подпрограмма удаления записей из базы
*/
#include        "buhg.h"


void            clprov()
{
class iceb_tu_str vstr("");
short           m,g;
int             kom;
char		strsql[1024];
int		kom1;
int		kolud=0;
int		koluds=0;
int		metkazap=0;
short	dt,mt,gt;
SQL_str		row;
SQLCURSOR       cur;





/*проверяем полномочия оператора*/
sprintf(strsql,"select gkb,gkn from icebuser where login='%s'",iceb_tu_getlogin());
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






poltekdat(&dt,&mt,&gt);


clear();
VVOD MENU(3);
MENU.VVOD_SPISOK_add_MD(gettext("Удаление проводок за месяц"));
MENU.VVOD_SPISOK_add_MD(gettext("Удаление проводок за год"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
kom=0; 
while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
 
if(kom == -1 || kom == 2)
 return;

clear();
helstr(LINES-1,0,"F10",gettext("выход"),NULL);
move(5,0);
if(kom == 0)
 {
  VVOD  VVOD1(2);

  
  VVOD1.VVOD_SPISOK_add_ZAG(gettext("Удаление всех проводок по всем счетам за месяц"));
  VVOD1.VVOD_SPISOK_add_MD(gettext("Введите дату (м.г)"));

  if((kom1=vvod1(&vstr,8,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
    return;
  if(kom1 == ESC)
    return;
 }

if(kom == 1)
 {
  VVOD  VVOD1(2);
  
  
  VVOD1.VVOD_SPISOK_add_ZAG(gettext("Удаление всех проводок по всем счетам за год"));
  VVOD1.VVOD_SPISOK_add_MD(gettext("Введите год"));

  if((kom1=vvod1(&vstr,5,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
    return;
  if(kom1 == ESC)
    return;
 }


m=g=0;
if(vstr.getdlinna() <= 1)
 {
  return;
 }
else
 {
  if(kom == 0)
   {
    polen(vstr.ravno(),&m,1,'.');
    polen(vstr.ravno(),&g,2,'.');
   }
  else
   {
    m=0;
    g=(short)ATOFM(vstr.ravno());
   }
 }
clearstr(LINES-1,0);

VVOD DANET(1);
if(kom == 0)
 {
  sprintf(strsql,gettext("Удалить проводки за %d.%dг.? Вы уверены ?"),m,g);
  DANET.VVOD_SPISOK_add_MD(strsql);
 }
if(kom == 1)
 {
  sprintf(strsql,gettext("Удалить проводки за %dг.? Вы уверены ?"),g);
  DANET.VVOD_SPISOK_add_MD(strsql);
 }
if(danet(&DANET,2,stdscr) == 2)
  return;

VVOD PAR(2);
PAR.VVOD_SPISOK_add_MD(gettext("Введите пароль"));

if(parolv(&PAR,dt,mt,gt,0) != 0)
  return;

clear();

GDITE();
strcpy(strsql,"select VERSION()");
sql_readkey(&bd,strsql,&row,&cur);
if(SRAV(row[0],"3.22",1) == 0)
  metkazap=1;  
printw("VERSION:%s\n",row[0]);
refresh();
if(kom == 0)
  printw(gettext("Удаляем проводки за %d.%dг.\n"),m,g);
if(kom == 1)
  printw(gettext("Удаляем проводки за %dг.\n"),g);
refresh();

memset(strsql,'\0',sizeof(strsql));

if(g != 0)
 sprintf(strsql,"delete from Prov where datp >= '%04d-01-01' and \
datp <= '%04d-12-31'",g,g);
if(g == 0)
 sprintf(strsql,"delete from Prov where datp = '0000-00-00'");

if(kom == 0)
 {
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"delete from Prov where datp >= '%04d-%02d-01' and \
datp <= '%04d-%02d-31'",g,m,g,m);
 }

sql_zapis(strsql,1,0);

kolud+=sql_rows(&bd);
printw("%s: %d\n",gettext("Количество удаленных записей"),kolud);

if(kolud > 0)
 {
  printw(gettext("Оптимизируем таблицу.\n"));
  refresh();
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table Prov");
  if(metkazap == 1)
   {
    if(sql_zap(&bd,strsql) != 0)
     msql_error(&bd,gettext("Ошибка оптимизации таблицы !"),strsql);
   }
  else
    sql_readkey(&bd,strsql,&row,&cur);
 }

if(kom == 0)
 {
  sprintf(strsql,"%s %d",gettext("Количество удаленных записей"),kolud);
  iceb_t_soob(strsql);
  return;
 } 
printw(gettext("Удаляем стартовые сальдо.\n"));
refresh();
sprintf(strsql,"delete from Saldo where gs=%d",g);

sql_zapis(strsql,1,0);

kolud+=koluds=sql_rows(&bd);
printw("%s: %d\n",gettext("Количество удаленных записей"),koluds);
if(kom > 0)
 {
  printw(gettext("Оптимизируем таблицу.\n"));
  refresh();
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table Saldo");
  if(metkazap == 1)
   {
    if(sql_zap(&bd,strsql) != 0)
     msql_error(&bd,gettext("Ошибка оптимизации таблицы !"),strsql);
   }
  else
    sql_readkey(&bd,strsql,&row,&cur);
 }
sprintf(strsql,"%s %d",gettext("Количество удаленных записей"),kolud);
iceb_t_soob(strsql);

}
