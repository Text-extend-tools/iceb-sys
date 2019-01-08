/*$Id: read_nast_ind.c,v 5.11 2014/07/31 07:09:17 sasa Exp $*/
/*15.07.2015	19.05.2000	Белых А.И.	read_nast_ind.c
Чтение настроек для расчёта индексации зарплаты
*/
#include "buhg.h"
#include "read_nast_ind.h"


int read_nast_ind(short mr,short gr,class index_ua *data)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

data->free();


sprintf(strsql,"select str from Alx where fil='zarindex.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarindex.alx");
  iceb_t_soob(strsql);
  return(1);
 }



short mi,gi;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(polen(row_alx[0],strsql,sizeof(strsql),1,'|') != 0)
    continue;

  if(SRAV("Дата индексации",strsql,0) == 0)
   {
    polen(row_alx[0],strsql,sizeof(strsql),2,'|');
    if(SRAV("Текущая",strsql,0) == 0)
     {
      mi=mr;
      gi=gr;
     }
    else
     if(rsdat1(&mi,&gi,strsql) != 0)
       continue;
    data->mr.plus(mi,-1);
    data->gr.plus(gi,-1);
    continue;
   }

  if(atoi(strsql) <= 0) //Значит первое поле не цифровое
   continue;

  if(rsdat1(&mi,&gi,strsql) != 0)
     continue;
  data->mi.plus(mi,-1);
  data->gi.plus(gi,-1);

  polen(row_alx[0],strsql,sizeof(strsql),2,'|');
  data->koef.plus(atof(strsql),-1);
 }






/*Читаем таблицу для индексации зарплаты выплаченной с задержкой*/
sprintf(strsql,"select str from Alx where fil='zarindexv.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  return(1);
 }


while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(polen(row_alx[0],strsql,sizeof(strsql),1,'|') != 0)
    continue;

  if(SRAV("Код начисления индексации невовремя выплаченной зарплаты",strsql,0) == 0)
   {
    polen(row_alx[0],&data->kninvz,2,'|');
    continue;
   }

  if(atoi(strsql) <= 0) //Значит первое поле не цифровое
   continue;

  if(rsdat1(&mi,&gi,strsql) != 0)
   continue;

  data->miv.plus(mi,-1);
  data->giv.plus(gi,-1);

  polen(row_alx[0],strsql,sizeof(strsql),2,'|');
  data->koefv.plus(atof(strsql),-1);

 }

if(iceb_t_poldan("Код перечисления зарплаты на счёт",strsql,"zarnast.alx") == 0)
 {
  data->kodpzns=atoi(strsql);
 }

if(iceb_t_poldan("Код получения зарплаты в кассе",strsql,"zarnast.alx") == 0)
 {
  data->kodpzvk=atoi(strsql);
 }


return(0);

}
