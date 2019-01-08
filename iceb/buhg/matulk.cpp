/*$Id: matulk.c,v 1.27 2013/08/19 06:44:35 sasa Exp $*/
/*26.01.2018	08.09.2001	Белых А.И.	matulk.c
Контроль логической корректности базы данных
*/
#include        <errno.h>
#include        "buhg.h"

extern char *imabaz;

void	matulk()
{
char		strsql[1024];
SQLCURSOR	cur;
SQLCURSOR	cur1;
SQL_str		row,row1;
int		kolstr,kolstr1;
int		kolerror=0;
time_t		tmm;
FILE		*ff;
char		imaf[64];

VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите режим"));

MENU.VVOD_SPISOK_add_MD(gettext("Проверка логической целосности базы данных"));
MENU.VVOD_SPISOK_add_MD(gettext("Проверка логической целосности базы данных с удалением нелогичных записей"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

int metka=0; //без удаления записей
while(menu3w(stdscr,&MENU,&metka,-1,-1,0) != 0);
 
if(metka == -1 || metka == 2)
 return;




time(&tmm);

clear();

if(metka == 1)
 {
  /*проверяем полномочия оператора*/
  sprintf(strsql,"select mub,mun,gkb,gkn from icebuser where login='%s'",iceb_tu_getlogin());
  if(readkey(strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s %s icebuser",gettext("Не найден логин"),iceb_tu_getlogin(),gettext("в таблице"));
    iceb_t_soob(strsql);
    return;
   }
  if(atoi(row[0]) != 0 || atoi(row[1]) != 0 || atoi(row[2]) != 0 || atoi(row[3]) != 0)
   {
    iceb_t_soob(gettext("У вас нет полномочий для этой операции!"));
    return;
   }

 }


sprintf(imaf,"matulk%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

iceb_t_zagolov(gettext("Проверка логической целосности базы данных"),0,0,0,0,0,0,ff);

printw("%s\n",gettext("Проверка логической целосности базы данных"));
printw("%s:%s %s\n",gettext("База данных"),imabaz,iceb_t_get_pnk("00",0));
fprintf(ff,"%s:%s %s\n",gettext("База данных"),imabaz,iceb_t_get_pnk("00",0));

GDITE();


printw("%s Dokummat1\n",gettext("Проверка таблицы"));
refresh();
fprintf(ff,"%s Dokummat1\n",gettext("Проверка таблицы"));

sprintf(strsql,"select distinct datd,sklad,nomd,tipz,nomkar from Dokummat1 order by datd asc");

GDITE();
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

printw("%s %d\n",gettext("Количество записей"),kolstr);
refresh();
fprintf(ff,"%s %d\n",gettext("Количесво записей в таблице"),kolstr);

fprintf(ff,"\
--------------------------------------\n");
fprintf(ff,"%s\n",
gettext("  Дата    |Склад|+/-|Номер документа"));
fprintf(ff,"\
--------------------------------------\n");

printw("\
-----------------------------------------------\n");
printw("%s\n",
gettext("  Дата    |Склад|+/-|Номер документа"));

printw("\
-----------------------------------------------\n");

kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  sprintf(strsql,"select tip from Dokummat where tip=%s and \
datd='%s' and sklad=%s and nomd='%s'",row[3],row[0],row[1],row[2]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) <= 0)
   {
    beep();
    printw("%s %-5s %-3s %s\n",iceb_tu_datzap(row[0]),row[1],row[3],row[2]);
    refresh();
    fprintf(ff,"%s %-5s %-3s %s\n",iceb_tu_datzap(row[0]),row[1],row[3],row[2]);
    kolerror++;

    if(metka == 1) 
     {
      sprintf(strsql,"delete from Dokummat1 where datd='%s' and sklad=%s and nomd='%s' and \
tipz=%s and nomkar=%s",row[0],row[1],row[2],row[3],row[4]);
      sql_zapis(strsql,1,0);
     }
   }
  if(row[4][0] == '0')
   continue;

  sprintf(strsql,"select nomk from Kart where sklad=%s and \
nomk=%s",row[1],row[4]);
  if(readkey(strsql,&row1,&cur1) <= 0)
   {
    beep();
    printw("%s %-5s %-3s %s\n",iceb_tu_datzap(row[0]),row[1],row[3],row[2]);
    printw("%s %s %s %s !\n",
    gettext("Не найдена карточка"),row[4],
    gettext("на складе"),row[3]);
    refresh();

    fprintf(ff,"%s %-5s %-3s %s\n",iceb_tu_datzap(row[0]),row[1],row[3],row[2]);
    fprintf(ff,"%s %s %s %s !",
    gettext("Не найдена карточка"),row[4],
    gettext("на складе"),row[3]);
    kolerror++;
   }
 }


printw("\n%s Dokummat2\n",gettext("Проверка таблицы"));
refresh();

fprintf(ff,"\n%s Dokummat2\n",gettext("Проверка таблицы"));


GDITE();

//sprintf(strsql,"select distinct god,sklad,nomd from Dokummat2 order by god asc"); с таким запросом почемуто неправильно работает
sprintf(strsql,"select god,sklad,nomd from Dokummat2 order by god asc");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

printw("%s %d\n",gettext("Количество записей"),kolstr);
refresh();
fprintf(ff,"%s %d\n",gettext("Количесво записей в таблице"),kolstr);
kolstr1=0;


if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  sprintf(strsql,"select tip from Dokummat where \
datd >='%s-01-01' and datd <= '%s-12-31' and sklad=%s and nomd='%s'",row[0],row[0],row[1],row[2]);
  if(readkey(strsql,&row1,&cur1) <= 0)
   {
    beep();
    printw("%10s %-5s %-3s %s\n",row[0],row[1],"",row[2]);
    refresh();
    fprintf(ff,"%s %-5s %-3s %s\n",row[0],row[1],"",row[2]);
    kolerror++;
    if(metka == 1) 
     {
      sprintf(strsql,"delete from Dokummat2 where god='%s' and sklad=%s and nomd='%s'",
      row[0],row[1],row[2]);
      sql_zapis(strsql,1,0);
     }
   }
 }


printw("\n%s Dokummat3\n",gettext("Проверка таблицы"));
refresh();

fprintf(ff,"\n%s Dokummat3\n","Проверка таблицы");

sprintf(strsql,"select distinct datd,sklad,nomd from Dokummat3 order by datd asc");

GDITE();
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

printw("%s %d\n",gettext("Количество записей"),kolstr);
refresh();
fprintf(ff,"%s %d\n",gettext("Количесво записей в таблице"),kolstr);

kolstr1=0;

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  sprintf(strsql,"select tip from Dokummat where \
datd='%s' and sklad=%s and nomd='%s'",row[0],row[1],row[2]);
  if(readkey(strsql,&row1,&cur1) <= 0)
   {
    beep();
    printw("%s %-5s %-3s %s\n",iceb_tu_datzap(row[0]),row[1],"",row[2]);
    refresh();
    fprintf(ff,"%s %-5s %-3s %s\n",iceb_tu_datzap(row[0]),row[1],"",row[2]);
    kolerror++;

    if(metka == 1) 
     {
      sprintf(strsql,"delete from Dokummat3 where datd='%s' and sklad=%s and nomd='%s'",
      row[0],row[1],row[2]);
      sql_zapis(strsql,1,0);
     }
   }
 }


GDITE();


printw("\n%s Dokummat4\n",gettext("Проверка таблицы"));
refresh();
fprintf(ff,"%s Dokummat4\n",gettext("Проверка таблицы"));

fprintf(ff,"\
--------------------------------------\n");
fprintf(ff,"%s\n",
gettext("  Дата    |Склад|Номер документа"));
fprintf(ff,"\
--------------------------------------\n");

printw("\
-----------------------------------------------\n");
printw("%s\n",
gettext("  Дата    |Склад|Номер документа"));

printw("\
-----------------------------------------------\n");

sprintf(strsql,"select distinct datd,skl,nomd from Dokummat4 order by datd asc");

GDITE();
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

printw("%s %d\n",gettext("Количество записей"),kolstr);
refresh();
fprintf(ff,"%s %d\n",gettext("Количесво записей в таблице"),kolstr);

kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  sprintf(strsql,"select tip from Dokummat where tip=2 and datd='%s' and sklad=%s and nomd='%s'",row[0],row[1],row[2]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) <= 0)
   {
    beep();
    printw("%s %-5s %s\n",iceb_tu_datzap(row[0]),row[1],row[2]);
    refresh();
    fprintf(ff,"%s %-5s %s\n",iceb_tu_datzap(row[0]),row[1],row[2]);
    fprintf(ff,"%s !\n",gettext("Не найдена шапка документа"));

    kolerror++;

    if(metka == 1) 
     {
      sprintf(strsql,"delete from Dokummat4 where datd='%s' and skl=%s and nomd='%s'",row[0],row[1],row[2]);
      sql_zapis(strsql,1,0);
     }
   }

 }





printw("\n%s Zkart\n",gettext("Проверка таблицы"));
refresh();
fprintf(ff,"\n%s Zkart\n",gettext("Проверка таблицы"));

fprintf(ff,"\
--------------------------------------\n");
fprintf(ff,"%s\n",
gettext("  Дата    |Склад|+/-|Номер документа"));
fprintf(ff,"\
--------------------------------------\n");

printw("\
-----------------------------------------------\n");
printw("%s\n",
gettext("  Дата    |Склад|+/-|Номер документа"));

printw("\
-----------------------------------------------\n");

sprintf(strsql,"select distinct sklad,nomk,nomd,datd,tipz from Zkart \
order by datd asc");

GDITE();
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

printw("%s %d\n",gettext("Количество записей"),kolstr);
refresh();
fprintf(ff,"%s %d\n",gettext("Количесво записей в таблице"),kolstr);

kolstr1=0;
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  sprintf(strsql,"select tip from Dokummat where tip=%s and \
datd='%s' and sklad=%s and nomd='%s'",row[4],row[3],row[0],row[2]);
  if(readkey(strsql,&row1,&cur1) <= 0)
   {
    beep();
    printw("%s %-5s %-3s %s\n",iceb_tu_datzap(row[3]),row[0],row[4],row[2]);
    printw("%s !\n",gettext("Не найдена шапка документа"));
    refresh();

    fprintf(ff,"%s %-5s %-3s %s\n",iceb_tu_datzap(row[3]),row[0],row[4],row[2]);
    fprintf(ff,"%s !\n",gettext("Не найдена шапка документа"));
    kolerror++;

    if(metka == 1) 
     {
      sprintf(strsql,"delete from Zkart where datd='%s' and sklad=%s and nomd='%s' and \
tipz=%s and nomk=%s",row[3],row[0],row[2],row[4],row[1]);
      sql_zapis(strsql,1,0);
     }
   }

  sprintf(strsql,"select tipz from Dokummat1 where tipz=%s and \
datd='%s' and sklad=%s and nomd='%s'",row[4],row[3],row[0],row[2]);
  if(readkey(strsql,&row1,&cur1) <= 0)
   {
    beep();
    printw("%s %-5s %-3s %s\n",iceb_tu_datzap(row[3]),row[0],row[4],row[2]);
    printw("%s !\n",gettext("Не найдена запись в документе"));
    refresh();
    fprintf(ff,"%s %-5s %-3s %s\n",iceb_tu_datzap(row[3]),row[0],row[4],row[2]);
    fprintf(ff,"%s !\n",gettext("Не найдена запись в документе"));
    kolerror++;

    if(metka == 1) 
     {
      sprintf(strsql,"delete from Zkart where datd='%s' and sklad=%s and nomd='%s' and \
tipz=%s and nomk=%s",row[3],row[0],row[2],row[4],row[1]);

      sql_zapis(strsql,1,0);
     }
   }
  sprintf(strsql,"select sklad from Kart where sklad=%s and nomk=%s",
  row[0],row[1]);
  if(readkey(strsql,&row1,&cur1) <= 0)
   {
    beep();
    printw("%s %-5s %-3s %s\n",iceb_tu_datzap(row[3]),row[0],row[4],row[2]);
    printw("%s !\n",gettext("Не найдена карточка"));
    refresh();
    fprintf(ff,"%s %-5s %-3s %s\n",iceb_tu_datzap(row[3]),row[0],row[4],row[2]);
    fprintf(ff,"%s !\n",gettext("Не найдена карточка"));
    kolerror++;

    if(metka == 1) 
     {
      sprintf(strsql,"delete from Zkart where datd='%s' and sklad=%s and nomd='%s' and \
tipz=%s and nomk=%s",row[3],row[0],row[2],row[4],row[1]);

      sql_zapis(strsql,1,0);
     }
   }
 }


printw("\n%s Kart\n",gettext("Проверка таблицы"));
refresh();
printw("\
------------------------------\n");
printw("%s\n",
gettext("Склад|Номер карточки"));

printw("\
------------------------------\n");

fprintf(ff,"\n%s Kart\n",gettext("Проверка таблицы"));
fprintf(ff,"\
--------------------\n");
fprintf(ff,"%s\n",
gettext("Склад|Номер карточки"));

fprintf(ff,"\
--------------------\n");


sprintf(strsql,"select sklad,nomk from Kart order by sklad asc");


GDITE();
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

printw("%s %d\n",gettext("Количество записей"),kolstr);
refresh();
fprintf(ff,"%s %d\n",gettext("Количесво записей в таблице"),kolstr);

kolstr1=0;

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  sprintf(strsql,"select tipz from Dokummat1 where \
sklad=%s and nomkar=%s limit 1",row[0],row[1]);
  if(readkey(strsql,&row1,&cur1) <= 0)
   {
    beep();
    printw("%-5s %s\n",row[0],row[1]);
    refresh();
    fprintf(ff,"%-5s %s\n",row[0],row[1]);
    kolerror++;

    if(metka == 1) 
     {
      sprintf(strsql,"delete from Kart where sklad=%s and nomk=%s",row[0],row[1]); 
      if(sql_zap(&bd,strsql) != 0)      
        msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
     }
   }
 }


/*проверяем проводки сделанные к документам*/
printw("\nПроверяем таблицу Prov\n");
fprintf(ff,"\nПроверяем таблицу Prov\n");

GDITE();

sprintf(strsql,"select * from Prov where kto='%s' order by datp asc",ICEB_MP_MATU);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
 }

printw("%s %d\n",gettext("Количесво записей в таблице"),kolstr);
fprintf(ff,"%s %d\n",gettext("Количесво записей в таблице"),kolstr);
refresh();
kolstr1=0;

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);


  /*Проверяем наличие записей в документе*/
  sprintf(strsql,"select datd from Dokummat1 where datd='%s' and sklad=%d and nomd='%s' limit 1",row[14],atoi(row[7]),row[6]);

  if(readkey(strsql) == 0)
   {
    kolerror++;

    printw("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8],row[9],row[10],row[11],row[12],row[13],row[14],row[15],row[16]);
    printw("%s!\n",gettext("Не найдены записи в документе"));
    refresh();

    fprintf(ff,"\n%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8],row[9],row[10],row[11],row[12],row[13],row[14],row[15],row[16]);
    fprintf(ff,"%s\n",strsql);
    fprintf(ff,"%s!\n",gettext("Не найдены записи в документе"));


    if(metka == 1)
     {
      /*проводки парные и они будут удалены по одной так как и к парной проводке тоже не будут найдены записи в документе*/

      sprintf(strsql,"delete from Prov where kto='%s' and pod=%d and nomd='%s' and datd='%s'",
      ICEB_MP_MATU,atoi(row[7]),row[6],row[14]);
      

      sql_zapis(strsql,1,0);

     }
   }  

 }



fclose(ff);

printw_vr(tmm);


if(kolerror == 0)
 {
  iceb_t_soob(gettext("Порядок, ошибок не обнаружено !"));
  unlink(imaf);
 }
else
 {
  VVOD SOOB(1);
  if(metka == 0)
    sprintf(strsql,"%s: %d",gettext("База не корректна ! Количество ошибок"),
    kolerror);
  if(metka == 1)
    sprintf(strsql,"%s: %d",gettext("Количество исправленых ошибок"),
    kolerror);

  SOOB.VVOD_SPISOK_add_MD(strsql);
  sprintf(strsql,"%s %s",gettext("Распечатку выгружено в файл"),imaf);
  SOOB.VVOD_SPISOK_add_MD(strsql);
  soobshw(&SOOB,stdscr,-1,-1,0,1);

  class spis_oth oth;

  oth.spis_imaf.plus(imaf);
  oth.spis_naim.plus(gettext("Протокол найденных ошибок"));

  iceb_t_ustpeh(imaf,3);

  iceb_t_rabfil(&oth,"");
    
 }


}
