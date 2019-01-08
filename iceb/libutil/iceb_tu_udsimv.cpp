/*$Id: iceb_tu_udsimv.c,v 5.6 2013/05/17 14:56:41 sasa Exp $*/
/*29.01.2012	10.11.2009	Белых А.И.	iceb_tu_udsimv.c
Удаление символа из строки
*/
#include <glib.h>
#include "util.h"

void iceb_tu_udsimv(char *stroka,int nomer_us)
{
if(iceb_tu_strlen(stroka) <= nomer_us) /*попытка удалить символ за концом строки*/
  return;
if(g_utf8_validate(stroka,-1,NULL) == TRUE) /*текст в utf8 кодировке*/
 {
  size_t dlina=strlen(stroka)+1;
  
  wchar_t strutf8[dlina];
  wchar_t strutf8cop[dlina];

  memset(strutf8cop,'\0',sizeof(strutf8cop));
  if((mbstowcs(strutf8,stroka,dlina)) == (size_t)-1)
   {
    printf("\n%s-Ошибка преобразования в wchar_t строку nomer_us=%d\n",__FUNCTION__,nomer_us);
    return;
   }
  for(unsigned int nomer=0; nomer < dlina-1; nomer++)
   {
    if(nomer >= (unsigned)nomer_us)
     {
      strutf8cop[nomer]= strutf8[nomer+1];
      continue;
     }
    strutf8cop[nomer]= strutf8[nomer];
   }  

  if((wcstombs(stroka,strutf8cop,dlina)) == (size_t)-1)
    printf("\n%s-Ошибка преобразования в char строку nomer_us=%d\n",__FUNCTION__,nomer_us);
   
 }
else
  for(int nomer=nomer_us; nomer< (int)strlen(stroka);nomer++)
      stroka[nomer]=stroka[nomer+1];



}
