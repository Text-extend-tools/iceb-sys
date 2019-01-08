/* $Id: zapzarn.c,v 5.12 2013/05/17 14:56:16 sasa Exp $ */
/*11.02.2003    05.04.1994      Белых А.И.      zapzarn.c
Программа записи текущей настройки на человека
*/
#include        "buhg.h"


void            zapzarn(short mz,short gz, //Дата записи
int podr, //Подразделение
long tabn, //Табельный номер
short kateg, //Категория
short sovm,  //0-не совместитель 1-совместитель
short zvan,  //Звание
const char *shet, //Счет учета
const char *lgot, //Коды льгот
short dn,short mn,short gn, //Дата начала работы
short dk,short mk,short gk, //Дата конца работы
short prov,   //0-не выполнены 1-выполнены
const char *dolg //Должность
)
{
struct  tm      *bf;
time_t          tmm;
char		strsql[512];
class iceb_tu_str dolgz(dolg);

sprintf(strsql,"replace into Zarn \
values (%d,%d,%d,%ld,%d,%d,%d,'%s','%s',%d,'%04d-%d-%d','%04d-%d-%d','%s',%d)",
gz,mz,podr,tabn,kateg,sovm,zvan,shet,lgot,prov,gn,mn,dn,gk,mk,dk,dolgz.ravno_filtr(),0);

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR)
  return;
 else
  {
   char bros[512];
   sprintf(bros,"%s-%s",__FUNCTION__,gettext("Ошибка записи !"));
   msql_error(&bd,bros,strsql);
  }
 }
time(&tmm);
bf=localtime(&tmm);

if(gz > bf->tm_year+1900 || (gz = bf->tm_year+1900 && mz >= bf->tm_mon+1))
 {

  sprintf(strsql,"update Zarn set \
podr=%d,\
kateg=%d,\
tabn=%ld,\
sovm=%d,\
zvan=%d,\
shet='%s',\
lgot='%s',\
prov=%d,\
datn='%04d-%d-%d',\
datk='%04d-%d-%d',\
dolg='%s',\
blok=%d \
  where (god > %d or (god = %d and mes > %d)) and tabn=%ld",
  podr,kateg,tabn,sovm,zvan,shet,lgot,prov,gn,mn,dn,gk,mk,dk,dolgz.ravno_filtr(),0,
  bf->tm_year+1900,bf->tm_year+1900,bf->tm_mon+1,tabn);

  if(sql_zap(&bd,strsql) != 0)
   {
    char bros[512];
    sprintf(bros,"%s-%s",__FUNCTION__,gettext("Ошибка записи !"));
    msql_error(&bd,bros,strsql);
   }
 }
}
