/*$Id: pib_end_dok.c,v 5.8 2013/05/17 14:56:07 sasa Exp $*/
/*31.10.2009	28.02.2005	Белых А.И.	pib_end_dok.c
Выдача концовки в файл
*/

#include        "buhg.h"

void pib_end_dok(int kol_na_liste,double sumalist,double itogoo,int nomer,
const char *rukov,const char *glavbuh,
FILE *ffkartr)
{
char strsql[512];
fprintf(ffkartr,"%-*s:%8.2f\n\n",iceb_tu_kolbait(16,"Всього по листу"),"Всього по листу",sumalist);
fprintf(ffkartr,"Кількість одержувачів коштів по листу:%d чоловік\n",kol_na_liste);
memset(strsql,'\0',sizeof(strsql));
preobr(sumalist,strsql,0);
fprintf(ffkartr,"на суму:%s\n",strsql);

fprintf(ffkartr,"%*s:%8.2f\n",iceb_tu_kolbait(16,"Разом"),"Разом",itogoo);


fprintf(ffkartr,"\nКількість одержувачів коштів: %d чоловік\n",nomer);

memset(strsql,'\0',sizeof(strsql));
preobr(itogoo,strsql,0);

fprintf(ffkartr,"На суму:%s\n\n",strsql);

fprintf(ffkartr,"\
              Керівник _______________%s\n\n\
    Головний бухгалтер _______________%s\n",rukov,glavbuh);

}
