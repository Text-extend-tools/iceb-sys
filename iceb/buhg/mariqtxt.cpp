/* $Id: mariqtxt.c,v 5.10 2013/05/17 14:56:04 sasa Exp $ */
/*25.03.2009	31.01.2001	Белых А.И.	mariqtxt.c
Распечатать сообщение на экане кассового регистратора
*/

#include        <time.h>
#ifdef HAVE_TORGDEV
#include        <libdev.h>
#endif
#include        "buhg.h"


int	mariqtxt(KASSA kasr, int nstr,const char *soob1)
{
char		strsql[512];

sprintf(strsql,"FSTART: MARIA_INDICATOR\n\
%s\n\
%s\n\
%d\n\
%s\n\
FEND:\n",
kasr.prodavec, kasr.parol, nstr, soob1);  
//  printw("\n%s\n",strsql);

return(iceb_cmd(kasr, strsql));
}
