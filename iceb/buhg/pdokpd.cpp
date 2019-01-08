/*$Id: pdokpd.c,v 5.8 2013/12/31 11:48:10 sasa Exp $*/
/*15.07.2015	12.01.2010	Белых А.И.	pdokpd.c
Просмотр платёжных поручений
*/
#include <errno.h>
#include "buhg.h"
#include        "dok4.h"



int pdokpd(const char *metka,
short dd,short md,short gd,
const char *nomdok)
{
int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
char strsql[512];
char tablica[32];
FILE *ff;
char imaf[64];
class iceb_tu_str naim("");

strcpy(tablica,"Pltp");

if(SRAV(ICEB_MP_PTRE,metka,0) == 0)
 {
  strcpy(tablica,"Pltt");
 }

sprintf(strsql,"select * from %s where datd='%04d-%02d-%02d' and nomd='%s'",tablica,gd,md,dd,nomdok);

if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s\n%d.%d.%d %s %s",gettext("Не найден документ!"),
  dd,md,gd,nomdok,metka);
  iceb_t_soob(strsql);
  return(1);

 }




sprintf(imaf,"pdokpd%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
if(SRAV(ICEB_MP_PPOR,metka,0) == 0)
 fprintf(ff,"%s\n",gettext("Платёжное поручение"));
if(SRAV(ICEB_MP_PTRE,metka,0) == 0)
 fprintf(ff,"%s\n",gettext("Платёжное требование"));

fprintf(ff,"%s:%d.%d.%d %s:%s\n",
gettext("Дата документа"),dd,md,gd,
gettext("Номер документа"),nomdok);

fprintf(ff,"%s:%s\n",gettext("Записал"),iceb_t_kszap(row[12]));
fprintf(ff,"%s:%s\n",gettext("Дата записи"),iceb_tu_vremzap(row[13]));


naim.new_plus("");
if(SRAV(ICEB_MP_PPOR,metka,0) == 0)
  sprintf(strsql,"select naik from Opltp where kod='%s'",row[2]);
if(SRAV(ICEB_MP_PTRE,metka,0) == 0)
  sprintf(strsql,"select naik from Opltt where kod='%s'",row[2]);

if(readkey(strsql,&row1,&cur1) == 1)
 naim.new_plus(row1[0]);

fprintf(ff,"%s:%s %s\n",gettext("Код операции"),row[2],naim.ravno());

class iceb_tu_str kontr;
class iceb_tu_str naim_kontr;
polen(row[8],&kontr,1,'#');
polen(row[8],&naim_kontr,2,'#');

fprintf(ff,"%s:%s %s\n",gettext("Контрагент"),kontr.ravno(),naim_kontr.ravno());

fprintf(ff,"%s:%s\n",gettext("Сумма платежа"),row[3]);

fprintf(ff,"\n%s\n-------------------------------------------------------\n",
gettext("Назначение платежа"));

sprintf(strsql,"select zapis from %sz where datd='%04d-%02d-%02d' and \
nomd='%s' and mz=0 order by nz asc",tablica,gd,md,dd,nomdok);
/*printw("\n%s",strsql);*/
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(4);
 }

short i=0;

if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  i++;
  fprintf(ff,"%s\n",row[0]);
 }
podpis(ff);
fclose(ff);

prosf(imaf);

unlink(imaf);


return(0);
}
