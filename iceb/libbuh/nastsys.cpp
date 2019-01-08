/*$Id: nastsys.c,v 5.21 2014/06/30 06:36:40 sasa Exp $*/
/*09.05.2015	23.09.1999	Белых А.И.	nastsys.c
Чтение системных настроек
Если вернули 0 файл прочитан
	     1-файл не найден
*/
#include "buhl.h"

class iceb_tu_str nameprinter(""); /*Имя принтера с которым будет работать программа lpr*/
int  kol_strok_na_liste=64; //Количесво строк на стандартном листе
int  kol_strok_na_liste_l=46; //Количесво строк на стандартном листе в ориентации ландшафт
extern int metka_utf8; /*0-односимвольная кодировка 1-utf8*/

extern int iceb_kod_podsystem;

int nastsys()
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[1024];
char		st1[1024];

if(iceb_kod_podsystem != 0)
 if(iceb_t_prr() != 0) /*проверка-разрешено ли работать с этой подсистемой оператору*/
  return(1);

kol_strok_na_liste=64;
kol_strok_na_liste_l=46;
//spis_print_a3.new_plus("");

sprintf(strsql,"select str from Alx where fil='nastsys.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob("Не найдены настройки nastsys.alx");
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  POLE(row_alx[0],st1,1,'|');

  if(SRAV(st1,"Количество строк на стандартном листе",0) == 0)
    {
     polen(row_alx[0],st1,sizeof(st1),2,'|');
     kol_strok_na_liste=atoi(st1);
     continue;
    }

  if(SRAV(st1,"Количество строк на стандартном листе в ориентации ландшафт",0) == 0)
    {
     polen(row_alx[0],st1,sizeof(st1),2,'|');
     kol_strok_na_liste_l=atoi(st1);
     continue;
    }
 }


/*переменная может быть установлена в другой подсистеме*/

if(getenv("PRINTER") != NULL)
 nameprinter.new_plus(getenv("PRINTER"));
else
 {
  nameprinter.new_plus(iceb_t_get_printer());
  if(nameprinter.ravno()[0] != '\0')
  if(setenv("PRINTER",nameprinter.ravno(),1) != 0)
   {
    sprintf(strsql,"%s-%s PRINTER %s",__FUNCTION__,gettext("Ошибка переустановки переменной"),nameprinter.ravno());
    iceb_t_soob(strsql);
   }
 
 }
return(0);

}
