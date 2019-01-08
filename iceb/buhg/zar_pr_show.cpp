/*$Id: zar_pr_show.c,v 5.3 2013/05/17 14:56:17 sasa Exp $*/
/*27.11.2011	26.11.2011	Белых А.И.	zar_pr_show.c
Просмотр протокола расчёта заработной платы
*/
#include <errno.h>
#include "buhg.h"

void zar_pr_show(int tipz,int tabn,short mr,short gr)
{
FILE *ff=NULL;
char imaf[64];
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
int kolstr=0;
int metka_fopen=0;
sprintf(imaf,"zarpr%d.lst",getpid());

if(tabn == 0)
 {
  if(tipz != 0)
    sprintf(strsql,"select sz from Zarpr where tz=%d and dr='%04d-%02d-01' order by tn asc,tz asc, nz asc",tipz,gr,mr);
  else
    sprintf(strsql,"select sz from Zarpr where dr='%04d-%02d-01' order by tn asc,tz asc, nz asc",gr,mr);
 }
else
 {
  sprintf(strsql,"select sz from Zarpr where tn=%d and tz=%d and dr='%04d-%02d-01' order by tz asc, nz asc",0,0,gr,mr);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return;
   }
  
  if(kolstr > 0)
   {
    if((ff = fopen(imaf,"w")) == NULL)
     {
      error_op_nfil(imaf,errno,"");
      return;
     }
    metka_fopen=1;
    while(cur.read_cursor(&row) != 0)
     {
      fprintf(ff,"%s",row[0]);
     }
   }

  sprintf(strsql,"select sz from Zarpr where tn=%d and tz=%d and dr='%04d-%02d-01' order by tz asc, nz asc",tabn,tipz,gr,mr);
 }


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  if(metka_fopen > 0)
   fclose(ff);
  return;
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Протокол хода расчёта не найден!"));
  if(metka_fopen > 0)
   fclose(ff);
  return;
 }






if(metka_fopen == 0)
 {
  if((ff = fopen(imaf,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return;
   }
 }
 
while(cur.read_cursor(&row) != 0)
 {
  fprintf(ff,"%s",row[0]);
 }

podpis(ff);
fclose(ff);
class spis_oth oth;
oth.spis_imaf.plus(imaf);

switch(tipz)
 {
  case 1:
    oth.spis_naim.plus(gettext("Протокол расчёта начислений"));
    break;

  case 2:
    oth.spis_naim.plus(gettext("Протокол расчёта удержаний"));
    break;

  case 3:
    oth.spis_naim.plus(gettext("Протокол расчёта coц. начислений на зарплату"));
    break;

  default:
    oth.spis_naim.plus(gettext("Протокол расчёта зарплаты"));
    break;    
 }

iceb_t_ustpeh(imaf,3);

iceb_t_rabfil(&oth,"");


}
