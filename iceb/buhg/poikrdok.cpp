/* $Id: poikrdok.c,v 5.17 2013/08/13 05:49:50 sasa Exp $ */
/*23.05.2013	25.10.2002	Белых А.И.	poikrdok.c
Поиск документов командировочных расходов по номеpу документа 
*/
#include        "buhg.h"


void		poikrdok()
{
class iceb_tu_str nomdok("");
short		dd,md,gd;
long		kolstr;
SQL_str         row;
class iceb_t_mstr spis_menu(0);
char		strsql[512];
class iceb_tu_str bros("");
int		K,i,kom;
int		pozz,pozz1;
short		mspz;
int		x=0,y=2;
int		prc;
int		X,Y;
short		metka;

metka=kom=pozz=pozz1=K=0;
VVOD VVOD1(2);
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите номер документа"));

naz2:;

helstr(LINES-1,0,"F1",gettext("помощь"),
"F10",gettext("выход"),NULL);


if((i=vvod1(&nomdok,40,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
  return;
if(i == FK1)
 {
  GDITE();
  iceb_t_pdoc("matu2_2.txt");
  clear();
  goto naz2;
 }

if(nomdok.getdlinna() <= 1)
  return;

naz1:;

GDITE();
short		kls=LINES-5;
  
sprintf(strsql,"select datd,nomd,kont,mprov,mpodt,vkom from Ukrdok where nomd='%s' \
order by datd desc",nomdok.ravno());
/*
printw("\n%s\n",strsql);
refresh();
*/
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }  

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Документ не найден !"));
  return;
 }

clear();

if(kolstr == 1 && metka == 0)
 {

  cur.read_cursor(&row);
  rsdat(&dd,&md,&gd,row[0],2);

  dirdokkr(dd,md,gd,&nomdok);
  return;  
 }

metka++;

naz:;
GDITE();
clear();

if(kolstr != 0 && pozz > kolstr)
  pozz = kolstr-1;

cur.poz_cursor(pozz);
mspz=2;
pozz1=0;
spis_menu.spis_str.free_class();
for(i=0; i < kls ;)
 {

  if(cur.read_cursor(&row) == 0)
    break;
  pozz1++;

  rsdat(&dd,&md,&gd,row[0],2);
  bros.new_plus("");
  if(row[4][0] == '0')
   bros.plus("?");
  if(row[3][0] == '0')
   bros.plus("*");

  sprintf(strsql,"%02d.%02d.%d|%-*s|%-*s|%-*s|%s",
  dd,md,gd,
  iceb_tu_kolbait(4,row[1]),row[1],
  iceb_tu_kolbait(5,row[2]),row[2],
  iceb_tu_kolbait(3,row[5]),row[5],
  bros.ravno());

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
 return;

if(i <= kls-1)
  spis_menu.spis_str.plus(gettext("Конец списка"));

helstr(LINES-1,0,"Enter",gettext("выбоp нужного"),
"F10",gettext("выход"),NULL);
move(0,0);
printw(gettext("Cписок найденых документов"));
move(1,0);
printw("      1        2    3    4");

move(Y=1,X=COLS-20);
printw(gettext("Расшифpовка полей:"));
move(++Y,X);
printw(gettext("1-дата выписки"));
move(++Y,X);
printw(gettext("2-номеp документа"));
move(++Y,X);
printw(gettext("3-контpагент"));
move(++Y,X);
printw(gettext("4-вид командировки"));

move(++Y,X);
printw(gettext("? не подтвеpжден"));
move(++Y,X);
printw(gettext("* не вы-нены пpов-ки"));

prc=(pozz+pozz1)*100/kolstr;
memset(strsql,'\0',sizeof(strsql));
kom=spis_menu.menu(kom,y,x,NULL,strsql,prc,&K,"",0);

if(kom == -7)
  goto naz1;

if(kom == -3) /*Стрелка вниз*/
 {
  if(i <kls)
   {
    beep();
    goto naz;
   }
  for(i=0; i < mspz  ; )
   {
    pozz++;

    cur.poz_cursor(pozz);
    if(cur.read_cursor(&row) == 0)
      break;

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

  polen(spis_menu.spis_str.ravno(kom),&bros,1,'|');
  rsdat(&dd,&md,&gd,bros.ravno(),1);
  polen(spis_menu.spis_str.ravno(kom),&nomdok,2,'|');

 }
 
switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
    return;

  case ENTER : /*Выбоp*/
     dirdokkr(dd,md,gd,&nomdok);
     goto naz1;

  default:
     goto naz;
 }

}
