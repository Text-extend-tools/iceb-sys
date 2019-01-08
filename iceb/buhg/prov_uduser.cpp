/*$Id: prov_uduser.c,v 5.4 2013/05/17 14:56:08 sasa Exp $*/
/*15.07.2015	03.09.2010	Белых А.И.	prov_uduser.c
Проверка возможности удаления оператора
Если вернули 0-информации введённой этим оператором нет
             1-есть информация введённая этим оператором
             2-ошибка
             3-не найден логин
*/
#include "buhg.h"

int prov_uduser(const char *user,const char *imabazp,
int metka_soob) /*0-молча 1-выдать сообщение*/
{
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
int nom_op=0;

if(metka_soob == 1)
 {
  clear();
  GDITE();
 }
sprintf(strsql,"select un from %s.icebuser where login='%s'",imabazp,user);
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s-%s:%s",__FUNCTION__,gettext("Неизвестный логин"),user);
  iceb_t_soob(strsql);
  return(3);
 }

nom_op=atoi(row[0]);

sprintf(strsql,"SHOW TABLES FROM %s",imabazp);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(2);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(0);
 }
int kolstr1=0;
int voz=0;
while(cur.read_cursor(&row) != 0)
 {
  if(metka_soob == 1)
   {
    printw("%s\n",row[0]);
    strzag(LINES-1,0,kolstr,++kolstr1);
   }

  sprintf(strsql,"select ktoz from %s.%s where ktoz=%d limit 1",imabazp,row[0],nom_op);

  if((voz=sql_readkey(&bd,strsql)) < 0 ) 
   {
    if(sql_nerror(&bd) == ER_BAD_FIELD_ERROR) /*Нет такой колонки в таблице*/
     {
      sprintf(strsql,"select ktoi from %s.%s where ktoi=%d limit 1",imabazp,row[0],nom_op);
      if((voz=sql_readkey(&bd,strsql)) < 0 ) 
       if(sql_nerror(&bd) == ER_BAD_FIELD_ERROR) /*Нет такой колонки в таблице*/
        continue;
     }
   }  

  if(voz == 0)
   continue;
  else 
   {
    if(metka_soob == 1)
     {
      sprintf(strsql,"%s %s!",gettext("Использутся в таблице"),row[0]);
      iceb_t_soob(strsql);
     }
    return(1);
   }
 }
return(0);
}
