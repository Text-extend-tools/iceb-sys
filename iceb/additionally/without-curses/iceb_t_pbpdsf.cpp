/*$Id: iceb_t_pbpds.c,v 5.10 2013/05/17 14:56:29 sasa Exp $*/
/*13.10.2012	25.03.2008	Белых А.И.	iceb_t_pbpds.c
Проверка блокировки подсистемы
Если вернули 0 - не заблокировано
             1 - заблокировано
*/

#include "bezcurses.h"

extern int iceb_kod_podsystem; /*текущий код подсистемы - устанавливается при запуске подсистемы*/

int iceb_t_pbpds(short mes,short god,int metka_soob) /*0-выдать сообщение 1-нет*/
{

if(iceb_kod_podsystem == 0)
 {
  printf("%s-%s\n",__FUNCTION__,gettext("Код подсистемы не определён!"));
  return(1);
 }

if(iceb_t_pblok(mes,god,iceb_kod_podsystem) != 0)
 {
  if(metka_soob == 0)
   {
    printf("%s %d.%d %s!\n",gettext("Дата"),mes,god,gettext("заблокирована"));
   }
  return(1);
 }

return(0);
}
/************************************/
int iceb_t_pbpds(short mes,short god)
{
return(iceb_t_pbpds(mes,god,0));
}
/********************************/

int iceb_t_pbpds(const char *data_str)
{
short d=0,m=0,g=0;
rsdat(&d,&m,&g,data_str,1);

return(iceb_t_pbpds(m,g,0));

}

