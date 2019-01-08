/*$Id: iceb_tu_fio.c,v 5.5 2013/08/13 05:50:32 sasa Exp $*/
/*22.07.2013	28.07.2008	Белых А.И.	iceb_tu_fio.c
Получение фамилии с инициалами
*/
#include "util.h"

void iceb_tu_fio(const char *fio,class iceb_tu_str *fio_in)
{


if(polen(fio,fio_in,1,' ') == 0)
 {
  class iceb_tu_str imq("");
  class iceb_tu_str otch("");
    
  polen(fio,&imq,2,' ');
  polen(fio,&otch,3,' ');
  fio_in->plus(" ");
  fio_in->plus(imq.ravno(),1);
  fio_in->plus(".");
  fio_in->plus(otch.ravno(),1);
  fio_in->plus(".");
 }
else
 {
  fio_in->new_plus(fio);
 }  

}
