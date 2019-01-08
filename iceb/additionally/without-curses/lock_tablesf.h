/*$Id: lock_tables.h,v 5.5 2013/05/17 14:56:30 sasa Exp $*/
/*16.02.2009	22.02.2004 	Белых А.И.	lock_tables.h
Класс для блокировки/разблокировки таблиц
*/

class lock_tables
 {
  short metka;  //0-не заблокированы 1-заблокированы
  public:
  
   
  lock_tables(); //конструктор без параметров

  lock_tables(const char*); //конструктор с параметрами
  lock_tables(iceb_tu_str*); //конструктор с параметрами

  ~lock_tables(); //деструктор

  int lock(const char *tabl); //блокировка таблиц
  int lock(iceb_tu_str *tabl); //блокировка таблиц
  int unlock(); //разблокировка таблиц

 };
