/*$Id: findkar.c,v 5.16 2013/05/17 14:56:02 sasa Exp $*/
/*08.12.2009	06.11.2001	Белых А.И.  	findkarf.c
Поиск карточек к которым можно привязать заданное количество материалла
Возвращает количество карточек к которым можно привязаться
           0- не найдено ни одной карточки с которой можно списать
              нужное количество
           -1 карточек вообще нет
*/
#include        <math.h>
#include "bezcurses.h"


int findkar(short dd,short md,short gd, //Дата привязки
int sklad,int kodmat,double kolih,
masiv_din_int *nomkart, //Массив с номерами карточек
masiv_din_double *kolihp, //Массив с количеством, которое можно привязать к карточкам
int kodopuc, //0-к любой 1-с нужной ценой
double cena, //Цена карточки к которой надо привязать
const char *shetu) //Счет учета
{
class ostatokt ost;
SQL_str		row;
int		kolstr;
char		strsql[1024];
double		kolihpriv=0.;
int		nomkar=0;
int		kolkar=0;

if(kodopuc == 0)
 sprintf(strsql,"select nomk from Kart where sklad=%d and kodm=%d",
 sklad,kodmat);

if(kodopuc == 1)
 sprintf(strsql,"select nomk from Kart where sklad=%d and kodm=%d and cena=%.10g",
sklad,kodmat,cena);

if(shetu[0] != '\0')
 {
  strcat(strsql," and shetu='");
  strcat(strsql,shetu);
  strcat(strsql,"'");
 }

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  msql_error(&bd,"__FUNCTION__",strsql);
if(kolstr == 0)
  return(-1);
while(cur.read_cursor(&row) != 0)
 {
  nomkar=atoi(row[0]);
  ostkar(1,1,gd,dd,md,gd,sklad,nomkar,&ost);
  if(ost.ostg[3] < 0.0000001)
    continue;

  kolkar++;
  nomkart->plus(nomkar,-1);

  if(ost.ostg[3] > kolih-kolihpriv)
   {
    kolihp->plus(kolih-kolihpriv,-1);
    kolihpriv+=kolih-kolihpriv;
   }
  else
   {
    kolihp->plus(ost.ostg[3],-1);
    kolihpriv+=ost.ostg[3];
   }

  if(fabs(kolih-kolihpriv) < 0.0000001)
    return(kolkar);
 }
return(kolkar);
}
