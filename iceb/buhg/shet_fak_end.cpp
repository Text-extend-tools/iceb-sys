/*$Id: shet_fak_end.c,v 5.10 2013/05/17 14:56:11 sasa Exp $*/
/*15.07.2015	21.12.2005	Белых А.И.	shet_fak_end.c
Распечатка концовки счёта-фактуры
*/

#include	"buhg.h"

void shet_fak_end(FILE *ff)
{

fprintf(ff,"\n%*s______________________\n\n",
iceb_tu_kolbait(20,gettext("Руководитель")),gettext("Руководитель"));
fprintf(ff,"%*s______________________\n",
iceb_tu_kolbait(20,gettext("Главный бухгалтер")),gettext("Главный бухгалтер"));


iceb_t_insfil("usl_shet_fak_end.alx",ff);

podpis(ff);
}
