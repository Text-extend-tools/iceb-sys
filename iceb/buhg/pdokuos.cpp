/*$Id: pdokuos.c,v 5.6 2013/12/31 11:48:10 sasa Exp $*/
/*15.07.2015	12.01.2009	Белых А.И.	pdokuos.c
Просмотр документа из подсистемы "Учёт основных средств"
*/
#include <errno.h>
#include "buhg.h"

extern double	okrcn;  /*Округление цены*/


int pdokuos(short dd,short md,short gd,const char *nomdok,int tipz)
{
char strsql[512];
int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
FILE *ff;
char imaf[64];
class iceb_tu_str naim("");

sprintf(strsql,"select * from Uosdok where datd='%04d-%02d-%02d' and nomd='%s' and tipz=%d",
gd,md,dd,nomdok,tipz);

if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s\n%d.%d.%d %s %d",gettext("Не найден документ!"),
  dd,md,gd,nomdok,tipz);
  iceb_t_soob(strsql);
  return(1);
 }

sprintf(imaf,"pdokuos%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
fprintf(ff,"%s\n",gettext("Учёт основных средств"));
if(tipz == 1)
 fprintf(ff,"%s\n",gettext("Приход"));
if(tipz == 2)
 fprintf(ff,"%s\n",gettext("Расход"));
fprintf(ff,"%s:%d.%d.%d %s:%s\n",
gettext("Дата документа"),dd,md,gd,
gettext("Номер документа"),nomdok);

naim.new_plus("");
sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[3]);
if(readkey(strsql,&row1,&cur1) == 1)
 naim.new_plus(row1[0]);

fprintf(ff,"%s:%s %s\n",gettext("Контрагент"),row[3],naim.ravno());

naim.new_plus("");
if(tipz == 1)
  sprintf(strsql,"select naik from Uospri where kod='%s'",row[2]);
if(tipz == 2)
  sprintf(strsql,"select naik from Uosras where kod='%s'",row[2]);
if(readkey(strsql,&row1,&cur1) == 1)
 naim.new_plus(row1[0]);

fprintf(ff,"%s:%s %s\n",gettext("Код операции"),row[2],naim.ravno());

fprintf(ff,"%s:%s\n",gettext("Записал"),iceb_t_kszap(row[13]));
fprintf(ff,"%s:%s\n",gettext("Дата записи"),iceb_tu_vremzap(row[14]));

 
if(atoi(row[25]) == 0)
 fprintf(ff,"%s\n",gettext("Документ неоплачен"));
if(atoi(row[25]) == 1)
 fprintf(ff,"%s\n",gettext("Документ оплачен"));

naim.new_plus("");
sprintf(strsql,"select naik from Uospod where kod=%s",row[5]);
if(readkey(strsql,&row1,&cur1) == 1)
 naim.new_plus(row1[0]);

fprintf(ff,"%s:%s %s\n",gettext("Подразделение"),row[5],naim.ravno());

naim.new_plus("");
sprintf(strsql,"select naik from Uosol where kod=%s",row[6]);
if(readkey(strsql,&row1,&cur1) == 1)
 naim.new_plus(row1[0]);

fprintf(ff,"%s:%s %s\n",gettext("Материально-ответственный"),row[6],naim.ravno());
fprintf(ff,"%s:%s\n",gettext("Основание"),row[12]);

if(row[15][0] != '\0')
 {
  fprintf(ff,"%s:%s ",gettext("Доверенность"),row[15]);
  fprintf(ff,"%s:%s\n",gettext("Дата"),iceb_tu_datzap(row[16]));
 }
if(row[20][0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Через кого"),row[20]);

if(row[17][0] != '\0')
 {
  naim.new_plus("");       
  sprintf(strsql,"select naik from Foroplat where kod='%s'",row[17]);
  if(readkey(strsql,&row1,&cur1) == 1)
   naim.new_plus(row1[0]);
  fprintf(ff,"%s:%s %s\n",gettext("Форма оплаты"),row[17],naim.ravno());
 }
if(row[18][0] != '0')
 fprintf(ff,"%s:%s\n",gettext("Дата получения налоговой накладной"),iceb_tu_datzap(row[18]));
if(row[19][0] != '0')
 fprintf(ff,"%s:%s\n",gettext("Номер налоговой накладной"),row[19]);

if(row[21][0] != '0')
 fprintf(ff,"%s:%s\n",gettext("Дата оплаты"),iceb_tu_datzap(row[21]));
/********
int mnds=atoi(row[22]);
float pnds=atof(row[23]);
**********/ 
fprintf(ff,"\
--------------------------------------------------------------------------------------\n");
fprintf(ff,"\
Инвентарн.|       Наименование           |   Налоговый учёт    |Бухгалтерский учёт   |\n\
  номер   |                              |Бал.стоим.|  Износ   |Бал.стоим.|  Износ   |\n");
fprintf(ff,"\
--------------------------------------------------------------------------------------\n");

sprintf(strsql,"select innom,bs,iz,ktoz,vrem,bsby,izby from Uosdok1 where datd='%04d-%02d-%02d' and nomd='%s'",gd,md,dd,nomdok);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
 }
double itogo[4];
memset(itogo,'\0',sizeof(itogo));

while(cur.read_cursor(&row) != 0)
 {
  naim.new_plus("");
  sprintf(strsql,"select naim from Uosin where innom=%s",row[0]);
  if(readkey(strsql,&row1,&cur1) == 1)
   naim.new_plus(row1[0]);

  fprintf(ff,"%10s %-*.*s %10s %10s %10s %10s %s %s\n",
  row[0],
  iceb_tu_kolbait(30,naim.ravno()),
  iceb_tu_kolbait(30,naim.ravno()),
  naim.ravno(),
  row[1],row[2],row[5],row[6],
  iceb_tu_vremzap(row[4]),
  iceb_t_kszap(row[3]));

  if(iceb_tu_strlen(naim.ravno()) > 30)
   fprintf(ff,"%10s %s\n","",iceb_tu_adrsimv(30,naim.ravno()));
  itogo[0]+=atof(row[1]);
  itogo[1]+=atof(row[2]);
  itogo[2]+=atof(row[5]);
  itogo[3]+=atof(row[6]);
 }

fprintf(ff,"\
--------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f %10.2f %10.2f %10.2f\n",
iceb_tu_kolbait(41,gettext("Итого")),
gettext("Итого"),
itogo[0],itogo[1],itogo[2],itogo[3]);
podpis(ff);
fclose(ff);

prosf(imaf);

unlink(imaf);

return(0);

}
