/*$Id: pib_str.c,v 5.9 2013/08/13 05:49:50 sasa Exp $*/
/*17.10.2006	28.02.2005	Белых А.И.	pib_str.c
Вывод строки в документ
*/
#include <stdio.h>

void pib_str(int nomer,const char *bankshet,double sum,const char *fio,
const char *indkod,
FILE *ffkartr)
{

 fprintf(ffkartr,"%3d %-16s %8.2f %10s %s\n",nomer,bankshet,sum,indkod,fio);
}
