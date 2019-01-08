/*$Id: vvodkod.c,v 5.16 2013/09/26 09:43:56 sasa Exp $*/
/*06.09.2013	07.08.2001	Белых А.И.	vvodkod.c
Проверка введён ли хоть один код в список кодов
Возврашаем количество найденных записей или 0 если записей нет или не выбрали код
*/
#include	"buhl.h"


int vvodkod(const char *tabl,class iceb_tu_str *kod,class iceb_tu_str *naik,
int Y,int X, /*Координаты вывода меню*/
int kls) /*0 или максимальное количество строк в меню*/
{
char	strsql[512];
SQL_str row;
SQLCURSOR cur;
int	kolstr=0;
int		poz,komv;

sprintf(strsql,"select kod,naik from %s",tabl);
if((kolstr=sql_readkey(&bd,strsql,&row,&cur)) == 1)
 {
  kod->new_plus(row[0]);
  naik->new_plus(row[1]);
 }
if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не введена нормативно-справочная информация !"));
 }

if(kolstr > 1)
 {
  naik->new_plus("");
  poz=komv=0;

  if(dirmat1(tabl,kod,naik,kls,X,Y,2,&poz,&komv,"","\0") != 0)
    return(0);

 }


return(kolstr);

}
