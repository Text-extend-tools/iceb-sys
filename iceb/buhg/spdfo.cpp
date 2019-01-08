/*$Id: spdfo.c,v 5.16 2013/05/17 14:56:11 sasa Exp $*/
/*14.01.2010	08.11.2002	Белых А.И.	spdfo.c
Получить все дебетовые суммы за период по списку частных предпринимателей
определенным счетам
*/
#include        "buhg.h"


void	spdfo(\
short dn,short mn,short gn, //Дата начала периода
short dk,short mk,short gk, //Дата конца периода
SPISOK *kontr,
SPISOK *dat, //Даты последнего платежа
double *debet,
char *shet)
{
char		strsql[512];
SQL_str		row;
int		kolstr=0;
int		nm=0;

//Берём строго за квартал
sprintf(strsql,"select datp,sh,kodkon,deb from Prov where datp >= '%04d-%d-%d' and \
datp <= '%04d-%d-%d' and kodkon <> '' and deb <> 0. and val=0 \
order by datp asc",gn,mn,1,gn,mn+2,31); 

/*************
sprintf(strsql,"select datp,sh,kodkon,deb from Prov where datp >= '%04d-%d-%d' and \
datp <= '%04d-%d-%d' and kodkon <> '' and deb <> 0. and val=0 \
order by datp asc",gn,mn,dn,gk,mk,dk); 
//printw("hp-%s\n",strsql);
*****************/

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
//printw("kolstr=%d\n",kolstr);
if(kolstr == 0)
  return;
int kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(shet,row[1],0,0) != 0)
    continue;

  if((nm=kontr->find(row[2])) == -1)
   continue;

  dat->plus(nm,row[0]);
  debet[nm]+=atof(row[3]);
 }

}

