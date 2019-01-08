/*$Id: xrnn_poi.h,v 5.15 2013-04-07 10:22:44 sasa Exp $*/
/*06.03.2013	19.02.2007	Белых А.И.	xrnn_poi.h
Реквизиты поиска нужных записей в реестрах выданных и полученных налоговых накладных
*/


class xrnn_poi
 {
  public:
   short dn,mn,gn;
   short dk,mk,gk;
   
   class iceb_tu_str nnn;  //Номер налоговой накладной
   class iceb_tu_str nomdok; //Номер документа первоисточника
   class iceb_tu_str inn;
   class iceb_tu_str naim_kontr;
   class iceb_tu_str koment;
   class iceb_tu_str kodgr;  // для распечатки полученных документов
   class iceb_tu_str kodgr1; //для поиска выданных документов
   class iceb_tu_str podsistema;
   class iceb_tu_str kodop; //Код операции в документе
   short dvnn,mvnn,gvnn; /*Дата выписки налоговой накладной- начало периода*/
   short dvnk,mvnk,gvnk; /*Дата выписки налоговой накладной - конец периода*/
   class iceb_tu_str proc_dot; /*Процент дотации*/
   class iceb_tu_str viddok; /*Вид документа*/
   short metka_poi; //0-нет поиска 1-есть   

   short metka_sort; //0-по номерам налоговых накладных 1 по номерам записей (только для выданых накладных)
   class iceb_tu_str nomer_porcii; /*номер порции документа*/
   
   class iceb_tu_str pr_ras;
   class masiv_din_int nomzap_v; //уникальные номера записей отмеченные в списке клавишей ins
   int metka_ins; //0-все записи 1-только отмеченные 2-только не отмеченные
   double suma[2];
  xrnn_poi()
   {
    clear_data();    
   }

 void clear_data()
  {
   nomer_porcii.new_plus("1");
   viddok.new_plus("");
   podsistema.new_plus("");
   kodgr.new_plus("");
   kodgr1.new_plus("");
   nomdok.new_plus("");
   metka_poi=0;
   dn=mn=gn=dk=mk=gk=0;
   dvnn=mvnn=gvnn=0;
   dvnk=mvnk=gvnk=0;
   nnn.new_plus("");
   inn.new_plus("");
   naim_kontr.new_plus("");
   koment.new_plus("");
   pr_ras.new_plus("");
   kodop.new_plus("");
   proc_dot.new_plus("");
   metka_sort=0;  
   metka_ins=0;
  }
 };
