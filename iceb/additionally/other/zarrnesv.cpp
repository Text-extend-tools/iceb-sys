/*$Id: zarrnesv.c,v 5.7 2011-12-02 09:48:19 sasa Exp $*/
/*30.11.2011	26.01.2011	Белых А.И.	zarrnesv.c
Чтение настроек для расчёта единого сочиального взноса
*/
#include <glib.h>
#include "buhg.h"

int kod_esv=0; /*Код удержания единого социального взноса*/
int kod_esv_bol=0; /*Код удержания единого социального взноса*/
int kod_esv_inv=0; /*Код удержания единого социального взноса*/
int kod_esv_dog=0; /*Код удержания единого социального взноса*/
int kod_esv_vs=0;
class iceb_tu_str kods_esv_all;  /*Все коды удержания единого социального взноса*/

int kodf_esv=0; /*Код фонда удержания единого социального взноса*/
int kodf_esv_bol=0; /*Код фонда удержания единого социального взноса*/
int kodf_esv_inv=0; /*Код фонда удержания единого социального взноса*/
int kodf_esv_dog=0; /*Код фонда удержания единого социального взноса*/
int kodf_esv_vs=0;

class iceb_tu_str knvr_esv_r; /*Коды не входящие в расчёт единого социального взноса с работника*/
class iceb_tu_str kod_zv_gr; /*коды званий гражданских*/

void zarrnesv()
{

iceb_t_poldan("Код удержания единого социального взноса",&kod_esv,"zarnast.alx");
iceb_t_poldan("Код удержания единого социального взноса для больничных",&kod_esv_bol,"zarnast.alx");
iceb_t_poldan("Код удержания единого социального взноса для инвалидов",&kod_esv_inv,"zarnast.alx");
iceb_t_poldan("Код удержания единого социального взноса для договоров",&kod_esv_dog,"zarnast.alx");
iceb_t_poldan("Код удержания единого социального взноса для военных",&kod_esv_vs,"zarnast.alx");
iceb_t_poldan("Коды начислений не входящие в расчёт удержания единого социального взноса",&knvr_esv_r,"zarnast.alx");

kods_esv_all.plus(kod_esv);
kods_esv_all.z_plus(kod_esv_bol);
kods_esv_all.z_plus(kod_esv_inv);
kods_esv_all.z_plus(kod_esv_dog);
kods_esv_all.z_plus(kod_esv_vs);

iceb_t_poldan("Код фонда единого социального взноса",&kodf_esv,"zarnast.alx");
iceb_t_poldan("Код фонда единого социального взноса для инвалидов",&kodf_esv_inv,"zarnast.alx");
iceb_t_poldan("Код фонда единого социального взноса для больничных",&kodf_esv_bol,"zarnast.alx");
iceb_t_poldan("Код фонда единого социального взноса для договоров",&kodf_esv_dog,"zarnast.alx");
iceb_t_poldan("Код фонда единого социального взноса для военных",&kodf_esv_vs,"zarnast.alx");

iceb_t_poldan("Коды званий гражданских",&kod_zv_gr,"zarnast.alx");


gchar *strsql;

strsql=g_strdup_printf("%s-Прочитали настройки\n\
Код удержания единого социального взноса:%d\n\
Код удержания единого социального взноса для больничных:%d\n\
Код удержания единого социального взноса для инвалидов:%d\n\
Код удержания единого социального взноса для договоров:%d\n\
Код удержания единого социального взноса для военных:%d\n\
Коды начислений не входящие в расчёт удержания единого социального взноса:%s\n\
Код фонда единого социального взноса:%d\n\
Код фонда единого социального взноса для инвалидов:%d\n\
Код фонда единого социального взноса для больничных:%d\n\
Код фонда единого социального взноса для договоров:%d\n\
Код фонда единого социального взноса для военных:%d\n\
Коды званий гражданских:%s\n",
  __FUNCTION__,
  kod_esv,
  kod_esv_bol,
  kod_esv_inv,
  kod_esv_dog,
  kod_esv_vs,
  knvr_esv_r.ravno(),
  kodf_esv,
  kodf_esv_bol,
  kodf_esv_inv,
  kodf_esv_dog,
  kodf_esv_vs,
  kod_zv_gr.ravno());

zar_pr_ins(strsql);

g_free(strsql);
  
}

