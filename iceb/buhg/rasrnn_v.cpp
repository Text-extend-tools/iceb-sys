/*$Id:$*/
/*30.11.2014	29.11.2014	Белых А.И.	rasrnn_v.c
Расчёт реестра выданных налоговых накладных
*/
#include "buhg.h"
#include "xrnn_poi.h"
#include "rnn_d5.h"

int rasrnn_v7(class xrnn_poi *data,int kol_strok,int period_type,FILE *ff,FILE *ff_prot,FILE *ff_xml,FILE *ff_dot,FILE *ff_xml_dot,FILE *ff_xml_rozkor,FILE *ff_lst_rozkor,class rnn_d5 *dod5);
int rasrnn_v8(class xrnn_poi *data,int period_type,FILE *ff,FILE *ff_prot,FILE *ff_xml,FILE *ff_dot,FILE *ff_xml_dot,FILE *ff_xml_rozkor,FILE *ff_lst_rozkor,class rnn_d5 *dod5);

int rasrnn_v(class xrnn_poi *data,int period_type,FILE *ff,FILE *ff_prot,FILE *ff_xml,FILE *ff_dot,FILE *ff_xml_dot,FILE *ff_xml_rozkor,FILE *ff_lst_rozkor,class rnn_d5 *dod5)
{

if(sravmydat(data->dn,data->mn,data->gn,1,12,2014) < 0)
 {
  int kol_strok=0;
  return(rasrnn_v7(data,kol_strok,period_type,ff,ff_prot,ff_xml,ff_dot,ff_xml_dot,ff_xml_rozkor,ff_lst_rozkor,dod5));
 }
else
  return(rasrnn_v8(data,period_type,ff,ff_prot,ff_xml,ff_dot,ff_xml_dot,ff_xml_rozkor,ff_lst_rozkor,dod5));

}