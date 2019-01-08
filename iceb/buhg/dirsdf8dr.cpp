/* $Id: dirsdf8dr.c,v 5.25 2013/11/05 10:50:48 sasa Exp $ */
/*01.01.2018	10.07.2000	Белых А.И.	dirsdf8dr.c
Просмотр списка документов формы 8др
*/
#include	"buhg.h"

void		sapkaf8dr(void);
int		nomdf8dr();

extern short	ggg;

void		dirsdf8dr()
{
long		kolstr;
long		pozz; /*Позиция первой строчки меню*/
long            pozz1; /*Количество строк прочитанных для заполнения меню*/
SQL_str         row;
char		strsql[512];
int             K;
short           kls;
short           x=0,y=2;
short		mspz; /*метка соответствия первой записи*/
short           i,kom;
int		prc;
class iceb_tu_str viddc("");
char		data1[32];
char		data2[32];
short		d,m,g;
class iceb_tu_str nomd("");
class iceb_t_mstr spis_menu(0);

clear();

VVOD VV(0);

pozz=kom=0;

naz1:;

GDITE();

kls=LINES-5;

sprintf(strsql,"select * from F8dr order by god desc,kvrt desc,nomd desc");
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
 
if(kolstr == 0)
 {
  sapkaf8dr();
lll:;

  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Не найдено ни одного документа !"));
  K=soobshw(&SOOB,stdscr,-1,-1,1,1);

  if(K == FK5 || K == SFK2 || K == SFK3 || K == ENTER)
     goto lll;
  if(K == KEY_RESIZE)
   goto naz1;  
  goto vper;
 }

naz:;
GDITE();

if(kolstr != 0 && pozz >= kolstr)
  pozz = kolstr-1;

cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
spis_menu.spis_str.free_class();
for(i=0; i<kls ; )
 {
  
  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;
  if(row[3][0] == '0')
   viddc.new_plus("З");
  if(row[3][0] == '1')
   viddc.new_plus("НЗ");
  if(row[3][0] == '2')
   viddc.new_plus("У");

  memset(data1,'\0',sizeof(data1));
  memset(data2,'\0',sizeof(data2));
  if(row[5][0] != '0')
   {
    rsdat(&d,&m,&g,row[5],2);
    sprintf(data1,"%02d.%02d.%d",d,m,g);
   }
  if(row[6][0] != '0')
   {
    rsdat(&d,&m,&g,row[6],2);
    sprintf(data2,"%02d.%02d.%d",d,m,g);
   }

  sprintf(strsql,"%s|%s|%-*s|%*s|%-*s|%10s|%10s|%s",
  row[0],row[1],
  iceb_tu_kolbait(4,row[2]),row[2],
  iceb_tu_kolbait(2,viddc.ravno()),viddc.ravno(),
  iceb_tu_kolbait(4,row[4]),row[4],
  data1,data2,row[8]);

  spis_menu.spis_str.plus(strsql);
  i++;

  if(pozz1 == 1) /*Первая запись удовлетворяет условиям поиска*/
   mspz=1;

 }

if(kom >= i)
  kom=i-1;

if(kom < 0)
 kom=0;

if(i <=kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

if(i == 0)
  goto lll;

sapkaf8dr();

prc=(pozz+pozz1)*100/kolstr;
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
  for(i=0; i< mspz  ; )
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

    i++;
   }

  goto naz;
 }
 
if(kom == -5) /*Страница вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
/*
  printw("\npozz=%d kls=%d kolstr=%d i=%d\n",
  pozz,kls,kolstr,i);
  OSTANOV();
*/
  if(pozz + kls >= kolstr)
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
  printw("\n вперед pozz=%d pozz1=%d i=%d kls=%d\n",pozz,pozz1,i,kls);
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

    i++;
   }
  goto naz;
 }

if(kom >= 0)
 {
  if(SRAV(gettext("Конец списка"),spis_menu.spis_str.ravno(kom),0) == 0)
     goto naz;
  if(pozz+kom >= kolstr)
   goto naz;
  polen(spis_menu.spis_str.ravno(kom),&nomd,3,'|');
 }

vper:;
switch(K)
 {  
  case ENTER:
  
     clear();

     dirf8dr(&nomd);
          
     clear();
    
     goto naz1;

  case FK10:
  case ESC:
    return;

  case FK1: /*Помощь*/
    GDITE();
    iceb_t_pdoc("zarp4_7_1_2.txt");
    if(kolstr > 0)
      goto naz;
    else
      goto lll;

  case PLU: /*Ввод нового документа*/
  case FK2: /*Ввод нового документа*/
/*****************
    VV.VVOD_delete();
    sprintf(strsql,"%d",ggg);
    VV.VVOD_SPISOK_add_data(strsql,5);
    VV.VVOD_SPISOK_add_data(2);
    sprintf(strsql,"%d",nomdf8dr());
    VV.VVOD_SPISOK_add_data(strsql,20);
    VV.VVOD_SPISOK_add_data(11);
    VV.VVOD_SPISOK_add_data(30);
    VV.VVOD_SPISOK_add_data(20);
    VV.VVOD_SPISOK_add_data(11);
******************/
    nomd.new_plus("");    
    if(vvodf8dr(&nomd,0) == 0)
     {
//      nomd.new_plus(VV.VVOD_SPISOK_return_data(2));
      dirf8dr(&nomd);
     }

    goto naz1;
    
  default:
    if(kolstr > 0)
      goto naz;
    else
      goto lll; 
 }

}

/********/
/*Шапка*/
/*******/

void	sapkaf8dr()
{
int		Y,X;

clear();


printw(gettext("Просмотр документов"));
move(1,0);
printw("    1  2   3   4  5       6           7      8");
move(Y=2,X=COLS-20);
printw(gettext("Расшифровка полей:"));

move(++Y,X);
printw(gettext("1-год"));
move(++Y,X);
printw(gettext("2-квартал"));
move(++Y,X);
printw(gettext("3-номер документа"));
move(++Y,X);
printw(gettext("4-вид документа"));
move(++Y,X);
printw(gettext("5-номер док. ГНА"));
move(++Y,X);
printw(gettext("6-дата документа"));
move(++Y,X);
printw(gettext("7-дата документа ГНА"));
move(++Y,X);
printw(gettext("8-кто записал"));

helstr(LINES-1,0,
"F1",gettext("помощь"),
"F2/+",gettext("запись"),
"F10",gettext("выход"),NULL);
}
