/* $Id: marsiv.c,v 5.10 2013/05/17 14:56:04 sasa Exp $ */
/*18.06.2002	06.03.2001	Белых А.И.	marsiv.c
Служебное внесение/извлечение
*/
#include        <time.h>
#ifdef HAVE_TORGDEV
#include        <libdev.h>
#endif
#include        "buhg.h"


void	marsiv(KASSA kasr, 
int metka) //0-внесение 1-извлечение
{
char		strsql[512];
class iceb_tu_str suma("");
int		kom1;
double		sum;
VVOD VVOD1(2);

helstr(LINES-1,0," F10 ",gettext("выход"),NULL);

if(metka == 0)
 VVOD1.VVOD_SPISOK_add_MD(gettext("Введите сумму служебного внесения."));
if(metka == 1)
 VVOD1.VVOD_SPISOK_add_MD(gettext("Введите сумму служебного извлечения."));


if((kom1=vvod1(&suma,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
  return;
if(kom1 == ESC)
  return;

GDITE();
sum=suma.ravno_atof();
if(metka == 1)
 sum*=(-1);
 
sprintf(strsql,"FSTART: MARIA_SLUG\n\
%s\n\
%s\n\
%.2f\n\
FEND:\n",
kasr.prodavec,kasr.parol,sum);  
//  printw("\n%s\n",strsql);
iceb_cmd(kasr, strsql);

}
