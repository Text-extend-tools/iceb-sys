/*$Id: parolv.c,v 5.9 2013/09/26 09:43:55 sasa Exp $*/
/*28.08.2012	31.01.2001	Белых А.И.
Формирование запроса и проверка пароля
Возвращаем 0 если пароль правильный
           1-неправильный
*/

#include	"buhl.h"


int parolv(VVOD *PAROL,
short d,short m,short g, //текущая дата
int metka) //0-проверка на сумму 1-проверка на ввод даты
{
char		bros[32];
char		parol[32];

if(d == 0)
 poltekdat(&d,&m,&g);

memset(bros,'\0',sizeof(bros));

if(vparol(PAROL,bros,16,stdscr,-1,-1,1) != 0)
  return(1);

if(metka == 1)
 {
  sprintf(parol,"%d+%d+%d",g,m,d);
  if(SRAV(bros,parol,0) != 0)
   {
    iceb_t_soob(gettext("Не верно введён пароль !"));
    return(1);
   }
 }

if(metka == 0)
if(atoi(bros) != d+m+g)
 {
  iceb_t_soob(gettext("Не верно введён пароль !"));
  return(1);
 }    

return(0);

}
/******************************************/
/******************************************/
int parolv(const char *text,
int metka) //0-проверка на сумму 1-проверка на ввод даты
{
class SPISOK str_text;
int kol_str=pole2(text,'\n');
class VVOD par(2);
class iceb_tu_str strzag;

if(kol_str == 0)
  par.VVOD_SPISOK_add_MD(text);
else
 for(int nomer=0; nomer <= kol_str-1;nomer++)
  {
   polen(text,&strzag,nomer+1,'\n');
   par.VVOD_SPISOK_add_MD(strzag.ravno());
  } 

return(parolv(&par,0,0,0,0));

}
