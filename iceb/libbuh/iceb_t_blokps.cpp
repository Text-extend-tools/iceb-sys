/*$Id: iceb_t_blokps.c,v 5.3 2013/12/31 11:48:19 sasa Exp $*/
/*29.11.2013	06.11.2012	Белых А.И.	iceb_t_blokps.c
Автоматическая блокировка подсистем
*/
#include "buhl.h"
extern int iceb_kod_podsystem;

void iceb_t_blokps()
{
char strsql[1024];

short d=0,m=0,g=0;
poltekdat(&d,&m,&g);

dpm(&d,&m,&g,4);
dpm(&d,&m,&g,4);

sprintf(strsql,"select kod from Blok where kod=%d and god=%d and mes=%d",iceb_kod_podsystem,g,m);
if(readkey(strsql) > 0)
 return;

sprintf(strsql,"select kod from Blok where kod=%d and god=%d and mes=0",iceb_kod_podsystem,g);
if(readkey(strsql) > 0)
 return;

sprintf(strsql,"insert into Blok (kod,god,mes,ktoz,vrem) values(%d,%d,%d,%d,%ld)",iceb_kod_podsystem,g,m,ICEB_KOD_OP_PROG,time(NULL));
//sql_zapis(strsql,0,1);
if(sql_zap(&bd,strsql) != 0)
 {
  int kod_oh=sql_nerror(&bd);
  if(kod_oh  == ER_DUP_ENTRY) //Запись уже есть
   {
    return;
   }

  if(kod_oh == ER_DBACCESS_DENIED_ERROR) //Только чтение
   {
    return;
   }

  msql_error(&bd,"__FUNCTION__",strsql);
  return;
 }

}
