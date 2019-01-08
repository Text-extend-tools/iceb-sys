/*$Id: hrasact.c,v 5.10 2013/09/26 09:43:33 sasa Exp $*/
/*14.07.2015	20.07.2003	Белых А.И.	hrasact.c
Шапка акта приема передачи материалов
*/
#include	"buhg.h"


void	hrasact(int *kolstrok,int *kollist,FILE *ff)
{
struct  tm      *bf;
time_t		tmm;

time(&tmm);
bf=localtime(&tmm);
short god=bf->tm_year+1900;


fprintf(ff,"%s\n",iceb_t_get_pnk("00",0));
fprintf(ff,"\
                                                                              %s\n\
                                                                       %s___________________ \n\
                                                                        ____________________\n\
                                                                       \"_____\"____________%d%s\n",
gettext("Утверждаю"),
gettext("Руководитель"),
god,
gettext("г."));

fprintf(ff,"%47s %s\n"," ",gettext("АКТ"));

fprintf(ff,"%39s \"_____\"____________%d%s\n",
" ",god,gettext("г."));

fprintf(ff,"\
%s:\n\
%s:\n\
%s:\n\
%s:\n\n\n\n",
gettext("Основание"),
gettext("Комиссия в составе"),
gettext("Председатель комиссии"),
gettext("Члены комиссии"));

fprintf(ff,"\n\
\"_____\"____________%d%s %s\n\n\n",
god,
gettext("г."),
gettext("комиссия составила акт приёмки под отчёт товароматериальных ценностей"));

*kolstrok=19;
*kollist=1;

sapkkiv(kolstrok,*kollist,ff);
}
