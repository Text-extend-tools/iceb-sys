/*$Id: dirkkr.c,v 5.39 2013/09/26 09:43:30 sasa Exp $*/
/*21.04.2017	23.10.2002	Белых А.И.	dirkkr.c
Ввод и корректировка кодов командировочных расходов
*/
#include <ctype.h>
#include        <errno.h>
#include        "buhg.h"

void	dirkkrsp(int,const char*,const char*,short dlinna_nazv);
int	dirkkrvvod(int,class iceb_tu_str*);
int	kodrashod(int);
int	dirkkrzap(int,const char*,const char*,const char*,int,VVOD*);
int     dirkkruz(const char*);
int	dirkkrpoi(SQL_str,const char*,const char*);
void	dirkkrras(SQLCURSOR*,int,const char*,const char*);


int dirkkr(class iceb_tu_str *kodkr,class iceb_tu_str *naimkr,
int metkareg) //0-ввод и корректировка 1-выбор
{
char		strsql[1024];
SQL_str		row,row1;
int		kolstr=0;
int		K=0;
long		pozz=0,pozz1=0;
int		i=0,kom=0,kom1=0;
short           XK=0,YK=3;
short 		mspz=0;
int		prc=0;
class iceb_tu_str naimpoi(naimkr->ravno());
class iceb_tu_str kodmv(""); /*Код вновь введенного расхода*/
class iceb_tu_str naigrup("");
class iceb_tu_str kgrm("");
class iceb_tu_str vstr("");
int		poz,komv;
class iceb_t_mstr spis_menu(0);
class SQLCURSOR curr;
short dlinna_nazv=0;
short max_dlinna_nazv=0;

VVOD VVOD1(2);

naz1:;

GDITE();
clear();
short           kls=LINES-6;

/*MEN5 MM5(kls,COLS,0);*/

K=0;
sprintf(strsql,"select * from Ukrkras order by kod asc");
/*
printw("\n%s\n",strsql);
refresh();
*/
SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);  
 }

if(kolstr == 0)
 {
lll:;
  dirkkrsp(metkareg,naimpoi.ravno(),kgrm.ravno(),30);
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == FK3 || K == ENTER || K == SFK3)
     goto lll;

  if(K == KEY_RESIZE)
    goto naz1;
    
  goto vper;
 }
if(naimpoi.ravno()[0] == '\0' && kodmv.getdlinna() > 1 && kolstr > kls  && \
pozz < kolstr-kls)
 {
  pozz=poivcur(kodmv.ravno(),0,&cur);
  kodmv.new_plus("");
 }

naz:;
GDITE();
clear();

for(i=0; i < kls ;) /*Вычисляем максимальную длинну названия*/
 {
  
  if(cur.read_cursor(&row) == 0)
    break;


  if(dirkkrpoi(row,naimpoi.ravno(),kgrm.ravno()) != 0)
   continue;

  if(iceb_tu_strlen(row[1]) > max_dlinna_nazv)
   max_dlinna_nazv=iceb_tu_strlen(row[1]);
  i++;
 }

cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
spis_menu.spis_str.free_class();
dlinna_nazv=COLS-52;
if(max_dlinna_nazv < dlinna_nazv)
 dlinna_nazv=max_dlinna_nazv;
if(dlinna_nazv < 30)
 dlinna_nazv=30;
for(i=0; i < kls ;)
 {
  
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;

  if(dirkkrpoi(row,naimpoi.ravno(),kgrm.ravno()) != 0)
   continue;

  sprintf(strsql,"%-*s|%-*.*s|%-*s|%-*s|%-*s|%.15g",
  iceb_tu_kolbait(2,row[0]),row[0],
  iceb_tu_kolbait(dlinna_nazv,row[1]),iceb_tu_kolbait(dlinna_nazv,row[1]),row[1],
  iceb_tu_kolbait(4,row[5]),row[5],
  iceb_tu_kolbait(6,row[3]),row[3],
  iceb_tu_kolbait(2,row[4]),row[4],
  atof(row[2]));

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
  if(kolstr > 0 && pozz > 0 )
   {
    pozz--;
    goto naz;
   }
  goto lll;
 }
if(i <= kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

dirkkrsp(metkareg,naimpoi.ravno(),kgrm.ravno(),dlinna_nazv);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,YK,XK,NULL,strsql,prc,&K,"",0);


if(kom == -7) //Изменение размеров экрана
  goto naz1;
if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  GDITE();
  for(i=0; i < mspz  ; )
   {
    pozz++;
    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;

    if(dirkkrpoi(row,naimpoi.ravno(),kgrm.ravno()) != 0)
     continue;
    i++;
   }
  kom=kls;
  goto naz;
 }
if(kom == -2) /*Стрелка вверх*/
 {
  GDITE();
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

    if(dirkkrpoi(row,naimpoi.ravno(),kgrm.ravno()) != 0)
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
/*  
  printw("\n вперед pozz=%d\n",pozz);
  OSTANOV();
*/
  goto naz;

 }

if(kom == -4) /*Страница вверх*/
 {
/*
  printw("\npozz=%d\n",pozz);
  refresh();
*/
  GDITE();
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

    if(dirkkrpoi(row,naimpoi.ravno(),kgrm.ravno()) != 0)
     continue;
    i++;
   }
  goto naz;
 }


if(kom >= 0)
 {
  class iceb_tu_str str_vib(spis_menu.spis_str.ravno(kom));
  if(SRAV(gettext("Конец списка"),str_vib.ravno(),0) == 0)
     goto naz;

  if(pozz+kom >= kolstr)
   goto naz;

  polen(str_vib.ravno(),kodkr,1,'|');
  polen(str_vib.ravno(),naimkr,2,'|');

 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return(1);

  case FK1: //Просмотр описания режима
    GDITE();
    iceb_t_pdoc("ukr3_3.txt");
    clear();

    goto naz1;

  case FK2: //Ввод новой записи
    kodmv.new_plus("");
    dirkkrvvod(0,&kodmv);
    naimpoi.new_plus("");
    goto naz1;

  case ENTER: /*Выход или корректировка*/
    if(metkareg == 1)
      return(0);

  case SFK2: //Корректировка записи
    dirkkrvvod(1,kodkr);
    kodmv.new_plus("");
    goto naz1;

  case FK3: //Удаление записи
    dirkkruz(kodkr->ravno());
    kodmv.new_plus("");
    goto naz1;

  case FK4:       /*Отобрать по образцу*/

    helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
    
    naimpoi.new_plus("");
    
    VVOD1.VVOD_delete();
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите образец для поиска"));

    if(vvod1(&naimpoi,40,&VVOD1,NULL,stdscr,-1,-1) == FK10)
      naimpoi.new_plus("");

    GDITE();

    clear();
    kom=pozz=0;
    if(kolstr == 0)
      goto lll;
    else
      goto naz;

  case FK5:    //Распечатать
    dirkkrras(&cur,kolstr,naimpoi.ravno(),kgrm.ravno());
    goto naz;
    
  case FK6:       /*Стать на номер */

    helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
    
    strsql[0]='\0';
    
    VVOD1.VVOD_delete();
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код материалла"));
    vstr.new_plus("");
    if((kom1=vvod1(&vstr,10,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
      goto naz;
    if(kom1 == ESC)
      goto naz;
    if(vstr.ravno()[0] == '\0')
      goto naz;
      
    naimpoi.new_plus("");
    pozz=0;
    kom=0;
    pozz=poivcur(vstr.ravno(),0,&cur);
    if(pozz == kolstr)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден код материалла"),vstr.ravno());
      iceb_t_soob(strsql);
      pozz=0;
     }
    clear();
    goto naz;
    
  case FK9:       /*Просмотр по группе*/


    helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
     
    kgrm.new_plus("");
    naigrup.new_plus("");
     
    VVOD1.VVOD_delete();
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код или наименование группы материалла"));

    if((i=vvod1(&kgrm,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
     {
      if(kolstr == 0)
        goto lll;
      else
        goto naz;
     }
    if(i == FK10)
     {
      if(kolstr == 0)
        goto lll;
      else
        goto naz;
     }

    if(kgrm.getdlinna() <= 1 || isdigit(kgrm.ravno()[0]) == 0)
     {
      clear();
      kgrm.new_plus("");
      poz=komv=0;
      if(dirmat1("Ukrgrup",&kgrm,&naigrup,-1,-1,-1,1,&poz,&komv,"","\0") != 0)
       {
        if(kolstr == 0)
          goto lll;
        else
          goto naz;
       }
     }

    /*Читаем код группы материалла*/
    sprintf(strsql,"select * from Grup where kod=%s",kgrm.ravno());

    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      kgrm.new_plus("");
      naigrup.new_plus("");

      sprintf(strsql,gettext("Нет группы материалла %s !"),kgrm.ravno());
      iceb_t_soob(strsql);

      if(kolstr == 0)
        goto lll;
      else
        goto naz;
     }
   naigrup.new_plus(row1[1]);
   kom=pozz=0;
   if(kolstr == 0)
     goto lll;
   else
     goto naz;


  default:
   if(kolstr > 0)
     goto naz;
   else
     goto lll;
 }
}
/************************************/
/* ВВод и корректировка записи      */
/************************************/

int dirkkrvvod(int regim, //0-ввод 1-корректировка
class iceb_tu_str *kodkr) //Код командировочных расходов для корректировки
{
struct  tm      *bf;
int	        metkacen=0; //0-цена с НДС 1-без НДС
char	        strsql[1024];
SQL_str	        row;
class iceb_tu_str kodgr("");
class iceb_tu_str eiz("");
class iceb_tu_str naim("");
class iceb_tu_str shet("");
class iceb_tu_str cena("");
int	        ktozap=0;
time_t          vremz=0;
class iceb_tu_str naimgrup("");
class iceb_tu_str naimeiz("");
int             K=0,N=1;
int		poz,komv;
struct OPSHET	shetv;
SQLCURSOR curr;

VVOD VV(0);

VV.VVOD_SPISOK_add_data(10);
VV.VVOD_SPISOK_add_data(60);
VV.VVOD_SPISOK_add_data(60);
VV.VVOD_SPISOK_add_data(80);
VV.VVOD_SPISOK_add_data(10);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(30);

VV.VVOD_SPISOK_add_ZAG(gettext("Ввод и корректировка командировочных расходов"));

if(kodkr->getdlinna() <= 1) //Ввод нового расхода
 {
  sprintf(strsql,"%d",kodrashod(1));
  VV.VVOD_SPISOK_zapis_data(0,strsql);
 }
else //Корректировка
 {
  sprintf(strsql,"select * from Ukrkras where kod=%s",kodkr->ravno());
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    sprintf(strsql,gettext("Не нашли расход %s !"),kodkr->ravno());
    iceb_t_soob(strsql);
    return(1);
   }

  naim.new_plus(row[1]);
  cena.new_plus(row[2]);
  shet.new_plus(row[3]);
  kodgr.new_plus(row[4]);
  eiz.new_plus(row[5]);
  metkacen=atoi(row[6]);
  ktozap=atoi(row[7]);
  vremz=atol(row[8]);

  VV.VVOD_SPISOK_add_ZAG(iceb_t_kszap(ktozap));
  bf=localtime(&vremz);
  sprintf(strsql,"%s %d.%d.%d%s %s: %d:%d:%d",
  gettext("Дата записи"),
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
  gettext("г."),
  gettext("Время"),
  bf->tm_hour,bf->tm_min,bf->tm_sec);

  VV.VVOD_SPISOK_add_ZAG(strsql);
    
  //Читаем наименование группы
  sprintf(strsql,"select naik from Ukrgrup where kod=%s",kodgr.ravno());
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
   naimgrup.new_plus(row[0]);

  //Читаем наименование единицы измерения
  sprintf(strsql,"select naik from Edizmer where kod='%s'",eiz.ravno());
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
   naimeiz.new_plus(row[0]);
   
  VV.VVOD_SPISOK_zapis_data(0,kodkr->ravno());
  VV.VVOD_SPISOK_zapis_data(1,naimgrup.ravno());
  VV.VVOD_SPISOK_zapis_data(2,naimeiz.ravno());
  VV.VVOD_SPISOK_zapis_data(3,naim.ravno());
  VV.VVOD_SPISOK_zapis_data(4,shet.ravno());
  sprintf(strsql,"%.2f",cena.ravno_atof());
  if(strsql[0] != '0')
   VV.VVOD_SPISOK_zapis_data(5,strsql);
  
 }

VV.VVOD_SPISOK_add_MD(gettext("Код расхода........"));
VV.VVOD_SPISOK_add_MD(gettext("Группа............."));
VV.VVOD_SPISOK_add_MD(gettext("Единица измерения.."));
VV.VVOD_SPISOK_add_MD(gettext("Наименование......."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт учета........."));
VV.VVOD_SPISOK_add_MD(gettext("Цена..............."));
VV.VVOD_SPISOK_add_MD(gettext("Цена введена......."));


naz:;
clear();

if(metkacen == 0)
  VV.VVOD_SPISOK_zapis_data(6,gettext("C НДС"));
else
  VV.VVOD_SPISOK_zapis_data(6,gettext("Без НДС"));

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F4",gettext("найти код"),
"F10",gettext("выход"),NULL);


VV.vramka(-1,-1,0);

attron(VV.VVOD_return_cs(iceb_CFS)); 

VV.vprintw(N,stdscr);


for(;;)
 {

  K=getch();

  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

  if(K ==  KEY_RESIZE)
    goto naz;

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));
    VV.vprintw_clear(N,stdscr);


    if(N == 1)  //Код группы
     {
      naimgrup.new_plus("");
      poz=komv=0;
      if(dirmat1("Ukrgrup",&kodgr,&naimgrup,-1,-1,-1,2,&poz,&komv,"","\0") == 0)
        VV.VVOD_SPISOK_zapis_data(N,naimgrup.ravno());
      N++;
      goto naz;
     }

    if(N == 2)  //Единица измерения
     {
      naimeiz.new_plus("");
      if(iceb_tl_ei(2,&eiz,&naimeiz) == 0)
        VV.VVOD_SPISOK_zapis_data(N,naimeiz.ravno());
       
      N++;
      goto naz;
     }

    if(N == 6)  //Метка ввода цены
     {
      attroff(VV.VVOD_return_cs(iceb_CFV));
      metkacen++;
      if(metkacen > 1)
       metkacen=0;
      goto naz;
     }

    VV.vgetstr(N,stdscr);
    
    if(N == 0)  /*Код расходов*/
     {
      if(VV.VVOD_SPISOK_return_data(N)[0] != '\0' && \
      SRAV(kodkr->ravno(),VV.VVOD_SPISOK_return_data(N),0) != 0)
       {
        sprintf(strsql,"select naim from Ukrkras where kod=%s",
        VV.VVOD_SPISOK_return_data(N));
        if(sql_readkey(&bd,strsql,&row,&curr) >= 1)
         {
          attroff(VV.VVOD_return_cs(iceb_CFV));
          VVOD SOOB(1);
          sprintf(strsql,gettext("Код расходов %s уже введён !"),VV.VVOD_SPISOK_return_data(N));
          SOOB.VVOD_SPISOK_add_MD(strsql);
          sprintf(strsql,gettext("Это: %s"),row[0]);
          SOOB.VVOD_SPISOK_add_MD(strsql);
          soobshw(&SOOB,stdscr,-1,-1,0,1);
          VV.data_plus(N,"");
          goto naz;
         }
        
       }
     }


    if(N == 4 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')  //Счёт учета
     {
      attroff(VV.VVOD_return_cs(iceb_CFV));
      if(prsh1(VV.VVOD_SPISOK_return_data(N),&shetv) != 0)
         goto naz;
     }

     
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    VV.vprintw(N,stdscr);

    N++;
   }

  if(K==SDN) /*Стрелка вниз*/
   {
    N++;
   }
  if(K==SUP) /*Стрелка вверх*/
   {
    N--;
   }
  if(N > VV.KLST-1)
   {
    N=0;
   }
  if(N < 0)
   {
    N=VV.KLST-1;
   }
  attron(VV.VVOD_return_cs(iceb_CFS)); 
  VV.vprintw(N,stdscr);

   if(K == ESC || K == FK10)
    {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     attron(VV.VVOD_return_cs(iceb_CFM));
     N=-1;
     break;
    }
  if(K == FK1) /*Помощь*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    GDITE();
    iceb_t_pdoc("ukr3_3_1.txt");
    clear();
    goto naz;
   }

   if(K == FK2 || K == PLU) /*Записываем*/
    {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     attron(VV.VVOD_return_cs(iceb_CFM));
     if(VV.VVOD_SPISOK_return_data(0)[0] == '\0' || ATOFM(VV.VVOD_SPISOK_return_data(0)) == 0.)
      {
       iceb_t_soob(gettext("Не введён код расхода !"));
       goto naz;
      }
     if(ATOFM(VV.VVOD_SPISOK_return_data(0)) < 0)
      {
       iceb_t_soob(gettext("Код не может быть отрицательным !"));
       goto naz;
      }
     if(VV.VVOD_SPISOK_return_data(1)[0] == '\0')
      {
       iceb_t_soob(gettext("Не введена группа расхода !"));
       goto naz;
      }

     if(VV.VVOD_SPISOK_return_data(3)[0] == '\0')
      {
       iceb_t_soob(gettext("Не введено наименование расхода !"));
       goto naz;
      }

     if(VV.VVOD_SPISOK_return_data(0)[0] != '\0' && \
     SRAV(kodkr->ravno(),VV.VVOD_SPISOK_return_data(0),0) != 0)
      {
       sprintf(strsql,"select naim from Ukrkras where kod=%s",
       VV.VVOD_SPISOK_return_data(0));
       if(sql_readkey(&bd,strsql,&row,&curr) >= 1)
        {
         attroff(VV.VVOD_return_cs(iceb_CFV));
         VVOD SOOB(1);
         sprintf(strsql,gettext("Код расходов %s уже введён !"),VV.VVOD_SPISOK_return_data(N));
         SOOB.VVOD_SPISOK_add_MD(strsql);
         sprintf(strsql,gettext("Это: %s"),row[0]);
         SOOB.VVOD_SPISOK_add_MD(strsql);
         soobshw(&SOOB,stdscr,-1,-1,0,1);
         VV.data_plus(N,"");
         goto naz;
        }
      }
     if(VV.VVOD_SPISOK_return_data(4)[0] != '\0')
      if(prsh1(VV.VVOD_SPISOK_return_data(4),&shetv) != 0)
       goto naz;

     if(dirkkrzap(regim,kodkr->ravno(),kodgr.ravno(),eiz.ravno(),metkacen,&VV) != 0)
       goto naz;
     break;
    }

  if(K == FK4) /*Взять новый код*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    VV.data_plus(0,kodrashod(1));
    goto naz;
   }
 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
refresh();
return(0);
}


/*****************************/
/*Шапка                      */
/*****************************/
void	dirkkrsp(int metkareg,const char *naimpoi,const char *kgrm,short dlinna_nazv)
{
int	i1;
int	X;

move(0,0);
printw(gettext("Ввод и корректировка командировочных расходов"));
move(2,0);
printw("   1               2                 %*s3      4   5   6",dlinna_nazv-30,"");

mvprintw(i1=2,X=COLS-21,gettext("Расшифровка полей:"));

mvprintw(++i1,X,gettext("1 код расходов"));
mvprintw(++i1,X,gettext("2 наименование"));
mvprintw(++i1,X,gettext("3 ед.измерения"));
mvprintw(++i1,X,gettext("4 счет"));
mvprintw(++i1,X,gettext("5 группа"));
mvprintw(++i1,X,gettext("6 цена"));

if(kgrm[0] != '\0' || naimpoi[0] != '\0')
 {
  attron(A_BLINK);
  mvprintw(i1+=2,X,gettext("Поиск"));

  if(naimpoi[0] != '\0')
    mvprintw(++i1,X,"%s:%s",gettext("Наим."),naimpoi);

  if(kgrm[0] != '\0')
    mvprintw(++i1,X,"%s:%s",gettext("Группа"),kgrm);

  attroff(A_BLINK);

 }
 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F6",gettext("код"),
"F9",gettext("груп."),
"F10",gettext("выход"),NULL);
}

/**********************************/
/*Получение свободного кода*/
/**********************************/

int	kodrashod(int metkag) //0-не выводить GDITE 1-выводить
{
int             i;
char		strsql[512];
SQL_str         row;
long		kolstr;

if(metkag == 1)
 GDITE();

i=0;

sprintf(strsql,"select kod from Ukrkras order by kod asc");
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return(0);
 }

while(cur.read_cursor(&row) != 0)
 {
//  printw("%s\n",row[0]);
  i++;

  if(i != atoi(row[0]))
     return(i);
 }
i++;
return(i);

}
/********************************************/
/*Запись                                    */
/********************************************/

int dirkkrzap(int regim,  //0-ввод 1-корректировка
const char *kodkr,
const char *kgr,
const char *eiz,
int  mnds,
VVOD *VV)
{
class iceb_tu_str naimkr("");
time_t vrem;
char   strsql[1024];

naimkr.new_plus(VV->VVOD_SPISOK_return_data(3));

time(&vrem);
/*Новая запись*/
if(regim == 0)       
 {
  sprintf(strsql,"insert into Ukrkras \
values (%s,'%s',%.6g,'%s',%s,'%s',%d,%d,%ld)",
  VV->VVOD_SPISOK_return_data(0),naimkr.ravno_filtr(),atof(VV->VVOD_SPISOK_return_data(5)),
  VV->VVOD_SPISOK_return_data(4),kgr,eiz,mnds,iceb_t_getuid(),vrem);
 }
/*Корректировка записи*/
if(regim == 1)       
 {
  sprintf(strsql,"update Ukrkras \
set \
kod=%s,\
naim='%s',\
cena=%.6g,\
shet='%s',\
kgr=%s,\
eiz='%s',\
mnds=%d,\
ktoz=%d,\
vrem=%ld \
where kod=%s",
  VV->VVOD_SPISOK_return_data(0),naimkr.ravno_filtr(),atof(VV->VVOD_SPISOK_return_data(5)),
  VV->VVOD_SPISOK_return_data(4),kgr,eiz,mnds,iceb_t_getuid(),vrem,
  kodkr);
 } 
 /*
printw("\n%s\n",strsql);
OSTANOV(); 
 */

if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
   {
    iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
    return(1);
   }
  else
   {
    msql_error(&bd,gettext("Ошибка записи !"),strsql);
    return(1);
   }
 }
return(0);
}

/*********************************/
/*Удаление записи                */
/*********************************/

int   dirkkruz(const char *kodkr)
{
char	strsql[512];
int	kolstr=0;


VVOD DANET(1);
DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

if(danet(&DANET,2,stdscr) == 2)
 return(1);

VVOD SOOBN(1);
sprintf(strsql,"%s:%s",gettext("Проверяем записи в таблице"),"Ukrdok1");
SOOBN.VVOD_SPISOK_add_MD(strsql);
soobsh(&SOOBN,-1,-1,1);

//Проверяем возможность удаления записи
sprintf(strsql,"select kodr from Ukrdok1 where kodr=%s limit 1",kodkr);
SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
   {
    msql_error(&bd,gettext("Ошибка"),strsql);
    return(1);
   }
 }
else
 {   
  if(kolstr != 0)
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Удалить невозможно !"));
    SOOB.VVOD_SPISOK_add_MD(gettext("Есть записи с этим кодом"));
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    return(1);
   }
 }



sprintf(strsql,"delete from Ukrkras where kod = %s",kodkr);
      /*      printw("\nstrsql=%s\n",strsql);*/
if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
  {
   iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
   return(1);
  }
 else
  {
   msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
   return(1);
  }
 }
return(0);

}
/*****************************/
/*Поиск                      */
/*****************************/

int	dirkkrpoi(SQL_str row,const char *naimpoi,const char *kodgr)
{

if(naimpoi[0] != '\0' && strstrm(row[1],naimpoi) == 0)
  return(1);

if(proverka(kodgr,row[4],0,0) != 0)
  return(2);

return(0);

}
/**********************************/
/*Распечатка                      */
/**********************************/

void	dirkkrras(SQLCURSOR *cur,int kolstr,const char *naimpoi,const char *kodgrup)
{
SQL_str		row;
int		kolstr1=0;
FILE		*ff;
char		imaf[64];
char		metkacen[64];


sprintf(imaf,"vkr%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return;
   }

iceb_t_zagolov(gettext("Распечатка расходов на командировки"),0,0,0,0,0,0,ff);

fprintf(ff,"\
---------------------------------------------------------------------------\n");
fprintf(ff,"\
Код| Наименование командировочных расходов  |Ед.из.| Счёт |Груп.|   Цена  |\n");
fprintf(ff,"\
---------------------------------------------------------------------------\n");

cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {
  printw("%3s %-40s %6s %6s %5s %9.2f\n",
  row[0],row[1],row[5],row[3],row[4],atof(row[2]));
  refresh();
  strzag(LINES-1,0,kolstr,++kolstr1);
  
  if(dirkkrpoi(row,naimpoi,kodgrup) != 0)
   continue;

  metkacen[0]='\0';
  if(row[2][0] != '0')
   {
    if(row[6][0] == '0')
     strcpy(metkacen,gettext("С НДС"));
    if(row[6][0] == '1')
     strcpy(metkacen,gettext("Без НДС"));
   }   

  fprintf(ff,"%*s %-*s %*s %*s %*s %9.2f %s\n",
  iceb_tu_kolbait(3,row[0]),row[0],
  iceb_tu_kolbait(40,row[1]),row[1],
  iceb_tu_kolbait(6,row[5]),row[5],
  iceb_tu_kolbait(6,row[3]),row[3],
  iceb_tu_kolbait(5,row[4]),row[4],
  atof(row[2]),metkacen);

 }
fprintf(ff,"\
---------------------------------------------------------------------------\n");

podpis(ff);

fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);

oth.spis_naim.plus(gettext("Распечатка расходов на командировки"));
iceb_t_ustpeh(imaf,3);

iceb_t_rabfil(&oth,"");
}
