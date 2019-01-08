/* $Id: provkont.c,v 5.5 2013-04-07 10:22:43 sasa Exp $ */
/*16.09.1999	16.09.1999	Белых А.И.	provkont.c
Проверка наличия кода контрагента в списке счета и в общем списке
Если вернули 0 есть
	     1 нет в списке счета
	     2 нет в общем списке
	     3 нет ни в списке счета ни в общем списке
*/
#include	"buhg.h"

int provkont(char sh[], //Счет
char kont[]) //Код контрагента
{
char		strsql[512];
int		voz;

voz=0;

sprintf(strsql,"select kodkon from Skontr where ns='%s' and \
kodkon='%s'",sh,kont);
if(sql_readkey(&bd,strsql) != 1)
 voz+=1;

sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kont);
if(sql_readkey(&bd,strsql) != 1)
 voz+=2;

return(voz);

}
