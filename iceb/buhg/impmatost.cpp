/*$Id: impmatost.c,v 5.4 2014/07/31 07:09:16 sasa Exp $*/
/*05.09.2015	15.06.2013	Белых А.И.	impmatost.c
Импорт остатков в документ материального учёта
*/

#include        <sys/stat.h>
#include "buhg.h"

void impmatost(int tipz,
int sklad,
short dd,short md,short gd,
const char *nomdok)
{
char strsql[2048];
struct stat work;
int kom=0;
static class iceb_tu_str imafim("");

VVOD VVOD1(2);
VVOD1.VVOD_SPISOK_add_ZAG(gettext("Импорт материалов из файла"));
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите имя файла с материалами"));

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
    iceb_t_pdoc("matu2_1_2_1.txt");
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
#define KOLKOL 17
class iceb_tu_str rek[KOLKOL];
OPSHET reksh;
SQL_str row;
class SQLCURSOR cur;
int kodmat=0;
int ktoi=iceb_t_getuid();
int nomer_kart=0;
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
  if(rek[4].getdlinna() > 1)
   {
    if(prsh1(rek[4].ravno(),&reksh) != 0)
     {
      fprintf(filtmp.ff,"%s",strok.ravno());
      fprintf(filtmp.ff,"#%s %s\n",gettext("Не подходит счёт учёта"),rek[4].ravno());
      kolihoh++;
      continue;  
     }
   }
  else
   {
    fprintf(filtmp.ff,"%s",strok.ravno());
    fprintf(filtmp.ff,"#%s\n",gettext("Не введён счёт учёта"));
    kolihoh++;
    continue;  
   }

  /*проверяем единицу измерения*/
  if(rek[3].getdlinna() > 1)
   {
    sprintf(strsql,"select kod from Edizmer where kod='%s'",rek[3].ravno());
    if(readkey(strsql,&row,&cur) != 1)
     {
      sprintf(strsql,"%s %s!",gettext("Не найдена единица измерения"),rek[3].ravno());
      iceb_t_soob(strsql);
      fprintf(filtmp.ff,"#%s\n",strsql);
      kolihoh++;
      continue;
     }
   }
  else
   {
    fprintf(filtmp.ff,"%s",strok.ravno());
    fprintf(filtmp.ff,"#%s\n",gettext("Не введена единица измерения"));
    kolihoh++;
    continue;  
   }
/*
0  sklad    Код склада
1  nomk     Номер карточки

2  kodm     Код материалла
3  mnds     0-цена c НДС 1-цена без НДС
4  ei       Единица измерения
5  shetu    Счет учета
6  cena     Цена учета
7  cenap    Цена продажи для розничной торговли
8  krat     Кратность
9  nds      Н.Д.С
10 fas      Фасовка
11 kodt     Код тары
12 ktoi     Кто записал
13 vrem     Время записи
14 datv     Дата ввода в эксплуатацию (для малоценки) / Дата регистрации препарата для медикаментов
15 innom    Инвентарный номер (для малоценки) / Серия для медикаментов
16 rnd      Регистрационный номер документа для мед. препарата/дата ввода в эксплуатацию для инвентарного номера
17 nomz     Номер заказа
18 dat_god  Конечная дата годности товара
*/
  
//  fprintf(filost.ff,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%f|\n",
//  row[2],naim.ravno(),row[3],row[4],row[5],row[6],row[7],row[8],row[9],row[10],row[11],row[14],row[15],row[16],row[17],row[18],ostatok);

  /*проверяем есть ли материал в списке материалов*/
  sprintf(strsql,"select kodm from Material where naimat='%s'",rek[1].ravno());
  if(readkey(strsql,&row,&cur) == 1)
   {
    kodmat=atoi(row[0]);
   }
  else
   {
    class lock_tables kkk("LOCK TABLES Material WRITE,Uslugi READ");
    /*проверяем есть ли в списке материалов код материала если есть нужно загрузить с другим кодом*/
    if(rek[0].ravno_atoi() != 0)
     {
      kodmat=rek[0].ravno_atoi();
      sprintf(strsql,"select kodm from Material where kodm=%d",rek[0].ravno_atoi());
      if(readkey(strsql) == 1)
       {
        kodmat=nomkm();
       }
      else
       {
        sprintf(strsql,"select kodus from Uslugi where kodus=%d",kodmat);
        if(readkey(strsql) == 1)
         {
          kodmat=nomkm();
         }
        
       }
     }
    else
      kodmat=nomkm();

    sprintf(strsql,"insert into Material (kodm,naimat,ei,ktoz,vrem) \
values (%d,'%s','%s',%d,%ld)",
    kodmat,rek[1].ravno_filtr(),rek[3].ravno_filtr(),ktoi,time(NULL));
    if(sql_zapis(strsql,1,0) != 0)
     {
      fprintf(filtmp.ff,"%s",strok.ravno());
      fprintf(filtmp.ff,"#%s Material\n",gettext("Ошибка записи в таблицу"));
      kolihoh++;
      continue;
     }
   }  
  nomer_kart=0;
  if(tipz == 1) /*для приходного документа определяем в какую карточку запись. Для расходного без привязки к карточке*/
   {
    /*проверяем есть ли нужная карточка*/
    //  index (kodm,sklad,shetu,cena,ei,nds,cenap,krat,mnds),
    sprintf(strsql,"select nomk from Kart where kodm=%d and sklad=%d and shetu='%s' and cena=%s and ei='%s' and nds=%s and cenap=%s and krat=%s and mnds=%s",
    kodmat,sklad,rek[4].ravno(),rek[5].ravno(),rek[3].ravno_filtr(),rek[8].ravno(),rek[6].ravno(),rek[7].ravno(),rek[2].ravno());
    if(readkey(strsql,&row,&cur) > 0)
     {
      nomer_kart=atoi(row[0]);
      /*Если катрочка есть то может быт и запись в документе проверяем и увеличиваем количество если есть*/
      sprintf(strsql,"select kolih from Dokummat1 where sklad=%d and nomkar=%d and datd='%04d-%02d-%02d' and nomd='%s' and kodm=%d and tipz=%d",
      sklad,nomer_kart,gd,md,dd,nomdok,kodmat,tipz);
      if(readkey(strsql,&row,&cur) == 1)
       {
        double kolih=atof(row[0])+rek[16].ravno_atof();
        sprintf(strsql,"update Dokummat1 set kolih=%.10g where sklad=%d and nomkar=%d and datd='%04d-%02d-%02d' and nomd='%s' and kodm=%d and tipz=%d",
        kolih,sklad,nomer_kart,gd,md,dd,nomdok,kodmat,tipz);
        sql_zapis(strsql,1,0);
        continue;
       }      
     }
    else
     {
      class lock_tables kkk("LOCK TABLES Kart WRITE");
      nomer_kart=nomkr(sklad);
      
      sprintf(strsql,"insert into Kart (sklad,nomk,kodm,mnds,ei,shetu,cena,cenap,\
krat,nds,fas,kodt,\
ktoi,vrem,\
datv,innom,rnd,nomz,dat_god)\
 values (%d,%d,%d,%s,'%s','%s',%s,%s,\
%s,%s,%s,%s,\
%d,%ld,\
'%s','%s','%s','%s','%s')",
      sklad,nomer_kart,kodmat,rek[2].ravno(),rek[3].ravno_filtr(),rek[4].ravno(),rek[5].ravno(),rek[6].ravno(),
      rek[7].ravno(),rek[8].ravno(),rek[9].ravno(),rek[10].ravno(),
      ktoi,time(NULL),
      rek[11].ravno(),rek[12].ravno(),rek[13].ravno(),rek[14].ravno(),rek[15].ravno());

      if(sql_zapis(strsql,1,0) != 0)
       {
        fprintf(filtmp.ff,"%s",strok.ravno());
        fprintf(filtmp.ff,"#%s Kart\n",gettext("Ошибка записи в таблицу"));
        kolihoh++;
        continue;
       }
     }
   }
/**************
  strcpy(strsql,"CREATE TABLE Dokummat1 (\
datd DATE not null default '0000-00-00',\
sklad  smallint not null  default 0 references Sklad (kod),\
nomd   char(16) not null default '',\
nomkar int not null default 0,\
kodm   int not null  default 0 references Material (kodm),\
kolih  double(16,6) not null default 0,\
cena   double(16,6) not null default 0,\
ei     varchar(8) not null default '' references Edizmer (kod),\
voztar smallint not null default 0,\
nds    float(2) not null default 0,\
mnds   smallint not null default 0,\
ktoi   smallint unsigned not null default 0,\
vrem   int unsigned not null default 0,\
tipz   smallint not null default 0,\
nomkarp int not null default 0,\
shet   varchar(12) not null default '',\
dnaim  varchar(70) not null default '',\
nomz   varchar(20) not null default '',\
index (sklad,nomd,kodm,nomkar),\
index (sklad,nomkar),\
index (kodm,sklad),\
index (datd,sklad,nomd))");

0  datd   Дата документа
1  sklad  Склад
2  nomd   Номер документа
3  nomkar Номер карточки
4  kodm   Код материалла
5  kolih  Количество
6  cena   Цена
7  ei     Единица измерения
8  voztar 1-Метка возвратной тары
9  nds    НДС
10 mnds   0- цена без НДС 1-цена с НДС в том числе
11 ktoi   Кто записал
12 vrem   Время записи
13 tipz   1-приход 2-расход
14 nomkarp номер карточки парного документа
15 shet   счёт списания/приобретения
16 dnaim  Дополнительное наименование материалла
17 nomz   Номер заказа
*/
  /*записываем в документ*/
  sprintf(strsql,"insert into Dokummat1 (datd,sklad,nomd,nomkar,kodm,kolih,cena,ei,nds,mnds,ktoi,vrem,tipz,shet) \
values('%04d-%02d-%02d',%d,'%s',%d,%d,%s,%s,'%s',%s,%s,%d,%ld,%d,'%s')",
  gd,md,dd,sklad,nomdok,nomer_kart,kodmat,rek[16].ravno(),rek[5].ravno(),rek[3].ravno_filtr(),rek[8].ravno(),rek[2].ravno(),
  ktoi,time(NULL),tipz,rek[4].ravno());

  if(sql_zapis(strsql,1,0) != 0)
   {
    fprintf(filtmp.ff,"%s",strok.ravno());
    fprintf(filtmp.ff,"#%s Dokummat1\n",gettext("Ошибка записи в таблицу"));
    kolihoh++;
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
