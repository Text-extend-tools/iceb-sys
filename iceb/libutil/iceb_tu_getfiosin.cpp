/*$Id:$*/
/*28.05.2016	28.05.2016	Белых А.И.	iceb_tu_getfiosin.c
получить из фамилии имени отчества фамилию с инициалами
*/
#include "util.h"

const char *iceb_tu_getfiosin(const char *fio,int metka) /*0-инициалы после фамилии 1-до*/
{
static char stroka[512];
memset(stroka,'\0',sizeof(stroka));
class iceb_tu_str famil("");
class iceb_tu_str ima("");
class iceb_tu_str oth("");

if(polen(fio,&famil,1,' ') != 0)
 famil.new_plus(fio);
polen(fio,&ima,2,' ');
polen(fio,&oth,3,' ');

if(metka == 0)
 sprintf(stroka,"%s %.*s. %.*s.",
 famil.ravno(),
 iceb_tu_kolbait(1,ima.ravno()),
 ima.ravno(),
 iceb_tu_kolbait(1,oth.ravno()),
 oth.ravno());

if(metka == 1)
 sprintf(stroka,"%.*s. %.*s. %s",
 iceb_tu_kolbait(1,ima.ravno()),
 ima.ravno(),
 iceb_tu_kolbait(1,oth.ravno()),
 oth.ravno(),
 famil.ravno());

return(stroka);
}