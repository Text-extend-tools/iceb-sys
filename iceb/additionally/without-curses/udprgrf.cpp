/* $Id: udprgr.c,v 5.19 2013/05/17 14:56:32 sasa Exp $ */
/*04.02.2011	14.06.1998	Белых А.И.	udprgrf.c
Удаление группы проводок принадлежащих одному документу
Если вернули 0 - проводки удалены
             1 - нет
*/
#include "bezcurses.h"


int udprgr(const char *kto,short dd,short md,short gd,const char *nn,int pod,int tipz)
{
char		strsql[512];
char		bros[512];


if(iceb_t_pbp(kto,dd,md,gd,nn,pod,tipz,gettext("Проводки удалить невозможно!")) != 0)
 return(1);

sprintf(strsql,"delete from Prov where kto='%s' and pod=%d \
and nomd='%s' and datd = '%04d-%02d-%02d' and tz=%d",
kto,pod,nn,gd,md,dd,tipz);

if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) != ER_DBACCESS_DENIED_ERROR)
   {
    sprintf(bros,"%s-%s",__FUNCTION__,gettext("Ошибка удаления группы проводок !"));
    msql_error(&bd,bros,strsql);
   }
  return(1);
 }
return(0);

}

