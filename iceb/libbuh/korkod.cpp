/*$Id: korkod.c,v 5.32 2013/09/26 09:43:55 sasa Exp $*/
/*28.03.2018	30.03.2000	Белых А.И.	korkod.c
Программа замены одного кода на другой во всех таблицах
*/
#include	"buhl.h"


void korkod(const char *tabl, //Имя таблицы
const char *skod, //Старый код
const char *nkod) //Новый код
{
char		strsql[1024];

VVOD SOOBN(1);
SOOBN.VVOD_SPISOK_add_MD(gettext("Выполняется каскадная корректировка кода в таблицах !"));
soobsh(&SOOBN,-1,-1,1);

GDITE();



if(SRAV(tabl,"Uoshz",0) == 0)
 {
  sprintf(strsql,"update Uosinp set hzt='%s' where hzt='%s'",nkod,skod);
  if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

  sprintf(strsql,"update Uosamor set hzt='%s' where hzt='%s'",nkod,skod);
  if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

  sprintf(strsql,"update Uosamor1 set hzt='%s' where hzt='%s'",nkod,skod);
  if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
  return;
 }

if(SRAV(tabl,"Uoshau",0) == 0)
 {
  sprintf(strsql,"update Uosinp set hau='%s' where hau='%s'",nkod,skod);
  if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

  return;
 }



if(SRAV(tabl,"Kateg",0) == 0)
 {
  sprintf(strsql,"update Kartb set kateg=%s where kateg=%s",
  nkod,skod);
  if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

  sprintf(strsql,"update Zarn set kateg=%s where kateg=%s",
  nkod,skod);
  if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

  return;
 }

if(SRAV(tabl,"Tabel",0) == 0)
 {
  sprintf(strsql,"update Ztab set kodt=%s where kodt=%s",
  nkod,skod);
  if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

  sprintf(strsql,"update Nash set kodt=%s where kodt=%s",
  nkod,skod);
  if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

  return;
 }


if(SRAV(tabl,"Grup",0) == 0)
 {
  sprintf(strsql,"update Material set kodgr=%s where kodgr=%s",
  nkod,skod);
  if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
  return;
 }

if(SRAV(tabl,"Gkont",0) == 0)
 {
  sprintf(strsql,"update Kontragent set grup='%s' where grup='%s'",
  nkod,skod);
  if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
  return;
 }

if(SRAV(tabl,"Uslgrup",0) == 0)
 {
  sprintf(strsql,"update Uslugi set kodgr=%s where kodgr=%s",
  nkod,skod);
  if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
  return;
 }


if(SRAV(tabl,"Uosgrup",0) == 0)
 {
  sprintf(strsql,"update Uosinp set hna=%s where hna=%s",
  nkod,skod);
  if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

  sprintf(strsql,"update Uosamor set hna=%s where hna=%s",
  nkod,skod);
  if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
  return;
 }

if(SRAV(tabl,"Uosgrup1",0) == 0)
 {
  sprintf(strsql,"update Uosinp set hnabu=%s where hnabu=%s",
  nkod,skod);
  if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

  sprintf(strsql,"update Uosamor1 set hna=%s where hna=%s",
  nkod,skod);
  if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
  return;
 }


if(SRAV(tabl,"Opltp",0) == 0)
 {
  sprintf(strsql,"update Pltp set oper='%s' where oper='%s'",
  nkod,skod);
  if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

  sprintf(strsql,"update Tpltp set oper='%s' where oper='%s'",
  nkod,skod);
  if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

  sprintf(strsql,"update Prov set oper='%s' where oper='%s' and tz=2 and kto='%s'",nkod,skod,ICEB_MP_PPOR);
  if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

  return;
 }

if(SRAV(tabl,"Opltt",0) == 0)
 {
  sprintf(strsql,"update Pltt set oper='%s' where oper='%s'",
  nkod,skod);
  if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

  sprintf(strsql,"update Tpltt set oper='%s' where oper='%s'",
  nkod,skod);
  if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

  sprintf(strsql,"update Prov set oper='%s' where oper='%s' and tz=1 and kto='%s'",nkod,skod,ICEB_MP_PTRE);
  if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
  return;
 }

if(SRAV(tabl,"Reegrup1",0) == 0)
 {
  sprintf(strsql,"update Reenn set gr='%s' where gr='%s' and tp=1",nkod,skod);
  if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

  return;
 }
if(SRAV(tabl,"Reegrup2",0) == 0)
 {
  sprintf(strsql,"update Reenn set gr='%s' where gr='%s' and tp=2",nkod,skod);
  if(sql_zap(&bd,strsql) != 0)
    msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

  return;
 }
 
VVOD SOOB(1);
SOOB.VVOD_SPISOK_add_MD("Для этой таблицы не предусмотрена корректировка !!!");
SOOB.VVOD_SPISOK_add_MD(tabl);
soobshw(&SOOB,stdscr,-1,-1,0,1);


}
