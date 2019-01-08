/* $Id: xuos.c,v 5.318 2014/07/31 07:09:18 sasa Exp $ */
/*23.05.2018    08.04.1996      Белых А.И.      xuos.c
Пpогpамма учета основных сpедств
*/
#define DVERSIQ "06.12.2018"
#include        <ctype.h>
#include        "buhg.h"
#include        "uos.h"


int             uosnast(void);
void		rabdok(void);
int  uosvamot(class spis_oth*);
int  uosvamotby(class spis_oth*);
int  uosdvos(short,class spis_oth*);
void		uosrp(void);
int  uosvosii(int,class spis_oth*);
void            rasamort(const char*);
void            smenakof(int);
void            poiuosdok(void);
void		rashetamort(void);
void		rasamortby(void);
void		rasamortnu(void);
void		uosnastp(void);
int dvos(class spis_oth*);
int uosinved(class spis_oth*);
void		impuosdok(void);
void            uosrs(void);
int uos_otvm(class spis_oth*);
int uos_fomtr(class spis_oth*);
int uosvos(int,class spis_oth*);
int uosrbs(class spis_oth*);
int uosvibo(class spis_oth*);
int uospin(class spis_oth*);
int uosinu(class spis_oth*);
void uosdoo(void);
int uos_sp(int metka,class spis_oth*);
int uospos(class spis_oth*);
int uospzp(class spis_oth*);
int dirdm(int metkar,class iceb_tu_str *kodm,class iceb_tu_str *naim);
void    uosbd();
void uossin();
int uosoik(class spis_oth *oth);
int uosisos(class spis_oth *oth);
int uosplc(class spis_oth *oth);
void xuos_start();
void xuos_inst();
int eksuos(class spis_oth *oth);

class iceb_tu_str kat_for_nal_nak("");
class UOS      uos;
SQL_baza	bd;
extern char     *printer; /*Печать*/
extern char     *printer1; /*Приэкранная печать*/
extern float    nemi; /*Необлагаемый минимум*/
extern char	*imabaz;
extern char	*host;
extern char	*parol_pol;
extern int iceb_kod_podsystem;

int  main(int argc,char **argv,char **envp)
{
iceb_kod_podsystem=ICEB_PS_UOS;
int             kom;
long            in;
int             podd,kodotl;
class iceb_tu_str inv("");
short		i;
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

icebstart(argc,argv);
xuos_start();

VVOD MENU(4);
VVOD VVOD1(2);



int uid=0;
class iceb_tu_str fioop;

uid=iceb_t_getuid();
fioop.plus(iceb_t_getfioop());

clear();
kom=0;
for(;;)
 {

  clear();

  MENU.VVOD_delete();
  
  MENU.VVOD_SPISOK_add_ZAG(gettext("Программа ведения учета основных средств"));
  MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
  
  MENU.VVOD_SPISOK_add_MD(gettext("Работа с документами"));//0
  MENU.VVOD_SPISOK_add_MD(gettext("Просмотр карточки инвентарного номера"));//1
  MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка основных средств"));//2
  MENU.VVOD_SPISOK_add_MD(gettext("Ввод нормативно-справочной информации"));//3
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатка отчётов"));//4
  MENU.VVOD_SPISOK_add_MD(gettext("Расчёт амортизации"));//5
  MENU.VVOD_SPISOK_add_MD(gettext("Сменить инвентарный номер"));//6
  MENU.VVOD_SPISOK_add_MD(gettext("Инструменты"));//7
  MENU.VVOD_SPISOK_add_MD(gettext("Конец работы"));//8

  znak(0,COLS-26);
  infosys(12,COLS-27,host,VERSION,DVERSIQ,0,0,0,imabaz,uid,fioop.ravno());

  if(menu3(&MENU,&kom,1,0,0) != 0)
    continue;
  class iceb_tu_str naim("");
  switch (kom)
   {
    case 8:
    case -1:
	   goto kon;
	   break;
    case 0 :
      rabdok();
      break;

    case 1 :
      clear();
      move(0,0);
      printw("%s\n",gettext("Поиск инвентарного номера"));
      printw(" %s: %d.%d%s\n",
      gettext("Дата поиска"),
      mt,gt,gettext("г."));

      helstr(LINES-1,0,"F10",gettext("выход"),NULL);

      inv.new_plus("");
      VVOD1.VVOD_delete();
      VVOD1.VVOD_SPISOK_add_MD(gettext("Введите инвентарный номер"));
      if((i=vvod1(&inv,24,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
       break;
      if(i == ESC)
        break;

      GDITE();
      podd=0;

      if(isdigit(inv.ravno()[0]) != 0)  /*Значит символ число*/
       {
        in=inv.ravno_atoi();
        kodotl=podd=0;
        poiin(in,1,mt,gt,&podd,&kodotl);
        VVOD VV(0);
        vos(&VV,in,2,31,mt,gt,podd,kodotl);
       }
      else
       {
        iceb_t_soob(gettext("Инвентарный номер должен быть цыфровым !"));
       }

      break;

    case 2:
      in=0;
      diros(&in,&naim,0);
      break;

    case 3 :
      nsiuos();
      break;

    case 4:
      rasoth();
      break;

    case 5:
      rashetamort();
      break;

    case 6:
      uossin();
      break;

    case 7:
      xuos_inst();
      break;

      
    default:
      kom=0;
      break;
   
   }
 }

kon:;

exit_iceb(0);

}
/**************************/
/**************************/
void xuos_nkd()
{
int          kom;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Настройка концовки документов"));

MENU.VVOD_SPISOK_add_MD(gettext("Ввод текста до подписей в накладной"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Ввод текста после подписей в накладной"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2


kom=0;

for(;;)
 {
  clear();

  while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
  class spis_oth oth;
  switch (kom)
   {
    case 2 :
    case -1 :
      return;
      break;

    case 0 :
      iceb_redfil("uosnakend_dp.alx",0);
      break;
    case 1 :
      iceb_redfil("uosnakend_pp.alx",0);
      break;

   }
 }
}

/***************************************/
/*Ввод нормативно-справочной информации*/
/***************************************/

void    nsiuos()
{
int             kom=0;
int		poz,komv;
class iceb_tu_str kodn("");
class iceb_tu_str naim("");

VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Ввод и корректировка нормативно-справочной информации"));

MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка операций приходов"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка операций расходов"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка подразделений"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка гpупп (ШНАО) основных средств для нал. учета"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка гpупп (ШНАО) основных средств для бух. учета"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка шифров аналитического учета"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка шифров затрат"));//6
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка материально-ответственных"));//7
MENU.VVOD_SPISOK_add_MD(gettext("Работа з файлом настройки"));//8
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка списка контрагентов"));//9
MENU.VVOD_SPISOK_add_MD(gettext("Смена поправочных коэффициентов в карточках для налогового учёта"));//10
MENU.VVOD_SPISOK_add_MD(gettext("Смена поправочных коэффициентов в карточках для бухгалтерского учёта"));//11
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка списка форм оплаты"));//12
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка догогоцінних металів"));//13
MENU.VVOD_SPISOK_add_MD(gettext("Настpойка автоматического выполнения пpоводок"));//14
MENU.VVOD_SPISOK_add_MD(gettext("Настpойка концовки документов"));//15
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//16

kom=0;

for(;;)
 {
  clear();

  while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
  
  class iceb_tu_str kods("");
  class iceb_tu_str naims("");  
  
  switch (kom)
   {
    case 16:
    case -1:
      return;

    case 0 :
      diruospri(0,&kods,&naims);
      break;

    case 1 :
      diruosras(0,&kods,&naims);

      break;

    case 2 :
      diruospod(0,&kods,&naims,0);
      break;

    case 3:
      diruosgrnu(0,&kods,&naims);
      break;

    case 4:
      diruosgrbu(0,&kods,&naims);
      break;

    case 5 :
      clear();
      kodn.new_plus("");
      naim.new_plus("");
      poz=komv=0;
      dirmat1("Uoshau",&kodn,&naim,LINES-5,0,2,0,&poz,&komv,"","\0");
      break;

    case 6 :
      clear();
      kodn.new_plus("");
      naim.new_plus("");
      poz=komv=0;
      dirmat1("Uoshz",&kodn,&naim,LINES-5,0,2,0,&poz,&komv,"","\0");
      break;

    case 7 :
      dirmatot(0,&kods,&naims,0);
      break;

    case 8 :
      if(iceb_redfil("uosnast.alx",0) == 0)
        uosnast();
      break;

    case 9 :
     clear();


     kodn.new_plus("");
     naim.new_plus("");

     dirkontr(&kodn,&naim,0);

     break;

    case 10 :
     smenakof(0);
     break;

    case 11 :
     smenakof(1);
     break;

    case 12 :
      clear();
      kodn.new_plus("");
      naim.new_plus("");
      poz=komv=0;
      dirmat1("Foroplat",&kodn,&naim,LINES-5,0,2,0,&poz,&komv,"","\0");

      break;

    case 13 :
      dirdm(0,&kods,&naims);
      break;

    case 14:
      menuvnp(3,0);
      break;

    case 15:
      xuos_nkd();
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
void            rabdok()
{
int          kom;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Работа с документами"));

MENU.VVOD_SPISOK_add_MD(gettext("Ввод нового документа"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Поиск введенного документа"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Просмотр всех введенных документов"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Установка/снятие блокировки"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Импорт документов"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Пропорциональное распределение сумм"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Дооценка основных средств с нулевой остаточной стоимостью (бух.учёт)"));//6
MENU.VVOD_SPISOK_add_MD(gettext("Перенос основных средств из материального учёта"));//7
MENU.VVOD_SPISOK_add_MD(gettext("Дооценка основных средств для бюджетных организаций"));//8
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//9


kom=0;

for(;;)
 {
  clear();

  while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

  class spis_oth oth;
  switch (kom)
   {
    case 9:
    case -1 :
      return;
      
    case 0 :
      uosndok();
      break;

    case 1 :
      poiuosdok();
      break;


    case 2 :
      diruosdok();
      break;

    case 3 :
      iceb_tl_blok();
      break;

    case 4 :
      impuosdok();
      break;

    case 5 :
      uosrs();
      break;

    case 6 :
      uosdoo();
      break;

    case 7 :
      if(uospin(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 8 :
      uosbd();
      break;


    default:
      kom=0;
      break;
   }
 }
}
/**************************/
/**************************/
void xuos_rpssp()
{
int          kom;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Расчёт движения основных средств по счетам списания/получения"));

MENU.VVOD_SPISOK_add_MD(gettext("Расчёт движения основных средств по счетам списания"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт движения основных средств по счетам получения"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2


kom=0;

for(;;)
 {
  clear();

  while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
  class spis_oth oth;
  switch (kom)
   {
    case 2 :
    case -1 :
      return;
      break;

    case 0 :
      if(uos_sp(2,&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;
    case 1 :
      if(uos_sp(1,&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

   }
 }
}


/***************************/
/*движение основных средств*/
/***************************/
void xuos_dos()
{
int           kom;
VVOD MENU(3);

MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Распечатка отчётов"));

MENU.VVOD_SPISOK_add_MD(gettext("Распечатать ведомость движения основных средств по подразделениям"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать ведомость движения основных средств по мат.ответственным"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//18

kom=0;
for(;;)
 {
  clear();

  while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
  class spis_oth oth;
  switch (kom)
   {
    case 2 :
    case -1 :
      return;

    case 0 :
      if(uosdvos(0,&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 1 :
      if(uosdvos(1,&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;
    default:
      kom=0;
      break;
   }
 }
}
/***************************/
/*амортотчисления*/
/***************************/
void xuos_ao()
{
int           kom;
VVOD MENU(3);

MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Распечатка отчётов"));

MENU.VVOD_SPISOK_add_MD(gettext("Распечатать ведомость амортизационных отчислений для налогового учёта"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать ведомость амортизационных отчислений для бухгалтерского учёта"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//18

kom=0;
for(;;)
 {
  clear();

  while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
  class spis_oth oth;
  switch (kom)
   {
    case 2 :
    case -1 :
      return;

    case 0 :
      if(uosvamot(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 1 :
      if(uosvamotby(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;
    default:
      kom=0;
      break;
   }
 }
}
/***************************/
/*ведомость износа и остаточной стоимости*/
/***************************/
void xuos_vios()
{
int           kom;
VVOD MENU(3);

MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Распечатка отчётов"));

MENU.VVOD_SPISOK_add_MD(gettext("Распечатать ведомость износа и остаточной стоимости (налоговый учет)"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать ведомость износа и остаточной стоимости (бухгалтерский учет)"));//6
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//18

kom=0;
for(;;)
 {
  clear();

  while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
  class spis_oth oth;
  switch (kom)
   {
    case 2 :
    case -1 :
      return;

    case 0 :
      if(uosvosii(0,&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 1 :
      if(uosvosii(1,&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    default:
      kom=0;
      break;
   }
 }
}
/***************************/
/*ведомость остаточной стоимости*/
/***************************/
void xuos_os()
{
int           kom;
VVOD MENU(3);

MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Распечатка отчётов"));

MENU.VVOD_SPISOK_add_MD(gettext("Распечатать ведомость остаточной стоимости (налоговый учет)"));//12
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать ведомость остаточной стоимости (бухгалтерский учет)"));//13
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//18

kom=0;
for(;;)
 {
  clear();

  while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
  class spis_oth oth;
  switch (kom)
   {
    case 2 :
    case -1 :
      return;

    case 0 :
      if(uosvos(0,&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 1 :
      if(uosvos(1,&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    default:
      kom=0;
      break;
   }
 }
}
/***************************/
/*распечатать перечень основных средств*/
/***************************/
void xuos_poz()
{
int           kom;
VVOD MENU(3);

MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Распечатка отчётов"));

MENU.VVOD_SPISOK_add_MD(gettext("Распечатать инвентаризационную опись"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать перечень основных средств"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать типовую форму NoОЗ-7"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать типовую форму NoОЗ-9"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать перечень полученных основных средств"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//5

kom=0;
for(;;)
 {
  clear();

  while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
  class spis_oth oth;
  switch (kom)
   {
    case 5 :
    case -1 :
      return;

    case 0 :
      if(uosinved(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 1:
      if(uospos(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 2:
      if(uosoik(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 3:
      if(uosisos(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 4:
      if(uospzp(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    default:
      kom=0;
      break;
   }
 }
}
/*********************/
/*Распечатка отчётов*/
/*********************/
void    rasoth()
{
int           kom;
VVOD MENU(3);

MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Распечатка отчётов"));

MENU.VVOD_SPISOK_add_MD(gettext("Распечатать ведомость движения основных средств"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать ведомость амортизационных отчислений"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать реестр проводок по операциям"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать ведомость износа и остаточной стоимости"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать движение по документам"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать список годов за которые введена информация"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать перечень основных средсв"));//6
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать отчёт для министерства"));//7
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт формы учёта материально-технических средств"));//8
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать ведомость остаточной стоимости"));//9
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт балансовой стоимости по мат.ответственным и счетам учёта"));//10
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт движения основных средств по счетам списания/получения"));//11
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт ведомости начисления износа для бюджетных организаций"));//12
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт разницы балансовой стоимости и износа"));//13
//MENU.VVOD_SPISOK_add_MD(gettext("Распечатать перечень основных средсв"));//14
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//14

kom=0;
for(;;)
 {
  clear();

  while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
  class spis_oth oth;
  switch (kom)
   {
    case 14 :
    case -1 :
      return;

    case 0 :
      xuos_dos();
      break;

    case 1 :
      xuos_ao();
      break;

    case 2 :
      uosrp();
      break;

    case 3 :
      xuos_vios();
      break;

    case 4 :
      if(dvos(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 5 :
      poigod(4);
      break;

    case 6 :
      xuos_poz();
      break;

    case 7 :
      if(uos_otvm(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 8 :
      if(uos_fomtr(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 9 :
      xuos_os();
      break;

    case 10 :
      if(uosrbs(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 11 :
      xuos_rpssp();
      break;

    case 12 :
      if(uosvibo(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 13 :
      if(uosinu(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;



    default:
      kom=0;
      break;
   }
 }
}

/*******************/
/*Расчёт амортизации*/
/*********************/

void	rashetamort()
{
int             kom;
VVOD MENU(3);

kom=0;

MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);

MENU.VVOD_SPISOK_add_MD(gettext("Расчёт амортизации для налогового учёта"));
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт амортизации для бухгалтерского учёта"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

switch (kom)
 {
  case 2 :
  case -1 :
    return;

  case 0 :
    rasamortnu();
     break;

  case 1 :
     rasamortby();
     break;

  default:
    kom=0;
    break;
 }
}
/*****************************/
/*Старт программы*/
/**************************/
void xuos_start()
{
/*Чтение настpойки*/
if(nastsys() != 0)
 exit_iceb(1);
if(uosnast() != 0)
 exit_iceb(1);

if(iceb_t_perzap(1) != 0) /*Проверка на первичный запуск в текущем дне и удаление *.lst */
 exit_iceb(1);

iceb_t_blokps(); /*автоматическая блокировка подсистемы*/
}
/****************************************/
/*Инструменты*/
/******************************************/
void xuos_inst()
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
MENU.VVOD_SPISOK_add_MD(gettext("Экспорт основных средств"));//5
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
        xuos_start();
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
      if(uosplc(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 5:     
      if(eksuos(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;      

    default:
      kom=0;
      break;
   }
 }

}
