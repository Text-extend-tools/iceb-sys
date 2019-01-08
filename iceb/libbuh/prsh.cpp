/* $Id: prsh.c,v 5.15 2013/09/26 09:43:55 sasa Exp $ */
/*09.03.2016    16.11.1993      Белых А.И.      prsh.c
Подпрограмма проверки наличия счета
Если вернули 0 - счета нет
	     1 - счёт есть и не имеет субсчетов
	     2 - счёт есть и имеет субсчета
	     3 - счёт является субсчетом
             4 - счёт заблокирован в плане счетов
*/
#include	"buhl.h"


int prsh(const char *ssh, //Номер счета
OPSHET *shetv) //Возвращаем тип сальдо
{
char		strsql[512];
SQL_str         row;
int		kolstr;
class iceb_tu_str shet("");
short	vplsh=0; /*0-двух уровневый план счетов 1-многоуровневый*/

if(iceb_t_poldan("Многопорядковый план счетов",strsql,"nastrb.alx") == 0)
 if(SRAV("Вкл",strsql,1) == 0)
  vplsh=1;

shet.new_plus(ssh);

class SQLCURSOR cur;
sprintf(strsql,"select * from Plansh where ns like '%s%%' \
order by ns asc",shet.ravno_filtr());
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return(-1);
 }


if(kolstr <= 0)
  return(0);

cur.read_cursor(&row);

if(atoi(row[10]) == 1) /*счёт заблокирован в плане счетов*/
 return(4);

strcpy(shetv->naim,row[1]);
shetv->tips=atoi(row[2]);
shetv->vids=atoi(row[3]);
shetv->saldo=atoi(row[6]);
shet.new_plus(row[0]);
shetv->stat=atoi(row[7]);
shetv->val=atoi(row[8]);
shetv->ksb=atoi(row[9]);

if(kolstr == 1 && SRAV(ssh,shet.ravno(),0) != 0)
  return(0);

if(kolstr == 1 && SRAV(ssh,shet.ravno(),0) == 0 && shetv->vids == 0)
   return(1);

/*Для двух уровневой системы счетов*/
if(vplsh == 0)
 if(kolstr >= 1 && SRAV(ssh,shet.ravno(),0) == 0 && shetv->vids == 1)
    return(3);

/*Многоуровневый план счетов*/
if(vplsh == 1)
 {
  if(kolstr == 1 && SRAV(ssh,shet.ravno(),0) == 0 && shetv->vids == 1)
      return(3);
  if(kolstr > 1 && SRAV(ssh,shet.ravno(),0) == 0 && shetv->vids == 1)
      return(2);
 }
 
if(kolstr > 1 && SRAV(ssh,shet.ravno(),0) == 0)
 return(2);


return(0);
}
