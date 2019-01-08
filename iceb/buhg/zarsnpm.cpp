/*$Id: zarsnpm.c,v 5.4 2013/09/26 09:43:46 sasa Exp $*/
/*28.03.2018	18.01.2012	Белых А.И.	zarsnpm.c
Свод начислений по месяцам
*/
#include <errno.h>
#include "buhg.h"

void zarsnpm_pz(short mn,short gn,short mk,short gk,FILE *ff);
void zarsnpm_rs(int kolih_mes,class masiv_din_double *suma,class masiv_din_double *suma_plus,FILE *ff);


extern short mmm,ggg;

int zarsnpm(class spis_oth *oth)
{
int kom1=0;
char strsql[1024];
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
static class iceb_tu_str kp("");
static class iceb_tu_str tabnom("");
static class iceb_tu_str nahis("");
static class iceb_tu_str kategor("");
int tabn=0;
if(dat1.ravno()[0] == '\0')
 {
  sprintf(strsql,"%d.%d",mmm,ggg);
  dat1.new_plus(strsql);
 }
VVOD MENU(3);
VVOD VV(0);

VV.VVOD_SPISOK_add_data(dat1.ravno(),8); 
VV.VVOD_SPISOK_add_data(dat2.ravno(),8); 
VV.VVOD_SPISOK_add_data(kp.ravno(),128); 
VV.VVOD_SPISOK_add_data(tabnom.ravno(),128); 
VV.VVOD_SPISOK_add_data(nahis.ravno(),128); 
VV.VVOD_SPISOK_add_data(3);
VV.VVOD_SPISOK_add_data(kategor.ravno(),128); 

VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт свода начислений и по месяцам"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала периода..(м.г).."));//0
VV.VVOD_SPISOK_add_MD(gettext("Дата конца периода...(м.г).."));//1
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения....(,,)..."));//2
VV.VVOD_SPISOK_add_MD(gettext("Табельный номер......(,,)..."));//3
VV.VVOD_SPISOK_add_MD(gettext("Код начисления.......(,,)..."));//4
VV.VVOD_SPISOK_add_MD(gettext("Код категории........(,,)..."));//5

naz:;

clear();



helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчёт"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

int rkka=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
kp.new_plus(VV.data_ravno(2));
tabnom.new_plus(VV.data_ravno(3));
nahis.new_plus(VV.data_ravno(4));
kategor.new_plus(VV.data_ravno(5));

class iceb_tu_str kodv("");
class iceb_tu_str naim("");
switch(rkka)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("zarp4_3_6.txt");
   clear();
   goto naz;
   
  case FK10:
  case ESC:
   return(1);
  case FK2:
  case PLU:
   break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка начислений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка табельных номеров"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка категорий"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
    naim.new_plus("");
    switch (kom1)
     {
      case 4 :
      case -1:
       goto naz;

      case 0 :
        kp.new_plus("");
        if(dirzarpodr(2,&kp,&naim) == 0)
         VV.data_z_plus(2,kp.ravno());
        goto naz;

      case 1 :
        if(dirnach(1,&nahis,&naim) == 0)
         VV.VVOD_SPISOK_zapis_data(4,nahis.ravno());
        goto naz;

      
      case 2 :
      
        if(dirtab(&tabn,&naim,0,1) == 0)
         {
          tabnom.z_plus(tabn);
          VV.VVOD_SPISOK_zapis_data(3,tabnom.ravno());
         }
        goto naz;

      case 3 :
        vibrek("Kateg",&kategor,&naim);
        VV.VVOD_SPISOK_zapis_data(5,kategor.ravno());
        goto naz;

     }  

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
    goto naz;
 }
short mn=0,gn=0;
short mk=0,gk=0;
if(rsdat1(&mn,&gn,dat1.ravno()) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата начала !"));
  goto naz;
 }

if(dat2.ravno()[0] == '\0')
 {
  mk=mn; gk=gn;
 }
else
 if(rsdat1(&mk,&gk,dat2.ravno()) != 0)
  {
   iceb_t_soob(gettext("Не верно введена дата конца !"));
   goto naz;
  }

int kolih_mes=period(1,mn,gn,1,mk,gk,1);

class masiv_din_double suma_mes;
suma_mes.make_class(kolih_mes);

class masiv_din_double suma_mes_itogo_podr;
suma_mes_itogo_podr.make_class(kolih_mes);

class masiv_din_double suma_mes_itogo_all;
suma_mes_itogo_all.make_class(kolih_mes);

class SPISOK mesac;
short d,m,g;
d=1;
m=mn;
g=gn;
for(int nom=0; nom < kolih_mes; nom++)
 {
  sprintf(strsql,"%d.%d",m,g);
  mesac.plus(strsql);
  dpm(&d,&m,&g,3);  
 }
SQL_str row,row1,row2;
class SQLCURSOR cur,cur1,cur2;
int kolstr=0;

sprintf(strsql,"select tabn,fio,podr,kateg from Kartb order by podr asc,kateg asc,fio asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной карточки !"));
  return(1);
 }
FILE *ff;
char imaf[64];
sprintf(imaf,"spm%d.lst",getpid());
if((ff=fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }
iceb_t_zagolov(gettext("Свод начислений по месяцам"),1,mn,gn,1,mk,gk,ff);
if(tabnom.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Табельный номер"),tabnom.ravno());
if(kp.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код подразделения"),kp.ravno());
if(kategor.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код категории"),kategor.ravno());
if(nahis.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код начисления"),nahis.ravno());
 
class iceb_tu_str liniq("--------------------------------------");
for(int nom=0; nom <= kolih_mes; nom++)
 liniq.plus("-----------");

int kolstr1=0;
int kolstr2=0;
double suma=0.;
int podr=0,podr1=0;
int kateg=0,kateg1=0;
int nomer=0;
class iceb_tu_str naim_podr("");
class iceb_tu_str naim_kateg("");
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(tabnom.ravno(),row[0],0,0) != 0)
   continue;
  if(proverka(kp.ravno(),row[2],0,0) != 0)
   continue;
  if(proverka(kategor.ravno(),row[3],0,0) != 0)
   continue;

  sprintf(strsql,"select datz,knah,suma from Zarp where tabn=%s and datz >= '%04d-%02d-1' and datz <= '%04d-%02d-31' and prn='1' and suma <> 0.",row[0],gn,mn,gk,mk);
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return(1);
   }
  if(kolstr2 == 0)
   continue;  

  podr=atoi(row[2]);
  kateg=atoi(row[3]);


  if(podr != podr1)
   {
    if(podr1 != 0)
     {
      fprintf(ff,"%s\n",liniq.ravno());
      fprintf(ff,"%*s|",iceb_tu_kolbait(37,gettext("Итого по подразделению")),gettext("Итого по подразделению"));
      zarsnpm_rs(kolih_mes,&suma_mes_itogo_podr,&suma_mes_itogo_all,ff); /*распечатываем строку*/
      suma_mes_itogo_podr.clear_class();
     }
    naim_podr.new_plus("");
    sprintf(strsql,"select naik from Podr where kod=%s",row[2]);
    if(readkey(strsql,&row2,&cur2) == 1)
     naim_podr.new_plus(row2[0]);

    fprintf(ff,"\n%s:%s %s\n",gettext("Подразделение"),row[2],naim_podr.ravno());

    zarsnpm_pz(mn,gn,mk,gk,ff); /*шапка*/

    naim_kateg.new_plus("");
    sprintf(strsql,"select naik from Kateg where kod=%s",row[3]);
    if(readkey(strsql,&row2,&cur2) == 1)
     naim_kateg.new_plus(row2[0]);    
    fprintf(ff,"%s:%s %s\n",gettext("Категория"),row[3],naim_kateg.ravno());

    podr1=podr;
    kateg1=kateg;
   }

  if(kateg1 != kateg)
   {
    naim_kateg.new_plus("");
    sprintf(strsql,"select naik from Kateg where kod=%s",row[3]);
    if(readkey(strsql,&row2,&cur2) == 1)
     naim_kateg.new_plus(row2[0]);    
    fprintf(ff,"%s:%s %s\n",gettext("Категория"),row[3],naim_kateg.ravno());
    kateg1=kateg;
   }




  suma_mes.clear_class();
  
  while(cur1.read_cursor(&row1) != 0)
   {
    if(proverka(nahis.ravno(),row1[1],0,0) != 0)
     continue;

    suma=atof(row1[2]);

    rsdat(&d,&m,&g,row1[0],2);

    sprintf(strsql,"%d.%d",m,g);

    if((nomer=mesac.find(strsql)) < 0)
     {
      sprintf(strsql,"%s %d.%d",gettext("Не найдена дата"),m,g);
      iceb_t_soob(strsql);
      continue;
     }
    suma_mes.plus(suma,nomer);
   }
  fprintf(ff,"%6s|%-*.*s|",
  row[0],
  iceb_tu_kolbait(30,row[1]),
  iceb_tu_kolbait(30,row[1]),
  row[1]);  

  zarsnpm_rs(kolih_mes,&suma_mes,&suma_mes_itogo_podr,ff); /*распечатываем строку*/

     
 }
fprintf(ff,"%s\n",liniq.ravno());
fprintf(ff,"%*s|",iceb_tu_kolbait(37,gettext("Итого по подразделению")),gettext("Итого по подразделению"));
zarsnpm_rs(kolih_mes,&suma_mes_itogo_podr,&suma_mes_itogo_all,ff); /*распечатываем строку*/

fprintf(ff,"%s\n",liniq.ravno());
fprintf(ff,"%*s|",iceb_tu_kolbait(37,gettext("Общий итог")),gettext("Общий итог"));
zarsnpm_rs(kolih_mes,&suma_mes_itogo_all,NULL,ff); /*распечатываем строку*/

podpis(ff);

fclose(ff);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Свод начислений по месяцам"));
iceb_t_ustpeh(imaf,0);

return(0);

}

/**************************************/
/*Печать заголовка*/
/*************************************/
void zarsnpm_pz(short mn,short gn,short mk,short gk,FILE *ff)
{

int kolih_mes=period(1,mn,gn,1,mk,gk,1);
class iceb_tu_str liniq("--------------------------------------");
for(int nom=0; nom <= kolih_mes; nom++)
 liniq.plus("-----------");

fprintf(ff,"%s\n",liniq.ravno());

fprintf(ff,gettext("  Т/н |          Фамилия             |"));
short d=1,m=mn,g=gn;
for(int nom=0; nom < kolih_mes; nom++)
 {
  fprintf(ff," %02d.%04d  |",m,g);
  dpm(&d,&m,&g,3);
 }
fprintf(ff,"%-*.*s|\n",
iceb_tu_kolbait(10,gettext("Итого")),
iceb_tu_kolbait(10,gettext("Итого")),
gettext("Итого"));

fprintf(ff,"%s\n",liniq.ravno());

/*************
123456|123456789012345678901234567890|
*************/



}
/********************************************/
/*распечатка строки*/
/************************************/
void zarsnpm_rs(int kolih_mes,class masiv_din_double *suma,class masiv_din_double *suma_plus,FILE *ff)
{
for(int nom=0; nom < kolih_mes; nom++)
 {
  fprintf(ff,"%10.2f|",suma->ravno(nom));
  if(suma_plus != NULL)
    suma_plus->plus(suma->ravno(nom),nom);
 }
fprintf(ff,"%10.2f|\n",suma->suma());
}
