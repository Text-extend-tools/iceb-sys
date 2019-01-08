/*$Id: iceb_tu_getlogin.c,v 5.5 2013/05/17 14:56:40 sasa Exp $*/
/*10.01.2011	05.09.2010	Белых А.И.	iceb_tu_getlogin.c
Получение догина оператора
*/
#include <pwd.h>
#include "util.h"
#ifdef	HAVE_UNISTD_H
#include <unistd.h>
#endif
const char *iceb_tu_getlogin()
{
struct  passwd  *ktor; /*Кто работает*/
static class iceb_tu_str login("");
if(login.getdlinna() <= 1)
 {
  ktor=getpwuid(getuid());
  login.new_plus(ktor->pw_name);
 }
return(login.ravno());

}