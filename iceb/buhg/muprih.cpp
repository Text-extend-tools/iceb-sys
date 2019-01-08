/*$Id:$*/
/*01.12.2016	03.10.2014	Белых А.И.	muprih.c
Получение последних приходов по материалам
*/
#include "buhg.h"
#include "mugetdpp.h"

void muprih_hap(int nomstr,int *kolstr,FILE *ff);

int muprih(class spis_oth *oth)
{
int kom=0,kom1=0;
int kgr=0;
int kodm=0;
static class iceb_tu_str datan("");
static class iceb_tu_str datak("");
static class iceb_tu_str shetu("");
static class iceb_tu_str sklad("");
static class iceb_tu_str grupa("");
static class iceb_tu_str kodmat("");
static class iceb_tu_str naimat("");
static class iceb_tu_str kodop("");

class VVOD VV(0);
class VVOD MENU(3);

VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка материалов с последней датой получения"));
VV.VVOD_SPISOK_add_ZAG(gettext("Введите реквизиты для получения распечатки"));

VV.VVOD_SPISOK_add_data(datan.ravno(),11);
VV.VVOD_SPISOK_add_data(datak.ravno(),11);
VV.VVOD_SPISOK_add_data(shetu.ravno(),128);
VV.VVOD_SPISOK_add_data(sklad.ravno(),128);
VV.VVOD_SPISOK_add_data(grupa.ravno(),128);
VV.VVOD_SPISOK_add_data(kodmat.ravno(),128);
VV.VVOD_SPISOK_add_data(naimat.ravno(),128);
VV.VVOD_SPISOK_add_data(kodop.ravno(),128);
  
VV.VVOD_SPISOK_add_MD(gettext("Дата начала...................."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца....................."));//1
VV.VVOD_SPISOK_add_MD(gettext("Счёт.....................(,,).."));//2
VV.VVOD_SPISOK_add_MD(gettext("Склад....................(,,).."));//3
VV.VVOD_SPISOK_add_MD(gettext("Группа материалов........(,,).."));//4
VV.VVOD_SPISOK_add_MD(gettext("Код материалла.......... (,,).."));//5
VV.VVOD_SPISOK_add_MD(gettext("Наименование материалла..(,,).."));//6
VV.VVOD_SPISOK_add_MD(gettext("Код операции.............(,,).."));//7

  

naz:;
clear();



helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

datan.new_plus(VV.data_ravno(0));
datak.new_plus(VV.data_ravno(1));
shetu.new_plus(VV.data_ravno(2));
sklad.new_plus(VV.data_ravno(3));
grupa.new_plus(VV.VVOD_SPISOK_return_data(4));
kodmat.new_plus(VV.VVOD_SPISOK_return_data(5));
naimat.new_plus(VV.VVOD_SPISOK_return_data(6));
kodop.new_plus(VV.VVOD_SPISOK_return_data(7));

class iceb_tu_str kods("");
class iceb_tu_str naims("");
switch (kom)
 {
  case ESC:
  case FK10:
     return(1);
     break;

  case FK1:
    GDITE();
    iceb_t_pdoc("matu5_4.txt");
    goto naz;

  case PLU:
  case FK2:
     break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка складов"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка материалов"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций"));//4
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//5

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 5 :
      case -1:
       goto naz;


      case 0 :
        if(dirsklad(1,&kods,&naims) == 0)
         {        
            VV.data_z_plus(3,kods.ravno());
         }
        goto naz;

      case 1 :
        vibrek("Grup",&kods);
        VV.VVOD_SPISOK_zapis_data(4,kods.ravno());
        goto naz;

      case 2 :

        if(dirmatr(&kgr,&kodm,&naims,1,0,"",0,0) == 0)
         {
            VV.data_z_plus(5,kodm);
         }

        clear();
        goto naz;

      case 3 :
        vibrek("Plansh",&shetu);
        VV.VVOD_SPISOK_zapis_data(2,shetu.ravno());
        goto naz;
      case 4:
//        vibrek("Prihod",&kodop);
       if(dirprihod(1,&kodop,&naims) == 0)
         VV.data_plus(7,kodop.ravno());
        goto naz;

     }

  case FK4:
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
    break;
 }

char strsql[512];
SQL_str row;
class SQLCURSOR cur;
int kolstr=0;

if(datan.getdlinna() > 1)
 if(rsdatp(datan.ravno(),&datak) != 0)
 goto naz;
GDITE();
sprintf(strsql,"select kodm,kodgr,naimat from Material order by naimat asc");
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

class iceb_fopen fil_lst;
char imaf_lst[64];
sprintf(imaf_lst,"prih%d.lst",getpid());
if(fil_lst.start(imaf_lst,"w") != 0)
 return(1);

iceb_t_zagolov(gettext("Список материалов с последней датой получения"),datan.ravno(),datak.ravno(),fil_lst.ff);

if(shetu.getdlinna() > 1)
 fprintf(fil_lst.ff,"%s:%s\n",gettext("Счёт учёта"),shetu.ravno());
if(sklad.getdlinna() > 1)
 fprintf(fil_lst.ff,"%s:%s\n",gettext("Склад"),sklad.ravno());
if(grupa.getdlinna() > 1)
 fprintf(fil_lst.ff,"%s:%s\n",gettext("Группа материала"),grupa.ravno());
if(kodmat.getdlinna() > 1)
 fprintf(fil_lst.ff,"%s:%s\n",gettext("Код материала"),kodmat.ravno());
if(naimat.getdlinna() > 1)
 fprintf(fil_lst.ff,"%s:%s\n",gettext("Наименование материала"),naimat.ravno());
if(kodop.getdlinna() > 1)
 fprintf(fil_lst.ff,"%s:%s\n",gettext("Код операции"),kodop.ravno());
 
muprih_hap(1,NULL,fil_lst.ff);
 
class iceb_fopen fil_csv;
char imaf_csv[64];
sprintf(imaf_csv,"prih%d.csv",getpid());
if(fil_csv.start(imaf_csv,"w") != 0)
 return(1);

iceb_t_zageks(gettext("Список материалов с последней датой получения"),fil_csv.ff);
fprintf(fil_csv.ff,"#%s\n",gettext("Расшифровка полей"));
fprintf(fil_csv.ff,"#1-%s\n",gettext("Счёт"));
fprintf(fil_csv.ff,"#2-%s\n",gettext("Склад"));
fprintf(fil_csv.ff,"#3-%s\n",gettext("Код материала"));
fprintf(fil_csv.ff,"#4-%s\n",gettext("Наименование материала"));
fprintf(fil_csv.ff,"#5-%s\n",gettext("Цена"));
fprintf(fil_csv.ff,"#6-%s\n",gettext("Количество"));
fprintf(fil_csv.ff,"#7-%s\n",gettext("Дата последнего получения"));
fprintf(fil_csv.ff,"#8-%s\n",gettext("Код контрагента"));
fprintf(fil_csv.ff,"#9-%s\n",gettext("Наименование контрагента"));

class iceb_fopen fil_txt;
char imaf_txt[64];
sprintf(imaf_txt,"priht%d.csv",getpid());
if(fil_txt.start(imaf_txt,"w") != 0)
 return(1);

class mugetdpp_str rk;
int kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(proverka(kodmat.ravno(),row[0],0,0) != 0)
   continue;

  if(proverka(grupa.ravno(),row[1],0,0) != 0)
   continue;

  if(proverka(naimat.ravno(),row[2],4,0) != 0)
   continue;

  if(mugetdpp(atoi(row[0]),&rk,datan.ravno(),datak.ravno()) != 0)
   continue;


  for(int nom=0; nom < rk.sklads.kolih(); nom++)
   {
    if(proverka(shetu.ravno(),rk.shetus.ravno(nom),0,0) != 0)
     continue;
    if(proverka(sklad.ravno(),rk.sklads.ravno(nom),0,0) != 0)
     continue;
    if(proverka(kodop.ravno(),rk.kodops.ravno(nom),0,0) != 0)
     continue;

    fprintf(fil_lst.ff,"%-*s %-5d %-8s %-*.*s %-*s %10.10g %10.10g %10.2f %s %s %s\n",
    iceb_tu_kolbait(8,rk.shetus.ravno(nom)),
    rk.shetus.ravno(nom),
    rk.sklads.ravno(nom),
    row[0],
    iceb_tu_kolbait(40,row[2]),
    iceb_tu_kolbait(40,row[2]),
    row[2],
    iceb_tu_kolbait(9,rk.eis.ravno(nom)),
    rk.eis.ravno(nom),
    rk.cenas.ravno(nom),
    rk.kolihs.ravno(nom),
    rk.cenas.ravno(nom)*rk.kolihs.ravno(nom),
    rk.dataps.ravno(nom),
    rk.kodkontrs.ravno(nom),
    iceb_t_get_pnk(rk.kodkontrs.ravno(nom),0));

    if(iceb_tu_strlen(row[2]) > 40)
     fprintf(fil_lst.ff,"%23s %s\n","",iceb_tu_adrsimv(40,row[2]));

    fprintf(fil_csv.ff,"%s|%d|%s|%s|%s|%.10g|%.10g|%s|%s|%s|\n",
    rk.shetus.ravno(nom),
    rk.sklads.ravno(nom),
    row[0],
    row[2],
    rk.eis.ravno(nom),
    rk.cenas.ravno(nom),
    rk.kolihs.ravno(nom),
    rk.dataps.ravno(nom),
    rk.kodkontrs.ravno(nom),
    iceb_t_get_pnk(rk.kodkontrs.ravno(nom),0));

    
    fprintf(fil_txt.ff,"%-*s%-5d%-8s%-*.*s%-*s%10.10g%10.10g%10.2f%s%-*s%-*s\n",
    iceb_tu_kolbait(20,rk.shetus.ravno(nom)),
    rk.shetus.ravno(nom),
    rk.sklads.ravno(nom),
    row[0],
    iceb_tu_kolbait(100,row[2]),
    iceb_tu_kolbait(100,row[2]),
    row[2],
    iceb_tu_kolbait(10,rk.eis.ravno(nom)),
    rk.eis.ravno(nom),
    rk.cenas.ravno(nom),
    rk.kolihs.ravno(nom),
    rk.cenas.ravno(nom)*rk.kolihs.ravno(nom),
    rk.dataps.ravno(nom),
    iceb_tu_kolbait(20,rk.kodkontrs.ravno(nom)),
    rk.kodkontrs.ravno(nom),
    iceb_tu_kolbait(255,iceb_t_get_pnk(rk.kodkontrs.ravno(nom),0)),
    iceb_t_get_pnk(rk.kodkontrs.ravno(nom),0));
   }
 }
fprintf(fil_lst.ff,"ICEB_LST_END\n\
--------------------------------------------------------------------------------------------------------------------------------------\n");
podpis(fil_lst.ff);

fil_lst.end();
fil_csv.end();
fil_txt.end();

oth->spis_imaf.plus(imaf_lst);
oth->spis_naim.plus(gettext("Список материалов с последней датой получения"));

kolstr=iceb_t_ustpeh(imaf_lst,3);
iceb_t_rnl(imaf_lst,kolstr,NULL,&muprih_hap);

oth->spis_imaf.plus(imaf_csv);
oth->spis_naim.plus(gettext("Список материалов с последней датой получения csv"));

oth->spis_imaf.plus(imaf_txt);
oth->spis_naim.plus(gettext("Список материалов с последней датой получения txt"));

return(0);
}
/*********************************/
/*шапка*/
/****************************/
void muprih_hap(int nomstr,int *kolstr,FILE *ff)
{
if(kolstr != NULL)
  *kolstr+=4;
fprintf(ff,"%110s %s N%d\n","",gettext("Страница"),nomstr);

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
  Счёт  |Склад|Код мат.|     Наименование материала             |Един.изм.|  Цена    |Количество|  Сумма   |Дата пол. | Контрагент\n"));
/*
12345678 12345 12345678 1234567890123456789012345678901234567890 123456789 1234567890 1234567890 1234567890
*/
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------\n");
}
