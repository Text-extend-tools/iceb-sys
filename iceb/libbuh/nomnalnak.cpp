/*$Id: nomnalnak.c,v 5.14 2013/05/17 14:56:31 sasa Exp $*/
/*04.10.2009	07.08.2002	Белых А.И.	nomnalnak.c
Получение следующего наибольшего номера налоговой накладной
*/

#include        "buhl.h"


int   	nomnalnak(short mes,short god)
{

char	strsql[512];
char bros[512];
SQL_str row;
int	nomnalnak=0;
int 	nn;
int	kolstr;
short mn=1,mk=12;

if(iceb_t_poldan("Нумерация налоговых накладных обновляется каждый месяц",bros,"matnast.alx") == 0)
 {
  
  if(SRAV(bros,"Вкл",1) == 0)
   {
    mn=mes;
    mk=mes;
   }
//  printw("\nbros=%s mes=%d\n",bros,mes);
//  OSTANOV();
 }

sprintf(strsql,"select nomnn from Dokummat where tip=%d and datd >= '%04d-%02d-01' and datd <= '%04d-%02d-31' and nomnn <> ''",
2,god,mn,god,mk);

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
while(cur.read_cursor(&row) != 0)
 {
  nn=atoi(row[0]);
  if(nn > nomnalnak)
    nomnalnak=nn;  
 }

sprintf(strsql,"select nomnn from Usldokum where tp=%d and datd >= '%04d-%02d-01' and datd <= '%04d-%02d-31' and nomnn <> ''",
2,god,mn,god,mk);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
while(cur.read_cursor(&row) != 0)
 {
  nn=atoi(row[0]);
  if(nn > nomnalnak)
    nomnalnak=nn;  
 }

sprintf(strsql,"select nomnn from Uosdok where tipz=%d and datd >= '%04d-%02d-01' and datd <= '%04d-%02d-31' and nomnn <> ''",
2,god,mn,god,mk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
while(cur.read_cursor(&row) != 0)
 {
  nn=atoi(row[0]);
  if(nn > nomnalnak)
    nomnalnak=nn;  
 }
nomnalnak++;

return(nomnalnak);
}
