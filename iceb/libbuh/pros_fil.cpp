/*$Id: pros_fil.c,v 5.8 2013-04-07 10:22:54 sasa Exp $*/
/*27.03.2013	26.10.2005	Белых А.И.	pros_fil.c
Просмотр перед печатью файла
*/
#include "buhl.h"


void pros_fil(const char *imaf,const char *klush_s_fil)
{
class iceb_tu_str ppsf;
class iceb_tu_str filtr_iceb_ps;

iceb_t_poldan("Просмотрщик PostScript файлов",&ppsf,"nastsys.alx");
iceb_t_poldan("Фильт Epson->PostScript",&filtr_iceb_ps,"nastsys.alx");

class iceb_tu_str imaf_tmp(imaf);
imaf_tmp.plus(".ps");

char bros[1024];

sprintf(bros,"%s %s %s > %s",filtr_iceb_ps.ravno(),klush_s_fil,imaf,imaf_tmp.ravno());

//if(runsystem1(bros,1) != 0)
if(iceb_t_system(bros,0) != 0)
 {
  unlink(imaf_tmp.ravno());
  return;
 }

sprintf(bros,"%s %s",ppsf.ravno(),imaf_tmp.ravno());
runsystem1(bros,1);

unlink(imaf_tmp.ravno());
}
