/*$Id: uddusl.c,v 1.19 2012-11-29 09:48:57 sasa Exp $*/
/*16.02.2016	13.02.2002	Белых А.И.	uddusl.c
Удаление документов в учете услуг за год
*/
#include	"buhg.h"


void		uddusl()
{
int	kom1=0;
class iceb_tu_str god("");
char	strsql[512];
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
SQL_str row;
SQLCURSOR cur;

/*проверяем полномочия оператора*/
sprintf(strsql,"select uub,uun from icebuser where login='%s'",iceb_tu_getlogin());
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








VVOD DANET(1);

VVOD VVOD1(2);
VVOD1.VVOD_SPISOK_add_ZAG(gettext("Удаление всех документов за год"));
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите год"));

naz:;
clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F10",gettext("выход"),NULL);

kom1=vvod1(&god,5,&VVOD1,NULL,stdscr,-1,-1);

switch(kom1)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("usl2_4.txt");
   goto naz;

  case ENTER:
    break;
    
  case FK10:
  case ESC:
     return;
       
  default:
   goto naz;
 }

DANET.VVOD_delete();
sprintf(strsql,gettext("Удалить документы за %d год ? Вы уверены ?"),god.ravno_atoi());
DANET.VVOD_SPISOK_add_MD(strsql);

if(danet(&DANET,2,stdscr) == 2)
 return;

VVOD PAR(2);
PAR.VVOD_SPISOK_add_MD(gettext("Введите пароль."));
if(parolv(&PAR,dt,mt,gt,0) != 0)
  return;

clear();
GDITE();
printw("%s %s.\n",gettext("Удаление всех документов за год."),god.ravno());
printw("%s.\n",gettext("Удаление подтверждающих записей"));
int		metkazap=0;

strcpy(strsql,"select VERSION()");
sql_readkey(&bd,strsql,&row,&cur);
if(SRAV(row[0],"3.22",1) == 0)
  metkazap=1;  
printw("VERSION:%s\n",row[0]);
refresh();

sprintf(strsql,"delete from Usldokum2 where datd >= '%s-1-1' and \
datd <= '%s-12-31'",god.ravno(),god.ravno());
if(sql_zap(&bd,strsql) != 0)
  msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);

int kolud=sql_rows(&bd);
int ikolud=kolud;
printw("%s: %d\n",gettext("Количество удаленных записей"),kolud);
if(kolud > 0)
 { 
  printw("%s\n",gettext("Оптимизируем таблицу"));
  refresh();
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table Usldokum2");
  if(metkazap == 1)
   {
    if(sql_zap(&bd,strsql) != 0)
     msql_error(&bd,gettext("Ошибка оптимизации таблицы !"),strsql);
   }
  else
    sql_readkey(&bd,strsql);
 }


printw("%s.\n",gettext("Удаление приложений к документам"));
GDITE();

sprintf(strsql,"delete from Usldokum3 where god=%s",god.ravno());

if(sql_zap(&bd,strsql) != 0)
  msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);

kolud=sql_rows(&bd);
ikolud+=kolud;
printw("%s: %d\n",gettext("Количество удаленных записей"),kolud);
if(kolud > 0)
 { 
  printw("%s\n",gettext("Оптимизируем таблицу"));
  refresh();
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table Usldokum3");
  if(metkazap == 1)
   {
    if(sql_zap(&bd,strsql) != 0)
     msql_error(&bd,gettext("Ошибка оптимизации таблицы !"),strsql);
   }
  else
    sql_readkey(&bd,strsql);
 }


printw("%s.\n",gettext("Удаление записей в документах"));
GDITE();

sprintf(strsql,"delete from Usldokum1 where datd >= '%s-1-1' and \
datd <= '%s-12-31'",god.ravno(),god.ravno());

if(sql_zap(&bd,strsql) != 0)
  msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);

kolud=sql_rows(&bd);
ikolud+=kolud;
printw("%s: %d\n",gettext("Количество удаленных записей"),kolud);
if(kolud > 0)
 { 
  printw("%s\n",gettext("Оптимизируем таблицу"));
  refresh();
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table Usldokum1");
  if(metkazap == 1)
   {
    if(sql_zap(&bd,strsql) != 0)
     msql_error(&bd,gettext("Ошибка оптимизации таблицы !"),strsql);
   }
  else
    sql_readkey(&bd,strsql);
 }

printw("%s.\n",gettext("Удаление заголовков документов"));
GDITE();

sprintf(strsql,"delete from Usldokum where datd >= '%s-1-1' and \
datd <= '%s-12-31'",god.ravno(),god.ravno());

if(sql_zap(&bd,strsql) != 0)
  msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);

kolud=sql_rows(&bd);
ikolud+=kolud;
printw("%s: %d\n",gettext("Количество удаленных записей"),kolud);

if(kolud > 0)
 { 
  printw("%s\n",gettext("Оптимизируем таблицу"));
  refresh();
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table Usldokum");
  if(metkazap == 1)
   {
    if(sql_zap(&bd,strsql) != 0)
     msql_error(&bd,gettext("Ошибка оптимизации таблицы !"),strsql);
   }
  else
    sql_readkey(&bd,strsql);
 }

VVOD SOOB(1);
sprintf(strsql,"%s:%d",gettext("Количество удаленных записей"),ikolud);
SOOB.VVOD_SPISOK_add_MD(strsql);
soobshw(&SOOB,stdscr,-1,-1,0,1);


}
