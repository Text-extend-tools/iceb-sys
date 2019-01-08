/*$Id: xukr.c,v 5.96 2014/05/07 10:57:24 sasa Exp $*/
/*23.05.2018	22.10.2002	Белых А.И.	xukr.c
Подсистема "Учет командировочных расходов"
*/
#define DVERSIQ "06.12.2018"
#include        "buhg.h"

void xukrnsi(void);
int  ukrnast(void);
void xukrrsd(void);
void poikrdok(void);
void dirdokskr(int);
int  rppvokr(class spis_oth*);
void xukrro(void);
int  ukrras(class spis_oth*);
int  kprik(class spis_oth*);
int  ukrskk(class spis_oth*);
int  ukrspdok(class spis_oth*);
int  ukrspdok1(class spis_oth*);
int  ukrplc(class spis_oth*);
int  xukropp(class spis_oth*);
void xukr_sg();
void xukr_start();
void xukr_inst();
void ukrudgod();
void ukrgods();

extern int	iscolor;
extern char	*imabaz;
extern char	*host;
extern char	*parol_pol;
SQL_baza	bd;
extern short    startgod;
extern int iceb_kod_podsystem;

int             main(int argc,char **argv,char **envp)
{
iceb_kod_podsystem=ICEB_PS_UKR;
int             kom;

icebstart(argc,argv);
xukr_start();

VVOD MENU(4);


int uid=0;
class iceb_tu_str fioop;

uid=iceb_t_getuid();
fioop.plus(iceb_t_getfioop());


kom = 0;

for(;;)
 {

  clear(); /*Очистить экран и закрасить фоновым цветом*/
  MENU.VVOD_delete();

  MENU.VVOD_SPISOK_add_ZAG(gettext("Учет командировочных расходов"));
  MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);

  MENU.VVOD_SPISOK_add_MD(gettext("Работа с документами"));//0
  MENU.VVOD_SPISOK_add_MD(gettext("Ввод нормативно-справочной информации"));//1
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатать отчёты"));//2
  MENU.VVOD_SPISOK_add_MD(gettext("Инструменты"));//3
  MENU.VVOD_SPISOK_add_MD(gettext("Конец работы"));//4

  znak(0,COLS-26);
  infosys(12,COLS-27,host,VERSION,DVERSIQ,0,0,0,imabaz,uid,fioop.ravno());


  if(menu3(&MENU,&kom,1,0,0) != 0)
   continue;

  switch (kom)
   {

    case 4:
    case -1 :
      goto kon;
      break;

    case 0:
      xukrrsd();
      break;

    case 1:
      xukrnsi();
      break;

    case 2:
      xukrro();
      break;
      
    case 3:
      xukr_inst();
      break;

    default:
       kom=0;
       break;   
   }
 }

kon:;

exit_iceb(0);
}

/***************************************/
/*Ввод нормативно-справочной информации*/
/***************************************/

void xukrnsi()
{
int		kom=0;
int		poz,komv;
class iceb_tu_str kodn("");
class iceb_tu_str naimn("");
VVOD VVOD1(2);

VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Учет командировочных расходов"));
MENU.VVOD_SPISOK_add_ZAG(gettext("Ввод и корректировка нормативно-справочной информации"));

MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка групп командивочных расходов"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка списка единиц измерения"));  //1
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка расходов на командировку"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка видов командировок"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка списка контрагентов"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Работа с файлом настройки"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Настpойка автоматического выполнения пpоводок"));//6
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//7

for(;;)
 {
  clear();

  while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

  switch (kom)
   {
    case 7:
    case -1:
      return;
      break;

    case 0:
      clear();
      naimn.new_plus("");
      poz=komv=0;
      dirmat1("Ukrgrup",&kodn,&naimn,LINES-5,0,2,0,&poz,&komv,"","\0");
      break;

    case 1:
      naimn.new_plus("");
      iceb_tl_ei(0,&kodn,&naimn);
      break;

    case 2:
      naimn.new_plus("");
      dirkkr(&kodn,&naimn,0);
      break;

    case 3:
      clear();
      naimn.new_plus("");
      poz=komv=0;
      dirmat1("Ukrvkr",&kodn,&naimn,LINES-5,0,2,0,&poz,&komv,"","\0");
      break;

    case 4:
      clear(); /*Очистить экран */
      naimn.new_plus("");
      dirkontr(&kodn,&naimn,0);
      break;

      
    case 5:
      if(iceb_redfil("ukrnast.alx",0) == 0)
        ukrnast();
      break;

    case 6 :
      iceb_redfil("avtprukr.alx",0);
      break;

    default:
      kom=0;
      break;
  }
 }
}

/*******************************/
/*Работа с документами         */
/*******************************/

void	xukrrsd()
{
int             kom=0;
char		bros[512];
short		dd,md,gd;
class iceb_tu_str nomdok("");
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
VVOD MENU(3);

MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Работа с документами командировочных расходов"));

MENU.VVOD_SPISOK_add_MD(gettext("Ввод нового документа"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Поиск введённого документа"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Просмотр всех введеных документов"));//2
sprintf(bros,"%s %d.%d.%d%s",gettext("Просмотр документов начиная с"),dt,mt,gt,gettext("г."));//3
MENU.VVOD_SPISOK_add_MD(bros);
MENU.VVOD_SPISOK_add_MD(gettext("Установить/снять блокировку дат"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//5

kom=0;

for(;;)
 {
  clear();

  while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

  class spis_oth oth;
  switch (kom)
   {
    case 5:
    case -1 :
      return;
      break;

    case 0 :
      nomdok.new_plus("");
      dd=md=gd=0;
      if(vdkr(&dd,&md,&gd,&nomdok) == 0)
        dirdokkr(dd,md,gd,&nomdok);
      break;

    case 1 :
      poikrdok();
      break;

    case 2 :
      dirdokskr(0);
      break;

    case 3 :
      dirdokskr(1);
      break;

    case 4 :
      iceb_tl_blok();
       break;

    default:
      kom=0;
      break;
   }
 }

}

/***********************************/
/*Распечатка отчётов               */
/***********************************/

void		xukrro()
{
int		kom;

VVOD MENU(3);

kom=0;
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);

MENU.VVOD_SPISOK_add_MD(gettext("Распечатать реестр проводок по видам командировок"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать свод по контрагентам"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка приказов на командировки"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать свод счёт-код расхода"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать список документов по датам документов"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать список документов по датам приказов"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать свод расходов по поставщикам"));//6
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//7


while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
class spis_oth oth;
switch (kom)
 {
  case 7:
  case -1 :
    return;


  case 0:
   if(rppvokr(&oth) == 0)
    iceb_t_rabfil(&oth,"");
   break;

  case 1:
   if(ukrras(&oth) == 0)
     iceb_t_rabfil(&oth,"");
   break;

  case 2:
   if(kprik(&oth) == 0)
    iceb_t_rabfil(&oth,"");
   break;

  case 3:
   if(ukrskk(&oth) == 0)
    iceb_t_rabfil(&oth,"");
   break;

  case 4:
   if(ukrspdok(&oth) == 0)
    iceb_t_rabfil(&oth,"");
   break;

  case 5:
   if(ukrspdok1(&oth) == 0)
    iceb_t_rabfil(&oth,"");
   break;

  case 6:
   if(xukropp(&oth) == 0)
    iceb_t_rabfil(&oth,"");
   break;

  default:
    kom=0;
    break;
 }

}
/****************************/
/*Изменение стартового года*/
/*****************************/
void xukr_sg()
{
char strsql[512];
short dt=0,mt=0,gt=0;
poltekdat(&dt,&mt,&gt);

if(mt <= 2)
 return;

class iceb_tu_str god("");

iceb_t_poldan("Стартовый год",&god,"ukrnast.alx");

if(god.ravno_atoi() == gt)
  return;

if(god.ravno_atoi() > gt)
 {
  sprintf(strsql,"%s! %d > %d",gettext("Стартовый год больше текущего года"),god.ravno_atoi(),gt);
  iceb_t_soob(strsql);
  return;
 }
extern short startgod;
startgod=gt;
sprintf(strsql,"update Alx set str='Стартовый год|%d\n',ktoz=%d,vrem=%ld where fil='ukrnast.alx' and str like 'Стартовый год|%%'",gt,ICEB_KOD_OP_PROG,time(NULL));
//sql_zapis(strsql,1,0);
if(sql_zap(&bd,strsql) != 0)
 {
  int kod_oh=sql_nerror(&bd);
  if(kod_oh == ER_DBACCESS_DENIED_ERROR) //Только чтение
   {
    return;
   }

  msql_error(&bd,"__FUNCTION__",strsql);
  return;
 }

}
/******************************/
/*Старт программы*/
/*******************************/
void xukr_start()
{
/*Ищем настройку*/

if(nastsys() != 0)
  exit_iceb(1);
if(ukrnast() != 0)
  exit_iceb(1);

iceb_t_perzap(1); /*Проверка на первичный запуск в текущем дне и удаление *.lst */

iceb_t_blokps(); /*автоматическая блокировка подсистемы*/
xukr_sg(); /*Установка стартового года*/
}
/****************************************/
/*Удаление за год*/
/******************************************/
void xukr_udgod()
{
int kom=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Инструменты"));

MENU.VVOD_SPISOK_add_MD(gettext("Удаление документов за год"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка годов за которые есть документы"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2

for(;;)
 {
  clear();

  if(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0)
   continue;
  class spis_oth oth;   
  switch (kom)
   {
    case 2:
    case -1 :
      return;

    case 0:
      ukrudgod();
      break;

    case 1:
      ukrgods();
      break;

    default:
      kom=0;
      break;
   }
 }
}
/****************************************/
/*Инструменты*/
/******************************************/
void xukr_inst()
{
int kom=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Инструменты"));

MENU.VVOD_SPISOK_add_MD(gettext("Работа с файлами распечаток"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Сменить базу"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Сменить принтер"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Дополнительные программы"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Проверка логической целосности базы данных"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Удаление документов за год"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//6

for(;;)
 {
  clear();

  if(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0)
   continue;
  class spis_oth oth;   
  switch (kom)
   {
    case 6:
    case -1 :
      return;

    case 0:

      dirlst("lst,txt,csv,xml,dbf");
      break;


    case 1:
      if(smenabaz(1) == 0)
       {
        xukr_start();
        return;
       }
      break;

    case 2:
       mprintcap();
       break;

    case 3:
       if(iceb_t_menudpr(1) != 0)
        {
         class iceb_tu_str rp(gettext("Не найдена наcтройка для дополнительных программ!"));
         rp.ps_plus(gettext("Файл"));
         rp.plus(":",g_get_home_dir());
         rp.plus("/.iceB/menu_dpr.alx");
         iceb_t_soob(rp.ravno());
         }
       break;

    case 4:
      if(ukrplc(&oth) == 0)
        iceb_t_rabfil(&oth,"");
      break;

    case 5:
      xukr_udgod();
      break; 

    default:
      kom=0;
      break;
   }
 }

}
