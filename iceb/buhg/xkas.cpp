/* $Id: xkas.c,v 5.167 2014/07/31 07:09:18 sasa Exp $ */
/*23.05.2018	19.09.2000	Белых А.И.	xkas.c
Программа учета кассовых ордеров на приход и расход
*/
#define DVERSIQ "15.06.2018"
#include        "buhg.h"

void		rabkord(void);
void		rabkord1(void);
int kasnast(void);
int  kasoth(int,class spis_oth*);
int  xkasplc(class spis_oth*);
void		udkasordg(void);
void	        raspoth(void);
void dirnomved(void);
void dirkcnsl(void);
int  kasothcn(class spis_oth*);
void imp_kasord(void);
void saldokaskcn();
void xkas_start();
void xkas_inst();
void kasgods();

short   	mfnn=1; /*Метка формирования номера накладной
                      -1 - Автоматическая нумерация выключена
                       0 - Приходы и расходы - нумерация общая
                       1 - Приходы отдельно расходы отдельно
                      */
extern int	iscolor;
extern char	*imabaz;
extern char	*host;
extern char	*parol_pol;
SQL_baza	bd;
int		regim; /*Номер кассы, 0-доступ к данным всех касс*/
extern int iceb_kod_podsystem;

int main(int argc,char **argv,char **envp)
{
int kom=0;
iceb_kod_podsystem=ICEB_PS_UKO;

icebstart(argc,argv);
xkas_start();

VVOD MENU(4);



MENU.VVOD_SPISOK_add_MD(gettext("Работа с ордерами"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Ввод нормативно-справочной информации"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать отчёты"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Работа с журналом платёжных ведомостей"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Инструменты"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Конец работы"));//5
int uid=0;
class iceb_tu_str fioop;

uid=iceb_t_getuid();
fioop.plus(iceb_t_getfioop());

for(;;)
 {

  clear(); /*Очистить экран и закрасить фоновым цветом*/
  MENU.VVOD_delete_ZAG();
  MENU.VVOD_SPISOK_add_ZAG(gettext("Учет кассовых ордеров"));
  MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);

  znak(0,COLS-26);
  infosys(12,COLS-27,host,VERSION,DVERSIQ,0,0,0,imabaz,uid,fioop.ravno());

  if(menu3(&MENU,&kom,1,0,0) != 0)
   continue;

  switch (kom)
   {
    case 5:
    case -1 :
      goto kon;
      break;

    case 0 :
      rabkord1();
      break;

    case 1 :
      rabkord();
      break;

    case 2:
      raspoth();
      break;     


    case 3:
      dirnomved();
      break;
      
    case 4:
      xkas_inst();
      break;



    default:
      kom=0;
      break;
   }
 }

kon:;

exit_iceb(0);

}
/*******************/
/*Работа с НСИ*/
/*******************/
void		rabkord()
{
int		kom=0,kom1=0;
class iceb_tu_str kods("");
class iceb_tu_str naims("");

VVOD MENU(3);
VVOD MENU2(3);

kom=0;
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Ввод и корректировка нормативно-справочной информации"));

MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка списка касс")); //0
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка целевых назначений"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка списка операций приходов"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка списка операций расходов"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка сальдо по кодам целевого назначения"));//4
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
    case -1 :
      return;
      break;
    case 0:
      dirkas(0,&kods,&naims);
      break;

    case 1:
      kods.new_plus("");
      naims.new_plus("");
      dirkcn(0,&kods,&naims);
      break;
      
    case 2:
      dirkasop(1,0,&kods,&naims);
      break;

    case 3:
      dirkasop(2,0,&kods,&naims);
      break;
    
    case 4:
      dirkcnsl();
      break;

    case 5:
      iceb_redfil("kasnast.alx",0);
      break;

    case 6 :
      MENU2.VVOD_delete();
      MENU2.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное и нажмите Enter"));

      MENU2.VVOD_SPISOK_add_MD(gettext("Настpойка автоматического выполнения пpоводок для приходных кассовых оредров"));
      MENU2.VVOD_SPISOK_add_MD(gettext("Настpойка автоматического выполнения пpоводок для расходных кассовых ордеров"));
      MENU2.VVOD_SPISOK_add_MD(gettext("Выход"));

      kom1=0;
      while(menu3w(stdscr,&MENU2,&kom1,-1,-1,0) != 0);
      switch(kom1)
       {
        case 2:
        case -1:
          break;
        case 0:
          iceb_redfil("avtprokasp.alx",0);
          break;
        case 1:
          iceb_redfil("avtprokasr.alx",0);
          break;
       }
      break;
      

    default:
      kom=0;
      break;
   }
 }

}
/*******************/
/*Работа с ордерами*/
/*******************/
void		rabkord1()
{
int		kom;
short		dd,md,gd;
class iceb_tu_str nomd("");
class iceb_tu_str kassa("");

char bros[512];
poltekdat(&dd,&md,&gd);
VVOD MENU(3);
VVOD VV(0);

kom=0;
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);

MENU.VVOD_SPISOK_add_ZAG(gettext("Работа с документами"));

MENU.VVOD_SPISOK_add_MD(gettext("Ввод нового ордера на расход"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Ввод нового ордера на приход"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Поиск кассового ордера"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка ордеров"));//3

sprintf(bros,"%s %d.%d.%d%s",gettext("Просмотр кассовых ордеров за"),dd,md,gd,gettext("г."));//4
MENU.VVOD_SPISOK_add_MD(bros);

MENU.VVOD_SPISOK_add_MD(gettext("Установить/снять блокировку дат"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Импорт кассовых ордеров"));//6
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//7


for(;;)
 {
  clear();

  while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
  class spis_oth oth;

  switch (kom)
   {
    case 7:
    case -1 :
      return;
      break;
 
    case 0:
      if(vkasord(2,0,&kassa,&nomd,&dd,&md,&gd) == 0)
        dirkasord(kassa.ravno(),nomd.ravno(),dd,md,gd,2);

     break;   
 
      
    case 1:
      if(vkasord(1,0,&kassa,&nomd,&dd,&md,&gd) == 0)
        dirkasord(kassa.ravno(),nomd.ravno(),dd,md,gd,1);
     break;   

    case 2:
      poikasdok();
      break;

    case 3:
      dirkasords(0,0,0);
      break;   

    case 4:
      dirkasords(dd,md,gd);
      break;   

    case 5 :
      iceb_tl_blok();
      break;

    case 6 :
      imp_kasord();
      break;


    default:
     kom=0;
     break;
   }
 }

}

/*************************/
/*Распечатка отчётов     */
/*************************/
void	raspoth()
{
int kom=0;

VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Распечатка отчётов"));

MENU.VVOD_SPISOK_add_MD(gettext("Распечатка списка документов"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка свода по видам операций"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка целевой кассовой книги"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//3


for(;;)
 {
  clear();
  while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
  class spis_oth oth;
  switch (kom)
   {
    case 3 :
    case -1 :
      return;
      break;
    case 0:
      if(kasoth(0,&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;
    case 1:
      if(kasoth(1,&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;
    case 2:
      if(kasothcn(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;
   }
 }
}
/***************************/
/*старт программы*/
/*************************/
void xkas_start()
{
/*Ищем настройку*/
if(nastsys() != 0)
  exit_iceb(1);

if(kasnast() != 0)
  exit_iceb(1);

if(iceb_t_perzap(1) != 0) /*Проверка на первичный запуск в текущем дне и удаление *.lst */
 exit_iceb(1);

iceb_t_blokps(); /*автоматическая блокировка подсистемы*/
}



/****************************************/
/*Удаление за год*/
/******************************************/
void xkas_udgod()
{
int kom=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Инструменты"));

MENU.VVOD_SPISOK_add_MD(gettext("Удаление кассовых ордеров за год"));//0
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
      udkasordg();
      break;

    case 1:
      kasgods();
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
void xkas_inst()
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
MENU.VVOD_SPISOK_add_MD(gettext("Перенос сальдо"));//5
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
        xkas_start();
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
      if(xkasplc(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 5:
      saldokaskcn();
      break;

    case 6:
      xkas_udgod();
      break;

    default:
      kom=0;
      break;
   }
 }
}