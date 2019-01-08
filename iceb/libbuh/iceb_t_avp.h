/*$Id: iceb_t_avp.h,v 5.7 2013/11/05 10:50:57 sasa Exp $*/
/*29.03.2017	14.04.2009	Белых А.И.	iceb_t_avp.h
Класс для автоматического выполнения проводок
*/
class iceb_t_avp
 {
  public:
   double suma_s_nds; /*Сумма с НДС*/
   double suma_bez_nds; /*Сумма без НДС*/
   double suma_nds;     /*Сумма НДС*/
   double suma_uheta;   /*Сумма учёта*/
   double suma_iznosa; /*Сумма износа - Для учёта основных средств*/
   double suma_uslug_bez_nds;  /*Сумма услуг - в документах материального учёта*/
      
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
   short metka_avk; /*Метка автоматической вставки контрагента в список счёта 
                     0 запрос на работу со списком контрагентов если контрагента нет в списке счёта
                     1 автоматическая вставка кода контрагента в список счета для расчёта зарплаты
                     2 автоматическая вставка из других подсистем
                    ********************/
                                       
   


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
    metka_avk=0;
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
