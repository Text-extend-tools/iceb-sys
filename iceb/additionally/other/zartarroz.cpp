/*$Id: zartarroz.c,v 5.7 2011-01-13 13:43:25 sasa1 Exp $*/
/*09.12.2010	24.11.2008	Белых А.И.	zartarroz.c
Чтение из файла списка тарифных розрядов и коэффициентов к ним
Поиск по разряду нужного коэффициента
Распечатка таблицы
*/
#include <errno.h>
#include "buhg.h"
#ifdef	HAVE_UNISTD_H
#include        <unistd.h>
#endif
class zartarroz_data
 {
  public:
   class masiv_din_double kof;
   class masiv_din_int    roz;

   void clear()
    {
     kof.free_class();
     roz.free_class();
    }

 };

class zartarroz_data tarroz_kof;

int zartarroz()
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

tarroz_kof.clear();

sprintf(strsql,"select str from Alx where fil='zartarroz.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  return(1);
 }


class iceb_tu_str roz;
class iceb_tu_str kof;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(polen(row_alx[0],&roz,1,'|') != 0)
   continue;
  
  if(polen(row_alx[0],&kof,2,'|') != 0)
   continue;

  if(roz.ravno_atoi() == 0)
   continue;
  tarroz_kof.roz.plus(roz.ravno_atoi());  
  tarroz_kof.kof.plus(kof.ravno_atof());  
 }
return(0);
}
/*******************************/
/*Найти коэффициент для розряда*/
/*******************************/
/*Возвращаем коэффициент или -1
  если не нашди*/
  
float zartarroz(int roz)
{
int nomer=0;
if((nomer=tarroz_kof.roz.find(roz)) < 0)
 return(-1.);
return(tarroz_kof.kof.ravno(nomer));
}
/*********************************/
/*распечатка таблицы*/
/************************/
void zartarroz(double min_zar,FILE *ff)
{
zartarroz(); /*читаем настройки*/

fprintf(ff,"Мінімальна заробітна плата:%.2f\n",min_zar);
fprintf(ff,"\
---------------------------\n");
fprintf(ff,"\
Тарифні|Тарифні| Оклади,  |\n\
розряди|коефф. |тар.ставк.|\n");
fprintf(ff,"\
---------------------------\n");
double oklad=0.;

for(int ii=0; ii < tarroz_kof.roz.kolih(); ii++)
 {
  oklad=min_zar*tarroz_kof.kof.ravno(ii);
  oklad=okrug(oklad,1.);
  fprintf(ff,"%7d %7.7g %10.2f\n",tarroz_kof.roz.ravno(ii),tarroz_kof.kof.ravno(ii),oklad);
 }
fprintf(ff,"\
---------------------------\n");

}
