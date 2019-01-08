/* $Id: rasmat.c,v 5.26 2013/09/26 09:43:38 sasa Exp $ */
/*15.07.2015    01.02.1994      Белых А.И.      rasmat.c
Распечатка наличия конкретного материалла на всех складах
*/
#include <ctype.h>
#include        <errno.h>
#include        "buhg.h"

extern double	okrcn;  /*Округление цены*/
extern double	okrg1;  /*Округление цены*/

void            rasmat()
{
class ostatokt ost;
class iceb_tu_str bros("");
struct  tm      *bf;
time_t          tmm;
FILE            *ff;
char            imaf[64];
double          itogo,itogo1;
class iceb_tu_str naim("");
int		kodm,kgr;
short		i;
short		nkp;
double		bb;
char		mnds=' ';
char		strsql[512];
long		kolstr,kolpr;
SQL_str         row;
int		nk;
int		skl;
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
clear(); /*Очистить экран и закрасить фоновым цветом*/

VVOD VVOD1(2);

helstr(LINES-1,0,"F10",gettext("выход"),
"Enter",gettext("материалы"),
" ",gettext("(+) перед числом - поиск по числу в наименовании"),NULL);

VVOD1.VVOD_SPISOK_add_ZAG(gettext("Поиск всех карточек по заданому материалу"));

VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код или наименование материалла"));

if((i=vvod1(&naim,40,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
  return;
if(i == ESC)
 return;

kgr=0;
kodm=naim.ravno_atoi();

if(naim.getdlinna() <= 1 || isdigit(naim.ravno()[0]) == 0)
 {
  clear();
  if(polen(naim.ravno(),&bros,2,'+') == 0)
    naim.new_plus(bros.ravno());

  if(dirmatr(&kgr,&kodm,&naim,1,0,"",0,0) != 0)
    return;
 }
SQLCURSOR cur;

/*Узнаем наименование материалла*/
sprintf(strsql,"select naimat from Material where kodm=%d",
kodm);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден код материалла"),kodm);
  iceb_t_soob(strsql);
  return;
 }
naim.new_plus(row[0]);

GDITE();

sprintf(strsql,"select * from Kart where kodm=%d",kodm);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  printw("**%s\n",sql_error(&bd));
  OSTANOV();
 }

if(kolstr == 0)
 {
  sprintf(strsql,gettext("Не нашли карточек по %d %.30s"),kodm,naim.ravno());
  iceb_t_soob(strsql);
  return;
 }  

clearstr(LINES-1,0);
VVOD DANET(1);

DANET.VVOD_SPISOK_add_MD(gettext("Карточки с нулевыми остатками распечатывать ?"));

nkp=danet(&DANET,2,stdscr);

sprintf(imaf,"pk%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

time(&tmm);
bf=localtime(&tmm);
clear();
printw("\n\n%s %d %s\n",
gettext("Перечень карточек по материалу"),
kodm,naim.ravno());

printw("\
-----------------------------------------------------------------\n");

printw(gettext("Склад| Номер кар. |     Цена     | Счёт | Ед.изм.|   Остаток    |\n"));

printw("\
-----------------------------------------------------------------\n");
refresh();
fprintf(ff,"%s\n\n%s %d %s\n",
iceb_t_get_pnk("00",0),
gettext("Перечень карточек по материалу"),
kodm,naim.ravno());

if(nkp == 2)
  fprintf(ff,gettext("Карточки с нулевыми остатками не распечатаны.\n"));

fprintf(ff,"\
%s %d.%d.%d  %s - %d:%d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ff,"\
------------------------------------------------------------------\n");

fprintf(ff,gettext("Скл.| Н/к |   Цена   | Счёт | Ед.изм.|НДС| Остаток |    Сумма    |\n"));

fprintf(ff,"\
------------------------------------------------------------------\n");

itogo=itogo1=0.;
kolpr=0;
while(cur.read_cursor(&row) != 0)
 {
  skl=atoi(row[0]);
  nk=atoi(row[1]);
  ostkar(1,1,gt,31,mt,gt,skl,nk,&ost);

  if(nkp == 1 || (nkp == 2 && ost.ostg[3] > 0.0000001))
   {
    printw("%-5d %-12d %14s %-*s %-*s %14.10g\n",
    skl,nk,prcn(atof(row[6])),
    iceb_tu_kolbait(6,row[5]),row[5],
    iceb_tu_kolbait(8,row[4]),row[4],
    ost.ostg[3]);
    strzag(LINES-1,0,kolstr,kolpr);

/*
    printw("%-5d %-12d %14s %-6s %-8s %14.10g\n",
    mk9.skl,mk9.nk,prcn(mk9.cena),mk9.shu,mk9.ei,ost.ostg[3]);
    refresh();
*/  
    bb=atof(row[6])*ost.ostg[3];
    bb=okrug(bb,okrg1);
    itogo1+=bb;

    if(row[3][0] == '0')
      mnds=' ';
    if(row[3][0] == '1')
      mnds='+';
    fprintf(ff,"%-4d %-5d %10s %-*s %-*s %c%2.f %9.9g %13.2f\n",
    skl,nk,prcn(atof(row[6])),
    iceb_tu_kolbait(6,row[5]),row[5],
    iceb_tu_kolbait(8,row[4]),row[4],
    mnds,atof(row[9]),ost.ostg[3],bb);
/*             
    fprintf(ff,"%-4d %-5d %10s %-6s %-8s %c%2.f %9.9g %13.2f\n",
    mk9.skl,mk9.nk,prcn(mk9.cena),mk9.shu,mk9.ei,mnds,mk9.nds,ost.ostg[3],bb);
*/
   }
  itogo+=ost.ostg[3];
  kolpr++;
 }
printw("\
-----------------------------------------------------------------\n\
%*s %14.10g\n\
-----------------------------------------------------------------\n",
iceb_tu_kolbait(49,gettext("Итого")),gettext("Итого"),itogo);

fprintf(ff,"\
------------------------------------------------------------------\n\
%*s %14.10g %13.2f\n\
-------------------------------------------------------------------\n",
iceb_tu_kolbait(36,gettext("Итого")),gettext("Итого"),itogo,itogo1);

fclose(ff);

OSTANOV();
class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Перечень карточек по материалу."));

iceb_t_ustpeh(imaf,0);

iceb_t_rabfil(&oth,"");
}
