/* $Id: xmu.c,v 5.436 2014/07/31 07:09:18 sasa Exp $ */
/*23.05.2018	25.01.1999	Белых А.И.	xmu.c
Программа материального учета под SQL
*/
#define DVERSIQ "06.12.2018"
#include	"buhg.h"

void            smdok(short,short,short);
void		optimbaz(void);
void            matuddok(void);
void            rasmat(void);
void            rasgrup(short);
int dvtmcf1(class spis_oth*);
int dvtmcf2(class spis_oth*);
int rasspdok(class spis_oth*);
void            rppvo(void);
int rasnds(class spis_oth*);
int prognoz(class spis_oth*);
void            poimatdok(void);
void		srvcen(void);
void		doocsum(void);
void		impmatdok(void);
void		matulk(void);
void		rasosdk(void);
int kriost(class spis_oth*);
int		muinv(class spis_oth*);
int opss(class spis_oth*);
void dok_dover(void);
int spis_p_kontr(class spis_oth*);
int ost_skl_shet(class spis_oth*);
int dvusl_mu(class spis_oth*);
int xmurspd(class spis_oth*);
int mudmc(class spis_oth*);
int mumo(class spis_oth*);
int murfn(class spis_oth*);
void saldomu();
int dirnalog(int nomnal);
int muodk(class spis_oth *oth);
void xmu_saldo();
void xmu_start();
void xmu_inst();
int eksmu(class spis_oth *oth);
int muprih(class spis_oth *oth);
int musspi(class spis_oth *oth);
int dirsnrm();

class iceb_tu_str kat_for_nal_nak("");
extern char     *imabaz; /*Имя базы данных*/
extern short	startgod; /*Стартовый год просмотров*/
extern char	*host;
extern char	*parol_pol;
SQL_baza	bd;
extern int iceb_kod_podsystem;
extern class SPISOK sp_fio_klad; /*выбранные фамилии кладовщиков по складам rasdok.c*/
extern class masiv_din_int sp_kod_sklad; /*список кодов складов для фамилий кладовщиков rasdok.c*/

int             main(int argc,char **argv,char **envp)
{
int		kom;
class iceb_tu_str bros("");
int		skl,nk;
iceb_kod_podsystem=ICEB_PS_MU;
int uid=0;
class iceb_tu_str fioop;

icebstart(argc,argv);


//read_ini();

class VVOD MENU(4);
class VVOD PAR(2);

xmu_start();/*начало работы*/

uid=iceb_t_getuid();
fioop.plus(iceb_t_getfioop());

kom=0;
for(;;)
 {

  clear();

  MENU.VVOD_delete();

  MENU.VVOD_SPISOK_add_ZAG(gettext("Программа товарно-материального учета"));
  if(iceb_t_get_pnk("00",0) != NULL)
   {
    MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
   } 

  MENU.VVOD_SPISOK_add_MD(gettext("Работа с товарно-материальными документами"));//0
  MENU.VVOD_SPISOK_add_MD(gettext("Ввод нормативно-справочной информации"));//1
  MENU.VVOD_SPISOK_add_MD(gettext("Получение отчётов"));//2
  MENU.VVOD_SPISOK_add_MD(gettext("Просмотр карточки материалла"));//3
  MENU.VVOD_SPISOK_add_MD(gettext("Инструменты"));//4
  MENU.VVOD_SPISOK_add_MD(gettext("Конец работы"));//5 

  znak(0,COLS-26);
  infosys(12,COLS-27,host,VERSION,DVERSIQ,0,0,0,imabaz,uid,fioop.ravno());

  if(menu3(&MENU,&kom,1,0,0) != 0)
    continue;

  switch (kom)
   {
    case 5 :
    case -1 :
     exit_iceb(0);

    case 0 :
     rabdokm();
     break;

    case 1 :
     nsimatu();
     break;
    case 2 :
     rasoth();
     break;

    case 3:
      clear();
      mvprintw(0,0,gettext("Просмотр карточки материалла"));
      helstr(LINES-1,0," F10 ",gettext("выход"),NULL);

      bros.new_plus("");
      PAR.VVOD_delete();
      PAR.VVOD_SPISOK_add_MD(gettext("Введите номер склада"));

      if(vvod1(&bros,16,&PAR,NULL,stdscr,-1,-1) == FK10)
        break;
      skl=bros.ravno_atoi();
      if(skl == 0)
        break;


      PAR.VVOD_delete();
      PAR.VVOD_SPISOK_add_MD(gettext("Введите номер карточки"));

      bros.new_plus("");
      if(vvod1(&bros,16,&PAR,NULL,stdscr,-1,-1) == FK10)
        break;

      nk=bros.ravno_atoi();
      if(nk == 0)
        break;
        
      dirzkart(skl,nk);
      
      break;


    case 4:
     xmu_inst();
     break;
     
    default:
      kom=0;
      break;    
   }
 }

exit_iceb(0);
}
/*******************************************/
/*Ввод икорректировка концовок документов*/
/*******************************************/
void xmu_vkd()
{
int             kom;
VVOD MENU(3);

MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Ввод концовок документов"));

MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка концовки счёта"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка концовки накладной (до подписи)"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка концовки накладной (после подписи)"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//3


kom=0;

for(;;)
 {
  clear();

  while(menu3(&MENU,&kom,1,-1,-1) != 0);


  switch (kom)
   {
    case 3:
    case -1 :
      return;
      break;

    case 0 :
      iceb_redfil("matshetend.alx",0);
      break;

    case 1 :
      iceb_redfil("matnakend_dp.alx",0);
      break;

    case 2 :
      iceb_redfil("matnakend.alx",0);
      break;


    default:
      kom=0;
      break;
   }
 }
}

/***************************************/
/*Ввод нормативно-справочной информации*/
/***************************************/

void    nsimatu()
{
int             kom=0;
int             kgr,kodm;
char		bros[512];
int		poz,komv;
class iceb_tu_str kods;
class iceb_tu_str naim;

int kodiz=0,kodizv=0;
int urov=0;

VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Ввод и корректировка нормативно-справочной информации"));

MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка списка групп товаров и материалов"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка списка единиц измерения"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка списка товаров и материалов"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка списка операций приходов"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка списка операций расходов"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка списка складов"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка списка форм оплаты"));//6
MENU.VVOD_SPISOK_add_MD(gettext("Работа с файлом настройки"));//7
MENU.VVOD_SPISOK_add_MD(gettext("Работа со списком контрагентов"));//8
MENU.VVOD_SPISOK_add_MD(gettext("Работа с планом счетов"));//9
MENU.VVOD_SPISOK_add_MD(gettext("Работа со списком коректировок на сумму документа"));//10
MENU.VVOD_SPISOK_add_MD(gettext("Настpойка автоматического выполнения пpоводок"));//11
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка спецификаций на изделия"));//12
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка концовок документов"));//13
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка НДС"));//14
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка норм списания"));//15
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//16

kom=0;
for(;;)
 {

  clear();

  while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
  kods.new_plus("");
  naim.new_plus("");
  switch (kom)
   {
    case 16 :
    case -1 :
      return;
    case 0 :
      clear();
      poz=komv=0;
      dirmat1("Grup",&kods,&naim,LINES-5,0,2,0,&poz,&komv,"","\0");
      break;
    case 1 :
      iceb_tl_ei(0,&kods,&naim); 
      break;

    case 2 :
      clear();
      naim.new_plus("");
      bros[0]='\0';
      dirmatr(&kgr,&kodm,&naim,0,0,bros,0,0);
 
      break;

    case 3 :
      dirprihod(0,&kods,&naim); 
      break;

    case 4 :
      dirrashod(0,&kods,&naim); 
      break;

    case 5 :
      dirsklad(0,&kods,&naim);
      
      break;

    case 6 :
      clear();
      poz=komv=0;
      dirmat1("Foroplat",&kods,&naim,LINES-5,0,2,0,&poz,&komv,"","\0");

      break;

    case 7 :
      if(iceb_redfil("matnast.alx",0) == 0)
        matnast();
      break;

    case 8 :
      clear();
      kods.new_plus("");
      naim.new_plus("");

      dirkontr(&kods,&naim,0);

      break;

    case 9 :
      clear();
      kods.new_plus("");
      naim.new_plus("");

      dirshet(&kods,&naim,0);

      break;

    case 10 :
      iceb_redfil("makkor.alx",0);
      break;

    case 11 :
      menuvnp(1,0);
      break;

    case 12 :
      kodiz=0;kodizv=0;
      urov=0;
      
      dirspec(kodiz,&kodizv,"","",&urov,0);
      break;

    case 13 :
      xmu_vkd();
      break;



    case 14 :
      dirnalog(0);
      break;

    case 15 :
      dirsnrm();
      break;

    default:
      kom=0;
      break;
   }
 }
}

/********************/
/*Распечатка отчётов*/
/********************/   

void		rasoth()
{
int		kom=0,kom1=0;
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
VVOD MENU(3);

for(;;)
 {
  MENU.VVOD_delete();

  MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);

  MENU.VVOD_SPISOK_add_MD(gettext("Распечатать остатки на карточках"));//0
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатать движение товарно-материальных ценностей"));//1
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатать заготовку для отчёта"));//2
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатать список документов"));//3
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатать список документов по датам подтверждения"));//4
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатать реестр проводок по видам операций"));//5
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатать распределение НДС по видам операций"));//6
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатать прогноз реализации товаров"));//7
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатать свод розничной реализации"));//8
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатать суммы выполненых автоматических дооценок"));//9
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатать материалы имеющие критический остаток "));//10
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатать движение услуг"));//11
  MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//12

  while(menu3w(stdscr,&MENU,&kom,-1,-1,1) != 0);
  class spis_oth oth;
  switch (kom)
   {
    case 12 :
    case -1 :
      return;


    case 0 :
      MENU.VVOD_delete();
      MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное и нажмите Enter"));

      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать остатки на всех карточках"));//0
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать остатки на карточках по конкретному материалу"));//1
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать остатки на карточках по конкретной группе материалла"));//2
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать инвентаризационные ведомости"));//3
      MENU.VVOD_SPISOK_add_MD(gettext("Расчёт остатков по складам и счетам учёта"));//4
      MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//5
      kom1=0;
      while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

      switch (kom1)
       {
        case 5 :
        case -1 :
          break;
        case 0 :
          if(rasoskr(&oth) == 0)
           iceb_t_rabfil(&oth,"");
          break;
        case 1 :
          rasmat();
          break;
        case 2 :
          rasgrup(0);
          break;
        case 3 :
          if(muinv(&oth) == 0)
           iceb_t_rabfil(&oth,"");
          
          break;
        case 4 :
          if(ost_skl_shet(&oth) == 0)
           iceb_t_rabfil(&oth,"");
           
          break;
       }
      break;

    case 1 :
      MENU.VVOD_delete();
      MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0));
   
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать движение по конкретному материалу"));//0
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать движение товарно-материальных ценностей (форма 1)"));//1
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать движение товарно-материальных ценностей (форма 2)"));//2
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать движение товарно-материальных ценностей по счетам списания"));//3
      MENU.VVOD_SPISOK_add_MD(gettext("Расчёт списания/получения материаллов по контрагентам"));//4
      MENU.VVOD_SPISOK_add_MD(gettext("Расчёт движения по кодам материалов"));//5
      MENU.VVOD_SPISOK_add_MD(gettext("Расчёт материального отчёта"));//6
      MENU.VVOD_SPISOK_add_MD(gettext("Расчёт процента наценки"));//7
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать список материалов с последней датой получения"));//8
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать свод списания материалов по деталям"));//9
      MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//10
      kom1=0;
  naz:;

      clear();
       
      while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
      oth.clear();
      switch (kom1)
       {

        case 10 :
        case -1 :
          break;

        case 0 :
          rasosdk();
          goto naz;

        case 1 :
          if(dvtmcf1(&oth) == 0)
           iceb_t_rabfil(&oth,"");
          goto naz;

        case 2 :
          if(dvtmcf2(&oth) == 0)
           iceb_t_rabfil(&oth,"");
          goto naz;

        case 3 :
          if(opss(&oth) == 0)
           iceb_t_rabfil(&oth,"");
          goto naz;

        case 4 :
          if(spis_p_kontr(&oth) == 0)
           iceb_t_rabfil(&oth,"");
          goto naz;
        case 5:
          if(mudmc(&oth) == 0)
           iceb_t_rabfil(&oth,"");
          goto naz;
        case 6:
          if(mumo(&oth) == 0)
           iceb_t_rabfil(&oth,"");
          goto naz;
        case 7:
          if(murfn(&oth) == 0)
           iceb_t_rabfil(&oth,"");
          goto naz;
        case 8:
          if(muprih(&oth) == 0)
           iceb_t_rabfil(&oth,"");
          goto naz;
        case 9:
          if(musspi(&oth) == 0)
           iceb_t_rabfil(&oth,"");
          goto naz;
       }
      break;

    case 2 :
      if(muodk(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 3 :
      if(rasspdok(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 4 :
      if(xmurspd(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 5 :
      rppvo();
      break;

    case 6 :
      if(rasnds(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 7 :
      if(prognoz(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 8 :
      rozsvod(dt,mt,gt,0,"\0");
      break;

    case 9 :
      doocsum();
      break;

    case 10 :
      if(kriost(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 11 :
      if(dvusl_mu(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    default:
      break;
   }
 }
}
/********************/
/*Работа с документами*/
/********************/
void            rabdokm()
{
int             kom;
short		godn;
char		bros[512];
FILE            *ffs=NULL;
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
VVOD MENU(3);

MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Работа с товарно-материальными документами"));

MENU.VVOD_SPISOK_add_MD(gettext("Ввод нового документа"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Поиск введеного документа"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Просмотр всех введеных документов"));//2
sprintf(bros,"%s %d.%d.%d%s",gettext("Просмотр документов начиная с"),\
dt,mt,gt,gettext("г."));//3
MENU.VVOD_SPISOK_add_MD(bros);
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт средневзвешенных цен на материалы"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Импорт документов"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Списание розничной торговли"));//6
MENU.VVOD_SPISOK_add_MD(gettext("Установить/снять блокировку дат"));//7
MENU.VVOD_SPISOK_add_MD(gettext("Выписка доверенности"));//8
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//9

kom=0;

for(;;)
 {
  clear();

  while(menu3(&MENU,&kom,1,-1,-1) != 0);


  switch (kom)
   {
    case 9:
    case -1 :
      return;
      break;

    case 0 :
      vvoddok();
      break;

    case 1 :
      poimatdok();
      break;

    case 2 :
      godn=startgod;
      if(godn == 0)
       godn=gt;
      smdok(0,0,godn);
      break;

    case 3 :
      smdok(dt,mt,gt);
      break;

    case 4 :
      srvcen();
      break;

    case 5 :
      impmatdok();
      break;

    case 6 :
      sprozn(0,ffs);
      break;

    case 7 :
      iceb_tl_blok();
      break;


    case 8:
       dok_dover();
       break;

    default:
      kom=0;
      break;
   }
 }
}
/**************************************/
/*Проверка переноса сальдо*/
/******************************/
void xmu_saldo()
{
char strsql[512];
short dt=0,mt=0,gt=0;
poltekdat(&dt,&mt,&gt);

if(mt <= 2)
 return;

class iceb_tu_str god("");

iceb_t_poldan("Стартовый год",&god,"matnast.alx");

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
  iceb_t_soob(gettext("Необходимо перенести стартовые остатки по материалам!"));
  return;
 }


}
/****************************************/
/*Удаление за год*/
/******************************************/
void xmu_udgod()
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
      matuddok();
      break;

    case 1:
      poigod(2);
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
void xmu_inst()
{
class spis_oth oth;
int kom=0;
short dt,mt,gt;
class VVOD DANET(1);
class VVOD MENU(3);
class VVOD PAR(2);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Инструменты"));

MENU.VVOD_SPISOK_add_MD(gettext("Работа с файлами распечаток"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Сменить базу"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Сменить принтер"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Дополнительные программы"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Проверка логической целосности базы данных"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Удалить все записи розничной торговли"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Перенос стартовых остатков по материалам"));//6
MENU.VVOD_SPISOK_add_MD(gettext("Удаление документов за год"));//7
MENU.VVOD_SPISOK_add_MD(gettext("Экспорт остатков материалов"));//8
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//9

for(;;)
 {
  clear();

  if(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0)
   continue;
  class spis_oth oth;   
  switch (kom)
   {
    case 9:
    case -1 :
      return;

    case 0:

      dirlst("lst,txt,csv,xml,dbf");
      break;


    case 1:
      if(smenabaz(1) == 0)
       {
        xmu_start();/*начало работы*/
        sp_fio_klad.free_class();
        sp_kod_sklad.free_class();
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
      matulk();
      break;

    case 5:

      DANET.VVOD_delete();
      DANET.VVOD_SPISOK_add_MD(gettext("Удалить все записи в розничной торговле ? Вы уверены ?"));

      if(danet(&DANET,2,stdscr) == 2)
        break;
      poltekdat(&dt,&mt,&gt);
      PAR.VVOD_delete();
      PAR.VVOD_SPISOK_add_MD(gettext("Введите пароль"));
      if(parolv(&PAR,dt,mt,gt,1) != 0)
        break;
        
      GDITE();

      if(sql_zap(&bd,"delete from Roznica") != 0)
       msql_error(&bd,gettext("Ошибка удаления записей !"),"delete from Roznica");

      break;

    case 6 :
      saldomu();
      break;

    case 7:
      xmu_udgod();
      break;

    case 8:
      if(eksmu(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;     

    default:
      kom=0;
      break;
   }
 }

}
/******************/
/*начало работы*/
/********************/
void xmu_start()
{
/*Читаем нормативно-справочную информацию*/
if(nastsys() != 0)
 exit_iceb(1);
if(matnast() != 0)
 exit_iceb(1);

if(iceb_t_perzap(1) != 0) /*Проверка на первичный запуск в текущем дне и удаление *.lst */
 exit_iceb(1);

iceb_t_blokps(); /*автоматическая блокировка подсистемы*/

xmu_saldo(); /*проверка перенесения стартовых остатков*/
}
