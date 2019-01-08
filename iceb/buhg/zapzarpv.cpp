/*$Id: zapzarpv.c,v 5.12 2013/09/26 09:43:45 sasa Exp $*/
/*20.04.2016	03.03.2002	Белых А.И.	zapzarpv.c
Запись в таблицу Zarp и Zarn1
Если вернули 0 - записали
             1 - нет
*/
#include        "buhg.h"


int	zapzarpv(short dz,short mz,short gz, //Дата записи
long tabn, //Табельный номер
short prn, //1- начисление 2-удержание
short knah, //Код начисления /удержания
double suma, //Сумма
const char *shet, //Счет в запись
short mv,short gv,  //В счёт какого месяца
short kdn, //Количество дней
short nz, //Номер записи
const char *koment, //Коментарий
int podr, //подразделение
short dd,short md,short gd, //дата до которой действует
const char *nomdok, //номер документа
class ZARP zp) //Структура с запомненной записью которя была до корректировки
{
int i=0;

if((i=zapzarp(dz,mz,gz,tabn,prn,knah,suma,shet,mv,gv,kdn,nz,koment,podr,nomdok,zp)) == 1)
 {
  char bros[512];
  sprintf(bros,"%s-*%s",__FUNCTION__,gettext("Такая запись уже есть !"));
  iceb_t_soob(bros);
  return(1);
 }

if(i == 3)
 {
  iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
  return(3); 
 }

if(i != 0)
  return(2);
 
zapzarn1(tabn,prn,knah,dd,md,gd,shet);
return(0);
}
