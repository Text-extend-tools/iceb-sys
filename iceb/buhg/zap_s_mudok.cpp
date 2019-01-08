/*$Id: zap_s_mudok.c,v 5.17 2014/07/31 07:09:18 sasa Exp $*/
/*17.07.2014	15.02.2005	Белых А.И.	zap_s_mudok.c
Запись шапки документа материального учёта
Возвращаем 0-если записали
           1-если не записали
*/
#include "buhg.h"


int zap_s_mudok(int tipz,
short dd,short md,short gd,
int sklad,
const char *kontr,
const char *nomdok,
const char *nomnalnak,
const char *kodop,
int podt,
int prov,
const char *nomvstdok,
short dpnn,short mpnn,short gpnn,int metka_opl,const char *k00,float pnds)
{
char strsql[512];
time_t tmm;

//float pnds=iceb_t_pnds(dd,md,gd);

time(&tmm);

sprintf(strsql,"insert into Dokummat \
values (%d,'%04d-%02d-%02d',%d,'%s','%s','%s','%s',%d,%d,%d,%ld,'%s','%04d-%d-%d',%.2f,%d,'%s')",
tipz,gd,md,dd,sklad,kontr,nomdok,nomnalnak,kodop,podt,prov,iceb_t_getuid(),tmm,
nomvstdok,gpnn,mpnn,dpnn,pnds,metka_opl,k00);

if(sql_zap(&bd,strsql) != 0)
 {
  msql_error(&bd,gettext("Ошибка записи !"),strsql);
  return(1);
 }

return(0);
}

