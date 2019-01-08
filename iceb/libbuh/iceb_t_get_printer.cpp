/*$Id: iceb_t_get_printer.c,v 5.1 2014/05/07 10:57:29 sasa Exp $*/
/*02.07.2015	30.04.2014	Белых А.И.	iceb_t_get_printer.c
Получение имени принтера по умолчанию
*/

#include "buhl.h"

const char *iceb_t_get_printer()
{
static class iceb_tu_str name_printer("");

class iceb_tu_str putnafil(g_get_home_dir());
putnafil.plus("/",ICEB_KAT_ALX);
putnafil.plus("/lno.alx");

//printw("\n%s-%s\n",__FUNCTION__,putnafil.ravno());
//OSTANOV();
if(poldan("Принтер по умолчанию",&name_printer,putnafil.ravno()) == 0)
 return(name_printer.ravno());
else
 return("");



}
