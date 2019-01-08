/*$Id: zarprtn.c,v 5.15 2013/05/17 14:56:17 sasa Exp $*/
/*29.03.2017	07.02.2008	Белых А.И.	zarprtn.c
Проверка есть ли табельный номер в файле и входит ли дата в диапазон действия настройки
Если вернули 1- табельный номер есть
             0- нет
*/
#include "buhg.h"

int zarprtn(short mr,short gr,int tabn,int metka_s)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;

if(metka_s == 1)
    sprintf(strsql,"%s-Проверяем справочник пенсионеров\n",__FUNCTION__);
if(metka_s == 2)
    sprintf(strsql,"%s-Проверяем справочник инвалидов\n",__FUNCTION__);
if(metka_s == 3)
    sprintf(strsql,"%s-Проверяем справочник работающих по договорам подряда\n",__FUNCTION__);
zar_pr_ins(strsql);

sprintf(strsql,"select dnd,dkd,kom from Zargr where kg=%d and tn=%d and dnd <= '%04d-%02d-01' order by dnd desc limit 1",metka_s,tabn,gr,mr);
if(readkey(strsql,&row,&cur) <= 0)
 {
  sprintf(strsql,"%s-В справочнике записи нет\n",__FUNCTION__);
  zar_pr_ins(strsql);

  return(0);
 }
sprintf(strsql,"%s-Обнаружена запись dnd=%s dkd=%s kom=%s\n",__FUNCTION__,row[0],row[1],row[2]);
zar_pr_ins(strsql);

if(SRAV(row[1],"0000-00-00",0) == 0)
 {
  sprintf(strsql,"%s-Подходит\n",__FUNCTION__);
  zar_pr_ins(strsql);
  return(1);
 }
short d=0,m=0,g=0;
rsdat(&d,&m,&g,row[1],2);
if(sravmydat(1,mr,gr,1,m,g) > 0)
 return(0);

sprintf(strsql,"%s-Подходит\n",__FUNCTION__);
zar_pr_ins(strsql);
return(1);
}
