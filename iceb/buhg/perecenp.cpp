/* $Id: perecenp.c,v 5.15 2013/09/26 09:43:36 sasa Exp $ */
/*15.07.2015	11.06.2003	Белых А.И.	perecenp.c
Увеличение или уменьшение цен в накладной на заданный процент
Может выполняться только для приходных документов
*/
#include	"buhg.h"


void perecenp(short dd,short mm,short gg, //Дата
const char *nn, //Номер накладной
int skl) //Код склада
{
double		prnc;
class iceb_tu_str prnc1("");
short		kom;
extern double   okrcn;
char		strsql[512];
long		kolstr,kolstr1;
SQL_str         row,row1;
class iceb_tu_str naimat("");
double		cena,cenan;
int		kodm;
SQLCURSOR cur;
SQLCURSOR cur1;

GDITE();

//Все записи в документе не должны иметь привязки к карточкам

sprintf(strsql,"select nomkar from Dokummat1 where \
datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' and nomkar <> 0",
gg,mm,dd,skl,nn);
if(sql_readkey(&bd,strsql,&row,&cur) > 0)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("В документе есть записи привязанные к карточкам !"));
  SOOB.VVOD_SPISOK_add_MD(gettext("Переоценка не возможна !"));
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return;
 }

helstr(LINES-1,0,"F10",gettext("выход"),NULL);

VVOD VVOD1(2);

sprintf(strsql,gettext("Переоценка документа N%s от %d.%d.%d Скл.%d"),
nn,dd,mm,gg,skl);
VVOD1.VVOD_SPISOK_add_ZAG(strsql);

VVOD1.VVOD_SPISOK_add_ZAG(gettext("Для увеличения цены, процент нужно вводить со знаком \"+\", или без знака."));
VVOD1.VVOD_SPISOK_add_ZAG(gettext("Для уменьшения цены, процент нужно вводить со знаком \"-\"."));
     
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите процент."));

naz:;

if((kom=vvod1(&prnc1,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
  return;
if(kom == ESC)
  return;

  
if(prnc1.ravno()[0] == '\0')
 {
  iceb_t_soob(gettext("Процент не введён !"));
  goto naz;
 }

prnc=prnc1.ravno_atof();
if(prnc == 0.)
 {
  iceb_t_soob(gettext("Процент не может быть нулевым !"));
  goto naz;
 }

clear();
printw(gettext("Процент %s\n"),prnc1.ravno());
GDITE();

sprintf(strsql,"select kodm,cena from Dokummat1 where \
datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' order by kodm asc",
gg,mm,dd,skl,nn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
  return;

kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  kolstr1++;
  strzag(LINES-1,0,kolstr,kolstr1);
  cenan=0.;
  cena=atof(row[1]);
  kodm=atoi(row[0]);
  
  /*Узнаем наименование материалла*/
  
  sprintf(strsql,"select naimat from Material where kodm=%d",kodm);
  
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    naimat.new_plus(row1[0]);
  else
    naimat.new_plus("");
    
  printw("%-3s %-20.20s %10.10g > ",
  row[0],naimat.ravno(),cena);

  if(prnc >= 0.)
    cenan=cena+cena*prnc/100.;
  else
    cenan=cena+cena*prnc/(100.-prnc);

  cenan=okrug(cenan,okrcn);

  /*Меняем цену в документе*/
  sprintf(strsql,"update Dokummat1 \
set \
cena=%.10g \
where datd='%d-%02d-%02d' and sklad=%d and nomd='%s' and kodm=%d and nomkar=%d",
cenan,gg,mm,dd,skl,nn,kodm,0);

   sql_zapis(strsql,1,0);


  printw("%10.20g  %.2f%% \n",cenan,prnc);

  refresh();
  
 }


OSTANOV();
}
