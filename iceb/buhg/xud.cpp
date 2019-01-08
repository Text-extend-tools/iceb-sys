/*$Id: xud.c,v 5.35 2014/08/31 06:18:18 sasa Exp $*/
/*23.05.2018	23.03.2009	Белых А.И.	xud.c
Учёт доверенностей
*/
#define DVERSIQ "23.05.2018"
#include "buhg.h"

void poidov();
void dirdovs(short dnp,short mnp,short gnp);
void xud_sg();
void xud_start();
void xud_inst();
void xududgod();
void xudgods();

extern int	iscolor;
extern char	*imabaz;
extern char	*host;
extern char	*parol_pol;
int		kassa=0;
SQL_baza	bd;
extern int iceb_kod_podsystem;

void xud_rsd();

int             main(int argc,char **argv,char **envp)
{
iceb_kod_podsystem=ICEB_PS_UD;
int             kom;

icebstart(argc,argv);
xud_start();
VVOD MENU(4);



int uid=0;
class iceb_tu_str fioop;

uid=iceb_t_getuid();
fioop.plus(iceb_t_getfioop());

kom = 0;

for(;;)
 {
  clear();

  MENU.VVOD_delete();

  MENU.VVOD_SPISOK_add_ZAG(gettext("Учёт доверенностей"));
  MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
   
  MENU.VVOD_SPISOK_add_MD(gettext("Работа с доверенностями"));//0
  MENU.VVOD_SPISOK_add_MD(gettext("Работа с файлом настройки"));//1
  MENU.VVOD_SPISOK_add_MD(gettext("Работа с файлом настройки формы печати"));//2
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
      exit_iceb(0);
      break;

    case 0 :
      xud_rsd();
      break;

    case 1 :
      iceb_redfil("nastud.alx",0);
      break;

    case 2 :
      iceb_redfil("xudrasdok.alx",0);
      break;

    case 3:
      xud_inst();
      break;

    default:
       kom=0;
       break;   
   }
 }
exit_iceb(0);

}
/*******************************/
/*Работа с доверенностями*/
/****************************/
void xud_rsd()
{
int             kom;
char		bros[512];
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
VVOD MENU(3);

MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Работа с доверенностями"));

MENU.VVOD_SPISOK_add_MD(gettext("Ввод нового документа"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Поиск введеного документа"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Просмотр всех введеных документов"));//2
sprintf(bros,"%s %d.%d.%d%s",gettext("Просмотр документов начиная с"),\
dt,mt,gt,gettext("г."));//3
MENU.VVOD_SPISOK_add_MD(bros);
MENU.VVOD_SPISOK_add_MD(gettext("Установить/снять блокировку дат"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//5

kom=0;
class iceb_tu_str datdov("");
class iceb_tu_str ndov("");
for(;;)
 {
  clear();

  while(menu3(&MENU,&kom,1,-1,-1) != 0);


  switch (kom)
   {
    case 5:
    case -1 :
      return;
      break;

    case 0 :
      if(udvhd(&datdov,&ndov) == 0)
       dirdov(&datdov,&ndov);
       
      break;

    case 1:
      poidov();
      break;

    case 2:
      dirdovs(0,0,0);
      break;

    case 3:
      dirdovs(dt,mt,gt);
      break;
            
    case 4:
      iceb_tl_blok();
      break;

    default:
      kom=0;
      break;
   }
 }

}
/****************************/
/*Изменение стартового года*/
/*****************************/
void xud_sg()
{
char strsql[512];
short dt=0,mt=0,gt=0;
poltekdat(&dt,&mt,&gt);

if(mt <= 2)
 return;

class iceb_tu_str god("");

iceb_t_poldan("Стартовый год",&god,"nastud.alx");

if(god.ravno_atoi() == gt)
  return;

if(god.ravno_atoi() > gt)
 {
  sprintf(strsql,"%s! %d > %d",gettext("Стартовый год больше текущего года"),god.ravno_atoi(),gt);
  iceb_t_soob(strsql);
  return;
 }

sprintf(strsql,"update Alx set str='Стартовый год|%d\n',ktoz=%d,vrem=%ld where fil='nastud.alx' and str like 'Стартовый год|%%'",gt,ICEB_KOD_OP_PROG,time(NULL));
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
/*****************************/
/*Старт программы*/
/****************************/
void xud_start()
{
/*Ищем настройку*/

if(nastsys() != 0)
 exit_iceb(1);

if(iceb_t_perzap(1) != 0) /*Проверка на первичный запуск в текущем дне и удаление *.lst */
 exit_iceb(1);

iceb_t_blokps(); /*автоматическая блокировка подсистемы*/
xud_sg(); /*Установка стартового года*/
}
/****************************************/
/*Удаление за год*/
/******************************************/
void xud_udgod()
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
      xududgod();
      break;

    case 1:
      xudgods();
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
void xud_inst()
{
int kom=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Инструменты"));

MENU.VVOD_SPISOK_add_MD(gettext("Работа с файлами распечаток"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Сменить базу"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Сменить принтер"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Дополнительные программы"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Удаление документов за год"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//5

for(;;)
 {
  clear();

  if(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0)
   continue;
  switch (kom)
   {
    case 5:
    case -1 :
      return;

    case 0:

      dirlst("lst,txt,csv,xml,dbf");
      break;


    case 1:
      if(smenabaz(1) == 0)
       {
        xud_start();
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
     xud_udgod();
     break;
     
    default:
      kom=0;
      break;
   }
 }

}
