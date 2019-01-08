/* $Id: podoh1.c,v 5.62 2013/09/26 09:43:37 sasa Exp $ */
/*02.09.2016    01.12.1992      Белых А.И.      podoh1.c
Подпрограмма расчёта подоходного налога
*/
#include        "buhg.h"


extern short	metkarnb; //0-Расчёт налогов для бюджета пропорциональный 1-последовательный
extern char	*shetb; /*Бюджетные счета*/

void  podoh1(int tn, //Табельный номер*/
short mp,short gp,
int podr,
const char *uder_only)
{
char strsql[1024];


if(sravmydat(1,mp,gp,ICEB_DNDESV,ICEB_MNDESV,ICEB_GNDESV) < 0)
 {
  sprintf(strsql,"%s-Расчёт удержаний до %d.%d.%d г. не выполняется!\n",__FUNCTION__,ICEB_DNDESV,ICEB_MNDESV,ICEB_GNDESV);
  zar_pr_ins(strsql);
    
 }
else
 {
  if(metkarnb == 1 && shetb != NULL) /*Последовательный расчёт бюджет/хозрасчёт*/
   {
    sprintf(strsql,"%s-Последовательный расчёт подоходного налога для бюджетных начислений\n",__FUNCTION__);
    zar_pr_ins(strsql);
   }
  if(metkarnb == 0 && shetb != NULL) /*Пропрорциональный расчёт*/
   {
    sprintf(strsql,"%s-Пропорциональный расчёт подоходного налога для бюджетных начислений\n",__FUNCTION__);
    zar_pr_ins(strsql);
   }

  //расчёт подоходного налога
  ras_podoh1(tn,mp,gp,podr,0,0.,uder_only);
 }


return;
}
