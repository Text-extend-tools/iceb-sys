/* $Id: buhnast.c,v 5.21 2013/09/26 09:43:29 sasa Exp $ */
/*13.02.2016    21.12.1993      Белых А.И.      buhnast.c
Чтение настройки для бугалтерии
Если вернули 0-файл прочитан
	     1-нет
*/
#include        <errno.h>
#include        "buhg.h"

double		okrcn=0.;  /*Округление цены*/
double		okrg1=0.;  /*Округление суммы*/
short		startgoduos=0; /*Стартовый год для Учёта основных средств*/
short		startgod=0; /*Стартовый год просмотров в Материальном учёте*/
short		startgodb=0; /*Стартовый год главной книги*/
short		zkhp;  /*0-запрещено корректировать проводки из других подсистем 1-разрешено*/
short		vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/
char		*sbshet=NULL; //Список бюджетных счетов

int             buhnast()
{
class iceb_tu_str st1("");
char strsql[512];

vplsh=startgodb=zkhp=0;

if(sbshet != NULL)
 {
  delete [] sbshet;
  sbshet=NULL;
 }

int kolstr;
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select str from Alx where fil='nastrb.alx' order by ns asc");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob("Не найдены настройки nastrb.alx");
  return(1);
 }


while(cur.read_cursor(&row) != 0)
 {
  if(row[0][0] == '#')
   continue;

  polen(row[0],&st1,1,'|');

  if(SRAV(st1.ravno(),"Корректировка чужих проводок",0) == 0)
    {
     polen(row[0],&st1,2,'|');
     if(SRAV(st1.ravno(),"Вкл",1) == 0)
       zkhp=1;
     continue;
    }
  if(SRAV(st1.ravno(),"Многопорядковый план счетов",0) == 0)
    {
     polen(row[0],&st1,2,'|');
     if(SRAV(st1.ravno(),"Вкл",1) == 0)
       vplsh=1;
     continue;
    }
  if(SRAV(st1.ravno(),"Стартовый год",0) == 0)
    {
     polen(row[0],&startgodb,2,'|');
     continue;
    }

  if(SRAV(st1.ravno(),"Список бюджетных счетов",0) == 0)
   {
    class iceb_tu_str bros("");
    polen(row[0],&bros,2,'|');
    if(bros.ravno()[0] == '\0')
     continue;
    sbshet=new char[strlen(bros.ravno())+1];
    strcpy(sbshet,bros.ravno());
   }

 }


//printw("startgodb=%d\n",startgodb);
//OSTANOV();

startgod=0;

okrg1=okrcn=0.;

sprintf(strsql,"select str from Alx where fil='matnast.alx' order by ns asc");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob("Не найдены настройки matnast.alx");
  return(1);
 }


while(cur.read_cursor(&row) != 0)
 {
  if(row[0][0] == '#')
   continue;

  polen(row[0],&st1,1,'|');

  if(SRAV(st1.ravno(),"Стартовый год",0) == 0)
    {
     polen(row[0],&startgod,2,'|');
     continue;
    }

  if(SRAV(st1.ravno(),"Округление цены",0) == 0)
    {
     polen(row[0],&okrcn,2,'|');
     continue;
    }

  if(SRAV(st1.ravno(),"Округление суммы",0) == 0)
    {
     polen(row[0],&okrg1,2,'|');
     continue;
    }

 }

startgoduos=0;

sprintf(strsql,"select str from Alx where fil='uosnast.alx' order by ns asc");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob("Не найдены настройки uosnast.alx");
  return(1);
 }

while(cur.read_cursor(&row) != 0)
 {
  if(row[0][0] == '#')
   continue;
  polen(row[0],&st1,1,'|');

  if(SRAV(st1.ravno(),"Стартовый год",0) == 0)
    {
     polen(row[0],&startgoduos,2,'|');
     continue;
    }
 }
return(0);
}



