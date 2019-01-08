/*$Id: bsizt.h,v 5.4 2013/08/13 05:49:46 sasa Exp $*/
/*18.06.2013	16.11.2007	Белых А.И.	bsizt.h
Класс для определения балансовой стоимости
*/

class bsizw_data 
 {
  public:
    double sbs,siz; //Стартовая балансовая стоимость и износ для налогового учета
    double bs,iz; //Изменения балансовой стоимости и износа для налогового учета
    double iz1; //Амортизация для налогового учёта
    double sbsby,sizby; //Стартовая балансовая стоимость и износ для бух.учёта
    double bsby,izby; //Изменения балансовой стоимости и износа для бух.учёта
    double iz1by; //Амортизация для бух. учёта

    double kbsnu; /*Конечная балансовая стоимость*/
    double kiznu; /*Конечный износ*/

    double kbsbu; /*Конечная балансовая стоимость*/
    double kizbu; /*Конечный износ*/
    
  bsizw_data ()
   {
    clear();
   }
  
  void clear()
   {
    sbs=siz=0.;
    bs=iz=0.;
    iz1=0.;
    sbsby=sizby=0.;
    bsby=izby=0.;
    iz1by=0.;
    kbsnu=kiznu=0.;
    kbsbu=kizbu=0.;
   }
   
 };
