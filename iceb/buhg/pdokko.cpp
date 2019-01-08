/*$Id: pdokko.c,v 5.6 2013/12/31 11:48:10 sasa Exp $*/
/*15.07.2015	12.01.2010	Белых А.И.	pdokko.c
просмотр кассовых ордеров
*/
#include <errno.h>
#include "buhg.h"


int pdokko(short dd,short md,short gd,
int kassa,
int tipz,
const char *nomdok)
{
FILE *ff;
char imaf[64];
SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kolstr=0;
char strsql[512];
class iceb_tu_str naim("");

sprintf(strsql,"select * from Kasord where nomd='%s' and kassa=%d and tp=%d and god=%d",
nomdok,kassa,tipz,gd);
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s\n%d.%d.%d %d %s %d",gettext("Не найден документ!"),
  dd,md,gd,kassa,nomdok,tipz);
  iceb_t_soob(strsql);
  return(1);
 }

sprintf(imaf,"pdokko%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

if(tipz == 1)
 fprintf(ff,"%s\n",gettext("Приходный кассовый ордер"));
if(tipz == 2)
 fprintf(ff,"%s\n",gettext("Расходный кассовый ордер"));
 
fprintf(ff,"%s:%d.%d.%d %s:%s\n",
gettext("Дата документа"),dd,md,gd,
gettext("Номер документа"),nomdok);

fprintf(ff,"%s:%s\n",gettext("Записал"),iceb_t_kszap(row[13]));
fprintf(ff,"%s:%s\n",gettext("Дата записи"),iceb_tu_vremzap(row[14]));

naim.new_plus("");
if(tipz == 1)
  sprintf(strsql,"select naik from Kasop1 where kod='%s'",row[5]);
if(tipz == 2)
  sprintf(strsql,"select naik from Kasop2 where kod='%s'",row[5]);
if(readkey(strsql,&row1,&cur1) == 1)
 naim.new_plus(row1[0]);

fprintf(ff,"%s:%s %s\n",gettext("Код операции"),row[5],naim.ravno());

if(row[8][0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Основание"),row[8]);
if(row[9][0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Дополнение"),row[9]);
if(row[10][0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Фамилия"),row[10]);
if(row[11][0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Документ"),row[11]);
if(row[15][0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Номер бланка"),row[15]);
if(row[16][0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Номер ведомости"),row[16]);

fprintf(ff,"\
------------------------------------------------------------\n");
fprintf(ff,"\
Код к.| Наименование контрагента     |  Сумма   |Дата подт.|\n");
fprintf(ff,"\
------------------------------------------------------------\n");


sprintf(strsql,"select datp,kontr,suma,ktoi,vrem,koment from Kasord1 where kassa=%d and god=%d and tp=%d  and nomd='%s'",
kassa,gd,tipz,nomdok);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
 }
double itogo=0.;
while(cur.read_cursor(&row) != 0)
 {
  naim.new_plus("");
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[1]);
  if(readkey(strsql,&row1,&cur1) == 1)
   naim.new_plus(row1[0]);
  fprintf(ff,"%*s %-*.*s %10s %10s %s %s %s\n",
  iceb_tu_kolbait(6,row[1]),
  row[1],
  iceb_tu_kolbait(30,naim.ravno()),
  iceb_tu_kolbait(30,naim.ravno()),
  naim.ravno(),
  row[2],
  iceb_tu_datzap(row[0]),
  row[5],
  iceb_tu_vremzap(row[4]),
  iceb_t_kszap(row[3]));

  itogo+=atof(row[2]);
 }

fprintf(ff,"\
------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f\n",
iceb_tu_kolbait(37,gettext("Итого")),
gettext("Итого"),itogo);

podpis(ff);

fclose(ff);

prosf(imaf);

unlink(imaf);

return(0);

}
