/* $Id: mariq.c,v 5.32 2013/09/26 09:43:35 sasa Exp $ */
/*05.06.2008	29.01.2001	Белых А.И.	mariq.c
Программа настройки кассового регистратора
*/
#define         DVERSIQ "05.06.2008"
#include        <time.h>
#include        <pwd.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include        <locale.h>
#ifdef HAVE_TORGDEV
#include        <libdev.h>
#endif
#include        "buhg.h"
#ifdef	HAVE_UNISTD_H
#include <unistd.h>
#endif

SQL_baza	bd;
extern char	*imabaz;
short		startgodb;
short		vplsh;
int		ktorab=0;
int koderr = 0;
int	Model=0;

/***************************/
/* Програмировать внешний вид чека*/
/**********************************/

void	vidshek(KASSA kasr)
{
//int	hrift1=0;
//int     hrift2=0;
//int	N=0;
//int	K=0;
//int     KLST,MDLS;
//int     dlr;
//char	strsql[512];
//int	kom;

#ifdef HAVE_TORGDEV
VVOD MENU(4);
VVOD VV(0);


VV.VVOD_SPISOK_add_ZAG(gettext("Формирование внешнего вида чека."));


if (Model & (M_MARIA | M_AMC100F)) 
   VV.VVOD_SPISOK_add_MD(gettext("Заголовок чека.............."));
if (Model & M_MARIA) 
   VV.VVOD_SPISOK_add_data(19);
else if (Model & M_AMC100F) 
   VV.VVOD_SPISOK_add_data(18);

if (Model & (M_MARIA | M_AMC100F)) 
   VV.VVOD_SPISOK_add_MD(gettext("Заключительная строка......."));
if (Model & M_MARIA) 
   VV.VVOD_SPISOK_add_data(19);
else if (Model & M_AMC100F) 
   VV.VVOD_SPISOK_add_data(18);

if (Model & M_MARIA) {
   VV.VVOD_SPISOK_add_MD(gettext("Текст к  фис-ной инф-ции...."));
   VV.VVOD_SPISOK_add_data(45);
   }
else if (Model & M_AMC100F) {
   VV.VVOD_SPISOK_add_MD("");
   VV.VVOD_SPISOK_add_data(0);
   }

if (Model & (M_MARIA | M_AMC100F)) {
   VV.VVOD_SPISOK_add_MD(gettext("Шрифт заголовка............."));
   VV.VVOD_SPISOK_add_data(20);
   }
   
if (Model & M_MARIA) {
   VV.VVOD_SPISOK_add_MD(gettext("Текст после фи-ной инф-ции.."));
   VV.VVOD_SPISOK_add_data(45);
   }
else if (Model & M_AMC100F) {
   VV.VVOD_SPISOK_add_MD("");
   VV.VVOD_SPISOK_add_data(0);
   }

if (Model & (M_MARIA | M_AMC100F)) {
   VV.VVOD_SPISOK_add_MD(gettext("Шрифт окончания............."));
   VV.VVOD_SPISOK_add_data(20);
   }
naz:;

if(hrift1 == 0)
 VV.VVOD_SPISOK_zapis_data(3,gettext("Обычный"));
else {
 if (Model & (M_MARIA))
    VV.VVOD_SPISOK_zapis_data(3,gettext("Выделенный"));
 else if (Model & (M_AMC100F))
    VV.VVOD_SPISOK_zapis_data(3,gettext("Широкий"));
}
if(hrift2 == 0)
 VV.VVOD_SPISOK_zapis_data(5,gettext("Обычный"));
else {
 if (Model & (M_MARIA))
    VV.VVOD_SPISOK_zapis_data(5,gettext("Выделенный"));
 else if (Model & (M_AMC100F))
    VV.VVOD_SPISOK_zapis_data(5,gettext("Широкий"));
}

helstr(LINES-1,0,"F2/+",gettext("запись"),
"F10",gettext("выход"),NULL);

int PY=-1;
int PX=-1;

VV.vramka(&PY,&PX,0);


attron(VV.VVOD_return_cs(iceb_CFS)); 

move(PY+N+1,PX+1+dlr);
printw("%-*s",VV.VVOD_SPISOK_return_dls(N)-1,VV.VVOD_SPISOK_return_data(N));

for(;;)
 {

  refresh();
  K=getch();

  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  move(PY+N+1,PX+1+dlr);
  printw("%-*s",VV.VVOD_SPISOK_return_dls(N)-1,VV.VVOD_SPISOK_return_data(N));

  if(K == KEY_RESIZE)
   goto naz;

  if(K == KEY_MOUSE)
    K=mouseklic(&N,PY,PX,VV.MDLS,VV.KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));

    move(PY+N+1,PX+1+dlr);
    printw("%*s",VV.VVOD_SPISOK_return_dls(N)-1," ");
    move(PY+N+1,PX+1+dlr);
    if(N == 3)
     {
      MENU.VVOD_delete();
      MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите шрифт."));
      MENU.VVOD_SPISOK_add_MD(gettext("Обычный"));
      if (Model & M_MARIA)
         MENU.VVOD_SPISOK_add_MD(gettext("Выделенный"));
      else if (Model & M_AMC100F)
         MENU.VVOD_SPISOK_add_MD(gettext("Широкий"));

      kom=0;
      while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
      if(kom == -1)
        goto naz;       
      hrift1=kom; 
      N++;
      goto naz;       
     }

    if(N == 5)
     {
      MENU.VVOD_delete();
      MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите шрифт."));
      MENU.VVOD_SPISOK_add_MD(gettext("Обычный"));
      if (Model & M_MARIA)
         MENU.VVOD_SPISOK_add_MD(gettext("Выделенный"));
      else if (Model & M_AMC100F)
         MENU.VVOD_SPISOK_add_MD(gettext("Широкий"));
      kom=0;
      while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
      if(kom == -1)
        goto naz;       
      hrift2=kom;
      goto naz;
     }

//    mygetstr(VV.VVOD_SPISOK_return_data(N),VV.VVOD_SPISOK_return_dls(N),stdscr);
    VV.vgetstr(N,stdscr);
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    move(PY+N+1,PX+1+dlr);
    printw("%-*s",VV.VVOD_SPISOK_return_dls(N)-1,VV.VVOD_SPISOK_return_data(N));
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
  move(PY+N+1,PX+1+dlr);
  printw("%-*s",VV.VVOD_SPISOK_return_dls(N)-1,VV.VVOD_SPISOK_return_data(N));

  if(K == ESC || K == FK10)
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    N=-1;
    return;
    break;
   }
  if(K == FK2 || K == PLU) /*Записываем*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    break;
   }
 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
refresh();

GDITE();
  sprintf(strsql,"FSTART: MARIA_SETCHECK\n\
%s\n\
%s\n\
%s\n\
%s\n\
%s\n\
%d\n\
%s\n\
%d\n\
FEND:\n",
  kasr.prodavec, kasr.parol, VV.VVOD_SPISOK_return_data(0),
  VV.VVOD_SPISOK_return_data(1), VV.VVOD_SPISOK_return_data(2),
  hrift1, VV.VVOD_SPISOK_return_data(4),hrift2);  

  /* отправим команду серверу	*/
  iceb_cmd(kasr, strsql);
#endif
}

/******************************/
/*Програмирование налога */
/****************************/

void	prognal(KASSA kasr)
{
int	priznak=0;
int	N=0;
int	K=0;
char	strsql[512];
int	kom;

VVOD MENU(4);
VVOD VV(0);

VV.VVOD_SPISOK_add_data(5);
VV.VVOD_SPISOK_add_data(2);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(20);

VV.VVOD_SPISOK_add_ZAG(gettext("Програмировать налог."));
VV.VVOD_SPISOK_add_MD(gettext("Наименование налога..."));
VV.VVOD_SPISOK_add_MD(gettext("Номер налога.........."));
VV.VVOD_SPISOK_add_MD(gettext("Признак вложенности..."));
VV.VVOD_SPISOK_add_MD(gettext("Процент..............."));

naz:;

if(priznak == 0)
 VV.VVOD_SPISOK_zapis_data(2,gettext("Вложенный"));
if(priznak == 1)
 VV.VVOD_SPISOK_zapis_data(2,gettext("Наложенный"));
if(priznak == 2)
 VV.VVOD_SPISOK_zapis_data(2,gettext("Наложенный с вычитанием"));



helstr(LINES-1,0,"F2/+",gettext("запись"),
"F10",gettext("выход"),NULL);

VV.vramka(-1,-1,0);
attron(VV.VVOD_return_cs(iceb_CFS)); 

VV.vprintw(N,stdscr);

for(;;)
 {

  refresh();
  K=getch();

  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);
  if(K == KEY_RESIZE)
   goto naz;
   
  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));

    if(N == 1)
     {
      MENU.VVOD_delete();
      MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите номер налога."));
      MENU.VVOD_SPISOK_add_MD("1");
      MENU.VVOD_SPISOK_add_MD("2");
      MENU.VVOD_SPISOK_add_MD("3");
      MENU.VVOD_SPISOK_add_MD("4");
      MENU.VVOD_SPISOK_add_MD("5");
      MENU.VVOD_SPISOK_add_MD("6");
      MENU.VVOD_SPISOK_add_MD("7");
      MENU.VVOD_SPISOK_add_MD("8");
      kom=0;
      while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
      if(kom == -1)
        goto naz; 
      VV.data_plus(1,1+kom);
      N++;      
      goto naz;       
     }
    if(N == 2)
     {
      MENU.VVOD_delete();
      MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите признак."));
      MENU.VVOD_SPISOK_add_MD(gettext("Вложенный"));
      MENU.VVOD_SPISOK_add_MD(gettext("Наложенный"));
      MENU.VVOD_SPISOK_add_MD(gettext("Наложенный с вычитанием"));

      kom=0;
      while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
      if(kom == -1)
        goto naz; 
      priznak=kom;
      N++;
      goto naz;       
     }

//    mygetstr(VV.VVOD_SPISOK_return_data(N),VV.VVOD_SPISOK_return_dls(N),stdscr);
    VV.vgetstr(N,stdscr);

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
    return;
    break;
   }
  if(K == FK2 || K == PLU) /*Записываем*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    break;
   }
 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
refresh();

GDITE();

  sprintf(strsql,"FSTART: MARIA_NALOG\n\
%s\n\
%s\n\
%s\n\
%s\n\
%d\n\
%.2f\n\
FEND:\n",
  kasr.prodavec, kasr.parol, VV.VVOD_SPISOK_return_data(0),
  VV.VVOD_SPISOK_return_data(1), priznak, 
  ATOFM(VV.VVOD_SPISOK_return_data(3)));
//printw("\n%s\n",strsql);
//refresh();
  /* отправим команду серверу	*/
  iceb_cmd(kasr, strsql);
}
/********************************************/
/*Програмирование регистрационной информации*/
/********************************************/

void	progreginf(KASSA kasr)
{
char	strsql[512];
int		kom;

VVOD VV(0);

VV.VVOD_SPISOK_add_data(11);
VV.VVOD_SPISOK_add_data(13);
VV.VVOD_SPISOK_add_data(19);
VV.VVOD_SPISOK_add_data(19);
VV.VVOD_SPISOK_add_data(11);

VV.VVOD_SPISOK_add_ZAG(gettext("Програмировать реги-нную информацию о владельце."));

VV.VVOD_SPISOK_add_MD(gettext("Фискальный номер....."));
VV.VVOD_SPISOK_add_MD(gettext("Код плательщика НДС.."));
VV.VVOD_SPISOK_add_MD(gettext("Наименование фирмы..."));
VV.VVOD_SPISOK_add_MD(gettext("Адрес................"));
#if ( MARIA_PROTOCOL_VERSION > 1 )
VV.VVOD_SPISOK_add_MD(gettext("Код ЕГРПОУ..........."));
#endif

helstr(LINES-1,0,"F2/+",gettext("печать"),
"F10",gettext("выход"),NULL);


kom=VV.vvod(0,1,1,-1,-1);
switch (kom)
 {
  case ESC:
  case FK10:
    return;
/*
  case FK1:
    GDITE();
    prosf(strsql);
    clear();
    goto naz1;
*/
  case FK2:
  case PLU:
    break;
 }


GDITE();

  sprintf(strsql,"FSTART: MARIA_REGINFO\n\
%s\n\
#if( MARIA_PROTOCOL_VERSION > 1 )\
%s\n\
#endif\
%s\n\
%s\n\
%s\n\
%s\n\
%s\n\
FEND:\n",
  kasr.prodavec, kasr.parol,
  VV.VVOD_SPISOK_return_data(0),
#if ( MARIA_PROTOCOL_VERSION > 1 )
  VV.VVOD_SPISOK_return_data(4),
#endif
  VV.VVOD_SPISOK_return_data(1),
  VV.VVOD_SPISOK_return_data(2),
  VV.VVOD_SPISOK_return_data(3));

  /* отправим команду серверу	*/
  iceb_cmd(kasr, strsql);
}
/******************************/
/*Програмировать валюту*/
/*************************/
void	progval(KASSA kasr)
{
char	strsql[512];
int		kom;

VVOD VV(0);

VV.VVOD_SPISOK_add_data(10);
VV.VVOD_SPISOK_add_data(4);

VV.VVOD_SPISOK_add_ZAG(gettext("Програмировать валюту."));

VV.VVOD_SPISOK_add_MD(gettext("Количество знаков после десятичной точки.."));
VV.VVOD_SPISOK_add_MD(gettext("Наименование валюты......................."));

helstr(LINES-1,0,"F2/+",gettext("печать"),
"F10",gettext("выход"),NULL);

//kom=vvod(&LL,0,1,3);
kom=VV.vvod(0,1,1,-1,-1);
switch (kom)
 {
  case ESC:
  case FK10:
    return;
/*
  case FK1:
    GDITE();
    prosf(strsql);
    clear();
    goto naz1;
*/
  case FK2:
  case PLU:
    break;
 }


GDITE();

  sprintf(strsql,"FSTART: MARIA_VALUTA\n\
%s\n\
%s\n\
%s\n\
%s\n\
FEND:\n",
  kasr.prodavec, kasr.parol, VV.VVOD_SPISOK_return_data(0),
  VV.VVOD_SPISOK_return_data(1));  

  /* отправим команду серверу	*/
  iceb_cmd(kasr, strsql);
}
/*********************************/
/*Распечатать текст на индикаторе*/
/**********************************/

void	rastxtmar(KASSA kasr)
{
static char	soob1[20];
static char	soob2[20];
int		kom;

VVOD VV(0);

VV.VVOD_SPISOK_add_data(16);
VV.VVOD_SPISOK_add_data(16);

naz:;

VV.VVOD_SPISOK_zapis_data(0,soob1);
VV.VVOD_SPISOK_zapis_data(1,soob2);

helstr(LINES-1,0,"F2/+",gettext("печать"),
"F10",gettext("выход"),NULL);

VV.VVOD_SPISOK_add_MD(gettext("Первая строка.."));
VV.VVOD_SPISOK_add_MD(gettext("Вторая строка.."));

//kom=vvod(&LL,0,1,3);
kom=VV.vvod(0,1,1,-1,-1);

strcpy(soob1,VV.VVOD_SPISOK_return_data(0));
strcpy(soob2,VV.VVOD_SPISOK_return_data(1));

switch (kom)
 {
  case ESC:
  case FK10:
    return;
  case FK2:
  case PLU:
    break;
 }


GDITE();
if(VV.VVOD_SPISOK_return_data(0)[0] != '\0')
if(mariqtxt(kasr, 1, VV.VVOD_SPISOK_return_data(0)) != 0)
  goto naz;
if(VV.VVOD_SPISOK_return_data(1)[0] != '\0')
if(mariqtxt(kasr, 2, VV.VVOD_SPISOK_return_data(1)) != 0)
  goto naz;


}

int             main(int argc,char **argv,char **envp)
{
struct  passwd  *ktor; /*Кто работает*/

masiv_din_int MKN(0); //Только для того чтобы строилась программа

ktor=getpwuid(getuid());
ktorab=ktor->pw_uid;

(void)setlocale(LC_ALL,"");
(void)setlocale(LC_NUMERIC,"C"); //Переменная среды определяющая чем 
                                 //разделяется целая и дробная часть числа


init_screen();  /*Запуск экранного режима*/

#ifdef HAVE_TORGDEV
umask(0117); /*Установка маски для записи и чтения группы*/

/*Читаем ключи запуска программы*/
readkey(argc,argv,&parol,&imabaz,&host,&regim);


/* выбрать кассовый аппарат из меню */
while (nastrmariq(0, &kasr) == 0) {

/*
printw("handle = %d; sklad = %d; nomer = %d; \n\
host = %s; password = %s; port = %s; prod = %s;\n",
kasr.handle, kasr.sklad, kasr.nomer, kasr.host, kasr.parol, kasr.port, kasr.prodavec);
OSTANOV();
*/
   if ((kasr.handle=iceb_open_server(kasr.host, kasr.port)) < 0) errorkan(kasr.handle);
   else {
      VVOD MENU(4);
      Model = iceb_model(kasr.handle);

      sprintf(strsql, "%s %s.", 
      	gettext("Программа настройки кассового регистратора"), 
      	type2name(Model));
      	
      MENU.VVOD_SPISOK_add_ZAG(strsql);

      if (Model & M_MARIA) 
         MENU.VVOD_SPISOK_add_MD(gettext("Установка времени."));
      else
         MENU.VVOD_SPISOK_add_MD("");
      if (Model & M_MARIA) 
         MENU.VVOD_SPISOK_add_MD(gettext("Печать дневного отчётного чека."));
      else
         MENU.VVOD_SPISOK_add_MD("");
      if (Model & M_MARIA) 
         MENU.VVOD_SPISOK_add_MD(gettext("Печать отчётного чека за период."));
      else
         MENU.VVOD_SPISOK_add_MD("");
      if (Model & (M_MARIA | M_AMC100F)) 
         MENU.VVOD_SPISOK_add_MD(gettext("Формирование внешнего вида чека."));
      else
         MENU.VVOD_SPISOK_add_MD("");
      if (Model & M_MARIA) 
         MENU.VVOD_SPISOK_add_MD(gettext("Програмировать налог."));
      else
         MENU.VVOD_SPISOK_add_MD("");
      if (Model & M_MARIA) 
         MENU.VVOD_SPISOK_add_MD(gettext("Програмировать реги-нную информацию о владельце."));
      else
         MENU.VVOD_SPISOK_add_MD("");
      if (Model & M_MARIA) 
         MENU.VVOD_SPISOK_add_MD(gettext("Програмировать валюту."));
      else
         MENU.VVOD_SPISOK_add_MD("");
      if (Model & M_MARIA) 
         MENU.VVOD_SPISOK_add_MD(gettext("Напечатать пустой чек."));
      else
         MENU.VVOD_SPISOK_add_MD("");
      if (Model & M_MARIA) 
         MENU.VVOD_SPISOK_add_MD(gettext("Распечатать текст на индикаторе регистратора."));
      else
         MENU.VVOD_SPISOK_add_MD("");
      if (Model & M_MARIA) 
         MENU.VVOD_SPISOK_add_MD(gettext("Очистить индикатор кассового регистратора."));
      else
         MENU.VVOD_SPISOK_add_MD("");
      if (Model & M_MARIA) 
         MENU.VVOD_SPISOK_add_MD(gettext("Служебное внесение."));
      else
         MENU.VVOD_SPISOK_add_MD("");
      if (Model & M_MARIA) 
         MENU.VVOD_SPISOK_add_MD(gettext("Служебное изъятие."));
      else
         MENU.VVOD_SPISOK_add_MD("");
      MENU.VVOD_SPISOK_add_MD(gettext("Конец работы."));

      for(;;)
       {

        clear(); /*Очистить экран и закрасить фоновым цветом*/

        znak(0,COLS-20);
        mvprintw(Y=15,X=COLS-20,"Хост:%s",kasr.host);
        mvprintw(++Y,X,"Продавец:%s",kasr.prodavec);
        mvprintw(++Y,X,"Пароль:%s",kasr.parol);
        
        if(menu3(&MENU,&kom,1,0,0) != 0)
          continue;

        switch (kom)
         {
          case 12 :
          case -1 :
            goto kon;
            break;

          case 0 :
            settimmar(kasr);
            break;

          case 1 :
            printdayshek(kasr);
            break;

          case 2 :
            printpershek(kasr);
            break;
          case 3 :
            vidshek(kasr);
            break;
          case 4 :
            prognal(kasr);
            break;
          case 5 :
            progreginf(kasr);
            break;
          case 6 :
            progval(kasr);
            break;

          case 7 :
            GDITE();
              sprintf(strsql,"FSTART: MARIA_NULL\n\
%s\n\
%s\n\
FEND:\n",
              kasr.prodavec, kasr.parol);  
/*
printw("%s",strsql);
refresh();
*/
            /* отправим команду серверу	*/
            iceb_cmd(kasr, strsql);
            sleep(1);
            break;

          case 8 :
            rastxtmar(kasr);
            break;

          case 9 :
            GDITE();
            mariqtxt(kasr, 1, "");
            break;

          case 10 :
            marsiv(kasr,0);
            break;

          case 11 :
            marsiv(kasr, 1);
            break;

          default:
            kom=0;
            break;
         }
       }

      kon:;


      iceb_close_server(kasr.handle);
   }
 }
#endif
refresh();
endwin();
}

