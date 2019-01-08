/*$Id: iceb_t_openxml.c,v 5.8 2013/12/31 11:48:20 sasa Exp $*/
/*08.14.2016	22.07.2011	Белых А.И.	iceb_t_openxml.c
открытие xml файла для отчётности в налоговую
Если вернули 0-настройки прочитаны, файл открыт
             1- нет
*/
#include <errno.h>
#include "buhl.h"


int iceb_t_openxml(short dn,short mn,short gn, //Дата начала
short dk,short mk,short gk,//Дата конца
char *imaf_xml, //Имя файла
const char *kod_dok,
int nomer_dok_v_pakete, //Номер документа в пакете 
int *period_type, //Тип периода
class iceb_rnfxml_data *rek_zag_nn,
FILE **ff_xml)
{


*period_type=1; // 1-месяц 2 квартал 3 полугодие 4-9 месяцев 5 год

int periodd=period(dn,mn,gn,dk,mk,gk,1);


switch(periodd)
 {

  case 3:
   *period_type=2;
    break;

  case 6:
   *period_type=3;
    break;

  case 9:
   *period_type=4;
    break;

  case 12:
   *period_type=5;
    break;

  default:
   *period_type=1;
   break;
 };

sprintf(imaf_xml,"%.4s%010d%s100%07d%1d%02d%04d%4.4s.XML",
rek_zag_nn->kod_dpi.ravno(),
rek_zag_nn->kod_edrpuo.ravno_atoi(),
kod_dok,
nomer_dok_v_pakete,
*period_type,
mk,gk,
rek_zag_nn->kod_dpi.ravno());


if((*ff_xml = fopen(imaf_xml,"w")) == NULL)
 {
  error_op_nfil(imaf_xml,errno,"");
  return(1);
 }
return(0);
}
/***************************************************/
int iceb_t_openxml(short dn,short mn,short gn, //Дата начала
short dk,short mk,short gk,//Дата конца
char *imaf_xml, //Имя файла
const char *tip_dok,  //Тип документа  J12
const char *pod_tip_dok, //Подтип документа
const char *nomer_versii_dok, //Номер версии документа
int nomer_dok_v_pakete, //Номер документа в пакете 
int *period_type, //Тип периода
class iceb_rnfxml_data *rek_zag_nn,
FILE **ff_xml)
{
*period_type=1; // 1-месяц 2 квартал 3 полугодие 4-9 месяцев 5 год

int periodd=period(dn,mn,gn,dk,mk,gk,1);


switch(periodd)
 {

  case 3:
   *period_type=2;
    break;

  case 6:
   *period_type=3;
    break;

  case 9:
   *period_type=4;
    break;

  case 12:
   *period_type=5;
    break;

  default:
   *period_type=1;
   break;
 };

sprintf(imaf_xml,"%.4s%010d%3.3s%3.3s%2.2s100%07d%1d%02d%04d%4.4s.XML",
rek_zag_nn->kod_dpi.ravno(),
atoi(rek_zag_nn->kod_edrpuo.ravno()),
tip_dok,
pod_tip_dok,
nomer_versii_dok,
nomer_dok_v_pakete,
*period_type,
mk,gk,
rek_zag_nn->kod_dpi.ravno());


if((*ff_xml = fopen(imaf_xml,"w")) == NULL)
 {
  error_op_nfil(imaf_xml,errno,"");
  return(1);
 }
return(0);
}
