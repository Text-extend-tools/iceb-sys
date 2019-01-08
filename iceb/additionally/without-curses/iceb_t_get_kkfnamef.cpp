/*$Id:$*/
/*09.02.2016	06.02.2016	Белых А.И.	iceb_t_get_kkfname.c
Получить код коднтрагента для заданного наименования организации
Если вернули 0 - уже был
             1 - добавлен в список
            -1 - не добавлен 

*/
#include "bezcurses.h"

int iceb_t_get_kkfname(const char *naim, /*наименование контрагента*/
const char *kodkon, /*рекомендованный код контрагента*/
class iceb_tu_str *kkontr, /*код контрагента под которым записано наименование в общем сприске контрагентов*/
int *nom_kod_kontr) /*номер кода контрагента с которого нужно начать проверять список контрагентов*/
{
class iceb_tu_str naim1(naim);
class iceb_tu_str kodkon1(kodkon);
char strsql[2048];
SQL_str row;
class SQLCURSOR cur;

if(kodkon[0] != '\0') 
 {

  /*чтобы импорт был всегда правильным можно добавить точку в конец кода контрагента
   тогда перегрузка будет всегда правильная даже если будет изменено наименование контрагента
   в той базе откуда импортируется информация*/
  
  int kolbait=strlen(kodkon);
  if(kodkon[kolbait-1] == '.')
   {
    sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",kodkon1.ravno_filtr());
    if(readkey(strsql) == 1) /*есть в справочнике*/
     {
     
      kkontr->new_plus(kodkon1.ravno()); /*только так*/

      return(0);
     }
   }  
 }

/*Проверяем есть ли такое короткое наименование в списке контрагентов*/
sprintf(strsql,"select kodkon from Kontragent where naikon='%s'",naim1.ravno_filtr());
if(readkey(strsql,&row,&cur) == 1)
 {
  kkontr->new_plus(row[0]);
//  printw("%s-нашли такой контрагент в списке по короткому наименованию\n",__FUNCTION__);
  return(0);
 }
else
 {
  
  /*Проверяем есть ли такое полное наименование в списке контрагентов*/
  sprintf(strsql,"select kodkon from Kontragent where pnaim='%s'",naim1.ravno_filtr());
  if(readkey(strsql,&row,&cur) > 0) /*теоретически может быть больше одной записи*/
   {
    kkontr->new_plus(row[0]);
//   printw("%s-нашли такой контрагент в списке по полному наименованию\n",__FUNCTION__);
    return(0);
   }
  else
   {

    /*проверяем свободен ли рекомендованный код контрагента*/
    class lock_tables klk("LOCK TABLES Kontragent WRITE,icebuser READ");
    
    if(kodkon1.getdlinna() > 1) /*если рекомендованый код есть*/
     {
      sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",kodkon1.ravno_filtr());
      if(readkey(strsql) >= 1) /*код занят другим наименованием*/
       {
        kkontr->new_plus(iceb_t_getnkontr(*nom_kod_kontr));
        *nom_kod_kontr=kkontr->ravno_atoi();
//   printw("%s-код занят нашли свободный код\n",__FUNCTION__);
       }
      else /*код свободен*/
       kkontr->new_plus(kodkon1.ravno());    
     }
    else
     {
      /*рекомендованный код контрагента не указан*/
      kkontr->new_plus(iceb_t_getnkontr(*nom_kod_kontr));
      *nom_kod_kontr=kkontr->ravno_atoi();
//      printw("%s-нашли свободный код\n",__FUNCTION__);
     }
    
    /*записываем контрагента в справочник контрагентов*/
    sprintf(strsql,"insert into Kontragent (kodkon,naikon,pnaim,ktoz,vrem) values ('%s','%s','%s',%d,%ld)",
    kkontr->ravno_filtr(),naim1.ravno_filtr(),naim1.ravno_filtr(),iceb_t_getuid(),time(NULL));
    if(sql_zapis(strsql,1,0) != 0)
      return(-1);
    else
      return(1);
   }
 }


return(0);
}
