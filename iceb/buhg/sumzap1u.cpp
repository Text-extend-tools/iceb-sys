/* $Id: sumzap1u.c,v 5.11 2013/09/26 09:43:41 sasa Exp $ */
/*06.04.2016	18.01.2001	Белых А.И.	sumzap1u.c
Получение суммы по документа учета услуг
*/
#include        "buhg.h"

extern double	okrg1;  /*Округление суммы*/

void sumzap1u(int d,int m,int g, //Дата
const char *nn,  //Номер документа
int podr,  //код подразделения
double *suma,  //на которую начисляется НДС
double *sumabn, //cумма на которую не начисляется НДС
int lnds, //Метка льгот по НДС
int tp)  //1-приход 2-расход
{
long		kolstr=0;
SQL_str         row;
char		strsql[512];
double		bb;

*sumabn=0.;
*suma=0.;

/*Узнаем сумму всех записей*/
sprintf(strsql,"select cena,kolih from Usldokum1 where datd='%d-%d-%d' and \
podr=%d and nomd='%s' and tp=%d",
g,m,d,podr,nn,tp);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  
  bb=atof(row[0])*atof(row[1]);
  bb=okrug(bb,okrg1);

  if(lnds > 0)
    *sumabn+=bb;
  else
    *suma+=bb;

 }

}
