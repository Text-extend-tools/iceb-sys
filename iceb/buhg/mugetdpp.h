/*$Id:$*/
/*10.11.2014	06.11.2014	Белых А.И.	mugetdpp.h
поиск последних приходов
*/
class mugetdpp_str
 {
  public:
   /*реквизиты самого последнего прихода*/
   class iceb_tu_str datap;
   double cena;
   double kolih;
   class iceb_tu_str kodkontr;
   class iceb_tu_str ei;
   class iceb_tu_str shetu;
   class iceb_tu_str kodop;
   int sklad;
   int nomkart;

   /*Последние приходы на всех складах*/
   class SPISOK dataps;
   class masiv_din_double cenas;
   class masiv_din_double kolihs;
   class SPISOK kodkontrs;
   class SPISOK eis;
   class SPISOK shetus;
   class masiv_din_int sklads;
   class masiv_din_int nomkarts;
   class SPISOK kodops;
   
   mugetdpp_str()
    {
     clear();
    }

   void clear()
    {
     datap.new_plus("");
     cena=0.;
     kolih=0.;
     kodkontr.new_plus("");
     ei.new_plus("");
     shetu.new_plus("");
     sklad=0;
     nomkart=0;
     kodop.new_plus("");
     
     dataps.free_class();
     cenas.free_class();  
     kolihs.free_class();
     kodkontrs.free_class();
     eis.free_class();
     shetus.free_class();
     sklads.free_class();
     nomkarts.free_class();
     kodops.free_class();
    }
 };
 