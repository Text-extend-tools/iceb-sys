/*$Id: zarvpn.c,v 5.9 2013/08/13 05:50:03 sasa Exp $*/
/*20.04.2016	18.07.2008	Белых А.И.	zarvpn.c
Подпрограмма возврата подоходного налога
*/
#include "buhg.h"

extern short kodpn;   /*Код подоходного налога*/
extern short kodvpn; /*Код возврата подоходного налога*/

void zarvpn(int tabn,
short mes,short god,
int podr,
const char *uder_only)
{
if(kodvpn == 0)
 return;
 
char strsql[1024];
class ZARP     zp;
short		d;

dpm(&d,&mes,&god,5);

zp.dz=d;
zp.mesn=mes; zp.godn=god;
zp.nomz=0;
zp.podr=podr;

sprintf(strsql,"\n%s-Возврат подоходного налога\n\
---------------------------------------------------\n",__FUNCTION__);
zar_pr_ins(strsql);

if(proverka(uder_only,kodvpn,0,0) != 0)
 {
  sprintf(strsql,"Код %d исключён из расчёта\n",kodvpn);
  zar_pr_ins(strsql);
  return;   
 }
SQL_str row;
class SQLCURSOR cur;

/*Проверяем есть ли код возврата подоходного налога*/

sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='1' and \
knah=%d",tabn,kodvpn);

if(readkey(strsql,&row,&cur) <= 0)
 {
  sprintf(strsql,"Не введено начисление возврата подоходного налога %d !\n",kodvpn);
  zar_pr_ins(strsql);
  return;
 }

class iceb_tu_str shet(row[0]);
strcpy(zp.shet,shet.ravno());

int kolstr=0;
/*Читаем сумму подоходного налога*/
sprintf(strsql,"select suma from Zarp where tabn=%d and \
datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' and \
prn='2' and knah=%d and suma <> 0.",
tabn,god,mes,god,mes,kodpn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
if(kolstr == 0)
 {
  sprintf(strsql,"Не найдено записей с подоходным налогом!\n");
  zar_pr_ins(strsql);
  return;
 }

double suma_podoh=0.;
while(cur.read_cursor(&row) != 0)
 suma_podoh+=atof(row[0]);
 
sprintf(strsql,"Сумма подоходного налога=%.2f\n",suma_podoh);
zar_pr_ins(strsql);

zapzarp(d,mes,god,tabn,1,kodvpn,suma_podoh*-1,shet.ravno(),mes,god,0,0,"",podr,"",zp);
 

}
