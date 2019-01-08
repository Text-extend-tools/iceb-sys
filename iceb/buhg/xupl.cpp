/*$Id: xupl.c,v 5.142 2014/05/07 10:57:24 sasa Exp $*/
/*23.05.2018	21.03.2003	Балых А.И.	xupl.c
Подсистема "Учет путевых листов"
*/
#define DVERSIQ "23.05.2018"
#include        "buhg.h"

void		vnsi(void);
int		uplnast(void);
void		dirupls(void);
void		rabdok(void);
void		diruplds(short,short,short);
void		poiupldok(void);
int uplrpl(int,class spis_oth*);
void		rasoth(void);
int uplvstpsh(class spis_oth*);
int uplvst_kg(class spis_oth*);
void            dirscmt(void);
int upl_obvm(class spis_oth*);
void upl_plc(void);
void saldo_upl();
void saldo_upl();
void xupl_saldo();
void xupl_start();
void xupl_inst();
void upludgod();
void uplgods();

extern int	iscolor;
extern char	*imabaz;
extern char	*host;
extern char	*parol_pol;
SQL_baza	bd;
extern short    startgod;
extern int iceb_kod_podsystem;

int             main(int argc,char **argv,char **envp)
{
iceb_kod_podsystem=ICEB_PS_UPL;
int             kom=0;

icebstart(argc,argv);
xupl_start();

VVOD MENU(4);

int uid=0;
class iceb_tu_str fioop;

uid=iceb_t_getuid();
fioop.plus(iceb_t_getfioop());

for(;;)
 {

  clear(); /*Очистить экран и закрасить фоновым цветом*/
  MENU.VVOD_delete();

  MENU.VVOD_SPISOK_add_ZAG(gettext("Учет путевых листов"));
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
      rabdok();
      break;


    case 1:
      vnsi();
      break;

    case 2:
      rasoth();
      break;

    case 3:
      xupl_inst();
      break;
      
    default:
       kom=0;
       break;   
   }
 }

kon:;
exit_iceb(0);

}

/*******************************************************/
/*Ввод и корриктировка нормативно-справочной информации*/
/*******************************************************/

void	vnsi()
{
int		kom=0;
class iceb_tu_str kodn("");
class iceb_tu_str naimn("");
int		poz,komv;

VVOD VVOD1(2);

VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Учет путевых листов"));
MENU.VVOD_SPISOK_add_ZAG(gettext("Ввод и корректировка нормативно-справочной информации"));

MENU.VVOD_SPISOK_add_MD(gettext("Справочник подразделений"));          //0
MENU.VVOD_SPISOK_add_MD(gettext("Справочник марок топлива"));         //1
MENU.VVOD_SPISOK_add_MD(gettext("Справочник объектов учета топлива"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Справочник норм списания топлива"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Справочник автомобилей"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Справочник стартовых сальдо"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Файл настройки"));//6
MENU.VVOD_SPISOK_add_MD(gettext("Справочник средних цен на топливо и коэфф. перевода в Кг"));//7
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//8

for(;;)
 {
  clear();

  while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

  switch (kom)
   {
    case 8:
    case -1:
      return;
      break;

    case 0:
      clear();
      naimn.new_plus("");
      poz=komv=0;
      dirmat1("Uplpodr",&kodn,&naimn,LINES-5,0,2,0,&poz,&komv,"","\0");
      break;

    case 1:
      kodn.new_plus("");
      dirmt(&kodn,0);
      break;

    case 2:
      clear();
      naimn.new_plus("");
      dirout(&kodn,&naimn,0,0);
      break;
      
    case 3 :
      diruplnst(0,&kodn,&naimn);
      break;


    case 4:
      clear();
      naimn.new_plus("");
      diravt0(&kodn,&naimn,0);
      break;
      
    case 5:
      dirupls();
      break;

    case 6 :
      iceb_redfil("uplnast.alx",0);
      uplnast();
      break;

    case 7 :
      dirscmt();
      break;


    default:
      kom=0;
      break;
   }
 }
}
/**********************************/
/*Работа с документами            */
/**********************************/

void   rabdok()
{
char	bros[512];
int     kom=0;
short dd=0,md=0,gd=0;
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

class iceb_tu_str nomd("");
class iceb_tu_str podr("");

VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Работа с документами"));

MENU.VVOD_SPISOK_add_MD(gettext("Ввод нового путевого листа"));       //0
MENU.VVOD_SPISOK_add_MD(gettext("Поиск введеного документа"));          //1
MENU.VVOD_SPISOK_add_MD(gettext("Просмотр всех введеных документов"));  //2
sprintf(bros,"%s %d.%d.%d%s",gettext("Просмотр документов начиная с"),   //3
dt,mt,gt,gettext("г."));
MENU.VVOD_SPISOK_add_MD(bros);
MENU.VVOD_SPISOK_add_MD(gettext("Установить/снять блокировку дат"));    //4
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));                               //5

for(;;)
 {
  clear();

  while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

  switch (kom)
   {
    case 5:
    case -1:
      return;
      break;

    case 0:
      dd=md=gd=0;
      nomd.new_plus("");
      podr.new_plus("");
      if(vdupl1(&dd,&md,&gd,&nomd,&podr) == 0)
        dirupldok3(dd,md,gd,nomd.ravno(),podr.ravno_atoi());
      break;

    case 1:
      poiupldok();
      break;

    case 2 :
      diruplds(0,0,0);
      break;

    case 3 :
      diruplds(dt,mt,gt);
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
/*****************************/
/*распечатка отчётов         */
/*****************************/

void        rasoth()
{
int		kom=0;

VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);

MENU.VVOD_SPISOK_add_MD(gettext("Распечатать реестр путевых листов по водителям"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать реестр путевых листов по автомобилям"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать оборотную ведомость движения топлива по автомобилям"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать ведомость списания топлива по водителям и счетам"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать ведомость списания топлива по счетам в киллограммах"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать оборотную ведомость движения топлива"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//6

for(;;)
 {
  clear();

  while(menu3w(stdscr,&MENU,&kom,-1,-1,1) != 0);
  class spis_oth oth;

  switch (kom)
   {
    case 6 :
    case -1 :
      return;

    case 0 :
      if(uplrpl(0,&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;
    case 1 :
      if(uplrpl(2,&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 2 :
      if(uplrpl(1,&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 3 :

      if(uplvstpsh(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 4 :
      if(uplvst_kg(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 5 :
      if(upl_obvm(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

      
    default:
      break;
   }
 }
}
/**************************************/
/*Проверка переноса сальдо*/
/******************************/
void xupl_saldo()
{
char strsql[512];
short dt=0,mt=0,gt=0;
poltekdat(&dt,&mt,&gt);

if(mt <= 2)
 return;

class iceb_tu_str god("");

iceb_t_poldan("Стартовый год",&god,"uplnast.alx");

if(god.ravno_atoi() == gt)
  return;

if(god.ravno_atoi() > gt)
 {
  sprintf(strsql,"%s! %d > %d",gettext("Стартовый год больше текущего года"),god.ravno_atoi(),gt);
  iceb_t_soob(strsql);
  return;
 }

if(god.ravno_atoi() < gt)
 {
  iceb_t_soob(gettext("Необходимо перенести стартовые остатки топлива!"));
  return;
 }


}
/*******************************/
/*старт программы*/
/******************************/
void xupl_start()
{
/*Ищем настройку*/

if(nastsys() != 0)
  exit_iceb(1);
if(uplnast() != 0)
  exit_iceb(1);

iceb_t_perzap(1); /*Проверка на первичный запуск в текущем дне и удаление *.lst */

iceb_t_blokps(); /*автоматическая блокировка подсистемы*/
xupl_saldo();  /*проверка переноса стартовых остатков топлива*/
}

/****************************************/
/*Удаление за год*/
/******************************************/
void xupl_udgod()
{
int kom=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Инструменты"));

MENU.VVOD_SPISOK_add_MD(gettext("Удаление путевых листов за год"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка годов за которые есть путевые листы"));//1
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
      upludgod();
      break;

    case 1:
      uplgods();
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
void xupl_inst()
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
MENU.VVOD_SPISOK_add_MD(gettext("Перенос стартовых остатков топлива"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Удаление документов за год"));//6
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//7

for(;;)
 {
  clear();

  if(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0)
   continue;
  class spis_oth oth;   
  switch (kom)
   {
    case 7:
    case -1 :
      return;

    case 0:

      dirlst("lst,txt,csv,xml,dbf");
      break;


    case 1:
      if(smenabaz(1) == 0)
       {
        xupl_start();
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
      upl_plc();
      return;


    case 5:
      saldo_upl();
      break;

    case 6:
      xupl_udgod();
      break;
      
    default:
      kom=0;
      break;
   }
 }

}
