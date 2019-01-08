/*$Id: iceb_t_soob.c,v 5.7 2013/05/17 14:56:29 sasa Exp $*/
/*11.04.2012	18.11.2008	Белых А.И.	iceb_t_soob.c
Выдача сообщения на экран
*/
#include "buhl.h"

int iceb_t_soob(const char *soob,int metka_nlk, /*0-вывести "нажмите любую клавишу" 1-нет*/
int ten,WINDOW *win) /*0-без тени 1-с тенью*/
{
class SPISOK zagol;
int kolpol=pole2(soob,'\n');
if(kolpol == 0)
 zagol.plus(soob);
else
 for(int kk=0; kk < kolpol; kk++)
  if(polen(soob,&zagol,kk+1,'\n') != 0)
   break;


VVOD SOOB(1);
for(int kk=0; kk < zagol.kolih(); kk++)
  SOOB.VVOD_SPISOK_add_MD(zagol.ravno(kk));

return(soobshw(&SOOB,win,-1,-1,metka_nlk,ten));


}
/****************************************/

int iceb_t_soob(const char *soob)
{
return(iceb_t_soob(soob,0,1));
}
/********************************/
int iceb_t_soob(const char *soob,int metka_nlk, /*0-вывести "нажмите любую клавишу" 1-нет*/
int ten) /*0-без тени 1-с тенью*/
{

return(iceb_t_soob(soob,metka_nlk,ten,stdscr));

}

/*****************************/

int iceb_t_soob(const char *soob,WINDOW *win)
{
return(iceb_t_soob(soob,0,1,win));
}