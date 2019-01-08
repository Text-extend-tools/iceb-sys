/*$Id: oplmu.c,v 5.4 2013/05/17 14:56:06 sasa Exp $*/
/*16.12.2009	16.12.2009	Белых А.И.	oplmu.c
проверка кода операции и снятие метки неоплаты для внутренних документов
*/
#include "buhg.h"

void oplmu(short d,short m,short g,int tipz,const char *nn,int skll,const char *kodop)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

if(SRAV("000",nn,0) != 0)
 {
  if(tipz == 1)
    sprintf(strsql,"select vido from Prihod where kod='%s'",kodop);
  if(tipz == 2)
    sprintf(strsql,"select vido from Rashod where kod='%s'",kodop);
  if(readkey(strsql,&row,&cur) != 1)
   return;

  if(atoi(row[0]) == 0) 
   return;
 }
  
sprintf(strsql,"update Dokummat \
set \
mo=%d \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' and tip=%d",
1,g,m,d,skll,nn,tipz);

if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_DBACCESS_DENIED_ERROR)
  msql_error(&bd,"oplmu-Ошибка корректировки записи !",strsql);


}

