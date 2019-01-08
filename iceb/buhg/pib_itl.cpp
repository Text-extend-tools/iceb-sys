/*$Id: pib_itl.c,v 5.7 2013/05/17 14:56:07 sasa Exp $*/
/*30.10.2009	28.02.2005	Белых А.И.	pib_itl.c
Выдача итога по листу
Если вернули 0-конец листа
             1-нет            
*/
#include        "buhg.h"

extern int kol_strok_na_liste;

void pib_itl(int *kolstrkartr,double *sumalist,int *kol_na_liste,
const char *rukov,const char *glavbuh,
FILE *ffkartr)
{

*kolstrkartr+=1;
if(*kolstrkartr > kol_strok_na_liste-7)
 {
  char strsql[512];
  fprintf(ffkartr,"%-*s:%8.2f\n\n",iceb_tu_kolbait(16,"Всього по листу"),"Всього по листу",*sumalist);
  fprintf(ffkartr,"Кількість одержувачів коштів по листу:%d чоловік\n",*kol_na_liste);
  memset(strsql,'\0',sizeof(strsql));
  preobr(*sumalist,strsql,0);
  fprintf(ffkartr,"на суму:%s\n",strsql);

  fprintf(ffkartr,"\
              Керівник _______________%s\n\n\
    Головний бухгалтер _______________%s\n",rukov,glavbuh);

  *kolstrkartr=1;
  *sumalist=0.;
  fprintf(ffkartr,"\f");
  *kol_na_liste=0;
 }
}
