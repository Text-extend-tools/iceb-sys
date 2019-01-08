/*$Id: iceb_t_avp.h,v 5.6 2013/05/17 14:56:28 sasa Exp $*/
/*27.10.2013	14.04.2009	Белых А.И.	iceb_t_avp.h
Класс для автоматического выполнения проводок
*/
class iceb_t_avp
 {
  public:
   double suma_s_nds;
   double suma_bez_nds;
   double suma_nds;
   double suma_uheta;
   double suma_iznosa; /*Для учёта основных средств*/
   double suma_uslug_bez_nds; /*Сумма услуг без НДС*/   
   
   double suma_nds_vv; /*CУмма НДС введённая в ручную*/   
   int metka_snds; /*Метка выполнения проводки на сумму НДС введённую в ручную*/

   class iceb_tu_str kodop;
   class iceb_tu_str shetuh;
   class iceb_tu_str shetsp;   
   class iceb_tu_str kontr_v_dok;
   class iceb_tu_str nomer_dok; /*Номер документа поставщика*/
   
   /*Дата подтвержения*/
   short dp;
   short mp;
   short gp;

   class SPISOK vib_shets; /*Список счетов для выбора*/
   class SPISOK vib_shet;  /*выбранный из списка счёт для выполнения проводок*/

  iceb_t_avp()
   {
    clear();
    kodop.new_plus("");
    suma_nds_vv=0.;
    metka_snds=0;
    kontr_v_dok.plus("");
    nomer_dok.plus("");
   }
  void clear()
   {
    suma_iznosa=0.;
    suma_s_nds=0.;
    suma_bez_nds=0.;
    suma_nds=0.;
    suma_uheta=0.;
    suma_uslug_bez_nds=0.;
    shetuh.new_plus("");
    shetsp.new_plus("");
    dp=mp=gp=0;
   }
 };
