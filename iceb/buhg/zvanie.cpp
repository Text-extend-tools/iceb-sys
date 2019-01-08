/* $Id: zvanie.c,v 5.21 2013/09/26 09:43:46 sasa Exp $ */
/*20.04.2016    02.12.1992      Белых А.И.      zvanie.c
Подпрограмма начисления за звание
*/
#include        <errno.h>
#include        "buhg.h"


void zvanie(int tn,//Табельный номер
short mp,short gp, //Дата расчёта
int podr, //Код подразделения
const char *nah_only)
{
int             knzz; /*Код начисления за звание*/
double          nah=0.;
short           d=0;
class iceb_tu_str shet("");
char		strsql[1024];
int		zvan=0;
class ZARP     zp;
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"\n%s-Расчёт начисления за завание\n\
----------------------------------------------------------\n",__FUNCTION__);
zar_pr_ins(strsql); 

if(iceb_t_poldan("Код начисления за звание",strsql,"zarnast.alx") != 0)
 {
  sprintf(strsql,"%s-Не найдена настройка \"Код начисления за звание\"!\n",__FUNCTION__);
  zar_pr_ins(strsql); 
  return;
 }
knzz=atoi(strsql);
if(knzz == 0)
 {
  sprintf(strsql,"%s-Код начисления за звание равен нулю!\n",__FUNCTION__);
  zar_pr_ins(strsql); 
  return;
 }

if(proverka(nah_only,knzz,0,0) != 0)
 {
  sprintf(strsql,"%s-Код начисления за звание исключён из расчёта %d",__FUNCTION__,knzz);
  zar_pr_ins(strsql); 
  return;
 }

sprintf(strsql,"Код начисления за звание=%d\n",knzz);
zar_pr_ins(strsql); 
 
/*Проверяем есть ли код начисления в списке*/
sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='1' and knah=%d",tn,knzz); 
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s-Не нашли код начисления за звание %d в списке начислений\n",__FUNCTION__,knzz);
  zar_pr_ins(strsql); 
  return;
 }
shet.new_plus(row[0]);
//Читаем код звания
sprintf(strsql,"select zvan from Kartb where tabn=%d",tn); 
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s-Не найден код звания для %d\n",__FUNCTION__,tn);
  zar_pr_ins(strsql); 
  return;
 }
zvan=atoi(row[0]);

sprintf(strsql,"Код звания=%d\n",zvan);
zar_pr_ins(strsql); 

nah=0.;

sprintf(strsql,"select sdzz from Zvan where kod=%d",zvan);
if(readkey(strsql,&row,&cur) == 1)
 nah=atof(row[0]);

sprintf(strsql,"Сумма доплаты за звание = %.2f\n",nah);
zar_pr_ins(strsql); 

dpm(&d,&mp,&gp,5);
zp.dz=d;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;
strcpy(zp.shet,shet.ravno());
zapzarp(d,mp,gp,tn,1,knzz,nah,shet.ravno(),mp,gp,0,0,"",podr,"",zp); 

}
