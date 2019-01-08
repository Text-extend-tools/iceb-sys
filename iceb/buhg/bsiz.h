/*$Id: bsiz.h,v 5.9 2013/05/17 14:55:56 sasa Exp $*/
/*05.11.2007	03.10.2004	Белых А.И.	bsiz.h
Класс для расчёта балансовой стоимости основных средств
*/


class bsiz_per
 {
  public:

   double bs_nah_per;              //Балансовая стоимость на начало периода
   double izmen_iznosa_nah_per;    //Изменение износа на начало периода
   double amort_nah_per;           //Амортизация на начало периода
   
   double bs_kon_per;              //Балансовая стоимость на конец периода
   double izmen_iznosa_kon_per;    //Изменение износа на конец периода
   double amort_kon_per;           //Амортизация на конец периода

   double bs_prih_za_per;          //Приход за период балансовой стоимости
   double bs_rash_za_per;          //Расход за период балансовой стоимости

   double iznos_prih_za_per;        //Приход износа за период
   double iznos_rash_za_per;        //Расход износа за период

   double bs_raznica;              //Разница балансовой стоимости 
   double izmen_iznosa_raznica;    //Разница изменения износа
   double amort_raznica;           //Разница амортизации

   bsiz_per()
    {
     clear_data();
    }

   void clear_data()
    {
     bs_nah_per=izmen_iznosa_nah_per=amort_nah_per=0.;
     bs_kon_per=izmen_iznosa_kon_per=amort_kon_per=0.;
     bs_raznica=izmen_iznosa_raznica=amort_raznica=0.;
     bs_prih_za_per=bs_rash_za_per=0.;
     iznos_prih_za_per=iznos_rash_za_per=0.;
    }

  void plus(class bsiz_per *data)
   {
     bs_nah_per+=data->bs_nah_per;              
     izmen_iznosa_nah_per+=data->izmen_iznosa_nah_per;
     amort_nah_per+=data->amort_nah_per;
     
     bs_kon_per+=data->bs_kon_per;
     izmen_iznosa_kon_per+=data->izmen_iznosa_kon_per;
     amort_kon_per+=data->amort_kon_per;
     
     bs_prih_za_per+=data->bs_prih_za_per;
     bs_rash_za_per+=data->bs_rash_za_per;
     
     iznos_prih_za_per+=data->iznos_prih_za_per;
     iznos_rash_za_per+=data->iznos_rash_za_per;
     
     bs_raznica+=data->bs_raznica;
     izmen_iznosa_raznica+=data->izmen_iznosa_raznica;
     amort_raznica+=data->amort_raznica;
   }
  void class_printw()
   {
    printw("%s-%s:%.2f\n",__FUNCTION__,gettext("Балансовая стоимость на начало периода"),bs_nah_per);
    printw("%s-%s:%.2f\n",__FUNCTION__,gettext("Изменение износа на начало периода"),izmen_iznosa_nah_per);
    printw("%s-%s:%.2f\n",__FUNCTION__,gettext("Амортизация на начало периода"),amort_nah_per);

    printw("%s-%s:%.2f\n",__FUNCTION__,gettext("Балансовая стоимость на конец периода"),bs_kon_per);
    printw("%s-%s:%.2f\n",__FUNCTION__,gettext("Изменение износа на конец периода"),izmen_iznosa_kon_per);
    printw("%s-%s:%.2f\n",__FUNCTION__,gettext("Амортизация на конец периода"),amort_kon_per);

    printw("%s-%s:%.2f\n",__FUNCTION__,gettext("Приход за период балансовой стоимости"),bs_prih_za_per);
    printw("%s-%s:%.2f\n",__FUNCTION__,gettext("Расход за период балансовой стоимости"),bs_rash_za_per);

    printw("%s-%s:%.2f\n",__FUNCTION__,gettext("Разница балансовой стоимости"),bs_raznica);
    printw("%s-%s:%.2f\n",__FUNCTION__,gettext("Разница изменений износа"),izmen_iznosa_raznica);
    printw("%s-%s:%.2f\n",__FUNCTION__,gettext("Разница амортизации"),amort_raznica);
    refresh();
   }
 };

class bsiz_data
 {
  public:

 //Налоговый учёт
   class bsiz_per nu;

//Бухгалтерский учёт
   class bsiz_per bu;

   bsiz_data()
    {
     clear_data();
    }

   void clear_data()
    {
     nu.clear_data();
     bu.clear_data();     
    }

  void plus(class bsiz_data *data)
   {
    nu.plus(&data->nu);
    bu.plus(&data->bu);
   }   
 };
