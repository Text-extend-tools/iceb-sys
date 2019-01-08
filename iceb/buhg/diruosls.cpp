/*$Id: diruosls.c,v 5.3 2013/05/17 14:55:59 sasa Exp $*/
/*18.03.2011	18.03.2011	Белых А.И.	diruosls.c
Просмотр списка ликвидационной стоимости к основному средству
*/
#include "buhg.h"

void diruosls_sap(int innom,const char *naimuos);
int diruosls_vk(int innom,short dls,short mls,short gls);

void diruosls(int innom)
{
char		strsql[1024];
class iceb_t_mstr spis_menu(0);
SQL_str		row;
int		kolstr=0;
int		K=0;
int		pozz=0,pozz1=0;
int		i=0,kom=0;
int		mspz=0;
int		prc;
int             Y=3;
int             X=0;
short dls,mls,gls;
class iceb_tu_str naimuos("");
class SQLCURSOR cur;

sprintf(strsql,"select naim from Uosin where innom=%d",innom);
if(readkey(strsql,&row,&cur) == 1)
 naimuos.new_plus(row[0]);

//Создаем класс меню
VVOD DANET(1);
VVOD VVOD1(2);

naz1:;

short           kls=LINES-6;


sprintf(strsql,"select * from Uosls where inn=%d",innom);

  

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
lll:;

  diruosls_sap(innom,naimuos.ravno());
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
  
  sprintf(strsql,"%s|%10s|%10s|%s",
  iceb_tu_datzap(row[1]),
  row[2],
  row[3],
  row[4]);
    

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

diruosls_sap(innom,naimuos.ravno());

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
//    if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[5],poisk.naimpoi.ravno()) == 0)
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
//    if(poisk.naimpoi.ravno()[0] != '\0' && strstrm(row[5],poisk.naimpoi.ravno()) == 0)
//     continue;
    i++;
   }
  goto naz;
 }

if(kom >= 0)
 {
  class iceb_tu_str str_out(spis_menu.spis_str.ravno(kom));
  if(SRAV(gettext("Конец списка"),str_out.ravno(),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz; /*Стали на конец списка*/


  polen(str_out.ravno(),strsql,sizeof(strsql),1,'|');
  rsdat(&dls,&mls,&gls,strsql,1);
 }

vper:;

switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
     return;
     break;

  case FK1:  /*Получить подсказку*/
    GDITE();
    goto naz;

  case ENTER:
      
  case SFK2:   /*Корректировать*/
    diruosls_vk(innom,dls,mls,gls);
    goto naz1;

  case FK2:  /*Ввести запись*/
  case PLU:
    diruosls_vk(innom,0,0,0);
    goto naz1;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      sprintf(strsql,"delete from Uosls where inn=%d and data='%04d-%02d-%02d'",innom,gls,mls,dls);
      if(sql_zapis(strsql,0,0) == 0)
        goto naz1;
     }
    goto naz;


  default:
    goto naz;
    break;
 }

}
/**************************************/
/*Шапка*/
/**************************************/
void diruosls_sap(int innom,const char *naimuos)
{
short		y=2;
short           x=COLS-20;

clear();
printw("%s\n%d %s",gettext("Ввод и корректировка ликвидационной стоимости"),innom,naimuos);

mvprintw(2,0,"      1           2         3      4");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("дата"));
mvprintw(y++,x,"2-%s",gettext("лик.стоим. нал.у."));
mvprintw(y++,x,"3-%s",gettext("лик.стоим. бух.у."));
mvprintw(y++,x,"4-%s",gettext("коментарий"));

helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F3",gettext("удалить"),
"F10",gettext("выход"),NULL);

}
/*****************************************/
/*Ввод и корректировка*/
/*********************************/
int diruosls_vk(int innom,short dls,short mls,short gls)
{
char strsql[512];
int kom=0;
VVOD VV(0);

VV.VVOD_SPISOK_add_data(11);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(255);

if(dls == 0)
 VV.VVOD_SPISOK_add_ZAG(gettext("Ввод записи"));
else
 {
  SQL_str row;
  class SQLCURSOR cur;
  VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка записи"));
  sprintf(strsql,"select * from Uosls where inn=%d and data='%04d-%02d-%02d'",innom,gls,mls,dls);
  if(readkey(strsql,&row,&cur) != 1)
   {
    iceb_t_soob(gettext("Не найдена запись для корректировки!"));
    return(1);
   }
  VV.data_plus(0,iceb_tu_datzap(row[1]));
  VV.data_plus(1,row[2]);
  VV.data_plus(2,row[3]);
  VV.data_plus(3,row[4]);
  kzvz(row[5],row[6],&VV);
  
 }


VV.VVOD_SPISOK_add_MD(gettext("Дата........(д.м.г)..............................."));
VV.VVOD_SPISOK_add_MD(gettext("Ликвидационная стоимость для налогового учёта....."));
VV.VVOD_SPISOK_add_MD(gettext("Ликвидационная стоимость для бухгалтеского учёта.."));
VV.VVOD_SPISOK_add_MD(gettext("Коментарий........................................"));


naz:;

helstr(LINES-1,0,"F1",gettext("помощь"),
"F2/+",gettext("расчитать"),
"F3",gettext("реквизиты"),
"F4",gettext("очистить"),
"F5",gettext("настройка"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);


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

if(VV.prov_dat(0,0) != 0)
 {
  iceb_t_soob(gettext("Не правильно введена дата!"));
  goto naz;
 }

if(dls == 0) /*Ввод новой записи*/
 {
  sprintf(strsql,"insert into Uosls values(%d,'%s',%.2f,%.2f,'%s',%d,%ld)",
  innom,
  VV.ravno_sqldat(0),
  VV.data_ravno_atof(1),
  VV.data_ravno_atof(2),
  VV.data_ravno_filtr(3),
  iceb_t_getuid(),
  time(NULL));
 }
else
 {
  sprintf(strsql,"update Uosls set \
data='%s',\
lsnu=%.2f,\
lsbu=%.2f,\
kom='%s',\
ktoz=%d,\
vrem=%ld \
where inn=%d and data='%04d-%02d-%02d'",
  VV.ravno_sqldat(0),
  VV.data_ravno_atof(1),
  VV.data_ravno_atof(2),
  VV.data_ravno_filtr(3),
  iceb_t_getuid(),
  time(NULL),
  innom,
  gls,mls,dls);

 }



if(sql_zapis(strsql,1,1) != 0)
 goto naz;
 
return(0);

}
