/*$Id: usldopnal.c,v 5.13 2013-04-07 10:22:44 sasa Exp $*/
/*04.11.2000	21.03.2002	Белых А.И.	usldopnal.c
Определение процента и наименования дополнительного налога
Если вернули 0 нашли налог
             1 нет
*/

#include        <errno.h>
#include        "buhg.h"


int	usldopnal(int tipz, // 1+ 2 -
const char *kodop, //Код операции
float *procent,
class iceb_tu_str *naim)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
char strsql[512];
int kolstr=0;
class iceb_tu_str bros("");
int	vozvr=1;

*procent=0.;

sprintf(strsql,"select str from Alx where fil='uslnast.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"uslnast.alx");
  iceb_t_soob(strsql);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  
  if(polen(row_alx[0],&bros,2,'|') != 0)
    continue;
  if(tipz == 1 && strstrm(bros.ravno(),"+") != 1)
    continue;
  if(tipz == 2 && strstrm(bros.ravno(),"-") != 1)
    continue;
    
  if(polen(row_alx[0],&bros,3,'|') != 0)
    continue;
  if(proverka(bros.ravno(),kodop,0,1) != 0)
    continue;

  if(polen(row_alx[0],&bros,4,'|') != 0)
    continue;
  *procent=bros.ravno_atof();
  if(polen(row_alx[0],&bros,5,'|') != 0)
    continue;
  naim->new_plus(bros.ravno());
  vozvr=0;
  break;  
 }
 
return(vozvr);
}
