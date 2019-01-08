/*$Id: dirzarkh.c,v 5.5 2013/05/17 14:56:00 sasa Exp $*/
/*14.07.2015	09.12.2009	Белых А.И.	dirzarkh.c
Работа со списком карт-счетов работника предприятия
*/
#include "buhg.h"

int dirzarkh_vvod(int tabnom,const char *kodbn,const char *kartsh);
void dirzarkh_sap(int metkar);


int dirzarkh(int metkar, //0-работа 1-выбор из списка с корректировкой 2-чистый выбор
int tabnom,class iceb_tu_str *kod_banka,class iceb_tu_str *kartshet)
{
class iceb_tu_str naimb("");
class iceb_tu_str kodbn("");
class iceb_tu_str kartsh("");
class iceb_t_mstr spis_menu(0);
int		mspz=0;
int		pozz=0,pozz1=0;
char strsql[512];
class iceb_tu_str koment("");
int X=0,Y=3;
int kolstr=0;
int K=0;
int i=0;
int		prc;
int kom=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;

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


sprintf(strsql,"select * from Zarkh where tn=%d",tabnom);

  

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;
  dirzarkh_sap(metkar);
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одной записи !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

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
  naimb.new_plus("");
  sprintf(strsql,"select naik from Zarsb where kod=%s",row[1]);
  if(readkey(strsql,&row1,&cur1) == 1)
   naimb.new_plus(row1[0]);  
  
  sprintf(strsql,"%2s|%-20s|%s",
  row[1],row[2],naimb.ravno());
  

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

dirzarkh_sap(metkar);

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,Y,X,NULL,strsql,prc,&K,"",0);


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
//    if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[5],poisk.naimpoi.ravno()) == 0)
//     continue;
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
  //  if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[5],poisk.naimpoi.ravno()) == 0)
//     continue;
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
//     if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[5],poisk.naimpoi.ravno()) == 0)
//     continue;
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

  polen(spis_menu.spis_str.ravno(kom),&kodbn,1,'|');
  polen(spis_menu.spis_str.ravno(kom),&kartsh,2,'|');

 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     kod_banka->new_plus("");
     kartshet->new_plus("");
     return(1);
     break;

  case FK1:  /*Получить подсказку*/
    GDITE();
    iceb_t_pdoc("matu4_6.txt");
    goto naz;

  case ENTER:

    kartshet->new_plus(kartsh.ravno());
    kod_banka->new_plus(kodbn.ravno());
    if(metkar != 0)
      return(0);
      
  case SFK2:   /*Корректировать*/
    dirzarkh_vvod(tabnom,kodbn.ravno(),kartsh.ravno());
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:
    dirzarkh_vvod(tabnom,"","");
    goto naz1;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
//      if(dirzarkh_prov(kod_skl->ravno(),1) != 0)
//        goto naz;
      sprintf(strsql,"delete from Zarkh where tn=%d and kb=%d and nks='%s'",tabnom,kodbn.ravno_atoi(),kartsh.ravno());
      if(sql_zapis(strsql,0,0) == 0)
        goto naz1;
     }
    goto naz;


  default:
    goto naz;
    break;
 }

return(1);


}
/**************************/
/*Шапка                   */
/**************************/

void dirzarkh_sap(int metkar)
{
short		y=2;
short           x=COLS-20;



if(metkar != 2)
 {
  clear();
  printw(gettext("Ввод и корректировка списка картсчетов"));
 }
if(metkar == 1)
 {
  move(1,0);
  printw(gettext("Найдите нужное и нажмите \"Enter\""));
 }
                       
mvprintw(2,0,"   1            2          3");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("код банка"));
mvprintw(y++,x,"2-%s",gettext("картсчёт"));
mvprintw(y++,x,"3-%s",gettext("наим.банка"));


if(metkar < 2)
  helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F10",gettext("выход"),NULL);

if(metkar == 2)
  helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
  "F10",gettext("выход"),NULL);

}

/******************************/
/* Ввод и корректировка записи*/
/*******************************/

int dirzarkh_vvod(int tabnom,const char *kodbn,const char *kartsh)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
class iceb_tu_str kb("");
class iceb_tu_str naim("");
int kom=0;
VVOD VV(0);

VV.VVOD_SPISOK_add_data(10);//0
VV.VVOD_SPISOK_add_data(40);//1


if(kodbn[0] != '\0')
 {

  sprintf(strsql,"select * from Zarkh where tn=%d and kb=%s and nks='%s'",tabnom,kodbn,kartsh);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   return(1);   

  VV.VVOD_SPISOK_zapis_data(0,row[1]);
  VV.VVOD_SPISOK_zapis_data(1,row[2]);
   
  kzvz(row[3],row[4],&VV);     
 }



VV.VVOD_SPISOK_add_MD(gettext("Код банка.........."));//0
VV.VVOD_SPISOK_add_MD(gettext("Номер картсчёта...."));//1

naz:;

kom=VV.vvod(0,1,1,-1,-1);


switch(kom)
 {
  case FK1:   /*Помощь*/
//   GDITE();
//   iceb_t_pdoc("buhg5_1.txt"));
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;


  case FK3:

    if(dirsb(2,&kb,&naim) == 0)
      VV.data_plus(0,kb.ravno());

    goto naz;


  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

/*Запись****************/

if(VV.data_ravno_atoi(0) <= 0)
 {
  iceb_t_soob(gettext("Неправильно ввели код банка!"));
  goto naz;
 }

if(VV.data_ravno(1)[0] == '\0')
 {
  iceb_t_soob(gettext("Не ввели картсчёт!"));
  goto naz;
 }


sprintf(strsql,"select kod from Zarsb where kod=%d",VV.data_ravno_atoi(0));
if(readkey(strsql) != 1)
 {
  iceb_t_soob(gettext("Нет такого кода банка!"));
  goto naz;
 }



if(kodbn[0] == '\0') /*новая запись*/
 {
  sprintf(strsql,"select kb from Zarkh where tn=%d and kb=%d",tabnom,VV.data_ravno_atoi(0));
  if(readkey(strsql) > 0)
   {
    sprintf(strsql,gettext("Код банка %d уже есть!"),VV.data_ravno_atoi(0));
    iceb_t_soob(strsql);
    goto naz;
   }
  sprintf(strsql,"insert into Zarkh values(%d,%d,'%s',%d,%ld)",
  tabnom,VV.data_ravno_atoi(0),VV.data_ravno(1),iceb_t_getuid(),time(NULL));
 }
else
 {
  sprintf(strsql,"update Zarkh set \
kb=%d,\
nks='%s',\
ktoz=%d,\
vrem=%ld \
where tn=%d and kb=%s and nks='%s'",
  VV.data_ravno_atoi(0),
  VV.data_ravno(1),
  iceb_t_getuid(),
  time(NULL),
  tabnom,kodbn,kartsh);
       
 }

if(sql_zapis(strsql,0,0) != 0)
  goto naz; 

return(0);
}
