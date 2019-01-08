/*$Id: dok4.h,v 5.15 2013-01-02 10:38:36 sasa Exp $*/
/*29.12.2012	07.05.1998	Белых А.И.	dok4.h
Набор реквизитов для получения платежного поручения
*/
class REC
{
 public:
  /*Свои реквизиты*/
  class iceb_tu_str kodor;
  class iceb_tu_str naior; /*Наименование организации*/
  class iceb_tu_str gorod; /*Город*/
  class iceb_tu_str kod;   /*Код*/
  class iceb_tu_str naiban;/*Наименование банка*/
  class iceb_tu_str mfo;   /*МФО*/
  class iceb_tu_str nsh;   /*Номер счета в банке*/

  /*Чужие реквизиты*/
  class iceb_tu_str kodor1; /*Код организации*/
  class iceb_tu_str naior1; /*Наименование организации*/
  class iceb_tu_str gorod1; /*Город*/
  class iceb_tu_str kod1;   /*Код*/
  class iceb_tu_str naiban1;/*Наименование банка*/
  class iceb_tu_str mfo1;   /*МФО*/
  class iceb_tu_str nsh1;   /*Номер счета в банке*/

  double		nds;
  class iceb_tu_str shbzu; /*Счет банка по которому платят за услуги*/
  class iceb_tu_str nomdk; /*Номер документа*/
  double		sumd;      /*Сумма по документу*/
  double		uslb;      /*Сумма за услуги банка*/
  short		dd,md,gd;   /*Дата документа*/
  class iceb_tu_str kodop;  /*Код операции*/
  int		ktoz;       /*Кто записал*/
  long		vremz;      /*Время записи*/
  short		prov;         /*0-проводки сделаны 1-нет */
  short		podt;         /*Метка подтверждения*/
  short		dv,mv,gv;     //Дата валютирования
  class iceb_tu_str kodnr;    //Код не резидента

  int tipz; //1-платёжное требование 2-платёжное поручение

  //Уникальные реквизиты корректируемого (исходного)документа
  class iceb_tu_str nomdk_i;
  short ddi,mdi,gdi;
  //неуникальный реквизит корректировка которого приводит к исправлению записей в сопутствующих таблицах
  class iceb_tu_str kodop_i;

 REC()
  {
   clear();
  }

 void clear()
  {
   kodor.new_plus("");
   naior.new_plus("");
   gorod.new_plus("");
   kod.new_plus("");
   naiban.new_plus("");
   mfo.new_plus("");
   nsh.new_plus("");
   
   kodor1.new_plus("");
   naior1.new_plus("");
   gorod1.new_plus("");
   kod1.new_plus("");
   naiban1.new_plus("");
   mfo1.new_plus("");
   nsh1.new_plus("");

   nds=0.;
   shbzu.new_plus("");
   nomdk.new_plus("");
   sumd=0.;
   uslb=0.;
   dd=md=gd=0;
   kodop.new_plus("");
   ktoz=0;
   vremz=0;
   prov=1;
   podt=0;
   dv=mv=gv=0;
   kodnr.new_plus("");
   tipz=0;
   nomdk_i.new_plus("");
   ddi=mdi=0;
   kodop_i.new_plus("");
  }


};
