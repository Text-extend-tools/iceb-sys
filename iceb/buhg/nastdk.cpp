/* $Id: nastdk.c,v 5.19 2013/12/31 11:48:10 sasa Exp $ */
/*15.07.2015    10.12.1993      Белых А.И.      nastdk.c
Читаем настройку  для платежных документов
Если вернули 0 файл прочитан
	     1-нет
*/
#include        <errno.h>
#include        "buhg.h"

double          okrg1=0.01; /*Округление*/
short           kopk; /*0-копеек нет 1-копейки есть*/
short           shetg; /*1-счет для железной дороги 0-нет*/
double          smp0,smp1; /*Суммы за переводы внутри банка и вне банка*/
short		mpo;       /*0-искать реквизиты в файле 1-в базе данных*/
short           mvs;       /*1-с разбивкой суммы по счетам 0-без*/
short		kor; /*Корректор межстрочного растояния*/
short		ots; /*Отступ от левого края*/
short		kp=1; /*Количество копий платежки*/
short		startgodd; /*Стартовый год платежных документов*/
short		startgodb; /*Стартовый год просмотров главной книги*/
short		vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/
short		kolstrdok; //Количество строк между документами

int             nastdk(void)
{
class iceb_tu_str st1("");
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

mpo=mvs=kor=ots=0;

kp=1;
kolstrdok=10;
vplsh=startgodd=shetg=startgodb=0;
smp0=smp1=0.;
okrg1=0.01;

sprintf(strsql,"select str from Alx where fil='nastdok.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"nastdok.alx");
  iceb_t_soob(strsql);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  polen(row_alx[0],&st1,1,'|');

  if(SRAV(st1.ravno(),"Поиск организаций в базе",0) == 0)
    {
     polen(row_alx[0],&st1,2,'|');
     if(SRAV(st1.ravno(),"Вкл",1) == 0)
       mpo=1;
     continue;
    }
/*****************
  if(SRAV(st1.ravno(),"Разделитель копеек",0) == 0)
    {
     polen(row_alx[0],&st1,2,'|');
     if(SRAV(st1.ravno(),"Зап",1) == 0)
       rzk=1;
     continue;
    }
*****************/
  if(SRAV(st1.ravno(),"Разбивка суммы",0) == 0)
    {
     polen(row_alx[0],&st1,2,'|');
     if(SRAV(st1.ravno(),"Вкл",1) == 0)
       mvs=1;
     continue;
    }
  if(SRAV(st1.ravno(),"Стартовый год",0) == 0)
    {
     polen(row_alx[0],&startgodd,2,'|');
//     startgodd=(short)ATOFM(st1); 
     continue;
    }

  if(SRAV(st1.ravno(),"Счет для ЖД",0) == 0)
    {
     polen(row_alx[0],&st1,2,'|');
     shetg=0;
     if(SRAV(st1.ravno(),"Да",1) == 0)
       shetg=1;
     continue;
    }
  if(SRAV(st1.ravno(),"Копейки",0) == 0)
    {
     polen(row_alx[0],&st1,2,'|');
     kopk=0;
     if(SRAV(st1.ravno(),"Есть",0) == 0)
       kopk=1;
     continue;
    }
  if(SRAV(st1.ravno(),"Округление суммы",0) == 0)
    {
     polen(row_alx[0],&okrg1,2,'|');
     continue;
    }

  if(SRAV(st1.ravno(),"Количество копий платежки",0) == 0)
    {
     polen(row_alx[0],&kp,2,'|');
//     kp=(short)ATOFM(st1);
     continue;
    }

  if(SRAV(st1.ravno(),"Количество строк между документами",0) == 0)
    {
     polen(row_alx[0],&kolstrdok,2,'|');
//     if(st1.ravno()[0] != '\0')
//       kolstrdok=(short)ATOFM(st1);
     continue;
    }

  if(SRAV(st1.ravno(),"Корректор межстрочного растояния",0) == 0)
    {
     polen(row_alx[0],&kor,2,'|');
//     kor=(short)ATOFM(st1);
     continue;
    }
  if(SRAV(st1.ravno(),"Отступ от левого края",0) == 0)
    {
     polen(row_alx[0],&ots,2,'|');
//     ots=(short)ATOFM(st1);
     continue;
    }
  if(SRAV(st1.ravno(),"Сумма платежа",0) == 0)
    {
     polen(row_alx[0],&smp0,2,'|');
//     smp0=ATOFM(st1);
     polen(row_alx[0],&smp1,2,'/');
//     smp1=ATOFM(st1);
     continue;
    }


 }

vplsh=0;
iceb_t_poldan("Многопорядковый план счетов",&st1,"nastrb.alx");
if(SRAV(st1.ravno(),"Вкл",1) == 0)
 vplsh=1;

startgodb=0;
if(iceb_t_poldan("Стартовый год",&st1,"nastrb.alx") == 0)
 {
  startgodb=st1.ravno_atoi();
 }


if(kp == 0)
 kp=1;

//if(nds1 == 0.)
//  printf(gettext("Не введён \"Н.Д.С.\" !\n"));
if(okrg1 == 0.)
  printf(gettext("Не введено \"Округление суммы\" !\n"));
return(0);
}
