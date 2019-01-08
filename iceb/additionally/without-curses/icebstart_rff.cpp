/*$Id: icebstart_rf.c,v 5.8 2013/05/17 14:56:29 sasa Exp $*/
/*12.03.2017	07.09.2007	Белых А.И.	icebstart_rff.c
Чтение и выполнение запросов к базе данных сразу после открытия базы
*/
#include "bezcurses.h"

extern class iceb_tu_str kodirovka_iceb;
extern SQL_baza bd;

void icebstart_rff()
{
char stroka[1024];

if(kodirovka_iceb.getdlinna() > 1)
 {
  sprintf(stroka,"SET NAMES %s",kodirovka_iceb.ravno_udsimv("-"));
  if(sql_zap(&bd,stroka) != 0)
   {
    printf("%s-%d %s\n",__FUNCTION__,sql_nerror(&bd),sql_error(&bd));
    printf("%s-%s\n",__FUNCTION__,stroka);

   }
 }

if(sql_zap(&bd,"SET SESSION sql_mode = ''") != 0)
 {
  printf("%s-%d %s\n",__FUNCTION__,sql_nerror(&bd),sql_error(&bd));
  printf("%s-%s\n",__FUNCTION__,stroka);

 }
}