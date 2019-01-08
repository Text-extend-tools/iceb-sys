/* $Id: printdayshek.c,v 5.11 2013/09/26 09:43:37 sasa Exp $ */
/*18.06.2002	30.01.2001 	Белых А.И.	printdayshek.c
Печать дневного отчётного чека
********************************/
#include        <time.h>
#ifdef HAVE_TORGDEV
#include        <libdev.h>
#endif
#include        "buhg.h"

extern short ddd,mmm,ggg;

void	printdayshek(KASSA kasr)
{
int	dayreg;
char	strsql[512];

VVOD DANET(1);

DANET.VVOD_SPISOK_add_MD(gettext("Печать дневного отчётного чека."));

if(danet(&DANET,2,stdscr) == 2)
  return;
DANET.VVOD_delete();  

sprintf(strsql,"%s ?",gettext("Обнулять дневные регистры"));
DANET.VVOD_SPISOK_add_MD(strsql);

dayreg=0;
if(danet(&DANET,2,stdscr) == 1)
  dayreg=1;

GDITE();  

  sprintf(strsql,"FSTART: MARIA_DAY_REPORT\n\
%s\n\
%s\n\
%d\n\
FEND:\n",
  kasr.prodavec, kasr.parol,dayreg);  
  iceb_cmd(kasr, strsql);


if(dayreg == 1)
   settimmar(kasr);

}
