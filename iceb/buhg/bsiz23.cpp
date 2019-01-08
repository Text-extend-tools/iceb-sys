/* $Id: bsiz23.c,v 5.25 2013-04-18 09:26:02 sasa Exp $ */
/*13.07.2015    22.08.1997      Белых А.И.      bsiz23.c
Получение балансовой стоимсости и износа для групп 2 и 3
*/
#include        "buhg.h"
#include "poiinpd.h"

extern short	startgoduos; /*Стартовый год*/

void bsiz23(const char *grp,
short dp,short mp,short gp,
double *bs, //Изменение балансовой стоимости на дату
double *iz, //Изменение износа на дату
double *iz1, //Амортизация на дату
FILE *ff_prot)
{
long		kolstr;
SQL_str         row,row1;
SQLCURSOR curr;
char		strsql[1024];
short           d,m,g;
short		tp;
long		innom;
class iceb_tu_str naim("");
double		pbs,piz;
char metka_zap[8];
class poiinpdw_data rekin;

//printw("bsiz23--%s %d.%d.%d\n",grp,dp,mp,gp);


d=dp;
m=mp;
g=gp;
if(startgoduos != gp || mp != 1 || dp != 1)
  dpm(&d,&m,&g,2); 

*bs=*iz=*iz1=0.;

sprintf(strsql,"select datd,tipz,innom,podr,bs,iz,nomd,kodop from Uosdok1 where \
datd >= '%d-%d-%d' and datd <= '%d-%d-%d' and podt=1 \
order by datd asc, tipz desc",startgoduos,1,1,g,m,d);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
//  printf("\n%s\n",strsql);
  sprintf(strsql,"%s-%s",__FUNCTION__,gettext("Не найдено ни одной записи !"));
  iceb_t_soob(strsql);
  return;
 }



if(ff_prot != NULL)
 {
  fprintf(ff_prot,"%s:%s  %s:%d.%d.%d\n",
  gettext("Группа"),
  grp,
  gettext("Дата"),
  dp,mp,gp);

  fprintf(ff_prot,"\
--------------------------------------------------------------------------------------------------\n");

  fprintf(ff_prot,"\
 | Дата док.|Номер док.|Бал.стоим.| Износ    |Инв.н.|Наименование объекта          |Груп.|Код оп.|\n");

  fprintf(ff_prot,"\
--------------------------------------------------------------------------------------------------\n");

 }

while(cur.read_cursor(&row) != 0)
 {
/*
  printw("%s %s %s %s %s %s %s\n",
  row[0],row[1],row[2],row[3],row[4],row[5],row[6]);
  refresh();
*/
  if(SRAV(gettext("ОСТ"),row[7],0) == 0)
    continue;
      
  rsdat(&d,&m,&g,row[0],2);
  tp=atoi(row[1]);
  innom=atol(row[2]);
  pbs=atof(row[4]);
  piz=atof(row[5]);
  /*Читаем переменные данные*/
//  printw("%d.%d\n",mp,gp);
//  OSTANOV();
//  poiinpd(innom,mp,gp,shetu,hzt,hau,hna,&popkf,&soso,nomz,&mz,&gz,hnaby,&popkfby);
  poiinpd(innom,mp,gp,&rekin);
  if(SRAV(grp,rekin.hna.ravno(),0) != 0)
   continue;
  
  if(tp == 2)
   {
    pbs*=(-1.);
    piz*=(-1.);
//Износ не умножаем
   }

  if(ff_prot != NULL)
   {
    /*Читаем постоянную часть*/   
    sprintf(strsql,"select naim from Uosin where innom=%ld",innom);
    naim.new_plus("");
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      beep();
      printw("%s-%s %ld !\n",__FUNCTION__,gettext("Не найден инвентарный номер"),innom);
      OSTANOV();
     }
    else
     {
      naim.new_plus(row1[0]);
     }
    if(tp == 1)
     strcpy(metka_zap,"+");
    else
     strcpy(metka_zap,"-");
            
    fprintf(ff_prot,"%s %02d.%02d.%d %-*s %10.2f %10.2f %-6ld %-*.*s %-5s %-7s\n",
    metka_zap,d,m,g,
    iceb_tu_kolbait(10,row[6]),
    row[6],
    pbs,piz,innom,
    iceb_tu_kolbait(30,naim.ravno()),
    iceb_tu_kolbait(30,naim.ravno()),
    naim.ravno(),
    rekin.hna.ravno(),row[7]);

   }

  *bs=*bs+pbs;
  *iz=*iz+piz;

 }

if(ff_prot != NULL)
 {
  fprintf(ff_prot,"\
--------------------------------------------------------------------------------------------------\n");
  fprintf(ff_prot,"%23s %10.2f %10.2f\n",gettext("Итого"),*bs,*iz);
 }
/*
printw("bs23=%.2f iz-%.2f\n",*bs,*iz);
refresh();
OSTANOV();
*/

/*Узнаем амортизацию*/
/*********************
if(startgoduos != 0 && startgoduos != gp)
  sprintf(strsql,"select suma,mes,god,innom from Uosamor where \
((god = %d and mes >= %d) or \
(god > %d and god < %d) or \
(god = %d and mes < %d)) and \
innom=%d and suma <> 0. order by god,mes asc",
startgoduos,1,startgoduos,gp,gp,mp,atoi(grp)*(-1));
else
  sprintf(strsql,"select suma,mes,god,innom from Uosamor where \
god = %d and mes >= %d and  mes < %d and \
innom=%d and suma <> 0. order by god,mes asc",gp,1,mp,atoi(grp)*(-1));
***************************/

sprintf(strsql,"select suma,da from Uosamor where \
da >= '%04d-01-01' and da <= '%04d-%02d-01' and \
innom=%d and suma <> 0. order by da asc",startgoduos,gp,mp,atoi(grp)*(-1));

if(ff_prot != NULL)
  fprintf(ff_prot,"%s\n",strsql);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);

if(kolstr <= 0)
  return;
if(ff_prot != NULL)
 {
  fprintf(ff_prot,"Амортизация\n");

  fprintf(ff_prot,"\
-------------------\n");

  fprintf(ff_prot,"\
   Дата   |Сумма\n");

  fprintf(ff_prot,"\
-------------------\n");
 }
while(cur.read_cursor(&row) != 0)
 {
  if(ff_prot != NULL)
    fprintf(ff_prot,"%s %10s\n",iceb_tu_datzap(row[1]),row[0]);
  *iz1+=atof(row[0]);
 }

if(ff_prot != NULL)
 {
  fprintf(ff_prot,"\
-------------------\n");
  fprintf(ff_prot,"%7s:%10.2f\n",gettext("Итого"),*iz1);
  fprintf(ff_prot,"Общий износ=%.2f\n",*iz1+*iz);
  
 } 
//OSTANOV();
}






