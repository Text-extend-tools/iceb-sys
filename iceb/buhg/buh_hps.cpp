/*$Id: buh_hps.c,v 5.6 2013-04-18 09:26:02 sasa Exp $*/
/*17.04.2013	14.01.2008	Белых А.И.	buh_hps.c
Получение из шахматки по субсчетам шахматки по счетам
*/

#include "buhg.h"

void buh_hps(class SPISOK *deb,class SPISOK *kre,class masiv_din_double *suma,
class SPISOK *debsh,class SPISOK *kresh,class masiv_din_double *sumash)
{
char shet[64];

/*создаём дебетовый список по счетам*/
for(int ii=0; ii < deb->kolih(); ii++)
 {
  sprintf(shet,"%*.*s",iceb_tu_kolbait(2,deb->ravno(ii)),iceb_tu_kolbait(2,deb->ravno(ii)),deb->ravno(ii));
  if(debsh->find(shet) < 0)
   debsh->plus(shet);
 }

/*создаём кредитовый список по счетам*/
for(int ii=0; ii < kre->kolih(); ii++)
 {
  sprintf(shet,"%*.*s",iceb_tu_kolbait(2,kre->ravno(ii)),iceb_tu_kolbait(2,kre->ravno(ii)),kre->ravno(ii));
  if(kresh->find(shet) < 0)
   kresh->plus(shet);
 }
sumash->make_class(kresh->kolih()*debsh->kolih());

/*Заполняем массив суммами*/

int nomdsh=0,nomksh=0;
for(int nomer_deb=0; nomer_deb < deb->kolih(); nomer_deb++)
 {
  if((nomdsh=debsh->find(deb->ravno(nomer_deb),1)) < 0)
   {
    printw("%s-%s %s\n",__FUNCTION__,gettext("Не найден дебетовый счёт"),deb->ravno(nomer_deb));
/**********
    for(int ii=0; ii < debsh->kolih(); ii++)
     printw("%s\n",debsh->ravno(ii));
****************/
    OSTANOV();

    continue;
   }   
 
   for(int nomer_kre=0; nomer_kre < kre->kolih(); nomer_kre++)
    {
     if((nomksh=kresh->find(kre->ravno(nomer_kre),1)) < 0)
      {
        printw("%s-%s %s\n",__FUNCTION__,gettext("Не найден кредитовый счёт"),kre->ravno(nomer_kre));
/*************    
        for(int ii=0; ii < kresh->kolih(); ii++)
         printw("%s\n",kresh->ravno(ii));
**************/
        OSTANOV();
        continue;
      
      }   
     
     sumash->plus(suma->ravno(nomer_deb*kre->kolih()+nomer_kre), nomdsh*kresh->kolih()+nomksh);
    }
 }

}
