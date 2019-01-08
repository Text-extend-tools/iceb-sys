/*$Id: iceb_t_tmptab.h,v 5.3 2013/05/17 14:56:29 sasa Exp $*/
/*03.03.2010	03.03.2010	Белых А.И.	iceb_t_tmptab.c
Класс для работы с временными таблицами
*/
class iceb_t_tmptab
 {
  private:
   int metka;
   class iceb_tu_str ima_tab;  

  public:

  iceb_t_tmptab();
  int create_tab(const char *ima_tab,const char *zapros);
  int drop_tab();
  ~iceb_t_tmptab();
  
 };
