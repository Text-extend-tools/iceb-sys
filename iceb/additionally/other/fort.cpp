/*$Id:$*/
/*02.03.2017	26.11.2016	Белых А.И.	fort.c
программ работы с нарядами
*/
#define DVERSION "02.03.2017"


#include <iceb.h>
#include <buhl.h>
#include "fort.h"

void fort_rsn();
void fort_an();
int fort_rz(class spis_oth *oth);
void fort_nsi();
int dirceh(int metkar,class iceb_tu_str *kod_ceh,class iceb_tu_str *naim_ceh);
int dirgortti(const char *kod_ceha);
int dirfortnp(const char *kod_ceha);
void fort_ras();
int fort_rnp(class spis_oth *oth);
void fort_nz();

extern char	*imabaz;
extern char	*host;
extern char	*parol_pol;
SQL_baza bd;

int main(int argc,char **argv,char **envp)
{



icebstart(argc,argv);

if(iceb_t_perzap(1) != 0) /*Проверка на первичный запуск в текущем дне и удаление *.lst */
 exit_iceb(1);


VVOD MENU(4);
int k=0;


/*Ищем настройку*/

if(nastsys() != 0)
  goto kon;



MENU.VVOD_SPISOK_add_ZAG(gettext("Программа работы с нарядами и незавершёнкой"));
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0));

MENU.VVOD_SPISOK_add_MD(gettext("Работа с нарядами"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Незавершённое производство"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка нормативно справочной информации"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка отчётов"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//4

clear();

for (;;)
{
 clear();

 znak(0,COLS-26);
 infosys(12,COLS-27,host,VERSION,DVERSION,0,0,0,imabaz,iceb_t_getuid(),iceb_t_getfioop());

 
 menu3(&MENU,&k,1,0,0);

 class spis_oth oth;   

 switch (k)
  {
   case -1:
   case 4:
     goto kon;
     break;

   case 0:
    fort_rsn();
    break;

   case 1:
   fort_nz();
    break;
  case 2:
    fort_nsi();
    break;
    
   case 3:
    fort_ras();
    break;
  }
}
kon:;

sql_closebaz(&bd);


endwin();

}

/***************************/
/*работа с нарядами*/
/************************/
void fort_rsn()
{

short mes_nar=0;
short god_nar=0;
class iceb_tu_str nn("");
int podr=0;
int kom=0;

class VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Работа с нарядами"));

MENU.VVOD_SPISOK_add_MD(gettext("Ввод нового наряда"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Просмотр архива нарядов"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2

for(;;)
 {
  clear();

  if(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0)
   continue;
  switch (kom)
   {
    case 2:
    case -1 :
      return;

    case 0:
      mes_nar=god_nar=0;
      nn.new_plus("");
      podr=0;
      if(fort_vhn(&mes_nar,&god_nar,&nn,&podr) == 0)
       fort_anz(mes_nar,god_nar,nn.ravno(),podr);
      break;

    case 1:
      fort_an();
      break;
     
    default:
      kom=0;
      break;
   }
 }
}
/***************************/
/*работа с нормативно справочной информацией*/
/************************/
void fort_nsi()
{
int kom=0;
class iceb_tu_str kod("");
class iceb_tu_str naim("");

class VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Нормативно справочная информация"));

MENU.VVOD_SPISOK_add_MD(gettext("Справочник цехов"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Справочник трудоёмкости изготовления"));//1
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
     kod.new_plus("");
     naim.new_plus("");
     dirceh(0,&kod,&naim);
     break ;

   case 1:
    dirgortti("");
    break;
   }
 }
}

/***************************/
/*распечатка отчётов*/
/************************/
void fort_ras()
{

int kom=0;

class VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Распечатка отчётов"));

MENU.VVOD_SPISOK_add_MD(gettext("Распечатка нарядов"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Распечатка незавершённого производства"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2

class spis_oth oth;   
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
     if(fort_rz(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 1:
      if(fort_rnp(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;
     
    default:
      kom=0;
      break;
   }
 }
}
/****************************/
/**/
/*****************************/
void fort_nz()
{
int kom=0;
class iceb_tu_str kod_ceh("");
class iceb_tu_str naim("");

class VVOD VVOD1(2);
class VVOD MENU(3);

MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Незавершённое производство"));

MENU.VVOD_SPISOK_add_MD(gettext("Ввод незавершёнки по цеху"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Ввод незавершёнки по любому цеху"));//1
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
      helstr(LINES-1,0," F10 ",gettext("выход"),NULL);

      VVOD1.VVOD_delete();
      VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код цеха"));
      
      kod_ceh.new_plus("");
      
      if(vvod1(&kod_ceh,20,&VVOD1,NULL,stdscr,-1,-1) == FK10)
       break;

      if(kod_ceh.getdlinna() <= 1)
       {
        if(dirceh(2,&kod_ceh,&naim) != 0)
         break;
       }       
      dirfortnp(kod_ceh.ravno());
      break;

    case 1:
      dirfortnp("");
      break;
     
    default:
      kom=0;
      break;
   }
 }
}
