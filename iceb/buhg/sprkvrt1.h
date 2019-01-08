/*$Id: sprkvrt1.h,v 5.12 2012-10-30 09:24:26 sasa Exp $*/
/*06.08.2015	31.04.2005	Белых А.И.	sprkvrt1.h
Настройки для расчёта формы 1ДФ
*/

class sprkvrt1_nast
 {
  public:
  class SPISOK sheta;               //Настройки пар счетов для поиска нужных проводок

  class masiv_din_int kodi_pd; //коды признаков доходов
  class SPISOK kodi_nah;       // коды начислений к признакам доходов
  class masiv_din_int metka_podoh; /*0-подоходный налог берётся для соотвецтвующего кода признака доходая 1-не берётся*/

  class iceb_tu_str gosstrah; /*список кодов фондов начиления на зарплату*/

  short		TYP; /*Метка 0-юридическая особа 1-физическая особа*/
  short    *kodnvf8dr;  /*Коды не входящие в форму 1ДФ*/
  short    *kodud;  /*Коды удержаний не входящие в форму 1ДФ*/
  
  class masiv_din_int kodi_pd_prov; //коды признаков доходов для которых данные берём из главной книги
  
  class masiv_din_int kodkr; /*коды командировочных расходов*/
  class SPISOK kodd_for_kodkr; /*Коды доходов для кодов командировочных расходов*/
    
  sprkvrt1_nast() //Конструктор
   {
    gosstrah.new_plus("");
    TYP=0;    
    kodnvf8dr=NULL;
    kodud=NULL;
   }

  ~sprkvrt1_nast() //Деструкток
   {
    if(kodnvf8dr != NULL)
      delete [] kodnvf8dr;

    if(kodud != NULL)
      delete [] kodud;
   }

 };

