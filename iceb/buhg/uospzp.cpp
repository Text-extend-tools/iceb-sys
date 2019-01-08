/*$Id: uospzp.c,v 5.3 2013/11/05 10:50:49 sasa Exp $*/
/*16.07.2015	20.05.2013	Белых А.И.	uospzp.c
Распечатка перечня основных средств полученных за определённый период
*/
#include <errno.h>
#include "buhg.h"
#include "poiinpd.h"
#include "bsizt.h"

int uospzp(class spis_oth *oth)
{
int kom=0,kom1=0;
static class iceb_tu_str datn("");
static class iceb_tu_str datk("");
static class iceb_tu_str shetu("");
static class iceb_tu_str innom("");

class VVOD MENU(3);
class VVOD VV(0);
VV.VVOD_SPISOK_add_data(datn.ravno(),11);
VV.VVOD_SPISOK_add_data(datk.ravno(),11);
VV.VVOD_SPISOK_add_data(shetu.ravno(),255);
VV.VVOD_SPISOK_add_data(innom.ravno(),255);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.(д.м.г)....."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..(д.м.г)....."));//1
VV.VVOD_SPISOK_add_MD(gettext("Счёт учета........(,,).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Инвентарний номер.(,,).."));//3

VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка перечня полученных основных средств"));

naz:;

clear();

helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

datn.new_plus(VV.data_ravno(0));
datk.new_plus(VV.data_ravno(1));
shetu.new_plus(VV.data_ravno(2));
innom.new_plus(VV.data_ravno(3));

class iceb_tu_str kod_v("");
class iceb_tu_str naim_v("");
switch (kom)
 {

  case FK10:
  case ESC:
   return(1);

  case FK1:
    GDITE();
//    prosf(bros);
//    clear();
    goto naz;

  case FK2:
  case PLU:
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите  нужное и нажмите Enter"));

    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 1 :
      case -1:
       goto naz;

      case 0 :
        vibrek("Plansh",&kod_v,&naim_v);
        VV.data_z_plus(2,kod_v.ravno());
        goto naz;
     }
  case FK4:
    VV.VVOD_clear_data();
    goto naz;


  default:
    goto naz;
 }

short dn,mn,gn;
short dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,datn.ravno(),&dk,&mk,&gk,datk.ravno()) != 0)
  goto naz;

MENU.VVOD_delete();
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите какая балансовая стоимости должна быть в документе"));

MENU.VVOD_SPISOK_add_MD(gettext("Балансовая стоимость для бухгалтерского учёта"));
MENU.VVOD_SPISOK_add_MD(gettext("Балансовая стоимость для налогового учёта"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
int vbs=0;
while(menu3w(stdscr,&MENU,&vbs,-1,-1,0) != 0);

if(vbs == -1 || vbs == 2)
  goto naz;

class SQLCURSOR cur,cur1;
SQL_str row,row1;
char strsql[512];
int kolstr=0;

sprintf(strsql,"select innom,kodop from Uosdok1 where datd >= '%04d-%02d-%01d' and datd <= '%04d-%02d-%01d' and tipz=1 order by innom asc",
gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }
char imaf[64];
sprintf(imaf,"uospzp%d.lst",getpid());
class iceb_fopen fil;

if(fil.start(imaf,"w") != 0)
 return(1);

char imaf1[64];
sprintf(imaf1,"uospzp1%d.lst",getpid());
class iceb_fopen fil1;

if(fil1.start(imaf1,"w") != 0)
 return(1);

zagolov(gettext("Перечень полученных основных фондов"),dn,mn,gn,dk,mk,gk,iceb_t_get_pnk("00",0),fil.ff);
zagolov(gettext("Перечень полученных основных фондов"),dn,mn,gn,dk,mk,gk,iceb_t_get_pnk("00",0),fil1.ff);

if(shetu.getdlinna() > 1)
 {
  fprintf(fil.ff,"%s:%s\n",gettext("Счёт учёта"),shetu.ravno());
  fprintf(fil1.ff,"%s:%s\n",gettext("Счёт учёта"),shetu.ravno());
 }

if(innom.getdlinna() > 1)
 {
  fprintf(fil.ff,"%s:%s\n",gettext("Инвентарный номер"),innom.ravno());
  fprintf(fil1.ff,"%s:%s\n",gettext("Инвентарный номер"),innom.ravno());
 }
 
fprintf(fil.ff,"\
---------------------------------------------------------------------------------------------\n");

fprintf(fil.ff,gettext("\
 н/п  |Инветарный|  Наименование основного      |Количество|Дата ввода|Начальная |Остаточная|\n\
      |  номер   |       средства               |          |в эксплуа.|стоимость |стоимость |\n"));
fprintf(fil.ff,"\
---------------------------------------------------------------------------------------------\n");

fprintf(fil1.ff,"\
----------------------------------------------------------------------------------\n");

fprintf(fil1.ff,gettext("\
 н/п  |Инветарный|  Наименование основного      |Количество|Начальная |Остаточная|\n\
      |  номер   |       средства               |          |стоимость |стоимость |\n"));
fprintf(fil1.ff,"\
----------------------------------------------------------------------------------\n");

int kolstr1=0;
class poiinpdw_data rekin;
class bsizw_data bal_st;
int podr=0;
int matot=0;
double nach_bs=0.;
double ost_bs=0.;
int nomer_pp=0;
double itogo[2]={0,0};

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  /*проверяем код операции это должна быть внешняя операция*/
  sprintf(strsql,"select vido from Uospri where kod='%s'",row[1]);
  if(readkey(strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s %s!",gettext("Не найден код операции"),row[1]);
    iceb_t_soob(strsql);
    continue;
   }

  if(atoi(row1[0]) != 0)
   continue;
   
  if(proverka(innom.ravno(),row[0],0,0) != 0)
   continue;

  if(poiin(atoi(row[0]),dk,mk,gk,&podr,&matot) != 0) //Произошел расход или оно пришло позже даты
    continue;

  poiinpd(atoi(row[0]),mk,gk,&rekin);

  if(proverka(shetu.ravno(),rekin.shetu.ravno(),1,0) != 0)
    continue;

  bsiz(atoi(row[0]),podr,dk,mk,gk,&bal_st,NULL);

  if(vbs == 0)
   {
    nach_bs=bal_st.sbsby+bal_st.bsby;
    ost_bs=nach_bs-bal_st.sizby-bal_st.izby-bal_st.iz1by;
   }
  else
   {
    nach_bs=bal_st.sbs+bal_st.bs;
    ost_bs=nach_bs-bal_st.siz-bal_st.iz-bal_st.iz1;
   }
  
  itogo[0]+=nach_bs;
  itogo[1]+=ost_bs;
  
  sprintf(strsql,"select naim,datvv from Uosin where innom=%s",row[0]);
  if(readkey(strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s %s!",gettext("Не найден инвентарный номер"),row[0]);
    iceb_t_soob(strsql);
    continue;
   }

  fprintf(fil.ff,"%6d|%10s|%-*.*s|%10d|%10s|%10.2f|%10.2f|\n",
  ++nomer_pp,
  row[0],
  iceb_tu_kolbait(30,row1[0]),
  iceb_tu_kolbait(30,row1[0]),
  row1[0],
  1,
  iceb_tu_datzap(row1[1]),
  nach_bs,ost_bs);    

  for(int nom=30; nom < iceb_tu_strlen(row1[0]); nom+=30)
   fprintf(fil.ff,"%6s|%10s|%-*.*s|%10s|%10s|%10s|%10s|\n",
   "","",
   iceb_tu_kolbait(30,iceb_tu_adrsimv(nom,row1[0])),
   iceb_tu_kolbait(30,iceb_tu_adrsimv(nom,row1[0])),
   iceb_tu_adrsimv(nom,row1[0]),
   "","","","");
     
  fprintf(fil1.ff,"%6d|%10s|%-*.*s|%10d|%10.2f|%10.2f|\n",
  nomer_pp,
  row[0],
  iceb_tu_kolbait(30,row1[0]),
  iceb_tu_kolbait(30,row1[0]),
  row1[0],
  1,
  nach_bs,ost_bs);    

  for(int nom=30; nom < iceb_tu_strlen(row1[0]); nom+=30)
   fprintf(fil1.ff,"%6s|%10s|%-*.*s|%10s|%10s|%10s|\n",
   "","",
   iceb_tu_kolbait(30,iceb_tu_adrsimv(nom,row1[0])),
   iceb_tu_kolbait(30,iceb_tu_adrsimv(nom,row1[0])),
   iceb_tu_adrsimv(nom,row1[0]),
   "","","");
  
 }
fprintf(fil.ff,"\
---------------------------------------------------------------------------------------------\n");
fprintf(fil.ff,"%*s:%10.2f %10.2f\n",
iceb_tu_kolbait(70,gettext("Итого")),gettext("Итого"),
itogo[0],itogo[1]);

fprintf(fil1.ff,"\
----------------------------------------------------------------------------------\n");
fprintf(fil1.ff,"%*s:%10.2f %10.2f\n",
iceb_tu_kolbait(59,gettext("Итого")),gettext("Итого"),
itogo[0],itogo[1]);

podpis(fil.ff);
fil.end();
podpis(fil1.ff);
fil1.end();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Перечень полученных основных средств"));
oth->spis_imaf.plus(imaf1);
oth->spis_naim.plus(gettext("Перечень полученных основных средств без даты ввода"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);

sprintf(strsql,"%s:%10.2f %10.2f",gettext("Итого"),itogo[0],itogo[1]);
iceb_t_soob(strsql);

return(0);
}
