/*14.06.2017	10.06.2004	Белых 	kusnast.c
Чтение настроек
*/

#include <locale.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <iceb.h>
#include <buhg.h>

extern SQL_baza	bd;
extern char		*imabaz;
extern char		*putnansi;

float kvartp=0.; //квартирная плата
float otop=0.;   //отопление
float gas=0.;    //газ
float vbv=0.;    //вода без ванны
float vsv=0.;    //вода с ванной
float sbv=0.;    //стоки без ванны
float ssv=0.;    //стоки с ванной
float elek=0.;   //електроенергия
float nds=0.;    //НДС
float radio=0.;  //Стоимость радиоточки
float cena_vodi=0.; //Цена воды по счётчику
float cena_stoka=0.; //Цена стока по счётчику
float musor=0.;       /*цена вывоза мусора на 1 человека*/
class iceb_tu_str shin; /*Счёт сальдо по услугам оказанных организацией*/
class iceb_tu_str shout; /*Счёт сальдо услугам оказанных другими организациями*/

short		vplsh=1; /*0-двух уровневый план счетов 1-многоуровневый*/
short		startgodb=0; /*Стартовый год главной книги*/

int kusnast ()
{
SQL_str rowalx;
class SQLCURSOR curalx;
int kolstr=0;
char strsql[512];
char st1[1024];
cena_vodi=kvartp=otop=gas=vbv=vsv=elek=nds=radio=0.;
startgodb=0;
vplsh=1;


sprintf(strsql,"select str from Alx where fil='%s' order by ns asc","kusnast.alx");

if((kolstr=curalx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,"",strsql);
  return(1);
 }  

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s",gettext("Не найден файл"),"kusnast.alx");
  iceb_t_soob(strsql);
  return(1);
 }

while(curalx.read_cursor(&rowalx) != 0)
 {

  printw("%s",rowalx[0]);
  refresh();
    
  polen(rowalx[0],st1,sizeof(st1),1,'|');

  if(SRAV(st1,"Квартирная плата",0) == 0)
   {
    polen(rowalx[0],st1,sizeof(st1),2,'|');
    kvartp=ATOFM(st1);
    continue;
   }

  if(SRAV(st1,"Отопление",0) == 0)
   {
    polen(rowalx[0],st1,sizeof(st1),2,'|');
    otop=ATOFM(st1);
    continue;
   }
  if(SRAV(st1,"Газ",0) == 0)
   {
    polen(rowalx[0],st1,sizeof(st1),2,'|');
    gas=ATOFM(st1);
    continue;
   }
  if(SRAV(st1,"Вода без ванны",0) == 0)
   {
    polen(rowalx[0],&vbv,2,'|');
    continue;
   }
  if(SRAV(st1,"Вода с ванной",0) == 0)
   {
    polen(rowalx[0],&vsv,2,'|');
    continue;
   }
  if(SRAV(st1,"Стоки без ванны",0) == 0)
   {
    polen(rowalx[0],&sbv,2,'|');
    continue;
   }
  if(SRAV(st1,"Стоки с ванной",0) == 0)
   {
    polen(rowalx[0],&ssv,2,'|');
    continue;
   }
 if(SRAV(st1,"Электроэнергия",0) == 0)
   {
    polen(rowalx[0],st1,sizeof(st1),2,'|');
    elek=ATOFM(st1);
    continue;
   }

 if(SRAV(st1,"Радиоточка",0) == 0)
   {
    polen(rowalx[0],st1,sizeof(st1),2,'|');
    radio=ATOFM(st1);
    continue;
   }

 if(SRAV(st1,"Цена воды по счётчику",0) == 0)
   {
    polen(rowalx[0],st1,sizeof(st1),2,'|');
    cena_vodi=ATOFM(st1);
    continue;
   }

 if(SRAV(st1,"Цена стоков по счётчику",0) == 0)
   {
    polen(rowalx[0],&cena_stoka,2,'|');
    continue;
   }

 if(SRAV(st1,"Цена вывоза мусора",0) == 0)
   {
    polen(rowalx[0],st1,sizeof(st1),2,'|');
    musor=ATOFM(st1);
    continue;
   }

 if(SRAV(st1,"Счёт сальдо 1",0) == 0)
   {
    polen(rowalx[0],&shin,2,'|');
    continue;
   }
 if(SRAV(st1,"Счёт сальдо 2",0) == 0)
   {
    polen(rowalx[0],&shout,2,'|');
    continue;
   }


 }

if(iceb_t_poldan("Многопорядковый план счетов",st1,"nastb.alx") == 0)
 {
 if(SRAV(st1,"Вкл",1) == 0)
  vplsh=1;
 else
  vplsh=0;
 }
if(iceb_t_poldan("Стартовый год",st1,"nastb.alx") == 0)
 startgodb=atoi(st1);
   
return(0);
}


