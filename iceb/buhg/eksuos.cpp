/*$Id: eksuos.c,v 5.4 2013/11/24 08:23:03 sasa Exp $*/
/*14.07.2015	16.06.2013	Белых А.И.	eksuos.c
Экспорт основных средств из учёта основных средств
*/

#include "buhg.h"
#include "poiinpd.h"
#include "bsizt.h"

int eksuos(class spis_oth *oth)
{
int kom=0,kom1=0;
static class iceb_tu_str dataost("");
static class iceb_tu_str kodpdr("");
static class iceb_tu_str kodmot("");
static class iceb_tu_str shetuh("");
class iceb_tu_str kod("");
class iceb_tu_str naim("");

if(dataost.getdlinna() <= 1)
  dataost.plus_tek_dat();

class VVOD MENU(3);
class VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Экспорт основных средств"));

VV.VVOD_SPISOK_add_MD(gettext("Дата экспорта................(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Код подразделения...............(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Код материально ответственного..(,,).."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт учёта......................(,,).."));

VV.VVOD_SPISOK_add_data(dataost.ravno(),11);
VV.VVOD_SPISOK_add_data(kodpdr.ravno(),128);
VV.VVOD_SPISOK_add_data(kodmot.ravno(),128);
VV.VVOD_SPISOK_add_data(shetuh.ravno(),128);

naz:;

clear();
 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dataost.new_plus(VV.VVOD_SPISOK_return_data(0));
kodpdr.new_plus(VV.VVOD_SPISOK_return_data(1));
kodmot.new_plus(VV.VVOD_SPISOK_return_data(2));
shetuh.new_plus(VV.VVOD_SPISOK_return_data(3));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   iceb_t_pdoc("buhg5_1.txt");
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;


  case FK3:
    MENU.VVOD_delete();

    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка материально ответственных"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 3:
      case -1:
        goto naz;

      case 0 :
        if(diruospod(0,&kod,&naim,0) == 0)
          VV.data_z_plus(1,kod.ravno());
        break;

      case 1:
        if(dirmatot(0,&kod,&naim,0) == 0)
          VV.data_z_plus(2,kod.ravno());
        break;
    
      case 2:
        vibrek("Plansh",&shetuh);
        VV.data_plus(3,shetuh.ravno());
        break;

     }
    goto naz;


  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }
short dost=0,most=0,gost=0;
if(rsdat(&dost,&most,&gost,dataost.ravno(),1) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата!"));
  goto naz;
 }
clear();
char strsql[1024];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select * from Uosin");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  goto naz;
 }
class iceb_fopen fil;
char imafil[64];

sprintf(imafil,"eksuos%d.csv",getpid());

if(fil.start(imafil,"w") != 0)
 return(1);

class iceb_fopen filrs;
char imafilrs[64];

sprintf(imafilrs,"eksuosr%d.lst",getpid());

if(filrs.start(imafilrs,"w") != 0)
 return(1);

iceb_t_zageks(gettext("Экспорт основных средств"),filrs.ff);
fprintf(filrs.ff,"%s:%s\n",gettext("Дата экспорта"),dataost.ravno());


fprintf(fil.ff,"#Расшифровка полей записи:\n\
#1  инвентарный номер\n\
#2  год изготовления\n\
#3  наименование\n\
#4  завод изготовитель\n\
#5  паспорт\n\
#6  модель\n\
#7  заводской номер\n\
#8  дата ввода в эксплуатацию\n\
#9  счёт учета\n\
#10 шифр затрат\n\
#11 шифр аналитического учета\n\
#12 шифр нормы амотротчислений (группа) для налогового учёта\n\
#13 поправочный коэффициент для налогового учёта\n\
#14 состояние объекта 0-амортизация считается для бухучета и налогового учёта\n\
#                     1-амортизация не считается для бух.учета и налогового учёта\n\
#                     2-считается для бух.учета и не считается для налогового\n\
#                     3-не считается для бух учета и считается для налогового\n\
#15 номерной знак (для автомобилей)\n\
#16 шифр нормы амотротчислений (группа) для бух. учета\n\
#17 поправочный коэффициент для бух. учета\n\
#18 Балансовая стоимость для налогового учёта\n\
#19 Износ для налогового учёта\n\
#20 Балансовая стоимость для бухгалтерского учёта\n\
#21 Износ для бухгалтерского учёта\n\
#22 Балансовая стоимость для налогового учёта на начало года\n\
#23 Износ для налогового учёта на начало года\n");


if(kodpdr.getdlinna() > 1)
 {
  fprintf(fil.ff,"#%s:%s\n",gettext("Код подразделения"),kodpdr.ravno());
  fprintf(filrs.ff,"%s:%s\n",gettext("Код подразделения"),kodpdr.ravno());
 } 

if(kodmot.getdlinna() > 1)
 {
  fprintf(fil.ff,"#%s:%s\n",gettext("Код материально ответственного"),kodmot.ravno());
  fprintf(filrs.ff,"%s:%s\n",gettext("Код материально ответственного"),kodmot.ravno());
 } 
if(shetuh.getdlinna() > 1)
 {
  fprintf(fil.ff,"#%s:%s\n",gettext("Счёт учёта"),shetuh.ravno());
  fprintf(filrs.ff,"%s:%s\n",gettext("Счёт учёта"),shetuh.ravno());
 } 
fprintf(filrs.ff,"\
---------------------------------------------------------------------------------------------------\n");
fprintf(filrs.ff,gettext("\
Инв-й ном.|Подр.|М.о.л|Наименование основного средства|Нал.стоим.|Нал.износ |Бух.стоим.|Бух.износ |\n"));
/***********
1234567890 12345 12345 1234567890123456789012345678901 1234567890 1234567890 1234567890 1234567890
***********/
fprintf(filrs.ff,"\
---------------------------------------------------------------------------------------------------\n");
/*****************************
strcpy(strsql,"CREATE TABLE Uosin (\
innom   int unsigned not null default 0 primary key,\
god     smallint not null default 0,\
naim	varchar(80) not null default '',\
zaviz   varchar(60) not null default '',\
pasp	varchar(60) not null default '',\
model   varchar(60) not null default '',\
zavnom  varchar(60) not null default '',\
datvv   DATE not null default '0000-00-00',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0)");

0 innom   инвентарный номер
1 god     год изготовления
2 naim	наименование
3 zaviz   завод изготовитель
4 pasp	паспорт
5 model   модель
6 zavnom  заводской номер
7 datvv   дата ввода в эксплуатацию
8 ktoz  кто записал
9 vrem  время записи
*********************************/
int kolstr1=0;
int innom=0;
int podr=0;
int kodotl=0;
class poiinpdw_data rekin;
class bsizw_data bal_st; /*балансовая стоимость на дату остатка*/
class bsizw_data bal_stng; /*балансовая стоимость на начало года*/


double bs_nu=0.;
double iznos_nu=0.;

double bs_bu=0.;
double iznos_bu=0.;

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);
  innom=atoi(row[0]);
  
  if(poiin(innom,dost,most,gost,&podr,&kodotl) != 0)
   continue;

  if(proverka(kodpdr.ravno(),podr,0,0) != 0)
   continue;

  if(proverka(kodmot.ravno(),kodotl,0,0) != 0)
   continue;

  poiinpd(innom,most,gost,&rekin);

  if(proverka(shetuh.ravno(),rekin.shetu.ravno(),0,0) != 0)
   continue;

  printw("%10s %s\n",row[0],row[2]);
                               

  bsiz(innom,podr,1,1,gost,&bal_stng,NULL);
  bsiz(innom,podr,dost,most,gost,&bal_st,NULL);

  bs_nu=bal_st.kbsnu;
  iznos_nu=bal_st.kiznu;

  bs_bu=bal_st.kbsbu;
  iznos_bu=bal_st.kizbu;
  
/****************************************

    double sbs,siz; //Стартовая балансовая стоимость и износ для налогового учёта
    double bs,iz; //Изменения балансовой стоимости и износа для налогового учёта
    double iz1; //Амортизация для налогового учёта
    double sbsby,sizby; //Стартовая балансовая стоимость и износ для бух.учёта
    double bsby,izby; //Изменения балансовой стоимости и износа для бух.учёта
    double iz1by; //Амортизация для бух. учёта
strcpy(strsql,"CREATE TABLE Uosinp (\
innom   int unsigned not null default 0,\
mes	smallint not null default 0,\
god     smallint not null default 0,\
shetu   varchar(20) not null default '' references Plansh (ns),\
hzt     varchar(20) not null default '',\
hau     varchar(20) not null default '',\
hna     varchar(20) not null default '',\
popkof  double(12,6) not null default 0.,\
soso    smallint not null default 0,\
nomz	varchar(40) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
hnaby   varchar(20) not null default '',\
popkofby double(12,6) not null default 0.,\
unique(innom,god,mes))");

0  innom   инвентарный номер
1  mes	   месяц записи
2  god     год записи,\
3  shetu   счёт учета
4  hzt     шифр затрат
5  hau     шифр аналитического учета
6  hna     шифр нормы амотротчислений (группа) для налогового учёта
7  popkof  поправочный коэффициент для налогового учёта
8  soso    состояние объекта 0-амортизация считается для бухучета и налогового учёта
                             1-амортизация не считается для бух.учета и налогового учёта
                             2-считается для бух.учета и не считается для налогового
                             3-не считается для бух учета и считается для налогового
9  nomz	   номерной знак (для автомобилей)
10 ktoz    кто записал
11 vrem    время записи
12 hnaby   шифр нормы амотротчислений (группа) для бух. учета
13 popkofby  поправочный коэффициент для бух. учета

  class iceb_tu_str shetu; //Счёт учета
  class iceb_tu_str hzt; //Шифр затрат
  class iceb_tu_str hau; //Шифр аналитического учета
  class iceb_tu_str hna; //Шифр нормы амортотчислений для налогового учёта
  float popkf; //Поправочный коэффициент для налогового учёта
  int soso; //Состояние объекта
  class iceb_tu_str nomz;  //Номерной знак
  short mz; short gz;  //дата записи
  class iceb_tu_str hnaby; //Шифр нормы амортотчислений для бух. учета
  float popkfby;  ///Поправочный коэффициент для бух. учета


***************************************/
  fprintf(filrs.ff,"%10d|%5d|%5d|%-*.*s|%10.2f|%10.2f|%10.2f|%10.2f|\n",
  innom,
  podr,
  kodotl,
  iceb_tu_kolbait(31,row[2]),
  iceb_tu_kolbait(31,row[2]),
  row[2],
  bs_nu,iznos_nu,bs_bu,iznos_bu);  
    
  fprintf(fil.ff,"%s|%s|%s|%s|%s|%s|%s|%s|\
%s|%s|%s|%s|%f|%d|%s|%s|%f|\
%.2f|%.2f|%.2f|%.2f|%.2f|%.2f|\n",
  row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],
  rekin.shetu.ravno(),rekin.hzt.ravno(),rekin.hna.ravno(),rekin.hna.ravno(),rekin.popkf,rekin.soso,rekin.nomz.ravno(),rekin.hnaby.ravno(),rekin.popkfby,
  bs_nu,iznos_nu,bs_bu,iznos_bu,
  bal_stng.kbsnu,
  bal_stng.kiznu);
 }

fprintf(filrs.ff,"\
---------------------------------------------------------------------------------------------------\n");
podpis(filrs.ff);

fil.end();
filrs.end();

oth->spis_imaf.plus(imafilrs);
oth->spis_naim.plus(gettext("Экспорт основных средств (распечатка)"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
  iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);

oth->spis_imaf.plus(imafil);
oth->spis_naim.plus(gettext("Экспорт основных средств (файл для загрузки)"));

return(0);

}

