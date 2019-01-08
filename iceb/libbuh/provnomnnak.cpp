/*$Id: provnomnnak.c,v 1.16 2013-04-07 10:22:54 sasa Exp $*/
/*14.01.2010	19.01.2002 	Белых А.И.	provnomnnak.c
Проверка номера налоговой накладной
Если вернули 0-номера не найдено
             1-найден
*/
#include        "buhl.h"


int provnomnnak(short mes,short god,const char *nomnalnak)
{
char strsql[1024];
class iceb_tu_str bros("");
SQL_str row;
SQLCURSOR cur;
short mn=1,mk=12;


if(iceb_t_poldan("Нумерация налоговых накладных обновляется каждый месяц",&bros,"matnast.alx") == 0)
 {
  if(SRAV(bros.ravno(),"Вкл",1) == 0)
   {
    mn=mes;
    mk=mes;
   }
 }

sprintf(strsql,"select datd,nomd from Dokummat where tip=%d and \
datd >= '%04d-%02d-01' and datd <= '%04d-%02d-31' and nomnn='%s'",
2,god,mn,god,mk,nomnalnak);
if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
 {
  VVOD SOOB(1);

  sprintf(strsql,gettext("С номером %s налоговая накладная уже есть !"),nomnalnak);
  SOOB.VVOD_SPISOK_add_MD(strsql);

  sprintf(strsql,"%s %s %s",gettext("Материальный учет"),row[0],row[1]);
  SOOB.VVOD_SPISOK_add_MD(strsql);

  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(1);
 }

sprintf(strsql,"select datd,nomd from Usldokum where tp=%d and \
datd >= '%04d-%02d-01' and datd <= '%04d-%02d-31' and nomnn='%s'",
2,god,mn,god,mk,nomnalnak);
if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
 {
  VVOD SOOB(1);

  sprintf(strsql,gettext("С номером %s налоговая накладная уже есть !"),nomnalnak);
  SOOB.VVOD_SPISOK_add_MD(strsql);

  sprintf(strsql,"%s %s %s",gettext("В учете услуг"),row[0],row[1]);
  SOOB.VVOD_SPISOK_add_MD(strsql);

  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(1);
 }

sprintf(strsql,"select datd,nomd from Uosdok where tipz=%d and \
datd >= '%04d-%02d-01' and datd <= '%04d-%02d-31' and nomnn='%s'",
2,god,mn,god,mk,nomnalnak);
if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
 {
  VVOD SOOB(1);

  sprintf(strsql,gettext("С номером %s налоговая накладная уже есть !"),nomnalnak);
  SOOB.VVOD_SPISOK_add_MD(strsql);

  sprintf(strsql,"%s %s %s",gettext("В учете основных средств"),row[0],row[1]);
  SOOB.VVOD_SPISOK_add_MD(strsql);

  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(1);
 }
return(0);
}


