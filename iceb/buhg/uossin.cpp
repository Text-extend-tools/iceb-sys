/*$Id: uossin.c,v 5.3 2013/05/17 14:56:14 sasa Exp $*/
/*08.09.2011	08.09.2011	Белых А.И.	uossin.c
Смена инветарного номера
*/
#include "buhg.h"

void uossin()
{
char strsql[512];
int kom=0;
class iceb_tu_str innoms("");
class iceb_tu_str innomn("");

class VVOD VV(0);
VV.VVOD_SPISOK_add_ZAG(gettext("Смена инвентарного номера"));

VV.VVOD_SPISOK_add_MD(gettext("Инвентарный номер который нужно сменить....."));
VV.VVOD_SPISOK_add_MD(gettext("Инвентарный номер на который нужно сменить.."));

VV.VVOD_SPISOK_add_data("",30);
VV.VVOD_SPISOK_add_data("",30);

naz:;

clear();
 
helstr(LINES-1,0,
//"F1",gettext("помощь"),
"F2/+",gettext("заменить"),
"F4",gettext("очистить"),
"F10",gettext("выход"),NULL);

kom=VV.vvod(0,1,1,-1,-1);

innoms.new_plus(VV.VVOD_SPISOK_return_data(0));
innomn.new_plus(VV.VVOD_SPISOK_return_data(1));

switch(kom)
 {
  case FK1:   /*Помощь*/
   GDITE();
//   iceb_t_pdoc("buhg5_1.txt");
   goto naz;

  case ESC:
  case FK10:
   return;

  case FK2:
  case PLU:
     break;


  case FK4: /*Очистить меню*/
    VV.VVOD_clear_data();  
    goto naz;

  default:
     goto naz;
 }
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select innom from Uosin where innom=%d",innoms.ravno_atoi());
if(readkey(strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %d!",gettext("Нет инвентарного номера"),innoms.ravno_atoi());
  iceb_t_soob(strsql);
  goto naz;
 }

sprintf(strsql,"select innom from Uosin where innom=%d",innomn.ravno_atoi());
if(readkey(strsql,&row,&cur) == 1)
 {
  sprintf(strsql,"%s %d!",gettext("Есть уже инвентарный номер"),innomn.ravno_atoi());
  iceb_t_soob(strsql);
  goto naz;
 }

class VVOD DANET(1);
sprintf(strsql,"%s %d %s %d? %s?",gettext("Сменить инвентарный номер"),innoms.ravno_atoi(),gettext("на"),innomn.ravno_atoi(),gettext("Вы уверены"));
DANET.VVOD_SPISOK_add_MD(gettext(strsql));

if(danet(&DANET,2,stdscr) == 2)
 goto naz;

class lock_tables kkk("LOCK TABLE Uosin WRITE,Uosdok1 WRITE,Uosin1 WRITE,Uosinp WRITE,Uosamor WRITE,Uosamor1 WRITE,Uosindm WRITE");

sprintf(strsql,"update Uosin set innom=%d where innom=%d",innomn.ravno_atoi(),innoms.ravno_atoi());
if(sql_zapis(strsql,0,0) != 0)
 goto naz;

sprintf(strsql,"update Uosdok1 set innom=%d where innom=%d",innomn.ravno_atoi(),innoms.ravno_atoi());
sql_zapis(strsql,0,0);

sprintf(strsql,"update Uosin1 set innom=%d where innom=%d",innomn.ravno_atoi(),innoms.ravno_atoi());
sql_zapis(strsql,0,0);

sprintf(strsql,"update Uosinp set innom=%d where innom=%d",innomn.ravno_atoi(),innoms.ravno_atoi());
sql_zapis(strsql,0,0);


sprintf(strsql,"update Uosamor set innom=%d where innom=%d",innomn.ravno_atoi(),innoms.ravno_atoi());
sql_zapis(strsql,0,0);

sprintf(strsql,"update Uosamor1 set innom=%d where innom=%d",innomn.ravno_atoi(),innoms.ravno_atoi());
sql_zapis(strsql,0,0);

sprintf(strsql,"update Uosindm set innom=%d where innom=%d",innomn.ravno_atoi(),innoms.ravno_atoi());
sql_zapis(strsql,0,0);

kkk.unlock(); /*зарблокировка таблиц*/

iceb_t_soob(gettext("Смена выполненена"));
 
}
