/* $Id: zapprov.c,v 5.21 2013/08/13 05:50:23 sasa Exp $ */
/*20.11.2012    26.11.1997      Белых А.И.      zapprovf.c
Запись проводок
Если вернули 0 запись прошла успешно
             1 нет
*/
//#include        <pwd.h>
#include        <string.h>
#include        "bezcurses.h"
//#include <unistd.h>


int zapprov(int val, //0-балансовая проводка -1 не балансовая
int god,int mes,int den, //Дата
const char *sh, //Счет
const char *shk, //Счет коррепондент
const char *kor1, //Код контрагента счета
const char *kor2, //Код контрагента счета корреспондента
const char *kto,  //Кто записал
const char *nn,   //Номер документа
const char *kop,  //Код операции
double deb,  //Сумма в дебете
double kre,  //Сумма в кредите
const char *raz,  //Коментарий
short kp,    //Количество проводок
int pod,     //Подразделение
time_t vrem,   //Время записи
short gd,short md,short dd, //Дата документа
int tipz, //1-приход 2-расход 0-не определено
int kekv) //код экономической классификации расходов
{
int             ktor; /*Кто работает*/
char		strsql[2048];
class iceb_tu_str kontr1("");
class iceb_tu_str kontr2("");

/***********
printw("\n%s-%d %d.%d.%d %s %s %s %s %s %s %s %d %.2f %.2f %s\n",
__FUNCTION__,val,god,mes,den,sh,shk,kor1,kor2,kto,nn,kop,pod,deb,kre,raz);
OSTANOV();
************/
if(iceb_t_pbsh(mes,god,sh,shk,gettext("Невозможно записать проводку!")) != 0)
 return(1);

kodkontr(&kontr1,&kontr2,kor1,kor2);

ktor=iceb_t_getuid();

sprintf(strsql,"insert into Prov \
values (%d,'%04d-%02d-%02d','%s','%s','%s','%s','%s',%d,'%s',%.2f,%.2f,%d,%ld,\
'%s','%04d-%02d-%02d',%d,%d)",
val,god,mes,den,sh,shk,kontr1.ravno(),kto,nn,pod,kop,deb,kre,ktor,vrem,raz,
gd,md,dd,tipz,kekv);

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
  {
   printf("%s-%s\n",__FUNCTION__,gettext("У вас нет полномочий для выполнения этой операции !"));
   return(1);
  }
 else
  {
   msql_error(&bd,__FUNCTION__,strsql);
   return(1);
  }
 }
if(kp == 1)
  return(0);

sprintf(strsql,"insert into Prov \
values (%d,'%04d-%02d-%02d','%s','%s','%s','%s','%s',%d,'%s',%.2f,%.2f,%d,%ld,\
'%s','%04d-%02d-%02d',%d,%d)",
val,god,mes,den,shk,sh,kontr2.ravno(),kto,nn,pod,kop,kre,deb,ktor,vrem,raz,
gd,md,dd,tipz,kekv);

if(sql_zap(&bd,strsql) != 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return(1);
 }
return(0);
}




