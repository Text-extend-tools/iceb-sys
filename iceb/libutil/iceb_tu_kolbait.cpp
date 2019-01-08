/*$Id: iceb_tu_kolbait.c,v 5.8 2013/08/13 05:50:32 sasa Exp $*/
/*02.03.2015	30.09.2009	Белых А.И.	iceb_tu_kolbait.c
Возвращает количество байт которые занимает указанное количество символов в строке
*/
#include <glib.h>
#include <string.h>

int iceb_tu_kolbait(int kolih_simv,const char *str)
{
if(str == NULL)
 return(kolih_simv);
if(g_utf8_validate(str,-1,NULL) == TRUE) /*текст в utf8 кодировке*/
 {
  int kolprob=kolih_simv-g_utf8_strlen(str,-1);

  int razmer=strlen(str)+1;
  gchar *strout=new gchar[razmer];
  
  memset(strout,'\0',sizeof(razmer));
  g_utf8_strncpy(strout,str,kolih_simv);
  
  int kolihbait=strlen(strout);

  if(kolprob > 0)
   kolihbait+=kolprob;

  delete [] strout;

  return(kolihbait);
 }

return(kolih_simv);

}
