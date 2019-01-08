/* $Id: podtdok.c,v 5.15 2013-04-07 10:22:43 sasa Exp $ */
/*15.07.2015     30.5.1995       Белых А.И.      podtdok.c
Подтверждение всех записей в документе сразу
*/
#include	"buhg.h"


void podtdok(short dd,short md,short gd,int skl,const char *nomdok,
int tipz,int skl1,const char *nomdokp)
{
short           d1,m1,g1;
short           i;
char		strsql[512];
class iceb_tu_str vstr("");
VVOD DANET(1);
VVOD VVOD1(2);

move(0,0);

i=0;
DANET.VVOD_SPISOK_add_MD(gettext("Подтвердить весь документ ? Вы уверены ?"));
i=danet(&DANET,2,stdscr);

if(i == 2)
 return;

sprintf(strsql,"%d.%d.%d",dd,md,gd);
vstr.new_plus(strsql);
helstr(LINES-1,0,"F10",gettext("выход"),NULL);

VVOD1.VVOD_delete();
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите дату подтверждения"));

if(vvod1(&vstr,11,&VVOD1,NULL,stdscr,-1,-1) == FK10)
   return;


if(rsdat(&d1,&m1,&g1,vstr.ravno(),0) != 0)
 {
  iceb_t_soob(gettext("Не верно введена дата !"));
  return;
 }

if(SRAV1(g1,m1,d1,gd,md,dd) > 0)
 {
  iceb_t_soob(gettext("Дата менше даты выписки документа ! Подтверждение невозможно !"));
  return;
 }

//sprintf(strsql,"%s",iceb_t_imafnsi("blokmak.alx"));
if(iceb_t_pbpds(m1,g1) != 0)
  return;

podtdok1(dd,md,gd,skl,nomdok,tipz,d1,m1,g1,0);

if(skl1 != 0 && nomdokp[0] != '\0')
 {

  DANET.VVOD_delete();
  DANET.VVOD_SPISOK_add_MD(gettext("Подтвердить встречный документ ?"));

  if(danet(&DANET,2,stdscr) == 1)
   {
    if(tipz == 1)
      podtdok1(dd,md,gd,skl1,nomdokp,2,d1,m1,g1,0);

    if(tipz == 2)
      podtdok1(dd,md,gd,skl1,nomdokp,1,d1,m1,g1,0);

    GDITE();
    podvdok(dd,md,gd,nomdokp,skl1);
   }

 }

}
