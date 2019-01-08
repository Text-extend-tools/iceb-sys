/*$Id: rasizdel.c,v 5.19 2013-04-07 10:22:43 sasa Exp $*/
/*15.07.2015	19.01.2000	Белых А.И.	rasizdel.c
Распечатка списка всех введенных изделий
*/
#include        <errno.h>
#include        "buhg.h"


void		rasizdel()
{
FILE		*ff;
char		imaf[64];
SQL_str         row,row1;
char		strsql[1024];
int		kolstr;
class iceb_tu_str naim("");


sprintf(strsql,"select distinct kodi from Specif");
SQLCURSOR cur;
SQLCURSOR cur1;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0 )
  return;

sprintf(imaf,"izdel%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

fprintf(ff,"%s\n",gettext("Список всех введенных изделий."));
fprintf(ff,"-----------------------------------------------------\n");

while(cur.read_cursor(&row) != 0)
 {
  naim.new_plus("");
  sprintf(strsql,"select naimat from Material where kodm=%s",row[0]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код материалла"),row[0]);
    iceb_t_soob(strsql);
   }  
  else
   naim.new_plus(row1[0]);
  fprintf(ff,"%-4s %s\n",row[0],naim.ravno());
 }
podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Список всех введенных изделий"));
iceb_t_ustpeh(imaf,0);
iceb_t_rabfil(&oth,"");

}
