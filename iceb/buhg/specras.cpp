/*$Id: specras.c,v 5.32 2013/09/26 09:43:40 sasa Exp $*/
/*09.03.2017	19.01.2000	Белых А.И.	specras.c
Расчёт количества деталей на определенное количество 
изделий
*/
#include        <errno.h>
#include        "buhg.h"

void specras_hm(FILE *ff);
void specras_hspm(FILE *ff);

extern double	okrg1;  /*Округление суммы*/

void		specras(int kodiz) //Код изделия
{
double		koliz;
int		i;

SQL_str         row,row1;
char		strsql[1024];
FILE		*ff;
char		imaf[64];
int		kolnuz=0; /*Количество наименований узлов*/
int		kolndet; /*Количество наименований деталей*/
class iceb_tu_str naim("");
int		sss;
int		kolstr1;
class iceb_tu_str ei("");
int		kodm=0;
class iceb_tu_str vstr("");
class SPISOK ssm; /*свод списания по материалам "код материала|единица измерения"*/
class masiv_din_double ssm_kol; /*количество материала*/

int kolstr=0;

class VVOD VVOD1(2);

helstr(LINES-1,0,"F10",gettext("выход"),NULL);
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите количество изделий"));
     
     
if((i=vvod1(&vstr,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
   return;
if(i == ESC)
  return;
  
koliz=vstr.ravno_atof();

if(koliz == 0)
 {
  return;
 }

GDITE();
class iceb_t_razuz_data data;
data.kod_izd=kodiz;

if((kolndet=iceb_t_razuz_kod(&data)) == 0)
 {
  sprintf(strsql,"%s %d !",gettext("Не найдено ни одной детали для изделия"),kodiz);
  iceb_t_soob(strsql);
  return;  
 }

clear();
GDITE();
SQLCURSOR cur,cur1;
sprintf(strsql,"select naimat from Material where kodm=%d",kodiz);
if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден код материалла"),kodiz);
  iceb_t_soob(strsql);
 }  
else
 naim.new_plus(row1[0]);

printw("%s %d %s\n%s:%.10g\n",
gettext("Расчёт входимости деталей в изделие"),
kodiz,naim.ravno(),
gettext("Количество изделий"),
koliz);

char imaf_mat[64];
sprintf(imaf_mat,"specmatr%d.lst",kodiz);
class iceb_fopen film;
if(film.start(imaf_mat,"w") != 0)
 return;




sprintf(imaf,"specr%d.lst",kodiz);
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

short ddd=0;
short mmm=0;
short ggg=0;
poltekdat(&ddd,&mmm,&ggg);

sprintf(strsql,"%s %d %s",
gettext("Расчёт входимости деталей в изделие"),
kodiz,naim.ravno());

iceb_t_zagolov(strsql,ff);

sprintf(strsql,"%s %d %s",
gettext("Расчёт списания материалов на изделие"),
kodiz,naim.ravno());

iceb_t_zagolov(strsql,film.ff);

fprintf(ff,"%s:%.10g\n",gettext("Количество изделий"),koliz);
fprintf(film.ff,"%s:%.10g\n",gettext("Количество изделий"),koliz);

specras_hm(film.ff);

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Koд  |   Наименование   соcтавляющей          |Ед.из|Количество| Цена   |  Сумма   |  Остаток |  Разница |\n"));
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------\n");

printw("\
--------------------------------------------------------------------------------\n");
printw(gettext("\
 Koд  |    Наименование    |Количеств| Цена   |  Сумма   |  Остаток |  Разница |\n"));
//   123456789012345678901234567890 12345678 1234567890 1234567890
printw("\
--------------------------------------------------------------------------------\n");

double koliho=0.;
double kolihr=0.;
double cena=0.;
double suma=0.;
double isuma=0.;
double ostatok=0.;
int metka_pz=0;
kolstr1=0;
int nomer_v_sp=0;

int kolih_kod_ei=data.kod_det_ei.kolih();

for(sss=0; sss < kolih_kod_ei; sss++)
 {
  strzag(LINES-1,0,kolndet,++kolstr1);

  polen(data.kod_det_ei.ravno(sss),&kodm,1,'|');
  polen(data.kod_det_ei.ravno(sss),&ei,2,'|');
  cena=suma=0.;
    
  if(data.metka_mu.ravno(sss) == 0)
    sprintf(strsql,"select naimat,cenapr from Material where kodm=%d",kodm);

  if(data.metka_mu.ravno(sss) == 1)
    sprintf(strsql,"select naius,cena from Uslugi where kodus=%d",kodm);

  if(readkey(strsql,&row1,&cur1) != 1)
   {
    beep();
    printw("%s-%s\n",__FUNCTION__,strsql);
    printw("%s %d !\n",gettext("Не найден код записи"),kodm);
    OSTANOV();
    naim.new_plus("");
   }  
  else
   {
    naim.new_plus(row1[0]);
    if(row1[1] != NULL)
     cena=atof(row1[1]);
    if(cena != 0.)
     cena=okrug(cena,okrg1);
   }
//  printw("sss=%d kodm=%d kolih=%f\n",sss,kodm,kold.ravno(sss));
  
  suma=koliho=kolihr=0.;
  kolihr=data.kolih_det_ei.ravno(sss)*koliz;

  suma=cena*kolihr;
  if(suma != 0.)
   suma=okrug(suma,okrg1); 
  isuma+=suma;
  ostatok=0.;
  
  if(data.metka_mu.ravno(sss) == 0)
   {
    koliho=ostdok1(ddd,mmm,ggg,0,kodm);
    ostatok=koliho-kolihr;
   }
    
  printw("%-6d %-*.*s %10.10g %8.8g %10.2f %10.10g %10.10g\n",
  kodm,
  iceb_tu_kolbait(20,naim.ravno()),iceb_tu_kolbait(20,naim.ravno()),naim.ravno(),
  kolihr,
  cena,suma,koliho,ostatok);

  fprintf(ff,"%-6d %-*s %*s %10.10g %8.8g %10.2f %10.10g %10.10g\n",
  kodm,
  iceb_tu_kolbait(40,naim.ravno()),naim.ravno(),
  iceb_tu_kolbait(5,ei.ravno()),ei.ravno(),
  kolihr,cena,suma,koliho,ostatok);
  

  if(data.metka_mu.ravno(sss) == 1) /*для услуг списания материалов не может быть*/
    continue;
    
  /*списание материалов*/
  
  sprintf(strsql,"select km,kol,eim from Musnrm where kd=%d and vs=0",kodm);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,__FUNCTION__,strsql);
    continue;
   }
  
  fprintf(film.ff,"%6d %-*.*s %*s ",
  kodm,
  iceb_tu_kolbait(30,naim.ravno()),
  iceb_tu_kolbait(30,naim.ravno()),
  naim.ravno(),
  iceb_tu_kolbait(7,ei.ravno()),
  ei.ravno());

  if(kolstr == 0)
   {
    fprintf(film.ff,"%s\n",gettext("В справочнике не найдена норма списания на эту деталь"));
    continue;
   }

  metka_pz=0;  

  while(cur.read_cursor(&row) != 0)
   {
    double norma=atof(row[1]);
    double kolih_mat=norma*kolihr;
    
    class iceb_tu_str naim_mat("");
    sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[0]));
    if(readkey(strsql,&row1,&cur1) == 1)
     naim_mat.new_plus(row1[0]);
 
    if(metka_pz > 0)
     fprintf(film.ff,"%46s","");
     
    fprintf(film.ff,"%6s %-*.*s %*s %11.11g %10.10g %10.10g\n",             
    row[0],
    iceb_tu_kolbait(30,naim_mat.ravno()),
    iceb_tu_kolbait(30,naim_mat.ravno()),
    naim_mat.ravno(),
    iceb_tu_kolbait(7,row[2]),
    row[2],
    kolihr,
    norma,
    kolih_mat);
    
    metka_pz++;

    sprintf(strsql,"%s|%s",row[0],row[2]);
    if((nomer_v_sp=ssm.find(strsql)) < 0)
     ssm.plus(strsql);

    ssm_kol.plus(kolih_mat,nomer_v_sp);


   }
 }

fprintf(film.ff,"\
------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f\n",iceb_tu_kolbait(73,gettext("Итого")),gettext("Итого"),isuma);
printw("%*s:%10.2f\n",iceb_tu_kolbait(47,gettext("Итого")),gettext("Итого"),isuma);

kolnuz=data.kod_uz.kolih();


if(kolnuz != 0)
 {
  printw("\n%s:\n\
----------------------------------------------------------\n",
  gettext("Входимость узлов"));
  fprintf(ff,"\n%s:\n\
---------------------------------------------------------------------------\n",
  gettext("Входимость узлов"));  
  kolstr1=0;
  for(sss=0; sss < kolnuz; sss++)
   {


    kolstr1++;
    strzag(LINES-1,0,kolndet,kolstr1);
    for(int metka=0; metka < 2; metka++)
     {
      if(metka == 0)
       sprintf(strsql,"select naimat,ei from Material where kodm=%d",data.kod_uz.ravno(sss));
      if(metka == 1)
       sprintf(strsql,"select naius,ei from Uslugi where kodus=%d",data.kod_uz.ravno(sss));
      if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
       {
        if(metka == 1)
         {        
          beep();
          printw("%s %d !\n",gettext("Не найден код записи"),data.kod_uz.ravno(sss));
          OSTANOV();
         }
        ei.new_plus("");
        naim.new_plus("");
       }  
      else
       {
        naim.new_plus(row1[0]);
        ei.new_plus(row1[1]);
        break;
       }
     }


    printw("%-6d %-*s %10.10g\n",
    data.kod_uz.ravno(sss),
    iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
    data.kolih_uz.ravno(sss)*koliz);
    
    fprintf(ff,"%-6d %-*s %*s %10.10g\n",
    data.kod_uz.ravno(sss),
    iceb_tu_kolbait(40,naim.ravno()),naim.ravno(),
    iceb_tu_kolbait(5,ei.ravno()),ei.ravno(),
    data.kolih_uz.ravno(sss)*koliz);
    
   }

 }


podpis(ff);
fclose(ff);

podpis(film.ff);
film.end();


/*распечатываем свод списания материалов*/
char imaf_ssm[64];
sprintf(imaf_ssm,"ssm%d.lst",kodiz);
if(film.start(imaf_ssm,"w") != 0)
 return;

sprintf(strsql,"%s %d %s",
gettext("Свод списания материалов на изделие"),
kodiz,naim.ravno());

iceb_t_zagolov(strsql,film.ff);

fprintf(film.ff,"%s:%.10g\n",gettext("Количество изделий"),koliz);

specras_hspm(film.ff);

for(int nom=0; nom < ssm.kolih(); nom++)
 {
  polen(ssm.ravno(nom),&kodm,1,'|');
  polen(ssm.ravno(nom),&ei,2,'|');

  sprintf(strsql,"select naimat from Material where kodm=%d",kodm);
  if(readkey(strsql,&row1,&cur1) == 1)
   naim.new_plus(row1[0]);
  else
   naim.new_plus("");
    
  
  fprintf(film.ff,"%6d %-*.*s %*s %10.10g\n",
  kodm,
  iceb_tu_kolbait(30,naim.ravno()),
  iceb_tu_kolbait(30,naim.ravno()),
  naim.ravno(),
  iceb_tu_kolbait(7,ei.ravno()),
  ei.ravno(),
  ssm_kol.ravno(nom));

 }
fprintf(film.ff,"\
---------------------------------------------------------\n");

podpis(film.ff);
film.end();



OSTANOV();

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Расчёт входимости деталей на изделие"));

oth.spis_imaf.plus(imaf_mat);
oth.spis_naim.plus(gettext("Расчёт списания материалов на изделие"));

oth.spis_imaf.plus(imaf_ssm);
oth.spis_naim.plus(gettext("Свод списания материалов на изделие"));

for(int nom=0; nom < oth.spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth.spis_imaf.ravno(nom),3);

iceb_t_rabfil(&oth,"");

}
/*********************************************/
/*шапка по материалам*/
/**********************************************/
void specras_hm(FILE *ff)
{
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Код д.|    Наименование детали       |Ед.изм.|Код.м.|     Наименование материала   |Ед.изм.|Кол.деталей|Норма спи.|Кол.матер.|\n"));
/*
123456 123456789012345678901234567890 1234567 123456 123456789012345678901234567890 1234567 12345678901 1234567890 1234567890
*/

fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------\n");


}

/*********************************************/
/*шапка свода по материалам*/
/**********************************************/
void specras_hspm(FILE *ff)
{
fprintf(ff,"\
---------------------------------------------------------\n");
fprintf(ff,gettext("\
Код.м.|     Наименование материала   |Ед.изм.|Кол.матер.|\n"));
/*
123456 123456789012345678901234567890 1234567 123456 123456789012345678901234567890 1234567 12345678901 1234567890 1234567890
*/

fprintf(ff,"\
---------------------------------------------------------\n");


}
