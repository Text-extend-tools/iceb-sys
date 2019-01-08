/* $Id: ostkar.c,v 5.18 2013/08/29 05:37:31 sasa Exp $ */
/*26.08.2013    21.04.1997      Белых А.И.      ostkarf.c
получение остатка материалла по карточкам
		       С начала года         Период
		       включая период
		       кол.        цена      кол.   цена
Остаток начальный   0  000         000       000    000
Приход              1  000         000       000    000
Расход              2  000         000       000    000
Остаток             3  000         000       000    000
*/
#include        <math.h>
#include "bezcurses.h"

extern short	startgod; /*Стартовый год просмотров*/

void ostkar(short dn,short mn,short gn,short dk,short mk,
short gk,int skl,int nk,class ostatokt *ost)
{
double   okrg1=0.000001; /*Округление*/
double	okrcn=0.01;
short           dp,mp,gp;
short		d,m,g;
double          bb;
double          prf,prf1;
double          cnn=0.,cnn_real=0.;
short           i;
short           mkg;
short 		tp;
double		kolih,cena=0.,cena_real=0.;
SQL_str         row;
char		strsql[1024];
int		kolstr;
double		fas;
short		godn;
class SQLCURSOR cur;

godn=startgod;
if(godn == 0)
 godn=gn;
if(gn < startgod)
  godn=gn; 
/*Читаем реквизиты карточки материалла*/
sprintf(strsql,"select ei,cena,fas,cenap from Kart where sklad=%d and nomk=%d",
skl,nk);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {

  printf("%s-%s %d %s %d!",__FUNCTION__,gettext("Не найдена карточка"),nk,gettext("на складе"),skl);
  return;
 }
cena=atof(row[1]);
cena=okrug(cena,okrcn);
cena_real=atof(row[3]);
cena_real=okrug(cena_real,okrcn);

fas=atof(row[2]);

mkg=0;
if(SRAV(row[0],"кг",0) != 0 && SRAV(row[0],"Кг",0) != 0 &&
 SRAV(row[0],"КГ",0) != 0)
   mkg=1;

if(mn == 0)
 mn=1;
if(mk == 0)
 mk=12;

dp=dn;
mp=mn;
gp=gn;

dpm(&dp,&mp,&gp,2);

ost->clear();

sprintf(strsql,"select datdp,nomd,kolih,tipz from Zkart where sklad=%d \
and nomk=%d and datdp >= '%04d-01-01' and datdp <= '%04d-%02d-%02d' \
order by datdp asc",skl,nk,godn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return;
 }

if(kolstr == 0)
 {
  return;
 } 

cnn=prf=prf1=0.;
while(cur.read_cursor(&row) != 0)
 {
/*
  printw("%s %s %s %s\n",
  row[0],row[1],row[2],row[3]);
*/

  rsdat(&d,&m,&g,row[0],2);

  tp=atoi(row[3]);
  kolih=atof(row[2]);
        
  if(SRAV1(gk,mk,dk,g,m,d) > 0)
     break;

  if(tp == 1 && SRAV(row[1],"000",0) == 0 && g != godn)
    continue;
  if(tp == 1 && SRAV(row[1],"000",0) == 0 && g == godn)
   {
    /*printw("Остаток\n");*/
    ost->ostg[0]+=kolih;
    if(mkg == 1 )
      ost->kgg[0]+=kolih*fas;
    else
      ost->kgg[0]+=kolih;
    
    bb=kolih*cena;
    ost->ostgc[0]=okrug(bb,okrg1);
    bb=kolih*cena_real;
    ost->ostgc_real[0]=okrug(bb,okrg1);
    continue;
   }

  ost->ostg[tp]+=kolih;
  if(mkg == 1 )
    ost->kgg[tp]+=kolih*fas;
  else 
    ost->kgg[tp]+=kolih;

  bb=kolih*cena;
  ost->ostgc[tp]+=okrug(bb,okrg1);
  bb=kolih*cena_real;
  ost->ostgc_real[tp]+=okrug(bb,okrg1);

    /*Период*/
  if(SRAV1(gn,mn,dn,g,m,d) >= 0)
   if(SRAV1(gk,mk,dk,g,m,d) <= 0)
     {
/*      printw("Период\n");*/
      ost->ostm[tp]+=kolih;
      if(mkg == 1 )
	 ost->kgmm[tp]+=kolih*fas;
      else
	 ost->kgmm[tp]+=kolih;
      bb=kolih*cena;
      ost->ostmc[tp]+=okrug(bb,okrg1);
      bb=kolih*cena_real;
      ost->ostmc_real[tp]+=okrug(bb,okrg1);
     }

    /*До периода*/
  if(SRAV1(gp,mp,dp,g,m,d) <= 0)
   {
/*      printw("До периода\n");*/

      if(tp == 2)
	kolih*=(-1);

      prf+=kolih;

      if(mkg == 1 )
	  prf1+=kolih*fas;
      else
	  prf1+=kolih;

      bb=kolih*cena;
      cnn+=okrug(bb,okrg1);

      bb=kolih*cena_real;
      cnn_real+=okrug(bb,okrg1);
   }
 }
//OSTANOV();
ost->ostg[3]=ost->ostg[0]+ost->ostg[1]-ost->ostg[2];
ost->kgg[3]=ost->kgg[0]+ost->kgg[1]-ost->kgg[2];
ost->ostm[0]=ost->ostg[0]+prf;
ost->kgmm[0]=ost->kgmm[0]+prf1;
ost->ostm[3]=ost->ostm[0]+ost->ostm[1]-ost->ostm[2];
ost->kgmm[3]=ost->kgmm[0]+ost->kgmm[1]-ost->kgmm[2];

ost->ostgc[3]=ost->ostgc[0]+ost->ostgc[1]-ost->ostgc[2];

ost->ostmc[0]=ost->ostgc[0]+cnn;
ost->ostmc[3]=ost->ostmc[0]+ost->ostmc[1]-ost->ostmc[2];

ost->ostmc_real[0]=ost->ostgc_real[0]+cnn_real;
ost->ostmc_real[3]=ost->ostmc_real[0]+ost->ostmc_real[1]-ost->ostmc_real[2];

/*Убираем погрешность вычисления*/
for(i=0; i<4 ; i++)
 {
  if(ost->ostg[i] < 0.0000000001 && ost->ostg[i] > -0.0000000001)
     ost->ostg[i]=0.;
  if(ost->ostm[i] < 0.0000000001 && ost->ostm[i] > -0.0000000001)
    ost->ostm[i]=0.;
 }
/*
printw("Склад:%d карточка:%d Цена:%f\n%f %f\n%f %f\n%f %f\n%f %f\n",
skl,nk,cena,
ost->ostm[0],ost->ostmc[0],ost->ostm[1],ost->ostmc[1],ost->ostm[2],ost->ostmc[2],ost->ostm[3],ost->ostmc[3]);
OSTANOV();
*/
}
