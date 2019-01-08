/* $Id: perecenu.c,v 5.14 2013/09/26 09:43:36 sasa Exp $ */
/*15.07.2015	07.08.1998	Белых А.И.	perecenu.c
Увеличение или уменьшение цен в документе на заданный процент для
услуг
Может выполняться только для расходных документов
*/
#include	"buhg.h"


void perecenu(short dd,short mm,short gg, //Дата
const char *nn, //Номер документа
int podr, //Код подразделения
int tp, //1-приход 2-расход
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
int		kodm;
short		metka;
SQLCURSOR curr;
clear();

move(0,0);
printw("%s N%s %s %d.%d.%d%s %s:%d",
gettext("Переоценка документа"),
nn,
gettext("от"),
dd,mm,gg,
gettext("г."),
gettext("Подразделение"),
podr);

move(1,0);
printw(gettext("\
Если ввести \"*\", то цены будуть взяты из списка услуг.\n\
Для увеличения цены, процент нужно вводить со знаком \"+\", или без знака.\n\
Для уменьшения цены, процент нужно вводить со знаком \"-\"."));

helstr(LINES-1,0,"F10",gettext("выход"),NULL);

     
VVOD VVOD1(2);
     
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите процент."));

if((kom=vvod1(&prnc1,32,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
  return;
if(kom == ESC)
  return;

  
if(prnc1.ravno()[0] == '\0')
 {
  iceb_t_soob(gettext("Процент не введён !"));
  return;
 }

clear();
prnc=prnc1.ravno_atof();

printw(gettext("Процент %s\n"),prnc1.ravno());
GDITE();

sprintf(strsql,"select metka,kodzap,cena from Usldokum1 where \
datd='%04d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d",
gg,mm,dd,podr,nn,tp);
SQLCURSOR cur;
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
  metka=atoi(row[0]);
  kodm=atoi(row[1]);
  cena=atof(row[2]);
  
  if(metka == 0)
   {
    /*Читаем наименование материалла*/
    sprintf(strsql,"select naimat,cenapr from Material where kodm=%d",
    kodm);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
      {
       naimat.new_plus("");
       beep();
       printw(gettext("Не найдено наименование материалла !"));
       printw("\n");
       OSTANOV();
      }
    else
     naimat.new_plus(row1[0]);
   }  

  if(metka == 1)
   {
    /*Читаем наименование услуги*/
    sprintf(strsql,"select naius,cena from Uslugi where kodus=%s",
    row[1]);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
      {
       naimat.new_plus("");
       beep();
       printw("%s %s !",gettext("Не найден код услуги"),row[1]);
       printw("\n");
       OSTANOV();
      }
    else
     naimat.new_plus(row1[0]);
   }  
  cenam=atof(row1[1]);

  printw("%-3s %-20.20s %10.10g > ",
  row[0],naimat.ravno(),cena);
  if(prnc1.ravno()[0] == '*')
   {
    cenan=cenam-cenam*pnds/(100.+pnds);
   }
  else
   {
    if(prnc >= 0.)
      cenan=cena+cena*prnc/100.;
    else
      cenan=cena+cena*prnc/(100.-prnc);
   }

  cenan=okrug(cenan,okrcn);

  /*Меняем цену в подтверждающей записи*/
  sprintf(strsql,"update Usldokum2 \
set \
cena=%.10g \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s' and metka=%d and \
kodzap=%d and tp=%d",cenan,gg,mm,dd,podr,nn,metka,kodm,tp);

  sql_zapis(strsql,1,0);

  /*Меняем цену в документе*/
  sprintf(strsql,"update Usldokum1 \
set \
cena=%.10g \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s' and metka=%d and \
kodzap=%d and tp=%d",cenan,gg,mm,dd,podr,nn,metka,kodm,tp);

  sql_zapis(strsql,1,0);


  printw("%10.10g  %.2f%% %.10g\n",cenan,prnc,cenam);

  refresh();
  
 }


OSTANOV();
}
