/*$Id: murfn.c,v 5.7 2013/08/13 05:49:49 sasa Exp $*/
/*27.02.2017	24.12.2010	Белых А.И.	murfn.c
Расчёт фактической наценки
*/
#include <errno.h>
#include "buhg.h"


int murfn(class spis_oth *oth)
{
int kom=0,kom1=0;
int kgr=0,kodm=0;
char strsql[512];
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
static class iceb_tu_str sklad("");
static class iceb_tu_str kodop("");
static class iceb_tu_str kodmat("");
short dn=0,mn=0,gn=0;
short dk=0,mk=0,gk=0;



VVOD MENU(3);
VVOD VV(0);

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(sklad.ravno(),112);
VV.VVOD_SPISOK_add_data(kodop.ravno(),112);
VV.VVOD_SPISOK_add_data(kodmat.ravno(),112);

VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт процента наценки"));
VV.VVOD_SPISOK_add_ZAG(gettext("Введите нужные реквизиты"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.........."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..........."));//1
VV.VVOD_SPISOK_add_MD(gettext("Склад.........(,,)..."));//2
VV.VVOD_SPISOK_add_MD(gettext("Код операции..(,,)..."));//3
VV.VVOD_SPISOK_add_MD(gettext("Код материалла..(,,)."));//4

naz:;

clear();

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("поиск"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,0,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
sklad.new_plus(VV.data_ravno(2));
kodop.new_plus(VV.data_ravno(3));
kodmat.new_plus(VV.data_ravno(4));

class iceb_tu_str kods("");
class iceb_tu_str naim("");
switch (kom)
 {

  case FK10:
  case ESC:
   return(1);

  case FK1:
    GDITE();
    iceb_t_pdoc("matu5_5.txt");
    clear();
    goto naz;

  case FK2:
  case PLU:
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка складов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций расходов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка материалов"));
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
        if(dirsklad(1,&kods,&naim) == 0)
         VV.data_z_plus(2,kods.ravno());
        goto naz;

      case 1 :
        if(dirrashod(1,&kodop,&naim) == 0)
         VV.data_z_plus(3,kodop.ravno());
        goto naz;
        
      case 2 :
        if(dirmatr(&kgr,&kodm,&naim,1,0,"",0,0) == 0)
         {
          VV.data_z_plus(4,kodm);
         }
        
        goto naz;

     }
    goto naz;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;
     
  default:
    goto naz;
 }

if(rsdatp(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(0),&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(1)) != 0)
  goto naz;

class SQLCURSOR cur;
class SQLCURSOR cur1;
class SQLCURSOR cur2;
SQL_str row,row1,row2;
int kolstr=0;
int kolstr1=0;


sprintf(strsql,"select datd,sklad,nomd,kodop,pn from Dokummat where \
datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' and tip=2 and pod=1 order by datd asc",
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

FILE *ff;
char imaf[64];
sprintf(imaf,"rfn%d.lst",getpid());
if((ff=fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

iceb_t_zagolov(gettext("Расчёт процента наценки"),dn,mn,gn,dk,mk,gk,ff);

if(kodop.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Код операции"),kodop.ravno());
if(kodmat.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Код материалла"),kodmat.ravno());
if(sklad.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Код склада"),sklad.ravno());

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
Дата докум|Номер док.|Код мат.|   Наименование материала     |Ед.изм.|Количество|Цена учёта|Цена прод.|Процент|Сумма нац.|Дата записи, оператор\n");
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------\n");
/********
1234567890 1234567890 12345678 123456789012345678901234567890 1234567 1234567890 1234567890 1234567890 1234567 1234567890
**************/
double cena_uh=0.;
double cena_prod=0.;
double proc_nac=0.;
double kolih=0.;
int metka_nds=0;
float proc_nds=0.;
short dd,md,gd;
double suma_nac=0.;
double itogo_sum_nac=0;

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(proverka(sklad.ravno(),row[1],0,0) != 0)
   continue;

  if(proverka(kodop.ravno(),row[3],0,0) != 0)
   continue;
  rsdat(&dd,&md,&gd,row[0],2);
  
  proc_nds=atof(row[4]);
  
  metka_nds=0;

  /*читаем начисляестя ндс или нет*/
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%s and nomd='%s' and nomerz=11",
  gd,row[1],row[2]);
  if(readkey(strsql,&row1,&cur1) == 1)
   metka_nds=atoi(row1[0]);

       
  sprintf(strsql,"select nomkar,kodm,kolih,cena,ei,mnds,ktoi,vrem from Dokummat1 where \
  datd = '%s' and sklad=%s and nomd='%s'",row[0],row[1],row[2]);
  if(cur1.make_cursor(&bd,strsql) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return(1);
   }

  while(cur1.read_cursor(&row1) != 0)
   {
    if(proverka(kodmat.ravno(),row1[1],0,0) != 0)
     continue;
    naim.new_plus("");

    /*Узнаём наименование материала*/
    sprintf(strsql,"select naimat from Material where kodm=%s",row1[1]);
    if(readkey(strsql,&row2,&cur2) == 1)
     naim.new_plus(row2[0]);    

    cena_uh=0.;    
    /*Узнаём цену в карточке*/
    sprintf(strsql,"select mnds,cena,nds from Kart where sklad=%s and nomk=%s",row[1],row1[0]);
    if(readkey(strsql,&row2,&cur2) == 1)
     {
      cena_uh=atof(row2[1]);    
      if(atoi(row2[0]) == 0)
       cena_uh=cena_uh+cena_uh*atof(row2[2])/100.;
     }    

    cena_prod=atof(row1[3]);
    if(metka_nds == 0) /*начисляется ндс*/
     {
      cena_prod=cena_prod+cena_prod*proc_nds/100.;
     }

    kolih=atof(row1[2]);
    proc_nac=0.;
    if(cena_prod != 0. && cena_uh != 0.)
     {
      proc_nac=(cena_prod*100./cena_uh)-100.;
      proc_nac=okrug(proc_nac,0.001);
     }
    suma_nac=cena_prod*kolih-cena_uh*kolih;
    itogo_sum_nac+=suma_nac;

    fprintf(ff,"%02d.%02d.%04d|%-*s|%-8s|%-*.*s|%-*s|%10.10g|%10.10g|%10.10g|%7.7g|%10.2f|%s %s\n",
    dd,md,gd,
    iceb_tu_kolbait(10,row[2]),row[2],
    row1[1],
    iceb_tu_kolbait(30,naim.ravno()),        
    iceb_tu_kolbait(30,naim.ravno()),        
    naim.ravno(),        
    iceb_tu_kolbait(7,row1[4]),row1[4],
    atof(row1[2]),
    cena_uh,
    cena_prod,
    proc_nac,
    suma_nac,
    iceb_tu_vremzap(row1[7]),
    iceb_t_kszap(row1[6]));
   }
 }

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f\n",iceb_tu_kolbait(110,gettext("Итого")),gettext("Итого"),itogo_sum_nac);

podpis(ff);
fclose(ff);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт процента наценки"));

iceb_t_ustpeh(oth->spis_imaf.ravno(0),3);

return(0);
}

