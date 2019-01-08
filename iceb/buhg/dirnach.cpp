/*$Id: diruder.c,v 5.9 2014/08/31 06:18:18 sasa Exp $*/
/*20.03.2018	07.08.2014	Белых А.И.	dirnach.c
Работа со справочником начислений
*/
#include <errno.h>
#include "buhg.h"

void dirnach_sap(int metkar,const char *poisk);
int dirnach_vz(int kod);
void dirnach_kk(int skod,int nkod);
int dirnach_pvu(int kod);
void dirnach_ras(class SQLCURSOR *cur,const char *poisk);


int dirnach(int metkar, //0-работа 1-выбор из списка с корректировкой 2-чистый выбор
class iceb_tu_str *kod_nach,class iceb_tu_str *naim_kod)
{
class iceb_t_mstr spis_menu(0);
char strsql[2048];
int  Y=3;
int X=0;
int kolstr=0;
int K=0;
int pozz=0,pozz1=0;
int mspz=0;
SQL_str row;
int prc;
int kom=0;
int i=0;
class iceb_tu_str yesno("");
class SQLCURSOR cur;
class iceb_tu_str poisk("");
class iceb_tu_str vidn(""); /*0-основная,1-дополнительная 2-прочая*/
class iceb_tu_str fondz(""); /*0-входит в фонд зарплаты 1-не входит*/
class iceb_tu_str koment("");
class iceb_tu_str naim_vib("");
int kod_nachv=0;
//Создаем класс меню
VVOD DANET(1);
VVOD VVOD1(2);
if(metkar == 2)
 {
  Y=-1;
  X=-1;
  koment.new_plus(gettext("Выберите нужное и нажмите Enter"));
 }

naz1:;


short           kls=LINES-6;

sprintf(strsql,"select * from Nash order by kod asc");
  

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;

   
  dirnach_sap(metkar,poisk.ravno());

  K=iceb_t_soob(gettext("Не найдено ни одной записи !"));


  if(K == SFK2 || K == ENTER || K == FK3 || K == FK5 || K == FK6)
    goto lll;

  if(K == KEY_RESIZE)
   goto naz1;   

  goto vper;
 }


if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;

naz:;

cur.poz_cursor(pozz);
mspz=2;
pozz1=0;

spis_menu.spis_str.free_class();
for(i=0;i<=kls-1;)
 {
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;

  if(proverka(poisk.ravno(),row[1],4,0) != 0)
   continue;
  
  vidn.new_plus(gettext("Основная"));
  if(atoi(row[4]) == 1)
    vidn.new_plus(gettext("Дополнительная"));
  if(atoi(row[4]) == 2)
    vidn.new_plus(gettext("Прочая"));

   
  if(atoi(row[7]) == 0)
   yesno.new_plus(gettext("Да"));
  else
   yesno.new_plus(gettext("Нет"));

  fondz.new_plus(gettext("Фонд зарплаты"));
  if(atoi(row[8]) == 1)
    fondz.new_plus("");

  if(metkar == 2)
   sprintf(strsql,"%3s|%s",row[0],row[1]);
  else    
   sprintf(strsql,"%3s|%-*s|%-3s|%-*.*s|%-*.*s|%-*s|%s",
   row[0],
   iceb_tu_kolbait(6,row[2]),
   row[2],
   row[3],
   iceb_tu_kolbait(10,vidn.ravno()),  
   iceb_tu_kolbait(10,vidn.ravno()),  
   vidn.ravno(),
   iceb_tu_kolbait(10,fondz.ravno()),
   iceb_tu_kolbait(10,fondz.ravno()),
   fondz.ravno(),
   iceb_tu_kolbait(3,yesno.ravno()),
   yesno.ravno(),
   row[1]);
      
  spis_menu.spis_str.plus(strsql);
  i++;
  
  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
   mspz=1;

 }


if(i == 0)
 {
  if(kolstr != 0 && pozz > 0)
   {
    pozz--;
    goto naz;
   }
  goto lll;
 }  
if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));


dirnach_sap(metkar,poisk.ravno());

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,Y,X,NULL,strsql,prc,&K,koment.ravno(),COLS-26);


if(kom == -7) //Изменение границы экрана
  goto naz1;

if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  for(i=0; i< mspz   ; )
   {
    pozz++;
    if(pozz >= kolstr)
     {
      pozz=kolstr-1;
      break;
     }
    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;
    if(proverka(poisk.ravno(),row[1],4,0) != 0)
     continue;
    i++;
   }
  kom=kls-1;
  goto naz;
 }
if(kom == -2) /*Стрелка вверх*/
 {
  for(i=0; i < 1 ; )
   {

    pozz--;

    if(pozz <= 0)
     {
      beep();
      pozz=0;
      goto naz;
     }

    cur.poz_cursor(pozz);
    cur.read_cursor(&row);
    if(proverka(poisk.ravno(),row[1],4,0) != 0)
     continue;
    i++;
   }

  goto naz;
 }
 
if(kom == -5) /*Страница вниз*/
 {
  if(pozz + kls == kolstr)
   {
    pozz=kolstr-1;
    beep();
    goto naz;  
   }
  if(i == kls)
    pozz+=pozz1;
  else
    beep();
  if(pozz >= kolstr)
   {
    pozz=kolstr-pozz1;
    beep();
    goto naz;  
   }
  goto naz;

 }

if(kom == -4) /*Страница вверх*/
 {
  for(i=0; i<kls ; )
   {
    pozz--;

    if(pozz <= 0)
     {
      beep();
      pozz=0;
      goto naz;
     }

    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;
    if(proverka(poisk.ravno(),row[1],4,0) != 0)
     continue;
    i++;
   }
  goto naz;
 }

if(kom >= 0)
 {
  class iceb_tu_str str_vib(spis_menu.spis_str.ravno(kom));
  if(SRAV(gettext("Конец списка"),str_vib.ravno(),0) == 0)
     goto naz;

  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/

  polen(str_vib.ravno(),&kod_nachv,1,'|');
  if(metkar == 2)
   polen(str_vib.ravno(),&naim_vib,2,'|');
  else
   polen(str_vib.ravno(),&naim_vib,7,'|');
 }

vper:;
switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return(1);
     break;

  case FK1:  /*Получить подсказку*/
    GDITE();
    iceb_t_pdoc("zarp3_1_3.txt");
    goto naz;

  case FK2:  
    dirnach_vz(0);
    goto naz1;

  case ENTER: 
    if(metkar != 0)
     {
      kod_nach->new_plus(kod_nachv);
      naim_kod->new_plus(naim_vib.ravno());
      return(0);
     }

  case SFK2: 
    
    dirnach_vz(kod_nachv);
    goto naz1;

  case FK3:       /*Удалить запись*/
    if(dirnach_pvu(kod_nachv) != 0)
     goto naz;

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      goto naz;

    sprintf(strsql,"delete from Nash where kod=%d",kod_nachv);
    sql_zapis(strsql,1,0);
    goto naz1;

  case FK4:       /*Отобрать по образцу*/

     helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
     
     poisk.new_plus("");
     
     VVOD1.VVOD_delete();
     VVOD1.VVOD_SPISOK_add_MD(gettext("Введите образец для поиска"));

     if(vvod1(&poisk,40,&VVOD1,NULL,stdscr,-1,-1) == FK10)
       poisk.new_plus("");

     GDITE();

     goto naz1;

  case FK5:       /*Распечатать*/
   dirnach_ras(&cur,poisk.ravno());
    goto naz;

  default:
    goto naz;
    break;
 }

return(1);

}
/*********/
/*Шапка*/
/*********/
void dirnach_sap(int metkar,const char *poisk)
{
short		y=2;
short           x=COLS-20;


if(metkar != 2)
 {
  clear();
  printw(gettext("Список начислений"));
 }
if(metkar == 1)
 {
  move(1,0);
  printw(gettext("Найдите нужное и нажмите \"Enter\""));
 }

if(metkar < 2)
 {



  mvprintw(2,0,"   1     2    3      4          5       6   7");
  mvprintw(y++,x,gettext("Наименование полей:"));
  mvprintw(y++,x,"1-%s",gettext("код начислений"));
  mvprintw(y++,x,"2-%s",gettext("счёт"));
  mvprintw(y++,x,"3-%s",gettext("код табеля"));
  mvprintw(y++,x,"4-%s",gettext("вид зарплаты"));
  mvprintw(y++,x,"5-%s",gettext("фонд зарплаты"));
  mvprintw(y++,x,"6-%s",gettext("проводки"));
  mvprintw(y++,x,"7-%s",gettext("наименование"));

  y+=2;

  if(poisk[0] != '\0')
   {
    mvprintw(y++,x,"%s:",gettext("Поиск"));
    mvprintw(y++,x,"%s",poisk);
   }

  helstr(LINES-1,0,
  //"F1",gettext("помощь"),
  "F2/+",gettext("запись"),
  "F3",gettext("удалить"),
  "F5",gettext("печать"),
  "F10",gettext("выход"),NULL);
 }
 
if(metkar == 2)
  helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
  "F10",gettext("выход"),NULL);
 

}
/***********************/
/*Ввод и корректировка записи*/
/*****************************/

int dirnach_vz(int kod_nachv)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
class iceb_tu_str kodn("");
class iceb_tu_str naim("");
class iceb_tu_str shet("");
class iceb_tu_str vidn(gettext("Основная"));
int metka_vidn=0;
class iceb_tu_str fondz(gettext("Входит в фонд зарплаты"));
int metka_fondz=0;
int metka_prov=0;
class iceb_tu_str kod_tab("");
class iceb_tu_str yesno(gettext("Да"));
int N=0,K=0;
int poz=0,komv=0;

VVOD VV(0);

if(kod_nachv != 0) /*Корректировка записи*/
 {
  sprintf(strsql,"select * from Nash where kod=%d",kod_nachv);
  if(readkey(strsql,&row,&cur) != 1)
   {
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
    return(1);
   }
  kodn.new_plus(kod_nachv);
  naim.new_plus(row[1]);
  shet.new_plus(row[2]);
  kod_tab.new_plus(row[3]);
  
  metka_vidn=atoi(row[4]);
  if(metka_vidn == 1)
   vidn.new_plus(gettext("Дополнительная"));
  if(metka_vidn == 2)
   vidn.new_plus(gettext("Прочая"));

  metka_fondz=atoi(row[8]);
  if(metka_fondz == 1)
   fondz.new_plus(gettext("Не входит в фонд зарплаты"));

  metka_prov=atoi(row[7]);   
  if(metka_prov == 0)
    yesno.new_plus(gettext("Да"));
  else
    yesno.new_plus(gettext("Нет"));
  
  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));
  kzvz(row[3],row[4],&VV);
 }
else
 {
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));
  int nom=0;
  for(nom=1; ;nom++)
   {
    sprintf(strsql,"select kod from Nash where kod=%d",nom);
    if(readkey(strsql) == 0)
     break;  
   }
  kodn.new_plus(nom);
 }


VV.VVOD_SPISOK_add_MD(gettext("Код начисления.."));//0
VV.VVOD_SPISOK_add_MD(gettext("Наименование...."));//1
VV.VVOD_SPISOK_add_MD(gettext("Счёт............"));//2
VV.VVOD_SPISOK_add_MD(gettext("Проводки........"));//3
VV.VVOD_SPISOK_add_MD(gettext("Вид начисления.."));//4
VV.VVOD_SPISOK_add_MD(gettext("Фонд зарплаты..."));//5
VV.VVOD_SPISOK_add_MD(gettext("Код табеля......"));//6

VV.VVOD_SPISOK_add_data(kodn.ravno(),16);
VV.VVOD_SPISOK_add_data(naim.ravno(),60);
VV.VVOD_SPISOK_add_data(shet.ravno(),32);
VV.VVOD_SPISOK_add_data(yesno.ravno(),32);
VV.VVOD_SPISOK_add_data(vidn.ravno(),128);
VV.VVOD_SPISOK_add_data(fondz.ravno(),128);
VV.VVOD_SPISOK_add_data(kod_tab.ravno(),16);



naz:;

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);



helstr(LINES-1,0,"F2/+",gettext("запись"),
"F10",gettext("выход"),NULL);


VV.vramka(-1,-1,0);


for(;;)
 {
  attron(VV.VVOD_return_cs(iceb_CFS)); 

  VV.vprintw(N,stdscr);

  K=getch();

  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

  if(K == KEY_RESIZE)
    goto naz;

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));

    VV.vprintw_clear(N,stdscr);

    if(N == 3) /*проводки*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      attron(VV.VVOD_return_cs(iceb_CB));

      VVOD MENUW(3);

      sprintf(strsql,"%s (%s)",gettext("Да"),gettext("Проводки должны выполняться"));
      MENUW.VVOD_SPISOK_add_MD(strsql);

      sprintf(strsql,"%s (%s)",gettext("Нет"),gettext("Проводки недолжны выполняться"));
      MENUW.VVOD_SPISOK_add_MD(strsql);

      MENUW.VVOD_SPISOK_add_ZAG(gettext("Должны ли выполняться проводки для этого удержания ?"));
      if(menu3w(stdscr,&MENUW,&metka_prov,-1,-1,0) != 0)
       goto naz;

      switch (metka_prov)
       {
        case -1 :
         break;

        case 0 :
         VV.VVOD_SPISOK_zapis_data(3,gettext("Да"));
         break;

        case 1 :
         VV.VVOD_SPISOK_zapis_data(3,gettext("Нет"));
         break;
       }
      goto naz;
            
     }

    if(N == 4) /*вид начисления*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      attron(VV.VVOD_return_cs(iceb_CB));

      VVOD MENUW(3);

      MENUW.VVOD_SPISOK_add_MD(gettext("Основная"));
      MENUW.VVOD_SPISOK_add_MD(gettext("Дополнительная"));
      MENUW.VVOD_SPISOK_add_MD(gettext("Прочая"));

      if(menu3w(stdscr,&MENUW,&metka_vidn,-1,-1,0) != 0)
       goto naz;

      switch (metka_vidn)
       {
        case -1 :
         break;

        case 0 :
         VV.VVOD_SPISOK_zapis_data(4,gettext("Основная"));
         break;

        case 1 :
         VV.VVOD_SPISOK_zapis_data(4,gettext("Дополнительная"));
         break;

        case 2 :
         VV.VVOD_SPISOK_zapis_data(4,gettext("Прочая"));
         break;
       }

      
      goto naz;
            
     }

    if(N == 5) /*фонд зарплаты*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      attron(VV.VVOD_return_cs(iceb_CB));

      VVOD MENUW(3);

      MENUW.VVOD_SPISOK_add_MD(gettext("Входит в фонд зарплаты"));
      MENUW.VVOD_SPISOK_add_MD(gettext("Не входит в фонд зарплаты"));

      if(menu3w(stdscr,&MENUW,&metka_fondz,-1,-1,0) != 0)
       goto naz;

      switch (metka_fondz)
       {
        case -1 :
         break;

        case 0 :
         VV.VVOD_SPISOK_zapis_data(5,gettext("Входит в фонд зарплаты"));
         break;

        case 1 :
         VV.VVOD_SPISOK_zapis_data(5,gettext("Не входит в фонд зарплаты"));
         break;

       }

      
      goto naz;
            
     }

    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));




    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));
    VV.vprintw(N,stdscr);
    N++;
   }
  if(K==SDN) /*Стрелка вниз*/
   {
    N++;
   }
  if(K==SUP) /*Стрелка вверх*/
   {
    N--;
   }
  if(N > VV.KLST-1)
   {
    N=0;
   }
  if(N < 0)
   {
    N=VV.KLST-1;
   }

  if(K == ESC || K == FK10)
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));


    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CB));
    return(1);
   }

  if(K == FK2 || K == PLU) /*Записываем*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    break;
   }

  if(K == FK3)
   {
    VVOD MENUW(3);

    MENUW.VVOD_SPISOK_add_MD(gettext("Просмотр плана счетов"));
    MENUW.VVOD_SPISOK_add_MD(gettext("Просмотр списка табелей"));
    MENUW.VVOD_SPISOK_add_MD(gettext("Выход"));

    if(menu3w(stdscr,&MENUW,&metka_fondz,-1,-1,0) != 0)
     goto naz;

    switch (metka_fondz)
     {
      case -1 :
       break;

      case 0 :
       if(dirshet(&kodn,&naim,1) == 0)
        {
         VV.data_plus(2,kodn.ravno());
         clear();
        }
       goto naz;
       
      case 1 :
       kodn.new_plus("");
       naim.new_plus("");
       poz=komv=0;
       if(dirmat1("Tabel",&kodn,&naim,LINES-5,-1,-1,2,&poz,&komv,"","\0") == 0)
         VV.data_plus(6,kodn.ravno());
       goto naz;

      case 2:
       goto naz;
     }

   }

  if(K == FK4)
   {
    VV.VVOD_clear_data();
    goto naz;
   }
 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
refresh();

if(VV.data_ravno(0)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введён код!"));
  goto naz;
 }
if(VV.data_ravno(1)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введёно наименование!"));
  goto naz;
 }

if(VV.data_ravno(2)[0] != '\0')
 {
  struct OPSHET opshet;
  if(prsh1(VV.data_ravno(2),&opshet) != 0)
   goto naz;
 }

if(VV.data_ravno_atoi(6) != 0) /*проверяем есть ли такой код табеля*/
 {
  sprintf(strsql,"select kod from Tabel where kod=%d",VV.data_ravno_atoi(6));
  if(readkey(strsql) != 1)
   {
    iceb_t_soob(gettext("Не найден код табеля в справочнике табелей!"));
    goto naz;
   
   }
 }

if(kod_nachv != VV.data_ravno_atoi(0))
 {
  sprintf(strsql,"select kod from Nash where kod=%d",VV.data_ravno_atoi(0));
  if(readkey(strsql) == 1)
   {
    iceb_t_soob(gettext("Такой код удержания уже есть!"));
    goto naz;
   }

 }


int metka_kor_kod=0;

if(kod_nachv == 0) /*Ввод новой записи*/
 {
  sprintf(strsql,"insert into Nash values (%d,'%s','%s',%d,%d,%d,%ld,%d,%d)",
  VV.data_ravno_atoi(0),
  VV.data_ravno_filtr(1),
  VV.data_ravno_filtr(2),
  VV.data_ravno_atoi(6),
  metka_vidn,
  iceb_t_getuid(),time(NULL),
  metka_prov,
  metka_fondz);
  
 }
else /*Корректировка существующей*/
 {
  if(kod_nachv != VV.data_ravno_atoi(0))
   {
    VVOD DANET(1);
    DANET.VVOD_SPISOK_add_MD(gettext("Корректировать код? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      goto naz;
    metka_kor_kod=1;   
   }  

  sprintf(strsql,"update Nash set \
kod=%d,\
naik='%s',\
shet='%s',\
ktoz=%d,\
vrem=%ld,\
prov=%d,\
kodt=%d,\
vidn=%d,\
fz=%d \
where kod=%d",
  VV.data_ravno_atoi(0),
  VV.data_ravno_filtr(1),
  VV.data_ravno_filtr(2),
  iceb_t_getuid(),time(NULL),
  metka_prov,
  VV.data_ravno_atoi(6),
  metka_vidn,
  metka_fondz,
  kod_nachv);
 }

if(sql_zapis(strsql,1,0) != 0)
 goto naz;

if(metka_kor_kod == 1)
  dirnach_kk(kod_nachv,VV.data_ravno_atoi(0));

return(0);

}
/******************************************/
/*Корректировка кода удержания в таблицах*/
/*******************************************/
void dirnach_kk(int skod,int nkod)
{
char strsql[512];
VVOD SOOBN(1);
SOOBN.VVOD_SPISOK_add_MD(gettext("Выполняется каскадная корректировка кода в таблицах !"));
soobsh(&SOOBN,-1,-1,1);

sprintf(strsql,"update Zarp set knah=%d where prn='1' and knah=%d",nkod,skod);
if(sql_zapis(strsql,1,0) != 0)
 return;

sprintf(strsql,"update Zarn1 set knah=%d where prn='1' and knah=%d",nkod,skod);
sql_zapis(strsql,1,0);

sprintf(strsql,"update Zarsdo set kn=%d where kn=%d",nkod,skod);
sql_zapis(strsql,1,0);


}
/*********************************/
/*Проверка возможности удаления*/
/********************************/
int dirnach_pvu(int kod)
{
char strsql[512];
sprintf(strsql,"select knah from Zarp where prn='1' and knah=%d limit 1",kod);

if(readkey(strsql) > 0)
 {
  sprintf(strsql,"%s Zarp !",gettext("Удалить нельзя, потому, что есть записи в таблице"));
  iceb_t_soob(strsql);
  return(1);
 }

sprintf(strsql,"select knah from Zarn1 where prn='1' and knah=%d limit 1",kod);

if(readkey(strsql) > 0)
 {
  sprintf(strsql,"%s Zarn1 !",gettext("Удалить нельзя, потому, что есть записи в таблице"));
  iceb_t_soob(strsql);
  return(1);
 }

sprintf(strsql,"select kn from Zarsdo where kn=%d limit 1",kod);

if(readkey(strsql) > 0)
 {
  sprintf(strsql,"%s Zarsdo !",gettext("Удалить нельзя, потому, что есть записи в таблице"));
  iceb_t_soob(strsql);
  return(1);
 }

return(0);
}
/********************************************/
/*Распечатка */
/***********************************/
void dirnach_ras(class SQLCURSOR *cur,const char *poisk)
{
SQL_str row;
FILE *ff;
char imaf[64];
class iceb_tu_str danet("");

sprintf(imaf,"nach%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
  {
   error_op_nfil(imaf,errno,"");
   return;
  }

iceb_t_zagolov(gettext("Список начислений"),ff);

if(poisk[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Поиск"),poisk);
fprintf(ff,"\
--------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Код |   Наименование удержания     | Счёт |Код таб.|Вид начиления|Фонд зарплаты| Проводки |\n"));
//    123456789012345678901234567890 123456 12345678 1234567890123 1234567890123 
fprintf(ff,"\
--------------------------------------------------------------------------------------------\n");
class iceb_tu_str vidn("");
class iceb_tu_str fondz("");

cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {
  if(proverka(poisk,row[1],4,0) != 0)
   continue;

  vidn.new_plus(gettext("Основная"));
  if(atoi(row[4]) == 1)
    vidn.new_plus(gettext("Дополнительная"));
  if(atoi(row[4]) == 2)
    vidn.new_plus(gettext("Прочая"));

  if(atoi(row[7]) == 0)
   danet.new_plus("Да");
  else
   danet.new_plus("Нет");

  fondz.new_plus(gettext("Входит"));
  if(atoi(row[8]) == 1)
    fondz.new_plus("");
  
  fprintf(ff,"%5s|%-*.*s|%-*s|%-8s|%-*.*s|%-*.*s|%s\n",
  row[0],
  iceb_tu_kolbait(30,row[1]),
  iceb_tu_kolbait(30,row[1]),
  row[1],
  iceb_tu_kolbait(6,row[2]),
  row[2],
  row[3],
  iceb_tu_kolbait(13,vidn.ravno()),
  iceb_tu_kolbait(13,vidn.ravno()),
  vidn.ravno(),
  iceb_tu_kolbait(13,fondz.ravno()),
  iceb_tu_kolbait(13,fondz.ravno()),
  fondz.ravno(),
  danet.ravno());  
  
 }
fprintf(ff,"\
--------------------------------------------------------------------------------------------\n");
podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Список начислений"));
iceb_t_ustpeh(oth.spis_imaf.ravno(0),3);
iceb_t_rabfil(&oth,"");
 

}
