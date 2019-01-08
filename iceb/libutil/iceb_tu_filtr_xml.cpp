/*$Id: iceb_tu_filtr_xml.c,v 5.4 2013/05/17 14:56:40 sasa Exp $*/
/*15.08.2011	13.10.2006	Белых А.И.	iceb_tu_filtr_xml.c
Преобразование символьных строк по требованию налоговиков для символьных строк

> - &gt;
< - &lt;
" - &quot;
' - &apos;
& - &amp;

*/
#include <string.h>
#include "util.h"

const char *iceb_tu_filtr_xml(const char *stroka)
{
static class iceb_tu_str str_out;
str_out.new_plus(stroka);


iceb_tu_zvstr(&str_out,"&","&amp;"); /*обязательно первым так как он встречается в образцах на которые заменяют*/

iceb_tu_zvstr(&str_out,">","&gt;");

iceb_tu_zvstr(&str_out,"<","&lt;");

iceb_tu_zvstr(&str_out,"\"","&quot;");

iceb_tu_zvstr(&str_out,"'","&apos;");


return(str_out.ravno());
}
