/* $Id: rasnuptn.c,v 5.21 2013/09/26 09:43:38 sasa Exp $ */
/*15.07.2015	08.07.1999	Белих А.И.	rasnuptn.c
Распечатка всех начислений и удержаний за определенный месяц
с протоколом выдач в счёт этого месяца
*/
#include        <errno.h>
#include        "buhg.h"

extern short	ddd,mmm,ggg;

void rasnuptn(short mn,short gn,long tabn,const char *fio)
{
time_t          tmm;
struct  tm      *bf;
long		kolstr;
SQL_str         row,row1;
char		strsql[512];
short		d,m,g;
FILE		*ff;
char		imaf[64],imaf1[64];
int		knah,prn;
double		sym;
class iceb_tu_str nai("");
double		itog;
char		priz[8];
double		dnei,hass,kdnei;
double		idnei,ihass,ikdnei;
double		sal;
double		inah,iuder;
time_t		vrem;
double		saldb;
SQLCURSOR curr;
GDITE();
sprintf(imaf,"rnu%ld.lst",tabn);
sprintf(imaf1,"rnu1%ld.lst",tabn);

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }
fprintf(ff,"%s\n%s\n%ld %s\n%s %d.%d%s\n\
------------------------------------------------------------------------------\n",
iceb_t_get_pnk("00",0),
gettext("Распечатка всех начислений и удержаний"),
tabn,fio,
gettext("За"),
mn,gn,
gettext("г."));

printw("%s %d.%d%s\n\%ld %s\n",
gettext("Распечатка всех начислений и удерзжаний за"),
mn,gn,
gettext("г."),
tabn,fio);

time(&tmm);
bf=localtime(&tmm);
fprintf(ff,"\
%s %d.%d.%d%s  %s: %d:%d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);


sprintf(strsql,"select kodt,dnei,has,kdnei from Ztab where \
god=%d and mes=%d and tabn=%ld",gn,mn,tabn);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

fprintf(ff,"%s\n\
-----------------------------------------------------------------------------\n",
gettext("Введенный табель:"));
printw("%s\n\
----------------------------------------------------------------------------\n",
gettext("Введенный табель:"));
GDITE();

if(kolstr == 0)
 {
  printw(gettext("За %d.%dг. табель не введено !"),mn,gn);
  printw("\n");
  fprintf(ff,gettext("За %d.%dг. табель не введено !"),mn,gn);
  fprintf(ff,"\n");
 }

idnei=ihass=ikdnei=0.;

while(cur.read_cursor(&row) != 0)
 {

  /*Читаем наименование табеля*/
  sprintf(strsql,"select naik from Tabel where kod=%s",row[0]);
  
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    beep();
    printw("%s %s !\n",gettext("Не найден код табеля"),row[0]);
    OSTANOV();
    nai.new_plus("");
   }
  else
    nai.new_plus(row1[0]);

  dnei=atof(row[1]);
  hass=atof(row[2]);
  kdnei=atof(row[3]);

  printw("%3s %-*.*s %s%2.f %s%3.f %s%2.f\n",
  row[0],
  iceb_tu_kolbait(30,nai.ravno()),iceb_tu_kolbait(30,nai.ravno()),nai.ravno(),
  gettext("д"),dnei,
  gettext("ч"),hass,
  gettext("кд"),kdnei);

  fprintf(ff,"%3s %-*.*s %s%2.f %s%3.f %s%2.f\n",
  row[0],
  iceb_tu_kolbait(30,nai.ravno()),iceb_tu_kolbait(30,nai.ravno()),nai.ravno(),
  gettext("д"),dnei,
  gettext("ч"),hass,
  gettext("кд"),kdnei);

  idnei+=dnei;
  ihass+=hass;
  ikdnei+=kdnei;

 }
printw("%*s: %s%2.f %s%3.f %s%2.f\n",
iceb_tu_kolbait(33,gettext("Итого")),gettext("Итого"),
gettext("д"),idnei,
gettext("ч"),ihass,
gettext("кд"),ikdnei);

fprintf(ff,"%*s: %s%2.f %s%3.f %s%2.f\n",
iceb_tu_kolbait(33,gettext("Итого")),gettext("Итого"),
gettext("д"),idnei,
gettext("ч"),ihass,
gettext("кд"),ikdnei);

printw("\
----------------------------------------------------------------------------\n");
fprintf(ff,"\
----------------------------------------------------------------------------\n");

sprintf(strsql,"select * from Zarp where datz >='%d-%02d-01' and \
datz <= '%d-%02d-31' and tabn=%ld and suma <> 0. \
order by datz,prn,knah asc",gn,mn,gn,mn,tabn);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  fclose(ff);
  return;
 }

itog=0.;
sal=zarsald(1,mn,gn,tabn,&saldb);

inah=iuder=0.;
if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
/*
  printw("%s %s %s\n",row[0],row[1],row[2]);
  refresh();
*/
  rsdat(&d,&m,&g,row[0],2);
  prn=atoi(row[2]);
  knah=atoi(row[3]);
  sym=atof(row[4]);
  itog+=sym;
  memset(priz,'\0',sizeof(priz));  
  if(prn == 1)
   {
    inah+=sym;
    strcpy(priz,"+");
    sprintf(strsql,"select naik from Nash where kod=%d",knah);
   }
  if(prn == 2)
   {
    iuder+=sym;
    strcpy(priz,"-");
    sprintf(strsql,"select naik from Uder where kod=%d",knah);
   }     
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

  printw("%2d %02d.%02d.%d %s %-*.*s %2s.%s %-*s %10s %s\n",
  knah,d,m,g,priz,
  iceb_tu_kolbait(30,nai.ravno()),iceb_tu_kolbait(30,nai.ravno()),nai.ravno(),
  row[7],
  row[6],
  iceb_tu_kolbait(4,row[5]),row[5],
  prnbr(sym),row[12]);

  vrem=atol(row[11]);
  bf=localtime(&vrem);

  fprintf(ff,"%2d %02d.%02d.%d %s \
%-*.*s %2s.%s %-*s %10s %s %s %d.%d.%d %d:%d:%d\n",
  knah,d,m,g,priz,
  iceb_tu_kolbait(30,nai.ravno()),iceb_tu_kolbait(30,nai.ravno()),nai.ravno(),
  row[7],row[6],
  iceb_tu_kolbait(4,row[5]),row[5],
  prnbr(sym),row[12],iceb_t_kszap(row[10]),
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
  bf->tm_hour,bf->tm_min,bf->tm_sec);

 }

printw("%*s %10s\n",
iceb_tu_kolbait(59,gettext("Сальдо на начало месяца:")),gettext("Сальдо на начало месяца:"),prnbr(sal));
fprintf(ff,"%59s %10s\n",gettext("Сальдо на початок місяця:"),prnbr(sal));

printw("%*s %10s\n",
iceb_tu_kolbait(59,gettext("Итого начислено:")),gettext("Итого начислено:"),prnbr(inah));
fprintf(ff,"%59s %10s\n",gettext("Итого начислено:"),prnbr(inah));

printw("%*s %10s\n",
iceb_tu_kolbait(59,gettext("Итого удержано:")),gettext("Итого удержано:"),prnbr(iuder));
fprintf(ff,"%*s %10s\n",
iceb_tu_kolbait(59,gettext("Итого удержано:")),gettext("Итого удержано:"),prnbr(iuder));

printw("%*s: %10s\n",iceb_tu_kolbait(58,gettext("Итого")),gettext("Итого"),prnbr(itog));
fprintf(ff,"%*s: %10s\n",iceb_tu_kolbait(58,gettext("Итого")),gettext("Итого"),prnbr(itog));

printw("%*s %10s\n",iceb_tu_kolbait(59,gettext("Сальдо на конец месяца:")),gettext("Сальдо на конец месяца:"),prnbr(itog+sal));
fprintf(ff,"%*s %10s\n",iceb_tu_kolbait(59,gettext("Сальдо на конец месяца:")),gettext("Сальдо на конец месяца:"),prnbr(itog+sal));



fprintf(ff,"\n%s\n\
----------------------------------------------------------------------------\n",
gettext("Начисления и удержания в счёт этого месяца:"));
printw("\n%s\n\
----------------------------------------------------------------------------\n",
gettext("Начисления и удержания в счёт этого месяца:"));

GDITE();

sprintf(strsql,"select * from Zarp where godn=%d and \
mesn=%d and datz > '%d-%02d-31' and \
tabn=%ld and suma <> 0. order by datz,prn,knah asc",
gn,mn,gn,mn,tabn);

/*
sprintf(strsql,"select * from Zarp where godn=%d and \
mesn=%d and tabn=%ld order by prn,knah asc",
gn,mn,tabn);
*/
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
 }

itog=0.;
if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {

  rsdat(&d,&m,&g,row[0],2);
  prn=atoi(row[2]);
  knah=atoi(row[3]);
  sym=atof(row[4]);
  itog+=sym;
  memset(priz,'\0',sizeof(priz));  
  if(prn == 1)
   {
    strcpy(priz,"+");
    sprintf(strsql,"select naik from Nash where kod=%d",knah);
   }
  if(prn == 2)
   {
    strcpy(priz,"-");
    sprintf(strsql,"select naik from Uder where kod=%d",knah);
   }     
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

  printw("%2d %02d.%02d.%d %s %-*.*s %2s.%s %-*s %10s %s\n",
  knah,d,m,g,priz,
  iceb_tu_kolbait(30,nai.ravno()),iceb_tu_kolbait(30,nai.ravno()),nai.ravno(),
  row[7],row[6],
  iceb_tu_kolbait(4,row[5]),row[5],
  prnbr(sym),row[12]);

  vrem=atol(row[11]);
  bf=localtime(&vrem);

  fprintf(ff,"%2d %02d.%02d.%d %s %-*.*s %2s.%s %-*s %10s %s %s %d.%d.%d %d:%d:%d\n",
  knah,d,m,g,priz,
  iceb_tu_kolbait(30,nai.ravno()),iceb_tu_kolbait(30,nai.ravno()),nai.ravno(),
  row[7],row[6],
  iceb_tu_kolbait(4,row[5]),row[5],
  prnbr(sym),row[12],iceb_t_kszap(row[10]),
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
  bf->tm_hour,bf->tm_min,bf->tm_sec);

 }

printw("%*s: %10s\n",iceb_tu_kolbait(58,gettext("Итого")),gettext("Итого"),prnbr(itog));
fprintf(ff,"%*s: %10s\n",iceb_tu_kolbait(58,gettext("Итого")),gettext("Итого"),prnbr(itog));

fclose(ff);

sprintf(imaf1,"rnu1%ld.lst",tabn);
if((ff = fopen(imaf1,"w")) == NULL)
 {
  error_op_nfil(imaf1,errno,"");
  return;
 }

rasnuptn1(mn,gn,0,0,0,tabn,fio,ff);

fclose(ff);
printw("%s %s,%s\n",
gettext("Распечатка выгружена в файлы"),imaf,imaf1);

helstr(LINES-1,0,"Enter",gettext("работа с файлом"),
"F10",gettext("выход"),NULL);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка начислений и удержаний"));
oth.spis_imaf.plus(imaf1);
oth.spis_naim.plus(gettext("Распечатка начислений и удержаний без сальдо"));

ppp:;

d=getch();
switch(d)
 {
  case FK10:
  case ESC:
    for(int nom=0; nom < oth.spis_imaf.kolih(); nom++)
      unlink(oth.spis_imaf.ravno(nom));
    break;

  case ENTER:
  case FK2:
  case PLU:
    for(int nom=0; nom < oth.spis_imaf.kolih(); nom++)
     iceb_t_ustpeh(oth.spis_imaf.ravno(nom),0);
    iceb_t_rabfil(&oth,"");
    break;  

  default:
    goto ppp;
 }


}

