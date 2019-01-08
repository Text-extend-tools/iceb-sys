/*$Id: uslnast.c,v 5.27 2013/10/04 07:25:03 sasa Exp $*/
/*03.10.2013	11.02.2000	Белых А.И.	uslnast.c
Чтение настроек для услуг
Если вернули 0 файл прочитан
	     1 файл не прочитан
*/
#include        <errno.h>
#include	"buhg.h"

short		vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/
short		startgodus=0; /*Стартовый год просмотров услуг*/
short		startgodb=0; /*Стартовый год Главной книги*/
short		startgod; //Стартовый год просмотров в материальном учёте

class iceb_tu_str bndsu1(""); /*0% НДС реализация на територии Украины.*/
class iceb_tu_str bndsu2(""); /*0% НДС экспорт.*/
class iceb_tu_str bndsu3(""); /*0% Освобождение от НДС статья 5.*/
double		okrcn;  /*Округление цены*/
double		okrg1;  /*Округление суммы*/
short           mfnn; /*Метка формирования номера накладной
                      -1 - Автоматическая нумерация выключена
                       0 - Приходы и расходы - нумерация общая
                       1 - Приходы отдельно расходы отдельно
                       */
double		kursue=0.;/*Курс условной единицы*/

int		uslnast()
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
char strsql[512];
int kolstr=0;
class iceb_tu_str st1("");
class iceb_tu_str bros("");
short		voz;

printw("\n%s\n",
gettext("Чтение нормативно-справочной информации"));
refresh();

startgod=0;
okrg1=okrcn=0.;
vplsh=startgodus=0;
mfnn=(-1);
kursue=0.;

bndsu1.new_plus("");
bndsu2.new_plus("");
bndsu3.new_plus("");


sprintf(strsql,"select str from Alx where fil='uslnast.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"uslnast.alx");
  iceb_t_soob(strsql);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  polen(row_alx[0],&st1,1,'|');
  if(SRAV(st1.ravno(),"Раздельная нумерация документов",0) == 0)
    {
     mfnn=0;
     
     polen(row_alx[0],&st1,2,'|');
     if(st1.ravno()[0] == '\0' || st1.ravno()[0] == '\n')
      mfnn=(-1);
      
     if(SRAV(st1.ravno(),"Вкл",1) == 0)
       mfnn=1;
     continue;
    }

  if(SRAV(st1.ravno(),"Стартовый год",0) == 0)
    {
     polen(row_alx[0],&startgodus,2,'|');
     continue;
    }
  if(SRAV(st1.ravno(),"0% НДС реализация на територии Украины",0) == 0)
    {
     polen(row_alx[0],&bndsu1,2,'|');
     continue;
    }
  if(SRAV(st1.ravno(),"0% НДС экспорт",0) == 0)
    {
     polen(row_alx[0],&bndsu2,2,'|');
     continue;
    }
  if(SRAV(st1.ravno(),"Освобождение от НДС",0) == 0)
    {
     polen(row_alx[0],&bndsu3,2,'|');
     continue;
    }
 }


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

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  polen(row_alx[0],&st1,1,'|');
  if(SRAV(st1.ravno(),"Стартовый год",0) == 0)
    {
     polen(row_alx[0],&startgod,2,'|');
     continue;
    }
  if(SRAV(st1.ravno(),"Округление цены",0) == 0)
   {
    polen(row_alx[0],&okrcn,2,'|');
    continue;
   }

  if(SRAV(st1.ravno(),"Округление суммы",0) == 0)
    {
     polen(row_alx[0],&okrg1,2,'|');
     continue;
    }
  if(SRAV(st1.ravno(),"Курс УЕ",0) == 0)
    {
     polen(row_alx[0],&kursue,2,'|');
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

voz=0;
if(okrcn == 0.)
 {
  beep();
  voz++;
  printw(gettext("Не введено \"Округление цены\" !\n"));
 }

if(okrg1 == 0.)
 {
  beep();
  voz++;
  printw(gettext("Не введено \"Округление суммы\" !\n"));
 }

if(voz != 0)
 OSTANOV();

return(0);

}
