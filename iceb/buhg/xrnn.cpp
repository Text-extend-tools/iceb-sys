/*$Id: xrnn.c,v 5.124 2014/08/31 06:18:18 sasa Exp $*/
/*23.05.2018	28.08.2005	Белых А.И.	xrnn.c
Программа работы с реестром налоговых накладных
*/
#define DVERSIQ "06.12.2018"
#include        "buhg.h"

void dirrnn(short,short,short);
void dirrnn1(short,short,short);
int  ree_nast(void);
int rasrnn(class spis_oth*);
int rnn_rkontr(class spis_oth*);
void xrnn_udzap();
void xrnn_nsi();
void xrnn_rsd();
void xrnn_ro();
void xrnn_sg();
void xrnn_start();
void xrnn_inst();
void rnngods();

extern char	*parol_pol;
extern char	*host;
extern char	*imabaz;
SQL_baza	bd;
extern short dp,mp,gp; //Дата начала просмотра реестров
extern int iceb_kod_podsystem;

int             main(int argc,char **argv,char **envp)
{
iceb_kod_podsystem=ICEB_PS_RNN;
int             kom;
class iceb_tu_str kod("");
class iceb_tu_str naim("");

icebstart(argc,argv);
xrnn_start();

VVOD MENU(4);
VVOD VVOD1(2);



int uid=0;
class iceb_tu_str fioop;

uid=iceb_t_getuid();
fioop.plus(iceb_t_getfioop());


kom = 0;

for(;;)
 {
  clear();

  MENU.VVOD_delete();

  MENU.VVOD_SPISOK_add_ZAG(gettext("Программа ведения реестра налоговых накладных"));
  MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
   
  MENU.VVOD_SPISOK_add_MD(gettext("Работа с реестрами"));//0
  MENU.VVOD_SPISOK_add_MD(gettext("Распечатка отчётов"));//1
  MENU.VVOD_SPISOK_add_MD(gettext("Ввод нормативно-справочной информации"));//2
  MENU.VVOD_SPISOK_add_MD(gettext("Инструменты"));//3
  MENU.VVOD_SPISOK_add_MD(gettext("Конец работы"));//4

  znak(0,COLS-26);
  infosys(12,COLS-27,host,VERSION,DVERSIQ,0,0,0,imabaz,uid,fioop.ravno());


  if(menu3(&MENU,&kom,1,0,0) != 0)
   continue;
  switch (kom)
   {

    case 4 :
    case -1 :
      exit_iceb(0);
      break;

    case 0 :
      xrnn_rsd();
      break;

    case 1 :
      xrnn_ro();
      break;

    case 2 :
      xrnn_nsi();
      break;

    case 3:
      xrnn_inst();
      break;

    default:
       kom=0;
       break;   
   }
 }
exit_iceb(0);
}
/****************************/
/*Удалить записи за период*/
/************************/
void xrnn_udzap()
{
char strsql[512];
static class iceb_tu_str dat1(""),dat2("");
int kom=0;
SQL_str row;
class SQLCURSOR cur;



/*проверяем полномочия оператора*/
sprintf(strsql,"select nnb,nnn from icebuser where login='%s'",iceb_tu_getlogin());
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %s %s icebuser",gettext("Не найден логин"),iceb_tu_getlogin(),gettext("в таблице"));
  iceb_t_soob(strsql);
  return;
 }
if(atoi(row[0]) != 0 || atoi(row[1]) != 0)
 {
  iceb_t_soob(gettext("У вас нет полномочий для этой операции!"));
  return;
 }











VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Введите период за который нужно удалить записи"));

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.(д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..(д.м.г).."));

VV.VVOD_SPISOK_add_data(dat1.ravno(),11);
VV.VVOD_SPISOK_add_data(dat2.ravno(),11); 

naz:;

clear();
 
helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("удалить"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

move(LINES-1,0);

dat1.new_plus(VV.VVOD_SPISOK_return_data(0));
dat2.new_plus(VV.VVOD_SPISOK_return_data(1));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   prosf(strsql);
   goto naz;

  case ESC:
  case FK10:
   return;

  case FK2:
  case PLU:
     break;



  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

short dn,mn,gn;
short dk,mk,gk;

if(rsdatp(&dn,&mn,&gn,dat1.ravno(),&dk,&mk,&gk,dat2.ravno()) != 0)
 goto naz;

short d=dn;
short m=mn;
short g=gn;


while(sravmydat(d,m,g,dk,mk,gk) <= 0)
 {
  if(iceb_t_pbpds(m,g) != 0)
   goto naz;
  dpm(&d,&m,&g,3);
 }

poltekdat(&d,&m,&g);

VVOD PAR(2);
PAR.VVOD_SPISOK_add_MD(gettext("Введите пароль"));
if(parolv(&PAR,d,m,g,0) != 0)
 goto naz;

clear();

sprintf(strsql,"delete from Reenn where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d'",
gn,mn,dn,gk,mk,dk);

printw("%s\n",strsql);
refresh();

sql_zapis(strsql,0,0);

int kolud=sql_rows(&bd);
printw("%s: %d\n",gettext("Количество удаленных записей"),kolud);

sprintf(strsql,"delete from Reenn1 where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d'",
gn,mn,dn,gk,mk,dk);

printw("%s\n",strsql);
refresh();

sql_zapis(strsql,0,0);

int kolud1=sql_rows(&bd);

int	metka_ver=0;
strcpy(strsql,"select VERSION()");

readkey(strsql,&row,&cur);
if(SRAV(row[0],"3.22",1) == 0)
  metka_ver=1;  

class iceb_tu_str imatab("");

printw("VERSION:%s\n",row[0]);
for(int i=0; i < 2 ;i++)
 { 
  if(i == 0)
    imatab.new_plus("Reenn");
  if(i == 1)
    imatab.new_plus("Reenn1");

  printw(gettext("Оптимизация таблицы %s !\n"),imatab.ravno());
  GDITE();
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table %s",imatab.ravno());

  if(metka_ver == 1)
   {
    sql_zapis(strsql,1,0);
   }
  else
   {
    readkey(strsql);
   }

 }

printw("%s: %d\n",gettext("Количество удаленных записей"),kolud1);

sprintf(strsql,"%s: %d",gettext("Количество удаленных записей"),kolud+kolud1);
iceb_t_soob(strsql);




}
/**************************/
/*ввод и корректировка НСИ*/
/******************************/

void xrnn_nsi()
{
int   poz=0,komv=0;
int kom=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Ввод и корректировка нормативно-справочной информации"));

MENU.VVOD_SPISOK_add_MD(gettext("Ввод и кор-вка списка групп полученных док-тов"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Ввод и кор-вка списка групп выданных док-тов"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Настройка определения вида документа для полученных накладных"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Настройка определения вида документа для выданных накладных"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Работа с файлом настройки"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//5

for(;;)
 {
  clear();

  if(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0)
   continue;
  class iceb_tu_str kod("");
  class iceb_tu_str naim("");
  switch (kom)
   {
    case 5:
    case -1 :
      return;

    case 0 :
      clear();
      kod.new_plus("");
      naim.new_plus("");
      poz=komv=0;
      dirmat1("Reegrup1",&kod,&naim,LINES-5,0,2,0,&poz,&komv,"","\0");
      break;

    case 1 :
      clear();
      kod.new_plus("");
      naim.new_plus("");
      poz=komv=0;
      dirmat1("Reegrup2",&kod,&naim,LINES-5,0,2,0,&poz,&komv,"","\0");
      break;

    case 2:
      iceb_redfil("rnnovdp.alx",0);
      break;

    case 3:
      iceb_redfil("rnnovdr.alx",0);
      break;

    case 4:
      iceb_redfil("rnn_nast.alx",0);
      ree_nast();
      break;

   }
 }

}
/**********************/
/*работа с реестрами*/
/************************/
void xrnn_rsd()
{
char bros[512];
class iceb_tu_str vstr("");
int kom=0,kom1=0;
short dt,mt,gt;

poltekdat(&dt,&mt,&gt);

if(dt <= 20) /*реестр сдаётся до 20-числа*/
  dpm(&dt,&mt,&gt,4); /*уменьшить на месяц*/

VVOD VVOD1(2);
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Работа с реестрaми"));

MENU.VVOD_SPISOK_add_MD(gettext("Работа с реестром полученных налогових накладных"));//0

sprintf(bros,"%s %02d.%04d %s",gettext("Работа с реестром полученных налоговых накладных за"),mt,gt,gettext("г."));
MENU.VVOD_SPISOK_add_MD(bros); //1

MENU.VVOD_SPISOK_add_MD(gettext("Работа с реестром выданных налоговых накладных"));//2

sprintf(bros,"%s %02d.%04d %s",gettext("Работа с реестром выданных налоговых накладных за"),mt,gt,gettext("г."));
MENU.VVOD_SPISOK_add_MD(bros); //3

MENU.VVOD_SPISOK_add_MD(gettext("Установливать дату начала просмотра реестров"));//4
MENU.VVOD_SPISOK_add_MD(gettext("Установливать/снимать блокировку дат"));//5
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//6

for(;;)
 {
  clear();

  if(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0)
   continue;
  switch (kom)
   {
    case 6:
    case -1 :
      return;

    case 0 :
      dirrnn1(dp,mp,gp);//полученные 
      break;

    case 1 :
      dirrnn1(1,mt,gt);//полученные 
      break;

    case 2 :
      dirrnn(dp,mp,gp);//выданные
      break;

    case 3 :
      dirrnn(1,mt,gt);//выданные
      break;

    case 4 :
      memset(bros,'\0',sizeof(bros));
      if(dp != 0)
       sprintf(bros,"%d.%d.%d",dp,mp,gp);
      vstr.new_plus(bros); 
      VVOD1.VVOD_delete();
      VVOD1.VVOD_SPISOK_add_ZAG(gettext("Введите дату начала просмотра реестров (д.м.г)"));
datapr:;
      if((kom1=vvod1(&vstr,11,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
        break;
      if(kom1 == ESC)
        break;
      if(rsdat(&dp,&mp,&gp,vstr.ravno(),1) != 0)
       {
        iceb_t_soob(gettext("Неправильно введена дата !"));
        goto datapr;
       }
      break; 

    case 5:
      iceb_tl_blok();
      break;

   }
 }
}
/************************/
/*Распечатка отчётов*/
/************************/
void xrnn_ro()
{
int kom=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Распечатка отчётов"));

MENU.VVOD_SPISOK_add_MD(gettext("Распечатать реестр налоговых накладных"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать записи по контрагенту"));//1
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
      if(rasrnn(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

    case 1 :
      if(rnn_rkontr(&oth) == 0)
       iceb_t_rabfil(&oth,"");
      break;

   }
 }

}
/****************************/
/*Изменение стартового года*/
/*****************************/
void xrnn_sg()
{
char strsql[512];
short dt=0,mt=0,gt=0;
poltekdat(&dt,&mt,&gt);

if(mt <= 2)
 return;

class iceb_tu_str datanp("");

iceb_t_poldan("Дата начала просмотра реестра",&datanp,"rnn_nast.alx");

short d,m,g;
rsdat(&d,&m,&g,datanp.ravno(),1);

if(gt == g)
 return;

if(sravmydat(d,m,g,dt,mt,gt) > 0)
 {
  sprintf(strsql,"%s! %02d.%02d.%04d > %02d.%02d.%04d",gettext("Дата начала просмотра больше текущей даты"),d,m,g,dt,mt,gt);
  iceb_t_soob(strsql);
  return;
 }
extern short dp,mp,gp; //Дата начала просмотра реестров
dp=1; mp=1; gp=gt;
sprintf(strsql,"update Alx set str='Дата начала просмотра реестра|%02d.%02d.%04d\n',ktoz=%d,vrem=%ld where fil='rnn_nast.alx' and str like 'Дата начала просмотра реестра|%%'",1,1,gt,ICEB_KOD_OP_PROG,time(NULL));
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
/******************************/
/*Старт программы*/
/************************/
void xrnn_start()
{
/*Ищем настройку*/

if(nastsys() != 0)
 exit_iceb(1);

if(iceb_t_perzap(1) != 0) /*Проверка на первичный запуск в текущем дне и удаление *.lst */
 exit_iceb(1);

if(ree_nast() != 0)
 exit_iceb(1);

iceb_t_blokps(); /*автоматическая блокировка подсистемы*/
xrnn_sg(); /*Установка даты начала просмотра реестра*/
}
/****************************************/
/*Удаление за год*/
/******************************************/
void xrnn_udgod()
{
int kom=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Инструменты"));

MENU.VVOD_SPISOK_add_MD(gettext("Удаление записей за период"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка годов за которые есть записи в реестрах"));//1
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
      xrnn_udzap();
      break;

    case 1:
      rnngods();
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
void xrnn_inst()
{
int kom=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Инструменты"));

MENU.VVOD_SPISOK_add_MD(gettext("Работа с файлами распечаток"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Сменить базу"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Сменить принтер"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Дополнительные программы"));//3
MENU.VVOD_SPISOK_add_MD(gettext("Удалить записи за период"));//4
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
        xrnn_start();
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
       xrnn_udgod();
       break;

    default:
      kom=0;
      break;
   }
 }

}
