/* $Id: podtdok1.c,v 1.19 2013/08/13 05:49:50 sasa Exp $ */
/*08.12.2009     30.5.1995       Белых А.И.      podtdok1f.c
Подтверждение всех записей в документе сразу
*/
#include        <time.h>
#include        <math.h>
#include "bezcurses.h"


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

sprintf(strsql,"select * from Dokummat1 where datd='%d-%02d-%02d' and \
sklad=%d and nomd='%s' order by kodm asc",
gd,md,dd,skl,nomdok);
SQLCURSOR cur;
SQLCURSOR cur1;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 msql_error(&bd,__FUNCTION__,strsql);

if(kolstr <= 0 )
 {
  if(metkaeho == 0)
   {
    printf("%s\n",gettext("Не найдено ни одной записи !"));
   }
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  nk=atoi(row[3]);
  if(nk == 0)
   {
    if(metkaeho == 0)
      printf(gettext("не привязана к карточке\n"));
    continue;
   }
  kol1=atof(row[5]);
  cena=atof(row[6]);
  if(metkaeho == 0)
   {
    /*Узнаем наименование материалла*/
    sprintf(strsql,"select naimat from Material where kodm=%s",row[4]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      naimat.new_plus("");
      printf("%s-%s %s !\n",__FUNCTION__,gettext("Не найден код материалла"),row[4]);
     }
    else
      naimat.new_plus(row1[0]);

    printf("%-4s %-4s %-*.*s %10.10g - ",
    row[3],row[4],
    iceb_tu_kolbait(20,naimat.ravno()),
    iceb_tu_kolbait(20,naimat.ravno()),
    naimat.ravno(),
    kol1);
   }

  if(tipz == 2)
   {
    prdpp(skl,nk,&dp,&mp,&gp);

    if(SRAV1(g1,m1,d1,gp,mp,dp) > 0)
     {
      if(metkaeho == 0)
       {
        printf(gettext("\nДата подтверждения (%d.%d.%dг.) меньше чем дата первого прихода (%d.%d.%dг.)\n\
Такой расход не может быть ! Не подтверждено.\n"),
        d1,m1,g1,dp,mp,gp);
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
       if(metkaeho == 0)
         printf(gettext("неподтверждена (количество)\n"));
       continue;
      } 
     if(metkaeho == 0)
       printf(gettext("подтверждена\n"));
     zapvkr(d1,m1,g1,nomdok,skl,nk,dd,md,gd,tipz,kol1-kol2,cena);
   }
  else
   if(metkaeho == 0)
     printf(gettext("подтверждена раньше\n"));

 }

}
