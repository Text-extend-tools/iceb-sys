/* $Id: podtdok1.c,v 1.20 2013/09/26 09:43:37 sasa Exp $ */
/*15.07.2015     30.5.1995       Белых А.И.      podtdok1.c
Подтверждение всех записей в документе сразу
*/
#include	"buhg.h"


void podtdok1(const char *datdok, //Дата документа
int skl,const char *nomdok,
int tipz,
const char *datpod, //Дата подтверждения
int metkaeho) //0-выдавать на экран 1-работать молча
{
short dd=0,md=0,gd=0;
short d1=0,m1=0,g1=0;
rsdat(&dd,&md,&gd,datdok,1);
rsdat(&d1,&m1,&g1,datpod,1);

podtdok1(dd,md,gd,skl,nomdok,tipz,d1,m1,g1,metkaeho);
}
/*************************************/

void podtdok1(short dd,short md,short gd, //Дата документа
int skl,const char *nomdok,
int tipz,
short d1,short m1,short g1, //Дата подтверждения
int metkaeho) //0-выдавать на экран 1-работать молча
{
class ostatokt ost;
double          kol1,kol2;
short           dp,mp,gp;
SQL_str         row,row1;
int		kolstr;
char		strsql[512];
class iceb_tu_str naimat("");
int		nk;
double		cena;
struct  tm      *bf;
time_t          tmm;


time(&tmm);
bf=localtime(&tmm);

short mmm=bf->tm_mon+1;
short ggg=bf->tm_year+1900;

if(metkaeho == 0)
 {
  clear();
  GDITE(); 
 }

sprintf(strsql,"select * from Dokummat1 where datd='%d-%02d-%02d' and \
sklad=%d and nomd='%s' order by kodm asc",
gd,md,dd,skl,nomdok);
SQLCURSOR cur;
SQLCURSOR cur1;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 msql_error(&bd,__FUNCTION__,strsql);

if(kolstr == 0 )
 {
  if(metkaeho == 0)
   {
    iceb_t_soob(gettext("Не найдено ни одной записи !"));
   }
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  nk=atoi(row[3]);
  if(nk == 0)
   {
    if(metkaeho == 0)
      printw(gettext("не привязана к карточке\n"));
    continue;
   }
  kol1=atof(row[5]);
  cena=atof(row[6]);
  if(metkaeho == 0)
   {
    /*Узнаем наименование материалла*/
    sprintf(strsql,"select naimat from Material where kodm=%s",
    row[4]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      naimat.new_plus("");
        beep();
        printw("%s %s !\n",gettext("Не найден код материалла"),row[4]);
        OSTANOV();
     }
    else
      naimat.new_plus(row1[0]);

    printw("%-4s %-4s %-*.*s %10.10g - ",
    row[3],row[4],
    iceb_tu_kolbait(20,naimat.ravno()),
    iceb_tu_kolbait(20,naimat.ravno()),
    naimat.ravno(),
    kol1);
    refresh();
   }

  if(tipz == 2)
   {
    prdpp(skl,nk,&dp,&mp,&gp);

    if(SRAV1(g1,m1,d1,gp,mp,dp) > 0)
     {
      if(metkaeho == 0)
       {
        beep();
        printw(gettext("\nДата подтверждения (%d.%d.%dг.) меньше чем дата первого прихода (%d.%d.%dг.)\n\
Такой расход не может быть ! Не подтверждено.\n"),
        d1,m1,g1,dp,mp,gp);
        OSTANOV();
       }
      continue;
     }
   }

  /*Читаем в карточке количество реально выданное-полученное*/

  kol2=readkolk(skl,nk,dd,md,gd,nomdok);  

  ostkar(1,1,ggg,31,mmm,ggg,skl,nk,&ost);
  if(kol1 > kol2)
   {
     if(tipz == 2 && kol1-kol2 - ost.ostg[3] > 0.000000001)
      {
/*
       printw("\nkol1=%f-kol2=%f-ost.ostg[3]=%f > %f\n",
       kol1,kol2,ost.ostg[3],kol1-kol2-ost.ostg[3]); 
*/
       if(metkaeho == 0)
         printw(gettext("неподтверждена (количество)\n"));
       continue;
      } 
     if(metkaeho == 0)
       printw(gettext("подтверждена\n"));
     zapvkr(d1,m1,g1,nomdok,skl,nk,dd,md,gd,tipz,kol1-kol2,cena);
   }
  else
   if(metkaeho == 0)
     printw(gettext("подтверждена раньше\n"));


 }

if(metkaeho == 0)
 {
  printw("\n");
  OSTANOV();
 }
}
