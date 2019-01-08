/*$Id: dirkontr.c,v 5.81 2014/07/31 07:09:27 sasa Exp $*/
/*27.04.2017    17.01.1993              Белых А.И.      dirkontr.c
Просмотр ввод и корректировка кодов организаций
Если вернули 0 -значит выбрали
             1 - нет
*/
#include        <errno.h>
#include        <sys/stat.h>
#include	"buhl.h"

class dirkontr_poi
 {
  public:
    class iceb_tu_str naim_kont; /*Наименование контрагента*/
    class iceb_tu_str gorod; /*Город организации*/
    class iceb_tu_str gorodb; /*Город банка*/
    class iceb_tu_str kod;   /*Код*/
    class iceb_tu_str naimb; /*Наименование банка*/
    class iceb_tu_str mfo;   /*МФО*/
    class iceb_tu_str nsh;   /*Номер счета*/
    class iceb_tu_str inn;   /*Индивидуальный налоговый номер*/
    class iceb_tu_str kodgrkont; /*Код группы контрагента*/
    int metka_nal; /*0-все 1-общая система налогообложения 2-единый налог*/
  dirkontr_poi()
   {
    clear();
   }
  void clear()
   {
    gorod.new_plus("");
    gorodb.new_plus("");
    kod.new_plus("");
    naimb.new_plus("");
    mfo.new_plus("");
    nsh.new_plus("");
    inn.new_plus("");
    kodgrkont.new_plus("");
    metka_nal=0;
   }
 };


void		udnkk(SQLCURSOR*,long);
void            shbh5n(int);
int  		sravrek(SQL_str,class dirkontr_poi*);
void dirkontr_rs(class SQLCURSOR *cur,class dirkontr_poi *rpoi,short metpoi,int kolstr);
int dirkontr_mpoi(class dirkontr_poi *rpoi);
void dirkontr_f9(int *metkasort);
int dirkontr_uk(const char *kod_kontr);


int dirkontr(class iceb_tu_str *kodo,
class iceb_tu_str *nai,int mk) //0-ввод 1-выбор
{
class spis_oth oth;
class dirkontr_poi rpoi;
short           x=0,y=2;
int             K=0;
int             kom=0;
int             i=0,prc=0;
class iceb_tu_str kodor("");
class iceb_tu_str kodvk(""); /*Код только что введенного контрагента*/
long		kolstr;
long		pozz; /*Позиция первой строчки меню*/
long            pozz1; /*Количество строк прочитанных для заполнения меню*/
SQL_str         row;
char		strsql[1024];
short		mspz=0; /*метка соответствия первой записи*/
int		Y,X;
short		metpoi=0;
int		maxkz=0; //Максимальное количество знаков в коде контрагента с лидирующимим нолями
class iceb_tu_str naikont("");
int metkasort=0; //0-по кодам 1-по наименованиям
class iceb_t_mstr spis_menu(0);
int kod_start=1;
VVOD DANET(1);
VVOD  VV(0);
class VVOD  VVOD1(2);



rpoi.naim_kont.new_plus(nai->ravno());
metpoi=0;
if(nai != NULL)
  if(nai->getdlinna() > 1)
   metpoi=1;
pozz=pozz1=0;
kom=0;

//memset(bros,'\0',sizeof(bros));
iceb_t_poldan("Максимальное количество знаков в коде контрагента с лидирующими нолями",&maxkz,"nastrb.alx");
//maxkz=atoi(bros);

naz1:;
GDITE();
clear();
short           kls=LINES-5;
K=0;

if(metkasort == 0)
 sprintf(strsql,"select * from Kontragent order by kodkon asc");

if(metkasort == 1)
 sprintf(strsql,"select * from Kontragent order by naikon asc");

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,"",strsql);
  return(1);
 }
if(kolstr == 0)
 {

lll:;

  shbh5n(kolstr);
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == FK1 || K == FK2 || K == PLU || K == FK10 || K == ESC || K == FK9)
    goto vper;

  if(K == KEY_RESIZE)
    goto naz1;
    
  goto lll;
  
 }

if(rpoi.naim_kont.ravno()[0] == '\0' && kodvk.getdlinna() > 1 && kolstr > kls && pozz < kolstr-kls)
 {
  pozz=poivcur(kodvk.ravno(),0,&cur);
  kom=0;
  kodvk.new_plus("");
 }

naz:;
GDITE();
clear();

if(kolstr != 0 && pozz >= kolstr)
  pozz = kolstr-1;

cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
spis_menu.spis_str.free_class();
for(i=0; i<kls ; )
 {
  
  if((cur.read_cursor(&row)) == 0)
    break;

  pozz1++;

  if(metpoi == 1)
   if(sravrek(row,&rpoi) != 0)
    continue;

  sprintf(strsql,"%-*s|%s",iceb_tu_kolbait(8,row[0]),row[0],row[1]);

  i++;
  spis_menu.spis_str.plus(strsql);

  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
   mspz=1;

 }

if(i == 0)
 {
  if(metpoi == 1)  
   {
    clear();
    iceb_t_soob(gettext("Нет записей по заданым реквизитам поиска !"));

    rpoi.clear();

    metpoi=0;
   }
  pozz=0;
  goto naz;
 }

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;


if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));


shbh5n(kolstr);

if(metpoi == 1)
 {
  attron(A_BLINK);
  move(Y=5,X=COLS-25);
  printw(gettext("Поиск !"));
  if(rpoi.naim_kont.ravno()[0] != '\0')
   {
    move(++Y,X);
    printw(gettext("Наи.-%s"),rpoi.naim_kont.ravno());
   }
  if(rpoi.nsh.ravno()[0] != '\0')
   {
    move(++Y,X);
    printw(gettext("Сче.-%s"),rpoi.nsh.ravno());
   }
  if(rpoi.mfo.ravno()[0] != '\0')
   {
    move(++Y,X);
    printw(gettext("МФО -%s"),rpoi.mfo.ravno());
   }
  if(rpoi.gorod.ravno()[0] != '\0')
   {
    move(++Y,X);
    printw(gettext("Гор.-%s"),rpoi.gorod.ravno());
   }
  if(rpoi.kod.ravno()[0] != '\0')
   {
    move(++Y,X);
    printw(gettext("Код -%s"),rpoi.kod.ravno());
   }
  if(rpoi.naimb.ravno()[0] != '\0')
   {
    move(++Y,X);
    printw(gettext("Н.б.-%s"),rpoi.naimb.ravno());
   }
  if(rpoi.inn.ravno()[0] != '\0')
   {
    move(++Y,X);
    printw(gettext("ИНН -%s"),rpoi.inn.ravno());
   }
  if(rpoi.gorodb.ravno()[0] != '\0')
   {
    move(++Y,X);
    printw(gettext("Г.б.-%s"),rpoi.gorodb.ravno());
   }
  if(rpoi.kodgrkont.ravno()[0] != '\0')
   {
    move(++Y,X);
    printw(gettext("К.г.-%s"),rpoi.kodgrkont.ravno());
   }

  if(rpoi.metka_nal == 1)
   {
    move(++Y,X);
    printw(gettext("Общая система н.о."));    
   }
   if(rpoi.metka_nal == 2)
   {
    move(++Y,X);
    printw(gettext("Единый налог"));    
   }

  attroff(A_BLINK);
 }
prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,"",0);


if(kom == -7) //Изменить размер экрана
  goto naz1;

if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }

  for(i=0; i< mspz  ; )
   {
    pozz++;
    if(pozz >= kolstr)
     {
      pozz=kolstr-1;
      break;
     }
    cur.poz_cursor(pozz);
    if((cur.read_cursor(&row)) == 0)
      break;

    if(metpoi == 1)
     if(sravrek(row,&rpoi) != 0)
      continue;
    i++;
   }
  kom=kls;
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

    if(metpoi == 1)
     if(sravrek(row,&rpoi) != 0)
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
    cur.read_cursor(&row);

    if(metpoi == 1)
     if(sravrek(row,&rpoi) != 0)
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
     
  polen(spis_menu.spis_str.ravno(kom),strsql,sizeof(strsql),1,'|');
  i=poivcur(strsql,0,&cur);
  cur.poz_cursor(i);
  cur.read_cursor(&row);
  kodor.new_plus(row[0]);
  naikont.new_plus(row[1]);
 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return(1);


  case FK1:   /*Помощь*/
   GDITE();
//   prosf(iceb_t_imafdoc("buhg4_2.txt"));
   iceb_t_pdoc("buhg4_2.txt");
   clear();
   if(kolstr > 0)
     goto naz;
   else
     goto lll;

  case ENTER : //Выход или корректировка
    if(mk == 1)
     {
      kodo->new_plus(row[0]);
      if(nai != NULL)
        nai->new_plus(row[1]);
      
      return(0);
     } 

  case SFK2:   /*Корректировать*/

    vkontr("",&kodor,1);
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:

    GDITE();

    kodvk.new_plus("");
    if(maxkz == 0)
     {
      kodvk.new_plus(iceb_t_getnkontr(kod_start));
      kod_start=kodvk.ravno_atoi();
     }
    else
     {
      sprintf(strsql,"%0*d",maxkz,iceb_t_getnkontr(1));
      kodvk.new_plus(strsql);
     }

    if(vkontr("",&kodvk,0) != 0)
     kodvk.new_plus("");
    rpoi.naim_kont.new_plus(""); 
   
    goto naz1;

  case FK3:       /*Удалить запись*/

    if(SRAV(row[0],"00",0) == 0)
     {
      iceb_t_soob(gettext("Код контрагента 00 удалять нельзя!"));
      goto naz;
     }
    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      if(provud("Kontragent",row[0],1) != 0)
        goto naz;

      dirkontr_uk(row[0]);


      clear();
      goto naz1;
     }
    goto naz;
  
  case SFK3:/*Удаление организаций по предварительно сформированному списку*/
    udnkk(&cur,kolstr);
    goto naz1;

  case FK4:       /*Отобрать по образцу*/
    pozz=0;
    if((metpoi=dirkontr_mpoi(&rpoi)) == 1)
     kom=pozz=0;
    goto naz1;

  case FK5:       /*Распечатать */
    dirkontr_rs(&cur,&rpoi,metpoi,kolstr);
    clear();
    cur.poz_cursor(pozz);
    goto naz;

  case FK6:       /*Стать на номер */

    kom=0;
    rpoi.naim_kont.new_plus("");
    helstr(LINES-1,0,"F10",gettext("выход"),NULL);

    VVOD1.VVOD_delete();
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код контрагента"));

    kodor.new_plus("");
    if(vvod1(&kodor,16,&VVOD1,NULL,stdscr,-1,-1) == FK10)
      goto naz;

    kom=pozz=0;
    pozz=poivcur(kodor.ravno(),0,&cur);
    if(pozz == kolstr)
     {
      sprintf(strsql,"%s %s !",gettext("Не найдено код контрагента"),kodor.ravno());
      iceb_t_soob(strsql);
      pozz=0;
     }
    goto naz;
    
  case FK7:       /*Сальдо по организации */
    oth.clear();
    if(salorok(kodor.ravno(),naikont.ravno(),&oth) == 0)
     iceb_t_rabfil(&oth,"");
     
    goto naz;

  case FK8:       //Сменить код контрагента
    if(SRAV("00",kodor.ravno(),0) == 0)
     {
      iceb_t_soob(gettext("Код контрагента 00 нельзя заменять!"));
      goto naz;
     }
    DANET.VVOD_delete();
    sprintf(strsql,gettext("Заменить код контрагента %s ? Вы уверены ?"),kodor.ravno());
    DANET.VVOD_SPISOK_add_MD(strsql);

    if(danet(&DANET,2,stdscr) == 1)
     {
     
      helstr(LINES-1,0,"F10",gettext("выход"),NULL);

      class iceb_tu_str kodnov("");

      VVOD1.VVOD_delete();
      VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код контрагента которым надо заменить"));

      if(vvod1(&kodnov,10,&VVOD1,NULL,stdscr,-1,-1) == FK10)
        goto naz;
      sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",
      kodnov.ravno());
      if(sql_readkey(&bd,strsql) != 1)
       {
        sprintf(strsql,"%s %s !",gettext("Не найдено код контрагента"),kodnov.ravno());
        iceb_t_soob(strsql);
        goto naz;
       }

      VVOD PAROL(2);
      PAROL.VVOD_SPISOK_add_MD(gettext("Введите пароль"));
      strsql[0]='\0';
      vparol(&PAROL,strsql,10,stdscr,-1,-1,1);

      short 	dt,mt,gt;
      poltekdat(&dt,&mt,&gt);
      if(atoi(strsql) != gt+mt+dt)
       {
        iceb_t_soob(gettext("Не верно введён пароль !"));
        goto naz;
       }

      smenkodk(kodor.ravno(),kodnov.ravno()); //vkontr.c
      if(provud("Kontragent",kodor.ravno(),1) != 0)
        goto naz;

      sprintf(strsql,"delete from Skontr where kodkon='%s'",kodor.ravno());

      if(sql_zap(&bd,strsql) != 0)
       {
       if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
        {
         iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
         goto naz;
        }
       else
        msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
       }
      sprintf(strsql,"delete from Kontragent where kodkon='%s'",kodor.ravno());
//      printw("\nstrsql=%s\n",strsql);
//      OSTANOV();
      if(sql_zap(&bd,strsql) != 0)
        msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
      goto naz1;
     }
    goto naz;

  case FK9:       //Сменить сортировку
    dirkontr_f9(&metkasort);
    goto naz1;
    
     
  default:
    if(kolstr > 0)
      goto naz;
    else
      goto lll;
 }

}

/******************/
void shbh5n(int kolstr)
{
clear();
move(0,0);

printw("%s %s:%d",
gettext("Ввод и коректировка контрагентов. (Общий список.)"),
gettext("Количество"),kolstr);

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F6",gettext("найти код"),
"F7",gettext("сальдо"),
"F8",gettext("см.код"),
"F9",gettext("меню"),
"F10",gettext("выход"),NULL);
}

/********************************
Сравнение реквизитов
Если вернули 0 все равно
             !=0 чтото не соответствует
***********************************/   

int sravrek(SQL_str row,class dirkontr_poi *rpoi)
{


if(rpoi->naim_kont.ravno()[0] != '\0')
 if(strstrm(row[1],rpoi->naim_kont.ravno()) == 0)
  if(strstrm(row[16],rpoi->naim_kont.ravno()) == 0)
   return(1);

if(rpoi->naimb.ravno()[0] != '\0')
 if(strstrm(row[2],rpoi->naimb.ravno()) == 0)
  return(2);

if(rpoi->gorod.ravno()[0] != '\0')
 if(strstrm(row[3],rpoi->gorod.ravno()) == 0)
  return(3);

if(rpoi->gorodb.ravno()[0] != '\0')
 if(strstrm(row[4],rpoi->gorodb.ravno()) == 0)
  return(4);

if(rpoi->kod.ravno()[0] != '\0')
 if(strstrm(row[5],rpoi->kod.ravno()) == 0)
  return(5);

if(rpoi->mfo.ravno()[0] != '\0')
 if(strstrm(row[6],rpoi->mfo.ravno()) == 0)
  return(6);

if(rpoi->nsh.ravno()[0] != '\0')
 if(strstrm(row[7],rpoi->nsh.ravno()) == 0)
  return(7);

if(rpoi->inn.ravno()[0] != '\0')
 if(strstrm(row[8],rpoi->inn.ravno()) == 0)
   return(8);

if(rpoi->kodgrkont.ravno()[0] != '\0')
 if(SRAV(row[11],rpoi->kodgrkont.ravno(),0) != 0)
   return(9);
if(rpoi->metka_nal > 0)
 {
  if(rpoi->metka_nal == 1 && atoi(row[18]) != 0)
   return(10);
  if(rpoi->metka_nal == 2 && atoi(row[18]) != 1)
   return(10);
   
 }
return(0);

}
/**********************************************/
/*Удаление не используемых кодов контрагентов*/
/**********************************************/

void 	udnkk(SQLCURSOR *cur,long kolstr)
{
int		koludk;
SQL_str         row;
SQLCURSOR       cur1;
long		kolstr1;
char		strsql[512];
char		imatab[64];

VVOD DANET(1);
sprintf(strsql,gettext("Удалить не используемые контрагеты ? Вы уверены ?"));
DANET.VVOD_delete();
DANET.VVOD_SPISOK_add_MD(strsql);

if(danet(&DANET,2,stdscr) == 2)
  return;

GDITE();

clear();


cur->poz_cursor(0);
koludk=kolstr1=0;
while(cur->read_cursor(&row) != 0)
 {
  kolstr1++;
  strzag(LINES-1,0,kolstr,kolstr1);
  printw("%-*s %-*.*s",
  iceb_tu_kolbait(6,row[0]),
  row[0],
  iceb_tu_kolbait(40,row[1]),
  iceb_tu_kolbait(40,row[1]),
  row[1]);

  if(SRAV("00",row[0],0) == 0)
   continue;

  if(provud("Kontragent",row[0],0) != 0)
   {
    printw(" %s\n",gettext("используется"));
    continue;
   }
  koludk++;
  printw(" %s\n",gettext("удален"));

  dirkontr_uk(row[0]);

 }
GDITE();

printw("%s: %d\n",gettext("Количество удаленных контрагентов"),koludk);


int		metkazap=0;
strcpy(strsql,"select VERSION()");
sql_readkey(&bd,strsql,&row,&cur1);
if(SRAV(row[0],"3.22",1) == 0)
  metkazap=1;  
printw("VERSION:%s\n",row[0]);
refresh();

strcpy(imatab,"Kontragent");
printw(gettext("Оптимизация таблицы %s !\n"),imatab);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table %s",imatab);

if(metkazap == 1)
 {
  if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка оптимизации таблицы !"),strsql);
 }
else
  sql_readkey(&bd,strsql,&row,&cur1);

printw(gettext("Оптимизацию таблицы закончено.\n"));

strcpy(imatab,"Skontr");
printw(gettext("Оптимизация таблицы %s !\n"),imatab);
GDITE();
memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table %s",imatab);

if(metkazap == 1)
 {
  if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка оптимизации таблицы !"),strsql);
 }
else
  sql_readkey(&bd,strsql,&row,&cur1);

printw(gettext("Оптимизацию таблицы закончено.\n"));

OSTANOV();

}

/*************************************/
/*Импорт списка контрагентов из файла*/
/*************************************/

void	dirkontrzag()
{
struct stat work;
char		strsql[4096];
static char	imafprom[64];
static iceb_tu_str imafim("");
int		kom=0;
int		razmer=0;
class iceb_tu_str strok("");
int		i;
int		metkar=0;

VVOD VVOD1(2);
clear();
VVOD1.VVOD_SPISOK_add_ZAG(gettext("Импорт списка контрагентов из файла"));
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите имя файла"));

naz:;

clear();

helstr(LINES-1,0,
"F1",gettext("помощь"),
"F2/+",gettext("загруз."),
"F3",gettext("просмотр"),
"F10",gettext("выход"),NULL);

if((kom=vvod1(&imafim,COLS-6,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
  return;
if(kom == ESC)
  return;

switch(kom)
 {
  case FK1:
    GDITE();
    iceb_t_pdoc("buhg4_2_1.txt");
    clear();
    goto naz;

  case ENTER:
  case FK2:
  case PLU:
    if(imafim.getdlinna() <= 1)
     {
      dirkatl(&imafim);
      goto naz;
     }
    break;

  case FK3:
   GDITE();
   if(stat(imafim.ravno(),&work) != 0)
    {
     sprintf(strsql,"%s !",gettext("Нет такого файла"));
     iceb_t_soob(strsql);
     goto naz;
    }
    vizred(imafim.ravno());
    goto naz;

  default:
    goto naz;
 }


clear();
move(LINES-1,0);
GDITE();

if(stat(imafim.ravno(),&work) != 0)
 {
  sprintf(strsql,"%s !",gettext("Нет такого файла"));
  iceb_t_soob(strsql);
  goto naz;
 }

class iceb_fopen filin;
if(filin.start(imafim.ravno(),"r") != 0)
 return;

class iceb_fopen filtmp;

sprintf(imafprom,"prom%d.tmp",getpid());
if(filtmp.start(imafprom,"w") != 0)
 return;

int	ktoi=iceb_t_getuid();
time_t  vrem;
time(&vrem);
#define KOLKOL 17
class iceb_tu_str rek[KOLKOL];
int kolihoh=0;
int nom=0;
SQL_str row;
class SQLCURSOR cur;
razmer=0;
int nomer_kontr=1;
while(iceb_tu_fgets(&strok,filin.ff) == 0)
 {
  if(strok.ravno()[0] == '#')
   continue;
   
  i=strlen(strok.ravno());
  razmer+=i;
  if(strok.ravno()[i-2] == '\r') //Возврат каретки меняем на пробел
   iceb_tu_vstav(&strok," ",i-2,i-1,1);
//   strok[i-2]=' ';

  printw("%s",strok.ravno());
  strzag(LINES-1,0,work.st_size,razmer);




  if(metkar == 1)
   {
    fprintf(filtmp.ff,"%s",strok.ravno());
    continue;
   }
  
  for(nom=0; nom < KOLKOL; nom++)
   if(polen(strok.ravno(),&rek[nom],nom+1,'|') != 0)
    {
     fprintf(filtmp.ff,"%s",strok.ravno());
     fprintf(filtmp.ff,"#%s %d\n",gettext("Не найдено поле"),nom+1);
     kolihoh++;
     break;     
    }

  if(nom < KOLKOL)
   continue;


  /*проверяем может такой контрагент уже есть*/
  sprintf(strsql,"select kodkon from Kontragent where naikon='%s'",rek[1].ravno_filtr());
  if(readkey(strsql,&row,&cur) > 0)
   {
    fprintf(filtmp.ff,"%s",strok.ravno());
    fprintf(filtmp.ff,"#%s! %s %s!\n",gettext("Контрагент с таким коротким наименованием уже есть"),gettext("Код"),row[0]);
    kolihoh++;
    continue;

   }


  if(SRAV(rek[0].ravno(),"авто",0) == 0 || rek[0].ravno()[0] == '\0')
   {
    rek[0].new_plus(iceb_t_getnkontr(nomer_kontr));
    nomer_kontr=rek[0].ravno_atoi();
   }
  else
   {
    /*Проверяем может код контрагента уже занят - проверка наименования сделана выше*/
    sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",rek[0].ravno_filtr());
    if(readkey(strsql) > 0)
     {
      rek[0].new_plus(iceb_t_getnkontr(nomer_kontr));
      nomer_kontr=rek[0].ravno_atoi();

     }     
   }  

  sprintf(strsql,"insert into Kontragent (kodkon,naikon,naiban,adres,adresb,kod,mfo,nomsh,innn,nspnds,telef,grup,ktoz,vrem,na,regnom,pnaim,gk,en) \
values ('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s',\
%d,%ld,'%s','%s','%s','%s',%d)",
  rek[0].ravno_filtr(),
  rek[1].ravno_filtr(),
  rek[2].ravno_filtr(),
  rek[3].ravno_filtr(),
  rek[4].ravno_filtr(),
  rek[5].ravno_filtr(),
  rek[6].ravno_filtr(),
  rek[7].ravno_filtr(),
  rek[8].ravno_filtr(),
  rek[9].ravno_filtr(),
  rek[10].ravno_filtr(),
  rek[11].ravno_filtr(),
  ktoi,vrem,
  rek[12].ravno_filtr(),
  rek[13].ravno_filtr(),
  rek[14].ravno_filtr(),
  rek[15].ravno_filtr(),
  rek[16].ravno_atoi());

  if(sql_zap(&bd,strsql) != 0)
   {
    i=sql_nerror(&bd);
    if(i == ER_DUP_ENTRY) //Запись уже есть
     {
      printw("%s\n",gettext("Такая запись уже есть !"));
      OSTANOV();
      fprintf(filtmp.ff,"%s",strok.ravno());
      kolihoh++;
      continue;
     }

    if(i == ER_DBACCESS_DENIED_ERROR) //Толко чтение
     {
      iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
      fprintf(filtmp.ff,"%s",strok.ravno());
      metkar=1;
      continue;
     }
    fprintf(filtmp.ff,"%s",strok.ravno());
    msql_error(&bd,gettext("Ошибка записи !"),strsql);
    kolihoh++;
   }
 }
filin.end();
filtmp.end();
unlink(imafim.ravno());

rename(imafprom,imafim.ravno());
 
if(kolihoh != 0)
 {
  sprintf(strsql,"%s %d",gettext("Количество не загруженых записей"),kolihoh);
  iceb_t_soob(strsql);
 }


}
/*******************************************/
void dirkontr_create_field(DBASE_FIELD *f,const char *fn,char  ft,int l1,int l2,
int *header_len,int *rec_len)
{

strncpy(f->name, fn, 11);
f->type = ft;
f->length = l1;
f->dec_point = l2;
*header_len=*header_len+sizeof(DBASE_FIELD);
*rec_len=*rec_len+l1;
}
/***********************************************/
void dirkontr_creatheaddbf(char imaf[],long kolz)
{
time_t		tmm;
struct  tm      *bf;
FILE		*ff;
#define		kolpol 14 //Количество колонок (полей) в dbf файле
DBASE_FIELD f[kolpol];
DBASE_HEAD  h;
int fd;
int i;
int header_len, rec_len;

if((ff = fopen(imaf,"w")) == NULL)
   {
    error_op_nfil(imaf,errno,"");
    return;
   }

memset(&h,'\0',sizeof(h));
memset(&f, '\0', sizeof(f));

h.version = 3;

time(&tmm);
bf=localtime(&tmm);

h.l_update[0] = bf->tm_year;       /* yymmdd for last update*/
h.l_update[1] = bf->tm_mon+1;       /* yymmdd for last update*/
h.l_update[2] = bf->tm_mday;       /* yymmdd for last update*/

h.count = kolz;              /* number of records in file*/

header_len = sizeof(h);
rec_len = 0;
int shetshik=0;
dirkontr_create_field(&f[shetshik++],"CODE", 'N', 20, 5,&header_len,&rec_len);
dirkontr_create_field(&f[shetshik++],"NUM", 'C', 254, 0,&header_len,&rec_len);
dirkontr_create_field(&f[shetshik++],"NAME", 'C', 254, 0,&header_len,&rec_len);
dirkontr_create_field(&f[shetshik++],"BANK", 'N', 20, 0,&header_len,&rec_len);
dirkontr_create_field(&f[shetshik++],"ACCNUM", 'C', 254, 0,&header_len,&rec_len);
dirkontr_create_field(&f[shetshik++],"MFO", 'C', 254, 0,&header_len,&rec_len);
dirkontr_create_field(&f[shetshik++],"INDTAXNUM", 'C', 254, 0, &header_len,&rec_len);
dirkontr_create_field(&f[shetshik++],"CODENDS", 'C', 254, 0, &header_len,&rec_len);
dirkontr_create_field(&f[shetshik++],"ADR", 'C', 254, 2, &header_len,&rec_len);
dirkontr_create_field(&f[shetshik++],"SHORTNAME", 'C', 254, 0, &header_len,&rec_len);
dirkontr_create_field(&f[shetshik++],"NREZ", 'N', 20, 5, &header_len,&rec_len);
dirkontr_create_field(&f[shetshik++],"DRAGMET", 'C', 254, 0, &header_len,&rec_len);
dirkontr_create_field(&f[shetshik++],"CALCCONTRO", 'N', 20, 5, &header_len,&rec_len);
dirkontr_create_field(&f[shetshik++],"DEPT", 'C', 254, 0, &header_len,&rec_len);

h.header = header_len + 1;/* length of the header
                           * includes the \r at end
                           */
h.lrecl= rec_len + 1;     /* length of a record
                           * includes the delete
                           * byte
                          */
/*
 printw("h.header=%d h.lrecl=%d\n",h.header,h.lrecl);
*/


fd = fileno(ff);

if(write(fd, &h, sizeof(h)) == -1)
  printw("%s-%s\n",__FUNCTION__,strerror(errno));
for(i=0; i < kolpol; i++) 
 {
  if(write(fd, &f[i], sizeof(DBASE_FIELD)) == -1)
    printw("%s-%s\n",__FUNCTION__,strerror(errno));
 }
fputc('\r', ff);

fclose(ff);

}

/**************************/
/*Распечатка списка контрагентов*/
/********************************/
void dirkontr_rs(class SQLCURSOR *cur,class dirkontr_poi *rpoi,short metpoi,int kolstr)
{
GDITE();
FILE *ff,*ff1,*ff_dbf;
char imaf[64];
char imaf1[64];
char imaf_dbf_tmp[64];
char imaf_dbf[64];
char imafz[64];


sprintf(imaf,"Kont%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

sprintf(imaf1,"kont%d.lst",getpid());
if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  error_op_nfil(imaf1,errno,"");
  return;
 }

sprintf(imafz,"kontz%d.csv",getpid());
class iceb_fopen ffz;
if(ffz.start(imafz,"w") != 0)
 return;
 
sprintf(imaf_dbf_tmp,"kontr%d.tmp",getpid());
sprintf(imaf_dbf,"partner.dbf"); //реквизиты контрагента
if((ff_dbf = fopen(imaf_dbf_tmp,"w")) == NULL)
 {
  error_op_nfil(imaf_dbf_tmp,errno,"");
  return;
 }

iceb_t_zagolov(gettext("Список контрагентов"),ff1);

struct  tm      *bf;
time_t          tmm;
time(&tmm);
bf=localtime(&tmm);


fprintf(ff,"%s\n\n%s\n\
%s %d.%d.%d%s %s: %d:%d\n",
iceb_t_get_pnk("00",0),
gettext("Список контрагентов"),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
  Код   |           Наименование  контрагента    |\
   Наименование банка         |  Адрес контрагента           |\
     Адрес банка              |  Код     |   MФО    | Номер счета   |\
Нал. номер     |Н.пл. НДС |Телефон   |Группа|Рег.н.ЧП| Налоговый адрес\n"));
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff1,"\
----------------------------------------------------------------------\n");
fprintf(ff1,gettext("\
 Н.п/п|Код контр.|       Наименование контрагента         |Примечание|\n"));
fprintf(ff1,"\
----------------------------------------------------------------------\n");

/*********
123456|123456|123456789012345678901234567890|1234567890
*************/

fprintf(ffz.ff,"#%s\n#%s %s %s\n",
gettext("Экспорт списка контрагентов из базы"),
gettext("Выгружено из базы"),
iceb_t_get_namebase(),iceb_t_get_pnk("00",1));

fprintf(ffz.ff,"\
#%s %d.%d.%d%s  %s:%02d:%02d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ffz.ff,"#Расшифровка полей в записи:\n\
#1  Код контрагента\n\
#2  Наименование контрагента\n\
#3  Наименование банка\n\
#4  Адрес контрагента\n\
#5  Адрес банка\n\
#6  Код ЕГРПОУ\n\
#7  МФО\n\
#8  Номер счета\n\
#9  Индивидуальный налоговый номер\n\
#10 Номер свидетельства плательщика НДС\n\
#11 Телефон\n\
#12 Группа\n\
#13 Кто записал\n\
#14 Время записи\n\
#15 Налоговый адес\n\
#16 регистрационный номер частного предпринимателя\n\
#17 полное наименование контрагента\n\
#18 город контрагента\n\
#19 0-общая система налогообложения 1-единый налог\n");


class iceb_tu_str pnaim("");
int kl=0,i=0;
SQL_str row;
cur->poz_cursor(0);

while(cur->read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++i);
  if(metpoi == 1)
    if(sravrek(row,rpoi) != 0)
      continue;
  kl++;
  fprintf(ff,"%-*s %-*.*s %-*.*s %-*.*s %-*.*s %-*s %-*s\
 %-*s %-*s %-*s %-*.*s %-*s %-*s %s\n",
  iceb_tu_kolbait(8,row[0]),row[0],
  iceb_tu_kolbait(40,row[1]),iceb_tu_kolbait(40,row[1]),row[1],
  iceb_tu_kolbait(30,row[2]),iceb_tu_kolbait(30,row[2]),row[2],
  iceb_tu_kolbait(30,row[3]),iceb_tu_kolbait(30,row[3]),row[3],
  iceb_tu_kolbait(30,row[4]),iceb_tu_kolbait(30,row[4]),row[4],
  iceb_tu_kolbait(10,row[5]),row[5],
  iceb_tu_kolbait(10,row[6]),row[6],
  iceb_tu_kolbait(15,row[7]),row[7],
  iceb_tu_kolbait(15,row[8]),row[8],
  iceb_tu_kolbait(10,row[9]),row[9],
  iceb_tu_kolbait(10,row[10]),iceb_tu_kolbait(10,row[10]),row[10],
  iceb_tu_kolbait(6,row[11]),row[11],
  iceb_tu_kolbait(8,row[15]),row[15],
  row[14]);

  fprintf(ff1,"%6d|%-*s|%-*.*s|%10s|\n",
  kl,
  iceb_tu_kolbait(10,row[0]),row[0],
  iceb_tu_kolbait(40,row[1]),iceb_tu_kolbait(40,row[1]),row[1],
  "");

  for(int nom=40; nom < iceb_tu_strlen(row[1]); nom+=40)
   {
    fprintf(ff1,"%6s|%10s|%-*.*s|%10s|\n","","",
    iceb_tu_kolbait(40,iceb_tu_adrsimv(nom,row[1])),
    iceb_tu_kolbait(40,iceb_tu_adrsimv(nom,row[1])),
    iceb_tu_adrsimv(nom,row[1]),
    "");
   }

  if(row[16][0] != '\0')
     pnaim.new_plus(row[16]);
  else
     pnaim.new_plus(row[1]);

  fprintf(ff_dbf,"\
 %20s\
%-*s\
%-*.*s\
%-20s\
%-*s\
%-*s\
%-*s\
%-*s\
%-*.*s\
%-*s\
%-20s\
%-254s\
%-20s\
%-254s",
  "",
  iceb_tu_kolbait(254,row[5]),row[5],
  iceb_tu_kolbait(254,pnaim.ravno()),iceb_tu_kolbait(254,pnaim.ravno()),pnaim.ravno(),
  "",
  iceb_tu_kolbait(254,row[7]),row[7],
  iceb_tu_kolbait(254,row[6]),row[6],
  iceb_tu_kolbait(254,row[8]),row[8],
  iceb_tu_kolbait(254,row[9]),row[9],
  iceb_tu_kolbait(254,row[4]),iceb_tu_kolbait(254,row[4]),row[4],
  iceb_tu_kolbait(254,row[1]),row[1],
  "","","","");

  fprintf(ffz.ff,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
  row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8],row[9],row[10],row[11],row[14],row[15],row[16],row[17],row[18]);
  
 }
GDITE();

fputc(26,ff_dbf);
fclose(ff_dbf);

perecod(2,imaf_dbf_tmp);

dirkontr_creatheaddbf(imaf_dbf,kl);

/*Сливаем два файла*/
iceb_t_cat(imaf_dbf,imaf_dbf_tmp);

unlink(imaf_dbf_tmp);

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
%s - %d\n",gettext("Итого"),kl);

fprintf(ff1,"\
----------------------------------------------------------------------\n");

podpis(ff);

fclose(ff);

podpis(ff1);

fclose(ff1);
ffz.end();

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Список контрагентов"));

oth.spis_imaf.plus(imaf1);
oth.spis_naim.plus(gettext("Список контрагентов без реквизитов"));
for(int nom=0; nom < oth.spis_imaf.kolih(); nom++)
  iceb_t_ustpeh(oth.spis_imaf.ravno(nom),3);

oth.spis_imaf.plus(imafz);
oth.spis_naim.plus(gettext("Список контрагентов для загрузки в базу"));

oth.spis_imaf.plus(imaf_dbf);
oth.spis_naim.plus(gettext("Список контрагентов в форамате dbf"));


iceb_t_rabfil(&oth,"");
}
/************************************************/
/*поиск если вернули 1 делаем поиск*/
/*********************************************/
int dirkontr_mpoi(class dirkontr_poi *rpoi)
{
int N=0,K=0;
VVOD  VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Введите данные для поиска контрагента"));

VV.VVOD_SPISOK_add_MD(gettext("Наименование контрагента..."));//0
VV.VVOD_SPISOK_add_MD(gettext("Номер счета................"));//1
VV.VVOD_SPISOK_add_MD(gettext("МФО........................"));//2
VV.VVOD_SPISOK_add_MD(gettext("Город (контрагента)........"));//3
VV.VVOD_SPISOK_add_MD(gettext("Код........................"));//4
VV.VVOD_SPISOK_add_MD(gettext("Наименование банка........."));//5
VV.VVOD_SPISOK_add_MD(gettext("Индивидуальный нал. номер.."));//6
VV.VVOD_SPISOK_add_MD(gettext("Город (банка).............."));//7
VV.VVOD_SPISOK_add_MD(gettext("Код группы контрагента....."));//8
VV.VVOD_SPISOK_add_MD(gettext("Система налога............."));//9

VV.VVOD_SPISOK_add_data(rpoi->naim_kont.ravno(),112);//0
VV.VVOD_SPISOK_add_data(rpoi->nsh.ravno(),24);//1
VV.VVOD_SPISOK_add_data(rpoi->mfo.ravno(),16);//2
VV.VVOD_SPISOK_add_data(rpoi->gorod.ravno(),112);//3
VV.VVOD_SPISOK_add_data(rpoi->kod.ravno(),24);//4
VV.VVOD_SPISOK_add_data(rpoi->naimb.ravno(),112);//5
VV.VVOD_SPISOK_add_data(rpoi->inn.ravno(),24);//6
VV.VVOD_SPISOK_add_data(rpoi->gorodb.ravno(),112);//7
VV.VVOD_SPISOK_add_data(rpoi->kodgrkont.ravno(),24);//8
VV.VVOD_SPISOK_add_data(64);//9

 
naz:;

if(rpoi->metka_nal == 0)
 VV.data_plus(9,gettext("Все"));
if(rpoi->metka_nal == 1)
 VV.data_plus(9,gettext("Общая система"));
if(rpoi->metka_nal == 2)
 VV.data_plus(9,gettext("Единый налог"));


helstr(LINES-1,0,"F2/+",gettext("поиск"),
"F4",gettext("очистить"),
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

    if(N == 9) //система налогообложения
     {
      class VVOD MENU(3);

      MENU.VVOD_SPISOK_add_MD(gettext("Все"));
      MENU.VVOD_SPISOK_add_MD(gettext("Общая система"));
      MENU.VVOD_SPISOK_add_MD(gettext("Единый налог"));
      MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

      clearstr(LINES-1,0);
      
      int kom1=0;
      while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

      switch (kom1)
       {
        case 3 :
        case -1:
          goto naz;
      
        case 0 :
          rpoi->metka_nal=0;
          break;

        case 1 :
          rpoi->metka_nal=1;
          break;
        case 2 :
          rpoi->metka_nal=2;
          break;
       }
      goto naz;
     }

    VV.vprintw_clear(N,stdscr);

    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 0)
     {

     }

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
    return(0);
   }

  if(K == FK2 || K == PLU) /*Записываем*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));

    rpoi->naim_kont.new_plus(VV.VVOD_SPISOK_return_data(0));
    rpoi->nsh.new_plus(VV.VVOD_SPISOK_return_data(1));
    rpoi->mfo.new_plus(VV.VVOD_SPISOK_return_data(2));
    rpoi->gorod.new_plus(VV.VVOD_SPISOK_return_data(3));
    rpoi->kod.new_plus(VV.VVOD_SPISOK_return_data(4));
    rpoi->naimb.new_plus(VV.VVOD_SPISOK_return_data(5));
    rpoi->inn.new_plus(VV.VVOD_SPISOK_return_data(6));
    rpoi->gorodb.new_plus(VV.VVOD_SPISOK_return_data(7));
    rpoi->kodgrkont.new_plus(VV.VVOD_SPISOK_return_data(8));


    break;
   }
  if(K == FK4)
   {
    VV.VVOD_clear_data();  
    goto naz;
   }
 }
return(1);
}
/****************************************/
/*Меню F9*/
/******************************************/
void dirkontr_f9(int *metkasort)
{
int kom=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное"));

MENU.VVOD_SPISOK_add_MD(gettext("Изменить сортировку записей"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Импортировать контрагентов из файла"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2


if(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);

switch (kom)
 {
  case 2:
  case -1 :
    return;

  case 0:
    *metkasort+=1;
    if(*metkasort == 2)
      *metkasort=0;
    break;

  case 1:
    dirkontrzag();
    break;

  default:
    kom=0;
    break;
 }
}

/*************************************************/
/*Удаление контрагента*/
/***************************************************/
int dirkontr_uk(const char *kod_kontr)
{
char strsql[1024];

sprintf(strsql,"delete from Skontr where kodkon='%s'",kod_kontr);
if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
  {
   iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
   return(1);
  }
 else
  msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
 }
sprintf(strsql,"delete from Kontragent where kodkon='%s'",kod_kontr);
sql_zapis(strsql,0,0);

sprintf(strsql,"delete from Kontragent1 where kodkon='%s'",kod_kontr);
sql_zapis(strsql,0,0);

sprintf(strsql,"delete from Kontragent2 where kodkon='%s'",kod_kontr);
sql_zapis(strsql,0,0);

return(0);

}