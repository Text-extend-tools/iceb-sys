/* $Id: zaptabel.c,v 5.12 2011-01-13 13:43:25 sasa1 Exp $ */
/*24.06.2002	15.05.2001	Белых А.И.	zaptabel.c
Запись в таблицу строки с табелем
*/
#include        <pwd.h>
#include        <time.h>
#include        "buhg.h"
#ifdef	HAVE_UNISTD_H
#include <unistd.h>
#endif


int zaptabel(int mz, //0-новая запись 1-корректировка
short mp,short gp,
long tabn,
int tz,
float dnei,
float has,
float kdnei,
short dn,short mn,short gn,
short dk,short mk,short gk,
float kolrd,
float kolrh,
const char *koment, //Коментарий
int nomz, //Номер записи
int snomz) //Старый номер записи
{
time_t vrem;
char	strsql[512];

GDITE();

time(&vrem);


dnei=okrug(dnei,0.01);
has=okrug(has,0.01);
kdnei=okrug(kdnei,0.01);
kolrd=okrug(kolrd,0.01);
kolrh=okrug(kolrh,0.01);

if(mz == 0)
 {
  sprintf(strsql,"insert into Ztab \
  values (%d,%d,%ld,%d,%f,%f,%f,'%04d-%02d-%02d','%04d-%02d-%02d',%d,%ld,\
  %.2f,%.2f,'%s',%d)",
  gp,mp,tabn,tz,dnei,has,kdnei,gn,mn,dn,
  gk,mk,dk,iceb_t_getuid(),vrem,kolrd,kolrh,koment,nomz);

 }
if(mz == 1)
 {
  sprintf(strsql,"update Ztab \
set \
dnei=%f,\
has=%f,\
kdnei=%f,\
datn='%04d-%02d-%02d',\
datk='%04d-%02d-%02d',\
ktoz=%d,\
vrem=%ld, \
kolrd=%.2f, \
kolrh=%.2f,\
kom='%s',\
nomz=%d \
where god=%d and mes=%d and tabn=%ld and kodt=%d and nomz=%d",
  dnei,has,kdnei,gn,mn,dn,gk,mk,dk,
  iceb_t_getuid(),vrem,kolrd,kolrh,koment,nomz,
  gp,mp,tabn,tz,snomz);
 }
     
//printw("\nmz=%d strsql=%s\n",mz,strsql);
//OSTANOV();
        
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_ENTRY)
   {
    VVOD SOOB(1);
    sprintf(strsql,"zaptabel %s ",gettext("Такая запись уже есть !"));
    SOOB.VVOD_SPISOK_add_MD(strsql);
    sprintf(strsql,"%s: %ld",gettext("Табельный номер"),tabn);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    sprintf(strsql,"%s: %d",gettext("Код табеля"),tz);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    
    soobshw(&SOOB,stdscr,-1,-1,0,1);
   }
  else
    msql_error(&bd,"zartabel-Ошибка ввода записи табеля !",strsql);
  return(1);
 }
return(0);
}
