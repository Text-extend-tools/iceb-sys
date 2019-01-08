/* $Id: prbper.c,v 5.18 2013/09/26 09:43:37 sasa Exp $ */
/*23.05.2013    27.12.1996	Белых А.И.	prbper.c
Ищем все удержания расчётного месяца исключая расчётный месяц
*/

#include        "buhg.h"

extern char	*shetbu; /*Бюджетные счета удержаний*/
extern short	*obud; /*Обязательные удержания*/

double prbper(short mn,short gn,short mk,short gk,long tabb,
double *sumab, //Сумма бюджетных удержаний
int metka) //0-смотреть все удержания до даты и после даты 1-только после даты
{
double          sum=0.;
long		kolstr;
SQL_str         row;
char		strsql[1024];
char bros[1024];
*sumab=0.;

if(metka == 0)
  sprintf(strsql,"select knah,suma,shet,datz,godn,mesn from Zarp where tabn=%ld and \
godn=%d and mesn=%d and (datz < '%d-%d-01' or datz > '%d-%02d-31') \
and datz <= '%d-%d-31' and prn='2' and suma <> 0.",
  tabb,gn,mn,gn,mn,gn,mn,gk,mk);
if(metka == 1)
  sprintf(strsql,"select knah,suma,shet,datz,godn,mesn from Zarp where tabn=%ld and \
godn=%d and mesn=%d and  datz > '%d-%02d-31' \
and datz <= '%d-%d-31' and prn='2' and suma <> 0.",
  tabb,gn,mn,gn,mn,gk,mk);

sprintf(bros,"%s-%s\n",__FUNCTION__,strsql);
zar_pr_ins(bros);

//printw("%s\n",strsql);
class SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,"__FUNCTION__",strsql);
  return(0.);
 }
//printw("kolstr=%d\n",kolstr);

if(kolstr == 0)
  return(0.);

while(cur.read_cursor(&row) != 0)
 {
  //Если это обязательные удержания то это не выплата зарплаты
  if(provkod(obud,atoi(row[0])) >= 0) 
   continue;
  sprintf(strsql,"%s-%s %s.%s %s %s %s\n",__FUNCTION__,
  iceb_tu_datzap(row[3]),row[5],row[4],row[0],row[1],row[2]);   
  zar_pr_ins(strsql);

  sum+=atof(row[1]);

  
  if(proverka(shetbu,row[2],0,0) == 0)  
     *sumab=*sumab+atof(row[1]);;

 }

if(sum != 0.)
 sum=okrug(sum,0.01);

sprintf(strsql,"%s-Cумма=%f\n",__FUNCTION__,sum); 
zar_pr_ins(strsql);

return(sum);


}
