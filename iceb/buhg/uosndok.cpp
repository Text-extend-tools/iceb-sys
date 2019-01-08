/* $Id: uosndok.c,v 5.17 2013/08/13 05:49:57 sasa Exp $ */
/*25.01.2017	11.11.1999	Белых А.И.	uosndok.c
Ввод нового документа в учете основных средств
*/
#include        "buhg.h"
#include        "uos.h"

extern short           mdd;  /*0-обычный документ 1-двойной*/
extern class UOS uos;

void		uosndok()
{
short           mvnp=0; /*0-внешняя 1 -внутреняя 2-изменение стоимости*/
SQL_str         row1;
SQLCURSOR curr;
char		strsql[512];
class iceb_tu_str bros("");
static char     dat1[16];
short		d,m,g;

clear();
VVOD MENU(3);

MENU.VVOD_SPISOK_add_ZAG(gettext("Введение нового документа"));
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите какой вид документа вы будете вводить"));

MENU.VVOD_SPISOK_add_MD(gettext("Приходный документ"));
MENU.VVOD_SPISOK_add_MD(gettext("Расходный документ"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

uos.tipz=0;
int kom=0;
while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
uos.tipz=kom;

if(uos.tipz == 2 || uos.tipz == -1)
 return;

GDITE();

uos.tipz++;
uos.tipzv=1;
if(uos.tipz == 1)
  uos.tipzv=2;


/***************************************/
/*Вибираем конкретный приход или расход*/
/***************************************/

mdd=mvnp=0;

if(uos.tipz == 1)
 bros.new_plus("Uospri");
if(uos.tipz == 2)
 bros.new_plus("Uosras");

uos.kprr.new_plus("");
uos.naimpr.new_plus("");
clear();
if(uos.tipz == 1)
 {
//  if(dirmat1("Uospri",&uos.kprr,&uos.naimpr,LINES-5,0,2,1,&poz,&komv,"","\0") != 0 )
   if(diruospri(1,&uos.kprr,&uos.naimpr) != 0)
    return;
 }
else
 if(diruosras(1,&uos.kprr,&uos.naimpr) != 0)
  return;
  
sprintf(strsql,"select vido from %s where kod='%s'",bros.ravno(),uos.kprr.ravno());
if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
 {
  sprintf(strsql,gettext("Не найден код операции %s !"),uos.kprr.ravno());
  iceb_t_soob(strsql);
  return;
 }
mvnp=atoi(row1[0]);
if(mvnp == 1)
 mdd=1;

VVOD VV(0);
sozuosvdok(&VV);
uos.kontr.new_plus("");
if(mdd == 1)
 uos.kontr.new_plus("00");

uos.nomdok.new_plus("");
uos.nomdokp.new_plus("");

uos.naimo.new_plus("");
uos.kodol=uos.kodol1=uos.pod=uos.pod1=0;

uos.naipod.new_plus("");
uos.naipod1.new_plus("");
uos.naikol.new_plus("");
uos.naikol1.new_plus("");
uos.osn.new_plus("");
uos.vid_dog.new_plus("");

VV.VVOD_SPISOK_zapis_data(0,dat1);
uos.datpr.new_plus(dat1);

if(uosvdok(&VV,0) != 0)
 return;

sprintf(dat1,"%s",uos.datpr.ravno());
rsdat(&d,&m,&g,uos.datpr.ravno(),1);

class iceb_tu_str nomd(uos.nomdok.ravno());
uosdok(d,m,g,&nomd);
uos.nomdok.new_plus(nomd.ravno());
}
