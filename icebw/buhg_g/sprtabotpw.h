/*$Id: zar_sprav.h,v 1.2 2011-01-13 13:49:56 sasa Exp $*/
/*21.06.2016	26.05.2016	Белых А.И.	sprtabotpw.h
Реквизиты для расчёта отпускных
*/
class sprtabotp_poi
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str tabnom;
   short mn,gn;
   short dk,mk,gk;
   class iceb_u_str vid_otp;
   class iceb_u_str prikaz;
   class iceb_u_str period;
   class iceb_u_str kol_dnei;
   int metka_ri;  /*0-расчёт с использванием дожностных оклатдов 1-расчёт по коэффициентам введённым в файл*/
   class iceb_u_str fio;
   
   sprtabotp_poi()
    {
     clear_data();
    }
  
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    tabnom.new_plus("");
    vid_otp.new_plus("");
    prikaz.new_plus("");
    period.new_plus("");
    kol_dnei.new_plus("");
    dk=mn=gn=mk=gk=0;
    metka_ri=0;
    fio.new_plus("");
   }
 };
