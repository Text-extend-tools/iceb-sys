/*$Id: open_fil_nn_xml.c,v 5.12 2013/12/31 11:48:10 sasa Exp $*/
/*08.04.2016	13.10.2006	Белых А.И.	iceb_t_open_fil_nn_xml.c
Открытие файла для налогового документа в фармате xml
*/
#include <errno.h>
#include "buhl.h"

extern class iceb_rnfxml_data rek_zag_nn;
extern iceb_tu_str kat_for_nal_nak; //Каталог для налоговых накладных

int iceb_t_open_fil_nn_xml(char *imaf_xml,int pnd,int tipz,
short mn,short gn,
FILE **ff_xml)
{

if(kat_for_nal_nak.getdlinna() > 1)
 sprintf(imaf_xml,"%s/%.4s%010d%s100%07d%1d%02d%04d%4.4s.XML",
 kat_for_nal_nak.ravno(),
 rek_zag_nn.kod_dpi.ravno(),
 atoi(rek_zag_nn.kod_edrpuo.ravno()),
 iceb_t_getkoddok(1,mn,gn,1),
 pnd,
 1,
 mn,gn,
 rek_zag_nn.kod_dpi.ravno());
else
 sprintf(imaf_xml,"%.4s%010d%s100%07d%1d%02d%04d%4.4s.XML",
 rek_zag_nn.kod_dpi.ravno(),
 atoi(rek_zag_nn.kod_edrpuo.ravno()),
 iceb_t_getkoddok(1,mn,gn,1),
 pnd,
 1,
 mn,gn,
 rek_zag_nn.kod_dpi.ravno());

if((*ff_xml = fopen(imaf_xml,"w")) == NULL)
 {
  error_op_nfil(imaf_xml,errno,"");
  return(1);
 }

return(0);

}
