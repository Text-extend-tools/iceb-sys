/* $Id: perecen.c,v 5.16 2013/09/26 09:43:36 sasa Exp $ */
/*15.07.2015	07.08.1998	Белых А.И.	perecen.c
Увеличение или уменьшение цен в накладной на заданный процент
Может выполняться только для расходных накладных
*/
#include	"buhg.h"


void perecen(short dd,short mm,short gg, //Дата
const char *nn, //Номер накладной
int skl, //Код склада
float pnds)
{
double		prnc;
class iceb_tu_str prnc1("");
short		kom;
extern double   okrcn;
char		strsql[512];
long		kolstr,kolstr1;
SQL_str         row,row1;
class iceb_tu_str naimat("");
double		cena,cenan,cenam;
int		kodm,nk;
//extern  double  nds1;

helstr(LINES-1,0,"F10",gettext("выход"),NULL);

VVOD VVOD1(2);

sprintf(strsql,gettext("Переоценка документа N%s от %d.%d.%d Скл.%d"),
nn,dd,mm,gg,skl);
VVOD1.VVOD_SPISOK_add_ZAG(strsql);

VVOD1.VVOD_SPISOK_add_ZAG(gettext("При 0 проценте переоценивается по учетным ценам."));
VVOD1.VVOD_SPISOK_add_ZAG(gettext("Если ввести \"*\", то цены будуть взяты из списка материалов."));
VVOD1.VVOD_SPISOK_add_ZAG(gettext("Для увеличения цены, процент нужно вводить со знаком \"+\", или без знака."));
VVOD1.VVOD_SPISOK_add_ZAG(gettext("Для уменьшения цены, процент нужно вводить со знаком \"-\"."));
     
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите процент."));

if((kom=vvod1(&prnc1,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
  return;
if(kom == ESC)
  return;

  
if(prnc1.ravno()[0] == '\0')
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Процент не введён !"));
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return;
 }

clear();
prnc=prnc1.ravno_atof();

printw(gettext("Процент %s\n"),prnc1.ravno());
GDITE();

sprintf(strsql,"select kodm,cena,nomkar from Dokummat1 where \
datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' order by kodm asc",
gg,mm,dd,skl,nn);
SQLCURSOR cur;
SQLCURSOR cur1;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  return;
 }
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  kolstr1++;
  strzag(LINES-1,0,kolstr,kolstr1);
  cenan=0.;
  cena=atof(row[1]);
  kodm=atoi(row[0]);
  nk=atoi(row[2]);

  /*Узнаем наименование материалла*/
  sprintf(strsql,"select * from Material where kodm=%d",kodm);
  
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    beep();
    printw("%s %d !\n",gettext("Не найден код материалла"),kodm);
    OSTANOV();
    naimat.new_plus("");
    cenam=0.;
   }
  else
   {
    cenam=atof(row1[6]);
    naimat.new_plus(row1[2]);
   }
  printw("%-3s %d %-20.20s %10.10g > ",
  row[0],nk,naimat.ravno(),cena);
  if(prnc1.ravno()[0] == '0' || prnc1.ravno()[0] == '*')
   {
    if(prnc1.ravno()[0] == '0')
     {
      sprintf(strsql,"select cena from Kart where sklad=%d and nomk=%d",
      skl,nk);
      if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
       {
        beep();
        printw(gettext("Не нашли карточки ! Склад %d Номер карточки %d\n"),
        skl,nk);
        OSTANOV();
        continue;
       }
      cenan=atof(row1[0]);  
     }
    if(prnc1.ravno()[0] == '*')
     {  
      cenan=cenam-cenam*pnds/(100.+pnds);
     }
   }
  else
   {
    if(prnc >= 0.)
      cenan=cena+cena*prnc/100.;
    else
      cenan=cena+cena*prnc/(100.-prnc);
   }

  cenan=okrug(cenan,okrcn);

  /*Меняем цену в карточках*/
  if(nk != 0)
   {
    sprintf(strsql,"update Zkart \
set \
cena=%.10g \
where datd='%d-%02d-%02d' and nomd='%s' and sklad=%d \
and nomk=%d",cenan,gg,mm,dd,nn,skl,nk);

    sql_zapis(strsql,1,0);
   }
  /*Меняем цену в документе*/
  sprintf(strsql,"update Dokummat1 \
set \
cena=%.10g \
where datd='%d-%02d-%02d' and sklad=%d and nomd='%s' and kodm=%d and nomkar=%d",
cenan,gg,mm,dd,skl,nn,kodm,nk);

   sql_zapis(strsql,1,0);


  printw("%10.10g  %.2f%% %.10g\n",cenan,prnc,cenam);

  refresh();
  
 }


OSTANOV();
}
