/*$Id: spisok.h,v 5.11 2013/05/17 14:56:42 sasa Exp $*/
/*29.09.2012	09.04.2002	Белых А.И.	spisok.h
Односвязный список
*/
class STROKA
 {
  friend class SPISOK;
  char *ukstr;       /*Содержимое записи*/
  char *str_filtr;   /*строка в которую переписывается отфильтрованное содержимое записи*/
  int kolbait;
  class STROKA *next;
    
  STROKA(const char *masiv,int dlina,int metka);
  ~STROKA();
 };

class SPISOK
 {
  private:
    int metka;  //0-заголовок в начале списка 1- в конце
    class STROKA *zag;
    class STROKA *kon;  

     //Создать список
    void SPISOK_soz_spisok(int);
    void SPISOK_soz_spisok();

    //Удалить список
    void SPISOK_delete();

  public:

   //Конструктор
   SPISOK(int mr);

   //Конструктор без параметров
   SPISOK();

   //Деструктор
   ~SPISOK();



   //Добавить элемент в список 
   int plus(const char *masiv);

   //Добавить элемено в список без перевода строки если он есть
   int plus_bps(const char *masiv);
   
   /*Добавить данные к уже существующей строке*/
   int plus_strcat(int nomer,const char *masiv);
   /*Записать вместо существующих данных*/   
   void plus(int nomer,const char *masiv);
   
   //Добавить элемент в список 
   int plus(const char *masiv,int dlina);
   
   //Добавить элемент в список 
   int plus(int dlina);

   
   //Вернуть адрес элемента из списка
   const char *ravno(int nom);
   /*Вернуть в виде целого числа*/
   int ravno_atoi(int nom);
   /*Вернуть в виде дробного числа*/
   double ravno_atof(int nom);

   /*Получить в отфильтровнном виде для sql запроса*/
   const char *ravno_filtr(int nom);
   
   //Вернуть количество элементов в списке
   int kolih(void);
   
   //Найти в списке нужную строку с учетом регистра с учётом метки сравнения
   int find(const char *obr,int metka_srav);

   //Найти в списке нужную строку с учетом регистра с полным сравнением
   int find(const char *obr);

   //Найти в списке нужную строку без учета регистра c полным сравнением
   int find_r(const char *obr);
   //Найти в списке нужную строку без учета регистра  c полным или неполным сравнением
   int find_r(const char *obr,int metka);
   
   //Создать новый список удалив старый
   void free_class(void);
   
   /*Записать данные в уже существующий елемент списка с удалением его предыдущего содержимого*/
   int new_plus(int nomer,const char*);
   /*Удалить из списка*/
   int del(int nomer); 

   /*Вернуть адрес всего класса*/
   void *return_all(int nomer);

   /*Проверить дату*/
   int prov_dat(int nomer,int metka);
   int prov_dat1(int nomer,int metka);

   /*Вернуть дату в SQL-формате*/
   const char *ravno_sqldat(int nomer);
   /*Вернуть год*/
   short ravno_god(int nomer);
   /*скопировать в уже существующюю строку*/
   int copy(int nomer,const char *str);
   /*соединить строки*/
   int cat(int nomer,const char *str);
   /*Вернуть размер строки в байтах*/
   int kolbait(int nomer);
 };
