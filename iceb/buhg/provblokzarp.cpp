/* $Id: provblokzarp.c,v 5.12 2013-04-07 10:22:43 sasa Exp $ */
/*15.07.2015	26.05.2001	Белых А.И.	provblokzarp.c
Проверка блокировки корректировки данных в зарплате
*/
#include        "buhg.h"

extern short	flagrk; //0-с каточкой нико не работает 1-работает другой оператор

int	provblokzarp(long tabn,short mp,short gp)
{
char		strsql[512];
SQL_str		row;
int		blok;

if(flagrk != 0)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD(gettext("Карточка заблокирована"));
  SOOB.VVOD_SPISOK_add_MD(gettext("С карточкой уже работает другой оператор !"));
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return(1);

 }

if(iceb_t_pbpds(mp,gp) != 0)
 return(1);

class SQLCURSOR curr;
sprintf(strsql,"select blok from Zarn where god=%d and mes=%d and \
tabn=%ld",gp,mp,tabn);

if(sql_readkey(&bd,strsql,&row,&curr) == 1)
 {
  VVOD SOOB(1);
  blok=atoi(row[0]);
  if(blok == 0)
   return(0);
  sprintf(strsql,"%s %s",gettext("Карточка заблокирована"),iceb_t_kszap(blok));
  iceb_t_soob(strsql);
  return(1);
 }
return(0);
}
