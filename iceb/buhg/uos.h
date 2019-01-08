/*$Id: uos.h,v 5.15 2013/05/17 14:56:13 sasa Exp $*/
/*18.10.2012    29.06.1996      Белых А.И.      uos.h
*/
class UOS
 {
  public:
   /*Данные по документам*/

   short           tipz; /*приход/расход*/
   short           tipzv; /*приход/расход встречный при внутреннем перемещении*/
   class iceb_tu_str datpr; /*дата документа*/
   class iceb_tu_str kontr; /*Код поставщика/получателя*/
   class iceb_tu_str naimo; /*Наименование организации*/

   class iceb_tu_str kprr;   /*Код прихода/расхода*/
   class iceb_tu_str naimpr; /*Наименование прихода/расхода*/

   int             pod;    /*Код подразделения*/
   int             pod1;   /*Код встречного подразделения*/
   class iceb_tu_str naipod; /*Наименование подразделения*/
   class iceb_tu_str naipod1; /*Наименование встречного подразделения*/

   int		kodol;      /*Код ответственного лица*/
   int		kodol1;     /*Код встречного ответственного лица*/
   class iceb_tu_str naikol; /*Наименование ответственного лица*/
   class iceb_tu_str naikol1;  /*Наименование встречного ответственного лица*/

   class iceb_tu_str nomdok; /*Номер документа*/
   class iceb_tu_str nomdokp; /*Номер встречного документа*/

   class iceb_tu_str osn; /*Основание*/
   class iceb_tu_str dover;
   short           dvd,mvd,gvd; //Дата выдачи доверенности
   class iceb_tu_str forop; //Код формы оплаты
   class iceb_tu_str naiforop; //Наименование формы оплаты
   class iceb_tu_str vid_dog; /*вид договора*/
   class iceb_tu_str nomnn; //Номер налоговой накладной
   short             dnn,mnn,gnn; //Дата выдачи налоговой накладной
   short           dvnn,mvnn,gvnn; /*Дата выписки налоговой накладной*/
   class iceb_tu_str sherez; //Через кого
   short		dop,mop,gop; //Дата оплаты
   float           pnds; //Процент НДС действовавший на момент создания документа
   short		nds;
   /*
                       0 20% НДС.
                       1 0% НДС реализация на територии Украины.
                       2 0% НДС экспорт.
                       3 0% Освобождение от НДС статья 5.
   */

   /*Постоянные данные по инвентарному номеру*/

   short		godv;      /*Год выпуска*/
   class iceb_tu_str naim; /*Нименование инвентарного номера*/
   class iceb_tu_str zaviz; /*Завод изготовитель*/
   class iceb_tu_str pasp;  /*Паспорт*/
   class iceb_tu_str model; /*Модель*/
   class iceb_tu_str zavnom; /*Заводской номер*/
   short		dv,mv,gv;   /*Дата ввода в эксплуатацию*/

   /*Переменные данные по инвентарному номеру*/

   class iceb_tu_str shetu; /*Счет учета*/
   class iceb_tu_str hzt;   /*Шифр затрат*/
   class iceb_tu_str hau;   /*Шифр аналитического учета*/
   class iceb_tu_str hna;   /*Шифр нормы амортотчислений для налогового учета*/
   float		popkf;   /*Поправочный коэффициент  для налогового учета*/
   class iceb_tu_str hnaby;   /*Шифр нормы амортотчислений для бухгалтерского учета*/
   float		popkfby;   /*Поправочный коэффициент*/
   int		soso;    /*Состояние объекта 
                                0-амортизация считается для бухучета и налогового учета
                                1-амортизация не считается для бух.учета и налогового учета
                                2-считается для бух.учета и не считается для налогового
                                3-не считается для бух учета и считается для налогового
                              */
                              
   class iceb_tu_str nomz;  /*Номерной знак*/
  UOS()
   {
    clear();
   }
  void clear()
   {
    tipz=tipzv=0;
    datpr.new_plus("");
    kontr.new_plus("");
    pod=pod1=0;
    naimo.new_plus("");
    kprr.new_plus("");
    naimpr.new_plus("");
    naipod.new_plus("");
    naipod1.new_plus("");
    kodol=kodol1=0;
    naikol.new_plus("");
    naikol1.new_plus("");
    nomdok.new_plus("");
    nomdokp.new_plus("");
    osn.new_plus("");
    dover.new_plus("");
    dvd=mvd=gvd=0;
    forop.new_plus("");
    naiforop.new_plus("");
    vid_dog.new_plus("");
    nomnn.new_plus("");
    dvnn=mvnn=gvnn=0;
    sherez.new_plus("");
    dop=mop=gop=0;
    pnds=0.;
    nds=0;
    godv=0;
    naim.new_plus("");
    zaviz.new_plus("");
    pasp.new_plus("");
    model.new_plus("");
    zavnom.new_plus("");
    dv=mv=gv=0;
    shetu.new_plus("");
    hzt.new_plus("");
    hau.new_plus("");
    hna.new_plus("");
    hnaby.new_plus("");    
    popkfby=popkf=1.;
    soso=0;
    nomz.new_plus("");
   }
 };
