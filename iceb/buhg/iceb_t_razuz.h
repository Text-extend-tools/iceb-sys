/*$Id: iceb_t_razuz.h,v 5.4 2013/05/17 14:56:02 sasa Exp $*/
/*10.11.2007	05.11.2007	Белых А.И.	iceb_t_razuz.h
Класс для выполнения разузлования изделия
*/
class iceb_t_razuz_data
 {
  public:
   int kod_izd; /*Код изделия которое нужно разузловать*/
   class masiv_din_int kod_det; /*Список кодов не узлов входящие в это изделие*/

   class SPISOK kod_det_ei; /*Список кодов деталей вместе с единицами измерения*/
   class masiv_din_double kolih_det_ei; /*Количество не узлов входящих в изделие синхронизировано с kod_det_ei */
   class masiv_din_int metka_mu; /*Метки 0-товар 1-услуга синхронизировано с kod_det_ei*/
      
   class masiv_din_int kod_uz; /*Список кодов входящих узлов в изделие*/
   class masiv_din_double kolih_uz; /*Количество входящих узлов в изделие*/
      
   iceb_t_razuz_data()
    {
     kod_izd=0;
    }
   void clear()
    {
     kod_det.free_class();
     kod_uz.free_class();
     kolih_det_ei.free_class();
     kolih_uz.free_class();
     metka_mu.free_class();
     kod_det_ei.free_class();
    }
  
   double kolih_det(int kod) /*Расчёт количества по коду детали так как с разными единицами измерения код может быть в списке несколько раз*/
    {
     double suma=0.;
     int kod_p=0;
     for(int ii=0; ii < kod_det_ei.kolih(); ii++)
      {
       polen(kod_det_ei.ravno(ii),&kod_p,1,'|');
       if(kod_p != kod)
        continue;
       suma+=kolih_det_ei.ravno(ii);
      }
     return(suma);
    }
 };
