/*$Id: infosys.c,v 5.18 2014/05/07 10:57:30 sasa Exp $*/
/*31.12.2017	07.12.2000	Белых А.И.	infosys.c
Вывод на экран системной информации
*/
#include        "buhl.h"

extern class iceb_tu_str kodirovka_iceb; /*Определяется в icebstart.c и nastsys.c*/
extern int metka_utf8;
extern class iceb_tu_str nameprinter;

void infosys(int y,int x,const char *host,const char *verss,
const char *dvers,short ddd,short mmm,short ggg,
const char *imabaz,int uid,const char *fioop)
{

pid_t	mypid;

mypid=getpid();
short dt,mt,gt;
poltekdat(&dt,&mt,&gt);

move(y=12,x=COLS-29);
if(ddd == 0 && mmm == 0 && ggg == 0)
 {
  printw("%s %d.%d.%d%s",gettext("Текущая дата"),dt,mt,gt,
  gettext("г."));
 }
else
 if(ddd != 0)
  {
   printw("%s %d.%d.%d%s",gettext("Текущая дата"),ddd,mmm,ggg,
   gettext("г."));
  }
 else
  printw("%s %d.%d%s",gettext("Текущая дата"),mmm,ggg,
  gettext("г."));

if(imabaz[0] != '\0')
 {
  move(++y,x);
  printw("%s%s/%s/%d",gettext("База:"),imabaz,kodirovka_iceb.ravno(),metka_utf8);
 }
move(++y,x);
printw("%s%s",gettext("Хост:"),host);

move(++y,x);
printw("%s:%d",gettext("Номер процесса"),mypid);

move(++y,x);
printw("%s:%d %s",gettext("Логин"),uid,iceb_tu_getlogin());

move(++y,x);
printw("%s:%-.*s",gettext("Имя"),iceb_tu_kolbait(20,fioop),fioop);

move(++y,x);
if(strlen(nameprinter.ravno()) > 0)
 printw("%s:%-.*s",gettext("Принтер"),iceb_tu_kolbait(20,nameprinter.ravno()),nameprinter.ravno());
else
 printw("%s:%-.*s",gettext("Принтер"),iceb_tu_kolbait(20,gettext("По умолчанию")),gettext("По умолчанию"));

move(y+=2,x);

printw("%s © %s 1992-2018",gettext("Автор"),gettext("Белых А.И."));

move(++y,x);
printw("%s/0432/512973",gettext("г. Винница т."));

move(++y,x);
printw("%s +380676654526",gettext("Мобиль. тел."));

move(++y,x);
//printw("www.iceb.vc.ukrtel.net");
printw("%s",ICEBSAIT);

mvprintw(LINES-1,0,"%s %s %s %s%s",gettext("Версия"),verss,
gettext("от"),dvers,
gettext("г."));

//move(LINES-1,x-2);
//printw("http://www.iceb.vinnitsa.com");

}
