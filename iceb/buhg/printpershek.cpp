/* $Id: printpershek.c,v 5.12 2013/09/26 09:43:37 sasa Exp $ */
/*18.06.2002	15.02.2001	Белых А.И.	printpershek.c
Печать отчёта за период на кассовом регистраторе Мария
*/
#include        <time.h>
#ifdef HAVE_TORGDEV
#include        <libdev.h>
#endif
#include        "buhg.h"

extern short ddd,mmm,ggg;

void	printpershek(KASSA kasr)
{
int	kom;
short	dn,mn,gn,dk,mk,gk;
char	strsql[512];

VVOD VV(0);

VV.VVOD_SPISOK_add_MD(gettext("Дата начала.(д.м.г.).."));
VV.VVOD_SPISOK_add_MD(gettext("Дата конца..(д.м.г.).."));
VV.VVOD_SPISOK_add_data(11);
VV.VVOD_SPISOK_add_data(11);

naz:;

helstr(LINES-1,0,"F2/+",gettext("печать"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);
switch (kom)
 {
  case ESC:
  case FK10:
    return;
/*
  case FK1:
    GDITE();
    prosf(strsql);
    clear();
    goto naz1;
*/
  case FK2:
  case PLU:
    break;
 }

if(rsdatp(&dn,&mn,&gn,VV.VVOD_SPISOK_return_data(0),&dk,&mk,&gk,VV.VVOD_SPISOK_return_data(1)) != 0)
 goto naz;
 
GDITE();

  sprintf(strsql,"FSTART: MARIA_DATE_REPORT\n\
%s\n\
%s\n\
%d\n\
%d\n\
%d\n\
%d\n\
%d\n\
%d\n\
FEND:\n",
  kasr.prodavec, kasr.parol, dn, mn, gn, dk, mk, gk);  
  iceb_cmd(kasr, strsql);

}
