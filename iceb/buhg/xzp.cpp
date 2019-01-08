/* $Id: xzp.c,v 5.691 2014/08/31 06:18:18 sasa Exp $ */
/*18.09.2018    09.11.1992      Белых А.И.      xzp.c
Программа расчёта зарплаты
*/
#define DVERSIQ "18.09.2018"
#include        "buhg.h"
void            konrz(void);
void            clearzar(void);
void            dirzarsl(void);
int  rasved(short,class spis_oth*);
void            zarsend(void);
int  rspis(class spis_oth*);
int  spiin(class spis_oth*);
int  spininu(class spis_oth*);
int  rastab(class spis_oth*);
int  svodnu(class spis_oth*);
int  zarppn(class spis_oth*);
int  rasdolgi(class spis_oth*);
int  rasdolgi1(class spis_oth*);
int  reestal(class spis_oth*);
int  vedprz1(class spis_oth*);
int  zarstomn(class spis_oth*);
void            raspp(void);
void		rabkar(void);
int		zarnast(void);
int zarnast(void);
int		zarnastp(void);
int otrshd(class spis_oth*);
void		kategor(int);
void            dirsdf8dr(void);
void            dirtabelo(short,short);
int raszpodr(class spis_oth*);
int zarrasp(class spis_oth*);
int socstrm(class spis_oth*);
int zarspek(class spis_oth*);
void            zarspek1(void);
void            tabel(void);
void		raszall(int);
void		viknu(void);
int rasnash(class spis_oth*);
int raspol(class spis_oth*);
void		zarkart(int);
int zarsprsoc(class spis_oth*);
void		impzkart(void);
void		sprtab(int);
int nahpodr(class spis_oth*);
void            imp_ks(void);
int  snu(class spis_oth*);
int  zar_sdpf(class spis_oth*);
void xzp_fn(void);
int  dolizs(class spis_oth*);
void dirzardk(void);
int  zarszk(class spis_oth*);
void zar_rp(void);
int  zartarrozrs(class spis_oth*);
void poizardok();
void xzp_dok();
int zartarrozrs(class spis_oth *oth);
int zarkdpen(class spis_oth *oth);
int zarpensm1(class spis_oth *oth);
void zarsbka();
void saldozr();
void dirzarsoc();
int zaresvo(class spis_oth *oth);
int zarpd(class spis_oth *oth);
int zarkvrt(class spis_oth *oth);
int zarkvrt1(class spis_oth *oth);
int zarboln(class spis_oth *oth);
int zarpdoh(class spis_oth *oth);
int zarsnpm(class spis_oth *oth);
void dirzarskrdh();
void dirzarsvd();
void dirzarmzpm();
void dirzarrud();
void dirzarlgot();
void dirzargr(int metka_s);
void xzp_saldo();
void xzp_start();
void xzp_inst();
void impzkartf(void);
int zarsn(class spis_oth *oth);
int zarbol(class spis_oth *oth);
int zarsnpsmr(class spis_oth *oth);
void sprtabotp_m();

short           ddd,mmm,ggg; /*Месяц год*/
extern char	*imabaz;
extern char	*host;
extern char	*parol_pol;
SQL_baza	bd;
extern int iceb_kod_podsystem;

int main(int argc,char **argv,char **envp)
{
class iceb_tu_str bros("");
struct  tm      *bf;
time_t          tmm;
char            bros1[512];
int             kom,kom1;
unsigned int    i;
int uid=0;
class iceb_tu_str fioop;

iceb_kod_podsystem=ICEB_PS_ZP;

icebstart(argc,argv);

xzp_start();

VVOD MENU(4);
VVOD VVOD1(2);

time(&tmm);
bf=localtime(&tmm);

ddd=bf->tm_mday;
if(bf->tm_mday > 14)
   mmm=bf->tm_mon+1;
else
   mmm=bf->tm_mon+1-1;
ggg=bf->tm_year+1900;
if(mmm == 0)
 {
  mmm=12;
  ggg-=1;
 }

sprintf(bros1,"%d.%d%s",mmm,ggg,gettext("г."));
bros.new_plus(bros1);
i=0;

VVOD1.VVOD_SPISOK_add_MD(gettext("Исправте дату если нужно и нажмите \"Enter\" для продолжения"));

naz1:;

clear();

vvod1(&bros,11,&VVOD1,NULL,stdscr,-1,-1);

if(rsdat1(&mmm,&ggg,bros.ravno()) != 0)
 {
  i++;
  if(i == 2)
   goto kon;
   
  iceb_t_soob(gettext("Не верно введена дата !"));  
  goto naz1;
 }

uid=iceb_t_getuid();
fioop.plus(iceb_t_getfioop());

kom = 0;

for(;;)
 {

  clear(); /*Очистить экран и закрасить фоновым цветом*/

  MENU.VVOD_delete();

  MENU.VVOD_SPISOK_add_ZAG(gettext("Программа расчёта заработной платы"));
  MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);

  MENU.VVOD_SPISOK_add_MD(gettext("Работа с карточками"));//0
  MENU.VVOD_SPISOK_add_MD(gettext("Ввод нормативно-справочной информации"));//1
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатать отчёты"));//2
  MENU.VVOD_SPISOK_add_MD(gettext("Сменить текущую дату"));//3
  MENU.VVOD_SPISOK_add_MD(gettext("Инструменты"));//4
  MENU.VVOD_SPISOK_add_MD(gettext("Конец работы"));//5

  znak(0,COLS-26);
  infosys(12,COLS-27,host,VERSION,DVERSIQ,0,mmm,ggg,imabaz,uid,fioop.ravno());

  if(menu3(&MENU,&kom,1,0,0) != 0)
   continue;

  switch (kom)
   {
    case 5 :
    case -1 :
	   goto kon;
	   break;

    case 0 :
      rabkar();
      break;

    case 1 :
      vnsi(-1);
      break;
    case 2 :
      raspp();
      break;

    case 3:

naz:;
      helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
      
      bros.new_plus("");
      VVOD1.VVOD_delete();
      VVOD1.VVOD_SPISOK_add_MD(gettext("Введите новую дату (м.[г])"));
      if((kom1=vvod1(&bros,11,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
        break;
      if(kom1 == ESC)
        break;


      if(polen(bros.ravno(),bros1,sizeof(bros1),1,'.') == 0)
       {
        if(rsdat1(&mmm,&ggg,bros.ravno()) != 0)
         {
          iceb_t_soob(gettext("Не верно введена дата !"));  
          goto naz;
         }
       }
      else
       {
        mmm=bros.ravno_atoi();
        if(mmm < 1 || mmm > 12)
         {
          iceb_t_soob(gettext("Не верно введён месяц !"));  
          goto naz;
         }
       }
      break;

    case 4:
      xzp_inst();
      break;


    default:
      kom=0;
      break;
   }
 }

kon:;

exit_iceb(0);

return(0);
}


/*********************************************/
/*Меню ввода нормативно-справочной информации*/
/*********************************************/

void            vnsi(int rr)
{
int             kom=0;
int		poz,komv;
class iceb_tu_str kodn("");
class iceb_tu_str naim("");
VVOD MENU(3);

kom=0;
if(rr == -1)
 {
  MENU.VVOD_SPISOK_add_MD(gettext("Настройка программы"));
  MENU.VVOD_SPISOK_add_MD(gettext("Ввод данных для автоматизированного расчёта"));
  MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
  rr=0;
  while(menu3w(stdscr,&MENU,&rr,-1,-1,0) != 0); 

  if(rr == -1 || rr == 2 || rr == KEY_RESIZE)
   return;
  
 }

kom=0;
for(;;)
 {
  if(rr == 0)
   {
    clear();
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
    MENU.VVOD_SPISOK_add_ZAG(gettext("Настройка программы"));

    MENU.VVOD_SPISOK_add_MD(gettext("Работа с перечнем групп подразделений"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Работа с перечнем подразделений"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Работа с перечнем категорий"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Работа с перечнем начислений"));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Работа с перечнем удержаний"));//4
    MENU.VVOD_SPISOK_add_MD(gettext("Работа с перечнем званий"));//5
    MENU.VVOD_SPISOK_add_MD(gettext("Работа с перечнем видов табелей"));//6
    MENU.VVOD_SPISOK_add_MD(gettext("Работа с настройками для автоматизированнного расчёта"));//7
    MENU.VVOD_SPISOK_add_MD(gettext("Работа со стартовыми сальдо"));//8
    MENU.VVOD_SPISOK_add_MD(gettext("Работа с перечнем городов налоговых инспекций"));//9
    MENU.VVOD_SPISOK_add_MD(gettext("Работа с перечнем начислений на зарплату"));//10
    MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка списка контрагентов"));//11
    MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка списка банков"));//12
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//13

    while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

    class iceb_tu_str kodd("");
    class iceb_tu_str naim("");

    switch (kom)
     {
      case 13:
      case -1:
	   return;
	   break;

      case 0 :  //Ввод групп подразделений
        clear();
        kodn.new_plus("");
        naim.new_plus("");
        poz=komv=0;
        dirmat1("Zargrupp",&kodn,&naim,LINES-5,0,2,0,&poz,&komv,"","\0");
        break;

      case 1 :
        /*Ввод подразделений*/
        clear();
        kodn.new_plus("");
        naim.new_plus("");
        dirzarpodr(0,&kodn,&naim);
        break;

      case 2 :
	   /*Ввод категорий*/
        clear();
        kodn.new_plus("");
        naim.new_plus("");
        poz=komv=0;
        dirmat1("Kateg",&kodn,&naim,LINES-5,0,2,0,&poz,&komv,"","\0");
        break;

      case 3 :
       /*Ввод начислений*/
        dirnach(0,&kodd,&naim);

        break;

      case 4 :
        /*Ввод удержаний*/
        diruder(0,&kodd,&naim);
        break;

      case 5 :
        /*Ввод званий*/
        dirzvan(0,&kodn,&naim);
        break;

      case 6 :
        /*Ввод видов табеля*/
        clear();
       kodn.new_plus("");
       naim.new_plus("");
        poz=komv=0;
        dirmat1("Tabel",&kodn,&naim,LINES-5,0,2,0,&poz,&komv,"","\0");
        break;

      case 7 :
        xzp_fn();
	 break;


      case 8 :
	   dirzarsl();
	   break;



      case 9 :
	   /*Ввод городов налоговых инспекций*/
        clear();
       kodn.new_plus("");
       naim.new_plus("");
        poz=komv=0;
        dirmat1("Gnali",&kodn,&naim,LINES-5,0,2,0,&poz,&komv,"","\0");
        break;

      case 10:
	   /*Ввод соц. перечислений*/
        dirzarsoc();
        break;
        
      case 11 :
       clear();
       kodn.new_plus("");
       naim.new_plus("");
       dirkontr(&kodn,&naim,0);

       break;

      case 12 :
       clear();

       dirsb(0,&kodd,&naim);

       break;

      default:
        kom=0;
        break;
     }
  }

 if(rr == 1)
  {
   char flag_edit[128];
   memset(flag_edit,'\0',sizeof(flag_edit));
   class icebt_sql_flag flagedit;

    clear();
    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
    MENU.VVOD_SPISOK_add_ZAG(gettext("Ввод и корректировка данных для автоматизованого расчёта"));

    MENU.VVOD_SPISOK_add_MD(gettext("Ввести данные для удержания кредита"));//0
    MENU.VVOD_SPISOK_add_MD(gettext("Ввести данные для удержания алиментов"));//1
    MENU.VVOD_SPISOK_add_MD(gettext("Ввести перечень должностных окладов"));//2
    MENU.VVOD_SPISOK_add_MD(gettext("Ввести данные для автоматизированного расчёта начислений"));//3
    MENU.VVOD_SPISOK_add_MD(gettext("Ввести список инвалидов"));//4
    MENU.VVOD_SPISOK_add_MD(gettext("Ввести список работников работающих по договорам"));//5
    MENU.VVOD_SPISOK_add_MD(gettext("Ввести процент выполнения плана по подразделениям"));//6
    MENU.VVOD_SPISOK_add_MD(gettext("Ввести данные для расчёта индексации начислений"));//7
    MENU.VVOD_SPISOK_add_MD(gettext("Ввести данные для расчёта индексации на невовремя выплаченную зарплату"));//8
    MENU.VVOD_SPISOK_add_MD(gettext("Ввести перечень пенсионеров"));//9
    MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка льгот для расчёта подоходного налога"));//10
    MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка персональных процентов отчисления до пенс.фонда"));//11
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//12

    while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

    switch (kom)
     {
      case 12 :
      case -1 :
	   return;

      case 0 :
         dirzarrud();
	 break;

      case 1 :
         iceb_redfil("zaralim.alx",0);
	 break;

      case 2 :
         dirzarsdo(0);
	 break;

      case 3 :
         sprintf(flag_edit,"%szarnrn_edit",imabaz);
         if(flagedit.flag_on(flag_edit) != 0)
          {
           char bros[512];
           sprintf(bros,"%s\n%s",gettext("С файлом уже работает другой оператор !"),"zarproc.alx");
           iceb_t_soob(bros);
           break;
          }
         iceb_redfil("zarproc.alx",0);
         flagedit.flag_off();
	 break;



      case 4 :
         dirzargr(2);
	 break;

      case 5 :
         dirzargr(3);
	 break;

      case 6 :
         iceb_redfil("zarpodpr.alx",0);
	 break;

      case 7 :
         iceb_redfil("zarindex.alx",0);
	 break;

      case 8 :
         iceb_redfil("zarindexv.alx",0);
	 break;

      case 9 :
         dirzargr(1);
	 break;

      case 10 :
         dirzarlgot();
 	 break;

      case 11:
        iceb_redfil("zarsppo.alx",0);
        break;

        
      default:
        kom=0;
        break;
    }
   }
 }
}
/*********************/
/*Работа с формой 8др*/
/*********************/

void		form8dr()
{
int		kom;

VVOD MENU(3);

kom=0;


MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Отчёты в \"Государственную фискальную службу Украины\""));

MENU.VVOD_SPISOK_add_MD(gettext("Расчёт подоходного на выданную зарплату"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт формы 1ДФ"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Работа с архивом документов формы 1ДФ"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт ЕСВ на общеобязательное государственное страхование"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт суммы ЕСВ на выплаченную зарплату"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//5

naz:;

clear();

while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
class spis_oth oth;
switch (kom)
 {
  case 5 :
  case -1 :
    return;
  case 0:
    if(zarpd(&oth) == 0)
     iceb_t_rabfil(&oth,"");
    goto naz;
  case 1:
   if(zarkvrt1(&oth) == 0)
     iceb_t_rabfil(&oth,"");
    goto naz;
  case 2:
    dirsdf8dr();
    goto naz;

  case 3 :
    if(zarpensm1(&oth) == 0)
     iceb_t_rabfil(&oth,"");
    break;

  case 4 :
    if(zaresvo(&oth) == 0)
     iceb_t_rabfil(&oth,"");
    break;

  default:
    kom=0;
    goto naz;
 }
}
/***************************/
/*Отчёты в соц фонды*/
/****************************/
void xzp_socf()
{
int kom1=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Распечатать своды отчислений в гос. фонды"));

MENU.VVOD_SPISOK_add_MD(gettext("Распечатать отчёты в \"Государственную фискальную службу Украины\""));//0
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать спектр распределения зарплаты"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать свод расчёта ЕСВ"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать расчёт отчислений на выданную зарплату"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт статистической формы 1-ПВ"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать отчёт по пенсионерам и инвалидам"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт календарных дней для пенсионного стажа"));//6
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать справку по больничным"));//7
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт удержаний с больничного"));//8
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//9

for(;;)
 {
  clear();

  while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
  class spis_oth oth;
      
  switch (kom1)
   {
    case 9:
    case -1:
      return;

    case 0:
      form8dr();
      break;

    case 1:
      if(zarspek(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 2:
      if(socstrm(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 3:
      if(zarrasp(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;          

    case 4:
      zarspek1();
      break;

    case 5:
      if(zar_sdpf(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 6:
      if(zarkdpen(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 7:
      if(zarboln(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 8:
      if(zarbol(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;
   }
 }

}
/********************************/
/*Своды начислений по работникам*/
/*******************************/

void xzp_snpr()
{
int kom=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Распечатать своды начислений по работникам"));

MENU.VVOD_SPISOK_add_MD(gettext("Распечатать свод начислений по работникам"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать свод начислений и командировочных расходов по месяцам"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать свод начислений по месяцам"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать свод настроек начислений по работникам"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать свод начислений по счетам, месяцам, работникам"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//5

for(;;)
 {
  clear();

  if(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0)
   continue;
  class spis_oth oth;   
  switch (kom)
   {
    case 5:
    case -1 :
      return;


    case 0 :
       if(rasnash(&oth) == 0)
        iceb_t_rabfil(&oth,"");
      break;

    case 1 :
      if(zarszk(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 2 :
      if(zarsnpm(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 3 :
      if(zarsn(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 4 :
     
      if(zarsnpsmr(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;
       
   }
 }
}

/********************************/
/*распечатать перерасчёта*/
/*******************************/

void xzp_pereras()
{
int kom=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Распечатать перерасчёты"));

MENU.VVOD_SPISOK_add_MD(gettext("Распечатать перерасчёт подоходного налога"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Перерасчёт доходов"));//1
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


    case 0 :
      if(zarppn(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      zarnast(); //Перечитываем настройки потому, что они менялись в предыдыдущей подпрограмме
      break;

    case 1 :
      if(zarpdoh(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

   }
 }
}
/********************************/
/*распечатать долги*/
/*******************************/

void xzp_dolgi()
{
int kom=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Распечатать долги"));

MENU.VVOD_SPISOK_add_MD(gettext("Распечатать долги предприятия перед работниками"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать долги работников за месяц"));//1
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

    case 0 :
      if(rasdolgi(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;
    case 1 :
      if(rasdolgi1(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

   }
 }
}



/****************************/
/*Меню для выбора распечатки*/
/****************************/
void            raspp()
{
int             kom,kom1,kom2;

VVOD MENU(3);

kom=0;
for(;;)
 {
  clear();
  MENU.VVOD_delete();
  MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
  MENU.VVOD_SPISOK_add_ZAG(gettext("Распечатка отчётов"));

  MENU.VVOD_SPISOK_add_MD(gettext("Распечатать ведомость заработной платы"));//0
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатать свод по категориям"));//1
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатать отчёты по работникам"));//2
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатать реестр почтовых переводов алиментов"));//3
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатать отчёты по балансовым счетам"));//4
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатать заготовку для заполнения табеля"));//5
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатать своды отчислений в гос. фонды"));//6
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатать отчёты по подразделениям"));//7
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатать свод начислений и удeржаний"));//8
  MENU.VVOD_SPISOK_add_MD(gettext("Расчёт частей зарплатных счетов"));//9
  MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//10

  while(menu3(&MENU,&kom,1,-1,-1) != 0);
  
  class spis_oth oth;
  
  switch (kom)
   {
    case 10 :
    case -1 :
      return;


    case 0 :

      MENU.VVOD_delete();

      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать по подразделению в порядке возростания табельных номеров"));
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать по подразделению в алфавитном порядке"));
      MENU.VVOD_SPISOK_add_MD(gettext("Автоматическая разноска выплаченной зарплаты"));
      MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
      kom1=0;
      while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
      
      switch (kom1)
       {
	case 3 :
	case -1 :
	  break;

	case 0 :
	   if(rasved(0,&oth) == 0)
	    iceb_t_rabfil(&oth,"");
	   break;

	case 1 :
	   if(rasved(1,&oth) == 0)
	    iceb_t_rabfil(&oth,"");
	   break;
	case 2 :
	   zarsend();
	   break;
        default:
          break;
       }
      break;

    case 1 :
      MENU.VVOD_delete();

      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать свод начислений и удержаний по категориям"));
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать свод начислений/счет и удержаний/счет по категориям"));
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать свод отработанного времени по категориям"));
      MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
      kom1=0;
      while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
             
      switch (kom1)
       {
	case 3 :
	case -1 :
	  break;

	case 0 :
          kategor(0);
          break;

	case 1 :
          kategor(1);
          break;

	case 2 :
          if(otrshd(&oth) == 0)
           iceb_t_rabfil(&oth,"");
          break;

        default:
          break;        
       }
      break;

    case 2 :
      kom1=0;
nazc2:;
      clear();
      MENU.VVOD_delete();
      MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);

      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать список работников"));//0
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать справки о зарплате"));//1
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать список работников, которые имеют начисления или удержания"));//2
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать список работников, которые не имеют начисления или удержания"));//3
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать свод отработанного времени по работникам"));//4
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать свод начислений и удержаний по работникам"));//5
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать перерасчёт"));//6
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать долги"));//7
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать свод начислений по работникам"));//8
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать распределение зарплаты мужчины/женщины"));//9
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать карточки начислений/удержаний по работникам"));//10
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать список работников, которым начислена благотворительная помощь"));//11
      MENU.VVOD_SPISOK_add_MD(gettext("Расчёт премии"));//12
      MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//13

      while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
      oth.clear();
      switch (kom1)
       {
	case 13:
	case -1:
	  break;

	case 0 :
          if(rspis(&oth) == 0)
           iceb_t_rabfil(&oth,"");
          goto nazc2;

	case 1 :
          kom2=0;
          for(;;)
           {
            clear();
            MENU.VVOD_delete();

            MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);

            MENU.VVOD_SPISOK_add_MD(gettext("Распечатать справку о доходах"));//0
            MENU.VVOD_SPISOK_add_MD(gettext("Распечатать расчёт средней зарплаты для начисления больничного"));//1
            MENU.VVOD_SPISOK_add_MD(gettext("Распечатать справку для расчёта выплат на случай безработицы"));//2
            MENU.VVOD_SPISOK_add_MD(gettext("Распечатать справку о доходах в счёт месяцев начисления"));//3
            MENU.VVOD_SPISOK_add_MD(gettext("Распечатать расчёт отпускных"));//4
            MENU.VVOD_SPISOK_add_MD(gettext("Расчёт тарифов на базе тарифной сетки"));//5
            MENU.VVOD_SPISOK_add_MD(gettext("Распечатать расчёт больничного по беременности и родам"));//6
            MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//7

            while(menu3w(stdscr,&MENU,&kom2,-1,-1,0) != 0);
            
            oth.clear();
            switch(kom2)
             {
              case 7:
              case -1 :
                goto nazc2;
              case 0:
                sprtab(1);
                break;
              case 1:
                sprtab(2);
                break;
              case 2:
                sprtab(3);
                break;
              case 3:
                sprtab(4);
                break;
            
              case 4:
                sprtabotp_m();
                break;
              case 5:
                if(zartarrozrs(&oth) == 0)
                  iceb_t_rabfil(&oth,"");
                break;

              case 6:
                sprtab(5);
                break;
             }
           }
          break;
          
        case 2 :
          if(spiin(&oth) == 0)
           iceb_t_rabfil(&oth,"");
          goto nazc2;

        case 3 :
          if(spininu(&oth) == 0)
           iceb_t_rabfil(&oth,"");
          goto nazc2;

        case 4:
          if(rastab(&oth) == 0)
           iceb_t_rabfil(&oth,"");
          goto nazc2;

        case 5:
          if(svodnu(&oth) == 0)
           iceb_t_rabfil(&oth,"");
          goto nazc2;

        case 6:
          xzp_pereras();
          goto nazc2;
	case 7 :
          xzp_dolgi();
          goto nazc2;

	case 8 :
          xzp_snpr();
          goto nazc2;

	case 9 :
          if(raspol(&oth) == 0)
           iceb_t_rabfil(&oth,"");
          goto nazc2;

	case 10 :
          zarkart(0);
          goto nazc2;

	case 11 :
          zarkart(1);
          goto nazc2;


	case 12 :
          zar_rp();
          goto nazc2;


        default:
          goto nazc2;
          
       }
      break;
      
    case 3 :
       if(reestal(&oth) == 0)
        iceb_t_rabfil(&oth,"");
       break;
       
    case 4 :
      MENU.VVOD_delete();

      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать ведомость производственных затрат"));//0
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать доли бюджетных счетов"));//1
      MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2
      kom1=0;
      while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
      
      switch (kom1)
       {
	case 2 :
	case -1 :
          break;
	case 0 :
          if(vedprz1(&oth) == 0)
           iceb_t_rabfil(&oth,"");
          break;

        case 1:
          if(zarstomn(&oth) == 0)
           iceb_t_rabfil(&oth,"");
          break;
        default:
          break;          
       }
      break;


    case 5:
      tabel();
      break;

    case 6:
      xzp_socf();
      break;

    case 7:

      MENU.VVOD_delete();

      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать свод начислений и удержаний по подразделениям (форма 1)"));
      MENU.VVOD_SPISOK_add_MD(gettext("Распечатать свод начислений и удержаний по подразделениям (форма 2)"));
      MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
      kom1=0;
      while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
      
      switch (kom1)
       {
	case 2 :
	case -1 :
          break;
	case 0 :
          if(nahpodr(&oth) == 0)
           iceb_t_rabfil(&oth,"");
          break;
          
	case 1 :
          if(raszpodr(&oth) == 0)
           iceb_t_rabfil(&oth,"");
          break;
       }
      break;


    case 8:
      if(snu(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;
     
    case 9:
      if(dolizs(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;
     
    default:
      kom=0;
      break;
   }
 }
}
/*********************/
/*Импорт карточек*/
/*********************/

void xzp_impkartr()
{
int		kom;

VVOD MENU(3);

kom=0;


MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Импорт информации в базу"));

MENU.VVOD_SPISOK_add_MD(gettext("Импорт списка работников"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Импорт карт-счетов из файла"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Импорт карточек работников"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//3

naz:;

clear();

while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
class spis_oth oth;
switch (kom)
 {
  case 3 :
  case -1 :
    return;

  case 0:
    impzkart();
    goto naz;

  case 1:
    imp_ks();
    goto naz;

  case 2:
    impzkartf();
    goto naz;

  default:
    kom=0;
    goto naz;
 }
}
/**************************************/
/*выбор подразделения*/
/******************************/
void xzp_vp()
{
static class iceb_tu_str kod("");
class iceb_tu_str naim("");
int kom1=0;
class VVOD VVOD1(2);
VVOD1.VVOD_delete();
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код подразделения"));

naz:;

helstr(LINES-1,0,"F10",gettext("выход"),NULL);

kom1=vvod1(&kod,20,&VVOD1,NULL,stdscr,-1,-1);

switch(kom1)
 {
  case ESC:
  case FK10:
   return;
 }

int tabn=0;
class iceb_tu_str fio("");

if(kod.getdlinna() <= 1)
 {
  if(dirzarpodr(2,&kod,&naim) == 0)
   dirtab(&tabn,&fio,kod.ravno_atoi(),0);
 }
else
 {
  char strsql[512];
  sprintf(strsql,"select kod from Podr where kod=%d",kod.ravno_atoi());
  if(readkey(strsql) != 1)
   {
    sprintf(strsql,"%s %d %s!",gettext("Не найден код подразделения"),kod.ravno_atoi(),gettext("в справочнике подразделений"));
    iceb_t_soob(strsql);
    goto naz;
   }

  dirtab(&tabn,&fio,kod.ravno_atoi(),0);
 }
}


/************************/
/* Режимы работы с карточками */
/*******************************/
void		rabkar()
{
int             kom,kom1;
int		tabn;
class iceb_tu_str fio("");
int		pod;
char		bros[512];
class iceb_tu_str kod("");
class iceb_tu_str vstr("");
VVOD MENU(3);
VVOD VVOD1(2);

kom=0;

naz:;
clear();

MENU.VVOD_delete();

MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);

sprintf(bros,"%s %s %d.%d%s",gettext("Работа с карточками работников"),
gettext("Текущая дата"),
mmm,ggg,gettext("г."));

MENU.VVOD_SPISOK_add_ZAG(bros);

MENU.VVOD_SPISOK_add_MD(gettext("Поиск по табельному номеру"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Просмотр работников отдельного подразделения"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Просмотр всех работников"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Ввод нового работника"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Ввод отработанного времени"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Автоматизированный расчёт зарплат по всем карточкам"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка начисления или удержания"));//6
MENU.VVOD_SPISOK_add_MD(gettext("Контроль расчёта зарплаты"));//7
MENU.VVOD_SPISOK_add_MD(gettext("Установить/снять блокировку дат"));//8
MENU.VVOD_SPISOK_add_MD(gettext("Импорт информации в базу данных"));//9
MENU.VVOD_SPISOK_add_MD(gettext("Работа с документами"));//10
MENU.VVOD_SPISOK_add_MD(gettext("Автоматическая разноска выплаченной зарплаты"));//11
MENU.VVOD_SPISOK_add_MD(gettext("Автоматическая смена банка в карточках"));//12
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//13

while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

switch (kom)
 {
  case 13 :
  case -1 :
    return;

  case 0 :
    helstr(LINES-1,0," F10 ","вихід",NULL);
    
    VVOD1.VVOD_delete();
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите табельный номер"));
    vstr.new_plus("");
case0:;
    if((kom1=vvod1(&vstr,10,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
      goto naz;
    if(kom1 == ESC)
      goto naz;
    if((tabn=atol(vstr.ravno())) == 0)
     {
      kod.new_plus("");
      if(dirtab(&tabn,&kod,0,1) == 0)
       {
        vstr.new_plus(tabn);        
        clear();
        goto case0;
       }  
      goto naz;
     }
    GDITE();
    vkart(tabn,1);
    goto naz;

  case 1 :
   xzp_vp();
   goto naz;

  case 2 :
   tabn=0;
   pod=0;
   dirtab(&tabn,&fio,pod,0);
   goto naz;

  case 3 :
    vkart(0,0);
    goto naz;

  case 4 :
    dirtabelo(mmm,ggg);
    goto naz;

  case 5 :

    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_ZAG(gettext("Автоматизированный расчёт зарплат по всем карточкам"));

    MENU.VVOD_SPISOK_add_MD(gettext("Расчёт только удержаний по всем карточкам"));
    MENU.VVOD_SPISOK_add_MD(gettext("Расчёт только начислений по всем карточкам"));
    MENU.VVOD_SPISOK_add_MD(gettext("Расчёт начислений и удержаний по всем карточкам"));
    MENU.VVOD_SPISOK_add_MD(gettext("Расчёт только начислений на фонд оплаты труда"));
    MENU.VVOD_SPISOK_add_MD(gettext("Расчёт только проводок"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));
    kom1=0;
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);
    
    switch (kom1)
     {
      case 5 :
      case -1 :
        break;
      case 0:
        raszall(2);
        break;
      case 1:
        raszall(1);
        break;
      case 2:
        raszall(3);
        break;
      case 3:
        raszall(4);
        break;
      case 4:
        raszall(5);
        break;
     }
    goto naz;

  case 6 :
    viknu();
    goto naz;

  case 7:
    konrz();
    goto naz;

  case 8 :
    iceb_tl_blok();
    goto naz;

  case 9 :
    xzp_impkartr();
    goto naz;

  case 10 :
    xzp_dok();
    goto naz;

  case 11:
    zarsend();
    goto naz;

  case 12:
    zarsbka();
    goto naz;

  default:
    kom=0;
    goto naz;
 }
}
/*********************/
/*Работа с настроечными файлами*/
/*********************/

void xzp_fn()
{
int		kom;

VVOD MENU(3);

kom=0;


MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Работа с файлами настройки"));

MENU.VVOD_SPISOK_add_MD(gettext("Работа с файлом настройки"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка количества рабочих дней и часов"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Настройка выполнения проводок"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Работа со списком праздничных и выходных дней"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка минимальной зарплаты, прожиточного минимума"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка тарифных розрядов и коэффициентов для бюджетных организаций"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка тарифной сетки"));//6
MENU.VVOD_SPISOK_add_MD(gettext("Настройка списка банков для перечисления сумм на картсчета"));//7
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//8

naz:;

clear();

while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

switch (kom)
 {
  case 8 :
  case -1 :
    return;

  case 0:
    if(iceb_redfil("zarnast.alx",0) == 0)
       zarnast();
    goto naz;
  case 1:
    dirzarskrdh();
    goto naz;

  case 2:
    zarnvp();
    goto naz;

  case 3:
    dirzarsvd();
    goto naz;

  case 4:
    dirzarmzpm();
    goto naz;

  case 5:
    iceb_redfil("zartarroz.alx",0);
    goto naz;

  case 6:
    iceb_redfil("zartarroz1.alx",0);
    goto naz;

  case 7:
    iceb_redfil("zarbanks.alx",0);
    goto naz;

  default:
    kom=0;
    goto naz;
 }
}
/***********************/
/*Ввод нового документа*/
/*************************/
void xzp_vdok()
{
VVOD MENU(3);
short prn=0;

MENU.VVOD_SPISOK_add_MD(gettext("Ввод начислений"));
MENU.VVOD_SPISOK_add_MD(gettext("Ввод удержаний"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));


int kom1=0;
while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

switch (kom1)
 {
  case 2 :
  case -1:
    return;

  case 0 :
    prn=1;
    break;

  case 1 :
    prn=2;
    break;
 }

short dv=0;
short mv=0;
short gv=0;
class iceb_tu_str nomdok_v("");



if(dirzardk_vvod(prn,&dv,&mv,&gv,&nomdok_v) == 0)
  dirzardkz(prn,gv,dv,mv,gv,&nomdok_v);
 

}





/*********************/
/*Работа с документами*/
/*********************/

void xzp_dok()
{
int		kom;

VVOD MENU(3);

kom=0;



MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Работа с документами"));

MENU.VVOD_SPISOK_add_MD(gettext("Ввод нового документа"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Поиск документов"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Просмотр всех введённых документов"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//3

naz:;

clear();

while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

switch (kom)
 {
  case 3 :
  case -1 :
    return;

  case 0 :
    xzp_vdok();
    goto naz;

  case 1:
    poizardok();
    goto naz;

  case 2:
    dirzardk();
    goto naz;
    
  default:
    kom=0;
    goto naz;
 }
}
/**************************************/
/*Проверка переноса сальдо*/
/******************************/
void xzp_saldo()
{
char strsql[512];
short dt=0,mt=0,gt=0;
poltekdat(&dt,&mt,&gt);

if(mt <= 2)
 return;

class iceb_tu_str god("");

iceb_t_poldan("Стартовый год",&god,"zarnast.alx");

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
  iceb_t_soob(gettext("Необходимо перенести сальдо по работникам!"));
  return;
 }


}
/************************************/
/*старт программы*/
void xzp_start()
{
/*Ищем настройку*/
if(nastsys() != 0)
 exit_iceb(1);
if(zarnast() != 0)
 exit_iceb(1);

iceb_t_perzap(1); /*Проверка на первичный запуск в текущем дне и удаление *.lst */

iceb_t_blokps(); /*автоматическая блокировка подсистемы*/
xzp_saldo();  /*проверка переноса сальдо по работникам*/
}
/****************************************/
/*Удаление за год*/
/******************************************/
void xzp_udgod()
{
int kom=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Инструменты"));

MENU.VVOD_SPISOK_add_MD(gettext("Удаление начислений и удержаний"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка годов за которые есть записи"));//1
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
      clearzar();
      break;

    case 1:
      poigod(3);
      break;

    default:
      kom=0;
      break;
   }
 }
}
/*******************************/
/*инструмент*/
/*******************************/
void xzp_inst()
{
int kom=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Инструменты"));

MENU.VVOD_SPISOK_add_MD(gettext("Работа с файлами распечаток"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Сменить базу"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Сменить принтер"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Дополнительные программы"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Перенос сальдо"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Удалить ненужные данные по зарплате"));//5
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
        xzp_start();
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
      saldozr();
      break;

    case 5:
      xzp_udgod();
      break;

    default:
      kom=0;
      break;
   }
 }
}
