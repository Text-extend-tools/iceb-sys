/*$Id: uospos.c,v 5.5 2013/09/26 09:43:42 sasa Exp $*/
/*22.01.2016	08.10.2011	Белых А.И.	uospos.c
Распечатать перечень основных средств
*/
#include <math.h>
#include <errno.h>
#include "buhg.h"
#include "poiinpd.h"
#include "bsizt.h"

int uospos(class spis_oth *oth)
{
int kom=0;
int kom1=0;
static class iceb_tu_str datar("");
static class iceb_tu_str shetu("");
static class iceb_tu_str innom("");
static class iceb_tu_str naimos("");
static class iceb_tu_str podrp("");
static class iceb_tu_str matotp("");
class VVOD MENU(3);
class VVOD VV(0);
VV.VVOD_SPISOK_add_data(datar.ravno(),11);
VV.VVOD_SPISOK_add_data(shetu.ravno(),255);
VV.VVOD_SPISOK_add_data(innom.ravno(),255);
VV.VVOD_SPISOK_add_data(podrp.ravno(),255);
VV.VVOD_SPISOK_add_data(matotp.ravno(),255);
VV.VVOD_SPISOK_add_data(naimos.ravno(),255);

VV.VVOD_SPISOK_add_MD(gettext("Дата инвентаризации....."));//0
VV.VVOD_SPISOK_add_MD(gettext("Счёт учета........(,,).."));//1
VV.VVOD_SPISOK_add_MD(gettext("Инвентарний номер.(,,).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Подразделение.....(,,).."));//3
VV.VVOD_SPISOK_add_MD(gettext("Мат.ответственный.(,,).."));//4
VV.VVOD_SPISOK_add_MD(gettext("Наименование......(,,).."));//5

VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка перечня основных средств"));

naz:;

clear();

helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

datar.new_plus(VV.data_ravno(0));
shetu.new_plus(VV.data_ravno(1));
innom.new_plus(VV.data_ravno(2));
podrp.new_plus(VV.data_ravno(3));
matotp.new_plus(VV.data_ravno(4));
naimos.new_plus(VV.data_ravno(5));

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
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка мат.-ответственных"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 3 :
      case -1:
       goto naz;

      case 0 :
        vibrek("Plansh",&kod_v,&naim_v);
        VV.data_z_plus(1,kod_v.ravno());
        goto naz;
      case 1 :
        if(diruospod(1,&kod_v,&naim_v,0) == 0)
          VV.data_z_plus(3,kod_v.ravno());
        goto naz;
      case 2 :
        if(dirmatot(1,&kod_v,&naim_v,0) == 0)
          VV.data_z_plus(4,kod_v.ravno());
        goto naz;
    }
  case FK4:
    VV.VVOD_clear_data();
    goto naz;


  default:
    goto naz;
 }
short dr,mr,gr;
if(rsdat(&dr,&mr,&gr,datar.ravno(),1) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата"));
  goto naz;
 }

MENU.VVOD_delete();
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите какая балансовая стоимости должна быть в документе"));

MENU.VVOD_SPISOK_add_MD(gettext("Балансовая стоимость для бухгалтерского учёта"));
MENU.VVOD_SPISOK_add_MD(gettext("Балансовая стоимость для налогового учёта"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
int vbs=0;
while(menu3w(stdscr,&MENU,&vbs,-1,-1,0) != 0);

if(vbs == -1 || vbs == 2)
  goto naz;

GDITE();

class SQLCURSOR cur,cur1;
SQL_str row,row1;
char strsql[512];
int kolstr=0;
sprintf(strsql,"select innom,naim,datvv from Uosin order by innom asc");
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


/*сортировка по счетам учёта*/
class iceb_t_tmptab tabtmp;
const char *imatmptab={"uospos"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
innom INT NOT NULL,\
naim CHAR(255) NOT NULL,\
datvv DATE NOT NULL,\
shetu CHAR(64) NOT NULL,\
metka_ps INT NOT NULL,\
sklad int not null,\
nomk int not null,\
kodm int not null,\
bs double (16,2) NOT NULL,\
iz double (16,2) NOT NULL,\
kolih double (16,6) NOT NULL) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
  return(1);
  
int kolstr1=0;
class poiinpdw_data rekin;
class bsizw_data bal_st;
int podr=0;
int matot=0;
double nach_bs=0.;
double ost_bs=0.;

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(innom.ravno(),row[0],0,0) != 0)
   continue;

  if(proverka(naimos.ravno(),row[1],4,0) != 0)
    continue;

  if(poiin(atoi(row[0]),dr,mr,gr,&podr,&matot) != 0) //Произошел расход или оно пришло позже даты
    continue;

  if(proverka(podrp.ravno(),podr,0,0) != 0)
   continue;
  if(proverka(matotp.ravno(),matot,0,0) != 0)
   continue;
   
  poiinpd(atoi(row[0]),mr,gr,&rekin);

  if(proverka(shetu.ravno(),rekin.shetu.ravno(),1,0) != 0)
    continue;

  bsiz(atoi(row[0]),podr,dr,mr,gr,&bal_st,NULL);
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

  sprintf(strsql,"insert into %s values (%s,'%s','%s','%s',0,%d,0,0,%.2f,%.2f,1.)",
  imatmptab,
  row[0],
  iceb_tu_sqlfiltr(row[1]),
  row[2],
  rekin.shetu.ravno(),
  matot,
  nach_bs,ost_bs);

  sql_zapis(strsql,1,0);    


 }

/*смотрим основные средства в материальном учёте*/
sprintf(strsql,"select sklad,nomk,kodm,shetu,cena,innom,datv from Kart where innom != 0");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
class ostatokt ost;
class iceb_tu_str naim_mat("");

kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(proverka(innom.ravno(),row[5],0,0) != 0)
   continue;
  if(proverka(shetu.ravno(),row[3],1,0) != 0)
   continue;
   
  ostkar(1,1,gr,dr,mr,gr,atoi(row[0]),atoi(row[1]),&ost);

  if(fabs(ost.ostg[3]) < 0.0000001)
   continue;  

  sprintf(strsql,"select naimat from Material where kodm=%s",row[2]);
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_mat.new_plus(row1[0]);
  else
   naim_mat.new_plus("");
  
  if(proverka(naimos.ravno(),naim_mat.ravno(),4,0) != 0)
      continue;

  sprintf(strsql,"insert into %s values (%s,'%s','%s','%s',1,%s,%s,%s,%.2f,0.,%.10g)",
  imatmptab,
  row[0],
  naim_mat.ravno_filtr(),
  row[6],
  row[3],
  row[0],
  row[1],
  row[2],
  ost.ostgc[3],
  ost.ostg[3]);

  sql_zapis(strsql,1,0);    

 }

sprintf(strsql,"select * from %s order by shetu asc,naim asc",imatmptab);
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







kolstr1=0;
char imaf[64];
FILE *ff;

sprintf(imaf,"uospon%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

zagolov(gettext("Перечень основных фондов"),0,0,0,dr,mr,gr,iceb_t_get_pnk("00",0),ff);

if(shetu.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Счёт учёта"),shetu.ravno());
if(innom.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Инвентарный номер"),innom.ravno());
if(naimos.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Наименование"),naimos.ravno());
if(podrp.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Подразделение"),podrp.ravno());
if(matotp.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Материально ответственный"),matotp.ravno());
 
fprintf(ff,"\n%s\n",gettext("Подсистема \"Учёт основных средств\""));
 
fprintf(ff,"\
-------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
 н/п  |Инветарный|  Наименование основного      |Количество|Дата ввода|Начальная |Остаточная|П.с.|\n\
      |  номер   |       средства               |          |в эксплуа.|стоимость |стоимость |    |\n"));
fprintf(ff,"\
-------------------------------------------------------------------------------------------------\n");

int nomer_pp=0;
double itogo[2]={0,0};
double kolih=0.;
kolstr1=0;
class iceb_tu_str naim_mo_skl("");
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  nach_bs=atof(row[8]);
  ost_bs=atof(row[9]);
  if(atoi(row[4]) == 0)
   {
    sprintf(strsql,"select naik from Uosol where kod=%s",row[5]);
    if(readkey(strsql,&row1,&cur1) == 1)
     naim_mo_skl.new_plus(row1[0]);
    else
     naim_mo_skl.new_plus("");
     
    itogo[0]+=nach_bs;
    itogo[1]+=ost_bs;
    
    fprintf(ff,"%6d|%10s|%-*.*s|%10d|%10s|%10.2f|%10.2f|УОС|%s %s\n",
    ++nomer_pp,
    row[0],
    iceb_tu_kolbait(30,row[1]),
    iceb_tu_kolbait(30,row[1]),
    row[1],
    1,
    iceb_tu_datzap(row[2]),
    nach_bs,
    ost_bs,
    row[5],
    naim_mo_skl.ravno());    

    for(int nom=30; nom < iceb_tu_strlen(row[1]); nom+=30)
      fprintf(ff,"%6s|%10s|%-*.*s|%10s|%10s|%10s|%10s|   |\n",
      "","",
      iceb_tu_kolbait(30,iceb_tu_adrsimv(nom,row[1])),
      iceb_tu_kolbait(30,iceb_tu_adrsimv(nom,row[1])),
      iceb_tu_adrsimv(nom,row[1]),
      "","","","");
          
   }
  else
   {
    sprintf(strsql,"select naik from Sklad where kod=%s",row[5]);
    if(readkey(strsql,&row1,&cur1) == 1)
     naim_mo_skl.new_plus(row1[0]);
    else
     naim_mo_skl.new_plus("");

    kolih=atof(row[10]);

    fprintf(ff,"%6d|%*s|%-*.*s|%10.10g|%10s|%10.2f|%10.2f|МУ |%s %s\n",
    ++nomer_pp,
    iceb_tu_kolbait(10,row[0]),
    row[0],
    iceb_tu_kolbait(30,row[1]),
    iceb_tu_kolbait(30,row[1]),
    row[1],
    kolih,
    iceb_tu_datzap(row[2]),
    nach_bs,
    0.,
    row[5],
    naim_mo_skl.ravno());

    for(int nom=30; nom < iceb_tu_strlen(row[1]); nom+=30)
      fprintf(ff,"%6s|%10s|%-*.*s|%10s|%10s|%10s|%10s|   |\n",
      "","",
      iceb_tu_kolbait(30,iceb_tu_adrsimv(nom,row[1])),
      iceb_tu_kolbait(30,iceb_tu_adrsimv(nom,row[1])),
      iceb_tu_adrsimv(nom,row[1]),
      "","","","");
    
    itogo[0]+=nach_bs;

   }  
 }


fprintf(ff,"\
---------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f %10.2f\n",
iceb_tu_kolbait(70,gettext("Итого")),gettext("Итого"),
itogo[0],itogo[1]);


podpis(ff);
fclose(ff);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Перечень основных средств"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);

sprintf(strsql,"%s:%10.2f %10.2f",gettext("Итого"),itogo[0],itogo[1]);
iceb_t_soob(strsql);

return(0);
}
