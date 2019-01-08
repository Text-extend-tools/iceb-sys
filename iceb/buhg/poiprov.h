/*$Id: poiprov.h,v 5.5 2013/05/17 14:56:07 sasa Exp $*/
/*22.11.2011	26.03.2008	Белых А.И.	poiprov.h
Реквизиты для поиска проводок
*/

class poiprov_rek
 {
  public:
   class iceb_tu_str dat1;
   class iceb_tu_str dat2;
   class iceb_tu_str dat1z;
   class iceb_tu_str dat2z;
   class iceb_tu_str sh,shk;
   class iceb_tu_str deb,kre;
   class iceb_tu_str kor;
   class iceb_tu_str kop;
   class iceb_tu_str nn;
   class iceb_tu_str raz;
   class iceb_tu_str kto;
   class iceb_tu_str grupa;
   class iceb_tu_str status;
   class iceb_tu_str kekv_poi;
  
  class iceb_tu_str zapros;
  
  poiprov_rek()
   {
    clear();
   }
  void clear()
   {
    sh.new_plus("");
    shk.new_plus("");
    dat1.new_plus("");
    dat2.new_plus("");
    dat1z.new_plus("");
    dat2z.new_plus("");
    deb.new_plus("");
    kre.new_plus("");
    kor.new_plus("");
    kop.new_plus("");
    nn.new_plus("");
    raz.new_plus("");
    kto.new_plus("");
    grupa.new_plus("");
    status.new_plus("");
    kekv_poi.new_plus("");
    
   }  
 };
