/* $Id: settimmar.c,v 5.11 2013/09/26 09:43:40 sasa Exp $ */
/*20.06.2002	30.01.2001	Белых А.И.	settimmar.c
Установка времени в кассовом регистраторе
*/

#include        <time.h>
#ifdef HAVE_TORGDEV
#include        <libdev.h>
#endif
#include        "buhg.h"


int	settimmar(KASSA kasr)
{
struct  tm      *bf;
time_t          tmm;
class iceb_tu_str timev("");
int		hour,min,sec;
char		bros[512];
char		strsql[512];

time(&tmm);
bf=localtime(&tmm);


helstr(LINES-1,0,"F10",gettext("выход"),NULL);
VVOD VVOD1(2);

VVOD1.VVOD_SPISOK_add_MD(gettext("Введите точное время."));

sprintf(bros,"%02d:%02d",bf->tm_hour,bf->tm_min);
timev.new_plus(bros);
naz:;

if(vvod1(&timev,6,&VVOD1,NULL,stdscr,-1,-1) == FK10)
 return(1);

polen(timev.ravno(),bros,sizeof(bros),1,':');
hour=atoi(bros);
if(hour > 24 || hour < 0)
 {
  sprintf(strsql,"%s => %d",gettext("Неверно введён час"),hour);  
  iceb_t_soob(strsql);
  goto naz;
 }

polen(timev.ravno(),bros,sizeof(bros),2,':');
min=atoi(bros);
if(min > 60 || hour < 0)
 {
  sprintf(strsql,"%s => %d",gettext("Неверно введены минуты"),min);  
  iceb_t_soob(strsql);
  goto naz;
 }
sec=0;

GDITE();

  sprintf(strsql,"FSTART: MARIA_TIME\n\
%s\n\
%s\n\
%d\n\
%d\n\
%d\n\
FEND:\n",
  kasr.prodavec, kasr.parol, hour, min, sec);  
  if (iceb_cmd(kasr, strsql))
   return(1);
  else 
  {
   sprintf(strsql,"%s !",gettext("Время установлено"));  
   iceb_t_soob(strsql);
   return(0);
  }
}
