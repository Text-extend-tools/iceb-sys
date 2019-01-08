/* $Id: poiupldok.c,v 5.15 2013/08/13 05:49:51 sasa Exp $ */
/*17.03.2018	26.04.2003	Белых А.И.	poiupldok.c
Поиск документов по номеpу документа
*/
#include        "buhg.h"


void		poiupldok()
{
class iceb_tu_str nomdok("");
short		dd,md,gd;
long		kolstr;
SQL_str         row,row1;
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
class iceb_tu_str podr("");
class iceb_tu_str fio_vod("");
class SQLCURSOR cur,cur1;

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
//   prosf(strsql);
   clear();
   goto naz2;
 }

if(nomdok.getdlinna() <= 1)
  return;

naz1:;

GDITE();
short		kls=LINES-5;
  
sprintf(strsql,"select datd,kp,nomd,kv from Upldok where nomd='%s' \
order by datd desc",nomdok.ravno());
/*
printw("\n%s\n",strsql);
refresh();
*/

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

  podr.new_plus(row[1]);
  nomdok.new_plus(row[2]);
  
//  dirupld(dd,md,gd,&nomdok,&podr);
  dirupldok3(dd,md,gd,nomdok.ravno(),podr.ravno_atoi());
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

  /*узнаём фамилию водителя*/
  sprintf(strsql,"select naik from Uplouot where kod=%d and mt=1",atoi(row[3]));
  if(readkey(strsql,&row1,&cur1) == 1)
   fio_vod.new_plus(row1[0]);
  else
   fio_vod.new_plus("");

  rsdat(&dd,&md,&gd,row[0],2);

  sprintf(strsql,"%02d.%02d.%d|%-2s|%-5s|%s %s",
  dd,md,gd,
  row[1],
  row[2],
  row[3],fio_vod.ravno());

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
printw("      1      2    3    4");

mvprintw(Y=1,X=COLS-20,"%s:",gettext("Расшифpовка полей"));
mvprintw(++Y,X,"1-%s",gettext("дата выписки"));
mvprintw(++Y,X,"2 %s",gettext("код подразделения"));
mvprintw(++Y,X,"3-%s",gettext("номеp документа"));
mvprintw(++Y,X,"4-%s",gettext("водитель"));

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
  polen(spis_menu.spis_str.ravno(kom),&podr,2,'|');
  polen(spis_menu.spis_str.ravno(kom),&nomdok,3,'|');

 }
 
switch (K)
 {
  case FK10: /*Выход*/
  case ESC : /*Выход*/
    return;

  case ENTER : /*Выбоp*/
//     dirupld(dd,md,gd,&nomdok,&podr);
     dirupldok3(dd,md,gd,nomdok.ravno(),podr.ravno_atoi());
     goto naz1;

  default:
     goto naz;
 }

}
