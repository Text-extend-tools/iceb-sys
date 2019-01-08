/*$Id: zapzarp.c,v 5.23 2011-01-13 13:43:25 sasa1 Exp $*/
/*09.11.2008	25.05.1999	Белых А.И.	zapzarp.c
Запись строки начисления/удержания
Если вернули 0 - записали
             1 - запись уже есть
             2 - ошибка записи
             3 - у оператора доступ только на чтение
*/
#include        <time.h>
#include        <ctype.h>
#include        "buhg.h"
#ifdef	HAVE_UNISTD_H
#include <unistd.h>
#endif


int zapzarp(short d,short m,short g, //Дата записи
long tabn,short prn,
short knah,double suma,const char *shet,
short mesn,short godn, //В счет какого месяца
short kdn,
short nomz,
const char *kom,
int podr,
const char *nomdok,
struct ZARP zp)
{
time_t		vrem;
char		strsql[512];

if(zp.dz != 0)
 {
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"delete from Zarp where datz='%04d-%02d-%02d' and \
  tabn=%ld and prn='%d' and knah=%d and godn=%d and mesn=%d and \
  podr=%d and shet='%s' and nomz=%d",g,m,zp.dz,tabn,prn,knah,
  zp.godn,zp.mesn,zp.podr,zp.shet,zp.nomz);
        
//   printw("\nstrsql=%s\n",strsql);
  if(sql_zap(&bd,strsql) != 0)
   {
   if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Доступ только на чтение
    return(3);
   else
    {
     char bros[512];
     sprintf(bros,"%s-%s",__FUNCTION__,gettext("Ошибка удаления записи !"));
     msql_error(&bd,bros,strsql);
    }
   }
 }

time(&vrem);
suma=okrug(suma,0.01);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"insert into Zarp \
values ('%04d-%02d-%02d',%ld,'%d',%d,%.2f,'%s',%d,%d,%d,%d,%d,%ld,'%s',%d,'%s')",
g,m,d,tabn,prn,knah,suma,shet,godn,mesn,kdn,nomz,iceb_t_getuid(),vrem,kom,podr,nomdok);

//printw("zapzarp-%s\n",strsql);
//OSTANOV();

if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_ENTRY) //Запись уже есть
    return(1);
  else
   {
    msql_error(&bd,__FUNCTION__,strsql);
    return(2);
   }
 }

return(0);

}
