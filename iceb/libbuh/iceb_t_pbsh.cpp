/*$Id: iceb_t_pbsh.c,v 5.10 2013/05/17 14:56:29 sasa Exp $*/
/*11.03.2016	27.03.2008	Белых А.И.	iceb_t_pbsh.c
Проверка блокировки счёта
Если вернули 0- счёт не заблокирован
             1- заблокирован
*/
#include "buhl.h"

int iceb_t_pbsh(short mes,short god,const char *shet,const char *shetkor,const char *repl)
{
SQL_str row;
char strsql[1024];
class SQLCURSOR cur;

sprintf(strsql,"select bs from Plansh where ns='%s'",shet);
if(readkey(strsql,&row,&cur) == 1)
 {
  if(atoi(row[0]) == 1)
   {
    sprintf(strsql,"%s %s %s!",gettext("Счёт"),shet,gettext("заблокирован в плане счетов"));
    iceb_t_soob(strsql);
    return(1);
   }
 }
sprintf(strsql,"select bs from Plansh where ns='%s'",shetkor);
if(readkey(strsql,&row,&cur) == 1)
 {
  if(atoi(row[0]) == 1)
   {
    sprintf(strsql,"%s %s %s!",gettext("Счёт"),shetkor,gettext("заблокирован в плане счетов"));
    iceb_t_soob(strsql);
    return(1);
   }
 }

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
    sprintf(strsql,gettext("Счёт %s на %d.%d г. заблокирован!"),shet,mes,god);

    VVOD SOOB(1);
    if(repl[0] != '\0')
      SOOB.VVOD_SPISOK_add_MD(repl);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    return(1);
  
  }

if(shetkor[0] != '\0')
 if(proverka(row[1],shetkor,1,1) == 0)
  {
    sprintf(strsql,gettext("Счёт %s на %d.%d г. заблокирован!"),shetkor,mes,god);

    VVOD SOOB(1);
    if(repl[0] != '\0')
      SOOB.VVOD_SPISOK_add_MD(repl);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    return(1);
  
  }


return(0);
}
