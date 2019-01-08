/*$Id: zapzkor.c,v 5.14 2013/05/17 14:56:16 sasa Exp $*/
/*23.06.2009	15.02.2003	Белых А.И.	zapzkorf.c
Подпрограмма записи в кассовый ордер
Если вернули 0 - запись сделана
             1 - такая запись уже есть
             2 - другие ошибки записи
*/
#include        <time.h>
#include "bezcurses.h"
//#include <unistd.h>


int zapzkor(int metka, //0-ввод новой записи 1-корректировка старой
const char *kassa,int tipz,
short dd,short md,short gd,
const char *nomd,const char *kodkont,double suma,
const char *kontrz,
const char *koment)
{
time_t vrem=0;
char strsql[512];

time(&vrem);

//Уникальный ключ не позволит в одном документе иметь более одной записи на контрагента

if(metka == 0)
 sprintf(strsql,"insert into Kasord1 \
values(%s,%d,'%d-%d-%d','0000-00-00','%s','%s',%.2f,%d,0,%d,%ld,'%s')",
 kassa,tipz,gd,md,dd,nomd,kodkont,suma,gd,
 iceb_t_getuid(),vrem,koment);

if(metka == 1)
  sprintf(strsql,"update Kasord1 set \
kontr='%s',\
suma=%.2f,\
ktoi=%d,\
vrem=%ld,\
koment='%s' \
where kassa=%s and god=%d and tp=%d and nomd='%s' and kontr='%s'",
   kodkont,suma,iceb_t_getuid(),vrem,koment,
   kassa,gd,tipz,nomd,kontrz);

//printw("\nzapzkor-%s\n",strsql);
//OSTANOV();

if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_ENTRY) //Запись уже есть
   return(1);
  else
   {
    msql_error(&bd,__FUNCTION__,strsql);
    return(2);
   } 
 }
return(0);
}
