/*$Id: sapvin.c,v 5.16 2013/05/17 14:56:11 sasa Exp $*/
/*01.08.2011	03.08.2003	Белых А.И.	sapvin.c
Шапка инвентаризационной ведомости
*/
#include        "buhg.h"
#define		IMAFDPI "inventm.alx"




void sapvin(const char *matot,const char *mesqc,short g,short gos,int *kolstriv,int *kollistiv,
const char *sklad_podr,
FILE *ff2)
{



fprintf(ff2,"%s\n\n\
%40s %s N______\n\
%40s %s\n\
%40s \"_____\"_____________%d%s\n",
iceb_t_get_pnk("00",0),
" ",gettext("Инвентаризационная опись"),
" ",gettext("товарно-материальных ценностей"),
" ",gos,gettext("г."));

fprintf(ff2,gettext("\
                          РАСПИСКА\n\
 К началу проведения инвентаризации все расходные и приходные документы\n\
на ценности сданы в бухгалтерию и все ценности, поступившие на мою\n\
(нашу) ответственность, оприходованы, а выбывшие списаны в расход.\n\n"));
fprintf(ff2,"\n%s\n",sklad_podr);
fprintf(ff2,"\
%s __________________________%s\n\n",
gettext("Материально-ответственное лицо (лица)"),
matot);

*kolstriv=14;
*kolstriv+=iceb_t_insfil(IMAFDPI,ff2);


fprintf(ff2,"\n\
%*s:%10s \"______\" %s %d%s\n\
%15s %10s \"______\" %s %d%s\n",
iceb_tu_kolbait(15,gettext("Инвентаризация")),gettext("Инвентаризация"),
gettext("начата"),
mesqc,g,
gettext("г."),
" ",gettext("окончена"),
mesqc,g,
gettext("г."));

fprintf(ff2,"\n\%s:\n",gettext("При инвентаризации установлено следующее"));
*kolstriv+=5;

*kollistiv=1;
int kkk=1;
sapkkiv(kolstriv,kkk,ff2);

}
