/*$Id: dirout.c,v 5.48 2013/09/26 09:43:30 sasa Exp $*/
/*21.04.2017	23.03.2003	Белых А.И.	dirout.c
Воод и корректировка объектов учета остатков топлива (заправки и водители)
*/
#include        <errno.h>
#include        "buhg.h"

void            vvodout(const char*);
void            sapuot(int,const char*);
int             zapout(VVOD*,const char*,int,const char*,const char*);
int             provout(const char*);
void            rasout(SQLCURSOR*);
void            korout(const char*,const char*);

int dirout(class iceb_tu_str *kout,class iceb_tu_str *naim,
int metkar, //0-работа 1-выбор из списка с корректировкой 2-чистый выбор
int metkaz) //0-все 1-толко заправки 2-толко водителей
{
class iceb_t_mstr spis_menu(0);
char		strsql[512];
SQL_str		row;
int		kolstr=0;
int		K=0;
int		pozz=0,pozz1=0;
int		i=0,kom=0;
int		mspz=0;
int		prc;
int             Y=3;
int             X=0;
class iceb_tu_str koment("");
class iceb_tu_str vid("");
class iceb_tu_str naimpoi(naim->ravno());

SQLCURSOR curr;

if(metkar == 2)
 {
  Y=-1;
  X=-1;
  koment.new_plus(gettext("Выберите нужное и нажмите Enter"));
 }

//Создаем класс меню
VVOD DANET(1);
VVOD VVOD1(2);

naz1:;

short           kls=LINES-6;


if(metkaz == 0)
  sprintf(strsql,"select * from Uplouot order by kod asc");
if(metkaz == 1)
  sprintf(strsql,"select * from Uplouot where mt=0 order by kod asc");
if(metkaz == 2)
  sprintf(strsql,"select * from Uplouot where mt=1 order by kod asc");

//printw("\nstrsql=%s\n",strsql);
//OSTANOV();

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
if(kolstr == 0)
 {
lll:;
  sapuot(metkar,naimpoi.ravno());
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == SFK2 || K == ENTER || K == FK3 || K == FK5)
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

  if(naimpoi.ravno()[0] != '\0' && strstrm(row[3],naimpoi.ravno()) == 0)
   continue;
       
  if(row[1][0] == '0')
   vid.new_plus("Z");
  else
   vid.new_plus("V");

  sprintf(strsql,"%-3s|%s|%-3s|%-*s|%s",
  row[0],vid.ravno(),row[2],
  iceb_tu_kolbait(6,row[4]),row[4],
  row[3]);

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

sapuot(metkar,naimpoi.ravno());

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,Y,X,NULL,strsql,prc,&K,koment.ravno(),0);


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

    if(naimpoi.ravno()[0] != '\0' && strstrm(row[3],naimpoi.ravno()) == 0)
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

    if(naimpoi.ravno()[0] != '\0' && strstrm(row[3],naimpoi.ravno()) == 0)
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
/*  
  printw("\n вперед pozz=%d kolstr=%ld\n",pozz,kolstr);
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

    if(naimpoi.ravno()[0] != '\0' && strstrm(row[3],naimpoi.ravno()) == 0)
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

  polen(spis_menu.spis_str.ravno(kom),kout,1,'|');
  polen(spis_menu.spis_str.ravno(kom),naim,5,'|');

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
    iceb_t_pdoc("upl3_3.txt");
    goto naz;

  case ENTER:
    if(metkar != 0)
      return(0);
      
  case SFK2:   /*Корректировать*/
    vvodout(kout->ravno());
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:
    vvodout("");
    goto naz1;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      if(provout(kout->ravno()) != 0)
        goto naz;
      sprintf(strsql,"delete from Uplouot where kod=%s",kout->ravno());
      if(sql_zapis(strsql,0,0) == 0)
        goto naz1;
     }
    goto naz;

  case FK4:       /*Поиск*/

     helstr(LINES-1,0," F10 ",gettext("выход"),NULL);

     VVOD1.VVOD_delete();
     VVOD1.VVOD_SPISOK_add_MD(gettext("Введите образец для поиска"));
     
     naimpoi.new_plus("");
     
     vvod1(&naimpoi,112,&VVOD1,NULL,stdscr,-1,-1);

     GDITE();

     clear();
     kom=pozz=0;
     goto naz;

  case FK5:    //Распечатать
    rasout(&cur);
    goto naz;

  default:
    goto naz;
    break;
 }

return(1);


}
/**********************/
/*Ввод и корректировки*/
/**********************/

void	vvodout(const char *kodout)
{
char		strsql[512];
SQL_str		row;
time_t		vremz=0;
int		ktozap=0;
struct  tm      *bf;
int		N=1,K=0;
int             kom1=0;
int		vid=0;
class iceb_tu_str naimskl("");
class iceb_tu_str naimkont("");
class iceb_tu_str kodskl("");
class iceb_tu_str kodkont("");
class iceb_tu_str kod("");
class iceb_tu_str kods("");
class iceb_tu_str naim("");
SQLCURSOR curr;

VVOD VV(0);

VV.VVOD_SPISOK_add_data(10);//0
VV.VVOD_SPISOK_add_data(20);//1
VV.VVOD_SPISOK_add_data(80);//2
VV.VVOD_SPISOK_add_data(10);//3
VV.VVOD_SPISOK_add_data(10);//4


if(kodout[0] != '\0')
 {

  sprintf(strsql,"select * from Uplouot where kod=%s",kodout);
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   return;   

  VV.VVOD_SPISOK_zapis_data(0,row[0]);

  if(row[1][0] == '0')
   { 
    VV.VVOD_SPISOK_zapis_data(1,gettext("Заправка"));
    vid=0;
   }

  if(row[1][0] == '1')
   {
    VV.VVOD_SPISOK_zapis_data(1,gettext("Водитель"));
    vid=1;
   }

  VV.VVOD_SPISOK_zapis_data(2,row[3]);
  VV.VVOD_SPISOK_zapis_data(3,row[2]);
  VV.VVOD_SPISOK_zapis_data(4,row[4]);
  ktozap=atoi(row[5]);
  vremz=atol(row[6]);

  sprintf(strsql,"select naik from Sklad where kod=%s",VV.VVOD_SPISOK_return_data(3));
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    sprintf(strsql,gettext("Не найден склад %d в списке складов !"),atoi(VV.VVOD_SPISOK_return_data(3)));
    iceb_t_soob(strsql);
   }
  else
    naimskl.new_plus(row[0]);

  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",VV.VVOD_SPISOK_return_data(4));
  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),VV.VVOD_SPISOK_return_data(4));
    iceb_t_soob(strsql);
   }
  else
    naimkont.new_plus(row[0]);
  kodskl.new_plus(VV.VVOD_SPISOK_return_data(3));
  kodkont.new_plus(VV.VVOD_SPISOK_return_data(4));

 }
else
 {
  sprintf(strsql,"%d",oprsk("Uplouot"));
  VV.VVOD_SPISOK_zapis_data(0,strsql);
 }


VV.VVOD_SPISOK_add_MD(gettext("Код объекта..........."));//0
VV.VVOD_SPISOK_add_MD(gettext("Вид объекта..........."));//1
VV.VVOD_SPISOK_add_MD(gettext("Наименование объекта.."));//2
VV.VVOD_SPISOK_add_MD(gettext("Код склада............"));//3
VV.VVOD_SPISOK_add_MD(gettext("Код контрагента......."));//4


naz:;

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("реквизиты"),
"F10",gettext("выход"),NULL);

VV.VVOD_delete_ZAG();

if(kodout[0] != '\0')
  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));
else
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));

if(naimskl.ravno()[0] != '\0')
 {
  sprintf(strsql,"%s:%s",gettext("Склад"),naimskl.ravno());
  VV.VVOD_SPISOK_add_ZAG(strsql);
 }

if(naimkont.ravno()[0] != '\0')
 {
  sprintf(strsql,"%s:%s",gettext("Контрагент"),naimkont.ravno());
  VV.VVOD_SPISOK_add_ZAG(strsql);
 }

if(ktozap != 0)
 {
  VV.VVOD_SPISOK_add_ZAG(iceb_t_kszap(ktozap));
 }

if(vremz != 0)
 {
  bf=localtime(&vremz);
  sprintf(strsql,"%s: %d.%d.%d%s %s:%02d:%02d:%02d",
  gettext("Дата записи"),
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
  gettext("г."),
  gettext("Время"),
  bf->tm_hour,bf->tm_min,bf->tm_sec);
  VV.VVOD_SPISOK_add_ZAG(strsql);
 }


VV.vramka(2,0,0);
for(;;)
 {

  attron(VV.VVOD_return_cs(iceb_CFS)); 
  VV.vprintw(N,stdscr);
  K=getch();


  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

  if(K == KEY_RESIZE)
   {
//    getch();
    goto naz;
   }


  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == ENTER && N == 1)
   {
    VVOD MENUW(3);
    MENUW.VVOD_SPISOK_add_MD(gettext("Водитель"));
    MENUW.VVOD_SPISOK_add_MD(gettext("Заправка"));

    MENUW.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное и нажмите Enter"));
    kom1=0;
    if(menu3w(stdscr,&MENUW,&kom1,-1,-1,0) != 0)
     goto naz;
    switch (kom1)
     {
      case -1 :
        break;

      case 0 :
       VV.VVOD_SPISOK_zapis_data(1,gettext("Водитель"));
       vid=1;
       break;

      case 1 :
       VV.VVOD_SPISOK_zapis_data(1,gettext("Заправка"));
       vid=0;
       break;

     }
    N++;    
    goto naz;

   }
   
  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));
    VV.vprintw_clear(N,stdscr);

//    mygetstr(VV.VVOD_SPISOK_return_data(N),VV.VVOD_SPISOK_return_dls(N),stdscr);
    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 3 && VV.VVOD_SPISOK_return_data(N)[0] != '\0' )  //Склад
     {
      naimskl.new_plus("");
      sprintf(strsql,"select naik from Sklad where kod=%s",VV.VVOD_SPISOK_return_data(N));
      if(sql_readkey(&bd,strsql,&row,&curr) != 1)
       {
        sprintf(strsql,gettext("Не найден склад %d в списке складов !"),atoi(VV.VVOD_SPISOK_return_data(N)));
        iceb_t_soob(strsql);
        goto naz;
       }
      else
        naimskl.new_plus(row[0]);

      N++;
      goto naz;
     }

    if(N == 4 && VV.VVOD_SPISOK_return_data(N)[0] != '\0' )  //код контрагента
     {
      naimkont.new_plus("");
      sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",VV.VVOD_SPISOK_return_data(N));
      if(sql_readkey(&bd,strsql,&row,&curr) != 1)
       {
        sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),VV.VVOD_SPISOK_return_data(N));
        iceb_t_soob(strsql);
       }
      else
        naimkont.new_plus(row[0]);

      if(SRAV(kodkont.ravno(),VV.VVOD_SPISOK_return_data(N),0) != 0)
       {
        //Проверяем может этот код контрагента уже введён для другого водителя
//        sprintf(strsql,"select kod,naik from Uplouot where kontr='%s'",VV.VVOD_SPISOK_return_data(N));
        sprintf(strsql,"select kod,naik from Uplouot where kontr='%s' and mt=%d",VV.VVOD_SPISOK_return_data(N),vid);

        if(sql_readkey(&bd,strsql,&row,&curr) > 0)
         {
          VVOD SOOB(1);
          SOOB.VVOD_SPISOK_add_MD(gettext("Этот код контрагента уже введён для другого объекта !"));
          sprintf(strsql,"%s %s",row[0],row[1]);
          SOOB.VVOD_SPISOK_add_MD(strsql);
          soobshw(&SOOB,stdscr,-1,-1,0,1);
          goto naz;
         }
       }

      goto naz;
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
    N=-1;
    break;
   }
  if(K == FK1) /*Помощь*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    GDITE();
    iceb_t_pdoc("upl3_3_1.txt");
    clear();
    goto naz;
   }

  if(K == FK2 || K == PLU) /*Записываем*/
   {
    if(zapout(&VV,kodout,vid,kodskl.ravno(),kodkont.ravno()) != 0)
     goto naz;
    break;
   }

  if(K == FK3) //Реквизиты
   {
    attroff(VV.VVOD_return_cs(iceb_CFS)); 
    VVOD MENU(3);

//    MENU.VVOD_delete();
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка складов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Просмотр списка контрагентов"));
    MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

    clearstr(LINES-1,0);
    kom1=0;    
    while(menu3w(stdscr,&MENU,&kom1,-1,-1,0) != 0);

    switch (kom1)
     {
      case 2:
      case -1:
       break;

      case 0:
        kods.new_plus("");
        naim.new_plus("");
        if(dirsklad(1,&kods,&naim) == 0)
          VV.data_z_plus(3,kods.ravno());
                 
        break; 

      case 1:
        kod.new_plus("");
        if(vibrek("Kontragent",&kod,&naimkont) == 0)
          VV.VVOD_SPISOK_zapis_data(4,kod.ravno());
        break; 
     }
    goto naz;
   }
   

 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();
return;
}

/*********************/
/*Запись             */
/*********************/
//Если вернули 0 - записали 1-нет

int   zapout(VVOD *VV,const char *kodout,int vid,const char *kodskl,const char *kodkont)
{
char	strsql[1024];
SQL_str row=NULL;
class SQLCURSOR cur;

attroff(VV->VVOD_return_cs(iceb_CFS));
attron(VV->VVOD_return_cs(iceb_CFM));

if(VV->VVOD_SPISOK_return_data(0)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(1)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(2)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(3)[0] == '\0' ||\
   VV->VVOD_SPISOK_return_data(4)[0] == '\0')
 {
  iceb_t_soob(gettext("Не введены все обязательные реквизиты !"));
  return(1);
 }       

//Проверяем есть ли такой склад

sprintf(strsql,"select kod from Sklad where kod=%s",VV->VVOD_SPISOK_return_data(3));
if(sql_readkey(&bd,strsql) != 1)
 {

  sprintf(strsql,gettext("Не найден склад %s в подсистеме \"Материальный учет\" !"),VV->VVOD_SPISOK_return_data(3));

  iceb_t_soob(strsql);
  return(1);
 }

//Проверяем есть ли такой код контрагента

sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",VV->VVOD_SPISOK_return_data(4));
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),VV->VVOD_SPISOK_return_data(4));

  iceb_t_soob(strsql);
  return(1);
 }


if(SRAV(kodkont,VV->VVOD_SPISOK_return_data(4),0) != 0)
 {
  //Проверяем может этот код контрагента уже введён для другого водителя
  sprintf(strsql,"select kod,naik from Uplouot where kontr='%s' and mt=%d",
  VV->VVOD_SPISOK_return_data(4),vid);
  if(sql_readkey(&bd,strsql) > 0)
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    SOOB.VVOD_SPISOK_add_MD(gettext("Этот код контрагента уже введён для другого объекта!"));
    sprintf(strsql,"%s %s",row[0],row[1]);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    return(1);
   }
 }


if(SRAV(kodout,VV->VVOD_SPISOK_return_data(0),0) != 0)
 {
  sprintf(strsql,"select kod from Uplouot where kod=%s",VV->VVOD_SPISOK_return_data(0));
  if(sql_readkey(&bd,strsql) > 0)
   {
    iceb_t_soob(gettext("Такая запись уже есть !"));
    return(1);
   }
  if(kodout[0] != '\0')
   {
    VVOD DANET(1);
    DANET.VVOD_SPISOK_add_MD(gettext("Корректировать код ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      return(1);
   }

 }

time_t vrem;
time(&vrem);

if(kodout[0] == '\0') //Ввод новой записи
  sprintf(strsql,"insert into Uplouot values(%s,%d,%s,'%s','%s',%d,%ld)",
  VV->VVOD_SPISOK_return_data(0),  
  vid,
  VV->VVOD_SPISOK_return_data(3),  
  VV->VVOD_SPISOK_return_data(2),
  VV->VVOD_SPISOK_return_data(4),
  iceb_t_getuid(),vrem);
else
  sprintf(strsql,"replace into Uplouot values(%s,%d,%s,'%s','%s',%d,%ld)",
  VV->VVOD_SPISOK_return_data(0),  
  vid,
  VV->VVOD_SPISOK_return_data(3),  
  VV->VVOD_SPISOK_return_data(2),
  VV->VVOD_SPISOK_return_data(4),
  iceb_t_getuid(),vrem);

if(sql_zapis(strsql,0,0) != 0)
 return(1);

if(kodout[0] != '\0' && SRAV(kodout,VV->VVOD_SPISOK_return_data(0),0) != 0)
 korout(kodout,VV->VVOD_SPISOK_return_data(0)); //Корректировка кода таблицах


return(0);


}


/**************************/
/*Шапка                   */
/**************************/

void	sapuot(int metkar,const char *naimpoi)
{
short		y=2;
short           x=COLS-20;



if(metkar != 2)
 {
  clear();
  printw(gettext("Ввод и корректировка объектов остатков топлива"));
 }
if(metkar == 1)
 {
  move(1,0);
  printw(gettext("Найдите нужное и нажмите \"Enter\""));
 }

mvprintw(2,0,"   1  2  3     4     5");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,gettext("1-код объекта"));
mvprintw(y++,x,gettext("2-вид объекта"));
mvprintw(y++,x,gettext("  Z-заправка"));
mvprintw(y++,x,gettext("  V-водитель"));
mvprintw(y++,x,gettext("3-код склада"));
mvprintw(y++,x,gettext("4-контрагент"));
mvprintw(y++,x,gettext("5-наименование"));

if(naimpoi[0] != '\0')
 {
  y++;
  attron(A_BLINK);
  mvprintw(y++,x,gettext("Поиск:"));
  mvprintw(y++,x,"%s",naimpoi);
  attroff(A_BLINK);
  
 }

if(metkar < 2)
  helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F10",gettext("выход"),NULL);

if(metkar == 2)
  helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
  "F10",gettext("выход"),NULL);

}

/********************************/
/*Проверка возможности удаления*/
/********************************/
//Если вернули 0-можно удалять 1-нет
int provout(const char *kod)
{
char	strsql[512];

sprintf(strsql,"select kv from Upldok where kv=%s limit 1",kod);
if(readkey(strsql) > 0)
 {
  iceb_t_soob(gettext("Запись удалить нельзя потому, что есть документы с этим топливом !"));
  return(1);
 }

sprintf(strsql,"select kv from Upldok1 where kodsp=%s limit 1",kod);
if(readkey(strsql) > 0)
 {
  iceb_t_soob(gettext("Запись удалить нельзя потому, что есть документы с этим топливом !"));
  return(1);
 }

sprintf(strsql,"select kv from Uplsal where kv=%s limit 1",kod);
if(readkey(strsql) > 0)
 {
  iceb_t_soob(gettext("Запись удалить нельзя потому, что есть сальдо с этим топливом !"));
  return(1);
 }


sprintf(strsql,"select kzv from Uplavt where kzv=%s limit 1",kod);
if(readkey(strsql) > 0)
 {
  iceb_t_soob(gettext("Запись удалить нельзя потому, что есть автомобиль с этим кодом водителя!"));
  return(1);
 }

return(0);


}
/********************/
/*Распечатать       */
/********************/

void	rasout(SQLCURSOR *cur)
{
char	imaf[64];
FILE	*ff;
SQL_str row;
char    vid[2];

sprintf(imaf,"komt%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }


iceb_t_zagolov(gettext("Распечатка объектов учета топлива"),0,0,0,0,0,0,ff);


fprintf(ff,"\
--------------------------------------\n");
fprintf(ff,gettext("\
Код |Метка|Склад|Контр.|Наименование\n"));
fprintf(ff,"\
--------------------------------------\n");

cur->poz_cursor(0);

while(cur->read_cursor(&row) != 0)
 {
  if(row[1][0] == '0')
   strcpy(vid,"Z");
  else
   strcpy(vid,"V");
  fprintf(ff,"%-4s %-5s %-5s %-*s %s\n",
  row[0],vid,row[2],
  iceb_tu_kolbait(6,row[4]),row[4],row[3]);
 }
fprintf(ff,"\
--------------------------------------\n");
fprintf(ff,"%s\n",gettext("  Z-заправка"));
fprintf(ff,"%s\n",gettext("  V-водитель"));

podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка объектов учета топлива"));
iceb_t_ustpeh(imaf,3);
iceb_t_rabfil(&oth,"");
}
/**************************/
/*Корректировка кода      */
/**************************/
void  korout(const char *kods,const char *kodn)
{
char		strsql[512];

VVOD SOOBN(1);
SOOBN.VVOD_SPISOK_add_MD(gettext("Выполняется каскадная корректировка кода в таблицах !"));
soobsh(&SOOBN,-1,-1,1);

GDITE();

sprintf(strsql,"update Upldok set kv=%s where kv=%s",kodn,kods);
if(sql_zap(&bd,strsql) != 0)
 {
  msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
  return;
 }

sprintf(strsql,"update Upldok1 set kv=%s where kv=%s",kodn,kods);
if(sql_zap(&bd,strsql) != 0)
 {
  msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
  return;
 }

sprintf(strsql,"update Uplsal set kv=%s where kv=%s",kodn,kods);
if(sql_zap(&bd,strsql) != 0)
 {
  msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
  return;
 }

sprintf(strsql,"update Uplavt set kzv='%s' where kzv=%s",kodn,kods);
if(sql_zap(&bd,strsql) != 0)
 {
  msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql);
  return;
 }

//Удаляем запись которую откорректировали
sprintf(strsql,"delete from Uplouot where kod=%s",kods);
sql_zapis(strsql,1,0);

}
