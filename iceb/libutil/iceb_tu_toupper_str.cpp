/*$Id: iceb_tu_toupper_str.c,v 5.6 2013/05/17 14:56:41 sasa Exp $*/
/*09.10.2009	23.02.2007	Белых А.И.	iceb_tu_toupper_str.c
Перевод в строке всех букв в большие
*/
#include <ctype.h>
#include <string.h>
#include <glib.h>
void iceb_tu_toupper_str(char *stroka)
{

if(g_utf8_validate(stroka,-1,NULL) != TRUE)
 {
  for(unsigned i=0; i < strlen(stroka); i++)
   stroka[i]=toupper(stroka[i]);
 }
else
 {
  strcpy(stroka,g_utf8_strup(stroka,-1));
 }


}

