/*$Id:$*/
/*05.09.2013	03.09.2013	Белых А.И.	iceb_t_getkue.h
получение cуммы в валюте
*/
class iceb_getkue_dat
 {
  public:
   class iceb_tu_str dataz;
   class iceb_tu_str kod_val;
   double kurs_ue;

   iceb_getkue_dat(const char *kodval)
    {
     kod_val.plus(kodval);
     kurs_ue=1.;
     dataz.plus("");
    }
   double fromue(const char *suma,const char *data);
   double toue(const char *suma,const char *data);
   double get(const char *suma,const char *data,int metka_r);
 };
