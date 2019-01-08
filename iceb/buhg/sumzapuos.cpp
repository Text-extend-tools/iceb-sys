/* $Id: sumzapuos.c,v 1.11 2013/09/26 09:43:41 sasa Exp $ */
/*12.10.2009	18.01.2001	Белых А.И.	sumzapuos.c
Получение суммы по документа учета основных средств
*/
#include        "buhg.h"

extern double	okrg1;  /*Округление суммы*/

void sumzapuos(int d,int m,int g, //Дата
const char *nn,  //Номер документа
double *suma,  //сумма по документа
double *sumabn, //cумма без ндс
int lnds, //Метка льгот по НДС
int tp)  //1-приход 2-расход
{
long		kolstr;
SQL_str         row;
char		strsql[512];
double		bb;

*sumabn=0.;
*suma=0.;

/*Узнаем сумму всех записей*/
if(tp == 2)
 sprintf(strsql,"select cena from Uosdok1 where datd='%d-%d-%d' and \
nomd='%s' and tipz=%d",
 g,m,d,nn,tp);
else
 sprintf(strsql,"select bs from Uosdok1 where datd='%d-%d-%d' and \
nomd='%s' and tipz=%d",
  g,m,d,nn,tp);

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
//  printw("\n%s-%s\n",__FUNCTION__,row[0]);    
  bb=atof(row[0]);
  bb=okrug(bb,okrg1);

  if(lnds > 0)
    *sumabn+=bb;
  else
    *suma+=bb;

 }

}
