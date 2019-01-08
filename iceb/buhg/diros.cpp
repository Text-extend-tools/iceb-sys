/* $Id: diros.c,v 5.48 2013/09/26 09:43:30 sasa Exp $ */
/*24.01.2016	13.08.1999	Белых А.И.	diros.c
Просмотр списка основных средств
Если вернули 0 - выбрали
	     1 - нет
*/
#include <math.h>
#include	<errno.h>
#include	"buhg.h"
#include "poiinpd.h"
#include "bsizt.h"

/*Реквизиты поиска*/
class diros_poi
 {
  public:
  class iceb_tu_str naim;
  class iceb_tu_str zaviz;
  class iceb_tu_str pasp;
  class iceb_tu_str model;
  class iceb_tu_str godv;
  class iceb_tu_str zavnom;
  class iceb_tu_str datavv;
  class iceb_tu_str shetu;
  class iceb_tu_str hifrz;
  class iceb_tu_str hifrau;
  class iceb_tu_str hifrnao;
  class iceb_tu_str hifrnao_bu;
  class iceb_tu_str koeff;
  class iceb_tu_str koeffby;
  class iceb_tu_str nomz;
  class iceb_tu_str kodol;
  class iceb_tu_str podraz;
  int kolstr;  
  diros_poi()
   {
    kolstr=0;
    clear_data();
   }

  void clear_data()
   {
    naim.new_plus("");
    zaviz.new_plus("");
    pasp.new_plus("");
    model.new_plus("");
    godv.new_plus("");
    zavnom.new_plus("");
    datavv.new_plus("");
    shetu.new_plus("");
    hifrz.new_plus("");
    hifrau.new_plus("");
    hifrnao.new_plus("");
    hifrnao_bu.new_plus("");
    koeff.new_plus("");
    koeffby.new_plus("");
    nomz.new_plus("");
    kodol.new_plus("");
    podraz.new_plus("");
   }

 };
 
void		uospoisk(class diros_poi *data,int *mp);
int		uosprow(char[],char[],char[],char[],char[],char[],char[],char[],\
		char[],char[],char[],char[],char[],char[],char[],char[],SQL_str);
void		raspoi(SQLCURSOR*,long,int,class diros_poi*);
void		shpos(int metka,int mp,class diros_poi *data);
int		uosprov(class diros_poi *,SQL_str row);


int diros(long *in,class iceb_tu_str *naimm,
int metka)//0-просмотр 1-просмотр с выбором
{
int		prc;
int             i;
SQL_str         row;
class iceb_t_mstr spis_menu(0);
char		strsql[1024];
long		pozz,pozz1;
long		kolstr;
short           x=0,y=3;
int		kom,K;
long		in1=0;
class iceb_tu_str naim1("");
short		mspz;
int		podd,kodotl;
int		mp; /*0-поиска нет 1-поиск*/
class diros_poi data;
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
GDITE();
//Создаем класс меню
VVOD VV(0);

mp=0;



naim1.new_plus(naimm->ravno());

pozz=kom=0;

naz1:;

clear();
short           kls=LINES-6;


K=0;
sprintf(strsql,"select * from Uosin order by innom asc");

class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
data.kolstr=kolstr; 
if(kolstr == 0)
 {
lll:;
  shpos(metka,mp,&data);
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == FK2 || K == FK5 || K == ENTER)
     goto lll;

  if(K == KEY_RESIZE)
   goto naz1;
   
  goto vper;
 }

naz:;
GDITE();
clear();

if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;

cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
spis_menu.spis_str.free_class();
for(i=0; i < kls ;)
 {

  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;

  if(mp == 1)
  if(uosprov(&data,row) != 0)
   continue;

  sprintf(strsql,"%4s|%-*.*s|%s",
  row[0],
  iceb_tu_kolbait(30,row[2]),iceb_tu_kolbait(30,row[2]),row[2],row[6]);

  spis_menu.spis_str.plus(strsql);
  i++;

  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
   mspz=1;
 }

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i == 0)
 {
  if(kolstr > 0 && pozz > 0)
   {
    pozz--;
    goto naz;
   }
  goto lll;
 }
if(i <= kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));


shpos(metka,mp,&data);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,"",0);

if(kom == -7)
 goto naz1;
  
if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  for(i=0; i < mspz  ; )
   {
    pozz++;

    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;

    if(mp == 1)
    if(uosprov(&data,row) != 0)
      continue;
    i++;
   }
  kom=kls;
  goto naz;
 }
if(kom == -2) /*Стрелка вверх*/
 {
  for(i=0; i < 1 ; )
   {

    pozz--;

    if(pozz <= 0)
     {
      beep();
      pozz=0;
      goto naz;
     }

    cur.poz_cursor(pozz);
    cur.read_cursor(&row);

    if(mp == 1)
    if(uosprov(&data,row) != 0)
      continue;
    i++;
   }

  goto naz;
 }
 
if(kom == -5) /*Страница вниз*/
 {
  if(pozz + kls == kolstr)
   {
    pozz=kolstr-1;
    beep();
    goto naz;  
   }
  if(i == kls)
    pozz+=pozz1;
  else
    beep();
  if(pozz >= kolstr)
   {
    pozz=kolstr-pozz1;
    beep();
    goto naz;  
   }
  goto naz;

 }

if(kom == -4) /*Страница вверх*/
 {
  for(i=0; i<kls ; )
   {
    pozz--;

    if(pozz <= 0)
     {
      beep();
      pozz=0;
      goto naz;
     }

    cur.poz_cursor(pozz);
    cur.read_cursor(&row);

    if(mp == 1)
    if(uosprov(&data,row) != 0)
     continue;
    i++;
   }
  goto naz;
 }

if(kom >= 0)
 {
  if(SRAV(gettext("Конец списка"),spis_menu.spis_str.ravno(kom),0) == 0)
     goto naz;
  
  if(pozz+kom >= kolstr)
   goto naz;
 
  POLE(spis_menu.spis_str.ravno(kom),strsql,1,'|');
  in1=atol(strsql);
  polen(spis_menu.spis_str.ravno(kom),&naim1,2,'|');
 
 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return(1);

  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("uos4.txt");
   clear();
   if(kolstr > 0)
     goto naz;
   else
     goto lll;

  case ENTER : /*Выход с выбором*/
    if(metka == 1)
     {
      *in=in1;
      naimm->new_plus(naim1.ravno());

      return(0);
     }
    else
     {
      GDITE();
      kodotl=podd=0;
      poiin(in1,dt,mt,gt,&podd,&kodotl);
      VV.VVOD_delete();
      vos(&VV,in1,2,dt,mt,gt,podd,kodotl);
      goto naz;
     }     
    
  case FK2: /*Просмотр карточки материалла*/
     kodotl=podd=0;
     poiin(in1,dt,mt,gt,&podd,&kodotl);
     VV.VVOD_delete();
     vos(&VV,in1,2,dt,mt,gt,podd,kodotl);
     goto naz;

  case FK4: /*Поиск*/
     pozz=0;           
     uospoisk(&data,&mp);
     goto naz;

  case FK5: /*Распечатка*/
     raspoi(&cur,kolstr,mp,&data);

     goto naz;
     
  default:
   if(kolstr > 0)
     goto naz;
   else
     goto lll;
 }
}

/************/
/*Шапка меню*/
/************/

void		shpos(int metka,int mp,class diros_poi *data)
{
int		X,Y;
move(0,0);
printw("%s. %s:%d",gettext("Просмотр списка основных средств"),gettext("Количество"),data->kolstr);
if(metka == 1)
 {
  move(1,0);
  printw(gettext("Виберите нужное и нажмите Enter"));
 }
move(2,0);
printw("    1                 2                  3");

move(Y=2,X=COLS-20);
printw(gettext("Расшифровка полей:"));
move(++Y,X);
printw(gettext("1-инвентарный номер"));
move(++Y,X);
printw(gettext("2-наименование"));
move(++Y,X);
printw(gettext("3-зав.номер"));

if(mp == 1)
 {
  attron(A_BLINK);
  move(Y+=2,X);
  printw(gettext("Поиск:"));
  int kolsimv=0;
  if(data->naim.getdlinna() > 1)
   {
    move(++Y,X);
    kolsimv=COLS-X-iceb_tu_strlen(gettext("Наим.")-1);
    printw("%s:%.*s",gettext("Наим."),iceb_tu_kolbait(kolsimv,data->naim.ravno()),data->naim.ravno());
   }
  if(data->zaviz.getdlinna() > 1)
   {
    kolsimv=COLS-X-iceb_tu_strlen(gettext("Завод")-1);
    move(++Y,X);
    printw("%s:%.*s",gettext("Завод"),iceb_tu_kolbait(kolsimv,data->zaviz.ravno()),data->zaviz.ravno());
   }
  if(data->pasp.getdlinna() > 1)
   {
    kolsimv=COLS-X-iceb_tu_strlen(gettext("Пасп.")-1);
    move(++Y,X);
    printw("%s:%.*s",gettext("Пасп."),iceb_tu_kolbait(kolsimv,data->pasp.ravno()),data->pasp.ravno());
   }
  if(data->model.getdlinna() > 1)
   {
    kolsimv=COLS-X-iceb_tu_strlen(gettext("Мод.")-1);
    move(++Y,X);
    printw("%s:%.*s",gettext("Мод."),iceb_tu_kolbait(kolsimv,data->model.ravno()),data->model.ravno());
   }
  if(data->godv.getdlinna() > 1)
   {
    kolsimv=COLS-X-iceb_tu_strlen(gettext("Год в.")-1);
    move(++Y,X);
    printw("%s:%.*s",gettext("Год в."),iceb_tu_kolbait(kolsimv,data->godv.ravno()),data->godv.ravno());
   }
  if(data->zavnom.getdlinna() > 1)
   {
    kolsimv=COLS-X-iceb_tu_strlen(gettext("Зав.N")-1);
    move(++Y,X);
    printw("%s:%.*s",gettext("Зав.N"),iceb_tu_kolbait(kolsimv,data->zavnom.ravno()),data->zavnom.ravno());
   }
  if(data->datavv.getdlinna() > 1)
   {
    kolsimv=COLS-X-iceb_tu_strlen(gettext("Дата")-1);
    move(++Y,X);
    printw("%s:%.*s",gettext("Дата"),iceb_tu_kolbait(kolsimv,data->datavv.ravno()),data->datavv.ravno());
   }
  if(data->shetu.getdlinna() > 1)
   {
    kolsimv=COLS-X-iceb_tu_strlen(gettext("Счёт")-1);
    move(++Y,X);
    printw("%s:%.*s",gettext("Счёт"),iceb_tu_kolbait(kolsimv,data->shetu.ravno()),data->shetu.ravno());
   }
  if(data->hifrz.getdlinna() > 1)
   {
    kolsimv=COLS-X-iceb_tu_strlen(gettext("Ш.зат.")-1);
    move(++Y,X);
    printw("%s:%.*s",gettext("Ш.зат."),iceb_tu_kolbait(kolsimv,data->hifrz.ravno()),data->hifrz.ravno());
   }
  if(data->hifrau.getdlinna() > 1)
   {
    kolsimv=COLS-X-iceb_tu_strlen(gettext("Ш.ан.у.")-1);
    move(++Y,X);
    printw("%s:%.*s",gettext("Ш.ан.у."),iceb_tu_kolbait(kolsimv,data->hifrau.ravno()),data->hifrau.ravno());
   }
  if(data->hifrnao.getdlinna() > 1)
   {
    kolsimv=COLS-X-iceb_tu_strlen(gettext("Ш.н.а.о")-1);
    move(++Y,X);
    printw("%s:%.*s",gettext("Ш.н.а.о"),iceb_tu_kolbait(kolsimv,data->hifrnao.ravno()),data->hifrnao.ravno());
   }
  if(data->hifrnao_bu.getdlinna() > 1)
   {
    kolsimv=COLS-X-iceb_tu_strlen(gettext("Ш.н.а.о")-1);
    move(++Y,X);
    printw("%s:%.*s",gettext("Ш.н.а.о"),iceb_tu_kolbait(kolsimv,data->hifrnao_bu.ravno()),data->hifrnao_bu.ravno());
   }
  if(data->koeff.getdlinna() > 1)
   {
    kolsimv=COLS-X-iceb_tu_strlen(gettext("Коэф.ну")-1);
    move(++Y,X);
    printw("%s:%.*s",gettext("Коэф.ну"),iceb_tu_kolbait(kolsimv,data->koeff.ravno()),data->koeff.ravno());
   }
  if(data->koeffby.getdlinna() > 1)
   {
    kolsimv=COLS-X-iceb_tu_strlen(gettext("Коэф.бу")-1);
    move(++Y,X);
    printw("%s:%.*s",gettext("Коэф.бу"),iceb_tu_kolbait(kolsimv,data->koeffby.ravno()),data->koeffby.ravno());
   }
  if(data->nomz.getdlinna() > 1)
   {
    kolsimv=COLS-X-iceb_tu_strlen(gettext("Ном.з.")-1);
    move(++Y,X);
    printw("%s:%.*s",gettext("Ном.з."),iceb_tu_kolbait(kolsimv,data->nomz.ravno()),data->nomz.ravno());
   }
  if(data->kodol.getdlinna() > 1)
   {
    kolsimv=COLS-X-iceb_tu_strlen(gettext("М.отв.")-1);
    move(++Y,X);
    printw("%s:%.*s",gettext("М.отв."),iceb_tu_kolbait(kolsimv,data->kodol.ravno()),data->kodol.ravno());
   }
  if(data->podraz.getdlinna() > 1)
   {
    kolsimv=COLS-X-iceb_tu_strlen(gettext("Подр.")-1);
    move(++Y,X);
    printw("%s:%.*s",gettext("Подр."),iceb_tu_kolbait(kolsimv,data->podraz.ravno()),data->podraz.ravno());
   }

  attroff(A_BLINK);
 }

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2",gettext("карточка"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F10",gettext("выход"),NULL);
}

/******************/
/*Реквизиты поиска*/
/*******************/
/*Если вернули 0 ищем 
               1 нет
*/               
void		uospoisk(class diros_poi *data,int *mp) //0-поиск не задан 1-задан
{
int		kom,kom1;
char		strsql[512];
class iceb_tu_str kod("");
class iceb_tu_str nm("");
int		poz,komv;
class iceb_tu_str kodn("");
class iceb_tu_str naim("");
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
clear();
VVOD VV(0);
VVOD MENU(3);

*mp=0;
VV.VVOD_SPISOK_add_ZAG(gettext("Поиск основных средств"));
VV.VVOD_SPISOK_add_ZAG(gettext("Поиск производится по любой комбинации введенных реквизитов"));
sprintf(strsql,"%s %d.%d%s",gettext("Текущая дата"),mt,gt,
gettext("г."));
VV.VVOD_SPISOK_add_ZAG(strsql);

VV.VVOD_SPISOK_add_data(data->naim.ravno(),128);//0
VV.VVOD_SPISOK_add_data(data->zaviz.ravno(),128);//1
VV.VVOD_SPISOK_add_data(data->pasp.ravno(),128);//2
VV.VVOD_SPISOK_add_data(data->model.ravno(),128);//3
VV.VVOD_SPISOK_add_data(data->godv.ravno(),5);//4
VV.VVOD_SPISOK_add_data(data->zavnom.ravno(),128);//5
VV.VVOD_SPISOK_add_data(data->datavv.ravno(),11);//6
VV.VVOD_SPISOK_add_data(data->shetu.ravno(),128);//7
VV.VVOD_SPISOK_add_data(data->hifrz.ravno(),128);//8
VV.VVOD_SPISOK_add_data(data->hifrau.ravno(),128);//9
VV.VVOD_SPISOK_add_data(data->hifrnao.ravno(),128);//10
VV.VVOD_SPISOK_add_data(data->koeff.ravno(),32);//11
VV.VVOD_SPISOK_add_data(data->koeffby.ravno(),32);//12
VV.VVOD_SPISOK_add_data(data->nomz.ravno(),64);//13
VV.VVOD_SPISOK_add_data(data->kodol.ravno(),128);//14
VV.VVOD_SPISOK_add_data(data->podraz.ravno(),128);//15
VV.VVOD_SPISOK_add_data(data->hifrnao_bu.ravno(),128);//16

VV.VVOD_SPISOK_add_MD(gettext("Наименование...................(,,)."));//0
VV.VVOD_SPISOK_add_MD(gettext("Завод изготовитель.................."));//1
VV.VVOD_SPISOK_add_MD(gettext("Паспорт/чертеж N...................."));//2
VV.VVOD_SPISOK_add_MD(gettext("Модель/тип/марка...................."));//3
VV.VVOD_SPISOK_add_MD(gettext("Год выпуска........................."));//4
VV.VVOD_SPISOK_add_MD(gettext("Заводской номер....................."));//5
VV.VVOD_SPISOK_add_MD(gettext("Дата ввода в эксплуатацию..........."));//6
VV.VVOD_SPISOK_add_MD(gettext("Счёт учета.....................(,,)."));//7
VV.VVOD_SPISOK_add_MD(gettext("Шифр затрат амортиз. отч. .....(,,)."));//8
VV.VVOD_SPISOK_add_MD(gettext("Шифр аналитического учета......(,,)."));//9
VV.VVOD_SPISOK_add_MD(gettext("Группа налоговог учёта........ (,,)."));//10
VV.VVOD_SPISOK_add_MD(gettext("Поп-ный коэффициент н.учет.........."));//11
VV.VVOD_SPISOK_add_MD(gettext("Поп-ный коэф. бух. учет............."));//12
VV.VVOD_SPISOK_add_MD(gettext("Номерной знак......................."));//13
VV.VVOD_SPISOK_add_MD(gettext("Код ответственного лица........(,,)."));//14
VV.VVOD_SPISOK_add_MD(gettext("Подразделение..................(,,)."));//15
VV.VVOD_SPISOK_add_MD(gettext("Группа бухгалтерского учёта....(,,)."));//16

naz:;

helstr(LINES-1,0,"F2/+",gettext("поиск"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,0,-1,-1);

data->naim.new_plus(VV.VVOD_SPISOK_return_data(0));
data->zaviz.new_plus(VV.VVOD_SPISOK_return_data(1));
data->pasp.new_plus(VV.VVOD_SPISOK_return_data(2));
data->model.new_plus(VV.VVOD_SPISOK_return_data(3));
data->godv.new_plus(VV.VVOD_SPISOK_return_data(4));
data->zavnom.new_plus(VV.VVOD_SPISOK_return_data(5));
data->datavv.new_plus(VV.VVOD_SPISOK_return_data(6));
data->shetu.new_plus(VV.VVOD_SPISOK_return_data(7));
data->hifrz.new_plus(VV.VVOD_SPISOK_return_data(8));
data->hifrau.new_plus(VV.VVOD_SPISOK_return_data(9));
data->hifrnao.new_plus(VV.VVOD_SPISOK_return_data(10));
data->koeff.new_plus(VV.VVOD_SPISOK_return_data(11));
data->koeffby.new_plus(VV.VVOD_SPISOK_return_data(12));
data->nomz.new_plus(VV.VVOD_SPISOK_return_data(13));
data->kodol.new_plus(VV.VVOD_SPISOK_return_data(14));
data->podraz.new_plus(VV.VVOD_SPISOK_return_data(15));
data->hifrnao_bu.new_plus(VV.VVOD_SPISOK_return_data(16));

class iceb_tu_str kod_v("");
class iceb_tu_str naim_v("");
switch(kom)
 {
  case FK10:
  case ESC:
    *mp=0;
    return;

  case FK2:
  case PLU:
    *mp=1;
    return;
    
  case FK3: 

   
    MENU.VVOD_delete();

    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка шифров аналитического учёта"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка шифров норм амортотчислений нал. учёт"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка подразделений"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка мат.-ответственных"));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка шифров затрат"));//4
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка шифров норм амортотчислений бух. учёт"));//5
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка cчетов"));//6
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//7
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0); 


    switch(kom1)
     {

      case 0: 
        kod.new_plus("");
        nm.new_plus("");
        poz=komv=0;
        if(dirmat1("Uoshau",&kod,&nm,LINES-5,0,2,1,&poz,&komv,"","\0") == 0)
         VV.data_z_plus(9,kod.ravno());     
        break;

      case 1: 
        if(diruosgrnu(1,&kod_v,&naim_v) == 0)
          VV.data_z_plus(10,kod_v.ravno());     
        break;

      case 2: 
       if(diruospod(1,&kod_v,&naim_v,0) == 0)
         VV.data_z_plus(15,kod_v.ravno());     
        break;

      case 3: 

        if(dirmatot(1,&kod_v,&naim_v,0) == 0)
         VV.data_z_plus(14,kod_v.ravno());     
        break;

      case 4: 
     
        kod.new_plus("");
        nm.new_plus("");
        poz=komv=0;
        if(dirmat1("Uoshz",&kod,&nm,LINES-5,0,2,1,&poz,&komv,"","\0") == 0)
         VV.data_z_plus(8,kod.ravno());     
        break;

      case 5: 
        if(diruosgrnu(1,&kod_v,&naim_v) == 0)
          VV.data_z_plus(16,kod_v.ravno());     

        break;

      case 6: 
        kodn.new_plus("");
        naim.new_plus("");     
        if(dirshet(&kodn,&naim,1) == 0)
         VV.data_z_plus(7,kodn.ravno());     
        break;
     }
     
    goto naz;

  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();
    goto naz;

  default:
    goto naz;
 }



}

/*************************/
/*Проверка условий поиска*/
/**************************/

int		uosprov(class diros_poi *data,
SQL_str row) 
{
short		mm;
long		in;
int		poddz,kodotl;

short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

if(proverka(data->naim.ravno(),row[2],4,0) != 0)
 return(1);
/************
if(data->naim.getdlinna() > 1 && strstrm(row[2],data->naim.ravno()) == 0)
 return(1);
***************/
if(data->zaviz.getdlinna() > 1 && strstrm(row[3],data->zaviz.ravno()) == 0)
 return(2);

if(data->pasp.getdlinna() > 1 && strstrm(row[4],data->pasp.ravno()) == 0)
 return(3);

if(data->model.getdlinna() > 1 && strstrm(row[5],data->model.ravno()) == 0)
 return(4);

if(data->godv.getdlinna() > 1 && strstrm(row[2],data->godv.ravno()) == 0)
 return(5);

if(data->zavnom.getdlinna() > 1 && strstrm(row[6],data->zavnom.ravno()) == 0)
 return(6);

if(data->datavv.getdlinna() > 1)
 if(sravmydat(data->datavv.ravno(),row[7]) != 0)
  return(7);

if(data->shetu.getdlinna() > 1 || data->hifrz.getdlinna() > 1 || data->hifrau.getdlinna() > 1 ||\
data->hifrnao.getdlinna() > 1 || data->hifrnao_bu.getdlinna() > 1 || data->koeff.getdlinna() > 1 || data->koeffby.getdlinna() > 1 \
|| data->nomz.getdlinna() > 1)
 {
  class poiinpdw_data rekin;
  in=atol(row[0]);
  if((mm=poiinpd(in,mt,gt,&rekin)) != 0)
   {
    printw("Ошибка поиска переменных данных %d для %s !\n",mm,row[0]);
    OSTANOV();
   }
  else
   {
    if(proverka(data->shetu.ravno(),rekin.shetu.ravno(),1,0) != 0)
     return(8);
     
    if(proverka(data->hifrz.ravno(),rekin.hzt.ravno(),0,0) != 0)
     return(9);

    if(proverka(data->hifrau.ravno(),rekin.hau.ravno(),0,0) != 0)
     return(10);

    if(proverka(data->hifrnao.ravno(),rekin.hna.ravno(),0,0) != 0)
     return(11);

    if(proverka(data->hifrnao_bu.ravno(),rekin.hnaby.ravno(),0,0) != 0)
     return(11);

    if(data->koeff.getdlinna() > 1 && fabs(ATOFM(data->koeff.ravno()) - rekin.popkf) > 0.00009)
      return(12);
    if(data->koeffby.getdlinna() > 1 && fabs(ATOFM(data->koeffby.ravno()) - rekin.popkfby) > 0.00009)
      return(13);
    if(data->nomz.getdlinna() > 1 && strstrm(rekin.nomz.ravno(),data->nomz.ravno()) == 0)
     return(14);

   }
 }

if(data->podraz.getdlinna() > 1 || data->kodol.getdlinna() > 1)
 {
  in=atol(row[0]);
  poiin(in,dt,mt,gt,&poddz,&kodotl);

  if(proverka(data->kodol.ravno(),kodotl,0,0) != 0)
    return(14);
  if(proverka(data->podraz.ravno(),poddz,0,0) != 0)
    return(15);
 } 
return(0);
}


/*******************/
/*Распечатка поиска*/
/*******************/
void raspoi(SQLCURSOR *cur,long kolstr,int mp,class diros_poi *data)
{
FILE            *ff;
char            imaf[64];
SQL_str         row;
int		kolstr1;
int		poddz,kodotl;
int		in;
char		strsql[512];
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);


VVOD SOOBN(1);
SOOBN.VVOD_SPISOK_add_MD(gettext("Выгружаем записи в файл"));
soobsh(&SOOBN,-1,-1,1);

move(LINES-1,0);
GDITE();
sprintf(imaf,"uosp%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }


iceb_t_zagolov(gettext("Распечатка основных средств"),0,0,0,0,0,0,ff);

if(data->naim.getdlinna() > 1)
 fprintf(ff,"%s: %s\n",gettext("Наименование"),data->naim.ravno());
if(data->zaviz.getdlinna() > 1)
 fprintf(ff,"%s: %s\n",gettext("Завод"),data->zaviz.ravno());
if(data->pasp.getdlinna() > 1)
 fprintf(ff,"%s: %s\n",gettext("Паспорт/чертеж N"),data->pasp.ravno());
if(data->model.getdlinna() > 1)
 fprintf(ff,"%s: %s\n",gettext("Модель/тип/марка"),data->model.ravno());
if(data->godv.getdlinna() > 1)
 fprintf(ff,"%s: %s\n",gettext("Год выпуска"),data->godv.ravno());
if(data->zavnom.getdlinna() > 1)
 fprintf(ff,"%s: %s\n",gettext("Заводской номер"),data->zavnom.ravno());
if(data->datavv.getdlinna() > 1)
 fprintf(ff,"%s: %s\n",gettext("Дата ввода в эксплуатацию"),data->datavv.ravno());
if(data->shetu.getdlinna() > 1)
 fprintf(ff,"%s: %s\n",gettext("Счёт учета"),data->shetu.ravno());
if(data->hifrz.getdlinna() > 1)
 fprintf(ff,"%s: %s\n",
 gettext("Шифр затрат амортизационных отчислений"),data->hifrz.ravno());
if(data->hifrau.getdlinna() > 1)
 fprintf(ff,"%s: %s\n",gettext("Шифр аналитического учета"),data->hifrau.ravno());
if(data->hifrnao.getdlinna() > 1)
 fprintf(ff,"%s: %s\n",gettext("Группа налогового учёта"),data->hifrnao.ravno());
if(data->hifrnao_bu.getdlinna() > 1)
 fprintf(ff,"%s: %s\n",gettext("Группа бухгалтерского учёта"),data->hifrnao_bu.ravno());
if(data->koeff.getdlinna() > 1)
 fprintf(ff,"%s: %s\n",gettext("Поправочний коэффициент"),data->koeff.ravno());
if(data->koeffby.getdlinna() > 1)
 fprintf(ff,"%s: %s\n",gettext("Поправочний коэффициент"),data->koeffby.ravno());
if(data->nomz.getdlinna() > 1)
 fprintf(ff,"%s: %s\n",gettext("Номерной знак"),data->nomz.ravno());
if(data->kodol.getdlinna() > 1)
 fprintf(ff,"%s: %s\n",gettext("Код ответственного лица"),data->kodol.ravno());
if(data->podraz.getdlinna() > 1)
 fprintf(ff,"%s: %s\n",gettext("Подразделение"),data->podraz.ravno());

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Инв. номер|         Наименование                   |Дата ввод.|Заводс.ном.|Гос. номер |Бал.стоим.|  Износ   |Бал.стоим.|  Износ   |Материально|\n\
          |                                        |в эксплуа.|           |           |Налог.учёт|Налог.учёт|Бухг. учёт|Бухг.учёт |отвец. лицо|\n"));
/*
1234567890 1234567890123456789012345678901234567890 1234567890 12345678901 12345678901 1234567890 1234567890 1234567890 1234567890 12345678901
*/
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------\n");
//sb=si=0;
cur->poz_cursor(0);
kolstr1=0;
double bal_stoim_bu=0.;
double iznos_bu=0.;
double bal_stoim_nu=0.;
double iznos_nu=0.;
double itogo[4];
memset(&itogo,'\0',sizeof(itogo));
short dve,mve,gve;
class iceb_tu_str mat_otv("");
class iceb_tu_str fio_mo("");
SQL_str row1;
class SQLCURSOR cur1;
class poiinpdw_data rekin;
while(cur->read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(mp == 1)   
   if(uosprov(data,row) != 0)
     continue;

  in=atol(row[0]);

  bal_stoim_bu=0.;
  iznos_bu=0.;
  bal_stoim_nu=0.;
  iznos_nu=0.;

  if(poiin(in,dt,mt,gt,&poddz,&kodotl) == 0)
   {
    class bsizw_data bal_st;
    bsiz(in,poddz,dt,mt,gt,&bal_st,NULL);

    bal_stoim_nu=bal_st.sbs+bal_st.bs;
    iznos_nu=bal_st.siz+bal_st.iz+bal_st.iz1;
   
    bal_stoim_bu=bal_st.sbsby+bal_st.bsby;
    iznos_bu=bal_st.sizby+bal_st.izby+bal_st.iz1by;

    itogo[0]+=bal_stoim_nu;
    itogo[1]+=iznos_nu;
     
    itogo[2]+=bal_stoim_bu;
    itogo[3]+=iznos_bu;
     
   } 
 
  poiinpd(in,mt,gt,&rekin);
  rsdat(&dve,&mve,&gve,row[7],2);

  sprintf(strsql,"select naik from Uosol where kod=%d",kodotl);
  if(readkey(strsql,&row1,&cur1) == 1)
   fio_mo.new_plus(row1[0]);
  else
   fio_mo.new_plus("");   

  mat_otv.new_plus(kodotl);
  mat_otv.plus(" ",fio_mo.ravno());
    
  fprintf(ff,"%-10d %-*s %02d.%02d.%d %-*.*s %-*.*s %10.2f %10.2f %10.2f %10.2f %-*.*s\n",
  in,
  iceb_tu_kolbait(40,row[2]),
  row[2],
  dve,mve,gve,
  iceb_tu_kolbait(11,row[6]),iceb_tu_kolbait(11,row[6]),row[6],
  iceb_tu_kolbait(11,rekin.nomz.ravno()),iceb_tu_kolbait(11,rekin.nomz.ravno()),rekin.nomz.ravno(),
  bal_stoim_nu,iznos_nu,bal_stoim_bu,iznos_bu,
  iceb_tu_kolbait(11,mat_otv.ravno()),iceb_tu_kolbait(11,mat_otv.ravno()),mat_otv.ravno());

 }
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%-10s %-40s %10s %-10s %*s %10.2f %10.2f %10.2f %10.2f\n",
" "," "," "," ",
iceb_tu_kolbait(10,gettext("Итого")),gettext("Итого"),itogo[0],itogo[1],itogo[2],itogo[3]);

podpis(ff);


fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка основных средств"));
iceb_t_ustpeh(imaf,3);
iceb_t_rabfil(&oth,"");

}
