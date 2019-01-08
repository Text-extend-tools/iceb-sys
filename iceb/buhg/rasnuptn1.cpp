/* $Id: rasnuptn1.c,v 5.14 2013/09/26 09:43:38 sasa Exp $ */
/*15.07.2015	22.10.1999	Белых А.И.	rasnuptn1.c
Распечатка всех начислений и удержаний за определенный месяц
без учета сальдо
*/
#include        <errno.h>
#include        "buhg.h"

extern short	*obud; /*Обязательные удержания*/

void rasnuptn1(short mn,short gn,short dk,short mk,short gk,
int tabn,const char *fio,FILE *ff)
{
long		kolstr;
SQL_str         row,row1;
char		strsql[512];
short		prn;
short		d,m,g;
short		knah;
double		sym,itog,inah,iuder,iobud;
char		priz[8];
class iceb_tu_str nai("");
int		i;

if(obud == NULL)
 {
  sprintf(strsql,"%s-%s",__FUNCTION__,gettext("Не введены Обязательные удержания !"));
  iceb_t_soob(strsql);
  return;
 }

fprintf(ff,"%s %s %d.%d%s\n%d %s\n",
gettext("Начисления и удержания без учёта сальдо"),
gettext("За"),mn,gn,
gettext("г."),tabn,fio);

if(dk != 0)
 fprintf(ff,"%s %d.%d.%d%s\n",gettext("До"),dk,mk,gk,
 gettext("г."));

fprintf(ff,"%s\n",gettext("Коды обязательных платежей:"));
for(i=1; i < obud[0]+1 ; i++)
 fprintf(ff,"%d ",obud[i]);
if(dk == 0)
 sprintf(strsql,"select * from Zarp where tabn=%d and \
mesn=%d and godn=%d and suma <> 0. order by datz,prn,knah asc",
 tabn,mn,gn);
else
 sprintf(strsql,"select * from Zarp where tabn=%d and \
mesn=%d and godn=%d and datz <= '%d-%d-%d' and suma <> 0. \
order by datz,prn,knah asc",
 tabn,mn,gn,gk,mk,dk);
 
SQLCURSOR cur;
SQLCURSOR curr;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  fprintf(ff,"\n%s\n",gettext("Записей не найдено"));
  return;
 }

fprintf(ff,"\n%s:\n\
---------------------------------------------------------------------------\n",
gettext("Начисления"));

/*Распечатываем все начисления*/
sym=inah=iuder=iobud=itog=0;
while(cur.read_cursor(&row) != 0)
 {
  prn=atoi(row[2]);
  if(prn != 1)
   continue;
  rsdat(&d,&m,&g,row[0],2);
  knah=atoi(row[3]);
  sym=atof(row[4]);

  memset(priz,'\0',sizeof(priz));  
  inah+=sym;
  strcpy(priz,"+");
  sprintf(strsql,"select naik from Nash where kod=%d",knah);

  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    beep();
    if(prn == 1)
     printw("%s %s !\n",gettext("Не найден код начисления"),row[3]);
    if(prn == 2)
     printw("%s %s !\n",gettext("Не найден код удержания"),row[3]);
    OSTANOV();
    nai.new_plus("");
   }
  else
    nai.new_plus(row1[0]);

  fprintf(ff,"%2d %02d.%02d.%d %s %-*.*s %2s.%s %-*s %10s %s\n",
  knah,d,m,g,priz,
  iceb_tu_kolbait(30,nai.ravno()),iceb_tu_kolbait(30,nai.ravno()),nai.ravno(),
  row[7],row[6],
  iceb_tu_kolbait(4,row[5]),row[5],
  prnbr(sym),row[12]);
  
 }

fprintf(ff,"%*s: %10s\n",iceb_tu_kolbait(58,gettext("Итого")),gettext("Итого"),prnbr(inah));

/*Распечатываем все обязятельные платежи*/

fprintf(ff,"\n%s:\n\
-------------------------------------------------------------------------\n",
gettext("Обязательные удержания"));
cur.poz_cursor(0);
while(cur.read_cursor(&row) != 0)
 {
  prn=atoi(row[2]);
  if(prn != 2)
   continue;
  knah=atoi(row[3]);
  if(obud != NULL)
   {
    for(i=1; i <= obud[0] ; i++)
     if(knah == obud[i])
      break;
    if(i > obud[0])
     { 
//      printw("Пропустили %d %d %d\n",i,obud[0],knah);
      continue;
     }
   }
  
  rsdat(&d,&m,&g,row[0],2);
  sym=atof(row[4]);
  memset(priz,'\0',sizeof(priz));  

  iobud+=sym;
  strcpy(priz,"-");
  sprintf(strsql,"select naik from Uder where kod=%d",knah);

  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    beep();
    if(prn == 1)
     printw("%s %s !\n",gettext("Не найден код начисления"),row[3]);
    if(prn == 2)
     printw("%s %s !\n",gettext("Не найден код удержания"),row[3]);
    OSTANOV();
    nai.new_plus("");
   }
  else
    nai.new_plus(row1[0]);

  fprintf(ff,"%2d %02d.%02d.%d %s %-*.*s %2s.%s %-*s %10s %s\n",
  knah,d,m,g,priz,
  iceb_tu_kolbait(30,nai.ravno()),iceb_tu_kolbait(30,nai.ravno()),nai.ravno(),
  row[7],row[6],
  iceb_tu_kolbait(4,row[5]),row[5],
  prnbr(sym),row[12]);
  
 }
fprintf(ff,"%58s: %10s\n",gettext("Итого"),prnbr(iobud));
fprintf(ff,"%58s: %10s\n",gettext("К выдаче"),prnbr(inah+iobud));

/*Распечатываем все удержания без обязательных платежей*/

fprintf(ff,"\n%s:\n\
------------------------------------------------------------------------\n",
gettext("Другие удержания"));
cur.poz_cursor(0);
while(cur.read_cursor(&row) != 0)
 {
  prn=atoi(row[2]);
  if(prn != 2)
   continue;
  knah=atoi(row[3]);
  if(obud != NULL)
   {
    for(i=1; i <= obud[0] ; i++)
     if(knah == obud[i])
      break;
    if(i <= obud[0])
     { 
//      printw("Пропустили %d %d %d\n",i,obud[0],knah);
      continue;
     }
   }

  rsdat(&d,&m,&g,row[0],2);
  sym=atof(row[4]);
  memset(priz,'\0',sizeof(priz));  

  iuder+=sym;
  strcpy(priz,"-");
  sprintf(strsql,"select naik from Uder where kod=%d",knah);

  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    beep();
    if(prn == 1)
     printw("%s %s !\n",gettext("Не найден код начисления"),row[3]);
    if(prn == 2)
     printw("%s %s !\n",gettext("Не найден код удержания"),row[3]);
    OSTANOV();
    nai.new_plus("");
   }
  else
    nai.new_plus(row1[0]);

  fprintf(ff,"%2d %02d.%02d.%d %s %-*.*s %2s.%s %-*s %10s %s\n",
  knah,d,m,g,priz,
  iceb_tu_kolbait(30,nai.ravno()),iceb_tu_kolbait(30,nai.ravno()),nai.ravno(),
  row[7],row[6],
  iceb_tu_kolbait(4,row[5]),row[5],
  prnbr(sym),row[12]);
  
 }

fprintf(ff,"%*s: %10s\n",iceb_tu_kolbait(58,gettext("Итого")),gettext("Итого"),prnbr(iuder));
fprintf(ff,"%*s: %10s\n",iceb_tu_kolbait(58,gettext("Сальдо")),gettext("Сальдо"),prnbr(inah+iobud+iuder));

}
