/*$Id: iceb_tu_adrsimv.c,v 5.5 2013/11/05 10:51:00 sasa Exp $*/
/*25.10.2013	30.09.2009	Белых А.И.	iceb_tu_adrsimv.c
Возвращает адрес символа 
*/
#include <glib.h>
#include "util.h"
const char *iceb_tu_adrsimv(int nomer_simv,const char *str)
{
static const char *kk={""};

if(str == NULL)
 return(kk);
 
if(g_utf8_validate(str,-1,NULL) == TRUE) /*текст в utf8 кодировке*/
 {
  if(iceb_tu_strlen(str) >= nomer_simv)
   return(g_utf8_offset_to_pointer(str,nomer_simv));
  else
    return(kk);
 }

if((int)strlen(str) >= nomer_simv)
  return(&str[nomer_simv]);
else
  return(kk);
}