/*$Id: prosprkr.c,v 5.20 2013/08/13 05:49:51 sasa Exp $*/
/*24.05.2013	28.10.2002	Белых А.И.	prosprkr.c
Определение сумм по счетам для выполнения проводок для командировочных расходов
*/
#include        <math.h>
#include        "buhg.h"

extern double   okrg1; /*Округление*/


void prosprkr(short dd,short md,short gd,
const char *nomdok,
int metkareg) //0-суммы на экран 1-проверить все ли проводки сделаны
{
char		strsql[1024];
int		kolstr=0;
SQL_str		row;
int		nomer=0;
double		suma;
int             xv=COLS-26; 
int             yv=0;
double		itogo=0.;
int		kolshet=0;
int		i=0;
int		metkaprov=0;
int		metkavpr=0; //0-проводки не все сделаны 1-все

SPISOK SHETA(0);  //Список счетов
masiv_din_double SUM(0); //Суммы по счетам

sprintf(strsql,"select shet,kolih,cena,ss from Ukrdok1 \
where datd='%04d-%02d-%02d'and nomd='%s'",
gd,md,dd,nomdok);  
SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }
if(kolstr == 0)
  return;

while(cur.read_cursor(&row) != 0)
 {
  suma=atof(row[2])+atof(row[3]);
  suma=okrug(suma,okrg1);
  
  if((nomer=SHETA.find(row[0])) == -1)
   {
    SHETA.plus(row[0]); 
    kolshet++;
   }
  SUM.plus(suma,nomer);

 }

if(metkareg == 0)  //Вывод на экран
 {
  itogo=0.;

  for(i=0; i < kolshet ; i++)
   {
    mvprintw(yv++,xv,"%-*s %14s",
    iceb_tu_kolbait(8,SHETA.ravno(i)),
    SHETA.ravno(i),
    prnbr(SUM.ravno(i)));
    itogo+=SUM.ravno(i);
   }

  mvprintw(yv++,xv,"%-*s %14s",iceb_tu_kolbait(8,gettext("Итого")),gettext("Итого"),prnbr(itogo));

 }

if(metkareg == 1)  //Проверить все ли проводки сделаны
 {
  double sumaprov[kolshet];
  memset(&sumaprov,'\0',sizeof(sumaprov));
  
  sprintf(strsql,"select sh,shk,deb,kre from Prov \
where kto='%s' and pod=%d and nomd='%s' and datd='%04d-%02d-%02d'",
  ICEB_MP_UKR,0,nomdok,gd,md,dd);


  //printw("\n%s\n",strsql);
  //OSTANOV();

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
    return;
   }
  //printw("kolstr=%d\n",kolstr);
  //refresh();

  while(cur.read_cursor(&row) != 0)
   {
  //  printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
    metkaprov++;

    i=SHETA.find(row[1]);

    sumaprov[i]+=atof(row[3]);

   }
  VVOD SOOB(1);

  for(i=0; i < kolshet ; i++)
   {
    if(fabs(sumaprov[i] - SUM.ravno(i)) > 0.009)
     {
      metkavpr++;
      sprintf(strsql,gettext("По счету %s не сделаны все проводки ! %.14g != %.14g %d/%d"),
      SHETA.ravno(i),sumaprov[i],SUM.ravno(i),i,kolshet);
      SOOB.VVOD_SPISOK_add_MD(strsql);
     }
   }

  if(metkavpr > 0)
   {
    sprintf(strsql,"%s %s %s %d.%d.%dг.",gettext("Документ"),nomdok,gettext("дата"),dd,md,gd);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
   }

  if(metkavpr == 0 )
   {
    /*Проводки выполнены*/
    sprintf(strsql,"update Ukrdok \
set mprov=1 where datd='%04d-%02d-%02d' and nomd='%s'",
    gd,md,dd,nomdok);
   }
  else
   {
   //Если документ заблокирован то никаких меток не ставим
    if(iceb_t_pbpds(md,gd,1) != 0)
     return;

    /*Проводки не выполнены*/
    sprintf(strsql,"update Ukrdok \
set mprov=0 where datd='%04d-%02d-%02d' and nomd='%s'",
    gd,md,dd,nomdok);

   }

  if(sql_zap(&bd,strsql) != 0)
   if(sql_nerror(&bd) != ER_DBACCESS_DENIED_ERROR)
    {
     msql_error(&bd,__FUNCTION__,strsql);
    }


 }



}
