/*$Id: iceb_tu_getcurrentdir.c,v 5.2 2013/05/17 14:56:40 sasa Exp $*/
/*04.09.2012	04.09.2012	Белых А.И.	iceb_tu_getcurrentdir.c
Получение текущего каталога
*/
#include <glib.h>
#include "util.h"
const char *iceb_tu_getcurrentdir()
{

static class iceb_tu_str current_dir("");

gchar *bros=g_get_current_dir();
current_dir.new_plus(bros);
g_free(bros);

return(current_dir.ravno());



}
