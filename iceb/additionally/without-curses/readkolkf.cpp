/* $Id: readkolk.c,v 5.10 2013/05/17 14:56:09 sasa Exp $ */
/*17.02.2009	28.09.1997	Белых А.И.	readkolkf.c
Получение количества реально выданного по документа
*/
#include "bezcurses.h"

double readkolk(int skl,int nk,short d,short m,short g,const char *nn)
{
double		kol;
SQL_str         row;
char		strsql[512];
long		kolstr;

/*Читаем в карточке количество реально выданное-полученное*/
sprintf(strsql,"select kolih from Zkart where datd='%d-%02d-%02d' and \
nomd='%s' and sklad=%d and nomk=%d",
g,m,d,nn,skl,nk);

//printw("\nreadkolk-strsql=%s\n",strsql);
//refresh();

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
 }

if(kolstr <= 0)
  return(0.);
 
kol=0.;
while(cur.read_cursor(&row) != 0)
 {
//   printw("kol=%s\n",row[0]);
   kol+=atof(row[0]);
 } 

//printw("итог kol=%.2f\n",kol);
//OSTANOV();

return(kol);

}
