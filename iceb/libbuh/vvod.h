/*$Id: vvod.h,v 5.13 2013/05/17 14:56:32 sasa Exp $*/
/*11.10.2010	01.02.2002	Белых А.И.	vvod.h
Класс работы с меню
*/

const int  iceb_CSZ=0;  //0-Цвет строки заголовка
const int  iceb_CFV=1;  //1-Цвет открытой строки для ввода
const int  iceb_CB=2;   //2-Цвет букв нормальный
const int  iceb_CFM=3;  //3-Цвет меню
const int  iceb_CFS=4;  //4-Цвет выбранной строки
const int  iceb_CFR=5;  //5-Цвет рамки

class VVOD
 {

private:

  long    CSZ;        //0-Цвет строки заголовка
  long    CFV;        //1-Цвет открытой строки для ввода
  long    CB;         //2-Цвет букв нормальный
  long 	  CFM;        //3-Цвет меню
  long    CFS;        //4-Цвет выбранной строки
  long    CFR;        //5-Цвет рамки
  char    PO[8];      /*Горизонтальная полка рамки меню*/
  char    VE[8];      /*Вертикальная стенка рамки меню*/
  char    PVU[8],LVU[8];  /*Правый верхний и левый верхний уголок*/
  char    PNU[8],LNU[8];  /*Правый нижний и левый нижний уголок*/
  char    TENN;        /*Чем тенить*/


  class SPISOK  ZAG;     //Список строк заголовка меню
  class SPISOK  MD;      //Список реквизитов меню
  class SPISOK  data;    //Список с введенными реквизитами меню
  class masiv_din_int dls; /*Длинна строки в символах*/


public:

  int MDLS; /*Максимальная длинна строки в символах*/
  int KLST; /*Количество заданных строк*/
  int dlr;  /*Длинна реплик в символах*/
  int PY;  /*Позиция левгого верхнего угла меню по вертикали*/
  int PX;  /*Позиция левгого верхнего угла меню по гризонтали*/
  
  VVOD(int); //Конструктор
 

  int  vvod(int,int,int,int,int);  

  //Рамка на стандартном окне
//  void vramka(int*,int*,int*,int*,int*,int);
  void vramka(int,int,int);

  //Рамка на указанном окне
//  void wvramka(int*,int*,int*,int*,int*,int*,int*,int,int,WINDOW*,WINDOW**);
  void wvramka(int,int,int*,int*,int,int,int kolih_simv,WINDOW*,WINDOW**);

  void VVOD_SPISOK_add_ZAG(const char *stroka)
   {
    ZAG.plus(stroka);
   }   
  void VVOD_SPISOK_add_ZAG(const char *stroka,int dlinna)
   {
//    ZAG.plus(stroka,dlinna);
    ZAG.plus(stroka,iceb_tu_kolbait(dlinna,stroka));
   }   

  //Записать строку меню
  void VVOD_SPISOK_add_MD(const char *stroka)
   {
    MD.plus(stroka);
   }   
  void VVOD_SPISOK_add_data(int dlina)
   {
    data.plus(dlina*sizeof(wchar_t));
    dls.plus(dlina,-1);
   }   

  void VVOD_SPISOK_add_data(const char *stroka,int dlina)
   {
    data.plus(stroka,dlina*sizeof(wchar_t));
    dls.plus(dlina,-1);
   }   
  void VVOD_SPISOK_add_data(int stroka,int dlina)
   {
    char buf[512];
    sprintf(buf,"%d",stroka);
    data.plus(buf,dlina*sizeof(wchar_t));
    dls.plus(dlina,-1);
   }   
  /*Cоздать новый элемент списка и записать в него данные*/
  void data_sp_plus(const char *stroka,int dlina)
   {
    data.plus(stroka,dlina*sizeof(wchar_t));
    dls.plus(dlina,-1);
   }   

  //Записать в список данных
  void VVOD_SPISOK_zapis_data(int nomer,const char *stroka);

  //Записать в список данных
  void data_plus(int nomer,const char *stroka)
   {
    VVOD_SPISOK_zapis_data(nomer,stroka);
   }   

  //Добавить к строке 
  void data_strcat(int nomer,const char *stroka);

  //Записать в список данных через запятую
  void data_z_plus(int nomer,const char *stroka);

  //Записать в список данных через запятую
  void data_z_plus(int nomer,long stroka);

  //Записать в список данных через запятую
  void data_z_plus(int nomer,int stroka);
  
  //Записать в список данных
  void data_plus(int nomer,double stroka);

  //Записать в список данных с указанной разрядностью после запятой
  void data_plus(int nomer,double stroka,int pz);

  //Записать в список данных
  void data_plus(int nomer,int stroka);

  //Записать в список данных
  void data_plus(int nomer,long stroka);

  //Очистить список данных
  void VVOD_clear_data(void);

  //Записать в список меню
  void VVOD_SPISOK_zapis_MD(int nomer,const char *stroka)
   {
//    strcpy(MD.ravno(nomer),stroka);
    MD.copy(nomer,stroka);
   }   

  //Записать в список заголовка
  void VVOD_SPISOK_zapis_ZAG(int nomer,const char *stroka)
   {
//    strcpy(ZAG.ravno(nomer),stroka);
    ZAG.copy(nomer,stroka);
   }   

  int kolbait(int nom)
   {
    return(data.kolbait(nom));
   }

  //Вернуть данные меню
  const char *VVOD_SPISOK_return_data(int nomer)
   {
    return(data.ravno(nomer));
   }
  //Вернуть данные меню
  const char *data_ravno(int nomer)
   {
    return(data.ravno(nomer));
   }


  //Вернуть данные меню отфильтрованные для SQL-запроса
  const char *data_ravno_filtr(int nomer)
   {
    return(data.ravno_filtr(nomer));
   }

  //Вернуть данные меню
  double data_ravno_atof(int nomer);

  //Вернуть год есили в поле записана дата в формате дд.мм.гггг
  short data_ravno_god(int nomer);
  
  //Вернуть данные меню
  int data_ravno_atoi(int nomer)
   {
    return(atoi(data.ravno(nomer)));
   }
  //Вернуть данные меню
  long data_ravno_atol(int nomer)
   {
    return(atol(data.ravno(nomer)));
   }

  //Вернуть данные меню отфильтрованные для SQL запроса
  const char *VVOD_SPISOK_return_data_filtr(int nomer)
   {
    return(data.ravno_filtr(nomer));
   }

  //Вернуть строку меню
  const char *VVOD_SPISOK_return_MD(int nomer)
   {
    return(MD.ravno(nomer));
   }
  //Вернуть размерность данных
  int VVOD_SPISOK_return_dls(int nomer)
   {
    return(dls.ravno(nomer));
   }
  //Вернуть размерность данных
  int data_dlinna(int nomer)
   {
    return(dls.ravno(nomer));
   }

  //Вернуть количество строк в меню
  int VVOD_SPISOK_return_kolstr(void)
   {
    return(MD.kolih());
   }

  //Вернуть количество строк в заголовке меню
  int VVOD_SPISOK_return_kolstrZAG(void)
   {
    return(ZAG.kolih());
   }

  long VVOD_return_cs(int);

  /*Проверить дату*/
  int prov_dat(int nomer,int metka)
   {
    return(data.prov_dat(nomer,metka));
   }
  int prov_dat1(int nomer,int metka)
   {
    return(data.prov_dat1(nomer,metka));
   }
  /*Вернуть дату в SQL-формате*/
  const char *ravno_sqldat(int nomer)
   {
    return(data.ravno_sqldat(nomer));
   }
  /*Получить год*/
  short ravno_god(int nomer)
   {
    return(data.ravno_god(nomer));
   }
  /*Очистить массивы*/
  void VVOD_delete(void)
   {
    data.free_class();
    MD.free_class();
    ZAG.free_class();
    dls.free_class();
   }
  //Очистить 
  void VVOD_delete_MD(void)
   {
    MD.free_class();
   }
  //Очистить 
  void VVOD_delete_ZAG(void)
   {
    ZAG.free_class();
   }
  /*чтение клавиатурного ввода*/
  int vgetstr(int nom,WINDOW *okno);

  /*Вывод экран строки меню*/
  void vprintw(int nomer_stoki,WINDOW *win1);
  /*Очистить строку пробелами*/
  void vprintw_clear(int nomer_stoki,WINDOW *win1);
  /*Длинна видимой части строки в символах для ввода данных*/
  int dvh_str(int nomer_str);
  /*Записать дату*/
  void data_plus_dat(int nomer,short d,short m,short g);
  /*вернуть количество байт выделенное в троке для ввода данных*/
  int data_kolbait(int nomer);  
 };



