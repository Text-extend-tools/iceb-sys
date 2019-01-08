/* $Id: provzzag.c,v 5.6 2013/05/17 14:56:08 sasa Exp $ */
/*20.09.2006	26.05.2001	Белых А.И.	provzzag.c
Проверка записан ли стандартный набор начислений/удержаний
и запись его если он не записан
*/
#include        "buhg.h"

void        provzzag(short mp,short gp,
long tabn,
int prn,  //1-начисление 2-удержание 3-и начисление и удержание
int podr)
{
char		strsql[512];

if(prn == 1 || prn == 3)
 {
  sprintf(strsql,"select distinct tabn from Zarp where \
datz >= '%d-%d-1' and datz<='%d-%d-31' and tabn=%ld and prn='1' limit 1",
  gp,mp,gp,mp,tabn);

  if(readkey(strsql) == 0)
   zapzagot(mp,gp,tabn,1,podr);
 }

if(prn == 2 || prn == 3)
 {
  sprintf(strsql,"select distinct tabn from Zarp where \
datz >= '%d-%d-1' and datz<='%d-%d-31' and tabn=%ld and prn='2' limit 1",
  gp,mp,gp,mp,tabn);


  if(readkey(strsql) == 0)
    zapzagot(mp,gp,tabn,2,podr);
 }

}
