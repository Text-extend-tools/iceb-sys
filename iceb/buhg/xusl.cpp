/*$Id: xusl.c,v 5.212 2014/05/07 10:57:24 sasa Exp $*/
/*23.05.2018 	10.02.2000	Белых А.И.	xusl.c
Программа выписки счетов на услуги
*/
#define DVERSIQ "06.12.2018"
#include        "buhg.h"

void		normspi(void);
void            rabdoku(void);
void		rasotus(void);
int		uslnast(void);
void		poiusldok(void);
int  dvuslf2(class spis_oth*);
int  rasspdoku(class spis_oth*);
int  rppvou(class spis_oth*);
void		uddusl(void);
void 		impusldok(void);
int  opss_u(int,class spis_oth*);
int  xuslplc(class spis_oth*);
int usldupk(class spis_oth *oth);
int uslspo(class spis_oth *oth);
void xusl_sg();
void xusl_start();
void xusl_inst();

class iceb_tu_str kat_for_nal_nak("");
extern char	*imabaz;
extern char	*host;
extern char	*parol_pol;
extern short	startgodus; /*Стартовый год просмотров услуг*/
SQL_baza	bd;
int             kassa=0;
extern int iceb_kod_podsystem;

int  main(int argc,char *argv[],char *envp[])
{
iceb_kod_podsystem=ICEB_PS_UU;
int             kom=0;

icebstart(argc,argv);

xusl_start();

VVOD MENU(4);


MENU.VVOD_SPISOK_add_MD(gettext("Работа с документами"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Ввод нормативно-справочной информации"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать отчёты"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Инструменты"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Конец работы"));//4

int uid=0;
class iceb_tu_str fioop;

uid=iceb_t_getuid();
fioop.plus(iceb_t_getfioop());

for(;;)
 {

  clear();

  MENU.VVOD_delete_ZAG();
  MENU.VVOD_SPISOK_add_ZAG(gettext("Учёт услуг"));
  MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);

  znak(0,COLS-26);
  infosys(12,COLS-27,host,VERSION,DVERSIQ,0,0,0,imabaz,uid,fioop.ravno());

  if(menu3(&MENU,&kom,1,0,0) != 0)
    continue;

  switch (kom)
   {
    case 4 :
    case -1 :
      goto kon;
      break;

    case 0:
      rabdoku();
      break;

    case 2:
      rasotus();
      break;

    case 1:
      normspi();
      break;

    case 3:
      xusl_inst();
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

void		normspi()
{
int             kom=0;
int		poz,komv;
int		kodgr,kodus;
class iceb_tu_str kodn("");
class iceb_tu_str naim("");
VVOD MENU(3);

kom=0;
for(;;)
 {

  clear();

  MENU.VVOD_delete();
  MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
  MENU.VVOD_SPISOK_add_ZAG(gettext("Ввод и корректировка нормативно-справочной информации"));

  MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка списка групп услуг"));//0
  MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка списка единиц измерения"));//1
  MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка списка услуг"));//2
  MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка списка операций на приход"));//3
  MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка списка операций на расход"));//4
  MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка списка подразделений"));//5
  MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка списка форм оплаты"));//6
  MENU.VVOD_SPISOK_add_MD(gettext("Работа со списком контрагентов"));//7
  MENU.VVOD_SPISOK_add_MD(gettext("Работа с планом счетов"));//8
  MENU.VVOD_SPISOK_add_MD(gettext("Работа с файлом настройки"));//9
  MENU.VVOD_SPISOK_add_MD(gettext("Работа со списком коректировок на сумму документа"));//10
  MENU.VVOD_SPISOK_add_MD(gettext("Настpойка автоматического выполнения пpоводок"));//11
  MENU.VVOD_SPISOK_add_MD(gettext("Ввод и коректировка концовки счёта-фактури"));//12
  MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//13

  while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

  switch (kom)
   {
    case 13 :
    case -1 :
      return;
    case 0 :
      clear();
      kodn.new_plus("");
      naim.new_plus("");
      poz=komv=0;
      dirmat1("Uslgrup",&kodn,&naim,LINES-5,0,2,0,&poz,&komv,"","\0");
      break;

    case 2 :
      kodgr=kodus=0;
      naim.new_plus("");
      dirusl(&kodgr,&kodus,&naim,0,"");
      break;

    case 1 :
      kodn.new_plus("");
      naim.new_plus("");
      iceb_tl_ei(0,&kodn,&naim); 
      break;

    case 3 :
      diruslop1(0,&kodn,&naim);
      break;

    case 4 :
      diruslop2(0,&kodn,&naim); 
      break;

   case 5 :
      clear();
      kodn.new_plus("");
      naim.new_plus("");
      dirpodrusl(0,&kodn,&naim);
       
 
      break;

    case 6 :
      clear();
      kodn.new_plus("");
      naim.new_plus("");
      poz=komv=0;
      dirmat1("Foroplat",&kodn,&naim,LINES-5,0,2,0,&poz,&komv,"","\0");

      break;

    case 7 :
      clear();
      naim.new_plus("");
      dirkontr(&kodn,&naim,0);

      break;

    case 8 :
      clear();
      kodn.new_plus("");
      naim.new_plus("");
      dirshet(&kodn,&naim,0);

      break;

    case 9 :
      if(iceb_redfil("uslnast.alx",0) == 0)
        uslnast();
      break;

    case 10 :
      iceb_redfil("uslkor.alx",0);
      break;

    case 11 :
      menuvnp(2,0);
      break;

    case 12 :
      iceb_redfil("usl_shet_fak_end.alx",0);
      break;

    default:
      kom=0;
      break;
   }
 }
}
/********************/
/*Работа с документами*/
/********************/
void            rabdoku()
{
int             kom;
int		podr=0;
short		godn;
short		dd,md,gd;
char		strsql[512];
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
VVOD MENU(3);
VVOD VV(0);
godn=startgodus;
if(godn == 0)
 godn=gt;

kom=0;

MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Работа с документами"));

MENU.VVOD_SPISOK_add_MD(gettext("Ввод нового документа на оказание услуг"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Ввод нового документа на получение услуг"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Поиск введеного документа"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Просмотр всех введеных документов"));//3
sprintf(strsql,"%s %d.%d.%d%s",gettext("Просмотр документов начиная с"),//4
  dt,mt,gt,gettext("г."));
MENU.VVOD_SPISOK_add_MD(strsql);
MENU.VVOD_SPISOK_add_MD(gettext("Установить/снять блокировку дат"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Импорт документов"));//6
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//7



for(;;)
 {
  clear();

  while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
  
  dd=md=gd=0;
  class iceb_tu_str nomdok("");
  class spis_oth oth;
  switch (kom)
   {
    case 7:
    case -1 :
      return;
      break;

    case 0 :

      if(vdnu(&dd,&md,&gd,&podr,&nomdok,2) == 0)
        dirudok(dd,md,gd,2,podr,nomdok.ravno());

      break;

    case 1 :
      if(vdnu(&dd,&md,&gd,&podr,&nomdok,1) == 0)
        dirudok(dd,md,gd,1,podr,nomdok.ravno());

      break;

    case 2 :
      poiusldok();
      break;

    case 3 :
      dirusdok(0,0,godn);
      break;

    case 4 :
      dirusdok(dt,mt,gt);
      break;

    case 5 :
      iceb_tl_blok();
       break;


    case 6 :
       impusldok();
       break;


    default:
      kom=0;
      break;
   }
 }
}

/********************/
/*Распечатка отчётов*/
/**********************/

void		rasotus()
{
int		kom;

VVOD MENU(3);

kom=0;
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);

MENU.VVOD_SPISOK_add_MD(gettext("Распечатать движение услуг"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать список документов"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать реестр проводок по видам операций"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать движение услуг по счетам получения"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать движение услуг по счетам списания"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать списание/получение услуг по контрагентам"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать свод услуг по оплаченным документам"));//6
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//7


while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
class spis_oth oth;
switch (kom)
 {
  case 7 :
  case -1 :
    return;


  case 0:
   if(dvuslf2(&oth) == 0)
    iceb_t_rabfil(&oth,"");
   break;

  case 1:
   if(rasspdoku(&oth) == 0)
    iceb_t_rabfil(&oth,"");
   break;

  case 2:
   if(rppvou(&oth) == 0)
    iceb_t_rabfil(&oth,"");
   break;


  case 3:
   if(opss_u(1,&oth) == 0)
    iceb_t_rabfil(&oth,"");
   break;

  case 4:
   if(opss_u(2,&oth) == 0)
    iceb_t_rabfil(&oth,"");
   break;

  case 5:
    if(usldupk(&oth) == 0)
      iceb_t_rabfil(&oth,"");
    break;  

  case 6:
    if(uslspo(&oth) == 0)
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
void xusl_sg()
{
char strsql[512];
short dt=0,mt=0,gt=0;
poltekdat(&dt,&mt,&gt);

if(mt <= 2)
 return;

class iceb_tu_str god("");

iceb_t_poldan("Стартовый год",&god,"uslnast.alx");

if(god.ravno_atoi() == gt)
  return;

if(god.ravno_atoi() > gt)
 {
  sprintf(strsql,"%s! %d > %d",gettext("Стартовый год больше текущего года"),god.ravno_atoi(),gt);
  iceb_t_soob(strsql);
  return;
 }
extern short startgodus; /*Стартовый год просмотров услуг*/
startgodus=gt;

sprintf(strsql,"update Alx set str='Стартовый год|%d\n',ktoz=%d,vrem=%ld where fil='uslnast.alx' and str like 'Стартовый год|%%'",gt,ICEB_KOD_OP_PROG,time(NULL));
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
/*********************************/
/*Старт программы*/
/***********************/
void xusl_start()
{
/*Ищем настройку*/
if(nastsys() != 0)
  exit_iceb(1);

if(uslnast() != 0)
 exit_iceb(1);

if(iceb_t_perzap(1) != 0) /*Проверка на первичный запуск в текущем дне и удаление *.lst */
 exit_iceb(1);

iceb_t_blokps(); /*автоматическая блокировка подсистемы*/
xusl_sg(); /*установка стартового года*/

}
/****************************************/
/*Удаление за год*/
/******************************************/
void xusl_udgod()
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
      uddusl();
      break;

    case 1:
     poigod(8);
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
void xusl_inst()
{
int kom=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Инструменты"));

MENU.VVOD_SPISOK_add_MD(gettext("Работа с файлами распечаток"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Сменить базу"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Сменить принтер"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Дополнительные программы"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Удаление всех документов за год"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Проверка логической целосности базы данных"));//5
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
        xusl_start();
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

    case 4 :
       xusl_udgod();
       break;

    case 5:
     if(xuslplc(&oth) == 0)
      iceb_t_rabfil(&oth,"");
     break;

    default:
      kom=0;
      break;
   }
 }

}
