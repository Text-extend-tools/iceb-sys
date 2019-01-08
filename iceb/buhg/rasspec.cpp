/*$Id: rasspec.c,v 5.27 2013/08/13 05:49:54 sasa Exp $*/
/*09.03.2017	18.01.2000	Белых А.И.	rasspec.c
Распечатка спецификации изделия
*/
#include        <errno.h>
#include        "buhg.h"


void		rasspec(int kodiz) //Код изделия
{
SQL_str         row,row1;
char		strsql[512];
int		kolnuz;
class iceb_tu_str naim("");
char		imaf[64];
class iceb_tu_str bros("");
int		kodizr;
int		suz,zsuz;
int		kolstr;
int		kodd;

zsuz=0;
kodizr=kodiz;
class iceb_fopen fil;

sprintf(imaf,"spec%d.lst",getpid());
if(fil.start(imaf,"w") != 0)
 return;


class iceb_t_razuz_data data;
data.kod_izd=kodiz;
/*Определяем входящие узлы*/

kolnuz=razuz(&data);

/*
printw("Разузловка завершена.\n");
refresh();
*/
SQLCURSOR cur;

SQLCURSOR cur1;

naz:;


for(int metka_zap=0; metka_zap < 2; metka_zap++)
 {
  if(metka_zap == 0)
    sprintf(strsql,"select naimat from Material where kodm=%d",kodizr);
  if(metka_zap == 1)
    sprintf(strsql,"select naius from Uslugi where kodus=%d",kodizr);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    if(metka_zap == 1)
     {
      sprintf(strsql,"%s %d !",gettext("Не найден код записи"),kodizr);
      iceb_t_soob(strsql);
     }
   }  
  else
   { 
    naim.new_plus(row1[0]);
    break;
   }
 }
 
if(kodiz == kodizr)
 {
  fprintf(fil.ff,"%s %d %s\n\
--------------------------------------------------------------------\n",
  gettext("Спецификация на изделие"),
  kodizr,naim.ravno());
 }
else
 {
  fprintf(fil.ff,"\n%s %d %s\n\
--------------------------------------------------------------------\n",
  gettext("Спецификация на узел"), 
  kodizr,naim.ravno());
 }

sprintf(strsql,"select kodd,kolih,ei,kz from Specif where kodi=%d",kodizr);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return;
 }

if(kolstr == 0 )
 {
  /*В изделии нет ни одной записи*/
  printw("%s-Нет ни одной записи в изделии !!!\n%s\n",__FUNCTION__,strsql);
  OSTANOV();
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  kodd=atoi(row[0]);
  if(row[3][0] == '0')
   sprintf(strsql,"select naimat from Material where kodm=%s",row[0]);
  if(row[3][0] == '1')
   sprintf(strsql,"select naius from Uslugi where kodus=%s",row[0]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    if(row[3][0] == '0')
      sprintf(strsql,"%s-%s %d !",__FUNCTION__,gettext("Не найден код материалла"),kodd);
    if(row[3][0] == '1')
      sprintf(strsql,"%s-%s %d !",__FUNCTION__,gettext("Не найден код услуги"),kodd);

    iceb_t_soob(strsql);
    naim.new_plus("");
   }  
  else
   {
    naim.new_plus(row1[0]);
   }
  if(data.kod_uz.find(kodd) != -1)
     bros.new_plus(gettext("Узел"));
  else
     bros.new_plus("");
  fprintf(fil.ff,"%-4s %-*s %*s %*s %10.10g\n",
  row[0],
  iceb_tu_kolbait(40,naim.ravno()),naim.ravno(),
  iceb_tu_kolbait(5,row[2]),row[2],
  iceb_tu_kolbait(5,bros.ravno()),bros.ravno(),
  atof(row[1]));

 }

for(suz=zsuz ; suz < kolnuz ; suz++)
 {
  kodizr=data.kod_uz.ravno(suz);
  zsuz++;
  goto naz;
 }

podpis(fil.ff);

fil.end();

class spis_oth oth;

oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Спецификация на изделие"));

for(int nom=0; nom < oth.spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth.spis_imaf.ravno(nom),3);

iceb_t_rabfil(&oth,"");
}
