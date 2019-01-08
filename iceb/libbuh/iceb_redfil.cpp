/*$Id: iceb_redfil.c,v 1.18 2013/05/17 14:56:28 sasa Exp $*/
/*21.04.2011	19.10.2001	Белых А.И.	iceb_redfil.c
Проверка возможности редактирования файла и запуск редактирования
Если вернули 0 - вход в файл был
             1 - нет
*/
#include	"buhl.h"


extern char *imabaz;

int iceb_redfil(const char *imafil,int metka_r, /*0-проверять возможность доступа 1-не проверять*/
const char *editor)
{
class iceb_tu_str flag;
int metka_vr=0;

if(metka_r == 0)
  metka_vr=iceb_t_prn(); /*проверяем возможность корректировки настроек*/

//Установить флаг работы с файлом
flag.plus(imabaz);
flag.plus(imafil);
class icebt_sql_flag flagrf;

if(metka_vr == 0)
if(flagrf.flag_on(flag.ravno()) != 0)
 {
  iceb_t_soob(gettext("С файлом уже работает другой оператор !"));
  metka_vr=1; /*режим просмотра*/
 }

if(iceb_t_alxout(imafil) == 0)
 {
  rename(imafil,flag.ravno());
  
  if(metka_vr == 0)
   {
    vizred(flag.ravno(),editor); 
    rename(flag.ravno(),imafil);
    iceb_t_alxin(imafil);
   }
  else
   {
    prosf(flag.ravno());
    rename(flag.ravno(),imafil);
   }
 }
else
 {
  char buf[1024];
  sprintf(buf,"%s %s!",gettext("Не найден файл"),imafil);
  iceb_t_soob(buf);
 }
unlink(imafil);
return(0);
}
/*******************************************/

int iceb_redfil(const char *imafil,int metka_r) /*0-проверять возможность доступа 1-не проверять*/
{

return(iceb_redfil(imafil,metka_r,NULL));

}