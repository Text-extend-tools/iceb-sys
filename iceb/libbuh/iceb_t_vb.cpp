/*$Id: iceb_t_vb.c,v 5.4 2013/05/17 14:56:29 sasa Exp $*/
/*22.03.2010	18.03.2010	Белых А.И.	iceb_t_vb.c
Выбор базы данных
*/
#include <pwd.h>
#include "buhl.h"
extern class iceb_tu_str kodirovka_iceb;

int iceb_t_vb(const char *host,const char *parol,char **imabaz)
{
SQL_str row;
class SQLCURSOR cur;

char strsql[512];
struct  passwd  *ktor; /*Кто работает*/
ktor=getpwuid(getuid());

if(sql_openbaz(&bd,"",host,ktor->pw_name,parol) != 0)
 {
  eropbaz("mysql",ktor->pw_uid,ktor->pw_name,1);
  return(1);
 }  
readkey("select VERSION()",&row,&cur);
if(atof(row[0]) >= 4.1)
 {
  if(kodirovka_iceb.getdlinna() <= 1)
    sprintf(strsql,"SET NAMES koi8u");
  else
    sprintf(strsql,"SET NAMES %s",kodirovka_iceb.ravno_udsimv("-"));

  sql_zapis(strsql,1,0);
 }
class iceb_tu_str ima_baz("");
if(smenabaz_mysql(&ima_baz,1) != 0)
 {
  sql_closebaz(&bd);
  return(1);
 }
sql_closebaz(&bd);

(*imabaz)=new char[ima_baz.getdlinna()+1];
strcpy(*imabaz,ima_baz.ravno());


return(0);
}
