/* $Id: matuddok.c,v 5.20 2013/09/26 09:43:35 sasa Exp $ */
/*25.02.2017	03.01.2000	Белых А.И.	matuddok.c
Удаление документов за год
*/
#include        "buhg.h"


void		matuddok()
{
short		god;
time_t   	tmm;
long		kolstr,kolstr1,kolstr2,kolstr3;
SQLCURSOR cur;
SQLCURSOR cur1;
SQL_str         row,row1,row2;
char		strsql[512];
short		d,m,g;
short		dd,md,gd;
int		kom1;
int		tipz,skl;
class iceb_tu_str vstr("");
short		metud;
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
clear();


/*проверяем полномочия оператора*/
sprintf(strsql,"select mub,mun from icebuser where login='%s'",iceb_tu_getlogin());
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

VVOD DANET(1);

DANET.VVOD_SPISOK_add_MD(gettext("Не подтвержденные документы удалять ?"));

metud=danet(&DANET,2,stdscr);

VVOD PAR(2);
PAR.VVOD_SPISOK_add_MD(gettext("Введите пароль"));
if(parolv(&PAR,dt,mt,gt,0) != 0)
  return;

time(&tmm);
clear();
GDITE();

/*Если документ не подтвержден, то пропускаем его
Если подтвержден, проверяем нет ли подтверждений следующим годом
Если есть тоже пропускаем
*/
sprintf(strsql,"select datd,nomd,sklad,pod,tip from Dokummat \
where datd >= '%04d-01-01' and datd <= '%04d-12-31' order by datd asc",
god,god);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return;
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;
 }

printw("%s:%d\n",gettext("Количество документов"),kolstr);
refresh();

kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  printw("%s %s %s %s",row[0],row[1],row[2],row[3]);
  strzag(LINES-1,0,kolstr,++kolstr1);
  
  if(metud == 2)
   {
    if(row[3][0] == '0')
     {
      printw(" %s !\n",gettext("Не подтвержден"));
      continue;
     }
   }
  else
   goto vp;
   
  sprintf(strsql,"select nomkar from Dokummat1 where datd='%s' and \
nomd='%s' and sklad=%s",row[0],row[1],row[2]);  
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,__FUNCTION__,strsql);
    continue;
   }

  if(kolstr2 == 0)
   {
    goto vp;
   }

  while(cur1.read_cursor(&row1) != 0)
   {
    sprintf(strsql,"select datdp from Zkart where sklad=%s and \
nomk=%s and datd='%s' and nomd='%s'",row[2],row1[0],row[0],row[1]);
    SQLCURSOR cur2;
    if((kolstr3=cur2.make_cursor(&bd,strsql)) < 0)
     {
      msql_error(&bd,__FUNCTION__,strsql);
      OSTANOV();
     }

    if(kolstr3 == 0)
     {
      goto vp;
     }

    while(cur2.read_cursor(&row2) != 0)
     {
      rsdat(&d,&m,&g,row2[0],2);
      if(g > god)
       {
        beep();
        printw(gettext("Документ подтвержден в %d году.\n"),g);
        OSTANOV();
        continue;
       }
      } 
   }

vp:;
  printw(" %s\n",gettext("Удален"));


  tipz=atoi(row[4]); 
  rsdat(&dd,&md,&gd,row[0],2);
  skl=atoi(row[2]);

  matudd(0,1,0,tipz,dd,md,gd,row[1],skl,0,0);

  
 } 


printw("%s Dokummat",gettext("Оптимизация таблицы"));
GDITE();

strcpy(strsql,"select VERSION()");
sql_readkey(&bd,strsql,&row,&cur);

printw("VERSION:%s\n",row[0]);
refresh();

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table Dokummat");
sql_readkey(&bd,strsql);

printw("%s\n",gettext("Оптимизацию таблицы закончено"));


printw("%s Dokummat1",gettext("Оптимизация таблицы"));
GDITE();
memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table Dokummat1");
sql_readkey(&bd,strsql);

printw("%s\n",gettext("Оптимизацию таблицы закончено"));

printw("%s Dokummat2",gettext("Оптимизация таблицы"));
GDITE();
memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table Dokummat2");
sql_readkey(&bd,strsql);

printw("%s\n",gettext("Оптимизацию таблицы закончено"));

printw("%s Dokummat3",gettext("Оптимизация таблицы"));
GDITE();
memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table Dokummat3");
sql_readkey(&bd,strsql);

printw("%s\n",gettext("Оптимизацию таблицы закончено"));


printw("%s Kart",gettext("Оптимизация таблицы"));
GDITE();
memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table Kart");
sql_readkey(&bd,strsql);

printw("%s\n",gettext("Оптимизацию таблицы закончено"));

printw("%s Zkart",gettext("Оптимизация таблицы"));
GDITE();
memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table Zkart");
sql_readkey(&bd,strsql);

printw("%s\n",gettext("Оптимизацию таблицы закончено"));


printw("%s Dokummat4",gettext("Оптимизация таблицы"));
GDITE();
sql_readkey(&bd,"optimize table Dokummat4");

printw("%s\n",gettext("Оптимизацию таблицы закончено"));


printw_vr(tmm);

OSTANOV();

}
