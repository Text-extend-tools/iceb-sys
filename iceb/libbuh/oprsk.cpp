/*$Id: oprsk.c,v 5.14 2013-01-02 10:38:43 sasa Exp $*/
/*01.12.2016	24.03.2003	Белых А.И.	oprsk.c
Определение свободного кода
*/

#include	"buhl.h"

int oprsk(const char *tz)
{
char		strsql[512];
int		kod;
int metka_tip=0;
GDITE();
/*
printw("oprks-%s\n",tz);
OSTANOV();
*/

if(SRAV(tz,"Reegrup1",0) == 0)
 return(0);
if(SRAV(tz,"Reegrup2",0) == 0)
 return(0);

if(SRAV(tz,"Zargrupp",0) == 0)
 return(0);

if(SRAV(tz,"Ukrvkr",0) == 0)
 return(0);

if(SRAV(tz,"Usloper1",0) == 0 || SRAV(tz,"Usloper2",0) == 0)
 return(0);

if(SRAV(tz,"Kasop1",0) == 0 || SRAV(tz,"Kasop2",0) == 0)
 return(0);

if(SRAV(tz,"Rashod",0) == 0)
 return(0);

if(SRAV(tz,"Uospri",0) == 0)
 return(0);

if(SRAV(tz,"Uosras",0) == 0)
 return(0);

if(SRAV(tz,"Uoshau",0) == 0)
 return(0);

if(SRAV(tz,"Uoshz",0) == 0)
 return(0);

if(SRAV(tz,"Uosgrup",0) == 0)
 metka_tip=1;

if(SRAV(tz,"Uosgrup1",0) == 0)
 metka_tip=1;

if(SRAV(tz,"Edizmer",0) == 0)
 return(0);

/*Перечень форм оплаты*/
if(SRAV(tz,"Foroplat",0) == 0)
 return(0);

/*Перечень операций платежных поручений*/
if(SRAV(tz,"Opltp",0) == 0)
 return(0);

/*Перечень операций платежных требований*/
if(SRAV(tz,"Opltt",0) == 0)
 return(0);

/*Перечень фамилий сдающих наличку в банк*/
if(SRAV(tz,"Ksn",0) == 0)
 return(0);

/*Перечень операций сдачи налички в банк*/
if(SRAV(tz,"Oznvb",0) == 0)
 return(0);


for(kod=1; ;kod++)
 {
  if(metka_tip == 0)
   sprintf(strsql,"select kod from %s where kod=%d",tz,kod);
  if(metka_tip == 1)
   sprintf(strsql,"select kod from %s where kod='%d'",tz,kod);
  if(readkey(strsql) == 0)
   break;  
 }
return(kod);
}

