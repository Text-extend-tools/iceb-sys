/* $Id: xdk.c,v 5.200 2014/06/30 06:36:35 sasa Exp $ */
/*23.05.2018    25.10.1994      Белых А.И.      xdk.c
Программа распечатки различных документов
*/ 
#define DVERSIQ "23.05.2018"
#include        "buhg.h"
#include        "dok4.h"

int              nastdk(void);
void             platdok(const char[]);
void             peshetn(void);
void             dirobq(void);
void             udpdok(void);
void dok_dover();
int xdkplc(class spis_oth*);
void xdk_sg();
void xdk_start();
void xdk_inst();
void xdkpg();

class REC       rec;
extern short    kopk;   /*0-копеек нет 1-есть*/
extern char     *printer; /*Печать*/
extern double   okrg1; /*Округление*/
extern char	*imabaz;
extern char	*host;
extern char	*parol_pol;

SQL_baza	bd;
char            *imafkomtmp=NULL; //Имя файла куда выгружается комментарий для редактирования
short ddd,mmm,ggg; //Нужно для подпрограммы просмотра списка табельных номеров
extern int iceb_kod_podsystem;

int main(int argc,char **argv,char **envp)
{
char bros[512];
int kom=0;
iceb_kod_podsystem=ICEB_PS_PD;
int uid=0;
class iceb_tu_str fioop;

icebstart(argc,argv);

clear();
xdk_start();


VVOD MENU(4);
VVOD VVOD1(2);

sprintf(bros,"pl%d.tmp",getpid());
imafkomtmp=new char[strlen(bros)+1];
strcpy(imafkomtmp,bros);

poltekdat(&ddd,&mmm,&ggg);
uid=iceb_t_getuid();
fioop.plus(iceb_t_getfioop());
for(;;)
 {

  clear();

  MENU.VVOD_delete();

  MENU.VVOD_SPISOK_add_ZAG(gettext("Работа с бухгалтерскими документами"));
  MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);

  MENU.VVOD_SPISOK_add_MD(gettext("Работа с платежными поручениями"));//0 
  MENU.VVOD_SPISOK_add_MD(gettext("Работа с платежными требованиями"));//1
  MENU.VVOD_SPISOK_add_MD(gettext("Печать счетов"));//2
  MENU.VVOD_SPISOK_add_MD(gettext("Работа с документами для сдачи налички в банк"));//3
  MENU.VVOD_SPISOK_add_MD(gettext("Работа с файлом настройки"));//4
  MENU.VVOD_SPISOK_add_MD(gettext("Выписка доверенности"));//5
  MENU.VVOD_SPISOK_add_MD(gettext("Инструменты"));//6
  MENU.VVOD_SPISOK_add_MD(gettext("Конец работы"));//7

  znak(0,COLS-23);
  infosys(12,COLS-27,host,VERSION,DVERSIQ,0,0,0,imabaz,uid,fioop.ravno());

  if(menu3(&MENU,&kom,1,0,0) != 0)
   continue;

  class spis_oth oth;   
  switch (kom)
   {
    case 7:
    case -1 :
      goto kon;
      break;

    case 0 :
      platdok("Pltp");
      break;
    case 1 :
      platdok("Pltt");
      break;


    case 2 :
      peshetn();
      break;

    case 3 :
      dirobq();
      break;


    case 4 :

      if(iceb_redfil("nastdok.alx",0) == 0)
        nastdk();
      break;


    case 5:
       dok_dover();
       break;

    case 6:
       xdk_inst();       
       break;

    default:
      kom=0;
      break;
   }
 }
kon:;
unlink(imafkomtmp);

exit_iceb(0);

}
/****************************/
/*Изменение стартового года*/
/*****************************/
void xdk_sg()
{
char strsql[512];
short dt=0,mt=0,gt=0;
poltekdat(&dt,&mt,&gt);

if(mt <= 2)
 return;

class iceb_tu_str god("");

iceb_t_poldan("Стартовый год",&god,"nastdok.alx");

if(god.ravno_atoi() == gt)
  return;

if(god.ravno_atoi() > gt)
 {
  sprintf(strsql,"%s! %d > %d",gettext("Стартовый год больше текущего года"),god.ravno_atoi(),gt);
  iceb_t_soob(strsql);
  return;
 }
extern short startgodd; /*Стартовый год платежных документов*/
startgodd=gt;

sprintf(strsql,"update Alx set str='Стартовый год|%d\n',ktoz=%d,vrem=%ld where fil='nastdok.alx' and str like 'Стартовый год|%%'",gt,ICEB_KOD_OP_PROG,time(NULL));
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
/*******************************/
/*старт программы */
/******************************/
void xdk_start()
{
if(nastsys() != 0)
 exit_iceb(1);
if(nastdk() != 0)
 exit_iceb(1);

if(iceb_t_perzap(1) != 0) /*Проверка на первичный запуск в текущем дне и удаление *.lst */
 exit_iceb(1);

iceb_t_blokps(); /*автоматическая блокировка подсистемы*/
xdk_sg(); /*установка стартового года*/

}
/****************************************/
/*Удаление за год*/
/******************************************/
void xdk_udgod()
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
      udpdok();
      break;

    case 1:
      xdkpg();
      break;

    default:
      kom=0;
      break;
   }
 }
}
/************************************/
/*инструменты*/
/*********************/
void xdk_inst()
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
MENU.VVOD_SPISOK_add_MD(gettext("Удаление данных за год"));//5
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
        xdk_start();
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
       if(xdkplc(&oth) == 0)
         iceb_t_rabfil(&oth,"");
       
       break;

    case 5:
     xdk_udgod();
     break;

    default:
      kom=0;
      break;
   }
 }
}
