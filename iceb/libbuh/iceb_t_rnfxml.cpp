/*$Id: iceb_t_rnfxml.c,v 5.6 2013/11/05 10:50:57 sasa Exp $*/
/*12.04.2016	22.07.2011	Белых А.И.	iceb_t_rnfxml.c
Чтение настроек необходимых для создания xml файлов с отчётностью
*/
#include "buhl.h"

extern iceb_tu_str kodirovka_iceb;

void iceb_t_rnfxml(class iceb_rnfxml_data *rek_zag_nn)
{
char strsql[512];

iceb_t_poldan("Код области",&rek_zag_nn->kod_oblasti,"rnn_nast.alx");
iceb_t_poldan("Код административного района",&rek_zag_nn->kod_admin_r,"rnn_nast.alx");
iceb_t_poldan("Код ГНИ",&rek_zag_nn->kod_dpi,"rnn_nast.alx");
iceb_t_poldan("Кодовая таблица",&rek_zag_nn->kod_tabl,"rnn_nast.alx");


if(rek_zag_nn->kod_tabl.getdlinna() <= 1)
 rek_zag_nn->kod_tabl.new_plus(kodirovka_iceb.ravno());
 
//читаем реквизиты
SQL_str row;
SQLCURSOR cur;

sprintf(strsql,"select naikon,adres,kod,innn,nspnds,telef,pnaim from Kontragent where kodkon='00'");
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  if(row[6][0] != '\0')
    rek_zag_nn->naim_kontr.new_plus(row[6]);
  else
    rek_zag_nn->naim_kontr.new_plus(row[0]);
  
  rek_zag_nn->adres.new_plus(row[1]);
  rek_zag_nn->kod_edrpuo.new_plus(row[2]);
  rek_zag_nn->innn.new_plus(row[3]);
  rek_zag_nn->nspnds.new_plus(row[4]);
  rek_zag_nn->telefon.new_plus(row[5]);
 }
}
