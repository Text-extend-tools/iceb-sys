/*$Id: iceb_t_rsdatp1.c,v 5.7 2013-04-07 10:22:54 sasa Exp $*/
/*27.03.2013	09.03.2007	Белых А.И.	iceb_t_rsdatp1.c
Расшифровка двух дат
Елси вернули 0-даты введены без ошибок
             1-нет
*/
#include "buhl.h"


int iceb_t_rsdatp1(short *mn,short *gn,const char *datan,short *mk,short *gk,const char *datak)
{

*mn=*gn=*mk=*gk=0;

if(rsdat1(mn,gn,datan) != 0)
 {
  iceb_t_soob(gettext("Неправильно введена дата начала !"));
  return(1);
 }
if(datak[0] == '\0')
 {
  *mk=*mn;
  *gk=*gn;
 }
else
  if(rsdat1(mk,gk,datak) != 0)
   {
    iceb_t_soob(gettext("Неправильно введена дата конца !"));
    return(1);
   }


if(sravmydat(1,*mn,*gn,1,*mk,*gk) > 0)
 {
  iceb_t_soob(gettext("Дата начала не может быть больше даты конца !"));
  return(1);
 }



return(0);
}
