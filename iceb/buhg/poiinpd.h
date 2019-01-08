/*$Id: poiinpd.h,v 5.3 2013-01-02 10:38:36 sasa Exp $*/
/*16.11.2007	16.11.2007	Белых А.И.	poiinpdw.h
Класс для чтения переменных данных на инвентарный номер
*/

class poiinpdw_data
 {
  public:
  class iceb_tu_str shetu; //Счет учета
  class iceb_tu_str hzt; //Шифр затрат
  class iceb_tu_str hau; //Шифр аналитического учета
  class iceb_tu_str hna; //Шифр нормы амортотчислений для налогового учета
  float popkf; //Поправочный коэффициент для налогового учета
  int soso; //Состояние объекта
  class iceb_tu_str nomz;  //Номерной знак
  short mz; short gz;  //дата записи
  class iceb_tu_str hnaby; //Шифр нормы амортотчислений для бух. учета
  float popkfby;  ///Поправочный коэффициент для бух. учета
  
  poiinpdw_data()
   {
    clear();
   }

  void clear()
   {
    shetu.new_plus("");
    hzt.new_plus("");
    hau.new_plus("");
    hna.new_plus("");
    popkf=0;
    soso=0;
    nomz.new_plus("");
    mz=0;
    gz=0;
    hnaby.new_plus("");
    popkfby=0;
   }

 };

