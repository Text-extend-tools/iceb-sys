/* $Id: provuin.c,v 5.9 2013/05/17 14:56:08 sasa Exp $ */
/*07.09.2011    04.07.1996      Белых А.И.      provuin.c
Проверка возможности удаления инвентарного номера
Возвращаем 0- если можно удалять
	   1- если нет
*/
#include        "buhg.h"


int provuin(int in)
{
char		strsql[512];

sprintf(strsql,"select innom from Uosdok1 where innom=%d",in);

int voz=readkey(strsql);

if(voz  > 0 || voz < 0)
 return(1);

/*Если нет ни одной записи в документах - можно удалять*/

return(0);
}
