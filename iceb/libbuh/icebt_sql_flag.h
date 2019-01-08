/*$Id: icebt_sql_flag.h,v 5.4 2013/05/17 14:56:30 sasa Exp $*/
/*13.01.2011	16.08.2004	Белых А.И.	icebt_sql_flag.h
Класс для установки/снятия флага
*/

class icebt_sql_flag
 {
  class iceb_tu_str flag;
  short      metka; //0-флаг не установлен 1-установлен
  public:
  
  icebt_sql_flag(void); //Конструктор
  icebt_sql_flag(const char*); //Конструктор

  ~icebt_sql_flag();             //Деструктор
  int flag_on(void); //Установить флаг
  int flag_on(const char*); //Установить флаг
  int flag_off(void); //Снять флаг
 
 };
