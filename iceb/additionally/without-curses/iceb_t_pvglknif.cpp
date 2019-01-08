/*$Id: iceb_t_pvglkni.c,v 5.6 2013/05/17 14:56:29 sasa Exp $*/
/*11.09.2010	17.10.2008	Белых А.И.	iceb_t_pvglknif.c
Проверка блокировки подсистемы "Главная книга"
Если вернули 0 - не заблокировано
             1 - заблокировано
*/
#include "bezcurses.h"

int iceb_t_pvglkni(const char *data)
{
short d,m,g;
rsdat(&d,&m,&g,data,1);
return(iceb_t_pvglkni(m,g));
}
/***************************/
int iceb_t_pvglkni(short mes,short god)
{

if(iceb_t_pblok(mes,god,ICEB_PS_GK) != 0)
 {
  printf("%s-%s %d.%d %s!\n",__FUNCTION__,gettext("На дату"),mes,god,gettext("проводки заблокированы"));
  return(1);
 }
return(0);

}
