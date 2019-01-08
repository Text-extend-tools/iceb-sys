/*$Id: iceb_t_prn.c,v 5.4 2013/05/17 14:56:29 sasa Exp $*/
/*17.09.2010	08.09.2010	Белых А.И.	iceb_t_prn.c
Проверка на возможность изменять настройки подсистемы
*/
#include "buhl.h"

extern int iceb_kod_podsystem;
int iceb_t_prn()
{
 return(iceb_t_prn(iceb_kod_podsystem));
}
/**********************************/

int iceb_t_prn(int podsystem)
{

char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
char name_kol[32];
memset(name_kol,'\0',sizeof(name_kol));

switch(podsystem)
 {
  case ICEB_PS_GK:
   strcpy(name_kol,"gkn");
   break;

  case ICEB_PS_MU:
   strcpy(name_kol,"mun");
   break;

  case ICEB_PS_PD:
   strcpy(name_kol,"pdn");
   break;

  case ICEB_PS_ZP:
   strcpy(name_kol,"zpn");
   break;

  case ICEB_PS_UOS:
   strcpy(name_kol,"osn");
   break;

  case ICEB_PS_UU:
   strcpy(name_kol,"uun");
   break;

  case ICEB_PS_UKO:
   strcpy(name_kol,"kon");
   break;

  case ICEB_PS_UKR:
   strcpy(name_kol,"krn");
   break;

  case ICEB_PS_UPL:
   strcpy(name_kol,"pln");
   break;

  case ICEB_PS_RNN:
   strcpy(name_kol,"nnn");
   break;

  case ICEB_PS_UD:
   strcpy(name_kol,"udn");
   break;

  case ICEB_PS_ABD:
   return(0);

 };

if(name_kol[0] == '\0')
 {
  sprintf(strsql,"%s-%s",__FUNCTION__,gettext("Код подсистемы не определён!"));
  iceb_t_soob(strsql);
  return(0);

 }
  
sprintf(strsql,"select %s from icebuser where login='%s'",name_kol,iceb_tu_getlogin());
//printw("%s-%s\n",__FUNCTION__,strsql);
//OSTANOV();
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s\n%s\n%s %s",
  iceb_t_getnps(),
  gettext("У Вас нет разрешения для корректировки настроек!"),
  gettext("Ненайден логин"),
  iceb_tu_getlogin());

  iceb_t_soob(strsql);

  return(1);
  
 }

if(atoi(row[0]) == 1)
 {
  sprintf(strsql,"%s\n%s",
  iceb_t_getnps(),
  gettext("У Вас нет разрешения для корректировки настроек!"));
  iceb_t_soob(strsql);
  return(1);
  
 }


return(0);



}
