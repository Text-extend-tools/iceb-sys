/*$Id: uplvst_kg.c,v 5.23 2013/09/26 09:43:43 sasa Exp $*/
/*21.11.2011	24.05.2005	Белых А.И.	uplvst_kg.c
Ведомость движения топлива в киллограммах
*/
#include        <errno.h>
#include        "buhg.h"

void  itogvst_kg(int,char*,int,double,double,FILE*);
void     shetvst_kg(int*,int*,FILE*);
void     svst_kg(int*,int*,FILE*);
void uplvst_kg_it(int metka,double *itog,FILE *ff);


extern int      kol_strok_na_liste;

int uplvst_kg(class spis_oth *oth)
{
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
static class iceb_tu_str toplivo("");
static class iceb_tu_str voditel("");
static class iceb_tu_str shet("");
int		kom=0,kom1=0;
short		dn,mn,gn;
short		dk,mk,gk;
char	strsql[1024];
class iceb_tu_str kod("");
class iceb_tu_str naimk("");

SQLCURSOR curr;

VVOD VV(0);
VVOD MENU(3);

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(toplivo.ravno(),128);
VV.VVOD_SPISOK_add_data(voditel.ravno(),128);
VV.VVOD_SPISOK_add_data(shet.ravno(),128);

VV.VVOD_SPISOK_add_ZAG(gettext("Распечатать ведомость списания топлива по счетам в киллограммах"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.(д.м.г)."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..(д.м.г)."));
VV.VVOD_SPISOK_add_MD(gettext("Марка топлива..(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Код водителя...(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт...........(,,)."));

naz:;

clear();


helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dat1.new_plus(VV.data_ravno(0));
dat2.new_plus(VV.data_ravno(1));
toplivo.new_plus(VV.data_ravno(2));
voditel.new_plus(VV.data_ravno(3));
shet.new_plus(VV.data_ravno(4));

switch(kom)
 {
  case FK1:
    GDITE();
    iceb_t_pdoc("upl4_3.txt");
    clear();
    goto naz;
  case FK10:
  case ESC:
   return(1);
  case FK2:
  case PLU:
   break;

  case FK3:  /*Просмотр реквизитов*/
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка марок топлива"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка водителей"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//3

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 3 :
      case -1:
       goto naz;

      case 0 :
        vibrek("Uplmt",&toplivo);
        VV.VVOD_SPISOK_zapis_data(2,toplivo.ravno());
        goto naz;

      case 1:
        naimk.new_plus("");
        if(dirout(&kod,&naimk,1,2) == 0)
          VV.data_z_plus(3,kod.ravno());
        goto naz;

      case 2:
        vibrek("Plansh",&shet);
        VV.VVOD_SPISOK_zapis_data(4,shet.ravno());
        goto naz;

     }
    goto naz;
    

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
 }

if(rsdatp(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(0),&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(1)) != 0)
  goto naz;

clear();
printw("***%s:%d.%d.%d => %d.%d.%d \n",
gettext("Диапазон поиска"),
dn,mn,gn,dk,mk,gk);

time_t 		tmm;
time(&tmm);

GDITE();

long	kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
FILE    *ff;
char    imaf[64];
int	kollist=0;
int	kolstrok=0;

sprintf(strsql,"select datd,nomd,kt,shet,zt,kp from Upldok2a where datd >= '%d-%d-%d' and \
datd <= '%d-%d-%d' order by shet asc",gn,mn,dn,gk,mk,dk);


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



class SPISOK shet_kodt; //Список счёт->код топлива
class masiv_din_double suma_shet_kodt; //Расход топлива в литрах

long kolstr1=0;
int nomer=0;
double suma;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(proverka(toplivo.ravno(),row[2],0,0) != 0)
   continue;


  if(proverka(shet.ravno(),row[3],0,0) != 0)
   continue;
    
  if(voditel.getdlinna() > 1)
   {  
    
    sprintf(strsql,"select kv from Upldok where datd = '%s' and kp=%s and nomd='%s'",row[0],row[5],row[1]);
    if(readkey(strsql,&row1,&cur1) == 1)
     {
      if(proverka(voditel.ravno(),row1[0],0,0) != 0)
       continue;
      
     }  
    else
     {
      VVOD SOOB(1);
      SOOB.VVOD_SPISOK_add_MD(gettext("Не найдена шапка документа !"));
      sprintf(strsql,"%s %s",row[0],row[1]);
      SOOB.VVOD_SPISOK_add_MD(strsql);
      soobshw(&SOOB,stdscr,-1,-1,0,1);
      continue;    
     }
   }
  suma=atof(row[4]);
  sprintf(strsql,"%s|%s",row[3],row[2]);

  if((nomer=shet_kodt.find(strsql)) == -1)
   shet_kodt.plus(strsql);

  suma_shet_kodt.plus(suma,nomer);
 }

sprintf(imaf,"vstkg%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }


kolstrok=5;
iceb_t_zagolov(gettext("Ведомость списания топлива по счетам в киллограммах"),dn,mn,gn,dk,mk,gk,ff);

if(toplivo.ravno()[0] != '\0')
 {
  kolstrok++;
  fprintf(ff,"%s:%s\n",gettext("Код топлива"),toplivo.ravno());  
 }

if(voditel.ravno()[0] != '\0')
 {
  kolstrok++;
  fprintf(ff,"%s:%s\n",gettext("Код водителя"),voditel.ravno());  
 }

if(shet.ravno()[0] != '\0')
 {
  kolstrok++;
  fprintf(ff,"%s:%s\n",gettext("Счёт"),shet.ravno());  
 }

svst_kg(&kollist,&kolstrok,ff);
char shet_sp[64];
char kod_top[64];
double koef=0.;
double cn_suma=0.;
char shet_sp_z[64];
double srcn=0.;
double itog_shet[3];

memset(&itog_shet,'\0',sizeof(itog_shet));

double itog[3];
memset(&itog,'\0',sizeof(itog));

memset(shet_sp,'\0',sizeof(shet_sp));
memset(shet_sp_z,'\0',sizeof(shet_sp_z));
for(int i=0; i < shet_kodt.kolih() ; i++)
 {
  polen(shet_kodt.ravno(i),shet_sp,sizeof(shet_sp),1,'|');  
  polen(shet_kodt.ravno(i),kod_top,sizeof(kod_top),2,'|');  
  if(SRAV(shet_sp,shet_sp_z,0) != 0)
   {
    if(shet_sp_z[0] != '\0')
     uplvst_kg_it(0,itog_shet,ff);
    
    memset(&itog_shet,'\0',sizeof(itog_shet));
    strcpy(shet_sp_z,shet_sp);
   }  


  //читаем среднюю цену и коэффициент перевода
  koef=0.;
  srcn=0.;
  sprintf(strsql,"select srcn,kp from Uplmtsc where datsc <= '%04d-%02d-%02d' and kodt='%s' \
order by datsc desc",
  gn,mn,dn,kod_top);

  if(readkey(strsql,&row,&cur) > 0)
   {
    srcn=atof(row[0]);
    koef=atof(row[1]);
   }  
  suma=koef*suma_shet_kodt.ravno(i);
  suma=okrug(suma,0.001);
  
  shetvst_kg(&kolstrok,&kollist,ff);
  
  cn_suma=srcn*suma;
  
  itog_shet[0]+=suma_shet_kodt.ravno(i);
  itog_shet[1]+=suma;
  itog_shet[2]+=cn_suma;

  itog[0]+=suma_shet_kodt.ravno(i);
  itog[1]+=suma;
  itog[2]+=cn_suma;
  
  fprintf(ff,"%-*s %-*s %14.14g %11.11g %14.14g %7.2f %7.2f\n",
  iceb_tu_kolbait(6,shet_sp),shet_sp,
  iceb_tu_kolbait(11,kod_top),kod_top,
  suma_shet_kodt.ravno(i),koef,suma,srcn,cn_suma);
  
 }
uplvst_kg_it(0,itog_shet,ff);
fprintf(ff,"\
-----------------------------------------------------------------------------\n");
uplvst_kg_it(1,itog,ff);

podpis(ff);
fclose(ff);

printw("%s: %d\n",gettext("Количество листов"),kollist);

printw_vr(tmm);


OSTANOV();
oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Ведомость списания топлива по счетам в киллограммах"));

iceb_t_ustpeh(imaf,0);

return(0);
}
/*************************************/
/*Шапка                              */
/*************************************/

void    svst_kg(int *kollist,int *kolstrok,FILE *ff)
{
*kollist+=1;
*kolstrok+=5;

fprintf(ff,"%60s%s%d\n","",gettext("Лист N"),*kollist);

fprintf(ff,"\
-----------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Счёт |Код топлива|Расход топлива|Коэффициент|Расход топлива|Средняя| Сумма |\n\
      |           |   в литрах   | перевода  |в киллограммах| цена  |       |\n"));
fprintf(ff,"\
-----------------------------------------------------------------------------\n");
//123456 12345678901 12345678901234 12345678901 12345678901234 1234567 1234567
}

/********************************/
/*Счетчик                       */
/********************************/
void     shetvst_kg(int *kolstrok,int *kollist,FILE *ff)
{
*kolstrok+=1;
if(*kolstrok > kol_strok_na_liste)
 {
  fprintf(ff,"\f");
  *kolstrok=0;
  svst_kg(kollist,kolstrok,ff);
 }  

}
/****************************/
/*Выдача итоговой строки*/
/**************************/
void uplvst_kg_it(int metka, //0-итог по счёту 1-общий
double *itog,FILE *ff)
{

char strsql[112];
if(metka == 0)
 sprintf(strsql,"%s",gettext("Итого по счёту"));
if(metka == 1)
 sprintf(strsql,"%s",gettext("Общий итог"));

fprintf(ff,"%*s:%11.11g %11s %14.14g %7s %7.2f\n",
iceb_tu_kolbait(21,strsql),strsql,itog[0]," ",itog[1]," ",itog[2]);



}

