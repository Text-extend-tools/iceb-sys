/*$Id: kormat.c,v 5.8 2013/05/17 14:56:04 sasa Exp $*/
/*24.02.2017	27.06.2001	Белых А.И.	kormat.c
Каскадная корректировка кода материалла
*/
#include        "buhg.h"


void kormat(int kodmatn,int kodmats)
{
char strsql[1024];

VVOD SOOBN(1);
SOOBN.VVOD_SPISOK_add_MD(gettext("Выполняется каскадная корректировка кода материалла в таблицах"));
soobsh(&SOOBN,LINES-6,0,1);

GDITE();


sprintf(strsql,"update Roznica set kodm=%d where kodm=%d and metka=1",kodmatn,kodmats);

if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   {
    VVOD SOOB(1);
    sprintf(strsql,"%s Roznica",gettext("Таблица"));
    SOOB.VVOD_SPISOK_add_MD(strsql);
    SOOB.VVOD_SPISOK_add_MD(gettext("Ошибка корректировки записи !"));
    SOOB.VVOD_SPISOK_add_MD(sql_error(&bd));
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    GDITE();
   }
 }

sprintf(strsql,"update Dokummat1 set kodm=%d where kodm=%d",kodmatn,kodmats);

if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   {
    VVOD SOOB(1);
    sprintf(strsql,"%s Dokummat1",gettext("Таблица"));
    SOOB.VVOD_SPISOK_add_MD(strsql);
    SOOB.VVOD_SPISOK_add_MD(gettext("Ошибка корректировки записи !"));
    SOOB.VVOD_SPISOK_add_MD(sql_error(&bd));
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    GDITE();
   }
 }

sprintf(strsql,"update Dokummat4 set km=%d where km=%d",kodmatn,kodmats);

sql_zapis(strsql,1,0);

sprintf(strsql,"update Dokummat4 set kd=%d where kd=%d",kodmatn,kodmats);

sql_zapis(strsql,1,0);

sprintf(strsql,"update Kart set kodm=%d where kodm=%d",kodmatn,kodmats);

if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   {
    VVOD SOOB(1);
    sprintf(strsql,"%s Kart",gettext("Таблица"));
    SOOB.VVOD_SPISOK_add_MD(strsql);
    SOOB.VVOD_SPISOK_add_MD(gettext("Ошибка корректировки записи !"));
    SOOB.VVOD_SPISOK_add_MD(sql_error(&bd));
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    GDITE();
   }
 }


sprintf(strsql,"update Usldokum1 set kodzap=%d where kodzap=%d and \
metka=0",kodmatn,kodmats);

if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   {
    VVOD SOOB(1);
    sprintf(strsql,"%s Usldokum1",gettext("Таблица"));
    SOOB.VVOD_SPISOK_add_MD(strsql);
    SOOB.VVOD_SPISOK_add_MD(gettext("Ошибка корректировки записи !"));
    SOOB.VVOD_SPISOK_add_MD(sql_error(&bd));
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    GDITE();
   }
 }

sprintf(strsql,"update Usldokum2 set kodzap=%d where kodzap=%d and metka=0",kodmatn,kodmats);

if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   {
    VVOD SOOB(1);
    sprintf(strsql,"%s Usldokum2",gettext("Таблица"));
    SOOB.VVOD_SPISOK_add_MD(strsql);
    SOOB.VVOD_SPISOK_add_MD(gettext("Ошибка корректировки записи !"));
    SOOB.VVOD_SPISOK_add_MD(sql_error(&bd));
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    GDITE();
   }
 }

sprintf(strsql,"update Specif set kodi=%d where kodi=%d",kodmatn,kodmats);
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   {
    VVOD SOOB(1);
    sprintf(strsql,"%s Specif",gettext("Таблица"));
    SOOB.VVOD_SPISOK_add_MD(strsql);
    SOOB.VVOD_SPISOK_add_MD(gettext("Ошибка корректировки записи !"));
    SOOB.VVOD_SPISOK_add_MD(sql_error(&bd));
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    GDITE();
   }
 }
sprintf(strsql,"update Specif set kodd=%d where kodd=%d",kodmatn,kodmats);
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   {
    VVOD SOOB(1);
    sprintf(strsql,"%s Specif",gettext("Таблица"));
    SOOB.VVOD_SPISOK_add_MD(strsql);
    SOOB.VVOD_SPISOK_add_MD(gettext("Ошибка корректировки записи !"));
    SOOB.VVOD_SPISOK_add_MD(sql_error(&bd));
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    GDITE();
   }
 }

sprintf(strsql,"update Uplmt set kodm=%d where kodm=%d",kodmatn,kodmats);
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   {
    VVOD SOOB(1);
    sprintf(strsql,"%s Uplmt",gettext("Таблица"));
    SOOB.VVOD_SPISOK_add_MD(strsql);
    SOOB.VVOD_SPISOK_add_MD(gettext("Ошибка корректировки записи !"));
    SOOB.VVOD_SPISOK_add_MD(sql_error(&bd));
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    GDITE();
   }
 }

sprintf(strsql,"update Restdok1 set kodz=%d where kodz=%d and mz=0",kodmatn,kodmats);
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   {
    VVOD SOOB(1);
    sprintf(strsql,"%s Restdok1",gettext("Таблица"));
    SOOB.VVOD_SPISOK_add_MD(strsql);
    SOOB.VVOD_SPISOK_add_MD(gettext("Ошибка корректировки записи !"));
    SOOB.VVOD_SPISOK_add_MD(sql_error(&bd));
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    GDITE();
   }
 }

}
