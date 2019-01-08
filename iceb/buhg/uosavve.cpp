/*$Id: uosavve.c,v 5.1 2013-04-07 10:22:44 sasa Exp $*/
/*14.02.2013	14.02.2013	Белых А.И.	uosavve.c
Распечатка акта ввода в эксплуатацию
*/
#include <errno.h>
#include "buhg.h"
#include "poiinpd.h"
#include "bsizt.h"

void uosavve(int invnom)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[1024];
FILE *ff;
char imaf[64];
int kom=0;
class iceb_tu_str naim_invnom("");
sprintf(strsql,"select naim from Uosin where innom=%d",invnom);
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %d!",gettext("Не найден инвентарный номер"),invnom);
  iceb_t_soob(strsql);
  return;
 }
naim_invnom.new_plus(row[0]);

static class iceb_tu_str data_prik("");
static class iceb_tu_str nomer_prik("");
static class iceb_tu_str data_akta("");
static class iceb_tu_str nomer_akta("");
static class iceb_tu_str data_nakl("");
static class iceb_tu_str nomer_nakl("");


class VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка акта ввода в эксплуатацию"));

VV.VVOD_SPISOK_add_MD(gettext("Дата приказа о создании комиссии..(д.м.г)."));
VV.VVOD_SPISOK_add_MD(gettext("Номер приказа о создании комиссии........."));
VV.VVOD_SPISOK_add_MD(gettext("Дата акта ввода в эксплуатацию...(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Номер акта ввода в эксплуатацию..........."));
VV.VVOD_SPISOK_add_MD(gettext("Дата накладной...................(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Номер накладной..........................."));

VV.VVOD_SPISOK_add_data(data_prik.ravno(),11);
VV.VVOD_SPISOK_add_data(nomer_prik.ravno(),20);
VV.VVOD_SPISOK_add_data(data_akta.ravno(),11);
VV.VVOD_SPISOK_add_data(nomer_akta.ravno(),20);
VV.VVOD_SPISOK_add_data(data_nakl.ravno(),11);
VV.VVOD_SPISOK_add_data(nomer_nakl.ravno(),20);

naz:;

 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F5",gettext("настройка"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

data_prik.new_plus(VV.VVOD_SPISOK_return_data(0));
nomer_prik.new_plus(VV.VVOD_SPISOK_return_data(1));
data_akta.new_plus(VV.VVOD_SPISOK_return_data(2));
nomer_akta.new_plus(VV.VVOD_SPISOK_return_data(3));
data_nakl.new_plus(VV.VVOD_SPISOK_return_data(4));
nomer_nakl.new_plus(VV.VVOD_SPISOK_return_data(5));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   iceb_t_pdoc("buhg5_1.txt");
   goto naz;

  case ESC:
  case FK10:
   return;

  case FK2:
  case PLU:
     break;


  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

short da=0,ma=0,ga=0;
short dn=0,mn=0,gn=0;
short dp=0,mp=0,gp=0;

if(rsdat(&da,&ma,&ga,data_akta.ravno(),1) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата акта!"));
  goto naz;
 }

if(rsdat(&dn,&mn,&gn,data_nakl.ravno(),1) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата накладной!"));
  goto naz;
 }

if(rsdat(&dp,&mp,&gp,data_prik.ravno(),1) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата приказа!"));
  goto naz;
 }

int kod_pod=0,kod_mo=0;
/*ищем в каком подразделении и у какого мат.ответцтвенного лица числится на дату акта*/

poiin(invnom,da,ma,ga,&kod_pod,&kod_mo);

class iceb_tu_str naim_podr("");
class iceb_tu_str naim_mo("");

/*читаем наименование подразделения и материально - отвецтвенного*/

sprintf(strsql,"select naik from Uosol where kod=%d",kod_mo);
if(readkey(strsql,&row,&cur) == 1)
 naim_mo.new_plus(row[0]);

sprintf(strsql,"select naik from Uospod where kod=%d",kod_pod);
if(readkey(strsql,&row,&cur) == 1)
 naim_podr.new_plus(row[0]);
 
/*узнаём счёт учёта*/
class poiinpdw_data rin;
poiinpd(invnom,ma,ga,&rin);

/*узнаём балансовую стоимость*/

class bsizw_data bsiz_invnom;
bsiz(invnom,kod_pod,31,ma,ga,&bsiz_invnom,NULL);




sprintf(imaf,"avve%d.lst",invnom);

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }




fprintf(ff,"%s\n",iceb_t_get_pnk("00",0));

fprintf(ff,"%110s          ЗАТВЕРДЖУЮ\n","");
fprintf(ff,"\n");
fprintf(ff,"%110s ___________________________\n","");
fprintf(ff,"%110s (підпис керівника установи)\n","");
fprintf(ff,"\n");
fprintf(ff,"%110s _____________________20____\n","");

fprintf(ff,"%55s                АКТ N%s\n","",nomer_akta.ravno());
fprintf(ff,"%55s введення в експлуатацію основних засобів\n","");
fprintf(ff,"%55s       від %02d.%02d.%d р.\n","",da,ma,ga);
fprintf(ff,"\n");

fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"\
|     Найменування об'єкта     |Місце знаходження|Матеріално-відповідальна|Рахунок|Інвентарний| Первісна  |   Знос    |Перелік налагоджувальних|\n\
|                              |    об'єкта      |       особа            |обліку |номер      | вартість  |           |       робіт            |\n");

fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------------------\n");

/*
 123456789012345678901234567890 12345678901234567 123456789012345678901234 1234567 12345678901 12345678901 12345678901 123456789012345678901234
*/


fprintf(ff,"|%-*.*s|%-*.*s|%-*.*s|%-*s|%-11d|%11.2f|%11.2f|                        |\n",
iceb_tu_kolbait(30,naim_invnom.ravno()),
iceb_tu_kolbait(30,naim_invnom.ravno()),
naim_invnom.ravno(),
iceb_tu_kolbait(17,naim_podr.ravno()),
iceb_tu_kolbait(17,naim_podr.ravno()),
naim_podr.ravno(),
iceb_tu_kolbait(24,naim_mo.ravno()),
iceb_tu_kolbait(24,naim_mo.ravno()),
naim_mo.ravno(),
iceb_tu_kolbait(7,rin.shetu.ravno()),
rin.shetu.ravno(),
invnom,
bsiz_invnom.sbs,
bsiz_invnom.siz);

if(iceb_tu_strlen(naim_invnom.ravno()) > 30)
 fprintf(ff,"|%-*.*s|%17s|%24s|%7s|%11s|%11s|%11s|%24s|\n",
 iceb_tu_kolbait(30,iceb_tu_adrsimv(30,naim_invnom.ravno())),
 iceb_tu_kolbait(30,iceb_tu_adrsimv(30,naim_invnom.ravno())),
 iceb_tu_adrsimv(30,naim_invnom.ravno()),
 "","","","","","","");


fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"Комісією створеною на підставі наказу, розпорядження від %02d.%02d.%d р. N %s проведено огляд\n\
%s,\n\
що підлягає передачі в експлуатацію на підставі накладної (вимоги) від %02d.%02d.%d р. N %s\n",
dp,mp,gp,nomer_prik.ravno(),naim_invnom.ravno(),dn,mn,gn,nomer_nakl.ravno());

fprintf(ff,"\nВ момент приймання об'єкт знаходиться %s\n",naim_podr.ravno());

fprintf(ff,"\nПід час установки та налагодження:\n\n\
1)перевірено відповідність об'єкта даним технічного паспорту, марки, дати виготовлення, усі показники відповідають технічній та іншій супровідний\n\
документації\n");

fprintf(ff,"\nВисновки: Об'єкт готовий до експлуатації\n\n\
Голова комісії:________________________________________   ____________________________  _______________________________\n\
                            (посада)                              (підпис)                     (прізвище, ініціали)\n\
\n\
Члени комісі:  ________________________________________   ____________________________  _______________________________\n\
                            (посада)                              (підпис)                     (прізвище, ініціали)\n\
\n\
               ________________________________________   ____________________________  _______________________________\n\
                            (посада)                              (підпис)                     (прізвище, ініціали)\n\
\n\
               ________________________________________   ____________________________  _______________________________\n\
                            (посада)                              (підпис)                     (прізвище, ініціали)\n");
                             
podpis(ff);

fclose(ff);
iceb_t_ustpeh(imaf,3);
class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка акта ввода в эксплуатацию"));
iceb_t_rabfil(&oth,"");

}

