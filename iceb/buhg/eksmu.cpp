/*$Id: eksmu.c,v 5.4 2013/11/24 08:23:03 sasa Exp $*/
/*05.09.2015	15.06.2013	Белых А.И.	eksmu.c
Экспорт остатков из материального учёта
*/
#include "buhg.h"

int eksmu(class spis_oth *oth)
{
int kom=0,kom1=0;
static class iceb_tu_str dataost("");
static class iceb_tu_str kodskl("");
static class iceb_tu_str shetuh("");
class iceb_tu_str kod("");
class iceb_tu_str naim("");

if(dataost.getdlinna() <= 1)
  dataost.plus_tek_dat();

class VVOD MENU(3);
class VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Экспорт остатков материалов"));

VV.VVOD_SPISOK_add_MD(gettext("Дата остатка.(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Код склада.....(,,)..."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт учёта.....(,,)..."));

VV.VVOD_SPISOK_add_data(dataost.ravno(),11);
VV.VVOD_SPISOK_add_data(kodskl.ravno(),128);
VV.VVOD_SPISOK_add_data(shetuh.ravno(),128);

naz:;

clear();
 
helstr(LINES-1,0,
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dataost.new_plus(VV.VVOD_SPISOK_return_data(0));
kodskl.new_plus(VV.VVOD_SPISOK_return_data(1));
shetuh.new_plus(VV.VVOD_SPISOK_return_data(2));

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

    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка складов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 2 :
      case -1:
        goto naz;
    
      case 1 :
        vibrek("Plansh",&shetuh);
        VV.data_plus(2,shetuh.ravno());
        break;

      case 0 :
        if(dirsklad(1,&kodskl,&naim) == 0)
          VV.data_z_plus(1,kod.ravno());
                 
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
SQL_str row,row1;
class SQLCURSOR cur,cur1;

sprintf(strsql,"select * from Kart");

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

class iceb_fopen filost;
char imafost[64];
sprintf(imafost,"eks%d.csv",getpid());
if(filost.start(imafost,"w") != 0)
 return(1);

class iceb_fopen filras;
char imafras[64];
sprintf(imafras,"eksr%d.lst",getpid());
if(filras.start(imafras,"w") != 0)
 return(1);

iceb_t_zageks(gettext("Экспорт остатков из материального учёта"),filost.ff);

fprintf(filost.ff,"#%s:%s\n",gettext("Дата остатка"),dataost.ravno());


iceb_t_zagolov(gettext("Экспорт остатков материалов"),filras.ff);
fprintf(filras.ff,"%s:%s\n",gettext("Организация"),iceb_t_get_pnk("00",1));
fprintf(filras.ff,"%s:%s\n",gettext("Дата остатка"),dataost.ravno());

if(kodskl.getdlinna() > 1)
 {
  fprintf(filost.ff,"#%s:%s\n",gettext("Код склада"),kodskl.ravno());
  fprintf(filras.ff,"%s:%s\n",gettext("Код склада"),kodskl.ravno());
 }
if(shetuh.getdlinna() > 1)
 {
  fprintf(filost.ff,"#%s:%s\n",gettext("Счёт учёта"),shetuh.ravno());
  fprintf(filras.ff,"%s:%s\n",gettext("Счёт учёта"),shetuh.ravno());
 }

fprintf(filost.ff,"#Расшифровка полей записи:\n\
#1  Код материалла\n\
#2  Наименование материала\n\
#3  0-цена c НДС 1-цена без НДС\n\
#4  Единица измерения\n\
#5  Счёт учета\n\
#6  Цена учета\n\
#7  Цена продажи для розничной торговли\n\
#8  Кратность (количество в упаковке)\n\
#9  Н.Д.С\n\
#10 Фасовка (вес одной упаковки)\n\
#11 Код тары\n\
#12 Дата ввода в эксплуатацию (для малоценки) / Дата регистрации препарата для медикаментов\n\
#13 Инвентарный номер (для малоценки) / Серия для медикаментов\n\
#14 Регистрационный номер документа для мед. препарата/дата ввода в эксплуатацию для инвентарного номера\n\
#15 Номер заказа\n\
#16 Конечная дата годности товара\n\
#17 Количество\n");
  



fprintf(filras.ff,"\
------------------------------------------------------------------------------------------\n");
fprintf(filras.ff,gettext("\
Склад|Карточка|Код мат.|Наименование материала|Счёт учёта|Един.изм.|Цена учёта|Количество|\n"));

/*****  
12345 12345678 12345678 1234567890123456789012 1234567890 123456789 1234567890 1234567890
*********/
fprintf(filras.ff,"\
------------------------------------------------------------------------------------------\n");
double ostatok=0.;

int kolstr1=0;
class ostatokt ost;

while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(proverka(kodskl.ravno(),row[0],0,0) != 0)
    continue;

  if(proverka(shetuh.ravno(),row[5],0,0) != 0)
    continue;

  ostkar(1,1,gost,dost,most,gost,atoi(row[0]),atoi(row[1]),&ost);
  if(ost.ostg[3] <= 0.00000009)
   continue;   
  ostatok=ost.ostg[3];
  sprintf(strsql,"select naimat from Material where kodm=%s",row[2]);
  if(readkey(strsql,&row1,&cur1) == 1)
   naim.new_plus(row1[0]);
  else
   naim.new_plus("");

/*
0  sklad    Код склада
1  nomk     Номер карточки

2  kodm     Код материалла
3  mnds     0-цена c НДС 1-цена без НДС
4  ei       Единица измерения
5  shetu    Счёт учета
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
  printw("%6s %6s %6s %s\n",row[0],row[1],row[2],naim.ravno());
  
  fprintf(filost.ff,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%f|\n",
  row[2],naim.ravno(),row[3],row[4],row[5],row[6],row[7],row[8],row[9],row[10],row[11],row[14],row[15],row[16],row[17],row[18],ostatok);
  
  fprintf(filras.ff,"%-5s|%-8s|%-8s|%-*.*s|%-*s|%-*s|%10s|%10.10g|\n",
  row[0],
  row[1],
  row[2],
  iceb_tu_kolbait(22,naim.ravno()),
  iceb_tu_kolbait(22,naim.ravno()),
  naim.ravno(),
  iceb_tu_kolbait(10,row[5]),
  row[5],
  iceb_tu_kolbait(9,row[4]),
  row[4],
  row[6],
  ostatok);
 }
 
fprintf(filras.ff,"\
------------------------------------------------------------------------------------------\n");

podpis(filras.ff);
filost.end();
filras.end();

oth->spis_imaf.plus(imafras);
oth->spis_naim.plus(gettext("Экспорт остатков материалов (распечатка)"));

for(int nom=0; nom < oth->spis_imaf.kolih(); nom++)
  iceb_t_ustpeh(oth->spis_imaf.ravno(nom),3);

oth->spis_imaf.plus(imafost);
oth->spis_naim.plus(gettext("Экспорт остатков материалов (файл для загрузки)"));



return(0);
}
