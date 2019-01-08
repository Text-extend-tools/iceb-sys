/*$Id: diralx.c,v 5.7 2013/05/17 14:55:57 sasa Exp $*/
/*13.07.2015	30.10.2010	Белых А.И.	diralx.c
Просмотр списка настроечных файлов
*/
#include <errno.h>
#include "buhg.h"

void diralx_sap(const char *imabazv,const char *naimpoi,const char *editor);
void diralx_sr(class iceb_tu_str *editor);
void diralx_rn(SQLCURSOR *cur,const char *vibfil);


void diralx(const char *imabazv)
{
char strsql[2048];
SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kolstr=0;
class iceb_tu_str naimpoi("");
int K=0;
int pozz=0,pozz1=0;
int i=0,kom=0;
int mspz=0;
int prc;
int Y=3;
int X=0;
class iceb_t_mstr spis_menu(0);
class iceb_tu_str imafil("");
class VVOD DANET(1);
class iceb_tu_str editor("");

class VVOD VVOD1(2);
memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"USE %s",imabazv);
sql_zapis(strsql,1,0);

naz1:;

short           kls=LINES-6;

sprintf(strsql,"select distinct fil from Alx order by fil asc");



if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
lll:;
  diralx_sap(imabazv,naimpoi.ravno(),editor.ravno());
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

  if(naimpoi.getdlinna() > 1 && strstrm(row[0],naimpoi.ravno()) == 0)
   continue;

  /*Читаем время и кто записал*/
  sprintf(strsql,"select ktoz,vrem from Alx where fil='%s' order by ns asc limit 1",row[0]);
  readkey(strsql,&row1,&cur1);    

  sprintf(strsql,"%-*s|%s|%s",iceb_tu_kolbait(20,row[0]),row[0],iceb_tu_vremzap(row1[1]),iceb_t_kszap(row1[0]));
  
  i++;
  spis_menu.spis_str.plus(strsql);
  
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

diralx_sap(imabazv,naimpoi.ravno(),editor.ravno());

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));

kom=spis_menu.menu(kom,Y,X,NULL,strsql,prc,&K,"",COLS-21);


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
    if(naimpoi.getdlinna() > 1 && strstrm(row[0],naimpoi.ravno()) == 0)
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
    if(naimpoi.getdlinna() > 1 && strstrm(row[0],naimpoi.ravno()) == 0)
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
    if(naimpoi.getdlinna() > 1 && strstrm(row[0],naimpoi.ravno()) == 0)
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

  polen(spis_menu.spis_str.ravno(kom),&imafil,1,'|');

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
//    iceb_t_pdoc("uos5_10.txt");
    goto naz;


  case FK2:  /*Ввести запись*/
  case ENTER:
  case PLU:
    iceb_redfil(imafil.ravno(),1,editor.ravno());
    goto naz1;

  case FK3:       /*Удалить запись*/

    DANET.VVOD_delete();
    DANET.VVOD_SPISOK_add_MD(gettext("Удалить ? Вы уверены ?"));

    if(danet(&DANET,2,stdscr) == 1)
     {
      class VVOD PAR(2);
      PAR.VVOD_SPISOK_add_MD(gettext("Введите пароль"));
      if(parolv(&PAR,0,0,0,0) != 0)
       goto naz;
      memset(strsql,'\0',sizeof(strsql));
      sprintf(strsql,"delete from Alx where fil='%s'",imafil.ravno());
      if(sql_zapis(strsql,0,0) == 0)
        goto naz1;
     }
    goto naz;

  case FK4:       /*Поиск*/

     helstr(LINES-1,0," F10 ",gettext("выход"),NULL);

     VVOD1.VVOD_delete();
     VVOD1.VVOD_SPISOK_add_MD(gettext("Введите образец для поиска"));
     
     naimpoi.new_plus("");
     
     vvod1(&naimpoi,40,&VVOD1,NULL,stdscr,-1,-1);

     GDITE();

     clear();
     kom=pozz=0;
     goto naz;

  case FK5:    //Распечатать
    diralx_rn(&cur,imafil.ravno());
//    diralx_ras(&cur);
    goto naz;

  case FK6:    /*Сменить редактор*/
    diralx_sr(&editor);

    goto naz;


  default:
    goto naz;
    break;
 }
}
/**************************/
/*Шапка                   */
/**************************/

void diralx_sap(const char *imabazv,const char *naimpoi,const char *editor)
{
short		y=2;
short           x=COLS-20;



clear();
printw(gettext("Список файлов настройки"));
printw(" %s:%s",gettext("База"),imabazv);
if(editor[0] != '\0')
 printw(" %s:%s",gettext("Редактор"),editor);
 
mvprintw(2,0,"           1                     2            3");
mvprintw(y++,x,gettext("Наименование полей:"));
mvprintw(y++,x,"1-%s",gettext("наименование"));
mvprintw(y++,x,"2-%s",gettext("дата"));
mvprintw(y++,x,"3-%s",gettext("оператор"));

if(naimpoi[0] != '\0')
 {
  y++;
  attron(A_BLINK);
  mvprintw(y++,x,gettext("Поиск:"));
  mvprintw(y++,x,"%s",naimpoi);
  attroff(A_BLINK);
  
 }

helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("ред-вать"),
"F3",gettext("удалить"),
"F4",gettext("поиск"),
"F5",gettext("печать"),
"F6",gettext("редактор"),
"F10",gettext("выход"),NULL);

}
/********************/
/*Распечатать список файлов*/
/********************/

void diralx_ras(SQLCURSOR *cur)
{
char	imaf[64];
FILE	*ff;
SQL_str row,row1;
class SQLCURSOR cur1;
char strsql[512];
sprintf(imaf,"alx%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }


iceb_t_zagolov(gettext("Список файлов настройки"),ff);


fprintf(ff,"\
-------------------------------------------------\n");
fprintf(ff,gettext("\
      Имя файла     |     Дата записи   |Оператор\n"));
/*      
12345678901234567890 1234567890123456789
*/

fprintf(ff,"\
-------------------------------------------------\n");

cur->poz_cursor(0);

while(cur->read_cursor(&row) != 0)
 {
  /*Читаем время и кто записал*/
  sprintf(strsql,"select ktoz,vrem from Alx where fil='%s' order by ns asc limit 1",row[0]);
  readkey(strsql,&row1,&cur1);    

  fprintf(ff,"%-*s %s %s\n",
  iceb_tu_kolbait(20,row[0]),row[0],
  iceb_tu_vremzap(row1[1]),
  iceb_t_kszap(row1[0]));
  
 }
fprintf(ff,"\
-------------------------------------------------\n");

podpis(ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Список файлов настройки"));
iceb_t_ustpeh(imaf,3);
iceb_t_rabfil(&oth,"");
}
/********************/
/*Распечатать файл*/
/********************/

void diralx_rf(const char *imafil)
{
char	imaf[64];
FILE	*ff;
SQL_str row;
class SQLCURSOR cur;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imafil);
if(cur.make_cursor(&bd,strsql) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

sprintf(imaf,"%s",imafil);

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }



while(cur.read_cursor(&row) != 0)
 {
  fprintf(ff,"%s",row[0]);  
 }

fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
sprintf(strsql,"%s %s",gettext("Распечатка файла"),imafil);
oth.spis_naim.plus(strsql);

iceb_t_ustpeh(imaf,3);
iceb_t_rabfil(&oth,"");
}
/**************************************************/
/*Смена редактора*/
/******************************/

void diralx_sr(class iceb_tu_str *editor)
{
class VVOD VVOD1(2);

helstr(LINES-1,0," F10 ",gettext("выход"),NULL);
     
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите редактор"));

if(vvod1(editor,40,&VVOD1,NULL,stdscr,-1,-1) == FK10)
 editor->new_plus("");


}
/*******************/
/*Распечатка*/
/********************/
void diralx_rn(SQLCURSOR *cur,const char *vibfil)
{
int kom=0;
VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное"));

MENU.VVOD_SPISOK_add_MD(gettext("Распечатать список файлов"));//0
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать выбранный файл"));//1
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));//2

for(;;)
 {

  if(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0)
   continue;
  class spis_oth oth;   
  switch (kom)
   {
    case 2:
    case -1 :
      return;

    case 0:
      diralx_ras(cur);
      return;

    case 1:
      diralx_rf(vibfil);
      return;

    default:
      kom=0;
      break;
   }
 }
}
