/*$Id: zarkvrt_zapvtabl.c,v 5.1 2012-11-29 09:48:58 sasa Exp $*/
/*19.03.2015	30.10.2012	Белых А.И.	zarkvrt_zapvtabl.c
*/
/**********************************************/
/*Запись во временную таблицу*/
/****************************************/

#include "buhg.h"

void  zarkvrt_zapvtabl(const char *imatmptab,
const char *inn,
int kod_doh,
const char *kod_lgoti,
double dohod_nah,
double dohod_vip,
double podoh_nah,
double podoh_vip,
const char *datanr,
const char *datakr,
const char *fio,       /*фамилия или наименование контрагента*/
const char *tab_kontr, /*табелыный номeр или код контрагента*/
FILE *ff_prot)
{
char strsql[512];

sprintf(strsql,"select inn from %s where inn='%s' and pd=%d and kl='%s'",imatmptab,inn,kod_doh,kod_lgoti);

if(readkey(strsql) == 0) /*нет такой записи*/
 {
   sprintf(strsql,"insert into %s values('%s',%d,%.2f,%.2f,%.2f,%.2f,'%s','%s','%s','%s','%s')",
   imatmptab,
   inn,
   kod_doh,
   dohod_nah,
   dohod_vip,
   podoh_nah,
   podoh_vip,
   datanr,
   datakr,
   kod_lgoti,
   fio,
   tab_kontr);
 }
else /*запись существует*/
 {
   sprintf(strsql,"update %s set \
ndoh=ndoh+%.2f,\
vdoh=vdoh+%.2f,\
npod=npod+%.2f,\
vpod=vpod+%.2f,\
kl='%s' \
where inn='%s' and pd=%d and kl='%s'",
   imatmptab,
   dohod_nah,
   dohod_vip,
   podoh_nah,
   podoh_vip,
   kod_lgoti,
   inn,kod_doh,kod_lgoti);
 }
//fprintf(ff_prot,"%s-%s\n",__FUNCTION__,strsql);
     
sql_zapis(strsql,1,0);


}
