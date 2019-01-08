/*$Id: uosplc.c,v 5.2 2012-11-29 09:48:57 sasa Exp $*/
/*16.07.2015	11.10.2012	Белых А.И.	uosplc.c
Проверка логической целосности подсистемы "Учёт основных средств"
*/
#include <errno.h>
#include "buhg.h"

int uosplc(class spis_oth *oth)
{
char strsql[2048];
SQL_str row;
class SQLCURSOR cur;
int kolstr=0;
int kolstr1=0;
int koloh=0;
int koludzap=0;
FILE *ff_prot;
char imaf_prot[64];

class VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(gettext("Проверка логической целосности базы"));
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0));

MENU.VVOD_SPISOK_add_MD(gettext("Проверка без удаления нелогичных записей"));
MENU.VVOD_SPISOK_add_MD(gettext("Проверка с удалением нелогичных записей"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

clearstr(LINES-1,0);

int metka_prov=0;
while(menu3w(stdscr,&MENU,&metka_prov,-1,-1,0) != 0);

switch (metka_prov)
 {
  case 2 :
  case -1:
    return(1);

  case 0 :
    break;

  case 1 :
    break;
  default:
   return(1); 
 }



time_t time_start=time(NULL);
clear();
if(metka_prov == 1)
 {
  /*проверяем полномочия оператора*/
  sprintf(strsql,"select osb,osn,gkb,gkn from icebuser where login='%s'",iceb_tu_getlogin());
  if(readkey(strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s %s icebuser",gettext("Не найден логин"),iceb_tu_getlogin(),gettext("в таблице"));
    iceb_t_soob(strsql);
    return(1);
   }
  if(atoi(row[0]) != 0 || atoi(row[1]) != 0 || atoi(row[2]) != 0 || atoi(row[3]) != 0)
   {
    iceb_t_soob(gettext("У вас нет полномочий для этой операции!"));
    return(1);
   }

 }


sprintf(imaf_prot,"uosplc%d.lst",getpid());

if((ff_prot = fopen(imaf_prot,"w")) == NULL)
 {
  error_op_nfil(imaf_prot,errno,"");
  return(1);
 }

if(metka_prov == 0)
 {
  sprintf(strsql,"%s",gettext("Проверка без удаления нелoгичных записей"));
  printw("%s\n",strsql);
 }  
else
 {
  sprintf(strsql,"%s",gettext("Проверка c удалением нелoгичных записей"));
  printw("%s\n",strsql);
 }
zagolov(strsql,"","",iceb_t_get_pnk("00",0),ff_prot);

fprintf(ff_prot,"\n%s\n",gettext("Подсистема \"Учёт основных средств\""));


printw("%s Uosdok1\n",gettext("Проверяем таблицу"));
fprintf(ff_prot,"%s Uosdok1\n",gettext("Проверяем таблицу"));

sprintf(strsql,"select * from Uosdok1 order by datd asc,tipz asc,nomd asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
 }

printw("%s %d\n",gettext("Количесво записей в таблице"),kolstr);
fprintf(ff_prot,"%s %d\n",gettext("Количесво записей в таблице"),kolstr);



if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

 

  /*Проверяем наличие шапки документа*/
  sprintf(strsql,"select tipz from Uosdok where datd='%s' and nomd='%s'",row[0],row[4]);

  if(readkey(strsql) == 0)
   {
    koloh++;
    printw("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8],row[9],row[10],row[11],row[12],row[13],row[14],row[15],row[16],row[17]);
    printw("%s!\n",gettext("Не найдена шапка документа"));
    refresh();
    
    fprintf(ff_prot,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8],row[9],row[10],row[11],row[12],row[13],row[14],row[15],row[16],row[17]);
    fprintf(ff_prot,"%s\n",strsql);
    fprintf(ff_prot,"%s!\n",gettext("Не найдена шапка документа"));

    if(metka_prov == 1)
     {
      sprintf(strsql,"delete from Uosdok1 where datd='%s' and nomd='%s'",row[0],row[4]);

      if(sql_zapis(strsql,1,0) == 0)
       koludzap++;

     }

   }  


 }

printw("\nПроверяем таблицу Uosin\n");
fprintf(ff_prot,"\nПроверяем таблицу Uosin\n");

sprintf(strsql,"select * from Uosin order by innom asc");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
 }

printw("%s %d\n",gettext("Количесво записей в таблице"),kolstr);
fprintf(ff_prot,"%s %d\n",gettext("Количесво записей в таблице"),kolstr);

kolstr1=0;

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  /*Проверяем наличие записей в документах*/
  sprintf(strsql,"select innom from Uosdok1 where innom=%s limit 1",row[0]);

  if(readkey(strsql) == 0)
   {
    koloh++;

    printw("%s|%s|%s|%s|%s|%s|%s|%s|%s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8]);
    printw("%s!\n",gettext("Не найдено записей в документах"));
    refresh();

    fprintf(ff_prot,"%s|%s|%s|%s|%s|%s|%s|%s|%s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8]);
    fprintf(ff_prot,"%s\n",strsql);
    fprintf(ff_prot,"%s!\n",gettext("Не найдено записей в документах"));


    if(metka_prov == 1)
     {
      /*проводки парные и они будут удалены по одной так как и к парной проводке тоже не будут найдены записи в документе*/

      sprintf(strsql,"delete from Uosin where innom=%s",row[0]);

      if(sql_zapis(strsql,1,0) == 0)
       koludzap++;

     }
   }  

 }

/*проверяем проводки сделанные к кассовым ордерам*/
printw("\nПроверяем таблицу Prov\n");
fprintf(ff_prot,"\nПроверяем таблицу Prov\n");

sprintf(strsql,"select * from Prov where kto='%s' order by datp asc",ICEB_MP_UOS);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
 }

printw("%s %d\n",gettext("Количесво записей в таблице"),kolstr);
fprintf(ff_prot,"%s %d\n",gettext("Количесво записей в таблице"),kolstr);

kolstr1=0;

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  /*Проверяем наличие записей в документе*/
  sprintf(strsql,"select tipz from Uosdok1 where datd='%s' and nomd='%s' limit 1",row[14],row[6]);

  if(readkey(strsql) == 0)
   {
    koloh++;

    printw("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8],row[9],row[10],row[11],row[12],row[13],row[14],row[15],row[16]);
    printw("%s!\n",gettext("Не найдены записи в документе"));
    refresh();

    fprintf(ff_prot,"\n%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8],row[9],row[10],row[11],row[12],row[13],row[14],row[15],row[16]);
    fprintf(ff_prot,"%s\n",strsql);
    fprintf(ff_prot,"%s!\n",gettext("Не найдены записи в документе"));


    if(metka_prov == 1)
     {
      /*проводки парные и они будут удалены по одной так как и к парной проводке тоже не будут найдены записи в документе*/

      sprintf(strsql,"delete from Prov where kto='%s' and pod=%d and nomd='%s' and datd='%s'",
      ICEB_MP_UOS,atoi(row[7]),row[6],row[14]);
      

      if(sql_zapis(strsql,1,0) == 0)
       koludzap++;

     }
   }  

 }

printw("\nПроверяем таблицу Uosamor\n");
fprintf(ff_prot,"\nПроверяем таблицу Uosamor\n");

sprintf(strsql,"select * from Uosamor where innom > 0 order by innom asc"); /*Отрицательные инветарные номера для амортизации вцелом по группе*/
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
 }

printw("%s %d\n",gettext("Количесво записей в таблице"),kolstr);
fprintf(ff_prot,"%s %d\n",gettext("Количесво записей в таблице"),kolstr);

kolstr1=0;

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  /*Проверяем наличие записей основного средства*/
  sprintf(strsql,"select innom from Uosin where innom=%s",row[0]);

  if(readkey(strsql) == 0)
   {
    koloh++;

    printw("%s|%s|%s|%s|%s|%s|%s|%s|%s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8]);
    printw("%s!\n",gettext("Не найден инвентарный номер"));
    refresh();

    fprintf(ff_prot,"%s|%s|%s|%s|%s|%s|%s|%s|%s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8]);
    fprintf(ff_prot,"%s\n",strsql);
    fprintf(ff_prot,"%s!\n",gettext("Не найден инвентарный номер"));


    if(metka_prov == 1)
     {
      /*проводки парные и они будут удалены по одной так как и к парной проводке тоже не будут найдены записи в документе*/

      sprintf(strsql,"delete from Uosamor where innom=%s",row[0]);

      if(sql_zapis(strsql,1,0) == 0)
       koludzap++;

     }
   }  

 }

printw("\nПроверяем таблицу Uosamor1\n");
fprintf(ff_prot,"\nПроверяем таблицу Uosamor1\n");

sprintf(strsql,"select * from Uosamor1 order by innom asc");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
 }

printw("%s %d\n",gettext("Количесво записей в таблице"),kolstr);
fprintf(ff_prot,"%s %d\n",gettext("Количесво записей в таблице"),kolstr);

kolstr1=0;

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  /*Проверяем наличие записей основного средства*/
  sprintf(strsql,"select innom from Uosin where innom=%s",row[0]);

  if(readkey(strsql) == 0)
   {
    koloh++;

    printw("%s|%s|%s|%s|%s|%s|%s|%s|%s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8]);
    printw("%s!\n",gettext("Не найден инвентарный номер"));
    refresh();

    fprintf(ff_prot,"%s|%s|%s|%s|%s|%s|%s|%s|%s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8]);
    fprintf(ff_prot,"%s\n",strsql);
    fprintf(ff_prot,"%s!\n",gettext("Не найден инвентарный номер"));


    if(metka_prov == 1)
     {
      /*проводки парные и они будут удалены по одной так как и к парной проводке тоже не будут найдены записи в документе*/

      sprintf(strsql,"delete from Uosamor1 where innom=%s",row[0]);

      if(sql_zapis(strsql,1,0) == 0)
       koludzap++;

     }
   }  

 }

printw("\nПроверяем таблицу Uosin1\n");
fprintf(ff_prot,"\nПроверяем таблицу Uosin1\n");

sprintf(strsql,"select * from Uosin1 order by innom asc");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
 }

printw("%s %d\n",gettext("Количесво записей в таблице"),kolstr);
fprintf(ff_prot,"%s %d\n",gettext("Количесво записей в таблице"),kolstr);

kolstr1=0;

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  /*Проверяем наличие записей основного средства*/
  sprintf(strsql,"select innom from Uosin where innom=%s",row[0]);

  if(readkey(strsql) == 0)
   {
    koloh++;

    printw("%s|%s|%s\n",row[0],row[1],row[2]);
    printw("%s!\n",gettext("Не найден инвентарный номер"));
    refresh();

    fprintf(ff_prot,"%s|%s|%s\n",row[0],row[1],row[2]);
    fprintf(ff_prot,"%s\n",strsql);
    fprintf(ff_prot,"%s!\n",gettext("Не найдены записи в документе"));


    if(metka_prov == 1)
     {
      /*проводки парные и они будут удалены по одной так как и к парной проводке тоже не будут найдены записи в документе*/

      sprintf(strsql,"delete from Uosin1 where innom=%s",row[0]);

      if(sql_zapis(strsql,1,0) == 0)
       koludzap++;

     }
   }  

 }

printw("\nПроверяем таблицу Uosinp\n");
fprintf(ff_prot,"\nПроверяем таблицу Uosinp\n");

sprintf(strsql,"select * from Uosinp order by innom asc");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
 }

printw("%s %d\n",gettext("Количесво записей в таблице"),kolstr);
fprintf(ff_prot,"%s %d\n",gettext("Количесво записей в таблице"),kolstr);

kolstr1=0;

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  /*Проверяем наличие записей основного средства*/
  sprintf(strsql,"select innom from Uosin where innom=%s",row[0]);

  if(readkey(strsql) == 0)
   {
    koloh++;

    printw("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8],row[9],row[10],row[11],row[12],row[13]);
    printw("%s!\n",gettext("Не найден инвентарный номер"));
    refresh();

    fprintf(ff_prot,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8],row[9],row[10],row[11],row[12],row[13]);
    fprintf(ff_prot,"%s\n",strsql);
    fprintf(ff_prot,"%s!\n",gettext("Не найдены записи в документе"));


    if(metka_prov == 1)
     {
      /*проводки парные и они будут удалены по одной так как и к парной проводке тоже не будут найдены записи в документе*/

      sprintf(strsql,"delete from Uosinp where innom=%s",row[0]);

      if(sql_zapis(strsql,1,0) == 0)
       koludzap++;

     }
   }  

 }
printw("\nПроверяем таблицу Uosindm\n");
fprintf(ff_prot,"\nПроверяем таблицу Uosindm\n");

sprintf(strsql,"select * from Uosindm  order by innom asc");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
 }

printw("%s %d\n",gettext("Количесво записей в таблице"),kolstr);
fprintf(ff_prot,"%s %d\n",gettext("Количесво записей в таблице"),kolstr);

kolstr1=0;

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  /*Проверяем наличие записей основного средства*/
  sprintf(strsql,"select innom from Uosin where innom=%s",row[0]);

  if(readkey(strsql) == 0)
   {
    koloh++;

    printw("%s|%s|%s|%s|%s|%s\n",row[0],row[1],row[2],row[3],row[4],row[5]);
    printw("%s!\n",gettext("Не найден инвентарный номер"));
    refresh();

    fprintf(ff_prot,"%s|%s|%s|%s|%s|%s\n",row[0],row[1],row[2],row[3],row[4],row[5]);
    fprintf(ff_prot,"%s\n",strsql);
    fprintf(ff_prot,"%s!\n",gettext("Не найдены записи в документе"));


    if(metka_prov == 1)
     {
      /*проводки парные и они будут удалены по одной так как и к парной проводке тоже не будут найдены записи в документе*/

      sprintf(strsql,"delete from Uosindm where innom=%s",row[0]);

      if(sql_zapis(strsql,1,0) == 0)
       koludzap++;

     }
   }  

 }


printw("\n%s %d\n",gettext("Количество ошибок"),koloh);
fprintf(ff_prot,"\n%s %d\n",gettext("Количество ошибок"),koloh);
if(koludzap > 0)
 {
  printw("%s %d\n",gettext("Количество удалённых записей"),koludzap);
  fprintf(ff_prot,"%s %d\n",gettext("Количество удалённых записей"),koludzap);
 }

podpis(ff_prot);
fclose(ff_prot);

oth->spis_imaf.plus(imaf_prot);
oth->spis_naim.plus(gettext("Протокол найденных ошибок"));


for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
  iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);


printw_vr(time_start);
OSTANOV();


return(0);
}

