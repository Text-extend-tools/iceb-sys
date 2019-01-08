/*$Id: buhpsd.c,v 5.21 2013/11/05 10:50:48 sasa Exp $*/
/*13.07.2015	03.07.2008	Белых А.И.	buhpsd.c
Проверка согласованности проводок с данными в подсистемах
*/
#include <errno.h>
#include "buhg.h"

#include "poiinpd.h"

int buhpsd_uos(short dn,short mn,short gn,short dk,short mk,short gk,const char *sheta,FILE *ff);
void buhpsd_pr(short dn,short mn,short gn,short dk,short mk,short gk,const char *shet_uh,class iceb_tu_str *naim_shet,double *deb_kre);
void buhpsd_sap(short dn,short mn,short gn,short dk,short mk,short gk,FILE *ff);
int buhpsd_mu(short dn,short mn,short gn,short dk,short mk,short gk,const char *sheta,FILE *ff);


int buhpsd(class spis_oth *oth)
{
static class iceb_tu_str dat1(""),dat2("");
static class iceb_tu_str sheta("");
char strsql[512];
static class iceb_tu_str podsist("");
int kom1=0;
VVOD MENU(3);
VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Проверка согласованности данных в подсистемах"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт..........(,,)..."));
VV.VVOD_SPISOK_add_MD(gettext("Подсистема..........."));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11); 
VV.VVOD_SPISOK_add_data(sheta.ravno(),128);
VV.VVOD_SPISOK_add_data(podsist.ravno(),16);

naz:;

clear();
 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

int kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
sheta.new_plus(VV.data_ravno(2));
podsist.new_plus(VV.data_ravno(3));

class iceb_tu_str naim("");

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("buhg5_13.txt");
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;

  case FK3:
    MENU.VVOD_delete();

    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подсистем"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 2 :
      case -1:
        break;
      
      case 0:
 
        vibrek("Plansh",&sheta,&naim);
        VV.data_plus(2,sheta.ravno());
        break;

      case 1:
        MENU.VVOD_delete();
        MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное и нажмите Enter"));

        sprintf(strsql,"%-3s %s",ICEB_MP_MATU,gettext("Материальный учет"));
        MENU.VVOD_SPISOK_add_MD(strsql);

        sprintf(strsql,"%-3s %s",ICEB_MP_UOS,gettext("Учет основных средств"));
        MENU.VVOD_SPISOK_add_MD(strsql);

        clearstr(LINES-1,0);
        
        kom1=0;
        while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
        switch (kom1)
         {
          case 5 :
          case -1:
           goto naz;

          case 0 :
             VV.data_plus(3,ICEB_MP_MATU);
             break;
          case 1 :
             VV.data_plus(3,ICEB_MP_UOS);
             break;
         }

        break;
     }
    goto naz;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
    goto naz;
 }

short dn,mn,gn;
short dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
 goto naz;

time_t tmmn;
time(&tmmn);

clear();
GDITE();

FILE *ff;
char imaf[64];
sprintf(imaf,"psd%d.lst",getpid());
if((ff=fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

iceb_t_zagolov(gettext("Проверка согласованности данных в подсистемах"),dn,mn,gn,dk,mk,gk,ff);

/*  Смотрим "Материальный учёт"  */
if(proverka(podsist.ravno(),ICEB_MP_MATU,0,0) == 0)
  buhpsd_mu(dn,mn,gn,dk,mk,gk,sheta.ravno(),ff);

/*Учёт основных средств*/
if(proverka(podsist.ravno(),ICEB_MP_UOS,0,0) == 0)
  buhpsd_uos(dn,mn,gn,dk,mk,gk,sheta.ravno(),ff);


podpis(ff);

fclose(ff);

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Протокол проверки согласованности данных в подсистемах"));

iceb_t_ustpeh(imaf,3);
printw_vr(tmmn);

OSTANOV();

return(0);
}
/************/
/*Шапка*/
/********/
void buhpsd_sap(short dn,short mn,short gn,
short dk,short mk,short gk,FILE *ff)
{
fprintf(ff,"\
---------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
    Подсиcтема      |  Cальдо на %02d.%02d.%04d   |   Оборот за период      |  Cальдо на %02d.%02d.%04d   |\n\
                    |   Дебет    |   Кредит   |   Дебет    |   Кредит   |   Дебет    |   Кредит   |\n"),
dn,mn,gn,dk,mk,gk);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------\n");
}
/*****************************/
/*Получение данных по проводкам*/
/*******************************/

void buhpsd_pr(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *shet_uh,class iceb_tu_str *naim_shet,double *deb_kre)
{
naim_shet->new_plus("");
int  saldo=0;
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
sprintf(strsql,"select nais,saldo from Plansh where ns='%s'",shet_uh);
if(readkey(strsql,&row,&cur) == 1)
 {
  naim_shet->new_plus(row[0]);
  saldo=atoi(row[1]);
 }

if(saldo != 3)
   rpshet_ss(shet_uh,dn,mn,gn,dk,mk,gk,deb_kre);
else
   rpshet_rs(shet_uh,dn,mn,gn,dk,mk,gk,deb_kre);

}
/***************************/
/*Учёт основных средств*/
/**************************/

int buhpsd_uos(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *sheta,
FILE *ff)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;

printw("\n%s:%s\n",gettext("Подсистема"),
gettext("Учёт основных средств"));
GDITE();
sprintf(strsql,"select innom from Uosin");

int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 return(0);




int kolstr1=0;
/*Переменные данные по инвентарному номеру*/
class poiinpdw_data rekin;
int podr=0;

int kodotl=0;
long innom=0;
class SPISOK shet_uh;

const int razmer_mas=4;

class masiv_din_double suma;
int nomer_sheta=0;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  innom=atol(row[0]);
  /*Смотрим на начало и на конец периода, так как может быть приход в периоде или расход*/
  if(poiin(innom,dn,mn,gn,&podr,&kodotl) != 0)
   if(poiin(innom,dk,mk,gk,&podr,&kodotl) != 0)
    continue;

  
  poiinpd(innom,mk,gk,&rekin);


  if(proverka(sheta,rekin.shetu.ravno(),1,0) != 0)
   continue;


  if((nomer_sheta=shet_uh.find(rekin.shetu.ravno())) < 0)
   {
    shet_uh.plus(rekin.shetu.ravno());
    suma.plus_str(razmer_mas);
    
    nomer_sheta=shet_uh.find(rekin.shetu.ravno()); /*определяем под каким номером записан*/

   }  

  class ostuos_data uos_ost;


  ostuos(innom,dn,mn,gn,dk,mk,gk,0,0,&uos_ost,NULL);

  for(int ii=0; ii < razmer_mas; ii++)
   {
    suma.plus(uos_ost.ostmcby[ii],nomer_sheta*razmer_mas+ii);
   }  
 }

GDITE();


fprintf(ff,"\n%s:%s\n",gettext("Подсистема"),
gettext("Учёт основных средств"));


buhpsd_sap(dn,mn,gn,dk,mk,gk,ff); /*шапка*/
double deb_kre[6];
int kolih_shet=shet_uh.kolih();
class iceb_tu_str naim_shet;
kolstr1=0;
for(nomer_sheta=0 ; nomer_sheta < kolih_shet; nomer_sheta++)
 {
  strzag(LINES-1,0,kolih_shet,++kolstr1);

  buhpsd_pr(dn,mn,gn,dk,mk,gk,shet_uh.ravno(nomer_sheta),&naim_shet,deb_kre);

  fprintf(ff,"\n%s %s\n",shet_uh.ravno(nomer_sheta),naim_shet.ravno());
  
  printw("\n%s %s\n",shet_uh.ravno(nomer_sheta),naim_shet.ravno());

  fprintf(ff,"\
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");

  fprintf(ff,"%-*.*s|%12.2f|%12s|%12.2f|%12.2f|%12.2f|%12s|\n",
  iceb_tu_kolbait(20,gettext("Учёт основных средств")),
  iceb_tu_kolbait(20,gettext("Учёт основных средств")),
  gettext("Учёт основных средств"),
  suma.ravno(nomer_sheta*razmer_mas+0),
  "",
  suma.ravno(nomer_sheta*razmer_mas+1),
  suma.ravno(nomer_sheta*razmer_mas+2),
  suma.ravno(nomer_sheta*razmer_mas+3),
  "");

  printw("%-*.*s|%10.2f|%10s|%10.2f|%10.2f|%10.2f|%10s|\n",
  iceb_tu_kolbait(10,gettext("Учёт основных средств")),
  iceb_tu_kolbait(10,gettext("Учёт основных средств")),
  gettext("Учёт основных средств"),
  suma.ravno(nomer_sheta*razmer_mas+0),
  "",
  suma.ravno(nomer_sheta*razmer_mas+1),
  suma.ravno(nomer_sheta*razmer_mas+2),
  suma.ravno(nomer_sheta*razmer_mas+3),
  "");

  fprintf(ff,"%-*.*s|%12.2f|%12.2f|%12.2f|%12.2f|%12.2f|%12.2f|\n",
  iceb_tu_kolbait(20,gettext("Главная книга")),
  iceb_tu_kolbait(20,gettext("Главная книга")),
  gettext("Главная книга"),
  deb_kre[0],
  deb_kre[1],
  deb_kre[2],
  deb_kre[3],
  deb_kre[4],
  deb_kre[5]);

  printw("%-*.*s|%10.2f|%10.2f|%10.2f|%10.2f|%10.2f|%10.2f|\n",
  iceb_tu_kolbait(10,gettext("Главная книга")),
  iceb_tu_kolbait(10,gettext("Главная книга")),
  gettext("Главная книга"),
  deb_kre[0],
  deb_kre[1],
  deb_kre[2],
  deb_kre[3],
  deb_kre[4],
  deb_kre[5]);

  fprintf(ff,"%-*.*s|%12.2f|%12.2f|%12.2f|%12.2f|%12.2f|%12.2f|\n",
  iceb_tu_kolbait(20,gettext("Разница")),
  iceb_tu_kolbait(20,gettext("Разница")),
  gettext("Разница"),
  suma.ravno(nomer_sheta*razmer_mas+0)-deb_kre[0],
  deb_kre[1],
  suma.ravno(nomer_sheta*razmer_mas+1)-deb_kre[2],
  suma.ravno(nomer_sheta*razmer_mas+2)-deb_kre[3],
  suma.ravno(nomer_sheta*razmer_mas+3)-deb_kre[4],
  deb_kre[5]);

  printw("%-*.*s|%10.2f|%10.2f|%10.2f|%10.2f|%10.2f|%10.2f|\n",
  iceb_tu_kolbait(10,gettext("Разница")),
  iceb_tu_kolbait(10,gettext("Разница")),
  gettext("Разница"),
  suma.ravno(nomer_sheta*razmer_mas+0)-deb_kre[0],
  deb_kre[1],
  suma.ravno(nomer_sheta*razmer_mas+1)-deb_kre[2],
  suma.ravno(nomer_sheta*razmer_mas+2)-deb_kre[3],
  suma.ravno(nomer_sheta*razmer_mas+3)-deb_kre[4],
  deb_kre[5]);
  
  
 
 
 }
fprintf(ff,"\
---------------------------------------------------------------------------------------------------\n");

return(0);
}

/************************/
/*Материальный учёт*/
/**********************/
int buhpsd_mu(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *sheta,
FILE *ff)
{
char strsql[512];
class SPISOK shet_uh; /*Список счетов учёта в материальном учёте*/

printw("\n%s:%s\n",gettext("Подсистема"),
gettext("Материальный учёт"));
GDITE();

sprintf(strsql,"select distinct shetu from Kart");

SQLCURSOR cur;
SQL_str row;
int kolstr=0;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не нашли ни одной записи !"));
  return(1);
 }  
int kolstr1=0;
 
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  shet_uh.plus(row[0]);
 }
GDITE();
int kolih_shet=shet_uh.kolih();
/*Создаём массив для числовых данных*/

double suma_mat[kolih_shet][4];
memset(suma_mat,'\0',sizeof(suma_mat));

sprintf(strsql,"select sklad,nomk,shetu from Kart");


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }


class ostatokt ost;
kolstr1=0;
int nomer_sheta=0; 
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(sheta,row[2],1,0) != 0)
   continue;

  if((nomer_sheta=shet_uh.find(row[2])) < 0)
   {
    printw("Не найден счёт %s в списке счетов !\n",row[2]);
    OSTANOV();
    continue;
   }
 
  ostkar(dn,mn,gn,dk,mk,gk,atoi(row[0]),atoi(row[1]),&ost);

  suma_mat[nomer_sheta][0]+=ost.ostmc[0];
  suma_mat[nomer_sheta][1]+=ost.ostmc[1];
  suma_mat[nomer_sheta][2]+=ost.ostmc[2];
  suma_mat[nomer_sheta][3]+=ost.ostmc[3];

 }
GDITE();



/*Сравниваем данные с проводками в Главной книге*/

fprintf(ff,"\n%s:%s\n",gettext("Подсистема"),
gettext("Материальный учёт"));


buhpsd_sap(dn,mn,gn,dk,mk,gk,ff); /*шапка*/

class iceb_tu_str naim_shet;
double deb_kre[6];
kolstr1=0;
for(nomer_sheta=0; nomer_sheta < kolih_shet; nomer_sheta++)
 {
  strzag(LINES-1,0,kolih_shet,++kolstr1);
  if(proverka(sheta,shet_uh.ravno(nomer_sheta),1,0) != 0)
   continue;

  buhpsd_pr(dn,mn,gn,dk,mk,gk,shet_uh.ravno(nomer_sheta),&naim_shet,deb_kre);

  fprintf(ff,"\n%s %s\n",shet_uh.ravno(nomer_sheta),naim_shet.ravno());
  printw("\n%s %s\n",shet_uh.ravno(nomer_sheta),naim_shet.ravno());

  fprintf(ff,"\
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");

  fprintf(ff,"%-*.*s|%12.2f|%12s|%12.2f|%12.2f|%12.2f|%12s|\n",
  iceb_tu_kolbait(20,gettext("Материальный учёт")),
  iceb_tu_kolbait(20,gettext("Материальный учёт")),
  gettext("Материальный учёт"),
  suma_mat[nomer_sheta][0],
  "",
  suma_mat[nomer_sheta][1],
  suma_mat[nomer_sheta][2],
  suma_mat[nomer_sheta][3],
  "");

  printw("%-*.*s|%10.2f|%10s|%10.2f|%10.2f|%10.2f|%10s|\n",
  iceb_tu_kolbait(10,gettext("Материальный учёт")),
  iceb_tu_kolbait(10,gettext("Материальный учёт")),
  gettext("Материальный учёт"),
  suma_mat[nomer_sheta][0],
  "",
  suma_mat[nomer_sheta][1],
  suma_mat[nomer_sheta][2],
  suma_mat[nomer_sheta][3],
  "");

  fprintf(ff,"%-*.*s|%12.2f|%12.2f|%12.2f|%12.2f|%12.2f|%12.2f|\n",
  iceb_tu_kolbait(20,gettext("Главная книга")),
  iceb_tu_kolbait(20,gettext("Главная книга")),
  gettext("Главная книга"),
  deb_kre[0],
  deb_kre[1],
  deb_kre[2],
  deb_kre[3],
  deb_kre[4],
  deb_kre[5]);

  printw("%-*.*s|%10.2f|%10.2f|%10.2f|%10.2f|%10.2f|%10.2f|\n",
  iceb_tu_kolbait(10,gettext("Главная книга")),
  iceb_tu_kolbait(10,gettext("Главная книга")),
  gettext("Главная книга"),
  deb_kre[0],
  deb_kre[1],
  deb_kre[2],
  deb_kre[3],
  deb_kre[4],
  deb_kre[5]);

  fprintf(ff,"%-*.*s|%12.2f|%12.2f|%12.2f|%12.2f|%12.2f|%12.2f|\n",
  iceb_tu_kolbait(20,gettext("Разница")),
  iceb_tu_kolbait(20,gettext("Разница")),
  gettext("Разница"),
  suma_mat[nomer_sheta][0]-deb_kre[0],
  deb_kre[1],
  suma_mat[nomer_sheta][1]-deb_kre[2],
  suma_mat[nomer_sheta][2]-deb_kre[3],
  suma_mat[nomer_sheta][3]-deb_kre[4],
  deb_kre[5]);

  printw("%-*.*s|%10.2f|%10.2f|%10.2f|%10.2f|%10.2f|%10.2f|\n",
  iceb_tu_kolbait(10,gettext("Разница")),
  iceb_tu_kolbait(10,gettext("Разница")),
  gettext("Разница"),
  suma_mat[nomer_sheta][0]-deb_kre[0],
  deb_kre[1],
  suma_mat[nomer_sheta][1]-deb_kre[2],
  suma_mat[nomer_sheta][2]-deb_kre[3],
  suma_mat[nomer_sheta][3]-deb_kre[4],
  deb_kre[5]);
  
  
 
 
 }
fprintf(ff,"\
---------------------------------------------------------------------------------------------------\n");
return(0);
}
