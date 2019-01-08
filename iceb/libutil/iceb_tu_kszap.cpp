/*$Id: iceb_tu_kszap.c,v 5.7 2013/05/17 14:56:41 sasa Exp $*/
/*15.01.2010	15.01.2010	Белых А.И.	iceb_tu_kszap.c
Расшифровка логина и формирование символьной строки с логином и фамилией оператора
*/
#include <glib.h>
#include <ctype.h>
#include        <stdio.h>
#include        <stdlib.h>
#include        <pwd.h>
#include        "util.h"


const char *iceb_tu_kszap(char *kto)
{
if(kto != NULL)
 {
  static char stroka[512];
  struct  passwd  *ktoz; /*Кто записал*/
  if(isdigit(kto[0]) != 0) /*Значит символ число*/
   return(iceb_tu_kszap(atoi(kto)));
  else
   {
    
    if((ktoz=getpwnam(kto)) != NULL)
      return(iceb_tu_kszap(ktoz->pw_uid));
    else
     {
      sprintf(stroka,"%s %s",kto,gettext("Неизвестный логин"));
      return(stroka);
     }
    
   }
 }

return("NULL");

}

/**************************************/
/**************************************/

const char *iceb_tu_kszap(int kto)
{
static char stroka[512];
memset(stroka,'\0',sizeof(stroka));
struct  passwd  *ktoz; /*Кто записал*/

if((ktoz=getpwuid(kto)) != NULL)
 {
  sprintf(stroka,"%d %.*s",kto,iceb_tu_kolbait(20,ktoz->pw_gecos),ktoz->pw_gecos);
 }
else
 {
  sprintf(stroka,"%d %s",kto,gettext("Неизвестный логин"));
 }

return(stroka);

}
