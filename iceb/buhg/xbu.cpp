/* $Id: xbu.c,v 5.350 2014/08/31 06:18:18 sasa Exp $ */
/*05.03.2018    18.02.1993      Белых А.И.      xbu.c
  Программа ведения бухгалтерских счетов
*/
#define DVERSIQ "05.03.2018"
#include "buhg.h"
void		sheta(void);
void            raspeh(void);
void    	norm(void);
void		vprov(void);
void		prpp(void);
int  glkni(class spis_oth*);
int  glkninbs(class spis_oth*);
int  salallk(class spis_oth*);
int  rnnen(class spis_oth*);
void		clprov(void);
void poiprov();
int   guror(char[],short,short,short,short,short,short,short,char[],class spis_oth*);
int  gurornbs(class guror_rr *rekrs,class spis_oth*);
int  gurorr(class guror_rr *rekrs,class spis_oth*);
int  gurornrs(class guror_rr *rekrs,class spis_oth*);
void		vprovgr(void);
void		impprov(void);
void            vipiska(void);
void		pvbanks(void);
int  buhkom(class spis_oth*);
int  buhhah(class spis_oth*);
int  buhsdkr(class spis_oth*);
int  buhrznp(class spis_oth*);
int  buhrpz(class spis_oth*);
int  buhgsp(class spis_oth*);
int  buhvdz(class spis_oth*);
int  buh_rnr_sh(class spis_oth*);
int  buhspk(class spis_oth*);
int mord1(class spis_oth*);
int mord12(class spis_oth*);
int mord4(int,class spis_oth*);
int mord6(int,class spis_oth*);
int mord5(class spis_oth*);
int mord9(int,class spis_oth*);
void xbu_rmo();
int buhkaogo(class spis_oth*);
int buh_kaukz(class spis_oth*);
int buh_kaufz(class spis_oth*);
int buh_kekv(class spis_oth*);
int buhpsd(class spis_oth *oth);
int gosss(class guror_rr *rekrs,class spis_oth *oth);
void saldogk();
void xbu_saldo();
void xbu_inst();
void xbu_start();
int dirkv(const char *kod_val);

extern int	iscolor;
extern char	*imabaz;
extern char	*host;
extern char	*parol_pol;
int		kassa=0;
SQL_baza	bd;
extern int iceb_kod_podsystem;

int main(int argc,char **argv,char **envp)
{
iceb_kod_podsystem=ICEB_PS_GK;
int             kom;
class iceb_tu_str fioop;
int uid=0;

icebstart(argc,argv);

xbu_start();

class VVOD MENU(4);

uid=iceb_t_getuid();


kom = 0;
for(;;)
 {
  clear();

  MENU.VVOD_delete();

  MENU.VVOD_SPISOK_add_ZAG(gettext("Главная бухгалтеская книга"));
  MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);

  MENU.VVOD_SPISOK_add_MD(gettext("Работа со счетами"));//0
  MENU.VVOD_SPISOK_add_MD(gettext("Ввод нормативно-справочной информации"));//1
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатать отчёты"));//2
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатать отчёты для бюджетных ор-ций Украины"));//3
  MENU.VVOD_SPISOK_add_MD(gettext("Инструменты"));//4
  MENU.VVOD_SPISOK_add_MD(gettext("Конец работы"));//5

  znak(0,COLS-26);
  infosys(12,COLS-27,host,VERSION,DVERSIQ,0,0,0,imabaz,uid,iceb_t_getfioop());


  if(menu3(&MENU,&kom,1,0,0) != 0)
   continue;
   
  switch (kom)
   {

    case 5 :
    case -1 :
      exit_iceb(0);
      break;

    case 0 :
      sheta();
      break;
    case 1 :
      norm();
      break;

    case 2 :
      raspeh();
      break;

    case 3 :
      xbu_rmo();
      break;
    case 4: 
      xbu_inst();
      break;

    default:
       kom=0;
       break;   
   }
 }
exit_iceb(0);

}

/********************************/
/*Меню выбора работы со счетами*/
/********************************/
void            sheta()
{
int             kom=0;
char		strsql[512];
short dt=0,mt=0,gt=0;

VVOD VV(0);
VVOD MENU(3);
poltekdat(&dt,&mt,&gt);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Работа со счетами"));
sprintf(strsql,"%s %d.%d.%d%s",gettext("Текущая дата"),
dt,mt,gt,gettext("г."));
MENU.VVOD_SPISOK_add_ZAG(strsql);

MENU.VVOD_SPISOK_add_MD(gettext("Ввод проводок"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Поиск проводок"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Установка/снятие блокировки проводок"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Ввод проводок по группам контрагентов"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Импорт проводок из файла"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Розноска банковской виписки"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//6

for(;;)
 {

  clear();


  if(menu3(&MENU,&kom,1,-1,-1) != 0)
   continue;
  clearstr(0,0);
  
  class poiprov_rek poi;
     
  switch (kom)
   {
    case 6:
    case -1:
      return;

    case 0:
      vprov();
      break;

    case 1:
       poiprov();
       break;

    case 2:
      iceb_tl_blok();
      break;


    case 3:
      vprovgr();
      break;

    case 4:
      impprov();
      break;

    case 5:
      vipiska();
      break;

    default:
      kom=0;
      break;
  }
 }
}
/******************/
/*Расчёт шахматки*/
/******************/
void xbu_ras_sah()
{
int		kom;

VVOD MENU(3);

kom=0;



MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Расчёт шахматки"));

MENU.VVOD_SPISOK_add_MD(gettext("Расчёт по заданным вариантам расчёта"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт по счёту"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт по заданным спискам дебетовых и кредитовых счетов"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт распределения административных затрат на доходы"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт видов затрат по элементам"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт по спискам групп контрагентов"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт валовых доходов и затрат"));//6
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//7

naz:;

clear();

while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

class spis_oth oth;   

switch (kom)
 {
  case 7 :
  case -1 :
    return;

  case 0 :
    if(buhhah(&oth) == 0)
       iceb_t_rabfil(&oth,"");
    goto naz;

  case 1:
    if(buh_rnr_sh(&oth) == 0)
      iceb_t_rabfil(&oth,"");
    goto naz;

  case 2 :
    if(buhsdkr(&oth) == 0)
     iceb_t_rabfil(&oth,"");
    break;

  case 3 :
    if(buhrznp(&oth) == 0)
     iceb_t_rabfil(&oth,"");
    break;

  case 4 :
    if(buhrpz(&oth) == 0)
     iceb_t_rabfil(&oth,"");
    break;

  case 5 :
    if(buhgsp(&oth) == 0)
     iceb_t_rabfil(&oth,"");
    break;

  case 6 :
    if(buhvdz(&oth) == 0)
     iceb_t_rabfil(&oth,"");
    break;

    
  default:
    kom=0;
    goto naz;
 }
}
/******************/
/*Расчёт сальдо*/
/******************/
void xbu_ras_sal()
{
char strsql[1024];
int		kom;
int kom1=0;
SQL_str row;
class SQLCURSOR cur;
VVOD MENU(3);
VVOD VVOD1(2);

kom=0;



MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Расчёт сальдо"));

MENU.VVOD_SPISOK_add_MD(gettext("Расчёт сальдо по всем контрагентам"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт сальдо по контрагенту"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2

naz:;

clear();

while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

class spis_oth oth;   
class iceb_tu_str nm("");
class iceb_tu_str kodo("");

switch (kom)
 {
  case 2 :
  case -1 :
    return;

  case 0 :
    if(salallk(&oth) == 0)
       iceb_t_rabfil(&oth,"");
    goto naz;

  case 1:
nom3:;
      clearstr(LINES-1,0);
      helstr(LINES-1,0,"F1",gettext("помощь"),
      "Enter",gettext("контрагенты"),"F10",
      gettext("выход"),NULL);
      
      VVOD1.VVOD_delete();
      VVOD1.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);

      VVOD1.VVOD_SPISOK_add_ZAG(gettext("Расчёт сальдо по контрагенту"));
      VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код контрагента"));
      if((kom1=vvod1(&kodo,40,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
       goto naz;


      switch(kom1)
       {
        case ESC:
        case FK10:
          goto naz;

        case FK1:
          GDITE();
          iceb_t_pdoc("buhg5_4.txt");
          goto nom3;

        case ENTER:
          if(kodo.getdlinna() <= 1)
           if(dirkontr(&kodo,&nm,1) != 0)
            {
             clear();
             goto naz;
            }
          break;

        default:
          goto nom3;
          
       }      
      if(kodo.getdlinna() <= 1)
        if(dirkontr(&kodo,&nm,1) != 0)
          break;

      sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kodo.ravno());
      if(readkey(strsql,&row,&cur) != 1)
       {
        sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kodo.ravno());
        iceb_t_soob(strsql);
        break;
       }
      nm.new_plus(row[0]);
      if(salorok(kodo.ravno(),nm.ravno(),&oth) == 0)
       iceb_t_rabfil(&oth,"");
       

    goto naz;

    
  default:
    kom=0;
    goto naz;
 }
}

/****************************/
/*Меню для выбора распечатки*/
/****************************/
void            raspeh()
{
int             kom,kom1,kom2;
short           dn,mn,gn,dk,mk,gk;
static class guror_rr rekrs;
class iceb_tu_str nm("");
class iceb_tu_str kodo("");
SQL_str   	row;
SQLCURSOR       cur;
char		bros[512];
short		nbs=0;
short saldo_rs=0;
VVOD MENU(3);
VVOD VV(0);
VVOD VVOD1(2);

kom=0;
for(;;)
 {

naz1:;
  clear();

  MENU.VVOD_delete();
  MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
  MENU.VVOD_SPISOK_add_ZAG(gettext("Распечатка отчётов"));

  MENU.VVOD_SPISOK_add_MD(gettext("Расчёт оборотного баланса и главной книги"));//0
  MENU.VVOD_SPISOK_add_MD(gettext("Расчёт журнала-ордера по счёту"));//1
  MENU.VVOD_SPISOK_add_MD(gettext("Расчёт сальдо"));//2 
  MENU.VVOD_SPISOK_add_MD(gettext("Расчёт оборотного баланса по внебалансовим счетам"));//3
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатка документов для перечисления на карт-счета"));//4
  MENU.VVOD_SPISOK_add_MD(gettext("Отчёт по комментариям"));//5
  MENU.VVOD_SPISOK_add_MD(gettext("Расчёт шахматки"));//6
  MENU.VVOD_SPISOK_add_MD(gettext("Расчёт акта сверки по группе контрагента"));//7
  MENU.VVOD_SPISOK_add_MD(gettext("Проверка согласованности данных в подсистемах"));//8
  MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//9

  if(menu3(&MENU,&kom,0,-1,-1) != 0)
   continue;
  class spis_oth oth;   
  
  switch (kom)
   {
    case 9:
    case -1 :
      return;

    case 0 :
      if(glkni(&oth) == 0)
       iceb_t_rabfil(&oth,"");
       
      break;

    case 1 :

      VV. VVOD_delete();
      VV.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
      VV.VVOD_SPISOK_add_ZAG(gettext("Расчёт журнала-ордера по счёту"));

      VV.VVOD_SPISOK_add_MD(gettext("Номер счета.................."));
      VV.VVOD_SPISOK_add_MD(gettext("Дата начала........(д.м.г)..."));
      VV.VVOD_SPISOK_add_MD(gettext("Дата конца.........(д.м.г)..."));
      VV.VVOD_SPISOK_add_MD(gettext("Код группы контрагента.(,,).."));
      VV.VVOD_SPISOK_add_MD(gettext("Код валюты..................."));

      VV.VVOD_SPISOK_add_data(rekrs.shet.ravno(),24);
      VV.VVOD_SPISOK_add_data(rekrs.dat1.ravno(),11);
      VV.VVOD_SPISOK_add_data(rekrs.dat2.ravno(),11);
      VV.VVOD_SPISOK_add_data(rekrs.mso.ravno(),112);
      VV.VVOD_SPISOK_add_data(rekrs.kod_val.ravno(),16);

naz:;

      clear();


      helstr(LINES-1,0,"F1",gettext("помощь"),
      "F2/+",gettext("расчёт"),
      "F3",gettext("реквизиты"),
      "F4",gettext("очистить"),
      "F10",gettext("выход"),NULL);      


      kom1=VV.vvod(0,1,1,-1,-1);
      
      rekrs.shet.new_plus(VV.data_ravno(0));
      rekrs.dat1.new_plus(VV.data_ravno(1));
      rekrs.dat2.new_plus(VV.data_ravno(2));
      rekrs.mso.new_plus(VV.data_ravno(3));
      rekrs.kod_val.new_plus(VV.data_ravno(4));
      switch(kom1)
       {
        case FK1:   /*Помощь*/
         GDITE();
         iceb_t_pdoc("buhg5_2.txt");
         goto naz;
       
        case FK2:
        case PLU:
          break;

        case FK3:
          MENU.VVOD_delete();
          MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка счетов"));
          MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка групп контрагентов"));
          MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка валют"));
          MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

          clearstr(LINES-1,0);
          kom2=0;          
          if(menu3w(stdscr,&MENU,&kom2,-1,-1,0) != 0)
           goto naz;
          switch (kom2)
           {
            case 3 :
            case -1:
             break;

            case 0 :
              rekrs.shet.new_plus("");
              if(vibrek("Plansh",&rekrs.shet) == 0)
                 VV.data_plus(0,rekrs.shet.ravno());
              break;

            case 1 :
              vibrek("Gkont",&rekrs.mso);
              VV.data_plus(3,rekrs.mso.ravno());
              break;

            case 2 :
              nm.new_plus("");
              if(dirsval(1,&rekrs.kod_val,&nm) == 0)
               VV.data_plus(4,rekrs.kod_val.ravno());
              break;
            }

          goto naz;

        case FK4:
         VV.VVOD_clear_data();         
         goto naz;

        case FK10:
        case ESC:
          goto naz1;
       
        default:
         kom1=0;
         goto naz;
       }

      if(rekrs.shet.ravno()[0] == '\0')
       {
        iceb_t_soob(gettext("Не введён счёт !"));
        goto naz;
       }

      if(rsdatp(&dn,&mn,&gn,rekrs.dat1.ravno(),&dk,&mk,&gk,rekrs.dat2.ravno()) != 0)
        goto naz;

      sprintf(bros,"select saldo,stat from Plansh where ns='%s'",rekrs.shet.ravno());

      if(readkey(bros,&row,&cur) != 1)
       {
        sprintf(bros,gettext("Не найден счёт %s в плане счетов !"),rekrs.shet.ravno());
        iceb_t_soob(bros);
        goto naz;
       }
      saldo_rs=atoi(row[0]);
      nbs=atoi(row[1]);      
      if(rekrs.kod_val.getdlinna() > 1)
       {
        sprintf(bros,"select kod from Glksval where kod='%s'",rekrs.kod_val.ravno());

        if(readkey(bros) != 1)
         {
          sprintf(bros,"%s %s!",gettext("В справочкике валют не найден код валюты"),rekrs.kod_val.ravno());
          iceb_t_soob(bros);
          goto naz;
         }
       }             

      if(saldo_rs < 3)
       {
        if(nbs == 0)
         {
          if(gosss(&rekrs,&oth) == 0)
            iceb_t_rabfil(&oth,"");
         }
        if(nbs == 1)
          if(gurornbs(&rekrs,&oth) == 0)
            iceb_t_rabfil(&oth,"");
       }
      else
       {
        if(nbs == 0)
          if(gurorr(&rekrs,&oth) == 0)
           iceb_t_rabfil(&oth,"");
           
        if(nbs == 1)
          if(gurornrs(&rekrs,&oth) == 0)
           iceb_t_rabfil(&oth,"");
       }
      break;

    case 2 :
      xbu_ras_sal();
      break;


    case 3 :
      if(glkninbs(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 4 :
      pvbanks();

      break;

    case 5 :
      if(buhkom(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 6 :
      xbu_ras_sah();
      break;



    case 7 :
      if(buhspk(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 8 :
     if(buhpsd(&oth) == 0)
       iceb_t_rabfil(&oth,"");
     break;

    default:
      kom=0;
      goto naz1;
   }
 }
}

/***************************************/
/*Ввод нормативно-справочной информации*/
/***************************************/

void    norm()
{
int             kom=0;

class iceb_tu_str sht("");
class iceb_tu_str naimsh("");

struct OPSHET	shetv;
int		poz,komv;

VVOD VVOD1(2);

VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Ввод и корректировка нормативно-справочной информации"));

MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка плана счетов"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка списка контрагентов"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка списка контрагентов для счёта"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка стартового сальдо по счетам"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка стартового сальдо по контрагентам"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Работа с файлом настройки"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка списка групп контрагентов"));//6
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и корректировка списка суббалансов"));//7
MENU.VVOD_SPISOK_add_MD(gettext("Настройка списка банков для перечисления сумм на картсчета"));//8
MENU.VVOD_SPISOK_add_MD(gettext("Справочник валют"));//9
MENU.VVOD_SPISOK_add_MD(gettext("Справочник курсов валют"));//10
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//11

for(;;)
 {
  clear();

  if(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0)
   continue;

  class iceb_tu_str kod_str("");
  class iceb_tu_str naim_str("");
   
  switch (kom)
   {
    case 11 :
    case -1 :
      return;

    case 0 :

      clear(); /*Очистить экран и закрасить фоновым цветом*/
      
      dirshet(&kod_str,&naim_str,0);
      break;

    case 1 :

     clear(); /*Очистить экран */
     dirkontr(&kod_str,&naim_str,0);

     break;


    case 2 :

     sht.new_plus("");
     naimsh.new_plus("");

lll:;
     clear(); /*Очистить экран и закрасить фоновым цветом*/
     VVOD1.VVOD_delete();
     helstr(LINES-1,0," F10 ",gettext("выход"),NULL);

     VVOD1.VVOD_SPISOK_add_MD(gettext("Введите счёт с развернутым сальдо"));
     if(vvod1(&sht,16,&VVOD1,NULL,stdscr,-1,-1) == FK10)
       break;

     if(sht.getdlinna() <= 1)
      {
       if(dirshet(&sht,&naimsh,1) == 0)
        goto lll;
       else 
        break;
      }

     if(prsh1(sht.ravno(),&shetv) != 0)
       goto lll;

     if(shetv.saldo != 3)
      {
       iceb_t_soob(gettext("Счёт не с развернутым сальдо !"));
       goto lll;
      }

     dirskont(sht.ravno(),&kod_str,&naim_str,0);

     break;

    case 3 :
     clear(); /*Очистить экран и закрасить фоновым цветом*/
     dirsaldo("0");
     break;

    case 4 :
     clear(); /*Очистить экран и закрасить фоновым цветом*/
     dirsaldo("1");
     break;

    case 5 :
      if(iceb_redfil("nastrb.alx",0) == 0)
        buhnast();
      break;

    case 6 :
      clear();
      poz=komv=0;
      dirmat1("Gkont",&kod_str,&naim_str,LINES-5,0,2,0,&poz,&komv,"","\0");
      break;

    case 7:
      dirsubbal(0,&kod_str,&naim_str);
      break;


    case 8:
      iceb_redfil("zarbanks.alx",0);
      break;
            
    case 9:
      dirsval(0,&kod_str,&naim_str);
      break;

    case 10:
      if(dirsval(2,&kod_str,&naim_str) == 0)
       dirkv(kod_str.ravno());
      break;

    default:
      kom=0;
      break;
  }
 }
}
/*****************************************************************************/
/*Расчёт и распечатка мемориальных ордеров для бюджетных организаций Украины*/
/*****************************************************************************/

void xbu_rmo()
{
int kom=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Распечатать отчёты для бюджетных организаций Украины"));

MENU.VVOD_SPISOK_add_MD(gettext("Расчёт мемориальных ордеров 1,2,3"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт мемориалього ордера N4"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт мемориалього ордера N5"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт мемориалього ордера N6"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт мемориалього ордера N7"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт мемориалього ордера N9"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт мемориалього ордера N10"));//6
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт мемориалього ордера N12"));//7
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт мемориалього ордера N13"));//8
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт карточки аналитического учёта наличных операций"));//9
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт карточки аналитического учёта кассовых расходов"));//10
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт карточки аналитического учёта фактических расходов"));//11
MENU.VVOD_SPISOK_add_MD(gettext("Расчёт оборота по КЭКЗ"));//12
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//13

for(;;)
 {
  clear();

  if(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0)
   continue;
  class spis_oth oth;   
  switch (kom)
   {
    case 13:
    case -1 :
      return;

    case 0 :
      if(mord1(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 1 :
      if(mord4(4,&oth) ==0)
       iceb_t_rabfil(&oth,"");
      break;

    case 2 :
      if(mord5(&oth) ==0)
       iceb_t_rabfil(&oth,"");
       
      break;

    case 3 :
      if(mord6(6,&oth) ==0)
       iceb_t_rabfil(&oth,"");
      break;

    case 4 :
      if(mord6(7,&oth) ==0)
       iceb_t_rabfil(&oth,"");
      break;


    case 5:
      if(mord9(0,&oth) ==0)
       iceb_t_rabfil(&oth,"");
      break;



    case 6:
      if(mord9(1,&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;
    case 7:
      if(mord12(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 8:
      if(mord9(2,&oth) ==0)
       iceb_t_rabfil(&oth,"");
      break;

    case 9:
      if(buhkaogo(&oth) ==0)
       iceb_t_rabfil(&oth,"");
      break;

    case 10:
      if(buh_kaukz(&oth) ==0)
       iceb_t_rabfil(&oth,"");
      break;

    case 11:
      if(buh_kaufz(&oth) ==0)
       iceb_t_rabfil(&oth,"");
      break;

    case 12:
      if(buh_kekv(&oth) ==0)
       iceb_t_rabfil(&oth,"");
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
void xbu_saldo()
{
char strsql[512];
short dt=0,mt=0,gt=0;
poltekdat(&dt,&mt,&gt);

if(mt <= 2)
 return;

class iceb_tu_str god("");

iceb_t_poldan("Стартовый год",&god,"nastrb.alx");

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
  iceb_t_soob(gettext("Необходимо перенести сальдо!"));
  return;
 }


}
/****************************************/
/*Удаление за год*/
/******************************************/
void xbu_udgod()
{
int kom=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Инструменты"));

MENU.VVOD_SPISOK_add_MD(gettext("Удаление проводок"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка годов за которые есть проводки"));//1
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
      clprov();
      break;

    case 1:
      poigod(1);
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
void xbu_inst()
{
int kom=0;
class VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Инструменты"));

MENU.VVOD_SPISOK_add_MD(gettext("Работа с файлами распечаток"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Сменить базу"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Сменить принтер"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Дополнительные программы"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Удаление проводок"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Проверка логической целосности базы данных"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Перенос сальдо"));//6
MENU.VVOD_SPISOK_add_MD(gettext("Личные настройки оператора"));//7
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//8

for(;;)
 {
  clear();

  if(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0)
   continue;
  class spis_oth oth;   
  switch (kom)
   {
    case 8:
    case -1 :
      return;

    case 0:

      dirlst("lst,txt,csv,xml,dbf");
      break;


    case 1:
      if(smenabaz(1) == 0)
       {
        xbu_start();
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
      xbu_udgod();
      break;
 
    case 5:
       prpp();
       break;

    case 6:
      saldogk();
      break;

    case 7:
      iceb_t_lnom();
      break;

    default:
      kom=0;
      break;
   }
 }

}
/************************************/
/*старт программы*/
/******************************/
void xbu_start()
{
/*Ищем настройку*/

if(nastsys() != 0)
 exit_iceb(1);
if(buhnast() != 0)
 exit_iceb(1);

if(iceb_t_perzap(1) != 0) /*Проверка на первичный запуск в текущем дне и удаление *.lst */
 exit_iceb(1);

iceb_t_blokps(); /*автоматическая блокировка подсистемы*/
xbu_saldo(); /*проверка сальдо*/
}
/******************************************/
void test()
{

class iceb_tu_str stroka("");
class iceb_fopen kkk;

if(kkk.start("test.txt","r") != 0)
 return;
printw("\n%s start\n",__FUNCTION__);
while(iceb_tu_fgets(&stroka,kkk.ff) == 0)
 printw("%s-%s",__FUNCTION__,stroka.ravno());
printw("%s end\n",__FUNCTION__);
  
OSTANOV();



}
