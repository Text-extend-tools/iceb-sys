/*$Id: iceb_t_pvglkni.c,v 5.6 2013/05/17 14:56:29 sasa Exp $*/
/*11.09.2010	17.10.2008	Белых А.И.	iceb_t_pvglkni.c
Проверка блокировки подсистемы "Главная книга"
Если вернули 0 - не заблокировано
             1 - заблокировано
*/
#include "buhl.h"

int iceb_t_pvglkni(const char *data)
{
short d,m,g;
rsdat(&d,&m,&g,data,1);
return(iceb_t_pvglkni(m,g));
}

int iceb_t_pvglkni(short mes,short god)
{

if(iceb_t_pblok(mes,god,ICEB_PS_GK) != 0)
 {
  char strsql[112];
  sprintf(strsql,gettext("На дату %d.%d г. проводки заблокированы!"),mes,god);
  iceb_t_soob(strsql);
  return(1);
 }
return(0);

}
