/*$Id: udprov.c,v 5.17 2013/09/26 09:43:55 sasa Exp $*/
/*29.11.2010	27.11.1997	Белых А.И.	udprov.c
Уделение проводки (пары)
Если вернули 0 удаление прошло успешно
             1 - нет
*/
#include        "buhl.h"


int     	udprov(int val, //0-балансовый счёт -1 не балансовый
short god,short mes,short den, //Дата проводки
const char *sh,   //Счет
const char *shk,  //Счет корреспондент
time_t vrem,   //Время записи
double deb,  //Дебет
double kre,  //Кредит
const char *koment, //Коментарий
short kp) //Количество проводок
{
char strsql[1024];
char strok[1024];
/*
printw("udprov-%d %d.%d.%d %s %s %ld %.2f %.2f\n",
val,god,mes,den,sh,shk,vrem,deb,kre);
OSTANOV();
*/
if(iceb_t_pbsh(mes,god,sh,shk,gettext("Проводку удалить невозможно!")) != 0) /*Проверка блокировки счетов*/
 return(1);
 
sprintf(strsql,"delete from Prov where val=%d and datp='%04d-%02d-%02d' \
and sh='%s' and shk='%s' and vrem=%ld and deb=%.2f and kre=%.2f and \
komen='%s'",
val,god,mes,den,sh,shk,vrem,deb,kre,koment);

//printw("\n%s\n",strsql);
//OSTANOV();

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
  {
   iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
   return(1);
  }
 else
  {
   sprintf(strok,"1 %s-%s",__FUNCTION__,gettext("Ошибка удаления записи !"));
   msql_error(&bd,strok,strsql);
   return(1);
  }
 }
if(kp == 1 || val == -1)
  return(0);

sprintf(strsql,"delete from Prov where val=%d and datp='%04d-%02d-%02d' \
and sh='%s' and shk='%s' and vrem=%ld and deb=%.2f and kre=%.2f and \
komen='%s'",
val,god,mes,den,shk,sh,vrem,kre,deb,koment);

if(sql_zap(&bd,strsql) != 0)
 {
  sprintf(strok,"2 %s-%s",__FUNCTION__,gettext("Ошибка удаления записи !"));
  msql_error(&bd,strok,strsql);
  return(1);
 }
return(0);   
}
