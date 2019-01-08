/*$Id: iceb_t_fopen.h,v 5.3 2013/08/13 05:50:22 sasa Exp $*/
/*12.03.2013	12.03.2013	Белых А.И.	iceb_t_fopen.h
Класс для отрытия/закрытия файла
*/
class iceb_fopen
 {
  public:
   FILE *ff;

  iceb_fopen() /*конструктор*/
   {
    ff=NULL;
   }

  ~iceb_fopen() /*деструктор*/
   {
    end();
   }

  int start(const char *imaf,const char *wr);

  
  void end()
   {
    if(ff != NULL)
     fclose(ff);
    ff=NULL;
   }
  
 };
