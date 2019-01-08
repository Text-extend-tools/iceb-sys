/*$Id: podpis.c,v 5.12 2013/09/26 09:43:55 sasa Exp $*/
/*17.02.2010	09.10.2000	Белых А.И.	podpis.c
Подпись в отчётах
*/
#include	"buhl.h"


void	podpis(FILE *ff)
{

fprintf(ff,"\n\n%s \"iceB\" %s %s",
gettext("Система"),
gettext("Версия"),VERSION);

char bros[512];
memset(bros,'\0',sizeof(bros));

if(iceb_t_poldan("Печатать фамилию оператора в распечатках отчётов",bros,"nastsys.alx") == 0)
 {
  if(SRAV(bros,"Вкл",1) == 0)
   {
   
    class iceb_tu_str fioop(iceb_t_getfioop()); /*нужно так - по другому сбоит*/

    fprintf(ff," %s:%d %.*s",
    gettext("Распечатал"),
    iceb_t_getuid(),
    iceb_tu_kolbait(30,fioop.ravno()),fioop.ravno());
   }
 }
fprintf(ff,"\n");

}
