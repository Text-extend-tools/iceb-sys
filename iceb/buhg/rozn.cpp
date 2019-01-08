/* $Id: rozn.c,v 5.60 2014/05/07 10:57:23 sasa Exp $ */
/*18.01.2016	01.02.1998	Белых А.И.	rozn.c
Программа розничной торговли
*/
#define         DVERSIQ "15.07.2015"
#include        <pwd.h>
#include        <locale.h>
#ifdef HAVE_TORGDEV
#include	<libdev.h>
#endif
#include	"buhg.h"


pid_t		mypid;
int		kassa=0;
short           mdd;  /*0-обычный документ 1-двойной для расход*/
extern char	*imabaz;
SQL_baza	bd;
char		mylogin[L_cuserid];
int		ktorab=0;
class iceb_tu_str fio("");

int             main(int argc,char **argv,char **envp)
{
struct  passwd  *ktor; /*Кто работает*/
SQL_str   	row;
int		kom;
class iceb_tu_str kto("");
extern char *host;
int		skl=0;
char		strsql[1024];
class iceb_tu_str naiot("");
class iceb_tu_str grup("");
KASSA		kasr;
int uid=0;
class iceb_tu_str fioop;
SQLCURSOR curr; 
short dt,mt,gt;

ktor=getpwuid(getuid());
ktorab=ktor->pw_uid;

mypid=getpid();

kto.new_plus(ktor->pw_name);


/* прочитать инициализационный файл (параметры устройств) */
//read_ini();


icebstart(argc,argv);
VVOD MENU(4);
VVOD VVOD1(2);

kasr.handle = -1;
/* прочитаем параметры кассового регистратора из файла настройки	*/
#ifdef HAVE_TORGDEV
if (nastrmariq(1, &kasr) == 0) 
 {
/*************   
   printw("handle = %d; sklad = %d; nomer = %d; \n\
   host = %s; password = %s; port = %s; prod = %s;\n",
   kasr.handle, kasr.sklad, kasr.nomer, kasr.host, kasr.parol, kasr.port, kasr.prodavec);
   OSTANOV();
***************/   
   /* если успешно - пытаемся связаться с кассовым сервером		*/
   if ((kasr.handle=iceb_open_server(kasr.host, kasr.port)) < 0)
      errorkan(kasr.handle); // ошибка открытия связи
   else 
    {		     // успешно
     skl = kasr.sklad;
     kassa = kasr.nomer;
    }
 }
#endif

if(skl == 0)
 {
  class iceb_tu_str kod_v("");
  class iceb_tu_str naim_v("");
  if(dirsklad(2,&kod_v,&naim_v) != 0)
   {
    iceb_t_soob(gettext("Не выбран отдел !"));  
    goto kon;
   }
  skl=kod_v.ravno_atoi();
 }
sprintf(strsql,"select naik from Sklad where kod=%d",skl);
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  iceb_t_soob(gettext("Не найден код отдела !"));
  goto kon;
 }
naiot.new_plus(row[0]);

if(kassa == 0)
 {
  helstr(LINES-1,0,"F10",gettext("выход"),NULL);

  VVOD1.VVOD_delete();
  VVOD1.VVOD_SPISOK_add_MD(gettext("Введите номер рабочего места"));
  class iceb_tu_str vstr("");
  if((kom=vvod1(&vstr,3,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
   {
    goto kon;
   }
  if(kom == ESC)
   {
    goto kon;
   }
  if((kassa=vstr.ravno_atoi()) == 0)
   {
    iceb_t_soob(gettext("Не определен номер рабочего места !"));
    goto kon;
   }
 }
 
helstr(LINES-1,0,"F10",gettext("выход"),NULL);

ktor=getpwuid(ktorab);
fio.new_plus(ktor->pw_gecos);



/*Читаем нормативно-справочную информацию*/
nastsys();
matnast();

/*Читаем какие группы настроены для склада*/
sprintf(strsql,"Группы для склада %d",skl);

iceb_t_poldan(strsql,&grup,"matnast.alx");


kom=0;

MENU.VVOD_SPISOK_add_ZAG(gettext("Программа розничной торговли"));
MENU.VVOD_SPISOK_add_ZAG(iceb_t_get_pnk("00",0),40);
sprintf(strsql,"%d %s",skl,naiot.ravno());
MENU.VVOD_SPISOK_add_ZAG(strsql);
sprintf(strsql,"%s %d",gettext("Номер рабочего места:"),kassa);
MENU.VVOD_SPISOK_add_ZAG(strsql);

MENU.VVOD_SPISOK_add_MD(gettext("Розничная продажа"));
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать все продажи"));
MENU.VVOD_SPISOK_add_MD(gettext("Распечатать свод всех продаж"));
MENU.VVOD_SPISOK_add_MD(gettext("Конец работы"));


uid=iceb_t_getuid();
fioop.plus(iceb_t_getfioop());
 
for(;;)
 {

  clear();

  znak(0,COLS-26);
  infosys(12,COLS-27,host,VERSION,DVERSIQ,0,0,0,imabaz,uid,fioop.ravno());

  move(LINES-1,0);
  printw("%s %s %s %s%s",gettext("Версия"),VERSION,
  gettext("от"),DVERSIQ,
  gettext("г."));

  if(menu3(&MENU,&kom,1,0,0) != 0)
   continue;

  switch (kom)
   {
    case 3 :
    case -1 :
	   goto kon;

    case 0 :

     poltekdat(&dt,&mt,&gt);
     rozprod(dt,mt,gt,kassa,0,skl,grup.ravno(),-1,kasr);
     break;

    case 1 :

     poltekdat(&dt,&mt,&gt);
     rozvse(dt,mt,gt,kassa,fio.ravno());
     break;
    case 2 :

     poltekdat(&dt,&mt,&gt);
     rozsvod(dt,mt,gt,kassa,fio.ravno());
     break;
    default:
     kom=0;
     break;
   }
 }

kon:;


sql_closebaz(&bd);
#ifdef HAVE_TORGDEV
if (kasr.handle >= 0) iceb_close_server(kasr.handle);
#endif

refresh();
endwin();
return(0);
}
