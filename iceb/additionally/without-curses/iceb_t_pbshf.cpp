/*$Id: iceb_t_pbsh.c,v 5.10 2013/05/17 14:56:29 sasa Exp $*/
/*10.01.2011	27.03.2008	Белых А.И.	iceb_t_pbshf.c
Проверка блокировки счёта
Если вернули 0- счёт не заблокирован
             1- заблокирован
*/
//#include <pwd.h>
#include <errno.h>
#include "bezcurses.h"
//#include <unistd.h>

int iceb_t_pbsh(short mes,short god,const char *shet,const char *shetkor,const char *repl)
{
SQL_str row;
char strsql[1024];
class SQLCURSOR cur;

sprintf(strsql,"select log,shet from Blok where kod=%d and god=%d and mes=%d",ICEB_PS_GK,god,mes);
if(readkey(strsql,&row,&cur) == 0)
 {
  sprintf(strsql,"select log,shet from Blok where kod=%d and god=%d and mes=0",ICEB_PS_GK,god);
  if(readkey(strsql,&row,&cur) == 0)
   return(0);
 }

if(proverka(row[0],iceb_tu_getlogin(),0,1) == 0)
 return(0);

if(shet[0] != '\0')
 if(proverka(row[1],shet,1,1) == 0)
  {
    printf("%s %s %s %d.%d %s!\n",gettext("Счёт"),shet,gettext("на"),mes,god,gettext("заблокирован"));

    if(repl[0] != '\0')
      printf("%s\n",repl);
    return(1);
  
  }

if(shetkor[0] != '\0')
 if(proverka(row[1],shetkor,1,1) == 0)
  {
    printf("%s %s %s %d.%d %s!\n",gettext("Счёт"),shetkor,gettext("на"),mes,god,gettext("заблокирован"));

    if(repl[0] != '\0')
      printf("%s\n",repl);

    return(1);
  
  }


return(0);
}
