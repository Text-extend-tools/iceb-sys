/* $Id: dirsaldo.c,v 5.49 2014/08/31 06:18:18 sasa Exp $ */
/*05.03.2018    17.09.1993      Белых А.И.      dirsaldo.c
Ввод и корректировка сальдо по счетам и организациям
*/
#include <math.h>
#include        <sys/stat.h>
#include        <errno.h>
#include        "buhg.h"
class dirsaldo_poi
 {
  public:
    class iceb_tu_str shetpoi;
    class iceb_tu_str kontrpoi;
    class iceb_tu_str godpoi;
    class iceb_tu_str grupapoi;
    class iceb_tu_str naim_kontr;
    int	 metpoi;

  dirsaldo_poi()
   {
    clear();
   }
   
  void clear()
   {
    shetpoi.new_plus("");
    kontrpoi.new_plus("");
    godpoi.new_plus("");
    grupapoi.new_plus("");
    naim_kontr.new_plus("");
    metpoi=0;
   }
 };
 
void dirsaldo_vz(const char *kkk,short god_k,const char *shet_k,const char *kontr_k);
void            shbh4(const char*,double,double,class dirsaldo_poi*);
int	poisaldo(SQL_str,class dirsaldo_poi*);
void dirsaldo_uzs(SQLCURSOR *cur,int kolstr,class dirsaldo_poi*);
void dirsaldo_zg(const char *kkk);
void dirsaldo_rs(class SQLCURSOR *cur,class dirsaldo_poi *rpoi,const char *tz);


int dirsaldo(const char *tz)
{
class iceb_t_mstr spis_menu(0);
class dirsaldo_poi rpoi;
short           y=2,x=0;
class iceb_tu_str sin(""); /*Счёт */
class iceb_tu_str korg(""); /*Код организации */
short           godd=0; /*Год*/
int             K;
int		prc;
short           i;
int             kom=0,kom1=0;
class iceb_tu_str sh1("");
class iceb_tu_str sh2("");
double		deb,kre;
long		kolstr;
long		pozz,pozz1;
SQL_str         row,row1;
char		strsql[2048];
double		dos=0.,kos=0.;
struct OPSHET	shetv;
int		kl=0;
int		mspz=0;
class iceb_tu_str naim("");
short dt,mt,gt;



pozz=kom=K=0;

//Создаем класс меню
VVOD VV(0);
VVOD DANET(1);
VVOD VVOD1(2);
VVOD PAR(2);

naz1:;

short           kls=LINES-5;


sprintf(strsql,"select * from Saldo where kkk='%s' order by gs desc,ns asc",tz);

deb=kre=0.;
SQLCURSOR cur;
SQLCURSOR cur1;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;
  shbh4(tz,deb,kre,&rpoi);
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == FK6 || K == SFK2 || K == ENTER || K == FK3 || K == FK5)
    goto lll;
  if(K == KEY_RESIZE)
   goto naz1;   
  goto vper;
 }
else
 {
  while(cur.read_cursor(&row) != 0)
  {
   if(rpoi.metpoi == 1)
   if(poisaldo(row,&rpoi) != 0)
     continue;
   
   deb+=atof(row[4]);
   kre+=atof(row[5]);
  }

 }
if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;
naz:;



cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
char strmenu[1024];
spis_menu.spis_str.free_class();
for(i=0;i<=kls-1;)
 {
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;

  if(rpoi.metpoi == 1)
   if(poisaldo(row,&rpoi) != 0)
    continue;
  
  memset(strmenu,'\0',sizeof(strmenu));
  if(tz[0] == '0')
    sprintf(strmenu,"%s|%-*s|%10.2f|%10.2f",
    row[1],
    iceb_tu_kolbait(10,row[2]),row[2],
    atof(row[4]),atof(row[5]));
  
  if(tz[0] == '1')
   {
    naim.new_plus("");
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[3]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
      naim.new_plus(row1[0]);

    sprintf(strmenu,"%s|%-*s|%-*s|%10.2f|%10.2f|%s",
    row[1],
    iceb_tu_kolbait(6,row[2]),row[2],
    iceb_tu_kolbait(8,row[3]),row[3],
    atof(row[4]),atof(row[5]),
    naim.ravno());
   }

  spis_menu.spis_str.plus(strmenu);
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


shbh4(tz,deb,kre,&rpoi);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,"",COLS-26);


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
    if(rpoi.metpoi == 1)
     if(poisaldo(row,&rpoi) != 0)
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

    if(rpoi.metpoi == 1)
     if(poisaldo(row,&rpoi) != 0)
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

    if(rpoi.metpoi == 1)
     if(poisaldo(row,&rpoi) != 0)
      continue;

    i++;
   }
  goto naz;
 }

if(kom >= 0)
 {
  if(SRAV(gettext("Конец списка"),spis_menu.spis_str.ravno(kom),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/

  polen(spis_menu.spis_str.ravno(kom),&godd,1,'|');

  polen(spis_menu.spis_str.ravno(kom),&sin,2,'|');
 
  korg.new_plus("");
  if(tz[0] == '0')
   {
    polen(spis_menu.spis_str.ravno(kom),&dos,3,'|');

    polen(spis_menu.spis_str.ravno(kom),&kos,4,'|');
   }

  if(tz[0] == '1')
   {
    
    polen(spis_menu.spis_str.ravno(kom),&korg,3,'|');

    polen(spis_menu.spis_str.ravno(kom),&dos,4,'|');

    polen(spis_menu.spis_str.ravno(kom),&kos,5,'|');
   }
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
    if(tz[0] == '0')
      sprintf(strsql,"%s","buhg4_4.txt");
    if(tz[0] == '1')
      sprintf(strsql,"%s","buhg4_5.txt");

    iceb_t_pdoc(strsql);
    goto naz;

  case SFK2:   /*Корректировать*/
  case ENTER:
    dirsaldo_vz(tz,godd,sin.ravno(),korg.ravno());

    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:
    dirsaldo_vz(tz,0,"","");
    goto naz1;

  case FK3:       /*Удалить запись*/

    if(iceb_t_pbpds(1,godd) != 0)
     goto naz1;
    if(iceb_t_pbsh(1,godd,sin.ravno(),"","") != 0)
     goto naz1;
     
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
     goto naz;
   
    sprintf(strsql,"delete from Saldo where kkk='%s' and gs=%d and ns='%s' and kodkon='%s' and deb=%.2f and kre=%.2f",
    tz,godd,sin.ravno(),korg.ravno(),dos,kos);

    if(sql_zap(&bd,strsql) != 0)
     {
       if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
        {
         iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
        }
       else
        msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
     }      
    goto naz1;

  case SFK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить все записи ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
     goto naz1;
    poltekdat(&dt,&mt,&gt);
    PAR.VVOD_delete();
    PAR.VVOD_SPISOK_add_MD(gettext("Введите пароль"));
    if(parolv(&PAR,dt,mt,gt,0) != 0)
      goto naz1;

    dirsaldo_uzs(&cur,kolstr,&rpoi);

    goto naz1;

  case FK4:       /*Поиск*/
    VV.VVOD_delete();

    VV.VVOD_SPISOK_add_ZAG(gettext("Введите данные для поиска"));

    VV.VVOD_SPISOK_add_MD(  gettext("Счёт................(,,)."));
    VV.VVOD_SPISOK_add_MD(  gettext("Год......................"));
    VV.VVOD_SPISOK_add_data(rpoi.shetpoi.ravno(),128);
    VV.VVOD_SPISOK_add_data(rpoi.godpoi.ravno(),5);
    if(SRAV(tz,"1",0) == 0)
     {
      VV.VVOD_SPISOK_add_MD(gettext("Код контрагета......(,,)."));
      VV.VVOD_SPISOK_add_MD(gettext("Группа контрагента..(,,)."));
      VV.VVOD_SPISOK_add_MD(gettext("Наименование кон-та.(,,)."));
      VV.VVOD_SPISOK_add_data(rpoi.kontrpoi.ravno(),128);
      VV.VVOD_SPISOK_add_data(rpoi.grupapoi.ravno(),128);
      VV.VVOD_SPISOK_add_data(rpoi.grupapoi.ravno(),128);
     }    
    
fk4:;
    rpoi.metpoi=0;

    helstr(LINES-1,0,"F2/+",gettext("поиск"),
//    "F3",gettext("реквизиты"),
    "F4",gettext("очистить"),
    "F10",gettext("выход"),NULL);

    kl=VV.vvod(0,1,1,-1,-1);

    switch(kl)
     {
      case FK10:
      case ESC:
        rpoi.metpoi=0;
        pozz=0;
        goto naz1;
        
      case FK2:
      case PLU:
        GDITE();
        rpoi.shetpoi.new_plus(VV.VVOD_SPISOK_return_data(0));
        rpoi.godpoi.new_plus(VV.VVOD_SPISOK_return_data(1));

        if(SRAV(tz,"1",0) == 0)
         {
          rpoi.kontrpoi.new_plus(VV.VVOD_SPISOK_return_data(2));
          rpoi.grupapoi.new_plus(VV.VVOD_SPISOK_return_data(3));
          rpoi.naim_kontr.new_plus(VV.VVOD_SPISOK_return_data(4));
         }

        rpoi.metpoi=1;
        pozz=0;
        goto naz1;

      case FK4:
        VV.VVOD_clear_data();
        goto fk4;
        
      default :
        goto fk4;
     }
    goto naz1;

  case FK5:       /*Распечатать*/
    dirsaldo_rs(&cur,&rpoi,tz);
    goto naz;

  case FK6:       /*Сменить счет*/

    clear(); /*Очистить экран и закрасить фоновым цветом*/
    printw(gettext("Смена номера счета"));

    helstr(LINES-1,0,"F10",gettext("выход"),NULL);
        
    sh1.new_plus("");
    sh2.new_plus("");
    VVOD1.VVOD_delete();
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите счет, который нужно сменить"));

    if((kom1=vvod1(&sh1,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
     goto naz;
    if(kom1 == ESC)
     goto naz;

    VVOD1.VVOD_delete();
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите счет, на который нужно сменить"));

    if((kom1=vvod1(&sh2,sizeof(sh2),&VVOD1,NULL,stdscr,-1,-1)) == FK10)
     goto naz;
    if(kom1 == ESC)
     goto naz;
     
    if(sh1.getdlinna() <= 1 || sh2.getdlinna() <= 1)
      goto naz;

    if(prsh(sh1.ravno(),&shetv) == 0)
     {
      sprintf(strsql,gettext("Не найден счёт %s в плане счетов !"),sh1.ravno());
      iceb_t_soob(strsql);
      goto naz;
     }

    if(prsh(sh2.ravno(),&shetv) == 0)
     {
      sprintf(strsql,gettext("Не найден счёт %s в плане счетов !"),sh2.ravno());
      iceb_t_soob(strsql);
      goto naz;
     }
    clearstr(LINES-1,0);

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Выполнить корректировку ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
       goto naz;

    GDITE();
   
    sprintf(strsql,"update Saldo set ns='%s' where ns='%s'",
    sh2.ravno(),sh1.ravno());
    if(sql_zap(&bd,strsql) != 0)
     {
       if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
        {
         iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
        }
       else
        msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
     }    
    goto naz1;

  case FK9: //Загрузка сальдо
    dirsaldo_zg(tz);
    goto naz1;

  default:
    goto naz;
    break;
 }
}

/***********************/
/*Шапка*/
/****************/
void shbh4(const char *tz,double deb,double kre,class dirsaldo_poi *rpoi)
{
short		y=2;
short           x=COLS-20;
move(0,0);
clear();
if(tz[0] == '0')
 {
  printw(gettext("Ввод и коректировка сальдо по счетам"));
  mvprintw(1,0,"   1       2          3          4 ");
  mvprintw(y++,x,gettext("Наименование полей:"));
  mvprintw(y++,x,gettext("1-год"));
  mvprintw(y++,x,gettext("2-счет"));
  mvprintw(y++,x,gettext("3-дебет"));
  mvprintw(y++,x,gettext("4-кредит"));
 }
if(tz[0] == '1')
 {
  printw(gettext("Ввод и коректировка сальдо по контрагентам"));
  mvprintw(1,0,"   1      2       3          4         5          6");
  mvprintw(y++,x,gettext("Наименование полей:"));
  mvprintw(y++,x,gettext("1-год"));
  mvprintw(y++,x,gettext("2-счет"));
  mvprintw(y++,x,gettext("3-код контрагента"));
  mvprintw(y++,x,gettext("4-дебет"));
  mvprintw(y++,x,gettext("5-кредит"));
  mvprintw(y++,x,gettext("6-наим.конт-та"));
 }

move(y+=2,x);

printw("%*.*s:%10.2f",
iceb_tu_kolbait(6,gettext("Дебет")),
iceb_tu_kolbait(6,gettext("Дебет")),
gettext("Дебет"),
deb);

move(++y,x);

printw("%*.*s:%10.2f",
iceb_tu_kolbait(6,gettext("Кредит")),
iceb_tu_kolbait(6,gettext("Кредит")),
gettext("Кредит"),
kre);

if(rpoi->metpoi == 1)
 {
  attron(A_BLINK);
  move(++y,x);
  printw("%s:",gettext("Поиск"));
  if(rpoi->shetpoi.getdlinna() > 1)
   {
    move(++y,x);
    printw("%s",rpoi->shetpoi.ravno());
   }
  if(rpoi->kontrpoi.getdlinna() > 1)
   {
    move(++y,x);
    printw("%s",rpoi->kontrpoi.ravno());
   }

  if(rpoi->naim_kontr.getdlinna() > 1)
   {
    move(++y,x);
    printw("%s",rpoi->naim_kontr.ravno());
   }

  if(rpoi->godpoi.getdlinna() > 1)
   {
    move(++y,x);
    printw("%s",rpoi->godpoi.ravno());
   }
  if(rpoi->grupapoi.getdlinna() > 1)
   {
    move(++y,x);
    printw("%s",rpoi->grupapoi.ravno());
   }
  attroff(A_BLINK);
 }
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F6",gettext("смена счета"),
"F9",gettext("импорт"),
"F10",gettext("выход"),NULL);

}
/*************************/
/*Проверка условий поиска*/
/*************************/
int	poisaldo(SQL_str row,class dirsaldo_poi *rpoi)
{
if(proverka(rpoi->shetpoi.ravno(),row[2],1,0) != 0)
 return(1);
if(proverka(rpoi->kontrpoi.ravno(),row[3],0,0) != 0)
 return(1);
if(proverka(rpoi->godpoi.ravno(),row[1],0,0) != 0)
 return(1);

if(rpoi->grupapoi.getdlinna() > 1)
 {
  SQLCURSOR cur;
  SQL_str   row1;
  char      strsql[512];
  sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[3]);
  if(sql_readkey(&bd,strsql,&row1,&cur) == 1)
   if(proverka(rpoi->grupapoi.ravno(),row1[0],0,0) != 0)
     return(1);
 }

if(rpoi->naim_kontr.getdlinna() > 1)
 {
  SQLCURSOR cur;
  SQL_str   row1;
  char      strsql[512];
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[3]);
  if(sql_readkey(&bd,strsql,&row1,&cur) == 1)
   if(proverka(rpoi->naim_kontr.ravno(),row1[0],4,0) != 0)
     return(1);
 }



return(0);

}
/************************************/
/*Удаление записей*/
/**********************/
void dirsaldo_uzs(SQLCURSOR *cur,
int kolstr,
class dirsaldo_poi *rpoi)
{


SQL_str row;
char strsql[1024];
int kolstr1=0;
cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(rpoi->metpoi == 1)
   if(poisaldo(row,rpoi) != 0)
    continue;

  sprintf(strsql,"delete from Saldo where kkk='%s' and gs=%s and ns='%s'",row[0],row[1],row[2]);
  
  sql_zapis(strsql,1,0);
 }

}
/********************************/
/*Загрузка сальдо из файла*/
/**************************/
void dirsaldo_zg(const char *kkk)
{
struct stat work;
static class iceb_tu_str imafim("");
char imafprom[64];
char            strsql[512];

naz:;

if(iceb_t_menu_import(&imafim,gettext("Импорт стартового сальдо по счетам"),"impsaldo.alx","buhg4_4_1.txt") != 0)
  return;

clear();
move(LINES-1,0);
GDITE();

if(stat(imafim.ravno(),&work) != 0)
 {
  sprintf(strsql,"%s !",gettext("Нет такого файла"));
  iceb_t_soob(strsql);
  goto naz;
 }

class iceb_fopen fil;
if(fil.start(imafim.ravno(),"r") != 0)
 goto naz;


class iceb_fopen filprom;

sprintf(imafprom,"prom%d.tmp",getpid());
if(filprom.start(imafprom,"w") != 0)
 goto naz;

int razmer=0;
class iceb_tu_str strok("");
int  i=0;
class iceb_tu_str god("");
class iceb_tu_str shet("");
class iceb_tu_str deb("");
class iceb_tu_str kre("");
class iceb_tu_str kontr("");
class iceb_tu_str naik("");
OPSHET ops;
time_t vrem;
time(&vrem);

int nomer_kontr=1;
double ksd=0.,ksk=0.;
double ideb=0.,ikre=0.;
int metka_sumo=0;
/*проверяем блокировку дат*/
while(iceb_tu_fgets(&strok,fil.ff) == 0)
 {
  if(strok.ravno()[0] == '#')
   continue;

  if(polen(strok.ravno(),&god,1,'|') != 0)
    continue;

  if(iceb_t_pbpds(1,god.ravno_atoi()) != 0)
    return;
 }

rewind(fil.ff);
class iceb_tu_str rek1("");
while(iceb_tu_fgets(&strok,fil.ff) == 0)
 {
  if(strok.ravno()[0] == '#')
   continue;
  i=strlen(strok.ravno());
  razmer+=i;

  printw("%s",strok.ravno());

  strzag(LINES-1,0,work.st_size,razmer);
  
  kontr.new_plus("");
  
  if(polen(strok.ravno(),&rek1,1,'|') != 0)
    continue;

  if(SRAV(rek1.ravno(),"KON",0) == 0) /*запись реквизитов контрагента*/
   {
    iceb_t_zkvsk(strok.ravno(),&nomer_kontr,filprom.ff);
    continue;
   }

  if(polen(strok.ravno(),&god,1,'|') != 0)
   {
    fprintf(filprom.ff,"%s",strok.ravno());
    continue;
   }

  if(SRAV(rek1.ravno(),"sumo",0) == 0) 
   {
    polen(strok.ravno(),&ksd,2,'|');
    polen(strok.ravno(),&ksk,3,'|');
    metka_sumo++;
    continue;
   }

  if(god.ravno_atoi() < 1900)
   {
    sprintf(strsql,"%s !",gettext("Неправильно введён год"));
    iceb_t_soob(strsql);

    fprintf(filprom.ff,"%s",strok.ravno());
    fprintf(filprom.ff,"#%s !\n",gettext("Неправильно введён год"));
    continue;
   }

  polen(strok.ravno(),&shet,2,'|');
  polen(strok.ravno(),&deb,3,'|');
  polen(strok.ravno(),&kre,4,'|');
  polen(strok.ravno(),&kontr,5,'|');
  polen(strok.ravno(),&naik,6,'|');
  
  //проверяем счёт 
  if(prsh1(shet.ravno(),&ops) != 0)
   {
    fprintf(filprom.ff,"%s",strok.ravno());
    fprintf(filprom.ff,"#%s\n",gettext("Не правильно введён счёт !"));
    continue;
   }    

  if(SRAV(kkk,"0",0) == 0 && ops.saldo == 3)
   {
    fprintf(filprom.ff,"%s",strok.ravno());
    fprintf(filprom.ff,"#%s\n",gettext("Счёт с развёрнутым сальдо!"));
    continue;
   }
  if(SRAV(kkk,"1",0) == 0 && ops.saldo == 0)
   {
    fprintf(filprom.ff,"%s",strok.ravno());
    fprintf(filprom.ff,"#%s\n",gettext("Счёт со свёрнутым сальдо!"));
    continue;
   }

  if(ops.saldo == 3) //Счёт с развёрнутым сальдо
   {

    if(naik.getdlinna() <= 1)
     {

      sprintf(strsql,"%s",gettext("Не введено наименование контрагента!"));
      iceb_t_soob(strsql);

      fprintf(filprom.ff,"%s",strok.ravno());
      fprintf(filprom.ff,"#%s\n",gettext("Не введено наименование контрагента!"));
      continue;

     }

    /*Проверяем наименование*/
    if(iceb_t_get_kkfname(naik.ravno(),kontr.ravno(),&kontr,&nomer_kontr) < 0)
       continue;

    //Проверяем есть ли контрагент в списке счёта
    sprintf(strsql,"select kodkon from Skontr where ns='%s' and kodkon='%s'",shet.ravno(),kontr.ravno());
    if(readkey(strsql) < 1)
     {
      //записываем контраген в список счёта
      sprintf(strsql,"insert into Skontr (ns,kodkon,ktoi,vrem) values ('%s','%s',%d,%ld)",shet.ravno(),kontr.ravno(),iceb_t_getuid(),vrem);
      sql_zapis(strsql,1,0);
     }
   }


  //Удаляем старую запись если она есть (нет уникального ключа)
  if(ops.saldo == 3)
   {
    sprintf(strsql,"select kkk from Saldo where kkk='%s' and gs=%d and ns='%s' and kodkon='%s'",
    kkk,god.ravno_atoi(),shet.ravno(),kontr.ravno());

    if(readkey(strsql) > 0)
     {
      /*записи могут повторятся если делается повторная загрузка сальдо без удаления сальдо в базе
        либо в базе нет сальдо а в файле записи повторяются
        этот вывод позволит увидеть какие записи повторяются если это была первая загрузка*/
      fprintf(filprom.ff,"%s",strok.ravno());
      fprintf(filprom.ff,"#%s\n",gettext("Этот контрагент с этим счётом уже был!"));
     }
    sprintf(strsql,"delete from Saldo where kkk='%s' and gs=%d and ns='%s' and kodkon='%s'",
    kkk,god.ravno_atoi(),shet.ravno(),kontr.ravno());
   }
  else
   {
    sprintf(strsql,"select kkk from Saldo where kkk='%s' and gs=%d and ns='%s'",
    kkk,god.ravno_atoi(),shet.ravno());

    if(readkey(strsql) > 0)
     {
      fprintf(filprom.ff,"%s",strok.ravno());
      fprintf(filprom.ff,"#%s\n",gettext("Этот счёт уже был!"));
     }
    sprintf(strsql,"delete from Saldo where kkk='%s' and gs=%d and ns='%s'",
    kkk,god.ravno_atoi(),shet.ravno());
   }

  sql_zapis(strsql,1,0);
     
  //записываем стартовое сальдо
//  sprintf(strsql,"replace into Saldo (kkk,gs,ns,kodkon,deb,kre,ktoi,vrem) values ('%s',%d,'%s','%s',%.2f,%.2f,%d,%ld)",
//  kkk,god.ravno_atoi(),shet.ravno(),kontr.ravno(),deb.ravno_atof(),kre.ravno_atof(),iceb_t_getuid(),vrem);

  sprintf(strsql,"insert into Saldo (kkk,gs,ns,kodkon,deb,kre,ktoi,vrem) values ('%s',%d,'%s','%s',%.2f,%.2f,%d,%ld)",
  kkk,god.ravno_atoi(),shet.ravno(),kontr.ravno(),deb.ravno_atof(),kre.ravno_atof(),iceb_t_getuid(),vrem);
  
  if(sql_zapis(strsql,1,0) == 0)
   {
    ideb+=deb.ravno_atof();
    ikre+=kre.ravno_atof();
   }
  else
   fprintf(filprom.ff,"%s\n",strsql);  

 }

if(metka_sumo != 0)
 {
  if(fabs(ideb-ksd) > 0.009)
   {
    fprintf(filprom.ff,"\n#%s: %.2f != %.2f\n",gettext("Итоговые дебеты не равны"),ideb,ksd);
    printw("\n%s: %.2f != %.2f\n\n",gettext("Итоговые дебеты не равны"),ideb,ksd);
    OSTANOV();
   }

  if(fabs(ikre-ksk) > 0.009)
   {
    fprintf(filprom.ff,"\n#%s: %.2f != %.2f\n\n",gettext("Итоговые кредиты не равны"),ikre,ksk);
    printw("\n%s: %.2f != %.2f\n\n",gettext("Итоговые кредиты не равны"),ikre,ksk);
    OSTANOV();
   }
 }

fil.end();
filprom.end();

unlink(imafim.ravno());

rename(imafprom,imafim.ravno());


}
/*************************/
/*Распечатка*/
/*********************/
void dirsaldo_rs(class SQLCURSOR *cur,class dirsaldo_poi *rpoi,const char *tz)
{
char imaf[64];
char imaf_csv[64];
class iceb_fopen fil;
class iceb_fopen fil_csv;

sprintf(imaf,"saldo%d.lst",getpid());
if(fil.start(imaf,"w") != 0)
 return;

sprintf(imaf_csv,"slcsv%d.lst",getpid());
if(fil_csv.start(imaf_csv,"w") != 0)
 return;

iceb_t_zagolov(gettext("Стартовое сальдо по счетам"),0,0,0,0,0,0,fil.ff);

fprintf(fil.ff,"-----------------------------------------------------------------\n");

char strsql[512];
double  deb=0.,kre=0.;
cur->poz_cursor(0);
SQL_str row,row1;
class SQLCURSOR cur1;
class iceb_tu_str naim("");
while(cur->read_cursor(&row) != 0)
 {
  if(rpoi->metpoi == 1)
   if(poisaldo(row,rpoi) != 0)
    continue;

  if(tz[0] == '1')
   {
    naim.new_plus("");
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[3]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
      naim.new_plus(row1[0]);

    fprintf(fil.ff,"%s %s %-*s %12.2f %12.2f %-*s %s\n",
    row[0],
    row[1],
    iceb_tu_kolbait(6,row[2]),row[2],
    atof(row[4]),atof(row[5]),
    iceb_tu_kolbait(7,row[3]),row[3],
    naim.ravno());


    fprintf(fil_csv.ff,"%s|%s|%.2f|%.2f|%s|%s\n",
    row[1],
    row[2],
    atof(row[4]),atof(row[5]),
    row[3],
    naim.ravno());

   }
  else
   {
    fprintf(fil.ff,"%s %s %-*s %12.2f %12.2f\n",
    row[0],row[1],
    iceb_tu_kolbait(6,row[2]),row[2],
    atof(row[4]),atof(row[5]));

    fprintf(fil_csv.ff,"%s|%s|%.2f|%.2f\n",
    row[1],
    row[2],
    atof(row[4]),atof(row[5]));
   }

  deb+=atof(row[4]);
  kre+=atof(row[5]);

 }
fprintf(fil.ff,"-----------------------------------------------------------------\n");
    
if(tz[0] == '1')
  fprintf(fil.ff,"%*s %12.2f %12.2f\n",iceb_tu_kolbait(15,gettext("Итого:")),gettext("Итого:"),deb,kre);
else    
  fprintf(fil.ff,"%*s %12.2f %12.2f\n",iceb_tu_kolbait(11,gettext("Итого:")),gettext("Итого:"),deb,kre);

fil.end();
fil_csv.end();

class spis_oth oth;

oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Стартовое сальдо по счетам"));
iceb_t_ustpeh(imaf,3);

oth.spis_imaf.plus(imaf_csv);
oth.spis_naim.plus(gettext("Стартовое сальдо в формате csv"));


iceb_t_rabfil(&oth,"");
}
/*******************************/
/*Ввод и корректировка */
/**************************/
void dirsaldo_vz(const char *kkk,short god_k,const char *shet_k,const char *kontr_k)
{
int             par;
static class iceb_tu_str godz(""),shetz("");
struct OPSHET	shetv;
int		ktoi=iceb_t_getuid();
time_t		vrem;
char		strsql[512];
int		i;
class iceb_tu_str kod("");
class iceb_tu_str naim("");
double		deb,kre;
SQL_str row;
class SQLCURSOR cur;

class VVOD VV(0);

VV.VVOD_SPISOK_add_data(5);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(20);


if(god_k == 0)
  VV.data_plus(0,godz.ravno());
else
 {
  sprintf(strsql,"select * from Saldo where kkk='%s' and gs=%d and ns='%s' and kodkon='%s'",
  kkk,god_k,shet_k,kontr_k);
  if(readkey(strsql,&row,&cur) != 1)
   {
    iceb_t_soob(gettext("Не нашли запись для корректировки !"));
    return;
   }

  
  VV.data_plus(0,row[1]);
  VV.data_plus(1,row[2]);
  if(kkk[0] == '0')
   {
    VV.data_plus(2,row[4]);
    VV.data_plus(3,row[5]);

   }
  else
   {
    VV.data_plus(2,row[3]);
    VV.data_plus(3,row[4]);
    VV.data_plus(4,row[5]);

   }
  kzvz(atoi(row[6]),atol(row[7]),&VV);

 }

if(kkk[0] == '0')
 {
  VV.VVOD_SPISOK_add_MD(gettext("Год....."));
  VV.VVOD_SPISOK_add_MD(gettext("Счёт...."));
  VV.VVOD_SPISOK_add_MD(gettext("Дебет..."));
  VV.VVOD_SPISOK_add_MD(gettext("Кредит.."));

 }
if(kkk[0] == '1')
 {
  if(god_k == 0 && VV.VVOD_SPISOK_return_data(1)[0] == '\0')
    VV.data_plus(1,shetz.ravno());
  VV.VVOD_SPISOK_add_MD(gettext("Год.............."));
  VV.VVOD_SPISOK_add_MD(gettext("Счёт............."));
  VV.VVOD_SPISOK_add_MD(gettext("Код контрагента.."));
  VV.VVOD_SPISOK_add_MD(gettext("Дебет............"));
  VV.VVOD_SPISOK_add_MD(gettext("Кредит..........."));
 }

naz:;

if(kkk[0] == '0')
  helstr(LINES-1,0,"F2/+",gettext("записать"),
  "F10",gettext("выход"),NULL);

if(kkk[0] == '1')
  helstr(LINES-1,0,"F2/+",gettext("записать"),
  "F3",gettext("контрагенты"),
  "F10",gettext("выход"),NULL);

par=0;
if(VV.VVOD_SPISOK_return_data(0)[0] != '\0')
  par = 1;
if(VV.VVOD_SPISOK_return_data(1)[0] != '\0')
  par = 2;

par=VV.vvod(par,1,1,-1,-1);

godz.new_plus(VV.VVOD_SPISOK_return_data(0));
shetz.new_plus(VV.VVOD_SPISOK_return_data(1));

int metka_prov=0;

switch(par)
 {
  case FK10:
  case ESC:
    break; 

  case FK2:
  case PLU:
    GDITE();
    if(VV.VVOD_SPISOK_return_data(1)[0] == '\0')
     {
      iceb_t_soob(gettext("Не введён счёт !"));
      goto naz;
     }
    if(VV.VVOD_SPISOK_return_data(0)[0] == '\0')
     {
      iceb_t_soob(gettext("Не введён год !"));
      goto naz;
     }

    if((i=prsh(VV.VVOD_SPISOK_return_data(1),&shetv)) == 0)
     {
      sprintf(strsql,gettext("Нет счета %s в плане счетов !"),VV.VVOD_SPISOK_return_data(1));
      iceb_t_soob(strsql);
      goto naz;
     }

    if(i == 2)
     {
      sprintf(strsql,gettext("Счёт %s имеет субсчета. Введите нужный субсчет."),VV.VVOD_SPISOK_return_data(1));
      iceb_t_soob(strsql);
      goto naz;
     }

    if(kkk[0] == '0' && shetv.saldo == 3)
     {
      iceb_t_soob(gettext("Счёт с развернутым сальдо. Сальдо вводиться по контрагентам !"));
      goto naz;
     }

    if(kkk[0] == '1' && shetv.saldo != 3)
     {
      iceb_t_soob(gettext("Счёт не с развернутым сальдо !"));
      goto naz;
     }

    time(&vrem);

    if(kkk[0] == '1')
     {
      /*Читаем наименование организации*/
      sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",VV.VVOD_SPISOK_return_data(2));
      if(readkey(strsql) != 1)
       {
        sprintf(strsql,"%s %s",gettext("Не найден код контрагента"),VV.VVOD_SPISOK_return_data(2));
        iceb_t_soob(strsql);

        goto naz;
       }

      sprintf(strsql,"select kodkon from Skontr where ns='%s' and \
kodkon='%s'",VV.VVOD_SPISOK_return_data(1),VV.VVOD_SPISOK_return_data(2));

      if(readkey(strsql) < 1)
       {
        sprintf(strsql,"insert into Skontr \
values ('%s','%s',%d,%ld)",VV.VVOD_SPISOK_return_data(1),VV.VVOD_SPISOK_return_data(2),ktoi,vrem);
        /*
          printw("\nstrsql=%s\n",strsql);
          OSTANOV();
        */
          GDITE();
          if(sql_zap(&bd,strsql) != 0)
           {
           if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
            {
             iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
             goto naz;
            }
           else
            {
             msql_error(&bd,gettext("Ошибка ввода записи"),strsql);
             goto naz;
            }
           }
       }
     }
    
    metka_prov=0;
    /*Проверяем может сальдо уже введено*/
    if(kkk[0] == '0')
     {    
      if(god_k != VV.data_ravno_atoi(0) || SRAV(shet_k,VV.data_ravno(1),0) != 0 )
       metka_prov=1;
     } 
    else
      if(god_k != VV.data_ravno_atoi(0) || SRAV(shet_k,VV.data_ravno(1),0) != 0 || SRAV(kontr_k,VV.data_ravno(2),0) != 0 )
       metka_prov=1;    
    
    if(metka_prov == 1)
     {
      if(kkk[0] == '0')
       sprintf(strsql,"select gs from Saldo \
where kkk='%s' and gs='%s' and ns='%s' and kodkon='%s'",
       kkk,VV.VVOD_SPISOK_return_data(0),VV.VVOD_SPISOK_return_data(1),"\0");

      if(kkk[0] == '1')
       sprintf(strsql,"select gs from Saldo \
where kkk='%s' and gs='%s' and ns='%s' and kodkon='%s'",
       kkk,VV.VVOD_SPISOK_return_data(0),VV.VVOD_SPISOK_return_data(1),VV.VVOD_SPISOK_return_data(2));

      if(readkey(strsql) >= 1)
       {
        iceb_t_soob(gettext("Такая запись уже есть !"));
        goto naz;
       }

     }

    /*Проверяем блокировку*/
    if(iceb_t_pbpds(1,VV.data_ravno_atoi(0)) != 0)
     goto naz;

    if(iceb_t_pbsh(1,VV.data_ravno_atoi(0),VV.data_ravno(1),shet_k,"") != 0)
     goto naz;

    if(god_k != 0 && god_k != VV.data_ravno_atoi(0))
     {
      if(iceb_t_pbpds(1,god_k) != 0)
       goto naz;
     }

    
    /*Записываем*/
    if(god_k == 0)
     {

      if(kkk[0] == '0')
       {
        deb=ATOFM(VV.VVOD_SPISOK_return_data(2));
        kre=ATOFM(VV.VVOD_SPISOK_return_data(3));
        sprintf(strsql,"insert into Saldo \
values ('%s',%s,'%s','%s',%.2f,%.2f,%d,%ld)",
        kkk,VV.VVOD_SPISOK_return_data(0),VV.VVOD_SPISOK_return_data(1),"\0",deb,kre,ktoi,vrem);
       }
      if(kkk[0] == '1')
       {
        deb=ATOFM(VV.VVOD_SPISOK_return_data(3));
        kre=ATOFM(VV.VVOD_SPISOK_return_data(4));
        sprintf(strsql,"insert into Saldo \
values ('%s',%s,'%s','%s',%.2f,%.2f,%d,%ld)",
        kkk,VV.VVOD_SPISOK_return_data(0),VV.VVOD_SPISOK_return_data(1),VV.VVOD_SPISOK_return_data(2),deb,kre,ktoi,vrem);
       }
     }

    if(god_k != 0)
     {
      if(kkk[0] == '0')
       {
        deb=ATOFM(VV.VVOD_SPISOK_return_data(2));
        kre=ATOFM(VV.VVOD_SPISOK_return_data(3));
        sprintf(strsql,"update Saldo \
set kkk='%s',gs=%s,ns='%s',kodkon='%s',deb=%.2f,kre=%.2f,ktoi=%d,\
vrem=%ld where kkk='%s' and gs=%d and ns='%s' and kodkon='%s'",
        kkk,VV.VVOD_SPISOK_return_data(0),VV.VVOD_SPISOK_return_data(1),"\0",deb,kre,ktoi,vrem,
        kkk,god_k,shet_k,kontr_k);
       }

      if(kkk[0] == '1')
       {
        deb=ATOFM(VV.VVOD_SPISOK_return_data(3));
        kre=ATOFM(VV.VVOD_SPISOK_return_data(4));
        sprintf(strsql,"update Saldo \
set kkk='%s',gs=%s,ns='%s',kodkon='%s',deb=%.2f,kre=%.2f,ktoi=%d, \
vrem=%ld where kkk='%s' and gs=%d and ns='%s' and kodkon='%s'",
        kkk,VV.VVOD_SPISOK_return_data(0),VV.VVOD_SPISOK_return_data(1),VV.VVOD_SPISOK_return_data(2),deb,kre,ktoi,vrem,
        kkk,god_k,shet_k,kontr_k);
       }
     }
     
    /*
      printw("\nstrsql=%s\n",strsql);
      OSTANOV();
    */
      GDITE();
      if(sql_zap(&bd,strsql) != 0)
       {
        if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
         {
          iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
          goto naz;
         }
        else
         {
          msql_error(&bd,gettext("Ошибка ввода записи"),strsql);
          goto naz;
         }
       }
     break;

  case FK3:  /*Просмотр списка контрагентов*/
    if(kkk[0] == '0')
     goto naz;  

    kod.new_plus("");
    naim.new_plus("");
    if(dirkontr(&kod,&naim,1) == 0)
     {
      VV.data_plus(2,kod.ravno());
     }  
    goto naz;

  default:
   goto naz;

 }  

}
