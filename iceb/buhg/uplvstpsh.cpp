/*$Id: uplvstpsh.c,v 5.4 2013/09/26 09:43:43 sasa Exp $*/
/*20.06.2013	27.05.2003	Белых А.И.	uplvstpsh.c
Ведомость списания топлива по водителям и счетам
*/
#include        <errno.h>
#include        "buhg.h"

void  itogvst(int metkai,const char *kodi,double toplivo,double zat_top_nor,double ned,double per,FILE *ff,FILE *ff_shet);
void     uplvstpsh_shetvst(int*,int*,FILE*);
void  uplvstpsh_svst(int*,int*,FILE*);
void uplvstpsh_rp(int *kolstrok,const char *toplivo,const char *voditel,const char *shet,const char*,FILE *ff);

extern int      kol_strok_na_liste;

int uplvstpsh(class spis_oth *oth)
{
static class iceb_tu_str dat1("");
static class iceb_tu_str dat2("");
static class iceb_tu_str toplivo("");
static class iceb_tu_str voditel("");
static class iceb_tu_str shet("");
static class iceb_tu_str kod_avtom("");
int		kom=0,kom1=0;
short		dn,mn,gn;
short		dk,mk,gk;
char	strsql[1024];
class iceb_tu_str kodn("");
class iceb_tu_str naimn("");
SQLCURSOR cur1;
VVOD VV(0);
VVOD MENU(3);

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(toplivo.ravno(),128);
VV.VVOD_SPISOK_add_data(voditel.ravno(),128);
VV.VVOD_SPISOK_add_data(shet.ravno(),128);
VV.VVOD_SPISOK_add_data(kod_avtom.ravno(),128);

VV.VVOD_SPISOK_add_ZAG(gettext("Распечатать ведомость списания топлива по водителям и счетам"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.(д.м.г)."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..(д.м.г)."));
VV.VVOD_SPISOK_add_MD(gettext("Марка топлива..(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Код водителя...(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт...........(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Код автомобиля.(,,)."));

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
kod_avtom.new_plus(VV.data_ravno(5));

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
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка автомобилей"));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Выход."));//4

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 4 :
      case -1:
       goto naz;

      case 0 :
        vibrek("Uplmt",&toplivo);
        VV.data_plus(2,toplivo.ravno());
        goto naz;

      case 1 :
        naimn.new_plus("");
        
        if(dirout(&kodn,&naimn,1,2) == 0)
          VV.data_z_plus(3,kodn.ravno());
        goto naz;

      case 2 :
        vibrek("Plansh",&shet);
        VV.data_plus(4,shet.ravno());
        goto naz;

      case 3 :
        naimn.new_plus("");
        if(diravt0(&kodn,&naimn,1) == 0)
         VV.data_z_plus(5,kodn.ravno());
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
time_t tmm;
time(&tmm);

printw("%s:%d.%d.%d => %d.%d.%d \n",
gettext("Диапазон поиска"),
dn,mn,gn,dk,mk,gk);

GDITE();

long	kolstr=0;
SQL_str row,row1;
FILE    *ff;
char    imaf[64];
int	kollist=0;
int	kolstrok=0;
short   d,m,g;

sprintf(strsql,"select * from Upldok2a where datd >= '%d-%d-%d' and datd <= '%d-%d-%d' order by datd asc",gn,mn,dn,gk,mk,dk);

SQLCURSOR cur;
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


/*для нужной нам сортировки создаём временную таблицу*/

class iceb_t_tmptab tabtmp;
const char *imatmptab={"uplvstpsh"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
datd DATE not null default '0000-00-00',\
kp int not null default 0,\
nomd char(32) not null default '',\
kt char(32) not null default '',\
shet char(32) not null default '',\
zt float(8,3) not null default 0.,\
zpn float(8,3) not null default 0.,\
kv int not null default 0,\
index(shet,kv,kt,datd)) ENGINE = MYISAM",imatmptab);
/*****
0 datd дата документа
1 kp   код подразделения
2 nomd номер документа
3 kt   код топлива
4 shet счёт
5 zt   затраты топлива фактические
6 zpn  затраты по норме
7 kv   код водителя
****/

if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
  return(1);
  
long kolstr1=0;

while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s\n",row[0],row[1],row[2]);
  strzag(LINES-1,0,kolstr,++kolstr1);
  if(proverka(toplivo.ravno(),row[3],0,0) != 0)
   continue;

  if(proverka(shet.ravno(),row[4],0,0) != 0)
   continue;
  /*читаем шапку документа чтобы узнать водителя и автомобиль*/
  sprintf(strsql,"select ka,kv from Upldok where datd='%s' and kp=%s and nomd='%s'",row[0],row[1],row[2]);
  if(readkey(strsql,&row1,&cur1) == 1)
   {
    if(proverka(voditel.ravno(),row1[1],0,0) != 0)
     continue;

    if(proverka(kod_avtom.ravno(),row1[0],0,0) != 0)
     continue;
   }     

  sprintf(strsql,"insert into %s values ('%s',%s,'%s','%s','%s',%s,%s,%s)",
  imatmptab,row[0],row[1],row[2],row[3],row[4],row[5],row[6],row1[1]);

  sql_zapis(strsql,1,0);    
 }


sprintf(strsql,"select * from %s order by shet,kv,kt,datd asc",imatmptab);


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


sprintf(imaf,"vst%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }


kolstrok=5;
iceb_t_zagolov(gettext("Ведомость списания топлива по счетам и водителям"),dn,mn,gn,dk,mk,gk,ff);

uplvstpsh_rp(&kolstrok,toplivo.ravno(),voditel.ravno(),shet.ravno(),kod_avtom.ravno(),ff);

char imaf_shet[64];
FILE *ff_shet;
sprintf(imaf_shet,"vsts%d.lst",getpid());

if((ff_shet = fopen(imaf_shet,"w")) == NULL)
 {
  error_op_nfil(imaf_shet,errno,"");
  return(1);
 }


iceb_t_zagolov(gettext("Cписание топлива по счетам"),dn,mn,gn,dk,mk,gk,ff_shet);

uplvstpsh_rp(&kolstrok,toplivo.ravno(),voditel.ravno(),shet.ravno(),kod_avtom.ravno(),ff_shet);

fprintf(ff_shet,"\
--------------------------------------------------------------------\n");
fprintf(ff_shet,gettext("\
 Счёт |Наименование счёта  |Фак.з.т.|По норме|Недорасход|Перерасход|\n"));
fprintf(ff_shet,"\
--------------------------------------------------------------------\n");




uplvstpsh_svst(&kollist,&kolstrok,ff);

double itoplivo[3];
double izat_top_nor[3];
double stoplivo=0.;
class iceb_tu_str naimvod;

memset(&itoplivo,'\0',sizeof(itoplivo));
memset(&izat_top_nor,'\0',sizeof(izat_top_nor));

class iceb_tu_str kodvod("");
class iceb_tu_str kodmt("");
class iceb_tu_str shetz("");


kolstr1=0;
//double nst,nstzg,ztvsn,ztvrn;
double zat_top_nor=0.;
double raznica=0.;
double nedorashod_top=0.,pererashod_top=0.;
double nedorashod_vod=0.,pererashod_vod=0.;
double nedorashod_shet=0.,pererashod_shet=0.;

while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4]);
//  refresh();  
  
  strzag(LINES-1,0,kolstr,++kolstr1);


  if(SRAV(kodmt.ravno(),row[3],0) != 0)
   {
    if(kodmt.ravno()[0] != '\0')
     {
      uplvstpsh_shetvst(&kolstrok,&kollist,ff);
      itogvst(0,kodmt.ravno(),itoplivo[0],izat_top_nor[0],nedorashod_top,pererashod_top,ff,NULL);
     }
    nedorashod_top=pererashod_top=0.;
    itoplivo[0]=0.;
    izat_top_nor[0]=0.;
    kodmt.new_plus(row[3]);
   }

  if(SRAV(kodvod.ravno(),row[7],0) != 0)
   {
    if(kodvod.ravno()[0] != '\0')
     {
      uplvstpsh_shetvst(&kolstrok,&kollist,ff);
      itogvst(0,kodmt.ravno(),itoplivo[0],izat_top_nor[0],nedorashod_top,pererashod_top,ff,NULL);

      uplvstpsh_shetvst(&kolstrok,&kollist,ff);
      itogvst(1,kodvod.ravno(),itoplivo[1],izat_top_nor[1],nedorashod_vod,pererashod_vod,ff,NULL);
     }
    nedorashod_top=pererashod_top=0.;
    nedorashod_vod=pererashod_vod=0.;

    itoplivo[0]=0.;
    izat_top_nor[0]=0.;

    itoplivo[1]=0.;
    izat_top_nor[1]=0.;

    kodvod.new_plus(row[7]);

    naimvod.new_plus("");
    sprintf(strsql,"select naik from Uplouot where kod=%s",kodvod.ravno());
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     naimvod.new_plus(row1[0]);
   }

  if(SRAV(shetz.ravno(),row[4],0) != 0)
   {
    if(shetz.ravno()[0] != '\0')
     {
      uplvstpsh_shetvst(&kolstrok,&kollist,ff);
      itogvst(0,kodmt.ravno(),itoplivo[0],izat_top_nor[0],nedorashod_top,pererashod_top,ff,NULL);

      uplvstpsh_shetvst(&kolstrok,&kollist,ff);
      itogvst(1,kodvod.ravno(),itoplivo[1],izat_top_nor[1],nedorashod_vod,pererashod_vod,ff,NULL);

      uplvstpsh_shetvst(&kolstrok,&kollist,ff);
      itogvst(2,shetz.ravno(),itoplivo[2],izat_top_nor[2],nedorashod_shet,pererashod_shet,ff,ff_shet);

     }
    nedorashod_top=pererashod_top=0.;
    nedorashod_vod=pererashod_vod=0.;
    nedorashod_shet=pererashod_shet=0.;
    memset(&itoplivo,'\0',sizeof(itoplivo));
    memset(&izat_top_nor,'\0',sizeof(izat_top_nor));

    shetz.new_plus(row[4]);
   }
  //читаем нормы расхода топлива

//  nst=nstzg=ztvsn=ztvrn=0.;


  rsdat(&d,&m,&g,row[0],2);

  uplvstpsh_shetvst(&kolstrok,&kollist,ff);


  stoplivo=atof(row[5]);
  stoplivo=okrug(stoplivo,0.001);  

  //определяем затраты топлива по норме
  zat_top_nor=atof(row[6]);
  zat_top_nor=okrug(zat_top_nor,0.001);

  raznica=stoplivo-zat_top_nor;
  raznica=okrug(raznica,0.001);
  
  fprintf(ff,"%-*s %-*s %-*.*s %-*s %02d.%02d.%04d %-8s %8.8g %8.8g",
  iceb_tu_kolbait(6,row[4]),row[4],
  iceb_tu_kolbait(4,row[7]),row[7],
  iceb_tu_kolbait(16,naimvod.ravno()),iceb_tu_kolbait(16,naimvod.ravno()),naimvod.ravno(),
  iceb_tu_kolbait(6,row[3]),row[3],
  d,m,g,row[2],stoplivo,zat_top_nor);  

  if(raznica < 0)
   {
    fprintf(ff," %10.10g\n",raznica);
    nedorashod_top+=raznica;
    nedorashod_vod+=raznica;
    nedorashod_shet+=raznica;
   }
  else
   {
    fprintf(ff," %10s %10.10g\n"," ",raznica);
    pererashod_top+=raznica;    
    pererashod_vod+=raznica;    
    pererashod_shet+=raznica;    
   }
  

  itoplivo[0]+=stoplivo;
  izat_top_nor[0]+=zat_top_nor;
  
  itoplivo[1]+=stoplivo;
  izat_top_nor[1]+=zat_top_nor;

  itoplivo[2]+=stoplivo;
  izat_top_nor[2]+=zat_top_nor;

 }

itogvst(0,kodmt.ravno(),itoplivo[0],izat_top_nor[0],nedorashod_top,pererashod_top,ff,NULL);
itogvst(1,kodvod.ravno(),itoplivo[1],izat_top_nor[1],nedorashod_vod,pererashod_vod,ff,NULL);
itogvst(2,shetz.ravno(),itoplivo[2],izat_top_nor[2],nedorashod_shet,pererashod_shet,ff,ff_shet);

podpis(ff);
podpis(ff_shet);
fclose(ff);
fclose(ff_shet);

printw("%s:%d\n",gettext("Количество листов"),kollist);
printw_vr(tmm);

OSTANOV();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Ведомость списания топлива по счетам и водителям"));
oth->spis_imaf.plus(imaf_shet);
oth->spis_naim.plus(gettext("Общие итоги"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nom),0);

return(0);
}
/*************************************/
/*Шапка                              */
/*************************************/

void    uplvstpsh_svst(int *kollist,int *kolstrok,FILE *ff)
{
*kollist+=1;
*kolstrok+=4;

fprintf(ff,"%60s%s%d\n","",gettext("Лист N"),*kollist);

fprintf(ff,"\
------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Счёт |К/в |Фамилия водителя|К.топ.|  Дата    |Ном.док.|Фак.з.т.|По норме|Недорасход|Перерасход|\n"));
fprintf(ff,"\
------------------------------------------------------------------------------------------------\n");
//123456 1234 1234567890123456 123456 1234567890 12345678 123456 12345678 1234
}

/************************************/
/*Итоговая строка                   */
/************************************/

void  itogvst(int metkai, //0-Итог по топливу 1-по водителю 2-по счету
const char *kodi, //Код итога
double toplivo,
double zat_top_nor,
double ned,
double per,
FILE *ff,
FILE *ff_shet)
{
char  strsql[512];
char  stroka[1024];
SQL_str  row;
SQLCURSOR curr;
if( toplivo == 0.)
  return;
  
memset(stroka,'\0',sizeof(stroka));

if(metkai == 0) //По топливу
 sprintf(stroka,"%s %s %s",
 gettext("Итого"),
 gettext("по"),
 kodi);

if(metkai == 1) //По водителю
 {
  sprintf(stroka,"%s %s %s ",
  gettext("Итого"),
  gettext("по"),
  kodi);

  sprintf(strsql,"select naik from Uplouot where kod=%s",kodi);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    strcat(stroka,row[0]);
 }

if(metkai == 2) //По счету
 {
  sprintf(stroka,"%s %s %s ",
  gettext("Итого"),
  gettext("по"),
  kodi);

  sprintf(strsql,"select nais from Plansh where ns='%s'",kodi);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    strcat(stroka,row[0]);

  fprintf(ff_shet,"%6s %*.*s %8.8g %8.8g %10.10g %10.10g\n",
  kodi,
  iceb_tu_kolbait(20,row[0]),
  iceb_tu_kolbait(20,row[0]),
  row[0],
  toplivo,zat_top_nor,ned,per);

 }
 
fprintf(ff,"%*s:%8.8g %8.8g %10.10g %10.10g\n",
iceb_tu_kolbait(55,stroka),stroka,toplivo,zat_top_nor,ned,per);

}

/********************************/
/*Счетчик                       */
/********************************/
void uplvstpsh_shetvst(int *kolstrok,int *kollist,FILE *ff)
{
*kolstrok+=1;
if(*kolstrok > kol_strok_na_liste)
 {
  fprintf(ff,"\f");
  *kolstrok=0;
  uplvstpsh_svst(kollist,kolstrok,ff);
 }  

}
/*********************************/
/*Реквизиты поиска */
/****************************/
void uplvstpsh_rp(int *kolstrok,const char *toplivo,
const char *voditel,
const char *shet,
const char *kod_avtom,
FILE *ff)
{
if(toplivo[0] != '\0')
 {
  *kolstrok+=1;
  fprintf(ff,"%s:%s\n",gettext("Код топлива"),toplivo);  
 }

if(voditel[0] != '\0')
 {
  *kolstrok+=1;
  fprintf(ff,"%s:%s\n",gettext("Код водителя"),voditel);  
 }

if(shet[0] != '\0')
 {
  *kolstrok+=1;
  fprintf(ff,"%s:%s\n",gettext("Счёт"),shet);  
 }
if(kod_avtom[0] != '\0')
 {
  *kolstrok+=1;
  fprintf(ff,"%s:%s\n",gettext("Код автомобиля"),kod_avtom);  
 }
}
