/*$Id: iceb_t_rnfxml.h,v 5.5 2013/05/17 14:56:29 sasa Exp $*/
/*03.04.2016	12.10.2006	Белых А.И.	iceb_t_rnfxml.h
Реквизиты шапки документов в xml формате
*/

class iceb_rnfxml_data
 {
  public:
  short dt,mt,gt;
  class iceb_tu_str kod_edrpuo;
  class iceb_tu_str kod_oblasti;
  class iceb_tu_str kod_admin_r;
  class iceb_tu_str naim_kontr;
  class iceb_tu_str adres;
  class iceb_tu_str innn;
  class iceb_tu_str nspnds;
  class iceb_tu_str kod_dpi;
  class iceb_tu_str kod_tabl;
  class iceb_tu_str telefon;
    
  int kod_gr_voz_tar; //Код группы возвратная тара  
//  int metka_eksp_nn; /*метка экспорта налоговой накладной для электронной регистрации в налоговой инспекции 0-выданная 1-копия оставленная у продавкца*/

  iceb_rnfxml_data()
   {
    kod_edrpuo.plus("");
    kod_oblasti.plus("");
    kod_admin_r.plus("");
    naim_kontr.plus("");
    kod_tabl.plus("");
    adres.plus("");
    innn.plus("");
    nspnds.plus("");
    kod_dpi.plus("");
    telefon.plus("");
    poltekdat(&dt,&mt,&gt);   
    kod_gr_voz_tar=0;
//    metka_eksp_nn=0;
   }
 };
