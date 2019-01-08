/*$Id: dirusl.c,v 5.64 2013/11/24 08:23:03 sasa Exp $*/
/*21.04.2017	11.02.2000	Белых А.И.	dirusl.c
Ввод и корректировка списка услуг
*/
#include        <errno.h>
#include <sys/stat.h>
#include <ctype.h>
#include        "buhg.h"

class dirusl_poi
{
 public:
  class iceb_tu_str naimpoi;
  class iceb_tu_str kgrm;
  int mpro; /*метка просмотра 0-обычный 2-помеченные записи*/
  int kolstr;  
  dirusl_poi()
   {
    kolstr=0;
    mpro=0;
    naimpoi.plus("");
    kgrm.plus("");
   }
};

void sapkausl(int mk,class dirusl_poi *poi,const char *naigrup,int dlinna_nazv);
void		udnusl(SQLCURSOR*,long);
int provusl(class dirusl_poi *poi,SQL_str row);
void 		otmusl(char[],long*,short,SQLCURSOR*);
void	        zamku(int);
void            korku(const char *skod,const char *nkod);
void dirusl_rs(class SQLCURSOR *cur,class dirusl_poi *poi);
void dirusl_f9(class SQLCURSOR *curs,class dirusl_poi *poi,class iceb_tu_str *naigrup);
int vuslug1(class iceb_tu_str *kodusl,int mk);

char		*masuslug; /*Массив кодов отмеченных услуг*/

int dirusl(int *kgr, //Код группы
int *kod, //Код услуги
class iceb_tu_str *nai, //Наименование
int mk,     //0-ввод 1-выбор
const char *grup)
{
class dirusl_poi poi;
class iceb_tu_str vstr("");
class spis_oth oth;
long		pozz,pozz1;
SQL_str         row;
char		strsql[1024];
short           x=0,y=4;
int             K;
short           kom,kom1;
int             i,prc;
int             i1;
class iceb_tu_str naigrup("");
class iceb_tu_str kodmv(""); /*Код вновь введенной услуги*/
short		mspz;
class iceb_tu_str kodusl("");
class iceb_tu_str bros("");
char		mom[4];
long		dlmas;
short dlinna_nazv=0;
short max_dlinna_nazv=0;

class iceb_t_mstr spis_menu(0);
SQLCURSOR curr;
//Создаем класс меню
class VVOD DANET(1);
class VVOD VVOD1(2);

poi.kgrm.new_plus(grup);
poi.naimpoi.new_plus(nai->ravno());
pozz=kom=0;


naz1:;
GDITE();
clear();
short           kls=LINES-7;

K=0;
sprintf(strsql,"select * from Uslugi order by kodus asc");
/*
printw("\n%s\n",strsql);
refresh();
*/
SQLCURSOR cur;

if((poi.kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,"",strsql);
  return(1);
 }

if(poi.kolstr == 0)
 {
lll:;
  sapkausl(mk,&poi,naigrup.ravno(),30);

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == FK3 || K == SFK9 || K == SFK3 || K == ENTER)
     goto lll;
  if(K == KEY_RESIZE)
    goto naz1;
  goto vper;
 }

if(poi.naimpoi.ravno()[0] == '\0' && kodmv.getdlinna() > 1 && poi.kolstr > kls  && \
 pozz < poi.kolstr-kls)
 {
  pozz=poivcur(kodmv.ravno(),0,&cur);
  kodmv.new_plus("");
 }

naz:;
GDITE();
clear();

if(poi.kolstr != 0 && pozz > poi.kolstr)
  pozz = poi.kolstr-1;

cur.poz_cursor(pozz);


for(i=0; i < kls ;) /*Вычисляем максимальную длинну названия*/
 {
  
  if(cur.read_cursor(&row) == 0)
    break;


  if(provusl(&poi,row) != 0)
    continue;

  if(iceb_tu_strlen(row[2]) > max_dlinna_nazv)
   max_dlinna_nazv=iceb_tu_strlen(row[2]);
  i++;
 }

cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
spis_menu.spis_str.free_class();
dlinna_nazv=COLS-52;
if(max_dlinna_nazv < dlinna_nazv)
 dlinna_nazv=max_dlinna_nazv;
if(dlinna_nazv < 30)
 dlinna_nazv=30;
for(i=0; i < kls ;)
 {

  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;
  if(provusl(&poi,row) != 0)
    continue;

  memset(mom,'\0',sizeof(mom));
  if(masuslug != NULL)
   {
    sprintf(strsql,"%s",row[0]);
    if(pole1(masuslug,strsql,',',0,&i1) == 0 || SRAV(strsql,masuslug,0) == 0)
      mom[0]='*';
   }

  if(atoi(row[6]) == 1)
   strcat(mom,"#");


  sprintf(strsql,"%-4s|%-2s|%-*.*s|%-*s|%-*s|%8.2f%s",
  row[0],row[1],
  iceb_tu_kolbait(dlinna_nazv,row[2]),iceb_tu_kolbait(dlinna_nazv,row[2]),row[2],
  iceb_tu_kolbait(3,row[3]),row[3],
  iceb_tu_kolbait(4,row[4]),row[4],
  atof(row[5]),mom);

  spis_menu.spis_str.plus(strsql);
  i++;

  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
     mspz=1;

 }

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i == 0)
 {
  if(poi.kolstr > 0 && pozz > 0)
   {
    pozz--;
    goto naz;
   }
  goto lll;
 }

if(i <= kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

sapkausl(mk,&poi,naigrup.ravno(),dlinna_nazv);

prc=(pozz+pozz1)*100/poi.kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,"",0);


if(kom == -7) //Изменение размеров экрана
 goto naz1;
 
if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  GDITE();
  for(i=0; i < mspz  ; )
   {
    pozz++;

    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;

    if(provusl(&poi,row) != 0)
      continue;
    i++;
   }
  kom=kls;
  goto naz;
 }
if(kom == -2) /*Стрелка вверх*/
 {
  GDITE();
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

    if(provusl(&poi,row) != 0)
      continue;
    i++;
   }

  goto naz;
 }
 
if(kom == -5) /*Страница вниз*/
 {
  if(pozz + kls == poi.kolstr)
   {
    pozz=poi.kolstr-1;
    beep();
    goto naz;  
   }
  if(i == kls)
    pozz+=pozz1;
  else
    beep();
  if(pozz >= poi.kolstr)
   {
    pozz=poi.kolstr-pozz1;
    beep();
    goto naz;  
   }
/*  
  printw("\n вперед pozz=%d\n",pozz);
  OSTANOV();
*/
  goto naz;

 }

if(kom == -4) /*Страница вверх*/
 {
/*
  printw("\npozz=%d\n",pozz);
  refresh();
*/
  GDITE();
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

    if(provusl(&poi,row) != 0)
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
  if(pozz+kom >= poi.kolstr)
   goto naz;

  polen(str_vib.ravno(),&kodusl,1,'|');

  *kod=kodusl.ravno_atoi();
 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return(1);

  case ENTER: /*Выход*/
     if(mk == 0)
       goto sfk2;
     return(0);

  case UMN:  /*Переключение режима вывода на экран*/
    if(poi.mpro == 2)
     poi.mpro=0; 
    else
     poi.mpro=2;

    goto naz;
  
  case INS : /*Пометить материал*/
    if(masuslug != NULL)
     {
      if(pole4(masuslug,kodusl.ravno(),',') == 0)
       {
        if(strlen(masuslug) == 0)
         {
          free(masuslug);
          masuslug=NULL;
          dlmas=0;
         }
        kom++; 
        goto naz;
       }
     } 
    sozdmas(strsql,&masuslug,&dlmas,',');
    kom++;
    goto naz;

  case FK1:   /*Помощь*/
   GDITE();
   iceb_t_pdoc("usl3_2.txt");
   clear();
   if(poi.kolstr > 0)
     goto naz;
   else
     goto lll;

  case FK2: /*Ввод новой услуги*/
  case PLU: /*Ввод новой услуги*/
    kodmv.new_plus("");
    vuslug1(&kodmv,0);

    poi.naimpoi.new_plus("");
    goto naz1;    

  case SFK2:   /*Корректировать запись*/

sfk2:;

    
    vuslug1(&kodusl,1);



    goto naz1;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      if(provud("Uslugi",kodusl.ravno(),1) != 0)
        goto naz;
        
      sprintf(strsql,"delete from Uslugi where kodus=%s",kodusl.ravno());
      if(sql_zap(&bd,strsql) != 0)
       {
       if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
        {
         iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
        }
       else
        msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
       }
      clear();
      kodmv.new_plus("");
      if(pozz == poi.kolstr-1)
       pozz=0;
      goto naz1;
     }
    goto naz;

  case SFK3:       /*Удалить не используемые записи*/
   udnusl(&cur,poi.kolstr);
   goto naz1;

  case FK4:       /*Отобрать по образцу*/

     helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
     
     poi.naimpoi.new_plus("");
     
     VVOD1.VVOD_delete();
     VVOD1.VVOD_SPISOK_add_MD(gettext("Введите образец для поиска"));

     vvod1(&poi.naimpoi,32,&VVOD1,NULL,stdscr,-1,-1);

     kom=pozz=0;
     if(poi.kolstr == 0)
       goto lll;
     else
       goto naz;

  case FK5:       /*Распечатать*/
   dirusl_rs(&cur,&poi);
   goto naz;

  case FK6:       /*Стать на номер */

    helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
    
    
    VVOD1.VVOD_delete();
    VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код услуги"));
    vstr.new_plus("");
    if((kom1=vvod1(&vstr,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
      goto naz;
    if(kom1 == ESC)
      goto naz;
    if(vstr.getdlinna() <= 1)
      goto naz;
      
    poi.naimpoi.new_plus("");
    pozz=0;
    kom=0;
    pozz=poivcur(vstr.ravno(),0,&cur);
    if(pozz == poi.kolstr)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден код услуги"),vstr.ravno());
      iceb_t_soob(strsql);
      pozz=0;
     }
    clear();
    goto naz;

  case SFK6:  //Замена одного кода услуги на другой с удалением первого
    zamku(kodusl.ravno_atoi());

    clear();
    goto naz1;

  case FK9:       /*меню*/
    dirusl_f9(&cur,&poi,&naigrup);
    goto naz1;

  default:
   if(poi.kolstr > 0)
     goto naz;
   else
     goto lll;
 }
}

/****************************/
/*Ввод и корректировка услуг*/
/*если вернули 0 - записали 1-нет*/
/****************************/
int vuslug1(class iceb_tu_str *kodusl, //Код материалла
int mk) //0- ввод нового 1 - корректировка
{
int kodg=0; //Код группы материалла
int kt=0; //Кто записал
time_t vr=0; //Время записи
class iceb_tu_str ei(""); //Единица измерения
struct  tm      *bf;
int             K;
int             N;
class iceb_tu_str naimm("");
class iceb_tu_str naimmz(""); /*Запомненное первичное наименование*/
class iceb_tu_str kodmz("");
class iceb_tu_str kodgg("");
time_t		vrem;
char		strsql[1024];
SQL_str         row;
int		poz,komv;
int		i;
struct OPSHET	shetv;
int		nds=0;
class iceb_tu_str bros("");
short           metka_korkod=0;
class iceb_tu_str naim_usl("");
class iceb_tu_str artikul("");
class iceb_tu_str ku("");
class iceb_tu_str shetu("");
class iceb_tu_str naim_gr("");
double cena=0.;

SQLCURSOR cur;
int voz=1;

if(mk == 1)
 {
  sprintf(strsql,"select * from Uslugi where kodus=%d",kodusl->ravno_atoi());
  if(readkey(strsql,&row,&cur) != 1)
   {
    printw("\n%s-%s\n",__FUNCTION__,strsql);
    OSTANOV();   
    return(1);
   }
  kodg=atoi(row[1]);
  naim_usl.new_plus(row[2]);
  ei.new_plus(row[3]);
  vr=atol(row[8]);
  kt=atoi(row[7]);
  nds=atoi(row[6]);
  artikul.new_plus(row[9]);
  ku.new_plus(row[10]);
  cena=atof(row[5]);
  shetu.new_plus(row[4]);

  /*Узнаем наименование группы материалла*/
  sprintf(strsql,"select naik from Uslgrup where kod=%d",kodg);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    naim_gr.new_plus("");
   }
  else
   naim_gr.new_plus(row[0]);
 }

if(kodusl->ravno_atoi() == 0)
 kodusl->new_plus(nomkm());


class VVOD VV(0);

VV.VVOD_SPISOK_add_data(10);//0
VV.VVOD_SPISOK_add_data(50);//1
VV.VVOD_SPISOK_add_data(80);//2
VV.VVOD_SPISOK_add_data(20);//3
VV.VVOD_SPISOK_add_data(50);//4
VV.VVOD_SPISOK_add_data(10);//5
VV.VVOD_SPISOK_add_data(30);//6
VV.VVOD_SPISOK_add_data(32);//7
VV.VVOD_SPISOK_add_data(32);//8

VV.VVOD_SPISOK_zapis_data(0,kodusl->ravno());
VV.VVOD_SPISOK_zapis_data(1,naim_gr.ravno());


VV.VVOD_SPISOK_zapis_data(2,naim_usl.ravno());
VV.VVOD_SPISOK_zapis_data(7,artikul.ravno());
VV.VVOD_SPISOK_zapis_data(8,ku.ravno());
if(cena > 0.009)
 {
  sprintf(strsql,"%.15g",cena);
  VV.VVOD_SPISOK_zapis_data(3,strsql);
 }
class iceb_tu_str naiei("");
if(ei.getdlinna() > 1)
 {
  /*Узнаем наименование единицы измерения*/
  sprintf(strsql,"select naik from Edizmer where kod='%s'",
  ei.ravno());
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,gettext("Не найдена единица измерения %s !"),ei.ravno());
    iceb_t_soob(strsql);
    ei.new_plus("");
   }
  else
    naiei.new_plus(row[0]);
 }

VV.VVOD_SPISOK_zapis_data(4,naiei.ravno());
VV.VVOD_SPISOK_zapis_data(5,shetu.ravno());
if(nds == 0.)
 VV.VVOD_SPISOK_zapis_data(6,gettext("C НДС"));
if(nds == 1.)
  VV.VVOD_SPISOK_zapis_data(6,gettext("Без НДС"));

/*******************/


kodmz.new_plus(VV.VVOD_SPISOK_return_data(0));
naimmz.new_plus(VV.VVOD_SPISOK_return_data(2));
kodgg.new_plus(kodg);

if(mk == 0)
 VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));
else
 VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));
 
if(kt != 0)
 {
  clearstr(1,0);
  sprintf(strsql,"%s %s",gettext("Записал"),iceb_t_kszap(kt));
  VV.VVOD_SPISOK_add_ZAG(strsql);
 }
if(vr != 0)
 {
  bf=localtime(&vr);
  clearstr(2,0);
  sprintf(strsql,"%s %d.%d.%d%s %s:%02d:%02d:%02d",
  gettext("Дата записи"),
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
  gettext("г."),
  gettext("Время"),
  bf->tm_hour,bf->tm_min,bf->tm_sec);
  VV.VVOD_SPISOK_add_ZAG(strsql);
 }

VV.VVOD_SPISOK_add_MD(gettext("Код услуги........."));//0
VV.VVOD_SPISOK_add_MD(gettext("Группа............."));//1
VV.VVOD_SPISOK_add_MD(gettext("Наименование......."));//2
VV.VVOD_SPISOK_add_MD(gettext("Цена..............."));//3
VV.VVOD_SPISOK_add_MD(gettext("Единица измерения.."));//4
VV.VVOD_SPISOK_add_MD(gettext("Счёт учета........."));//5
VV.VVOD_SPISOK_add_MD(gettext("Цена введена......."));//6
VV.VVOD_SPISOK_add_MD(gettext("Артикул............"));//7
VV.VVOD_SPISOK_add_MD(gettext("Код ГКПУ..........."));//8  Державний класіфікатор продукції і послуг


N=1;

naz:;

clear();
if(nds == 0)
  VV.data_plus(6,gettext("C НДС"));
if(nds == 1)
  VV.data_plus(6,gettext("Без НДС"));

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F4",gettext("найти код"),
"F10",gettext("выход"),NULL);


VV.vramka(-1,-1,1);


refresh();
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


    if(N == 0)  /*Код услуги*/
     {
      VV.vgetstr(N,stdscr);

      attroff(VV.VVOD_return_cs(iceb_CFV));
      attron(VV.VVOD_return_cs(iceb_CFM));

      if(mk == 0 && VV.VVOD_SPISOK_return_data(N)[0] != '\0')
       {
        sprintf(strsql,"select naimat from Material where kodm=%s",
        VV.VVOD_SPISOK_return_data(N));
        if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
         {
          attroff(VV.VVOD_return_cs(iceb_CFV));
          VVOD SOOB(1);
          sprintf(strsql,gettext("Код %s уже имеет материал !"),VV.VVOD_SPISOK_return_data(N));
          SOOB.VVOD_SPISOK_add_MD(strsql);
          sprintf(strsql,gettext("Это: %s"),row[0]);
          SOOB.VVOD_SPISOK_add_MD(strsql);
          SOOB.VVOD_SPISOK_add_MD(gettext("Нумерация материалов и услуг сквозная"));
          soobshw(&SOOB,stdscr,-1,-1,0,1);

          VV.data_plus(N,"");
          goto naz;
         }
        sprintf(strsql,"select naius from Uslugi where kodus=%s",
        VV.VVOD_SPISOK_return_data(N));
        if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
         {
          attroff(VV.VVOD_return_cs(iceb_CFV));
          VVOD SOOB(1);
          sprintf(strsql,gettext("Код услуги %s уже введён !"),VV.VVOD_SPISOK_return_data(N));
          SOOB.VVOD_SPISOK_add_MD(strsql);
          sprintf(strsql,gettext("Это: %s"),row[0]);
          SOOB.VVOD_SPISOK_add_MD(strsql);
          soobshw(&SOOB,stdscr,-1,-1,0,1);

          VV.data_plus(N,"");
          goto naz;
         }
       }
     }

    if(N == 1)   /*Группа услуги*/
     {

      VV.VVOD_SPISOK_zapis_data(N,"");
      naimm.new_plus("");
      kodgg.new_plus("");
      poz=komv=0;
      if(dirmat1("Uslgrup",&kodgg,&naimm,-1,-1,-1,2,&poz,&komv,"","\0") != 0)
       {
	kodgg.new_plus(kodg);
       }
      else
       {
        VV.VVOD_SPISOK_zapis_data(N,naimm.ravno());
       }
      N++;
      goto naz;
     }
    if(N == 2)  /*Наименование услуги*/
     {
      VV.vgetstr(N,stdscr);
      attroff(VV.VVOD_return_cs(iceb_CFV));
      attron(VV.VVOD_return_cs(iceb_CFM));
      /*Проверяем наименование*/
      if(SRAV(naimmz.ravno(),VV.VVOD_SPISOK_return_data(N),0) != 0)
       {
        bros.new_plus(VV.VVOD_SPISOK_return_data(N));
        sprintf(strsql,"select kodus from Uslugi where naius='%s'",bros.ravno_filtr());
 
        if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
         {
          attroff(VV.VVOD_return_cs(iceb_CFV));
          sprintf(strsql,"%s:%s",gettext("Такое наименование уже введено. Код"),row[0]);
          iceb_t_soob(strsql);
          VV.data_plus(N,"");
          goto naz;
         }
        }

      naimm.new_plus(VV.VVOD_SPISOK_return_data(N));

     }

    if(N == 3)  /*Цена реализации*/
      VV.vgetstr(N,stdscr);

    if(N == 4)  /*Единица измерения*/
     {
      VV.VVOD_SPISOK_zapis_data(N,"");
      naimm.new_plus("");

      if(iceb_tl_ei(2,&ei,&naimm) != 0)
       ei.new_plus("");

      VV.VVOD_SPISOK_zapis_data(N,naimm.ravno());
      N++;
      goto naz;
     }

    if(N == 5)  /*Счёт учета*/
     {
      VV.vgetstr(N,stdscr);
      if(VV.VVOD_SPISOK_return_data(N)[0] != '\0')
       {
        if((i=prsh(VV.VVOD_SPISOK_return_data(N),&shetv)) == 0)
         {
          attroff(VV.VVOD_return_cs(iceb_CFV));
          attroff(VV.VVOD_return_cs(iceb_CFV));

          sprintf(strsql,gettext("Нет счета %s в плане счетов !"),VV.VVOD_SPISOK_return_data(N));
          iceb_t_soob(strsql);

          VV.data_plus(N,"");
          goto naz;
         }

        if(i == 2)
         {
          attroff(VV.VVOD_return_cs(iceb_CFV));
          beep();
          attroff(VV.VVOD_return_cs(iceb_CFV));
 
          sprintf(strsql,gettext("Счёт %s имеет субсчета. Введите нужный субсчет !"),VV.VVOD_SPISOK_return_data(N));
          iceb_t_soob(strsql);
          VV.data_plus(N,"");
          goto naz;
         }
       }
     }
    if(N == 6)  /*Цена с НДС или нет*/
     {
      attroff(VV.VVOD_return_cs(iceb_CFV));
      nds++;
      if(nds > 1)
       nds=0;
      goto naz;
     }

    if(N == 7)  /*Артикул*/
      VV.vgetstr(N,stdscr);

    if(N == 8)  
      VV.vgetstr(N,stdscr);

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
    N=-1;
    voz=1;
    break;
   }

  if(K == FK2 || K == PLU) /*Записываем*/
   {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     attron(VV.VVOD_return_cs(iceb_CFM));
    if(VV.VVOD_SPISOK_return_data(0)[0] == '\0' || ATOFM(VV.VVOD_SPISOK_return_data(0)) == 0.)
     {
      iceb_t_soob(gettext("Не введён код услуги !"));
      goto naz;
     }
    if(ATOFM(VV.VVOD_SPISOK_return_data(0)) < 0)
     {
      iceb_t_soob(gettext("Код не может быть отрицательным !"));
      goto naz;
     }
    if(VV.VVOD_SPISOK_return_data(1)[0] == '\0')
     {
      iceb_t_soob(gettext("Не введена группа услуги !"));
      goto naz;
     }

    if(VV.VVOD_SPISOK_return_data(2)[0] == '\0')
     {
      iceb_t_soob(gettext("Не введено наименование услуги !"));
      goto naz;
     }
    if(mk == 0 )
     {
      sprintf(strsql,"select naimat from Material where kodm=%s",
      VV.VVOD_SPISOK_return_data(0));
      if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
       {
        attroff(VV.VVOD_return_cs(iceb_CFV));
        VVOD SOOB(1);
        sprintf(strsql,gettext("Код %s уже имеет материал !"),VV.VVOD_SPISOK_return_data(0));
        SOOB.VVOD_SPISOK_add_MD(strsql);
        sprintf(strsql,gettext("Это: %s"),row[0]);
        SOOB.VVOD_SPISOK_add_MD(strsql);
        SOOB.VVOD_SPISOK_add_MD(gettext("Нумерация материалов и услуг сквозная"));
        soobshw(&SOOB,stdscr,-1,-1,0,1);
        VV.data_plus(N,"");
        goto naz;
       }
      sprintf(strsql,"select naius from Uslugi where kodus=%s",
      VV.VVOD_SPISOK_return_data(0));
      if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
       {
        attroff(VV.VVOD_return_cs(iceb_CFV));
        VVOD SOOB(1);
        sprintf(strsql,gettext("Код услуги %s уже введён !"),VV.VVOD_SPISOK_return_data(0));
        SOOB.VVOD_SPISOK_add_MD(strsql);
        sprintf(strsql,gettext("Это: %s"),row[0]);
        SOOB.VVOD_SPISOK_add_MD(strsql);
        soobshw(&SOOB,stdscr,-1,-1,0,1);
        VV.data_plus(N,"");
        goto naz;
       }
     }
    time(&vrem);
    /*Обрабатываем апостроф*/
//    sqlfiltr(VV.VVOD_SPISOK_return_data(2),VV.VVOD_SPISOK_return_dls(2));
    if(VV.VVOD_SPISOK_return_data(3)[0] == '\0')
        VV.data_plus(3,"0");

    if(mk == 0)       
     {
      kodusl->new_plus(VV.VVOD_SPISOK_return_data(0));
      sprintf(strsql,"insert into Uslugi \
values (%s,%s,'%s','%s','%s',%.10g,%d,%d,%ld,'%s','%s')",
      VV.VVOD_SPISOK_return_data(0),
      kodgg.ravno(),VV.data_ravno_filtr(2),
      ei.ravno(),
      VV.VVOD_SPISOK_return_data(5),
      VV.data_ravno_atof(3),
      nds,
      iceb_t_getuid(),
      vrem,
      VV.data_ravno_filtr(7),
      VV.data_ravno_filtr(8));
     }
    /*Корректировка записи*/
    if(mk == 1)       
     {
      if(SRAV(kodmz.ravno(),VV.VVOD_SPISOK_return_data(0),0) != 0)
       {
        VVOD DANET(1);
        DANET.VVOD_SPISOK_add_MD(gettext("Корректированть код услуги ? Вы уверены ?"));

        if(danet(&DANET,2,stdscr) == 2)
          goto naz;
        metka_korkod=1;
       }            

      sprintf(strsql,"update Uslugi \
set \
kodus = %s,\
kodgr= %s,\
naius='%s',\
ei='%s',\
shetu='%s',\
cena=%.10g,\
nds=%d,\
ktoz=%d,\
vrem=%ld,\
art='%s',\
ku='%s' \
where kodus=%s",
VV.VVOD_SPISOK_return_data(0),
kodgg.ravno(),
VV.data_ravno_filtr(2),
ei.ravno(),
VV.VVOD_SPISOK_return_data(5),VV.data_ravno_atof(3),nds,iceb_t_getuid(),vrem,
VV.data_ravno_filtr(7),
VV.data_ravno_filtr(8),
kodmz.ravno());
     } 
/*
    printw("\n%s\n",strsql);
    refresh();

    OSTANOV(); 
*/

    if(sql_zap(&bd,strsql) != 0)
     {
     if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
      {
       iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
       goto naz;
      }
     else
      {
       msql_error(&bd,gettext("Ошибка записи !"),strsql);
       goto naz;
      }
     }
    voz=0;
    if(metka_korkod == 1)
       korku(kodmz.ravno(),VV.VVOD_SPISOK_return_data(0));
     
    break;
   }
  if(K == FK4) /*Взять код*/
   {
//    sprintf(VV.VVOD_SPISOK_return_data(0),"%d",nomkm());
    VV.data_plus(0,nomkm());
    goto naz;
   }
 }

attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
refresh();
return(voz);
}

/**************/
/*Шапка услуг*/
/**************/
void sapkausl(int mk, //0-ввод 1-выбор
class dirusl_poi *poi,const char *naigrup,int dlinna_nazv)
{

move(0,0);
clrtoeol();  /*Очистить стороку*/
printw("%s %d",gettext("Ввод и корректировка услуг"),poi->kolstr);

clearstr(1,0);
if(poi->kgrm.ravno()[0] != '\0')
 {
  attron(A_BLINK);
  printw(gettext("Поиск группы: %s %s"),poi->kgrm.ravno(),naigrup);
  attroff(A_BLINK);
 }

if(poi->naimpoi.ravno()[0] != '\0')
 {
  clearstr(2,0);
  attron(A_BLINK);
  printw("%s:%s",gettext("Найдено по образцу"),poi->naimpoi.ravno());
  attroff(A_BLINK);
 }
int raz=dlinna_nazv-28;
//if(raz < 15)
// raz=15;
 
move(3,0);
printw(gettext("Код  Груп.      Наименование           %*sЕд.из.Счёт   Цена."),raz,"");

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F6",gettext("код"),
"F9",gettext("меню"),
"F10",gettext("выход"),NULL);

}

/********************************/
/*Удаление не используемых услуг*/
/********************************/
void 	udnusl(SQLCURSOR *cur,long kolstr)
{
int		koludk;
SQL_str         row;
long		kolstr1;
char		strsql[512];
class iceb_tu_str imatab("");

VVOD DANET(1);
DANET.VVOD_SPISOK_add_MD(gettext("Удалить не используемые услуги ? Вы уверены ?"));

if(danet(&DANET,2,stdscr) == 2)
  return;



clear();
cur->poz_cursor(0);
koludk=kolstr1=0;
while(cur->read_cursor(&row) != 0)
 {
  kolstr1++;
  strzag(LINES-1,0,kolstr,kolstr1);
  printw("%-5s %-40s",row[0],row[2]);
  if(provud("Uslugi",row[0],0) != 0)
   {
    printw(" %s\n",gettext("используется"));
    continue;
   }
  koludk++;
  printw(" %s\n",gettext("удален"));

  sprintf(strsql,"delete from Uslugi where kodus=%s",row[0]);
  if(sql_zap(&bd,strsql) != 0)
   {
   if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
    {
     iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
     return;
    }
   else
     msql_error(&bd,gettext("Ошибка удаления записи !"),strsql);
   }
 }
printw("%s: %d\n",gettext("Количество удаленных услуг"),koludk);

imatab.new_plus("Uslugi");
printw(gettext("Оптимизация таблицы %s !\n"),imatab.ravno());
GDITE();

int		metkazap=0;
SQLCURSOR curr;
strcpy(strsql,"select VERSION()");
sql_readkey(&bd,strsql,&row,&curr);
if(SRAV(row[0],"3.22",1) == 0)
  metkazap=1;  
printw("VERSION:%s\n",row[0]);
refresh();

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"optimize table %s",imatab.ravno());

if(metkazap == 1)
 {
  if(sql_zap(&bd,strsql) != 0)
   msql_error(&bd,gettext("Ошибка оптимизации таблицы !"),strsql);
 }
else
  sql_readkey(&bd,strsql);

printw(gettext("Оптимизацию таблицы закончено\n"));

OSTANOV();

}
/*************************************************/
/*Проверка строки на соотвествие заданному поиску*/
/*************************************************/
//  if(provusl(kgrm.ravno(),row[1],naimpoi.ravno(),row[2],mpro,row[0]) != 0)
//int		provusl(const char *kgrm,const char kgrm1[],const char nm[],const char nm1[],
//short mpro,
//const char kodm[]) //Метка просмотра списка 0-обычный 2-помеченные записи

int provusl(class dirusl_poi *poi,SQL_str row)
{
int		i1;

if(proverka(poi->kgrm.ravno(),row[1],0,0) != 0)
 return(1);
/********************
if(kgrm[0] != '\0')
 if(pole1(kgrm,kgrm1,',',0,&i1) != 0 && SRAV(kgrm,kgrm1,0) != 0)
   return(1);
*******************/
if(proverka(poi->naimpoi.ravno(),row[2],4,0) != 0)
 return(2);
/**************************************    
if(nm[0] != '\0' && strstrm(nm1,nm) == 0)
  return(2);
******************************/

if(poi->mpro == 2 && (masuslug != NULL && pole1(masuslug,row[0],',',0,&i1) != 0))
  return(3);

return(0);
}
/*******************************/
/*Отметка материалов по образцу*/
/*******************************/
void otmusl(char obr[],long *dlmas,
short mt, //0-отметить 1-снять отметку
SQLCURSOR *cur)
{
long		ddd;
int		i;
SQL_str         row;
char		strsql[512];

ddd=*dlmas;
if(mt == 1 && masuslug == NULL)
  return;

GDITE();
if(mt == 1 && obr[0] == '\0')
 {
  free(masuslug); 
  masuslug=NULL;
  *dlmas=0;
  return;
 }

if(masuslug != NULL)
  memset(masuslug,'\0',ddd);

cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {

  if(obr[0] == '\0' || strstrm(row[2],obr) != 0)
   {
    sprintf(strsql,"%s",row[0]);
    if(mt == 0)
     { 
      if(masuslug != NULL)
       if(pole1(masuslug,strsql,',',0,&i) == 0)
         continue;
      sozdmas(strsql,&masuslug,&ddd,',');
     }
    if(mt == 1)
     {
      if(pole4(masuslug,strsql,',') == 0)
       {
        if(strlen(masuslug) == 0)
         {
          free(masuslug);
          masuslug=NULL;
          *dlmas=0;
          return;
         }
       }

     }
   }

 }

}
/*************************************/
/*Замена одного кода услуги на другой*/
/*************************************/

void	zamku(int kodu)
{
char		strsql[512];
int		kom1=0;
class iceb_tu_str koduz("");
SQL_str		row;
SQLCURSOR curr;

VVOD VVOD1(2);

sprintf(strsql,"%s:%d",gettext("Код услуги"),kodu);
VVOD1.VVOD_SPISOK_add_ZAG(strsql);
VVOD1.VVOD_SPISOK_add_ZAG(gettext("Введите код услуги на который его надо заменить"));

naz:;

if((kom1=vvod1(&koduz,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
  return;
if(kom1 == ESC)
  return;
if(kodu == koduz.ravno_atoi())
 {
  iceb_t_soob(gettext("Коды услуг одинаковые!"));
  goto naz;
 }
//Проверяем есть ли такой код в списке услуг
sprintf(strsql,"select kodus from Uslugi where kodus=%s",koduz.ravno());
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код услуги"),koduz.ravno());
  iceb_t_soob(strsql);
  goto naz;
 }

if(parolv(gettext("Введите пароль"),0) != 0)
 return;


VVOD SOOBN(1);
SOOBN.VVOD_SPISOK_add_MD(gettext("Выполняется каскадная корректировка кода в таблицах"));
soobsh(&SOOBN,-1,-1,1);

GDITE();

int nz=0;
int nomer_er=0;
nazad:;

sprintf(strsql,"update Usldokum1 set kodzap=%s,nz=%d where kodzap=%d and metka=1",koduz.ravno(),nz++,kodu);
if(sql_zap(&bd,strsql) != 0)
 {
  if((nomer_er=sql_nerror(&bd)) == ER_DBACCESS_DENIED_ERROR) //Только чтение
   {
    iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
    return;
   }
  else
   {
    if(nomer_er == ER_DUP_ENTRY) /*Запись уже есть*/     
     goto nazad;
    else
      msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
   }
 }

nz=0;

nazad1:;

sprintf(strsql,"update Usldokum2 set kodzap=%s,nz=%d where kodzap=%d and metka=1",koduz.ravno(),nz++,kodu);
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_ENTRY) /*Запись уже есть*/
   goto nazad1; 
  else
   msql_error(&bd,__FUNCTION__,strsql);
 }
 
sprintf(strsql,"update Roznica set kodm=%s where kodm=%d and metka=2",koduz.ravno(),kodu);
if(sql_zap(&bd,strsql) != 0)
  msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

sprintf(strsql,"update Specif set kodd=%s where kodd=%d and kz=1",koduz.ravno(),kodu);
if(sql_zap(&bd,strsql) != 0)
  msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);

nz=0;

nazad2:;

//sprintf(strsql,"select mz from Restdok1 where mz=1 and kodz=%s limit 1",kod);

sprintf(strsql,"update Restdok1 set kodz=%d,nz=%d where kodz=%d and mz=1",koduz.ravno_atoi(),nz++,kodu);
if(sql_zap(&bd,strsql) != 0)
 {

  if((nomer_er=sql_nerror(&bd)) != ER_NO_SUCH_TABLE) /*Такой таблицы нет*/
   {  
    if(nomer_er == ER_DUP_ENTRY) /*Запись уже есть*/
     goto nazad2; 
    else
     msql_error(&bd,__FUNCTION__,strsql);
   }
 }

sprintf(strsql,"%d",kodu);
if(provud("Uslugi",strsql,1) != 0)
  goto naz;
  
sprintf(strsql,"delete from Uslugi where kodus=%d",kodu);
sql_zapis(strsql,1,0);

}
/***********************************/
/*Корректирвка кода услуги         */
/***********************************/

void   korku(const char *skod,const char *nkod)
{
char strsql[512];

VVOD SOOBN(1);
SOOBN.VVOD_SPISOK_add_MD(gettext("Выполняется каскадная корректировка кода в таблицах !"));
soobsh(&SOOBN,-1,-1,1);

sprintf(strsql,"update Usldokum1 set kodzap=%s where kodzap=%s and metka=1",nkod,skod);
if(sql_zapis(strsql,1,0) != 0)
  return;

sprintf(strsql,"update Usldokum2 set kodzap=%s where kodzap=%s and metka=1",nkod,skod);
sql_zapis(strsql,1,0);


sprintf(strsql,"update Roznica set kodm=%s where kodm=%s and metka=2",nkod,skod);
sql_zapis(strsql,1,0);

sprintf(strsql,"update Specif set kodd=%s where kodd=%s and kz=1",nkod,skod);
sql_zapis(strsql,1,0);

}
/****************************/
void dirusl_rs_hap(int nom_str,int *kolstr,FILE *ff)
{
if(kolstr != NULL)
 *kolstr+=4;

fprintf(ff,"%55s %s N%d\n","",gettext("Страница"),nom_str);

fprintf(ff,"\
---------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Код |Гр.|        Наименование          |Ед.из.| Счёт |  Цена  |Артикул | Код\n"));
fprintf(ff,"\
---------------------------------------------------------------------------------\n");
}

/***********************************************/
/*Распечатка*/
/*********************************************/
void dirusl_rs(class SQLCURSOR *cur,class dirusl_poi *poi)
{
SQL_str row;
struct  tm      *bf;
time_t          tmm;
char imaf[64];
char imafcsv[64];

sprintf(imaf,"usl%d.lst",getpid());

class iceb_fopen filrs;
if(filrs.start(imaf,"w") != 0)
 return;

sprintf(imafcsv,"usl%d.csv",getpid());

class iceb_fopen filcsv;
if(filcsv.start(imafcsv,"w") != 0)
 return;

time(&tmm);
bf=localtime(&tmm);

fprintf(filrs.ff,"%s\n\n%s.\n\
%s %d.%d.%d %s: %d:%d\n",
iceb_t_get_pnk("00",0),
gettext("Распечатка справочника услуг"),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("Время"),
bf->tm_hour,bf->tm_min);

dirusl_rs_hap(1,NULL,filrs.ff);


fprintf(filcsv.ff,"#%s\n#%s %s %s\n",
gettext("Экспорт списка услуг"),
gettext("Выгружено из базы"),
iceb_t_get_namebase(),
iceb_t_get_pnk("00",1));

fprintf(filcsv.ff,"\
#%s %d.%d.%d%s  %s:%02d:%02d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(filcsv.ff,"#Расшифровка полей:\n\
#1 код услуги\n\
#2 код группы\n\
#3 наименование услуги\n\
#4 единица измерения\n\
#5 счёт учета\n\
#6 цена услуги\n\
#7 0-цена без НДС 1-цена с НДС\n\
#8 артикул\n\
#9 код УКТ ВЭД Украинская класификация товаров внешне экономической деятельности\n");


int i=0;
cur->poz_cursor(0);
int kolstr1=0;
while(cur->read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,poi->kolstr,++kolstr1);
  if(provusl(poi,row) != 0)
    continue;
  i++;

  fprintf(filrs.ff,"%-5s %-3s %-*.*s %-*s %-*s %8s %-*.*s %s\n",
  row[0],row[1],
  iceb_tu_kolbait(30,row[2]),iceb_tu_kolbait(30,row[2]),row[2],
  iceb_tu_kolbait(6,row[3]),row[3],
  iceb_tu_kolbait(6,row[4]),row[4],
  row[5],
  iceb_tu_kolbait(8,row[9]),
  iceb_tu_kolbait(8,row[9]),
  row[9],
  row[10]);

  if(iceb_tu_strlen(row[2]) > 30)
   fprintf(filrs.ff,"%5s %3s %s\n","","",iceb_tu_adrsimv(30,row[2]));   

  fprintf(filcsv.ff,"%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
  row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10]);
  
 }

fprintf(filrs.ff,"ICEB_LST_END\n\
---------------------------------------------------------------------------------\n");
fprintf(filrs.ff,"\
%s - %d\n",gettext("Количество записей"),i);

podpis(filrs.ff);

filrs.end();
filcsv.end();

class spis_oth oth;

oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка услуг"));

int orient=iceb_t_ustpeh(oth.spis_imaf.ravno(0),0);
iceb_t_rnl(oth.spis_imaf.ravno(0),orient,NULL,&dirusl_rs_hap);

oth.spis_imaf.plus(imafcsv);
oth.spis_naim.plus(gettext("Распечатка услуг для загрузки в базу"));

iceb_t_rabfil(&oth,"");
}

/*******************************************/
/*импорт*/
/**********************************************/
void dirusl_imp()
{
char strsql[2048];
struct stat work;
int kom=0;
static class iceb_tu_str imafim("");

VVOD VVOD1(2);
VVOD1.VVOD_SPISOK_add_ZAG(gettext("Импорт списка услуг из файла"));
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите имя файла с услугами"));

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
    iceb_t_pdoc("usl3_2_1.txt");
    clear();
    goto naz;

  case ENTER:
    if(imafim.ravno()[0] == '\0')
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

if(stat(imafim.ravno(),&work) != 0)
 {
  sprintf(strsql,"%s !",gettext("Нет такого файла"));
  iceb_t_soob(strsql);
  goto naz;
 }

clear();

char strok[2048];

class iceb_fopen filimp;
if(filimp.start(imafim.ravno(),"r") != 0)
 return;

char imaftmp[64];
sprintf(imaftmp,"imp%d.tmp",getpid());

class iceb_fopen filtmp;
if(filtmp.start(imaftmp,"w") != 0)
 return;

printw("%s\n\n",gettext("Загружаем записи из файла"));
int razmer=0;
int i=0;
#define KOLKOL 8
class iceb_tu_str rek[KOLKOL];
SQL_str row;
class SQLCURSOR cur;
int ktoi=iceb_t_getuid();
int kolihoh=0;
int nom=0;
while(fgets(strok,sizeof(strok),filimp.ff) != NULL)
 {
  if(strok[0] == '#')
   continue;
  
  i=strlen(strok);
  razmer+=i;
  if(strok[i-2] == '\r') //Возврат каректки меняем на пробел
   strok[i-2]=' ';

  printw("%s",strok);
  strzag(LINES-1,0,work.st_size,razmer);

  for(nom=0; nom < KOLKOL; nom++)
   if(polen(strok,&rek[nom],nom+1,'|') != 0)
    {
     fprintf(filtmp.ff,"%s",strok);
     fprintf(filtmp.ff,"#%s %d\n",gettext("Не найдено поле"),nom+1);
     kolihoh++;
     break;     
    }

  if(nom < KOLKOL)
   continue;

  /*Проверяем наименование услуги*/
  sprintf(strsql,"select naius from Uslugi where naius='%s'",rek[2].ravno_filtr());
  if(readkey(strsql,&row,&cur) != 0)
   {
    fprintf(filtmp.ff,"%s",strok);
    fprintf(filtmp.ff,"#%s\n",gettext("Такое наименование уже есть"));
    printw("%s\n",gettext("Такое наименование уже есть"));
    continue;
   }

  /*проверяем единицу измерения*/
  if(rek[3].getdlinna() > 1)
   {
    sprintf(strsql,"select naik from Edizmer where kod='%s'",rek[3].ravno_filtr());
    if(readkey(strsql) != 1)
     rek[3].new_plus("");
   }

  /*проверяем код группы*/
  if(rek[1].getdlinna() > 1)
   {
    sprintf(strsql,"select kod from Uslgrup where kod=%d",rek[1].ravno_atoi());
    if(readkey(strsql) != 1)
     rek[1].new_plus("");

   }

  class lock_tables kkk("LOCK TABLES Material READ,Uslugi WRITE");
  /*проверяем код материала*/
  if(rek[0].ravno_atoi() > 0)
   {
    sprintf(strsql,"select kodus from Uslugi where kodus='%d'",rek[0].ravno_atoi());
    if(readkey(strsql,&row,&cur) >= 1)
     {
      rek[0].new_plus(nomkm());
     }
    else
     {
      sprintf(strsql,"select kodm from Material where kodm=%d",rek[0].ravno_atoi());
      if(readkey(strsql) >= 1)
        rek[0].new_plus(nomkm());
     }
   }
  else
   rek[0].new_plus(nomkm());  
/******************
strcpy(strsql,"CREATE TABLE Uslugi (\
kodus   int not null default 0 primary key,\
kodgr   smallint not null default 0,\
naius   varchar(80) not null default '',\
ei      varchar(20) not null default '',\
shetu   varchar(20) not null default '' references Plansh (ns),\
cena    double(10,2) not null default 0,\
nds	real not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
art varchar(64) not null default '') ENGINE = MYISAM");


0 kodus   код услуги
1 kodgr   код группы
2 naius   наименование услуги
3 ei      единица измерения
4 shetu   счёт учета
5 cena    цена услуги
6 nds	0-НДС не применяется 1-применяется
7 ktoz	кто записал
8 vrem	время записи
9 art   артикул
*/
  sprintf(strsql,"insert into Uslugi (kodus,kodgr,naius,ei,shetu,cena,nds,ktoz,vrem,art) \
values (%d,%d,'%s','%s','%s',%.10g,%d,%d,%ld,'%s')",
  rek[0].ravno_atoi(),
  rek[1].ravno_atoi(),
  rek[2].ravno_filtr(),
  rek[3].ravno_filtr(),
  rek[4].ravno_filtr(),
  rek[5].ravno_atof(),
  rek[6].ravno_atoi(),  
  ktoi,
  time(NULL),
  rek[7].ravno_filtr());

  if(sql_zapis(strsql,1,0) != 0)
   {
    fprintf(filtmp.ff,"%s",strok);
    fprintf(filtmp.ff,"#%s Uslugi\n",gettext("Ошибка записи в таблицу"));
    kolihoh++;
    continue;
   }
 }

filtmp.end();
filimp.end();

unlink(imafim.ravno());

rename(imaftmp,imafim.ravno());

if(kolihoh == 0)
 iceb_t_soob(gettext("Все записи загружены"));
else
 {
  sprintf(strsql,"%s %d",gettext("Количество не загруженых записей"),kolihoh);
  iceb_t_soob(strsql);
 }

}



/****************************************/
/*меню*/
/******************************************/
void dirusl_f9(class SQLCURSOR *curs,class dirusl_poi *poi,class iceb_tu_str *naigrup)
{
int i=0;
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
int kom1=0;
class VVOD VVOD1(2);
class VVOD VV(0);
class VVOD DANET(1);
double krat=0.;
int kolstr1=0;

int kom=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное"));

MENU.VVOD_SPISOK_add_MD(gettext("Поиск по группе"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Переценка цен на заданный процент"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Импорт списка услуг из файла"));//2
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//3

for(;;)
 {

  if(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0)
   continue;
  class spis_oth oth;   
  switch (kom)
   {
    case 3:
    case -1 :
      return;

    case 0:
      helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
       
      poi->kgrm.new_plus("");
      naigrup->new_plus("");
       
      VVOD1.VVOD_delete();
      VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код или наименование группы услуг"));

      if((i=vvod1(&poi->kgrm,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
       {
        return;
       }
      if(i == FK10)
       {
        return;
       }

      if(poi->kgrm.getdlinna() <= 1 || isdigit(poi->kgrm.ravno()[0]) == 0)
       {
        clear();
        poi->kgrm.new_plus("");
        int poz=0,komv=0;
        if(dirmat1("Uslgrup",&poi->kgrm,naigrup,-1,0,3,1,&poz,&komv,"","\0") != 0)
         return;
       }

      /*Читаем код группы материалла*/
      sprintf(strsql,"select * from Uslgrup where kod=%s",poi->kgrm.ravno());

      if(sql_readkey(&bd,strsql,&row,&cur) != 1)
       {
        poi->kgrm.new_plus("");
        naigrup->new_plus("");

        sprintf(strsql,"%s %s !",gettext("Нет группы материалла"),poi->kgrm.ravno());
        iceb_t_soob(strsql);
        return;
       }
     naigrup->new_plus(row[1]);

     return;
     
    case 1:
      VV.VVOD_delete();
      VV.VVOD_SPISOK_add_ZAG(gettext("Переоценка всех цен на заданный процент"));

      VV.VVOD_SPISOK_add_MD(gettext("Процент.(+/-).."));
      VV.VVOD_SPISOK_add_MD(gettext("Группа (,,)...."));
      VV.VVOD_SPISOK_add_MD(gettext("Услуга   (,,).."));
      VV.VVOD_SPISOK_add_data(10);
      VV.VVOD_SPISOK_add_data(55);
      VV.VVOD_SPISOK_add_data(55);

  sfk9:;

      
          
      helstr(LINES-1,0,"F2/+",gettext("переценить"),
      "F10",gettext("выход"),NULL);
      
      kom1=VV.vvod(0,1,1,-1,-1);
      switch(kom1)
       {
        case FK10:
        case ESC:
          return;
        case FK2:
        case PLU:
          break;
        default:
         goto sfk9;
       }
      DANET.VVOD_delete();
      sprintf(strsql,
      gettext("Переценить на %s процентов ? Вы уверены ?"),
      VV.VVOD_SPISOK_return_data(0));
      DANET.VVOD_SPISOK_add_MD(strsql); 

      if(danet(&DANET,2,stdscr) == 2)
       return;
           
      krat=ATOFM(VV.VVOD_SPISOK_return_data(0));
      kolstr1=0;
      curs->poz_cursor(0);
      while(curs->read_cursor(&row) != 0)
       {
       
        strzag(LINES-1,0,poi->kolstr,++kolstr1);

        if(proverka(VV.VVOD_SPISOK_return_data(1),row[1],0,0) != 0)
         continue;
        if(proverka(VV.VVOD_SPISOK_return_data(2),row[0],0,0) != 0)
         continue;
        double cena=atof(row[5]);
        if(cena == 0.)
          continue;

        if(krat >= 0.)        
         cena=cena+cena*krat/100;
        else
         cena=cena+cena*krat/(100-krat);
         
        cena=okrug(cena,0.01);
                 
        sprintf(strsql,"update Uslugi \
set \
cena=%.2f,\
ktoz=%d,\
vrem=%ld \
where kodus=%s",cena,iceb_t_getuid(),time(NULL),row[0]);

        if(sql_zap(&bd,strsql) != 0)
         {
         if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
          {
           iceb_t_soob(gettext("У вас нет полномочий для выполнения этой операции !"));
           return;
          }
         else
          {
           msql_error(&bd,gettext("Ошибка записи !"),strsql);
          }
         }
       }

      break;

    case 2:
      dirusl_imp();
      return;
      
    default:
      kom=0;
      break;
   }
 }
}
