/*$Id: zar_read_tn1.c,v 5.5 2013/05/17 14:56:17 sasa Exp $*/
/*30.11.2011	04.04.2011	Белых А.И	zar_read_tn1.c
Чтение настроек минимальной зарплаты и прожиточного минимума 
по дате
*/
#include "buhg.h"

void zar_read_tn1(short dn,short mn,short gn, //дата на которую нам нужны данные
class zar_read_tn1h *nastr)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[1024];
/*читаем на начало года*/
sprintf(strsql,"select mz,pm from Zarmzpm where data <= '%04d-%02d-%d' order by data desc limit 1",gn,1,1);
if(readkey(strsql,&row,&cur) > 0)
 {
  nastr->minzar_ng=atof(row[0]);
  nastr->prog_min_ng=atof(row[1]);
  sprintf(strsql,"%s-Минимальная зарплата на начало %d года=%s Прожиточный минимум на начало года=%s\n",__FUNCTION__,gn,row[0],row[1]);
  zar_pr_ins(strsql);
 }
else
 {
  sprintf(strsql,"%s-Не найдено минимальную зарплату и прожиточный минимум на начало года %d\n",__FUNCTION__,gn);
  zar_pr_ins(strsql);
 }  
sprintf(strsql,"select mz,pm,ms from Zarmzpm where data <= '%04d-%02d-%d' order by data desc limit 1",gn,mn,dn);
if(readkey(strsql,&row,&cur) > 0)
 {
  nastr->minzar=atof(row[0]);
  nastr->prog_min_tek=atof(row[1]);
  nastr->max_sum_for_soc=atof(row[2]);
  sprintf(strsql,"%s-Дата %d.%d г. Минимальная зарплата %s Прожиточный минимум %s Максимальная сумма %s\n",__FUNCTION__,mn,gn,row[0],row[1],row[2]);
  zar_pr_ins(strsql);
 }
else
 {
  sprintf(strsql,"%s-Не найдено минимальную зарплату и прожиточный минимум на дату %d.%d.%d\n",__FUNCTION__,dn,mn,gn);
  zar_pr_ins(strsql);
 } 
}
