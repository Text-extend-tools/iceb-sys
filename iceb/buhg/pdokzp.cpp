/*$Id: pdokzp.c,v 5.7 2013/12/31 11:48:10 sasa Exp $*/
/*15.07.2015	13.01.2010	Белых А.И.	pdokzp.c
Просмотр начислений и удержаний в подсистеме "Заработная плата"
*/
#include <errno.h>
#include "buhg.h"


int pdokzp(short dd,short md,short gd,const char *nomdok)
{
char strsql[512];
int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
FILE *ff;
char imaf[64];
class iceb_tu_str tabnom("");
class iceb_tu_str fio("");

if(iceb_t_prr(ICEB_PS_ZP) != 0)
 return(1);


polen(nomdok,&tabnom,2,'-');
sprintf(strsql,"select fio,dolg from Kartb where tabn=%d",tabnom.ravno_atoi());
if(readkey(strsql,&row,&cur) != 1)
 {
  
  sprintf(strsql,"%s %d! %s",gettext("Не найден табельный номер"),tabnom.ravno_atoi(),nomdok);
  iceb_t_soob(strsql);
  return(1); 
 }

sprintf(imaf,"pdokzp%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }


fprintf(ff,"%s\n",gettext("Заработная плата"));
fprintf(ff,"%s:%s %s\n",gettext("Табельный номер"),tabnom.ravno(),row[0]);
fprintf(ff,"%s:%s\n",gettext("Должность"),row[1]);
fprintf(ff,"%s:%02d.%d\n",gettext("Дата начисления заработной платы"),md,gd);

sprintf(strsql,"select * from Zarp where datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' and tabn=%d \
and suma <> 0. order by prn,knah asc",gd,md,gd,md,tabnom.ravno_atoi());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
 }
fprintf(ff,"\
-------------------------------------------------------------------\n");
fprintf(ff,"\
 Код  |Наименование начисления/удерж.|  Дата    | Счёт |  Сумма   |\n");
fprintf(ff,"\
-------------------------------------------------------------------\n");
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  fio.new_plus("");  
  if(atoi(row[2]) == 1)
   sprintf(strsql,"select naik from Nash where kod=%d",atoi(row[3]));
  if(atoi(row[2]) == 2)
   sprintf(strsql,"select naik from Uder where kod=%d",atoi(row[3]));
  if(readkey(strsql,&row1,&cur1) == 1)
   fio.new_plus(row1[0]);
  fprintf(ff,"%6s %-*.*s %10s %*s %10s %s %s\n",
  row[3],
  iceb_tu_kolbait(30,fio.ravno()),
  iceb_tu_kolbait(30,fio.ravno()),
  fio.ravno(),
  iceb_tu_datzap(row[0]),
  iceb_tu_kolbait(6,row[5]),
  row[5],
  row[4],
  iceb_tu_vremzap(row[11]),
  iceb_t_kszap(row[10]));
 }
fprintf(ff,"\
-------------------------------------------------------------------\n");
podpis(ff); 
fclose(ff);

prosf(imaf);

unlink(imaf);

return(0);
}
