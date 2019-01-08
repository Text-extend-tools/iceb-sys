/*$Id: iceb_tu_file.h,v 5.3 2013/05/17 14:56:40 sasa Exp $*/
/*04.12.2007	04.12.2007	Белых А.И.	iceb_tu_file.h
Класс для гарантированного закрытия файла
*/

class iceb_tu_file
 {
  public:
   FILE *ff;
  
  iceb_tu_file()
   {
    ff=NULL;
   }
  ~iceb_tu_file()
   {
    close();
   }
  void close()
   {
    if(ff != NULL)
     fclose(ff);
    ff=NULL;
   }
 };
