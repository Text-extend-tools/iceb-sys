/*$Id: iceb_t_prr.c,v 5.4 2013/05/17 14:56:29 sasa Exp $*/
/*17.09.2010	05.09.2010	Белых А.И.	iceb_t_prr.c
Проверка на возможность работать с подсистемой
*/
#include "buhl.h"

extern int iceb_kod_podsystem;
int iceb_t_prr()
{
 return(iceb_t_prr(iceb_kod_podsystem));
}
/**********************************/

int iceb_t_prr(int podsystem)
{

char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
char name_kol[32];
memset(name_kol,'\0',sizeof(name_kol));

switch(podsystem)
 {
  case ICEB_PS_GK:
   strcpy(name_kol,"gkd");
   break;

  case ICEB_PS_MU:
   strcpy(name_kol,"mud");
   break;

  case ICEB_PS_PD:
   strcpy(name_kol,"pdd");
   break;

  case ICEB_PS_ZP:
   strcpy(name_kol,"zpd");
   break;

  case ICEB_PS_UOS:
   strcpy(name_kol,"osd");
   break;

  case ICEB_PS_UU:
   strcpy(name_kol,"uud");
   break;

  case ICEB_PS_UKO:
   strcpy(name_kol,"kod");
   break;

  case ICEB_PS_UKR:
   strcpy(name_kol,"krd");
   break;

  case ICEB_PS_UPL:
   strcpy(name_kol,"pld");
   break;

  case ICEB_PS_RNN:
   strcpy(name_kol,"nnd");
   break;

  case ICEB_PS_UD:
   strcpy(name_kol,"udd");
   break;

  case ICEB_PS_ABD:
   return(0);

 };

if(name_kol[0] == '\0')
 {
  sprintf(strsql,"%s-%s",__FUNCTION__,gettext("Код подсистемы не определён!"));
  iceb_t_soob(strsql);
  return(1);

 }
  
sprintf(strsql,"select %s from icebuser where login='%s'",name_kol,iceb_tu_getlogin());

if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s\n%s\n%s %s",
  iceb_t_getnps(),
  gettext("У Вас нет разрешения для работы с этой подсистемой!"),
  gettext("Ненайден логин"),
  iceb_tu_getlogin());

  iceb_t_soob(strsql);

  return(1);
  
 }
if(atoi(row[0]) == 1)
 {
  sprintf(strsql,"%s\n%s",
  iceb_t_getnps(),
  gettext("У Вас нет разрешения для работы с этой подсистемой!"));
  iceb_t_soob(strsql);
  return(1);
  
 }


return(0);



}
