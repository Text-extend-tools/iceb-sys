/*$Id: zar_pr_start.c,v 5.2 2011-12-19 12:06:54 sasa Exp $*/
/*29.11.2011	26.11.2011	Белых А.И.	zar_start_pr.c
Начало создания протокола хода расчёта зарплаты
*/
#include <errno.h>
#include <time.h>
#include "buhg.h"


int zar_pr_nomzap=0; /*нумерация строк в протоколе*/
short zar_pr_mes=0;
short zar_pr_god=0;
int zar_pr_tabn=0;
int zar_pr_tipz=-1;
FILE *zar_pr_ffp=NULL;

int zar_pr_start(const char *imaf_prot)
{

if((zar_pr_ffp = fopen(imaf_prot,"w")) == NULL)
 {
  error_op_nfil(imaf_prot,errno,__FUNCTION__);
  return(1);
 }
return(0);
}
/**************************************************/

void zar_pr_start(int tipz,int tabn,short mr,short gr)
{
char strsql[1024];

zar_pr_nomzap=0;
zar_pr_tabn=tabn;
zar_pr_tipz=tipz;
zar_pr_mes=mr;
zar_pr_god=gr;

sprintf(strsql,"DELETE FROM Zarpr where tn=%d and tz=%d and dr='%04d-%02d-01'",tabn,tipz,gr,mr);
sql_zapis(strsql,1,0);

if(tipz == 1)
 sprintf(strsql,"\n\n%s\nПротокол хода расчёта начислений\n",iceb_t_get_pnk("00",0));
if(tipz == 2)
 sprintf(strsql,"\n\n%s\nПротокол хода расчёта удержаний\n",iceb_t_get_pnk("00",0));
if(tipz == 3)
 sprintf(strsql,"\n\n%s\nПротокол хода расчёта начислений на фонд оплаты\n",iceb_t_get_pnk("00",0));

zar_pr_ins(strsql);

sprintf(strsql,"====================================================================\n");

zar_pr_ins(strsql);

sprintf(strsql,"Месяц расчёта %02d.%d г.\n",mr,gr);
zar_pr_ins(strsql);

time_t vrem=time(NULL);
struct tm *bf;
bf=localtime(&vrem);

sprintf(strsql,"\
%s %d.%d.%d%s  %s:%02d:%02d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

zar_pr_ins(strsql);

sprintf(strsql,"Оператор:%d %s\n",iceb_t_getuid(),iceb_t_getfioop());

zar_pr_ins(strsql);

class iceb_tu_str fio("");
SQL_str row;
class SQLCURSOR cur;
sprintf(strsql,"select fio from Kartb where tabn=%d",tabn);
if(readkey(strsql,&row,&cur) == 1)
 fio.new_plus(row[0]);

sprintf(strsql,"\nТабельный номер %d %s\n",tabn,fio.ravno());

zar_pr_ins(strsql);

}
