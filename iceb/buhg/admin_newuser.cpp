/*$Id: admin_newuser.c,v 5.5 2013/05/17 14:55:55 sasa Exp $*/
/*13.07.2015	03.09.2010	Белых А.И.	admin_newuser.c
Ввод нового оператора в таблицу операторов конкретной базы данных
*/
#include <pwd.h>
#include "buhg.h"


void admin_newuser(const char *imabazr,const char *user)
{
char strsql[1024];
class SQLCURSOR cur;
SQL_str row;
class iceb_tu_str fio("");

sprintf(strsql,"select fio from %s.icebuser where login='%s'",imabazr,user);

if(readkey(strsql,&row,&cur) == 1)
 {
//  sprintf(strsql,gettext("Логин %s для базы %s уже есть!\n%s"),user,imabazr,row[0]);
//  iceb_t_soob(strsql);
  return;

 }

class VVOD VVOD1(2);

VVOD1.VVOD_SPISOK_add_MD(gettext("Введите фамилию оператора"));

vvod1(&fio,64,&VVOD1,NULL,stdscr,-1,-1);
int nomer=0;
/*Узнаём свободный номер оператора*/
for(nomer=1;;nomer++)
 {
  int voz=0;
  sprintf(strsql,"select un from %s.icebuser where un=%d",imabazr,nomer);
  if((voz=readkey(strsql)) <= 0)
   {
    if(voz < 0)
     return;
    break;
   }
 }

struct  passwd  *ktoz; /*Кто записал*/
ktoz=getpwuid(getuid());

sprintf(strsql,"insert into %s.icebuser (login,fio,un,logz,vrem) values('%s','%s',%d,'%s',%ld)",
imabazr,user,fio.ravno(),nomer,ktoz->pw_name,time(NULL));
sql_zapis(strsql,1,0);

}
