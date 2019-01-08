/* $Id: rozsvod.c,v 5.35 2013/09/26 09:43:40 sasa Exp $ */
/*15.07.2015	02.02.1999	Белых А.И.	rozsvod.c
Получения свода продажи товаров
*/
#include	<errno.h>
#include	"buhg.h"

void  rassvoduz(masiv_din_int *,masiv_din_double *,FILE *);

extern double	okrg1;  /*Округление суммы*/

void rozsvod(short dp,short mp,short gp,int kassa,const char *fam)
{
struct  tm      *bf;
time_t		vrem;
SQL_str         row,row1;
long		kolstr;
char		strsql[512];
char		imaf[64];
FILE		*ff;
int		i;
int		kodm;
double		it;
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
short		dn,mn,gn;
short		dk,mk,gk;
int		kom=0;

if(dat1.ravno()[0] == '\0')
 {
  sprintf(strsql,"%d.%d.%d",dp,mp,gp);
  dat1.new_plus(strsql);
  dat2.new_plus(strsql);
 }

VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Свод розничной торговли за период"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..(д.м.г).."));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11); 

naz:;

clear();

helstr(LINES-1,0,\
//"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   prosf(strsql);
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


if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
 goto naz;

clear();
printw("%s %d.%d.%d => %d.%d.%d\n",gettext("Свод розничной торговли за период"),
dn,mn,gn,dk,mk,gk);

GDITE();


SQLCURSOR cur;

if(kassa != 0)
  sprintf(strsql,"select metka,kodm,kolih,cena from Roznica \
where datrp >= '%d-%02d-%02d' and datrp <= '%d-%02d-%02d' and kassa=%d order by nomer asc",
gn,mn,dn,gk,mk,dk,kassa);

if(kassa == 0)
  sprintf(strsql,"select metka,kodm,kolih,cena from Roznica \
where datrp >= '%d-%02d-%02d' and datrp <= '%d-%02d-%02d' order by kassa,nomer asc",
gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

SPISOK KODCENA(0);
masiv_din_double SUMA(0);
masiv_din_double KOLIH(0);

masiv_din_int KODIZDEL(0);
masiv_din_double KOLIHIZ(0);

SPISOK KODCENAU(0);
masiv_din_double SUMAU(0);
masiv_din_double KOLIHU(0);

double cena=0.;
int    nomer=0;
double suma=0.;
double kolih=0;
SQLCURSOR cur1;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  kodm=atoi(row[1]);
  
  kolih=atof(row[2]);
  
  cena=atof(row[3]);
  cena=okrug(cena,0.01);

  suma=kolih*cena;
  suma=okrug(suma,0.01);
  if(row[0][0] == '1')
   {
    //Проверяем не является ли изделием
    sprintf(strsql,"select kodi from Specif where kodi=%d",kodm);
    if(sql_readkey(&bd,strsql,&row1,&cur1) > 0)
     {
//      printw("Изделие\n");
      if((nomer=KODIZDEL.find(kodm)) < 0)
        KODIZDEL.plus(kodm,nomer);
      KOLIHIZ.plus(kolih,nomer);
     }

    sprintf(strsql,"%d|%.10g",kodm,cena);
    if((nomer=KODCENA.find(strsql)) < 0)
      KODCENA.plus(strsql);

    SUMA.plus(suma,nomer);
    KOLIH.plus(kolih,nomer);
   }
  else
   {
    sprintf(strsql,"%d|%.10g",kodm,cena);
    if((nomer=KODCENAU.find(strsql)) < 0)
      KODCENAU.plus(strsql);
    SUMAU.plus(suma,nomer);
    KOLIHU.plus(kolih,nomer);
   }
 }


printw(gettext("Распечатываем\n"));
refresh();

sprintf(imaf,"svd%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

fprintf(ff,"%s\n",iceb_t_get_pnk("00",0));
fprintf(ff,"%s %d.%d.%d => %d.%d.%d\n",gettext("Свод розничной торговли за период"),
dn,mn,gn,dk,mk,gk);

if(kassa != 0)
  fprintf(ff,"%s%d\n",gettext("По рабочему месту N"),kassa);
else
  fprintf(ff,gettext("По всем рабочим местам.\n"));

time(&vrem);
bf=localtime(&vrem);
fprintf(ff,"%s %d.%d.%d  %d:%d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,bf->tm_hour,bf->tm_min);

fprintf(ff,"%s\n",gettext("Материалы"));

fprintf(ff,"\
---------------------------------------------------------------------\n");
fprintf(ff,gettext("К.мт|  Наименование товара         |Количество|  Цена    |  Сумма   |\n"));
fprintf(ff,"\
---------------------------------------------------------------------\n");
it=0.;
int kolmatcen=KODCENA.kolih();
class iceb_tu_str kodmat("");
class iceb_tu_str cenach("");
class iceb_tu_str naim("");
class iceb_tu_str metkaiz("");
for(i=0; i < kolmatcen; i++)
 {
  polen(KODCENA.ravno(i),&kodmat,1,'|');
  polen(KODCENA.ravno(i),&cenach,2,'|');

  /*Читаем наименование материалла*/
  sprintf(strsql,"select naimat from Material where kodm=%s",
  kodmat.ravno());
  if(sql_readkey(&bd,strsql,&row,&cur1) == 1)
    naim.new_plus(row[0]);
  else
    naim.new_plus("");
  if(KODIZDEL.find(kodmat.ravno_atoi()) >= 0)
    metkaiz.new_plus(gettext("Изделие"));
  else
    metkaiz.new_plus("");
  it+=SUMA.ravno(i);

  fprintf(ff,"%4s %-*.*s %10.10g %10s %10.2f %s\n",
  kodmat.ravno(),
  iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
  KOLIH.ravno(i),cenach.ravno(),SUMA.ravno(i),metkaiz.ravno());
 }
fprintf(ff,"%*s: %10.2f\n",iceb_tu_kolbait(56,gettext("Итого")),gettext("Итого"),it);


if((kolmatcen=KODCENAU.kolih()) > 0)
 {
  fprintf(ff,"\n%s\n",gettext("Услуги"));

  fprintf(ff,"\
---------------------------------------------------------------------\n");
  fprintf(ff,gettext("К.ус|  Наименование услуги         |Количество|  Цена    |  Сумма   |\n"));
  fprintf(ff,"\
---------------------------------------------------------------------\n");
  it=0.;

  for(i=0; i < kolmatcen; i++)
   {
    polen(KODCENAU.ravno(i),&kodmat,1,'|');
    polen(KODCENAU.ravno(i),&cenach,2,'|');

    /*Читаем наименование услуги*/
    sprintf(strsql,"select naius from Uslugi where kodus=%s",
    kodmat.ravno());
    if(sql_readkey(&bd,strsql,&row,&cur1) == 1)
      naim.new_plus(row[0]);
    else
      naim.new_plus("");
    it+=SUMAU.ravno(i);
    fprintf(ff,"%4s %-*.*s %10.10g %10s %10.2f\n",
    kodmat.ravno(),
  iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
    KOLIHU.ravno(i),cenach.ravno(),SUMAU.ravno(i));
   }
  fprintf(ff,"%*s: %10.2f\n",iceb_tu_kolbait(56,gettext("Итого")),gettext("Итого"),it);
 }

//Обрабатываем изделия
rassvoduz(&KODIZDEL,&KOLIHIZ,ff);


podpis(ff);

fclose(ff);        

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка всех продаж"));
iceb_t_ustpeh(imaf,1);
iceb_t_rabfil(&oth,"");

}
/********************************************/
/*Распечатка спецификации входящих узлов    */
/********************************************/

void  rassvoduz(masiv_din_int *KODIZDEL,masiv_din_double *KOLIHIZ,FILE *ff)
{
//printw("rassvoduz-Разузлование изделий\n");
//refresh();
int koliz=KODIZDEL->kolih();

if(koliz == 0)
 return;
//printw("koliz=%d\n",koliz);


masiv_din_int	kodditog(0); /*Массив кодов деталей*/
masiv_din_double kolditog(0); /*Массив количества деталей*/
masiv_din_int   metka_zap_det; //метки 0-материал 1-услуга
int kolndet=0;
int nomer=0;
int nomdet=0,nomdet1=0;
int koddet=0;
double kolizdel=0.;
double koldet=0.;
for(nomer=0; nomer < koliz; nomer ++)
 {
  kolizdel=KOLIHIZ->ravno(nomer);

  class iceb_t_razuz_data data;
  /*Определяем входящие узлы*/


  /*Делаем расчёт входящих деталей*/
  if((kolndet=iceb_t_razuz_kod(&data)) == 0)
   {
    continue;  
   }
  //Записываем в итоговые массивы
  int kolih_kod_ei=data.kod_det_ei.kolih();    
  for(nomdet=0; nomdet < kolih_kod_ei; nomdet++)
   {
    if(data.metka_mu.ravno(nomdet) == 1)
     continue;
    polen(data.kod_det_ei.ravno(nomdet),&koddet,1,'|');
     
 //   koddet=kodd.ravno(nomdet);
    koldet=data.kolih_det_ei.ravno(nomdet);

    if((nomdet1=kodditog.find(koddet)) < 0)
       kodditog.plus(koddet,nomdet1);
    kolditog.plus(koldet*kolizdel,nomdet1);

   }

 }
//printw("Распечатываем\n");

class iceb_tu_str naim("");
double cena=0.,suma=0.;
char strsql[512];
SQL_str row;

fprintf(ff,"\n%s:\n",gettext("Материалы (детали) входящие в изделия"));
kolndet=kodditog.kolih();
//printw("kolndet=%d\n",kolndet);
SQLCURSOR cur;
for(nomer=0 ; nomer < kolndet; nomer++)
 {

  koddet=kodditog.ravno(nomer);
  koldet=kolditog.ravno(nomer);

  cena=suma=0.;
  /*Читаем наименование материалла*/
  sprintf(strsql,"select naimat,cenapr from Material where kodm=%d",koddet);

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   {
    naim.new_plus(row[0]);
    cena=atof(row[1]);
   }
  else
   {
    naim.new_plus("");
    cena=0.;
   }    
  suma=cena*koldet;
  suma=okrug(suma,okrg1);
  
  fprintf(ff,"%4d %-*.*s %10.10g %10s %10.2f\n",
  koddet,
  iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
  koldet,prcn(cena),suma);

 }

}

