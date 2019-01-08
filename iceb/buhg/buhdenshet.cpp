/*$Id: buhdenshet.c,v 5.7 2013/05/17 14:55:56 sasa Exp $*/
/*07.10.2009	01.09.2009	Белых А.И.	buhdenshet.c
Распечатка массива по дням и счетам
*/
#include <math.h>
#include "buhg.h"


void buhdenshet(const char *zagol,class SPISOK *dni,
class SPISOK *sheta,
class masiv_din_double *den_shet_suma,
int metka_f, /*0-формат А4 1-формат А3*/
FILE *ff)
{
int max_kolih_kolonok_na_liste=13;

class masiv_din_double itogo_kol;
class masiv_din_double itogo_gor;
int kolih_sheta=sheta->kolih();
int kolih_dni=dni->kolih();
int kolbl=0; 
itogo_gor.make_class(kolih_dni);
itogo_kol.make_class(kolih_sheta);
int list=0;
int kolbait=0;

if(metka_f == 1)
 max_kolih_kolonok_na_liste=26;
 
for(int nbl=0; nbl < kolih_sheta; nbl+=max_kolih_kolonok_na_liste)
 {
  if(nbl != 0)
   fprintf(ff,"\f");

     
  kolbl=kolih_sheta-nbl;
  if(kolbl > max_kolih_kolonok_na_liste)
   kolbl=max_kolih_kolonok_na_liste;
  if(nbl+max_kolih_kolonok_na_liste >= kolih_sheta)
    kolbl++;
      
  class iceb_tu_str liniq;
  for(int nomer=0; nomer < kolbl+1; nomer++)
    liniq.plus("-----------");


  double suma=0.;
  int mes=0;
  short d,m,g;
  for(int nomer_dni=0; nomer_dni < kolih_dni; nomer_dni++)
   {
    rsdat(&d,&m,&g,dni->ravno(nomer_dni),1);
    if(mes != m)
     {
      if(mes != 0)
       fprintf(ff,"\f");
       
      fprintf(ff,"%s %s:%d\n",zagol,gettext("Лист"),++list);
      fprintf(ff,"%s\n",liniq.ravno());
      kolbait= iceb_tu_kolbait(10,gettext("Дата"));
      fprintf(ff,"%-*.*s|",kolbait,kolbait,  gettext("Дата"));
      
      for(int nomer=nbl; nomer < nbl+max_kolih_kolonok_na_liste && nomer < kolih_sheta ; nomer++)
       fprintf(ff,"%*.*s|",iceb_tu_kolbait(10,sheta->ravno(nomer)),iceb_tu_kolbait(10,sheta->ravno(nomer)),sheta->ravno(nomer));

      if(nbl+max_kolih_kolonok_na_liste >= kolih_sheta)
       {
        kolbait=iceb_tu_kolbait(10,gettext("Итого"));
        fprintf(ff,"%*.*s|\n",kolbait,kolbait,gettext("Итого"));
       }
      else 
       fprintf(ff,"\n");
       
      fprintf(ff,"%s\n",liniq.ravno());
      mes=m;
     }
    fprintf(ff,"%s|",dni->ravno(nomer_dni));
    for(int nomer_sheta=nbl; nomer_sheta < nbl+max_kolih_kolonok_na_liste && nomer_sheta < kolih_sheta; nomer_sheta++)
     {
      suma=den_shet_suma->ravno(nomer_dni*kolih_sheta+nomer_sheta);
      if(fabs(suma) > 0.009)
       {
        fprintf(ff,"%10.2f|",suma);
        itogo_gor.plus(suma,nomer_dni);
        itogo_kol.plus(suma,nomer_sheta);
       }
      else
       fprintf(ff,"%10s|","");
     }

    if(nbl+max_kolih_kolonok_na_liste >= kolih_sheta)
     {
      if(fabs(itogo_gor.ravno(nomer_dni)) > 0.009)
        fprintf(ff,"%10.2f|\n",itogo_gor.ravno(nomer_dni));
      else
        fprintf(ff,"%10s|\n","");
     }    
    else
     fprintf(ff,"\n");
     
    fprintf(ff,"%s\n",liniq.ravno());
   }
  kolbait=iceb_tu_kolbait(10,gettext("Итого"));
  fprintf(ff,"%*.*s|",kolbait,kolbait,gettext("Итого"));

  for(int nomer=nbl; nomer < nbl+max_kolih_kolonok_na_liste && nomer < kolih_sheta; nomer++)
   fprintf(ff,"%10.2f|",itogo_kol.ravno(nomer));

  if(nbl + max_kolih_kolonok_na_liste >= kolih_sheta)
   {
    fprintf(ff,"%10.2f|\n",itogo_gor.suma());
    fprintf(ff,"%s\n",liniq.ravno());
   }
  else
   fprintf(ff,"\n");
 }

}
