/*$Id: impuosost.c,v 5.4 2014/07/31 07:09:16 sasa Exp $*/
/*13.02.2016	16.06.2013	Белых А.И.	impuosost.c
Импорт основных средств
*/
#include        <sys/stat.h>
#include "buhg.h"



void impuosost(int tipz,
short dd,short md,short gd,
const char *nomdok,
const char *kodop,
int podr,
int kodol)
{
char strsql[2048];
struct stat work;
int kom=0;
static class iceb_tu_str imafim("");

if(tipz == 2)
 {
  iceb_t_soob(gettext("Иморт осуществляется только в приходные документы!"));
  return;
 }

VVOD VVOD1(2);
VVOD1.VVOD_SPISOK_add_ZAG(gettext("Импорт основных средств из файла"));
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите имя файла"));

naz:;

clear();

helstr(LINES-1,0,
"F1",gettext("помощь"),
"F2/+",gettext("загруз."),
"F3",gettext("просмотр"),
"F10",gettext("выход"),NULL);

if((kom=vvod1(&imafim,COLS-6,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
  return;
if(kom == ESC)
  return;

switch(kom)
 {
  case FK1:
    GDITE();
    iceb_t_pdoc("uos2_1_4.txt");
    clear();
    goto naz;

  case ENTER:
    if(imafim.ravno()[0] == '\0')
     {
      dirkatl(&imafim);
      goto naz;
     }
    break;

  case FK3:
   GDITE();
   if(stat(imafim.ravno(),&work) != 0)
    {
     sprintf(strsql,"%s !",gettext("Нет такого файла"));
     iceb_t_soob(strsql);
     goto naz;
    }
    vizred(imafim.ravno());
    goto naz;

  default:
    goto naz;
 }

if(stat(imafim.ravno(),&work) != 0)
 {
  sprintf(strsql,"%s !",gettext("Нет такого файла"));
  iceb_t_soob(strsql);
  goto naz;
 }

clear();

class iceb_tu_str strok("");

class iceb_fopen filimp;
if(filimp.start(imafim.ravno(),"r") != 0)
 return;

char imaftmp[64];
sprintf(imaftmp,"imp%d.tmp",getpid());

class iceb_fopen filtmp;
if(filtmp.start(imaftmp,"w") != 0)
 return;

printw("%s\n\n",gettext("Загружаем записи из файла"));
int razmer=0;
#define KOLKOL 21
class iceb_tu_str rek[KOLKOL];
OPSHET reksh;
SQL_str row;
class SQLCURSOR cur;
int ktoi=iceb_t_getuid();
int kolihoh=0;
int nom=0;
while(iceb_tu_fgets(&strok,filimp.ff) == 0)
 {
  
  razmer+=strlen(strok.ravno());
  if(strok.ravno()[0] == '#')
   continue;

  printw("%s",strok.ravno());
  strzag(LINES-1,0,work.st_size,razmer);

  for(nom=0; nom < KOLKOL; nom++)
   if(polen(strok.ravno(),&rek[nom],nom+1,'|') != 0)
    {
     fprintf(filtmp.ff,"%s",strok.ravno());
     fprintf(filtmp.ff,"#%s %d\n",gettext("Не найдено поле"),nom+1);
     kolihoh++;
     break;     
    }

  if(nom < KOLKOL)
   continue;

  /*проверяем счёт*/
  if(rek[8].getdlinna() > 1)
  if(prsh1(rek[8].ravno(),&reksh) != 0)
   {
    rek[8].new_plus("");
   }
  /*проверяем шифр затрат*/
  sprintf(strsql,"select kod from Uoshz where kod='%s'",rek[9].ravno_filtr());
  if(readkey(strsql) != 1)
   {
    rek[9].new_plus("");
   }

  /*проверяем шифр аналитического учёта*/
  sprintf(strsql,"select kod from Uoshau where kod='%s'",rek[10].ravno_filtr());
  if(readkey(strsql) != 1)
   {
    rek[10].new_plus("");
   }

  /*проверяем группу налогового учёта (шифр нормы амортотчислений)*/
  sprintf(strsql,"select kod from Uosgrup where kod='%s'",rek[11].ravno_filtr());
  if(readkey(strsql) != 1)
   {
    rek[11].new_plus("");
   }

  /*проверяем группу бухгалтерского учёта (шифр нормы амортотчислений)*/
  sprintf(strsql,"select kod from Uosgrup1 where kod='%s'",rek[15].ravno_filtr());
  if(readkey(strsql) != 1)
   {
    rek[15].new_plus("");
   }
  
/**************
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
*/

  /*проверяем есть ли такой инвентарный номер*/
  sprintf(strsql,"select naim from Uosin where innom=%d",rek[0].ravno_atoi());
  if(readkey(strsql,&row,&cur) == 1)
   {
    rek[0].new_plus(invnomer(1));
   }

  sprintf(strsql,"insert into Uosin (innom,god,naim,zaviz,pasp,model,zavnom,datvv,ktoz,vrem) \
values (%d,%d,'%s','%s','%s','%s','%s','%s',%d,%ld)",
  rek[0].ravno_atoi(),rek[1].ravno_atoi(),rek[2].ravno_filtr(),rek[3].ravno_filtr(),
  rek[4].ravno_filtr(),rek[5].ravno_filtr(),rek[6].ravno_filtr(),rek[7].ravno_filtr(),ktoi,time(NULL));
  if(sql_zapis(strsql,1,0) != 0)
   {
    kolihoh++;
    fprintf(filtmp.ff,"%s",strok.ravno());
    fprintf(filtmp.ff,"#%s Uosin\n",gettext("Ошибка записи в таблицу"));
    continue;  
   }
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

  class iceb_tu_str shetu; //Счет учета
  class iceb_tu_str hzt; //Шифр затрат
  class iceb_tu_str hau; //Шифр аналитического учета
  class iceb_tu_str hna; //Шифр нормы амортотчислений для налогового учёта
  float popkf; //Поправочный коэффициент для налогового учёта
  int soso; //Состояние объекта
  class iceb_tu_str nomz;  //Номерной знак
  short mz; short gz;  //дата записи
  class iceb_tu_str hnaby; //Шифр нормы амортотчислений для бух. учета
  float popkfby;  ///Поправочный коэффициент для бух. учета
    
  fprintf(fil.ff,"%s|%s|%s|%s|%s|%s|%s|%s|\
%s|%s|%s|%s|%f|%d|%s|%s|%f|\
%.2f|%.2f|%.2f|%.2f|\n",
  row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],
  rekin.shetu.ravno(),rekin.hzt.ravno(),rekin.hna.ravno(),rekin.hna.ravno(),rekin.popkf,rekin.soso,rekin.nomz.ravno(),rekin.hnaby.ravno(),rekin.popkfby,
  bs_nu,iznos_nu,bs_bu,iznos_bu);  

*********************************/
  
  sprintf(strsql,"insert into Uosinp (innom,mes,god,shetu,hzt,hau,hna,popkof,soso,nomz,ktoz,vrem,hnaby,popkofby) \
values (%d,%d,%d,'%s','%s','%s','%s',%f,%d,'%s',%d,%ld,'%s',%f)",
  rek[0].ravno_atoi(),md,gd,rek[8].ravno_filtr(),rek[9].ravno_filtr(),rek[10].ravno_filtr(),rek[11].ravno_filtr(),
  rek[12].ravno_atof(),rek[13].ravno_atoi(),rek[14].ravno_filtr(),ktoi,time(NULL),rek[15].ravno_filtr(),rek[16].ravno_atof());

  if(sql_zapis(strsql,1,0) != 0)
   {
    kolihoh++;
    fprintf(filtmp.ff,"%s",strok.ravno());
    fprintf(filtmp.ff,"#%s Uosinp\n",gettext("Ошибка записи в таблицу"));
    continue;  
   }

/*********************
strcpy(strsql,"CREATE TABLE Uosdok1 (\
datd	DATE not null default '0000-00-00',\
tipz	smallint not null default 0,\
podt	smallint not null default 0,\
innom   int unsigned not null default 0,\
nomd	varchar(20) not null default '',\
podr	smallint not null default 0,\
kodol   int not null default 0,\
kol     smallint not null default 0,\
bs	double(12,2) not null default 0,\
iz	double(12,2) not null default 0,\
kind	float not null default 0,\
kodop   varchar(20) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
bsby	double(12,2) not null default 0,\
izby    double(12,2) not null default 0,\
cena	double(14,2) not null default 0,\
shs     varchar(20) not null default '',\
unique(datd,nomd,innom),\
index(innom,datd))");

0  datd	  дата документа
1  tipz	  1-приход 2-расход
2  podt	  0-не подтверждена 1-подтверждена
3  innom  инвентарный номер
4  nomd	  номер документа
5  podr   подразделение
6  kodol  код ответственного лица
7  kol    количество : +1 приход -1 расход 0 изменение стоимости
8  bs	  балансовая стоимость для налогового учёта
9  iz	  износ для налогового учёта
10 kind	  коэффициент индексации
11 kodop  код операции
12 ktoz   кто записал
13 vrem   время записи
14 bsby	  балансовая стоимость для бух-учета
15 izby	  износ для бух-учета
16 cena   цена продажи
17 shs    счёт получения/списания
*/
  sprintf(strsql,"insert into Uosdok1 (datd,tipz,podt,innom,nomd,podr,kodol,kol,bs,iz,kind,kodop,ktoz,vrem,bsby,izby,cena,shs) \
values('%04d-%02d-%02d',%d,%d,%d,'%s',%d,%d,%d,%.2f,%.2f,%f,'%s',%d,%ld,%.2f,%.2f,%.2f,'%s')",
  gd,md,dd,tipz,0,rek[0].ravno_atoi(),
  nomdok,podr,kodol,1,rek[17].ravno_atof(),rek[18].ravno_atof(),0.,kodop,ktoi,time(NULL),
  rek[19].ravno_atof(),rek[20].ravno_atof(),0.,rek[8].ravno_filtr());
  if(sql_zapis(strsql,1,0) != 0)
   {
    kolihoh++;
    fprintf(filtmp.ff,"%s",strok.ravno());
    fprintf(filtmp.ff,"#%s Uosdok1\n",gettext("Ошибка записи в таблицу"));
    continue;  
   }


 }
strzag(LINES-1,0,work.st_size,work.st_size);

filtmp.end();
filimp.end();

unlink(imafim.ravno());

rename(imaftmp,imafim.ravno());

if(kolihoh == 0)
 iceb_t_soob(gettext("Все записи загружены"));
else
 {
  sprintf(strsql,"%s %d",gettext("Количество не загруженых записей"),kolihoh);
  iceb_t_soob(strsql);
 }
}
