/*$Id: ostuos.h,v 5.5 2013/05/17 14:56:06 sasa Exp $*/
/*21.10.2005	21.10.2005	Белых А.И.	ostuos.h
Класс для получения движения в суммовом исчислении 
по основным средствам
*/

class ostuos_data
 {
  public:
  short           ostg[4]; //Оборот в количестве от стартового года до дати конца периода
  short           ostm[4]; //Оборот в количестве за период
  double          ostgc[4]; //Оборот в стоимости от стартового года до дати конца периода для налогового учёта
  double          ostmc[4]; //Оборот в стоимости за период для налогового учёта
  double          ostgcby[4]; //Оборот в стоимости от стартового года до дати конца периода для бухгалтерского учёта
  double          ostmcby[4]; //Оборот в стоимости за период для бухгалтерского учёта
  ostuos_data()
   {
    clear();
   }
  void clear()
   {
    for(int i=0; i < 4 ; i++)
     {
      ostgc[i]=ostmc[i]=ostgcby[i]=ostmcby[i]=0.;
      ostg[i]=ostm[i]=0;
     }
   }
 };