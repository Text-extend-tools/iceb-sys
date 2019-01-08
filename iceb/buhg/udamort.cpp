/* $Id: udamort.c,v 5.9 2013/05/17 14:56:13 sasa Exp $ */
/*16.08.2011    04.07.1996      Белых А.И.      udamort.c
Удаление амортизационных отчислений к документа
*/
#include        "buhg.h"


void udamort(int in)
{
char		strsql[512];

sprintf(strsql,"delete from Uosamor where  innom=%d",in);

sql_zapis(strsql,1,0);

sprintf(strsql,"delete from Uosamor1 where  innom=%d",in);

sql_zapis(strsql,1,0);

}
