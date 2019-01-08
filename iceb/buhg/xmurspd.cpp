/*$Id: xmurspd.c,v 5.15 2013/08/13 05:50:00 sasa Exp $*/
/*16.04.2017	26.03.2009	Белых А.И.	xmurspd.c
Распечатка списка по дате подтверждения документов
*/
#include        <errno.h>
#include        "buhg.h"

extern double   okrg1;
extern double	okrcn;

int xmurspd(class spis_oth *oth)
{
int kom=0,kom1=0;
char strsql[1024];
static class iceb_tu_str dat1(""),dat2("");
static class iceb_tu_str kopz("");
static class iceb_tu_str kodoz("");
static class iceb_tu_str sklz("");
static class iceb_tu_str prra("");
static class iceb_tu_str grupk(""); 

VVOD MENU(3);
VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(gettext("Распечатка списка документов по датам подтверждения"));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(kopz.ravno(),128);
VV.VVOD_SPISOK_add_data(kodoz.ravno(),128);
VV.VVOD_SPISOK_add_data(sklz.ravno(),128);
VV.VVOD_SPISOK_add_data(prra.ravno(),2);
VV.VVOD_SPISOK_add_data(grupk.ravno(),128);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала..(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца...(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Код операции..(,,)...."));
VV.VVOD_SPISOK_add_MD(gettext("Код контрагента..(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Склад (,,)............"));
VV.VVOD_SPISOK_add_MD(gettext("Приход/расход. (+/-).."));
VV.VVOD_SPISOK_add_MD(gettext("Код группы контр.(,,)."));

naz:;

clear();


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("поиск"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
dat2.new_plus(VV.VVOD_SPISOK_return_data(1));

kopz.new_plus(VV.VVOD_SPISOK_return_data(2));
kodoz.new_plus(VV.VVOD_SPISOK_return_data(3));
sklz.new_plus(VV.VVOD_SPISOK_return_data(4));
prra.new_plus(VV.VVOD_SPISOK_return_data(5));
grupk.new_plus(VV.VVOD_SPISOK_return_data(6));
class iceb_tu_str kods("");
class iceb_tu_str naims("");
switch (kom)
 {

  case FK10:
  case ESC:
   return(1);

  case FK1:
    GDITE();
    iceb_t_pdoc("matu5_8.txt");
    clear();
    goto naz;

  case FK2:
  case PLU:
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка складов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций приходов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций расходов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

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
         VV.data_z_plus(4,kods.ravno());
        goto naz;

      case 1 :
        vibrek("Kontragent",&kodoz);
        VV.VVOD_SPISOK_zapis_data(3,kodoz.ravno());
        goto naz;

      case 2 :
       if(dirprihod(1,&kods,&naims) == 0)
        VV.data_z_plus(2,kopz.ravno());
       goto naz;

      case 3 :
        if(dirrashod(1,&kods,&naims) == 0)
          VV.data_z_plus(2,kods.ravno());
        goto naz;

      case 4 :
        vibrek("Gkont",&grupk);
        VV.VVOD_SPISOK_zapis_data(6,grupk.ravno());
        goto naz;

     }
    goto naz;

  case FK4:
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
 }
short dn,mn,gn;
short dk,mk,gk;
if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
  goto naz;

int tipzz=0;
if(VV.VVOD_SPISOK_return_data(5)[0] == '+')
  tipzz=1;
if(VV.VVOD_SPISOK_return_data(5)[0] == '-')
  tipzz=2;
  

MENU.VVOD_delete();

MENU.VVOD_SPISOK_add_MD(gettext("Сортировка по датам"));
MENU.VVOD_SPISOK_add_MD(gettext("Сортировка по номерам документов"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

int srt=0;
while(menu3w(stdscr,&MENU,&srt,-1,-1,0) != 0);
if(srt == -1 || srt == 2)
  return(1);
time_t tmm=time(NULL);
clear(); /*Очистить экран*/

printw("%s %d.%d.%d%s %s %d.%d.%d%s\n",
gettext("Распечатка документов за период c"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."));
refresh();

GDITE();

SQL_str row;
class SQLCURSOR cur;
int kolstr=0;

if(srt == 0)
  sprintf(strsql,"select sklad,nomk,nomd,datdp,datd,tipz,kolih,cena from Zkart where datdp >= '%04d-%02d-%02d' \
  and datdp <= '%04d-%02d-%02d' order by datdp,nomd asc",gn,mn,dn,gk,mk,dk);

if(srt == 1)
  sprintf(strsql,"select sklad,nomk,nomd,datdp,datd,tipz,kolih,cena from Zkart where datdp >= '%04d-%02d-%02d' \
  and datdp <= '%04d-%02d-%02d' order by nomd,datdp asc",gn,mn,dn,gk,mk,dk);

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

int kolstr1=0;
char imaf[64];
sprintf(imaf,"muspd%d.tmp",getpid());
FILE *ff;
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
iceb_t_zagolov(gettext("Список документов по датам подтверждения"),dn,mn,gn,dk,mk,gk,ff);

if(tipzz == 1)
 {
  fprintf(ff,gettext("ТОЛЬКО ПРИХОДЫ\n"));
 }
if(tipzz == 2)
 {
  fprintf(ff,gettext("ТОЛЬКО РАСХОДЫ\n"));
 }

if(kopz.ravno()[0] != '\0')
 {
  fprintf(ff,"%s - %s\n",gettext("Код операции"),kopz.ravno());
 }
if(kodoz.ravno()[0] != '\0')
 {
  fprintf(ff,gettext("Код контрагента - %s\n"),kodoz.ravno());
 }

if(sklz.ravno()[0] != '\0')
 {
  fprintf(ff,gettext("Склад - %s\n"),sklz.ravno());
 }

if(grupk.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Код группы контрагента"),grupk.ravno());
 }
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
М|Дата подт.|Номер док.|Дата выпи.|Код оп.|Код мат.|Наименование материалла|Ед.изм.|Количество|   Цена   |Сумма з ПДВ|Наименование контрагента\n");
/****
             1234567890 1234567890 1234567 12345678 12345678901234567890123 1234567 1234567890 1234567890 12345678901
*******/
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------\n");
SQL_str row1;
class SQLCURSOR cur1;
class iceb_tu_str kod_kontr("");
class iceb_tu_str naim_kontr("");
class iceb_tu_str kod_op("");
class iceb_tu_str kod_mat("");
class iceb_tu_str naim_mat("");
class iceb_tu_str ei("");
char metka_dok[4];
short d1,m1,g1;
short d2,m2,g2;
int metka_nds=0;
double cena=0.;
double suma_s_nds=0.;
double kolih=0.;
float pnds=0.;
double itogo=0.;
double itogo_kol=0.;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(proverka(sklz.ravno(),row[0],0,0) != 0)
   continue;

  if(tipzz != 0)
   if(tipzz != atoi(row[5]))
       continue;

  /*Читаем шапку документа*/
  sprintf(strsql,"select kontr,kodop,pn from Dokummat where datd='%s' and sklad=%s and nomd='%s'",
  row[4],row[0],row[2]);
  if(readkey(strsql,&row1,&cur1) != 1)
   {
    beep();
    printw("Не нашли шапку документа !!! %s %s %s\n",row[4],row[0],row[2]);
    OSTANOV();
    continue;
   }
  pnds=atof(row1[2]);  
  kod_op.new_plus(row1[1]);
  kod_kontr.new_plus(row1[0]);

  if(proverka(kodoz.ravno(),row1[0],0,0) != 0)
   continue;  

  if(proverka(kopz.ravno(),row1[1],0,0) != 0)
   continue;  

  if(grupk.ravno()[0] != '\0')
   {
    if(SRAV(kod_kontr.ravno(),"00-",1) == 0) 
      sprintf(strsql,"select grup from Kontragent where kodkon='%s'","00");
    else
      sprintf(strsql,"select grup from Kontragent where kodkon='%s'",kod_kontr.ravno());
    if(readkey(strsql,&row1,&cur1) == 1)
     {
      if(proverka(grupk.ravno(),row1[0],0,0) != 0)
       continue;
    
     }
   }

  /*узнаём код материалла*/
  kod_mat.new_plus("");
  sprintf(strsql,"select kodm from Kart where sklad=%s and nomk=%s",row[0],row[1]);
  if(readkey(strsql,&row1,&cur1) == 1)
   kod_mat.new_plus(row1[0]);

  /*Читаем цену материалла*/
  cena=0.;
  sprintf(strsql,"select cena from Dokummat1 where datd='%s' and sklad=%s and nomd='%s' and kodm=%d and nomkar=%s",
  row[4],row[0],row[2],kod_mat.ravno_atoi(),row[1]);
  if(readkey(strsql,&row1,&cur1) == 1)
   cena=atof(row1[0]);
   
  /*Читаем метку НДС*/
  metka_nds=0;
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%s and nomd='%s' and nomerz=11",
  atoi(row[4]),row[0],row[2]);
  if(readkey(strsql,&row1,&cur1) ==  1)
   metka_nds=atoi(row1[0]);

  naim_kontr.new_plus("");
  /*узнаём наименование контрагента*/
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kod_kontr.ravno());
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_kontr.new_plus(row1[0]);
  
  /*узнаём наименование материалла*/
  naim_mat.new_plus("");
  sprintf(strsql,"select naimat from Material where kodm=%d",kod_mat.ravno_atoi());
  if(readkey(strsql,&row1,&cur1) == 1)
   naim_mat.new_plus(row1[0]);

  /*узнаём единицу измерени*/
  ei.new_plus("");
  sprintf(strsql,"select ei from Dokummat1 where datd='%s' and sklad=%s and nomd='%s' and nomkar=%s and kodm=%d",
  row[4],row[0],row[2],row[1],kod_mat.ravno_atoi());
  if(readkey(strsql,&row1,&cur1) == 1)
   ei.new_plus(row1[0]);
          
  memset(metka_dok,'\0',sizeof(metka_dok));
  if(atoi(row[5]) == 1)
   metka_dok[0]='+';
  else     
   metka_dok[0]='-';

  kolih=atof(row[6]);
  if(metka_nds == 0)
    suma_s_nds=(cena*kolih)+(cena*kolih*pnds/100.);
  else
    suma_s_nds=cena*kolih;

  suma_s_nds=okrug(suma_s_nds,0.01);
  itogo+=suma_s_nds;
  rsdat(&d1,&m1,&g1,row[3],2);
  rsdat(&d2,&m2,&g2,row[4],2);

  sprintf(strsql,"%s %02d.%02d.%04d %-*s %02d.%02d.%04d %-*s %-8s %-*.*s %-*s %10.10g %10.10g %10.2f %s %.*s",
  metka_dok,d1,m1,g1,
  iceb_tu_kolbait(10,row[2]),row[2],
  d2,m2,g2,
  iceb_tu_kolbait(7,kod_op.ravno()),kod_op.ravno(),
  kod_mat.ravno(),
  iceb_tu_kolbait(22,naim_mat.ravno()),iceb_tu_kolbait(22,naim_mat.ravno()),naim_mat.ravno(),
  iceb_tu_kolbait(7,ei.ravno()),ei.ravno(),
  kolih,cena,suma_s_nds,kod_kontr.ravno(),
  iceb_tu_kolbait(15,naim_kontr.ravno()),naim_kontr.ravno());

  printw("%-.*s\n",iceb_tu_kolbait(COLS-1,strsql),strsql);

  fprintf(ff,"%s %02d.%02d.%04d %-*s %02d.%02d.%04d %-*s %-8s %-*.*s %-*s %10.10g %10.10g %10.2f %s %.*s\n",
  metka_dok,d1,m1,g1,
  iceb_tu_kolbait(10,row[2]),row[2],
  d2,m2,g2,
  iceb_tu_kolbait(7,kod_op.ravno()),kod_op.ravno(),
  kod_mat.ravno(),
  iceb_tu_kolbait(23,naim_mat.ravno()),iceb_tu_kolbait(23,naim_mat.ravno()),naim_mat.ravno(),
  iceb_tu_kolbait(7,ei.ravno()),ei.ravno(),
  kolih,cena,suma_s_nds,
  kod_kontr.ravno(),
  iceb_tu_kolbait(15,naim_kontr.ravno()),naim_kontr.ravno());

  itogo_kol+=kolih;
 }

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s %10.2f %10s %10.2f\n",iceb_tu_kolbait(82,gettext("Итого")),gettext("Итого"),itogo_kol,"",itogo);
printw("%s: %10.2f %10.2f\n",gettext("Итого"),itogo_kol,itogo);


podpis(ff);

fclose(ff);



printw_vr(tmm);

OSTANOV();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Список документов по датам подтверждения"));
iceb_t_ustpeh(imaf,3);

return(0);

}
