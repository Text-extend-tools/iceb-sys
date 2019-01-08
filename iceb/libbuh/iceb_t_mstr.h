/*$Id: iceb_t_mstr.h,v 5.6 2013/05/17 14:56:28 sasa Exp $*/
/*16.02.2009	11.09.2007	Белых А.И.	iceb_t_mstr.h
Класс работы с меню срок произвольной длинны
*/



class iceb_t_mstr
 {
  private:
    long    CB;         /*Цвет букв нормальный*/
    long    CFM;        /*Цвет меню*/
    long    CFS;        /*Цвет выбранной строки*/
    long    CFR;        /*Цвет рамки*/
    char    PO[8];      /*Горизонтальная полка рамки меню*/
    char    VE[8];      /*Вертикальная стенка рамки меню*/
    char    PVU[8],LVU[8];  /*Правый верхний и левый верхний уголок*/
    char    PNU[8],LNU[8];  /*Правый нижний и левый нижний уголок*/
    char    TENN;        /*Чем тенить*/

  public:
   class SPISOK spis_str;
   //конструктор   
   iceb_t_mstr(int);

   int menu(int,int,int,int (*)(char[]),char[],int,int*,const char*,int);

 };
