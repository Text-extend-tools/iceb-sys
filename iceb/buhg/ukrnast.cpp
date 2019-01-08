/*$Id: ukrnast.c,v 5.23 2013/09/26 09:43:41 sasa Exp $*/
/*16.07.2015	22.10.2002	Белых А.И.	ukrnast.c
Чтение настроечной информации для подсистемы "Учет командировочных расходов."
*/
#include	"buhg.h"


short		vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/
short		startgod=0;
short		startgodb=0; /*Стартовый год просмотров главной книги*/
short           mfnn=0; //Метка формирования номера в этой подсистеме не задействована
double		okrcn=0.01;  /*Округление цены*/
double		okrg1=0.01;  /*Округление суммы*/

int		ukrnast(void)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
class iceb_tu_str st1("");

startgodb=startgod=0;

sprintf(strsql,"select str from Alx where fil='ukrnast.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"ukrnast.alx");
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
    polen(row_alx[0],&startgod,2,'|');
//    startgod=(short)ATOFM(st1); 
    continue;
   }
 }

 
iceb_t_poldan("Многопорядковый план счетов",&st1,"nastrb.alx");
if(SRAV(st1.ravno(),"Вкл",1) == 0)
 vplsh=1;

startgodb=0;
if(iceb_t_poldan("Стартовый год",&st1,"nastrb.alx") == 0)
  startgodb=st1.ravno_atoi();

sprintf(strsql,"select str from Alx where fil='matnast.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"matnast.alx");
  iceb_t_soob(strsql);
  return(1);
 }
okrg1=0.01;
okrcn=0.01;

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  polen(row_alx[0],&st1,1,'|');

  if(SRAV(st1.ravno(),"Округление суммы",0) == 0)
   {
     polen(row_alx[0],&okrg1,2,'|');
//     okrg1=ATOFM(st1);
     continue;
   }

  if(SRAV(st1.ravno(),"Округление цены",0) == 0)
   {
    polen(row_alx[0],&okrcn,2,'|');
//    okrcn=ATOFM(st1);
    continue;
   }
 }
return(0); 
}
