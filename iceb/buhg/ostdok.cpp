/* $Id: ostdok.c,v 5.11 2013/05/17 14:56:06 sasa Exp $ */
/*08.12.2009    21.09.1994      Белых А.И.      ostdok.c
Получение остатка по записям в документах привязанных
к карточкам
		       С начала года         Период
		       включая период
		       кол.        цена      кол.   цена
Остаток начальный   0  000         000       000    000
Приход              1  000         000       000    000
Расход              2  000         000       000    000
Остаток             3  000         000       000    000

*/
#include        <math.h>
#include	"buhg.h"

extern double   okrg1; /*Округление*/
extern short	startgod; /*Стартовый год просмотров*/

void ostdok(short dn,short mn,short gn,short dk,short mk,
short gk,int skk,int nkk,class ostatokt *ost)
{
extern double	okrcn;
double		cena;
int             i;
double          prf;
double          cnn;
short           dp,mp,gp;
short           d,m,g;
double          bb;
double          kol,koll;
short           tz; /*Тип записи*/
SQL_str         row;
char		strsql[512];
int		kolstr=0;
double		kolih=0.;
char		nomd[64];
short		godn;
SQLCURSOR cur;

/*
printw("\n%d.%d.%d %d.%d.%d %d %d\n",dn,mn,gn,dk,mk,gk,skk,nkk);
refresh();
*/
godn=startgod;
if(godn == 0)
 godn=gn;
if(gn < startgod)
  godn=gn; 
 
/*Читаем реквизиты карточки материалла*/
sprintf(strsql,"select cena from Kart where sklad=%d and nomk=%d",skk,nkk);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,gettext("ostkar-Не найдена карточка материалла %d на складе %d !"),nkk,skk);
  iceb_t_soob(strsql);
  return;
 }
cena=atof(row[0]);
cena=okrug(cena,okrcn);



dp=dn;
mp=mn;
gp=gn;

dpm(&dp,&mp,&gp,2);

ost->clear();
cnn=prf=0.;

sprintf(strsql,"select datd,kolih,nomd,tipz from Dokummat1 where \
sklad=%d and nomkar=%d and datd >= '%d-01-01' and \
datd <= '%d-%02d-%02d' order by datd asc",
skk,nkk,godn,gk,mk,dk);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

//printw("%s\nkolstr=%ld\n",strsql,kolstr);
//OSTANOV();

if(kolstr == 0)
  return;

kol=koll=0.;
while(cur.read_cursor(&row) != 0)
 {
/*
  if(skk == 11 && nkk == 90)
    printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  refresh();
*/
  rsdat(&d,&m,&g,row[0],2);    
  kolih=atof(row[1]);
  strcpy(nomd,row[2]);
  
  if(SRAV1(gk,mk,dk,g,m,d) > 0)
   {
/*    printw("%d.%d.%d %d.%d.%d\n",gk,mk,dk,g,m,d); */
     break;
   }

  kol=kolih;

  
  tz=atoi(row[3]);
 
  if(tz == 1 && SRAV(nomd,"000",0) == 0 && godn != g)
     continue;
  if(tz == 1 && SRAV(nomd,"000",0) == 0 && godn == g)
   {
    ost->ostg[0]+=kol;
    bb=kol*cena;
    ost->ostgc[0]=okrug(bb,okrg1);
    continue;
   }

  ost->ostg[tz]+=kol;
  bb=kol*cena;
  ost->ostgc[tz]+=okrug(bb,okrg1);

  /*Период*/
  if(SRAV1(gn,mn,dn,g,m,d) >= 0)
  if(SRAV1(gk,mk,dk,g,m,d) <= 0)
   {
//    printw("Період\n");
    ost->ostm[tz]+=kol;
    bb=kol*cena;
    ost->ostmc[tz]+=okrug(bb,okrg1);
   }

  /*До периода*/
  if(SRAV1(gp,mp,dp,g,m,d) <= 0)
   {
//    printw("До періода\n");
    if(tz == 2)
       kol*=(-1);
    prf+=kol;
    bb=kol*cena;
    cnn+=okrug(bb,okrg1);
   }

 }
ost->ostg[3]=ost->ostg[0]+ost->ostg[1]-ost->ostg[2];
ost->ostm[0]=ost->ostg[0]+prf;
ost->ostm[3]=ost->ostm[0]+ost->ostm[1]-ost->ostm[2];

ost->ostgc[3]=ost->ostgc[0]+ost->ostgc[1]-ost->ostgc[2];
ost->ostmc[0]=ost->ostgc[0]+cnn;
ost->ostmc[3]=ost->ostmc[0]+ost->ostmc[1]-ost->ostmc[2];
/*Убираем погрешность вычисления*/
for(i=0; i<4 ; i++)
 {
  if(ost->ostg[i] < 0.00000000001 && ost->ostg[i] > -0.00000000001)
     ost->ostg[i]=0.;
  if(ost->ostm[i] < 0.00000000001 && ost->ostm[i] > -0.00000000001)
    ost->ostm[i]=0.;
 }

//OSTANOV();
}
