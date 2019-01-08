/* $Id: kasnast.c,v 5.15 2013/05/17 14:56:03 sasa Exp $ */
/*14.07.2015	220.09.2000	Белых А.И.	kasnast.c
Чтение настроек для программы учета кассовых ордеров
Если вернули 0-файл прочитан
	     1-нет
*/
#include        "buhg.h"

short		vplsh; /*0-двух порядковый план счетов 1-мнопорядковый*/
short		startgodb; /*Стартовый год главной книги*/

int		kasnast()
{
char            st1[1024];
vplsh=startgodb=0;

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='nastrb.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"nastrb.alx");
  iceb_t_soob(strsql);
  return(1);
 }



while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  
  polen(row_alx[0],st1,sizeof(st1),1,'|');
  if(SRAV(st1,"Многопорядковый план счетов",0) == 0)
   {
    polen(row_alx[0],st1,sizeof(st1),2,'|');
    if(SRAV(st1,"Вкл",1) == 0)
      vplsh=1;
    continue;
   }
  if(SRAV(st1,"Стартовый год",0) == 0)
   {
    polen(row_alx[0],st1,sizeof(st1),2,'|');
    startgodb=(short)ATOFM(st1); 
    continue;
   }

 }
return(0);
}
