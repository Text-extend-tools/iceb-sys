/*$Id: uplnast.c,v 5.22 2013/08/13 05:49:58 sasa Exp $*/
/*20.06.2013	21.03.2003	Белых А.И. 	uplnast.c
Чтение настроек для подсистемы "Учет путевых листов"
*/
#include	"buhg.h"


short		vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/
short		startgodupl=0; //Стартовый год для "Учета путевых листов"
short		startgodb=0; /*Стартовый год просмотров главной книги*/

int	uplnast(void)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
class iceb_tu_str st1("");

startgodupl=0;

sprintf(strsql,"select str from Alx where fil='uplnast.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"uplnast.alx");
  iceb_t_soob(strsql);
  return(1);
 }


while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  polen(row_alx[0],&st1,1,'|');

  if(SRAV(st1.ravno(),"Стартовый год",0) == 0)
   {
    polen(row_alx[0],&startgodupl,2,'|');
//    startgodupl=(short)ATOFM(st1); 
    continue;
   }
 }


iceb_t_poldan("Многопорядковый план счетов",&st1,"nastrb.alx");
if(SRAV(st1.ravno(),"Вкл",1) == 0)
 vplsh=1;

startgodb=0;
if(iceb_t_poldan("Стартовый год",&st1,"nastrb.alx") == 0)
 {
  startgodb=st1.ravno_atoi();
 }

return(0);

}
