/*$Id: zar_mpsl.c,v 5.3 2013/05/17 14:56:16 sasa Exp $*/
/*14.04.2011	14.04.2011	Белых А.И	zar_mpsl.c
Отключение/включение применения социальной льготы
*/
#include "buhg.h"

int metka_psl=0; /*Метка применения социальной льготы 0-применяется 1-нет*/

void zar_mpsl()
{

metka_psl=0;

if(iceb_t_poldan_vkl("Отключение/включение применения социальной льготы при расчёте подоходного","zarnast.alx") != 0)
 return;
 
class VVOD DANET(1);
DANET.VVOD_SPISOK_add_MD(gettext("Расчёт подоходного налога без применения социальной льготы?"));

if(danet(&DANET,2,stdscr) == 1)
 metka_psl=1;
//printw("\n%s-metka_psl=%d\n",metka_psl);
//OSTANOV();
}
