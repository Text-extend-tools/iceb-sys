/*$Id: dvusl_mu.c,v 5.16 2013/05/17 14:56:01 sasa Exp $*/
/*01.12.2016	17.08.2006	Белых А.И.	dvusl_mu.c
Движение услуг в материальном учёте
*/
#include <errno.h>
#include "buhg.h"

void dvusl_mu_p(const char *sklad,const char *kontr,const char *grupa,const char *kodop,const char *pr_ras,const char *nomdok,const char *kodusl,FILE *ff);

extern double	okrcn;  /*Округление цены*/
   
int dvusl_mu(class spis_oth *oth)
{
char strsql[512];
static class iceb_tu_str dat1(""),dat2("");
static class iceb_tu_str sklad("");
static class iceb_tu_str kontr("");
static class iceb_tu_str grupa("");
static class iceb_tu_str kodop("");
static class iceb_tu_str pr_ras("");
static class iceb_tu_str nomdok("");
static class iceb_tu_str kodusl("");
class iceb_tu_str naik("");
int kom=0,kom1=0;
int kodgr,i_gr;

VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Распечатать движение услуг"));


VV.VVOD_SPISOK_add_MD(gettext("Дата начала.........."));  //0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..........."));  //1
VV.VVOD_SPISOK_add_MD(gettext("Склад.........(,,)..."));  //2
VV.VVOD_SPISOK_add_MD(gettext("Код контрагента.(,,)."));  //3
VV.VVOD_SPISOK_add_MD(gettext("Группа........(,,)..."));  //4
VV.VVOD_SPISOK_add_MD(gettext("Код операции..(,,)..."));  //5
VV.VVOD_SPISOK_add_MD(gettext("Приход/расход.(+/-).."));  //6
VV.VVOD_SPISOK_add_MD(gettext("Номер документа.(,,).")); //7
VV.VVOD_SPISOK_add_MD(gettext("Код услуги......(,,).")); //8

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);      //0
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);      //1
VV.VVOD_SPISOK_add_data(sklad.ravno(),128);    //2
VV.VVOD_SPISOK_add_data(kontr.ravno(),128);    //3
VV.VVOD_SPISOK_add_data(grupa.ravno(),128);    //4
VV.VVOD_SPISOK_add_data(kodop.ravno(),128);    //5
VV.VVOD_SPISOK_add_data(pr_ras.ravno(),2);  //6
VV.VVOD_SPISOK_add_data(nomdok.ravno(),128);  //7
VV.VVOD_SPISOK_add_data(kodusl.ravno(),128);  //8

naz:;

clear();
 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F5",gettext("настройка"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
sklad.new_plus(VV.data_ravno(2));
kontr.new_plus(VV.data_ravno(3));
grupa.new_plus(VV.data_ravno(4));
kodop.new_plus(VV.data_ravno(5));
pr_ras.new_plus(VV.data_ravno(6));
nomdok.new_plus(VV.data_ravno(7));
kodusl.new_plus(VV.data_ravno(8));
class iceb_tu_str kods("");
class iceb_tu_str naims("");
switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("matu5_14.txt");
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;

  case FK3:

    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка складов"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций приходов"));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка операций расходов"));//4
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка услуг"));//5
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//6

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
    
    
    switch (kom1)
     {
      case 6 :
      case -1:
       goto naz;

      case 0 :
        if(dirsklad(1,&kods,&naims) == 0)
          VV.data_z_plus(2,kods.ravno());
        goto naz;

      case 1 :
        vibrek("Kontragent",&kontr,&naims);
        VV.VVOD_SPISOK_zapis_data(3,kontr.ravno());
        goto naz;

      case 2 :
        vibrek("Uslgrup",&grupa,&naims);
        VV.VVOD_SPISOK_zapis_data(4,grupa.ravno());
        goto naz;

      case 3 :
       if(dirprihod(1,&kodop,&naims) == 0)
         VV.data_z_plus(5,kodop.ravno());
       goto naz;

      case 4 :
        if(dirrashod(1,&kodop,&naims) == 0)
          VV.data_z_plus(5,kodop.ravno());
        goto naz;

      case 5:
        kodgr=i_gr=0;
        naik.new_plus("");
        if(dirusl(&kodgr,&i_gr,&naik,1,"") == 0)
         {
          VV.data_z_plus(8,i_gr);
         }
        goto naz;
        

     }
    goto naz;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

GDITE();

short dn,mn,gn;
short dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
 goto naz;
int kolstr=0;
int kolstr1=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;

sprintf(strsql,"select * from Dokummat3 where datd >= '%04d-%02d-%02d' and \
datd <= '%04d-%02d-%02d'",gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

FILE *ff;
char imaf[64];
sprintf(imaf,"dum%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
iceb_t_zagolov(gettext("Расчёт движения услуг по документам \"Материального учёта\""),dn,mn,gn,dk,mk,gk,ff);

dvusl_mu_p(sklad.ravno(),kontr.ravno(),grupa.ravno(),kodop.ravno(),pr_ras.ravno(),nomdok.ravno(),kodusl.ravno(),ff);

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
+/-|Дата док. |Номер док.|Склад|Ед.изм.|Код.ус.|Количество| Сумма    |Наименование услуги\n"));
/****
1234567890 1234567890 12345 1234567 1234567 1234567890 1234567890
*****/

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------\n");

class SPISOK kodusl_ei;
class masiv_din_double m_kolih;
class masiv_din_double m_suma;

short dd,md,gd;
double kolih=0.;
double suma=0.;
double cena=0.;
char prihod_rashod[2];
int tipz=0;
double itogo_kolih=0.;
double itogo_suma=0.;
int nomer=0;
class iceb_tu_str naiusl("");
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(kodusl.ravno(),row[10],0,0) != 0)
    continue;
  
  

  //читаем шапку документа
  sprintf(strsql,"select * from Dokummat where datd='%s' and sklad=%s and nomd='%s'",
  row[0],row[1],row[2]);
  if(readkey(strsql,&row1,&cur1) != 1)
   {
    iceb_t_soob(gettext("Не найдена шапка документа !"));
    continue;    
   }
  
  if(proverka(sklad.ravno(),row1[2],0,0) != 0)
    continue;
  if(proverka(kodop.ravno(),row1[6],0,0) != 0)
    continue;
  if(proverka(kontr.ravno(),row1[3],0,0) != 0)
    continue;
  if(proverka(nomdok.ravno(),row1[4],0,0) != 0)
    continue;


  naiusl.new_plus("");
  if(atoi(row[10]) != 0)
   {
    sprintf(strsql,"select kodgr,naius from Uslugi where kodus=%s",row[10]);
    if(readkey(strsql,&row1,&cur1) == 1)
     naiusl.new_plus(row1[1]);
    if(proverka(grupa.ravno(),row1[0],0,0) != 0)
     continue;
   }  
  if(naiusl.ravno()[0] == '\0')
   naiusl.new_plus(row[7]);  
  else
   {
    naiusl.plus(" ",row[7]);
   }

  tipz=atoi(row1[0]);
  if(pr_ras.ravno()[0] == '+' && tipz == 2)
   continue;
  if(pr_ras.ravno()[0] == '-' && tipz == 1)
   continue;
  
  if(tipz == 1)
   strcpy(prihod_rashod,"+");  
  if(tipz == 2)
   strcpy(prihod_rashod,"-");  

  kolih=atof(row[4]);
  cena=atof(row[5]);
  cena=okrug(cena,okrcn);
  if(kolih > 0)
    suma=cena*kolih;
  else
    suma=cena;

  itogo_kolih+=kolih;
  itogo_suma+=suma;
  
  rsdat(&dd,&md,&gd,row[0],2);

  sprintf(strsql,"%s|%s",row[10],row[3]);
  if((nomer=kodusl_ei.find_r(strsql)) < 0)
    kodusl_ei.plus(strsql);
  m_kolih.plus(kolih,nomer);
  m_suma.plus(suma,nomer);
  
  fprintf(ff,"%-3s %02d.%02d.%04d %-*s %-5s %-*s %-7s %10.10g %10.2f %s\n",
  prihod_rashod,dd,md,gd,
  iceb_tu_kolbait(10,row[2]),row[2],
  row[1],
  iceb_tu_kolbait(7,row[3]),row[3],
  row[10],kolih,suma,naiusl.ravno());
  
 }

GDITE();
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.10g %10.2f\n",
iceb_tu_kolbait(47,gettext("Итого")),gettext("Итого"),itogo_kolih,itogo_suma);

podpis(ff);
fclose(ff);


char imaf_itog[50];
sprintf(imaf_itog,"dumi%d.lst",getpid());
if((ff = fopen(imaf_itog,"w")) == NULL)
 {
  error_op_nfil(imaf_itog,errno,"");
  return(1);
 }
iceb_t_zagolov(gettext("Расчёт движения услуг по документам \"Материального учёта\""),dn,mn,gn,dk,mk,gk,ff);

dvusl_mu_p(sklad.ravno(),kontr.ravno(),grupa.ravno(),kodop.ravno(),pr_ras.ravno(),nomdok.ravno(),kodusl.ravno(),ff);

fprintf(ff,"\
------------------------------------------------------------------\n");

fprintf(ff,gettext("\
 Код |Ед.из.| Наименование услуги          |Количество| Сумма    |\n"));

fprintf(ff,"\
------------------------------------------------------------------\n");

class iceb_tu_str kod_usl("");
class iceb_tu_str ei("");
itogo_kolih=itogo_suma=0.;
for(int kk=0; kk < kodusl_ei.kolih(); kk++)
 {
  polen(kodusl_ei.ravno(kk),&kod_usl,1,'|');
  polen(kodusl_ei.ravno(kk),&ei,2,'|');
  naiusl.new_plus("");
  sprintf(strsql,"select naius from Uslugi where kodus=%s",kod_usl.ravno());
  if(readkey(strsql,&row,&cur) == 1)
   naiusl.new_plus(row[0]);

  fprintf(ff,"%5s %-*s %-*.*s %10.10g %10.2f\n",
  kod_usl.ravno(),
  iceb_tu_kolbait(6,ei.ravno()),ei.ravno(),
  iceb_tu_kolbait(30,naiusl.ravno()),iceb_tu_kolbait(30,naiusl.ravno()),naiusl.ravno(),

  m_kolih.ravno(kk),m_suma.ravno(kk));
  itogo_kolih+=m_kolih.ravno(kk);
  itogo_suma+=m_suma.ravno(kk);
 }

fprintf(ff,"\
------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.10g %10.2f\n",iceb_tu_kolbait(43,gettext("Итого")),gettext("Итого"),itogo_kolih,itogo_suma);

podpis(ff);
fclose(ff);

sprintf(strsql,"%s %s",imaf,imaf_itog);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Расчёт движения услуг"));
oth->spis_imaf.plus(imaf_itog);
oth->spis_naim.plus(gettext("Общие итоги"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),0);
  
iceb_t_ustpeh(oth->spis_imaf.ravno(0),0);

return(0);
}
/***************************/
/*Вывод реквизитов поиска*/
/*************************/
void dvusl_mu_p(const char *sklad,const char *kontr,const char *grupa,const char *kodop,const char *pr_ras,const char *nomdok,const char *kodusl,FILE *ff)
{
if(sklad[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Склад"),sklad);
if(kontr[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код контрагента"),kontr);
if(grupa[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Группа"),grupa);
if(kodop[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код операции"),kodop);
if(pr_ras[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Приход/расход"),pr_ras);
if(nomdok[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Номер документа"),nomdok);
if(kodusl[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код услуги"),kodusl);
 

}
