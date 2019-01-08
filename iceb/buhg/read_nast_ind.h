/*$Id: read_nast_ind.h,v 5.6 2013/05/17 14:56:09 sasa Exp $*/
/*30.01.2010	30.01.2009	Белых А.И.	read_nast_ind.h
Класс для хранения данных для расчёта индексации заработной платы
*/

class index_ua
 {
  public:
/***********Индексация начислений *******************/
  class masiv_din_int mr; //Месяцы расчёта
  class masiv_din_int gr; //Года расчёта
//  class masiv_din_double prog_min;
  /****************/
  class masiv_din_int mi; //Месяцы коэффициентов 
  class masiv_din_int gi; //Годы коэффициентов
  class masiv_din_double koef; //Коэффициенты индексации

/********** Индексация зарплаты выплаченной с задержкой**************/
  class masiv_din_int miv; //Месяцы коэффициентов 
  class masiv_din_int giv; //Годы коэффициентов
  class masiv_din_double koefv; //Коэффициенты индексации
  int kninvz; /*Код начисления индексации не вовремя выплаченной зарплаты*/
  int kodpzns; /*Код перечисления зарплаты на счёт*/
  int kodpzvk; /*Код получения зарплаты в кассе*/

  index_ua()
   {
   kninvz=0;
   kodpzns=0;
   kodpzvk=0;

   }
  void free()
   {
    mr.free_class();
    gr.free_class();
//    prog_min.free_class();
    mi.free_class();
    gi.free_class();
    koef.free_class();

    miv.free_class();
    giv.free_class();
    koefv.free_class();
   }

 };
