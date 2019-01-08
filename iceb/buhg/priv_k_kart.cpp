/*$Id:$*/
/*22.03.2017	19.02.2005	Белых А.И.	priv_k_kart.c
Подпрограмма записи в документ материального учёта с привязкой к карточке
*/
#include <math.h>
#include "buhg.h"



void priv_k_kart(short dk,short mk,short gk,
const char *nomd,
int sklad,
int kodm,
double kolih,
double cena,
const char *shetu,
const char *ei,
double ndss,
int mnds,
int voztar,
const char *nomzak,
int metka_priv,
const char *shet_sp) //счёт списания
{
int tipz=2;

masiv_din_int nomkars;
masiv_din_double kolpriv;
int kolkar;  
if((kolkar=findkar(dk,mk,gk,sklad,kodm,kolih,&nomkars,&kolpriv,metka_priv,cena,shetu)) <= 0)
 {
  zapvdok(dk,mk,gk,sklad,0,kodm,nomd,kolih,cena,ei,ndss,mnds,voztar,tipz,0,shet_sp,nomzak,0,"");
 }
else
 {
  double kolihpi=0.;
  for(int sht=0; sht < kolkar; sht++)
   {
    int nomkar=nomkars.ravno(sht);
    double kolihp=kolpriv.ravno(sht);
    kolihpi+=kolihp;
    zapvdok(dk,mk,gk,sklad,nomkar,kodm,nomd,kolihp,cena,ei,ndss,mnds,voztar,tipz,0,shet_sp,nomzak,0,"");
   }
  if(fabs(kolih-kolihpi) > 0.000001)
    zapvdok(dk,mk,gk,sklad,0,kodm,nomd,kolih-kolihpi,cena,ei,ndss,mnds,voztar,tipz,0,shet_sp,nomzak,0,"");
 }

}

