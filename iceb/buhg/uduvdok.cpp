/* $Id: uduvdok.c,v 5.16 2013/05/17 14:56:13 sasa Exp $ */
/*16.04.2017	20.04.2000	Белых А.И.	uduvdok.c
удаление записи в документе и подтверждающей записи
Если вернули 0 удалили
             1 нет
*/
#include	"buhg.h"


int uduvdok(int tipz,short d,short m,short g,const char *nomd,
int podr,
int metka, //0-материал 1-услуга
int kodzap, //0-весь документ, если не равно нолю то этот код записи
int nomzap)
{
char		strsql[512];
/*
printw("uduvdok-%d %d.%d.%d %s %d %d %d\n",
tipz,d,m,g,nomd,podr,metka,kodzap);
refresh();
*/

/*Удаляем подтверждающие записи*/
if(kodzap == 0)
 sprintf(strsql,"delete from Usldokum2 where datd='%04d-%02d-%02d' and \
podr=%d and nomd='%s' and tp=%d",
 g,m,d,podr,nomd,tipz);
else
 sprintf(strsql,"delete from Usldokum2 where datd='%04d-%02d-%02d' and \
podr=%d and nomd='%s' and metka=%d and kodzap=%d and tp=%d and nz=%d",
 g,m,d,podr,nomd,metka,kodzap,tipz,nomzap);
/*
printw("\n%s\n",strsql);
refresh();
*/
if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
  {
   VVOD SOOB(1);
   SOOB.VVOD_SPISOK_add_MD(gettext("У вас нет полномочий для выполнения этой операции !"));
   soobshw(&SOOB,stdscr,-1,-1,0,1);
   return(1);
  }
 else
  msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
 }
/*Удаляем записи в документе*/
if(kodzap == 0)
 sprintf(strsql,"delete from Usldokum1 where datd='%04d-%02d-%02d' and \
podr=%d and nomd='%s' and tp=%d",
 g,m,d,podr,nomd,tipz);
else
 sprintf(strsql,"delete from Usldokum1 where datd='%04d-%02d-%02d' and \
podr=%d and nomd='%s' and metka=%d and kodzap=%d and tp=%d and nz=%d",
 g,m,d,podr,nomd,metka,kodzap,tipz,nomzap);
/*
printw("\n%s\n",strsql);
refresh();
*/
if(sql_zap(&bd,strsql) != 0)
  msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);

/*Удаляем шапку документа*/
if(kodzap == 0)
 {
  sprintf(strsql,"delete from Usldokum where datd='%04d-%02d-%02d' and \
podr=%d and nomd='%s' and tp=%d",
  g,m,d,podr,nomd,tipz);
 /*
 printw("\n%s\n",strsql);
 refresh();
 */
  if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);

  //Удаляем приложение к документа
  sprintf(strsql,"delete from Usldokum3 where podr=%d and god=%d and \
tp=%d and nomd='%s'",
   podr,g,tipz,nomd);
 /*
 printw("\n%s\n",strsql);
 refresh();
 */

  if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);

  //Удаляем список оплат к документу
  sprintf(strsql,"delete from Opldok where ps=2 and datd='%04d-%02d-%02d' and pd=%d and \
nomd='%s'",
  g,m,d,podr,nomd);

  sql_zapis(strsql,1,0);  

 }
return(0);
}
