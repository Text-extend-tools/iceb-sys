/*$Id: xkasplc.c,v 5.2 2012-11-29 09:48:57 sasa Exp $*/
/*11.10.2012	11.10.2012	Белых А.И.	xkasplc.c
Проверка логической целосности подсистемы "Учёт кассовых ордеров"
*/
#include <errno.h>
#include "buhg.h"

int xkasplc(class spis_oth *oth)
{
char strsql[1024];
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
  sprintf(strsql,"select kob,kon,gkb,gkn from icebuser where login='%s'",iceb_tu_getlogin());
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

sprintf(imaf_prot,"kasplc%d.lst",getpid());

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

fprintf(ff_prot,"\n%s\n",gettext("Подсистема \"Учёт кассовых ордеров\""));


printw("%s Kasord1\n",gettext("Проверяем таблицу"));
fprintf(ff_prot,"%s Kasord1\n",gettext("Проверяем таблицу"));

sprintf(strsql,"select * from Kasord1 order by kassa asc,datd asc,tp asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
 }

printw("%s %d\n",gettext("Количесво записей в таблице"),kolstr);
fprintf(ff_prot,"%s %d\n",gettext("Количесво записей в таблице"),kolstr);


short d,m,g;
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  rsdat(&d,&m,&g,row[2],2);
 
  if(row[1][0] == '0')
   {
    koloh++;
 
    printw("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8],row[9],row[10],row[11]);
    printw("%s tp %s Каsord1 %s !!!\n",gettext("Колонка"),gettext("в таблице"),gettext("равна нолю"));
    refresh();
    
    fprintf(ff_prot,"\n%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8],row[9],row[10],row[11]);
    fprintf(ff_prot,"%s tp %s Каsord1 %s !!!\n",gettext("Колонка"),gettext("в таблице"),gettext("равна нолю"));

    if(metka_prov == 1)
     {
      sprintf(strsql,"delete from Kasord1 where kassa=%d and god=%d and tp=%d and nomd='%s' and kontr='%s'",
      atoi(row[0]),g,atoi(row[1]),row[4],row[5]);

      if(sql_zapis(strsql,1,0) == 0)
       koludzap++;

     }
    continue;
   }

  /*Проверяем наличие шапки документа*/
  sprintf(strsql,"select tp from Kasord where nomd='%s' and kassa=%d and tp=%d and god=%d",
  row[4],atoi(row[0]),atoi(row[1]),g);

  if(readkey(strsql) == 0)
   {
    koloh++;
    printw("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8],row[9],row[10],row[11]);
    printw("%s!\n",gettext("Не найдена шапка документа"));
    refresh();
    
    fprintf(ff_prot,"\n%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8],row[9],row[10],row[11]);
    fprintf(ff_prot,"%s\n",strsql);
    fprintf(ff_prot,"%s!\n",gettext("Не найдена шапка документа"));

    if(metka_prov == 1)
     {
      sprintf(strsql,"delete from Kasord1 where kassa=%d and god=%d and tp=%d and nomd='%s' and kontr='%s'",
      atoi(row[0]),g,atoi(row[1]),row[4],row[5]);

      if(sql_zapis(strsql,1,0) == 0)
       koludzap++;

     }

   }  


 }

/*проверяем проводки сделанные к кассовым ордерам*/
printw("\nПроверяем таблицу Prov\n");
fprintf(ff_prot,"\nПроверяем таблицу Prov\n");

sprintf(strsql,"select * from Prov where kto='%s' order by datp asc",ICEB_MP_KASA);
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

  rsdat(&d,&m,&g,row[14],2);
  /*Проверяем наличие записей в документе*/
  sprintf(strsql,"select tp from Kasord1 where kassa=%d and god=%d and tp=%d and nomd='%s'",
  atoi(row[7]),g,atoi(row[15]),row[6]);

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
      ICEB_MP_KASA,atoi(row[7]),row[6],row[14]);
      

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

