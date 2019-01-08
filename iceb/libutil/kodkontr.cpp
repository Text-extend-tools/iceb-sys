/*$Id: kodkontr.c,v 1.8 2013/08/13 05:50:32 sasa Exp $*/
/*19.07.2013	25.07.2001	Белых А.И.	kodkontr.c
Готовим пару кодов контрагентов для записи в проводку
*/
#include	"util.h"


void kodkontr(class iceb_tu_str *kont1,class iceb_tu_str *kont2,const char *kont1z,const char *kont2z)
{

kont1->new_plus(kont1z);
kont2->new_plus(kont2z);

if(kont1->ravno()[0] == '\0')
 kont1->new_plus(kont2z);

if(kont2->ravno()[0] == '\0')
 kont2->new_plus(kont1z);

}
