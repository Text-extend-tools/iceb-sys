/*$Id:$*/
/*24.11.2016	22.11.2016	Белых А.И.	iceb_t_vrvr.h
вставка реквизитов в распечатку
перечень реквизитов и строк и позиций вставки
*/
class iceb_vrvr
 {
  public:
   class masiv_din_int ns; /*номера строк в которые надо вставить*/
   class masiv_din_int begin; /*позиция начала встваки*/
   class masiv_din_int end; /*позиция конца встваки*/
   class masiv_din_int hag;  /*шаг вставки*/
   class SPISOK str;         /*то что нужно вставить*/

   void free_class()
    {
     ns.free_class();
     begin.free_class();
     end.free_class();
     hag.free_class();
     str.free_class();
    }
 };
 