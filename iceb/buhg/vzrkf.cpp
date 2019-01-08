/*$Id: vzrkf.c,v 5.9 2013-04-07 10:22:44 sasa Exp $*/
/*03.04.2006	03.04.2006	Белых А.И.	vzrkf.c
Чтение контрагентов из файла
*/
#include "buhg.h"
#include "dok4.h"

extern class REC rec;

void		vzrkf()
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='platpor.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s-%s %s!",__FUNCTION__,gettext("Не найдена настройка"),"platpor.alx");
  iceb_t_soob(strsql);
  return;
 }

for(int nom=0; nom < 6; nom++)
  cur_alx.read_cursor(&row_alx);



if(polen(row_alx[0],&rec.naior,2,'|') != 0)
 {
  iceb_t_soob(gettext("Файл организаций искажен !!!\nАварийное завершение.\n"));
  return;
 }

polen(row_alx[0],&rec.gorod,3,'|');

polen(row_alx[0],&rec.kod,4,'|');

polen(row_alx[0],&rec.naiban,5,'|');

polen(row_alx[0],&rec.mfo,6,'|');

polen(row_alx[0],&rec.nsh,7,'|');

polen(row_alx[0],&rec.kodor,8,'|');

//polen(row_alx[0],rec.rsnds,sizeof(rec.rsnds),9,'|');

/*Смотрим может есть в строке "-" если есть берем его*/
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(row_alx[0][0] == '-')
   {
    polen(row_alx[0],&rec.naior,2,'|');

    polen(row_alx[0],&rec.gorod,3,'|');

    polen(row_alx[0],&rec.kod,4,'|');

    polen(row_alx[0],&rec.naiban,5,'|');

    polen(row_alx[0],&rec.mfo,6,'|');

    polen(row_alx[0],&rec.nsh,7,'|');

    polen(row_alx[0],&rec.kodor,8,'|');

    break;
   }
 }


cur_alx.poz_cursor(0);

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(row_alx[0][0] == '+')
   {

    polen(row_alx[0],&rec.naior1,2,'|');

    polen(row_alx[0],&rec.gorod1,3,'|');

    polen(row_alx[0],&rec.kod1,4,'|');

    polen(row_alx[0],&rec.naiban1,5,'|');

    polen(row_alx[0],&rec.mfo1,6,'|');

    polen(row_alx[0],&rec.nsh1,7,'|');

    polen(row_alx[0],&rec.kodor1,8,'|');

    break;
   }
 }
}
