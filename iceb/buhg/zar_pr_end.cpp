/*$Id: zar_pr_end.c,v 5.3 2013/05/17 14:56:16 sasa Exp $*/
/*29.11.2011	29.11.2011	Белых А.И.	zar_pr_end.c
Завершение протокола расчёта
*/
#include "buhg.h"

void zar_pr_end()
{

extern int zar_pr_nomzap;
extern short zar_pr_mes;
extern short zar_pr_god;
extern int zar_pr_tabn;
extern int zar_pr_tipz;
extern FILE *zar_pr_ffp;

if(zar_pr_tipz > 0)
 {
  zar_pr_nomzap=0;
  zar_pr_tabn=0;
  zar_pr_tipz=-1;
  zar_pr_mes=0;
  zar_pr_god=0;
 }
 
if(zar_pr_ffp != NULL)
 {

  podpis(zar_pr_ffp);
  fclose(zar_pr_ffp);
  zar_pr_ffp=NULL;

 }

}
