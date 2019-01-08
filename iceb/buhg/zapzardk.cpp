/*$Id: zapzardk.c,v 5.10 2013/05/17 14:56:16 sasa Exp $*/
/*24.11.2016	31.05.2006	Белых А.И.	zapzardk.c
Запись шапки документа для подсистемы "Заработная плата".
*/
#include "buhg.h"


int zapzardk(short dd,short md,short gd,
const char *nomdok,
int podr,
const char *koment,
short prn,
short dsd,short msd,short gsd)
{
char strsql[512];
time_t vrem;
time(&vrem);

class iceb_tu_str koment1(koment);

sprintf(strsql,"insert into Zardok values (%d,'%04d-%02d-%02d','%s',%d,'%s',%d,%ld,%d,'%04d-%02d-%02d')",
gsd,gd,md,dd,nomdok,podr,koment1.ravno_filtr(),iceb_t_getuid(),vrem,prn,gsd,msd,dsd);

return(sql_zapis(strsql,0,0));

}

