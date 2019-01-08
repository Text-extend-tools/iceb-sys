/* $Id: prognoz.c,v 5.38 2014/02/28 05:13:47 sasa Exp $ */
/*15.07.2015	16.04.1998	Белых А.И.	prognoz.c
Расчёт завоза товара
Смотрим реализацию за заказанный период времени
Вычисляем среднюю реализацию за день
Делим остаток на эту реализацию и вычисляем на сколько 
дней торговли хватит товара
*/
#include        <errno.h>
#include        "buhg.h"

extern double	okrg1;  /*Округление суммы*/
extern double	okrcn;  /*Округление цены*/

int prognoz(class spis_oth *oth)
{
char		bros[512];
struct  tm      *bf;
time_t          tmm;
FILE            *ff;
char            imaf[64];
static class iceb_tu_str dat1(""),dat2("");
static class iceb_tu_str sklz("");
static class iceb_tu_str grz("");
static class iceb_tu_str kodmz("");
static class iceb_tu_str koldneih(""); //Количество дней на которые надо расчитать завоз
int		koldnei=0; //Количество дней на которые надо расчитать завоз
int		skl;
int           kom,kom1;
short           d,m,g;
short           dn,mn,gn;
short           dk,mk,gk;
double		real,srzd;
int		kold,kdn;
int		i;
long		kolstr,kolstr1,kolstr2;
SQL_str         row,row1;
char		strsql[1024];
class iceb_tu_str naimat("");
double		ost;
class iceb_tu_str naiskl("");
int		kgr,kodm;
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
VVOD MENU(3);
VVOD VV(0);

VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт прогноза реализации"));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11);
VV.VVOD_SPISOK_add_data(sklz.ravno(),16);
VV.VVOD_SPISOK_add_data(grz.ravno(),128);
VV.VVOD_SPISOK_add_data(kodmz.ravno(),128);
VV.VVOD_SPISOK_add_data(koldneih.ravno(),4);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала........."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца.........."));
VV.VVOD_SPISOK_add_MD(gettext("Склад..............."));
VV.VVOD_SPISOK_add_MD(gettext("Группа.....(,,)....."));
VV.VVOD_SPISOK_add_MD(gettext("Код материалла.(,,)."));
VV.VVOD_SPISOK_add_MD(gettext("Количество дней....."));

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
sklz.new_plus(VV.data_ravno(2));
grz.new_plus(VV.data_ravno(3));
kodmz.new_plus(VV.data_ravno(4));
koldneih.new_plus(VV.data_ravno(5));
class iceb_tu_str kods("");
class iceb_tu_str naims("");
switch (kom)
 {

  case FK10:
  case ESC:
   return(1);

  case FK1:
    GDITE();
    iceb_t_pdoc("matu5_11.txt");
    clear();
    goto naz;

  case FK2:
  case PLU:
    break;

  case FK3:
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка складов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп"));
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
        if(dirsklad(1,&kods,&naims) == 0)
          VV.data_z_plus(2,kods.ravno());
        goto naz;

      case 1 :
        vibrek("Grup",&grz,&naims);
        VV.VVOD_SPISOK_zapis_data(3,grz.ravno());
        goto naz;

      case 2 :
        if(dirmatr(&kgr,&kodm,&naims,1,0,"",0,0) == 0)
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

if((skl=(int)ATOFM(VV.VVOD_SPISOK_return_data(2))) == 0)
 {
  iceb_t_soob(gettext("Не введён склад !"));
  goto naz;
 }
SQLCURSOR cur;

sprintf(strsql,"select * from Sklad where kod=%d",
skl);
if(sql_readkey(&bd,strsql,&row1,&cur) != 1)
 {
  sprintf(strsql,gettext("Не найден склад %d в списке складов !"),skl);
  iceb_t_soob(strsql);
  goto naz;
 }

koldnei=koldneih.ravno_atoi();

naiskl.new_plus(row1[1]);

if(rsdatp(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(0),&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(1)) != 0)
  goto naz;

/*Расчитываем количество дней в периоде*/

printw("%d.%d.%d %d.%d.%d\n",dn,mn,gn,dk,mk,gk);
refresh();

kold=period(dn,mn,gn,dk,mk,gk,0);

if(kold == 0)
 {
  iceb_t_soob(gettext("Количество дней в периоде равно нолю !"));
  goto naz;
 }

clear();
GDITE();
time(&tmm);
bf=localtime(&tmm);

sprintf(strsql,"select kodm,naimat,kodgr from Material order by kodgr,naimat asc");
if((kolstr2=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }



if(kolstr2 == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return(1);
 }


sprintf(imaf,"zav%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

printw(gettext("Период с %d.%d.%d до %d.%d.%d Склад %d Количество дней %d\n"),
dn,mn,gn,dk,mk,gk,skl,kold);
refresh();


fprintf(ff,"\
%s\n\n%s %d %s\n\
%s %d.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d%s  %s - %d:%d\n",
iceb_t_get_pnk("00",0),
gettext("Склад"),
skl,naiskl.ravno(),
gettext("Расчёт прогнозируемой реализации на основании реализации\nза период c"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ff,gettext("Эталонный период %d дней.\n"),kold);
fprintf(ff,gettext("Расчётный период %d дней.\n"),koldnei);
fprintf(ff,"\
-----------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
 К/м | Наименование товара    | Остаток |Реализация|Средняя|Кол. |   Дата   |Спрогнозированый\n"));
fprintf(ff,gettext("\
     |                        |         |за период |за день|дней |          |завоз товара\n"));
fprintf(ff,"\
-----------------------------------------------------------------------------------------------\n");
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
/*
  printw("%s %s\n",row[0],row[1]);
  refresh();
*/  
  strzag(LINES-1,0,kolstr2,kolstr1++);
  
  kodm=atoi(row[0]);
  naimat.new_plus(row[1]);
  if(proverka(kodmz.ravno(),row[0],0,0) != 0)
    continue;
  if(proverka(grz.ravno(),row[2],0,0) != 0)
    continue;
  ost=ostdok1(dt,mt,mt,skl,kodm);

  sprintf(strsql,"select kolih from Dokummat1 where \
kodm=%d and sklad=%d and tipz=%d and \
datd >= '%d-%02d-%02d' and datd <= '%d-%02d-%02d'",
  kodm,skl,2,gn,mn,dn,gk,mk,dk);
/*
  printw("strsql=%s\n",strsql);
*/    
  SQLCURSOR cur1;
  if((kolstr=cur1.make_cursor(&bd,strsql)) < 0)
   {
    printw("%s\n",sql_error(&bd));
    OSTANOV();
   }
/*    printw("kolstr=%ld\n",kolstr);*/
  if(kolstr == 0)
    continue;

  real=0.;
  while(cur1.read_cursor(&row1) != 0)
   {
/*  
    printw("%s %s\n",row1[0],row1[1]);
    refresh();
*/
    real+=atof(row1[0]);
   }

  
  srzd=real/kold;

  kdn=0;
  if(srzd > 0.)
    kdn=(int)(ost/srzd);        

  d=dt; m=mt; g=gt;
  for(i=0; i < kdn; i++)
    dpm(&d,&m,&g,1);

  printw("%-5d %-*.*s %9.9g %-10d %02d.%02d.%d\n",
  kodm,
  iceb_tu_kolbait(24,naimat.ravno()),iceb_tu_kolbait(24,naimat.ravno()),naimat.ravno(),
  ost,kdn,d,m,g);

  refresh();

  if(kdn > 0)
    sprintf(bros,"%02d.%02d.%d",d,m,g);
  else
    strcpy(bros,"**********");

  strsql[0]='\0';
  if(koldnei != 0)
    sprintf(strsql,"%.10g",koldnei*srzd);
        
  fprintf(ff,"%-5d %-*.*s %9.9g %10.10g %7.2f %-5d %s %s\n",
  kodm,
  iceb_tu_kolbait(24,naimat.ravno()),iceb_tu_kolbait(24,naimat.ravno()),naimat.ravno(),
  ost,real,srzd,kdn,bros,strsql);
    
 }
fprintf(ff,"\
-----------------------------------------------------------------------------------------------\n");

podpis(ff);
fclose(ff);

OSTANOV();
oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Прогноз реализации"));

iceb_t_ustpeh(oth->spis_imaf.ravno(0),3);

return(0);

}
