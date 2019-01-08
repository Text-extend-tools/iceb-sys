/*$Id: dirnalog.c,v 5.7 2013/05/17 14:55:58 sasa Exp $*/
/*13.07.2015	14.10.2010	Белых А.И.	dirnalog.c
Просмотр списка налогов на НДС
*/
#include <errno.h>
#include "buhg.h"

void dirnalog_sap(int);
int dirnalog_vz(int nomnal,short dz,short mz,short gz);


int dirnalog(int nomnal)
{
class iceb_t_mstr spis_menu(0);
char strsql[1024];
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
class SQLCURSOR cur;
short dz=0,mz=0,gz=0;

//Создаем класс меню
VVOD DANET(1);
VVOD VVOD1(2);

naz1:;


short           kls=LINES-6;

sprintf(strsql,"select * from Nalog where vn=%d order by dndn desc",nomnal);
  

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
lll:;

   
  dirnalog_sap(nomnal);

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

  rsdat(&dz,&mz,&gz,row[1],2);
  
  sprintf(strsql,"%02d.%02d.%04d|%8.2f|%s",
  dz,mz,gz,atof(row[2]),row[3]);  

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


dirnalog_sap(nomnal);

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
/****
    if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[5],poisk.naimpoi.ravno()) == 0)
     continue;
********/
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
/*******
    if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[5],poisk.naimpoi.ravno()) == 0)
     continue;
*******/
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
/*********
    if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[5],poisk.naimpoi.ravno()) == 0)
     continue;
**************/
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

  polen(str_vib.ravno(),strsql,sizeof(strsql),1,'|');
  rsdat(&dz,&mz,&gz,strsql,1);
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
    goto naz;

  case FK2:  
    dirnalog_vz(0,0,0,0);
    goto naz1;

  case ENTER: 
  case SFK2: 
    
    dirnalog_vz(0,dz,mz,gz);
    goto naz1;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 2)
      goto naz;
    sprintf(strsql,"delete from Nalog where vn=%d and dndn='%04d-%02d-%02d'",nomnal,gz,mz,dz);
    sql_zapis(strsql,1,0);
    goto naz1;


  default:
    goto naz;
    break;
 }

return(1);

}
/*********/
/*Шапка*/
/*********/
void dirnalog_sap(int nomnal)
{
short		y=2;
short           x=COLS-20;

clear();
if(nomnal == 0)
  printw(gettext("Налог на добавочную стоимость"));



mvprintw(2,0,"      1          2     3 ");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("дата"));
mvprintw(y++,x,"2-%s",gettext("процент"));
mvprintw(y++,x,"3-%s",gettext("коментарий"));


helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
//"F5",gettext("настройка"),
"F10",gettext("выход"),NULL);
 

}
/***********************/
/*Ввод и корректировка записи*/
/*****************************/

int dirnalog_vz(int nomnal,short dz,short mz,short gz)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
class iceb_tu_str dndn(""); /*дата начала действия налога*/
class iceb_tu_str proc("");
class iceb_tu_str koment("");
short dv=0,mv=0,gv=0;
int kom=0;

VVOD VV(0);

if(dz != 0) /*Корректировка записи*/
 {
  sprintf(strsql,"select * from Nalog where vn=%d and dndn='%04d-%02d-%02d'",nomnal,gz,mz,dz);
  if(readkey(strsql,&row,&cur) != 1)
   {
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
    return(1);
   }
  sprintf(strsql,"%02d.%02d.%04d",dz,mz,gz);
  dndn.new_plus(strsql);
  proc.new_plus(row[2]);
  koment.new_plus(row[3]);
  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));
  kzvz(row[4],row[5],&VV);
 }
else
  VV.VVOD_SPISOK_add_ZAG(gettext("Ввод новой записи"));



VV.VVOD_SPISOK_add_MD(gettext("Дата начала.действия (д.м.г).."));
VV.VVOD_SPISOK_add_MD(gettext("Процент......................."));
VV.VVOD_SPISOK_add_MD(gettext("Коментарий...................."));

VV.VVOD_SPISOK_add_data(dndn.ravno(),11);
VV.VVOD_SPISOK_add_data(proc.ravno(),11);
VV.VVOD_SPISOK_add_data(koment.ravno(),255);

naz:;

 
helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

dndn.new_plus(VV.VVOD_SPISOK_return_data(0));
proc.new_plus(VV.VVOD_SPISOK_return_data(1));
koment.new_plus(VV.VVOD_SPISOK_return_data(2));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   iceb_t_pdoc("buhg5_1.txt");
   goto naz;

  case ESC:
  case FK10:
   return(1);

  case FK2:
  case PLU:
     break;


  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }

if(rsdat(&dv,&mv,&gv,dndn.ravno(),1) != 0)
 {
  iceb_t_soob(gettext("Неправильно введена дата!"));
  goto naz;
 }

if(sravmydat(dz,mz,gz,dv,mv,gv) != 0)
 {
  /*Проверяем может уже есть такая запись*/
  sprintf(strsql,"select vn from Nalog where vn=%d and dndn='%04d-%02d-%02d'",nomnal,gv,mv,dv);

  if(readkey(strsql) == 1)
   {
    iceb_t_soob(gettext("Такая запись уже есть!"));
    goto naz;
   }
 }

if(dz == 0) /*Ввод новой записи*/
 {
  sprintf(strsql,"insert into Nalog values (%d,'%04d-%02d-%02d',%.2f,'%s',%d,%ld)",
  nomnal,gv,mv,dv,proc.ravno_atof(),koment.ravno_filtr(),iceb_t_getuid(),time(NULL));
  
 }
else /*Корректировка существующей*/
 {
  sprintf(strsql,"update Nalog set \
dndn='%04d-%02d-%02d',\
pr=%.2f,\
kom='%s', \
ktoz=%d, \
vrem=%ld \
where vn=%d and dndn='%04d-%02d-%02d'",
  gv,mv,dv,
  proc.ravno_atof(),
  koment.ravno_filtr(),
  iceb_t_getuid(),
  time(NULL),
  nomnal,
  gz,mz,dz); 
 }

sql_zapis(strsql,1,0);

return(0);

}
