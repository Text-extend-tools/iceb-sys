/* $Id: spec.c,v 5.53 2014/05/07 10:57:23 sasa Exp $ */
/*01.05.2014	12.01.2000	Белых А.И.	spec.c
Программа работы со спецификациями на изделия
*/
#define         DVERSIQ "01.05.2014"
#include        "buhg.h"

int   specnast(void);

pid_t           mypid;
extern int	iscolor; /*0-чернобелый монитор*/
int		kassa;
short    	mdd;  /*0-обычный документ 1-двойной*/
extern short		startgodb; /*Стартовый год просмотров главной книги*/
int		ktorab=0;
extern char *imabaz;

int             main(int argc,char **argv,char **envp)
{
int             kom=0;
char		*host=NULL;
int		urov;
int		kodiz,kodizv;

icebstart(argc,argv);
VVOD MENU(3);

/*Ищем настройку*/
if(nastsys() != 0)
  exit_iceb(1);

if(specnast() != 0)
  exit_iceb(1);
  

MENU.VVOD_SPISOK_add_MD("Ввод и корректировка спецификаций");
MENU.VVOD_SPISOK_add_MD(gettext("Конец работы"));

int uid=0;
class iceb_tu_str fioop;
uid=iceb_t_getuid();
fioop.plus(iceb_t_getfioop());

for(;;)
 {

  clear(); 

  move(0,0);
  printw("Программа работы со спецификациями изделий");
  move(1,0);
  printw("%.40s",iceb_t_get_pnk("00",0));

  znak(0,COLS-26);
  infosys(12,COLS-27,host,VERSION,DVERSIQ,0,0,0,imabaz,uid,fioop.ravno());

  if(menu3(&MENU,&kom,1,2,0) != 0)
    continue;

  switch (kom)
   {
    case 1 :
    case -1 :
      goto kon;
      break;

    case 0:
      kodiz=kodizv=0;
      urov=0;
      dirspec(kodiz,&kodizv,"","",&urov,0);
      break;     

    default:
      kom=0;
      break;
   }
 }

kon:;

sql_closebaz(&bd);
refresh();
endwin();
}
