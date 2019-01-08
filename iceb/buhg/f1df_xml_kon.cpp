/*$Id: f1df_xml_kon.c,v 5.10 2013/05/17 14:56:01 sasa Exp $*/
/*16.03.2015	23.04.2007	Белых А.И.	f1df_xml_kon.c
Концовка xml файла формы 1ДФ
*/

#include "buhg.h"

void f1df_xml_kon(double isumadn,
double isumad,
double isumann,
double isuman,
int nomer_xml_str,
int kolr,
int klls,
const char *naim_kontr,
const char *telef_kontr,
int TYP,
double doh_nah_vs, /*Общая сумма начисленного дохода для расчёта военного сбора*/
double doh_vip_vs, /*Общая сумма выплаченного дохода для расчёта военного сбора*/
double vs_nah, /*военный сбор начисленный*/
double vs_vip, /*военній сбор перечисленный*/
FILE *ff_xml)
{
fprintf(ff_xml,"  <R01G03A>%.2f</R01G03A>\n",isumadn);
fprintf(ff_xml,"  <R01G03>%.2f</R01G03>\n",isumad);
fprintf(ff_xml,"  <R01G04A>%.2f</R01G04A>\n",isumann);
fprintf(ff_xml,"  <R01G04>%.2f</R01G04>\n",isuman);

fprintf(ff_xml,"  <R02G01I>%d</R02G01I>\n",nomer_xml_str);
fprintf(ff_xml,"  <R02G02I>%d</R02G02I>\n",kolr);
fprintf(ff_xml,"  <R02G03I>%d</R02G03I>\n",klls);

fprintf(ff_xml,"  <R0205G03A>%.2f</R0205G03A>\n",doh_nah_vs);
fprintf(ff_xml,"  <R0205G03>%.2f</R0205G03>\n",doh_vip_vs);
fprintf(ff_xml,"  <R0205G04A>%.2f</R0205G04A>\n",vs_nah);
fprintf(ff_xml,"  <R0205G04>%.2f</R0205G04>\n",vs_vip);
class iceb_tu_str bros("");
class iceb_tu_str fioruk("");
class iceb_tu_str telef("");
  
iceb_t_poldan("Табельный номер руководителя",&bros,"zarnast.alx");
class iceb_tu_str inn("");
char strsql[512];
SQL_str row1;
class SQLCURSOR curr;
if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio,inn,telef from Kartb where tabn=%s",bros.ravno());
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    beep();
    printw("%s-%s %s!\n",
    __FUNCTION__,
    gettext("Не найден табельный номер руководителя"),
    bros.ravno());
    OSTANOV();
   }
  else
   {
    fioruk.new_plus(row1[0]);
    inn.new_plus(row1[1]);
    telef.new_plus(row1[2]);
//    ud_simv(telef,"-/.()+,");
   }
 } 
if(inn.ravno()[0] != '\0')
  fprintf(ff_xml,"  <HKBOS>%s</HKBOS>\n",inn.ravno());
if(fioruk.ravno()[0] != '\0')
  fprintf(ff_xml,"  <HBOS>%s</HBOS>\n",fioruk.ravno_filtr());
if(telef.ravno()[0] != '\0')
  fprintf(ff_xml,"  <HTELBOS>%s</HTELBOS>\n",telef.ravno_udsimv("-/.()+,"));

iceb_t_poldan("Табельный номер бухгалтера",&bros,"zarnast.alx");
fioruk.new_plus("");
telef.new_plus("");
if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio,inn,telef from Kartb where tabn=%s",bros.ravno());
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    beep();
    printw("%s-%s %s!\n",
    __FUNCTION__,
    gettext("Не знайден табельный номер главного бухгалтера"),
    bros.ravno());
    OSTANOV();
   }
  else
   {
    fioruk.new_plus(row1[0]);
    inn.new_plus(row1[1]);
    telef.new_plus(row1[2]);
//    ud_simv(telef,"-/.()+,");

   }
 }

if(inn.ravno()[0] != '\0')
  fprintf(ff_xml,"  <HKBUH>%s</HKBUH>\n",inn.ravno());
if(fioruk.ravno()[0] != '\0')
  fprintf(ff_xml,"  <HBUH>%s</HBUH>\n",fioruk.ravno_filtr());
if(telef.ravno()[0] != '\0')
  fprintf(ff_xml,"  <HTELBUH>%s</HTELBUH>\n",telef.ravno_udsimv("-/.()+,"));

short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

fprintf(ff_xml,"  <HFILL>%02d%02d%d</HFILL>\n",dt,mt,gt);


if(TYP == 1)
 {
  if(naim_kontr[0] != '\0')
    fprintf(ff_xml,"  <HFO>%s<HFO>\n",iceb_tu_filtr_xml(naim_kontr));
  if(telef_kontr[0] != '\0')
    fprintf(ff_xml,"  <HTELFO>%s<HTELFO>\n",iceb_tu_filtr_xml(telef_kontr));
 }

fprintf(ff_xml," </DECLARBODY>\n");
fprintf(ff_xml,"</DECLAR>\n");
fclose(ff_xml);

}


