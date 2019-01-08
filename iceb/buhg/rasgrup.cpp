/* $Id: rasgrup.c,v 5.26 2013/09/26 09:43:38 sasa Exp $ */
/*15.07.2015    19.11.1996      Белых А.И.      rasgrup.c
Получение распечатки всех карточек по определенной группе
материалла
*/
#include <ctype.h>
#include        <errno.h>
#include        "buhg.h"


extern double	okrg1;
extern double	okrcn;  /*Округление цены*/

void            rasgrup(short metka) //0-полный отчёт 1-сокращенный*/
{
class ostatokt ost;
char str[1024];
struct  tm      *bf;
time_t          tmm;
FILE            *ff,*ff1;
char            imaf[64],imaf1[64];
double          itogo,itogo1,bb,itkg;
class iceb_tu_str naim("");
class iceb_tu_str naimgr("");
class iceb_tu_str kod("");
short		nkp;
int		kgrm;
int		i=0;
char		mnds=' ';
double		ostk;
int		skl,sklz;
char		strsql[512];
long		kolstr,kolpr,kolstr1;
SQL_str         row,row1,row2;
int		nk;
double		cena;
int		poz,komv;
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);
clear();

VVOD VVOD1(2);

VVOD1.VVOD_SPISOK_add_MD(gettext("Распечатка карточек по группе материалла"));

helstr(LINES-1,0,"Enter",gettext("просмотр групп материалов"),
"F10",gettext("выход"),NULL);

VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код или наименование группы материалла"));

if(vvod1(&naimgr,24,&VVOD1,NULL,stdscr,-1,-1) == FK10)
  return;
if(i == ESC)
    return;

kgrm=naimgr.ravno_atoi();

if(naimgr.getdlinna() <= 1 || isdigit(naimgr.ravno()[0]) == 0)
 {
  clear();
  kod.new_plus("");
  poz=komv=0;
  if(dirmat1("Grup",&kod,&naimgr,LINES-5,0,2,1,&poz,&komv,"","\0") != 0)
   return;
  kgrm=kod.ravno_atoi();
 }

SQLCURSOR cur;

/*Читаем код группы материалла*/
sprintf(strsql,"select naik from Grup where kod=%d",kgrm);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не знайшли код групи"),kgrm);
  iceb_t_soob(strsql);
  return;
 }
naimgr.new_plus(row[0]);
  
GDITE();

sprintf(strsql,"select kodm from Material where kodgr=%d",kgrm);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  printw("**%s\n",sql_error(&bd));
  OSTANOV();
 }

if(kolstr == 0)
 {
  sprintf(strsql,gettext("Не нашли материалов с кодом группы %d !"),kgrm);
  iceb_t_soob(strsql);
  return;
 }  

nkp=2;
if(metka == 0)
 {
  VVOD DANET(1);

  clearstr(LINES-1,0);

  DANET.VVOD_SPISOK_add_MD(gettext("Карточки с нулевыми остатками распечатывать ?"));

  nkp=danet(&DANET,2,stdscr);
//  nkp=danet(&LL,2,stdscr,-1,-1);
 }

i=getpid();
sprintf(imaf,"gpk%d.tmp",i);

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

time(&tmm);
bf=localtime(&tmm);
clear();
printw("\n\n%s %d %s\n",gettext("Перечень карточек по группе"),kgrm,naimgr.ravno());
refresh();
if(metka == 0)
 {
 printw("\
---------------------------------------------------------------------\n");

 printw(gettext("Скл.| Н.к. |    Наименование материалла   |   Цена   | Остаток      |\n"));
 printw("\
---------------------------------------------------------------------\n");
 }
if(metka == 1)
 {
 printw("\
--------------------------------------------------------------------\n");
 printw(gettext("Ск|Н.к.|    Наименование материалла   |Е/и |Остат.карт.|Остат.док. |\n"));

 printw("\
--------------------------------------------------------------------\n");
 }

itogo=itogo1=itkg=0.;

kolpr=0;
while(cur.read_cursor(&row) != 0)
 {
  kolpr++;
  strzag(LINES-1,0,kolstr,kolpr);
  sprintf(strsql,"select * from Kart where kodm=%s",row[0]);
  SQLCURSOR cur1;
  if((kolstr1=cur1.make_cursor(&bd,strsql)) < 0)
   {
    printw("**%s\n",sql_error(&bd));
    OSTANOV();
   }

  if(kolstr1 == 0)
   {
    continue;
   }  

  while(cur1.read_cursor(&row1) != 0)
   {
    skl=atoi(row1[0]);  
    nk=atoi(row1[1]);  
    cena=atof(row1[6]);
    cena=okrug(cena,okrcn);
    
    ostkar(1,1,gt,31,mt,gt,skl,nk,&ost);
    ostk=ost.ostg[3];  

    if(nkp == 1 || (nkp == 2 && ost.ostg[3] > 0.0000001))
     {
      SQLCURSOR cur2;
      /*Узнаем наименование материалла*/
      sprintf(strsql,"select naimat from Material where kodm=%s",
      row[0]);
      if(sql_readkey(&bd,strsql,&row2,&cur2) != 1)
       {
        naim.new_plus("");
        beep();
        printw("%s %d !\n",gettext("Не нашли код материала"),row[0]);
        OSTANOV();
       }
      else
       naim.new_plus(row2[0]);

      if(metka == 0)    
       { 
        printw("%-4d %-6d %-*.*s %10s %15.10g\n",
        skl,nk,
        iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
        prcn(cena),ost.ostg[3]);

        bb=cena*ost.ostg[3];
        bb=okrug(bb,okrg1);
        itogo1+=bb;

        if(row1[3][0] == '0')
          mnds=' ';
        if(row1[3][0] == '1')
          mnds='+';

        itkg+=atof(row1[10])*ost.ostg[3];
           
        fprintf(ff,"%-4d|%-6d|%-*.*s|%10s|%*s|%-*s|%c%2.f|\
%11.10g|%9.2f|%6.2f|%10.2f|%.10g\n",
        skl,nk,
        iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
        prcn(cena),
        iceb_tu_kolbait(6,row1[5]),row1[5],
        iceb_tu_kolbait(8,row1[4]),row1[4],
        mnds,atof(row1[9]),ost.ostg[3],
        bb,atof(row1[10]),ost.ostg[3]*atof(row1[10]),atof(row1[8]));
       }

      if(metka == 1)    
       {
        ostdok(1,mt,gt,31,mt,gt,skl,nk,&ost);
        
        printw("%-2d %-4d %-*.*s %-*s %11.10g %11.10g\n",
        skl,nk,
        iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
        iceb_tu_kolbait(4,row1[4]),row1[4],
        ostk,ost.ostg[3]);

        fprintf(ff,"%-3d|%-4d|%-*.*s|%-*s|%11.10g|%11.10g|%.10g|%.10g\n",
        skl,nk,
        iceb_tu_kolbait(30,naim.ravno()),iceb_tu_kolbait(30,naim.ravno()),naim.ravno(),
        iceb_tu_kolbait(4,row1[4]),row1[4],
        ostk,ost.ostg[3],atof(row1[8]),atof(row1[10]));
       }
     }
     
    itogo+=ost.ostg[3];
   }
 }
GDITE();

fclose(ff);

/*Сортировка файла*/
//sprintf(bros,"sort -o %s -t\\| +0n -1 +2b -3 %s",imaf,imaf);
sprintf(str,"sort -o %s -t\\| -k1,2n -k3,4b %s",imaf,imaf);
runsystem(str);

if((ff = fopen(imaf,"r")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

i=getpid();
sprintf(imaf1,"gpk%d.lst",i);

if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  error_op_nfil(imaf1,errno,"");
  return;
 }
startfil(ff1);

fprintf(ff1,"%s\n\n%s %d %s\n",
iceb_t_get_pnk("00",0),gettext("Перечень карточек по группе"),kgrm,naimgr.ravno());

if(nkp == 2)
  fprintf(ff1,gettext("Карточки с нулевыми остатками не распечатаны !\n"));

fprintf(ff1,"\x0F");

if(metka == 0)
 {
  fprintf(ff1,"\
%s %d.%d.%d  %s - %d:%d\n",
  gettext("По состоянию на"),
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
  gettext("Время"),
  bf->tm_hour,bf->tm_min);

  fprintf(ff1,"\
----------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff1,gettext("Скл.| Н.к. |    Наименование материалла   |   Цена   | Счёт | Ед.изм.|НДС|  Остаток  |  Сумма  | Фас. |Килограмы |Кратность \n"));

  fprintf(ff1,"\
----------------------------------------------------------------------------------------------------------------------------\n");
 }

if(metka == 1)
 {
  fprintf(ff1,"\
%s %d.%d.%d  %s - %d:%d\n",
  gettext("По состоянию на"),
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
  gettext("Время"),
  bf->tm_hour,bf->tm_min);

  fprintf(ff1,"\
--------------------------------------------------------------------\n");

  fprintf(ff1,gettext("Ск.|Н.к.|    Наименование материалла   |Е/и |Остат.карт.|Остат.док.|\n"));

  fprintf(ff1,"\
--------------------------------------------------------------------\n");
 }

sklz=skl=0;
while(fgets(str,sizeof(str),ff) != NULL)
 {
  polen(str,&skl,1,' ');
  if(skl != sklz)
   {
    if(sklz != 0)
      fprintf(ff1,"--------------------------------------------------------------------\n");
    sklz=skl;
   }
  fprintf(ff1,"%s",str);
 }

if(metka == 0)
 {
  printw("\
-------------------------------------------------------------------------\n\
%*s:%15.10g\n\
-------------------------------------------------------------------------\n",
iceb_tu_kolbait(53,gettext("Итого")),gettext("Итого"),itogo);

  fprintf(ff1,"\
----------------------------------------------------------------------------------------------------------------------------\n\
%*s:%11.10g %9.2f %6s %10.2f\n\
----------------------------------------------------------------------------------------------------------------------------\n",
iceb_tu_kolbait(73,gettext("Итого")),gettext("Итого"),itogo,itogo1," ",itkg);
 }
 
fprintf(ff1,"\x12");

fclose(ff);
fclose(ff1);
unlink(imaf);

printw("\n%s \"%s\"\n\n",gettext("Распечатка выгружена в файл"),imaf1);

OSTANOV();
class spis_oth oth;
oth.spis_imaf.plus(imaf1);
oth.spis_naim.plus(gettext("Перечень карточек материалов по группе"));

iceb_t_ustpeh(imaf1,0);

iceb_t_rabfil(&oth,"");

}
