/*$Id: iceb_t_fioruk.h,v 5.8 2013/05/17 14:56:28 sasa Exp $*/
/*27.02.2010	04.04.2008	Белых А.И.	iceb_t_fioruk.h
Реквизиты руководителя
*/

class iceb_t_fioruk_rk
 {
  public:
   class iceb_tu_str fio;
   class iceb_tu_str inn;
   class iceb_tu_str telef;
   class iceb_tu_str dolg;
   class iceb_tu_str famil;
   class iceb_tu_str ima;
   class iceb_tu_str oth;
   class iceb_tu_str famil_inic;
   char inic[32];
  iceb_t_fioruk_rk()
   {
    clear();
   }
  void clear()
   {
    fio.plus("");
    inn.plus("");
    telef.plus("");
    dolg.plus("");
    famil.new_plus("");
    ima.new_plus("");
    oth.new_plus("");
    famil_inic.plus("");    
    memset(inic,'\0',sizeof(inic));
   }
   
 };
