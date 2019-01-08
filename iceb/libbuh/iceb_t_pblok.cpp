/*$Id: iceb_t_pblok.c,v 5.3 2013/05/17 14:56:29 sasa Exp $*/
/*27.09.2010	05.09.2010	Белых А.И.	iceb_t_pblok.c
Проверка блокировки подсистемы
Если вернули 0-не заблокировано
             1-заблокировано
*/
#include "buhl.h"

int iceb_t_pblok(short mes,short god,int kodps)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select log,shet from Blok where kod=%d and god=%d and mes=%d",kodps,god,mes);
if(readkey(strsql,&row,&cur) == 0)
 {
  sprintf(strsql,"select log,shet from Blok where kod=%d and god=%d and mes=%d",
  kodps,god,0);
  if(readkey(strsql,&row,&cur) == 0)
   return(0);   
 }

if(row[1][0] != '\0') /*если заблокирован хоть один счёт - значит дата целиком незаблокирована*/
  return(0);

if(proverka(row[0],iceb_tu_getlogin(),0,1) == 0)
  return(0);  

return(1);

}
