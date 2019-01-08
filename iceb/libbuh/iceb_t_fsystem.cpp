/*$Id: iceb_t_fsystem.c,v 5.7 2013/05/17 14:56:28 sasa Exp $*/
/*07.02.2016	17.03.2009	Белых А.И.	iceb_t_fsystem.c
Чтение из файла команд и выполнение 
*/
#include "buhl.h"

void iceb_t_fsystem(const char *imaf_nast,const char *zamena)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_nast);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  return;
 }

class iceb_tu_str stroka("");
int voz=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  stroka.new_plus(row_alx[0]);

  if(zamena[0] != '\0')
   iceb_tu_zvstr(&stroka,"ICEB_ZAMENA",zamena);
  
  voz=iceb_t_system(stroka.ravno(),0);
  
  
  if(voz != 0)
   OSTANOV();
 }
}
