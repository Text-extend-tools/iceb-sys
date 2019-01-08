/*$Id: iceb_t_lno.c,v 5.1 2014/05/07 10:57:30 sasa Exp $*/
/*02.07.2015	30.04.2014	Белых А.И.	iceb_t_lno.c
Работа с файлом личных настроек оператора
*/
#include <sys/stat.h>
#include        <errno.h>
#include "buhl.h"

int iceb_t_lno(const char *imaf)
{
char strsql[1024];

class iceb_tu_str putnafil(g_get_home_dir());
putnafil.plus("/",ICEB_KAT_ALX);

umask(0000); 

if(mkdir(putnafil.ravno(),0775) != 0)
 {
//  printw("%d\n%d\n%d\n%d\n%d\n%d\n",EPERM,EFAULT,EACCES,ENAMETOOLONG,ENOENT,EEXIST);
  if(errno != EEXIST) //Уже есть такой каталог
   { 
    VVOD SOOB(1);

    SOOB.VVOD_SPISOK_add_MD(__FUNCTION__);

    SOOB.VVOD_SPISOK_add_MD(gettext("Не могу создать каталог !"));
    sprintf(strsql,"%d %s",errno,strerror(errno));

    SOOB.VVOD_SPISOK_add_MD(strsql);

    SOOB.VVOD_SPISOK_add_MD(putnafil.ravno());

    soobshw(&SOOB,stdscr,-1,-1,0,1);

    umask(ICEB_UMASK); /*Установка маски для записи и чтения группы*/

    return(1);
   }
 }

putnafil.plus("/",imaf);
 
umask(ICEB_UMASK); /*Установка маски для записи и чтения группы*/


class iceb_tu_str flag(imaf);
int metka_vr=0;
/*так как это личные настройки то оператор должен иметь к ним доступ всегда*/
//metka_vr=iceb_t_prn(); /*проверяем возможность корректировки настроек*/

//Установить флаг работы с файлом
class icebt_sql_flag flagrf;

if(metka_vr == 0)
if(flagrf.flag_on(flag.ravno()) != 0)
 {
  iceb_t_soob(gettext("С файлом уже работает другой оператор !"));
  metka_vr=1; /*режим просмотра*/
 }

FILE *ff;

if((ff=fopen(putnafil.ravno(),"r")) == NULL)
 {
  if(errno == ENOENT) /*нет такого файла*/
   {
    if(iceb_t_alxout(imaf,putnafil.ravno()) != 0)
     {
      sprintf(strsql,"%s-%s %s",__FUNCTION__,gettext("Не найден в базе файл"),imaf);
      iceb_t_soob(strsql);
      return(1);
     }
   }
  else
   {
    error_op_nfil(imaf,errno,__FUNCTION__);
    return(1);
   }
 }
else
  fclose(ff);


if(metka_vr == 0)
 {
  vizred(putnafil.ravno(),NULL); 
 }
else
 {
  prosf(putnafil.ravno());
 }





return(0);
}
/*************************************************/
/*меню выбора личных настроек оператора*/
/*************************************************/
void iceb_t_lnom()
{
int kom=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(gettext("Личные настройки оператора"));

MENU.VVOD_SPISOK_add_MD(gettext("Настройка передачи файлов по сети"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Другие настройки"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2

for(;;)
 {
  clear();

  if(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0)
   continue;
  class spis_oth oth;   
  switch (kom)
   {
    case 2:
    case -1 :
      return;

    case 0:
      iceb_t_lno("spis_komp.alx");
      break;

    case 1:
      iceb_t_lno("lno.alx");
      break;

    default:
      kom=0;
      break;
   }
 }



}
