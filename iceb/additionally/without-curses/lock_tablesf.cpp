/*$Id: lock_tables.c,v 5.5 2013/05/17 14:56:30 sasa Exp $*/
/*16.02.2009	22.02.2004 	Белых А.И.	lock_tables.c
Класс для блокировки/разблокировки таблиц
*/
#include "bezcurses.h"

//extern SQL_baza	bd;

lock_tables::lock_tables() //конструктор без параметров
 {
  metka=0;
 }
lock_tables::lock_tables(iceb_tu_str *tabl) //конструктор с параметрами
 {
  lock(tabl);
 }
lock_tables::lock_tables(const char *tabl) //конструктор с параметрами
 {
  lock(tabl);
 }

lock_tables::~lock_tables() //деструктор
 {
  unlock();    
 }

int lock_tables::lock(const char *tabl) //блокировка таблиц
 {
  if(sql_zap(&bd,tabl) != 0)
   {
    msql_error(&bd,"Ошибка блокировки таблицы !",tabl);
    return(1);
   } 
//  printw("lock_tables-Таблицы заблокированы.\n");
  metka=1;
  return(0);    
 }
int lock_tables::lock(iceb_tu_str *tabl) //блокировка таблиц
 {
  if(sql_zap(&bd,tabl->ravno()) != 0)
    return(1);
    
  metka=1;
  return(0);    
 }
int lock_tables::unlock() //разблокировка таблиц
 {
  if(metka == 0)
   return(0);

  if(sql_zap(&bd,"UNLOCK TABLES") != 0)
   {
    msql_error(&bd,"Ошибка разблокировки таблицы !","UNLOCK TABLES");
    return(1);
   }
  metka=0;
//  printw("lock_tables-Таблицы разблокированы.\n");
  
  return(0);    

 }
