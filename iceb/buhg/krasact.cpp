/*$Id: krasact.c,v 5.9 2013/09/26 09:43:35 sasa Exp $*/
/*15.07.2015	20.07.2003	Белых А.И.	krasact.c
Концовка акта приема - передачи
*/

#include	"buhg.h"


void krasact(const char *shetz,const char *naimshet,
double kolihshet,double sumashet,
double kolihi,double sumai,
FILE *ff)
{
char strsql[512];

sprintf(strsql,"%s %s %s",gettext("Итого по счёту"),shetz,naimshet);
fprintf(ff,"%62s %10.10g %10.2f\n",strsql,kolihshet,sumashet);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n\
%62s %10.10g %10.2f\n",gettext("Общий итог"),kolihi,sumai);

fprintf(ff,"\n\n\
%s:\n\
%s:\n",
gettext("Председатель коммиссии"),
gettext("Члены коммисии"));

fprintf(ff,"\n%s:\n\n",gettext("Передал"));
fprintf(ff,"%s:\n\n",gettext("Принял"));

}
