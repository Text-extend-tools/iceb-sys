/*$Id: dok_end_usl.c,v 5.9 2013/08/13 05:49:47 sasa Exp $*/
/*16.04.2017	16.12.2005	Белых А.И.	dok_end_usl.c
Распечатка концовки документов
*/
#include "buhg.h"


double dok_end_usl(int tipz,
short dg,short mg,short gg,
const char *nomdok,
int podr,
int dlina,double sumkor,double itogo,float pnds,int lnds,float procent,
const char *naimnal,
FILE *ff)
{
double bb=0.;
char strsql[512];

if(sumkor != 0)
 {
  bb=sumkor*100./itogo;
  bb=okrug(bb,0.01);
  if(sumkor < 0)
    sprintf(strsql,"%s %.6g%% :",gettext("Снижка"),bb*(-1));
  if(sumkor > 0)
    sprintf(strsql,"%s %.6g%% :",gettext("Надбавка"),bb);

  fprintf(ff,"%*s%10.2f/З %s %.2f\n",iceb_tu_kolbait(dlina+1,strsql),
  strsql,sumkor,gettext("НДС"),sumkor+sumkor*pnds/100.);

  if(sumkor < 0)
    sprintf(strsql,gettext("Итого со снижкой :"));
  if(sumkor > 0)
    sprintf(strsql,gettext("Итого с надбавкой :"));

  fprintf(ff,"%*s%10.2f\n",iceb_tu_kolbait(dlina+1,strsql),strsql,sumkor+itogo);

 }

double bbproc=0.;
if(procent != 0.)
 {
  sprintf(strsql,"%s %.2f%% :",naimnal,procent);
  bbproc=(itogo+sumkor)*procent/100.;
  bbproc=okrug(bbproc,0.01);
  fprintf(ff,"%*s%10.2f\n",iceb_tu_kolbait(dlina+1,strsql),strsql,bbproc);

 }

bb=0.;

if(lnds == 0 )
 {
  double sumandspr=0.;
  if(tipz == 1)
   {
    SQL_str row;
    SQLCURSOR cur;
    sprintf(strsql,"select sumnds from Usldokum where \
datd='%04d-%d-%d' and podr=%d and nomd='%s' and tp=%d",
    gg,mg,dg,podr,nomdok,tipz);
    if(sql_readkey(&bd,strsql,&row,&cur) == 1)
      sumandspr=atof(row[0]);
   }

  sprintf(strsql,"%s %.2f%% :",gettext("НДС"),pnds);
  if(sumandspr != 0.)
    bb=sumandspr;
  else        
    bb=(itogo+sumkor)*pnds/100.;
  bb=okrug(bb,0.01);
  fprintf(ff,"%*s%10.2f\n",iceb_tu_kolbait(dlina+1,strsql),strsql,bb);
 }

double itogonds=(itogo+sumkor)+bb+ bbproc;
strcpy(strsql,gettext("Итого к оплате:"));
if(tipz == 1)
  strcpy(strsql,gettext("Итого:"));
fprintf(ff,"%*s%10.2f\n",iceb_tu_kolbait(dlina+1,strsql),strsql,itogonds);

return(itogonds);

}
