/*$Id: dirbuserd.c,v 5.5 2013/05/17 14:55:57 sasa Exp $*/
/*13.07.2015	02.09.2010	Белых А.И.	dirbuserd.c
Работа со списком разрешений для подсистем
*/

#include <pwd.h>
#include "buhg.h"


int dirbuserd_k(int pods,int user,int rr,int rb,int rn);

void dirbuserd(int user,const char *imabaz_v)
{
const int razmer=33;
int dostup[razmer];
char strsql[512];
char danet1[32];
char danet2[32];
char danet3[32];
class iceb_tu_str naimpsys;
class iceb_tu_str fioop("");
SQL_str row;
class SQLCURSOR cur;
int prc=100;
int K=0;
int kom=0;

naz1:;
class iceb_t_mstr spis_menu(0);

sprintf(strsql,"select * from icebuser where un=%d",user);
if(readkey(strsql,&row,&cur) == 1)
 fioop.new_plus(row[1]);

memset(dostup,'\0',sizeof(dostup));
for(int ii=0; ii < razmer; ii++)
  dostup[ii]=atoi(row[ii+6]);
 
//printw("\n%s\n",__FUNCTION__);
//refresh();
for(int ii=0; ii < razmer; ii+=3)
 {
  if(dostup[ii] == 0)
   strcpy(danet1,gettext("Да"));
  else
   strcpy(danet1,gettext("Нет"));

  if(dostup[ii+1] == 0)
   strcpy(danet2,gettext("Да"));
  else
   strcpy(danet2,gettext("Нет"));

  if(dostup[ii+2] == 0)
   strcpy(danet3,gettext("Да"));
  else
   strcpy(danet3,gettext("Нет"));

  if(ii == 0)
   naimpsys.new_plus(gettext("Главная книга"));

  if(ii == 3)
   naimpsys.new_plus(gettext("Материальный учёт"));

  if(ii == 6)
   naimpsys.new_plus(gettext("Платёжные документы"));

  if(ii == 9)
   naimpsys.new_plus(gettext("Заработная плата"));

  if(ii == 12)
   naimpsys.new_plus(gettext("Учёт основных средств"));

  if(ii == 15)
   naimpsys.new_plus(gettext("Учёт услуг"));

  if(ii == 18)
   naimpsys.new_plus(gettext("Учёт кассовых ордеров"));

  if(ii == 21)
   naimpsys.new_plus(gettext("Учёт командировочных расходов"));

  if(ii == 24)
   naimpsys.new_plus(gettext("Учёт путевых листов"));

  if(ii == 27)
   naimpsys.new_plus(gettext("Реестр налоговых накладных"));

  if(ii == 30)
   naimpsys.new_plus(gettext("Учёт доверенностей"));


  sprintf(strsql,"%-*.*s|%-*s|%-*s|%-*s",
  iceb_tu_kolbait(30,naimpsys.ravno()),
  iceb_tu_kolbait(30,naimpsys.ravno()),
  naimpsys.ravno(),
  iceb_tu_kolbait(4,danet1),danet1,
  iceb_tu_kolbait(4,danet2),danet2,
  iceb_tu_kolbait(4,danet3),danet3);

  spis_menu.spis_str.plus(strsql);
 }

for(;;)
 {
  clear();
  short	y=2;
  short x=COLS-20;

  helstr(LINES-1,0,
  "FK2",gettext("коррек."),
  "FK10",gettext("выход"),NULL);

  mvprintw(0,0,"%s %s:%s/%s",gettext("Список полномочий для оператора"),gettext("База"),imabaz_v,iceb_t_get_pnk("00",0));
  mvprintw(1,0,"%d %s",user,fioop.ravno());
  mvprintw(2,0,"                 1                2    3    4");

  mvprintw(y++,x,gettext("Наименование полей:"));
  mvprintw(y++,x,"1-%s",gettext("подсистема"));
  mvprintw(y++,x,"2-%s",gettext("работа"));
  mvprintw(y++,x,"3-%s",gettext("блокировка"));
  mvprintw(y++,x,"4-%s",gettext("настройка"));
  
  memset(strsql,'\0',sizeof(strsql));

  kom=spis_menu.menu(kom,3,0,NULL,strsql,prc,&K,"",0);
  switch(K)
   {
    case FK10:
    case ESC:
      return;
    
    case ENTER:
    case FK2:
      dirbuserd_k(kom,user,dostup[kom*3],dostup[kom*3+1],dostup[kom*3+2]);
      goto naz1;         

    default:
      break;    
   }
 }



}
/**********************/
/*Меню для выбора*/
/*******************/
int dirbuserd_menudn(int danet)
{
VVOD MENU(3);

MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите  нужное и нажмите Enter"));

MENU.VVOD_SPISOK_add_MD(gettext("Да"));
MENU.VVOD_SPISOK_add_MD(gettext("Нет"));
MENU.VVOD_SPISOK_add_MD(gettext("Выход"));

clearstr(LINES-1,0);

while(menu3w(stdscr,&MENU,&danet,-1,-1,0) != 0);

return(danet);
}

/*************************/
/*Корректировка*/
/**************************/
int dirbuserd_k(int pods,int user,int rr,int rb,int rn)
{
char strsql[512];
int N=0,K=0;
class iceb_tu_str danet1;
class iceb_tu_str danet2;
class iceb_tu_str danet3;


class VVOD VV(0);
VV.VVOD_SPISOK_add_MD(gettext("Разрешено работать с подсистемой.."));//0
VV.VVOD_SPISOK_add_MD(gettext("Разрешено блокировать подсистему.."));//1
VV.VVOD_SPISOK_add_MD(gettext("Разрешено настраивать подсистему.."));//2

VV.VVOD_SPISOK_add_data(32);//0
VV.VVOD_SPISOK_add_data(32);//0
VV.VVOD_SPISOK_add_data(32);//0

naz:;

//clear();

VV.VVOD_delete_ZAG();

VV.VVOD_SPISOK_add_ZAG(gettext("Корректировка"));



helstr(LINES-1,0,"F2/+",gettext("запись"),
"F10",gettext("выход"),NULL);



if(rr == 0)
 danet1.new_plus(gettext("Да"));
else
 danet1.new_plus(gettext("Нет"));

if(rb == 0)
 danet2.new_plus(gettext("Да"));
else
 danet2.new_plus(gettext("Нет"));

if(rn == 0)
 danet3.new_plus(gettext("Да"));
else
 danet3.new_plus(gettext("Нет"));

VV.data_plus(0,danet1.ravno());
VV.data_plus(1,danet2.ravno());
VV.data_plus(2,danet3.ravno());

VV.vramka(-1,-1,0);

for(;;)
 {
  attron(VV.VVOD_return_cs(iceb_CFS)); 

  VV.vprintw(N,stdscr);

  K=getch();

  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  VV.vprintw(N,stdscr);

  if(K == KEY_RESIZE)
   {
//    getch();
    goto naz;
   }

  if(K == KEY_MOUSE)
    K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));
    VV.vprintw_clear(N,stdscr);

    if(N == 0)
     {
      rr=dirbuserd_menudn(rr);

      N++;
      goto naz;
     }
    if(N == 1)
     {
      rb=dirbuserd_menudn(rb);

      N++;
      goto naz;
     }
    if(N == 2)
     {
      rn=dirbuserd_menudn(rn);

/*      N++; Для посленего не переводим*/
      goto naz;
     }

    VV.vgetstr(N,stdscr);
    
    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

//    if(N == 0)


    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));
    VV.vprintw(N,stdscr);
    N++;
   }
  if(K==SDN) /*Стрелка вниз*/
   {
    N++;
   }
  if(K==SUP) /*Стрелка вверх*/
   {
    N--;
   }
  if(N > VV.KLST-1)
   {
    N=0;
   }
  if(N < 0)
   {
    N=VV.KLST-1;
   }

  if(K == ESC || K == FK10)
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));
    N=-1;
    break;
   }

  if(K == FK2 || K == PLU) /*Записываем*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));

    struct  passwd  *ktoz; /*Кто записал*/
    ktoz=getpwuid(getuid());

    memset(strsql,'\0',sizeof(strsql));

    switch(pods)
     {
      case 0:
        sprintf(strsql,"update icebuser set gkd=%d,gkb=%d,gkn=%d,logz='%s',vrem=%ld where un=%d",
        rr,rb,rn,ktoz->pw_name,time(NULL),user);
        break;
      case 1:
        sprintf(strsql,"update icebuser set mud=%d,mub=%d,mun=%d,logz='%s',vrem=%ld where un=%d",
        rr,rb,rn,ktoz->pw_name,time(NULL),user);
        break;
      case 2:
        sprintf(strsql,"update icebuser set pdd=%d,pdb=%d,pdn=%d,logz='%s',vrem=%ld where un=%d",
        rr,rb,rn,ktoz->pw_name,time(NULL),user);
        break;
      case 3:
        sprintf(strsql,"update icebuser set zpd=%d,zpb=%d,zpn=%d,logz='%s',vrem=%ld where un=%d",
        rr,rb,rn,ktoz->pw_name,time(NULL),user);
        break;
      case 4:
        sprintf(strsql,"update icebuser set osd=%d,osb=%d,osn=%d,logz='%s',vrem=%ld where un=%d",
        rr,rb,rn,ktoz->pw_name,time(NULL),user);
        break;
      case 5:
        sprintf(strsql,"update icebuser set uud=%d,uub=%d,uun=%d,logz='%s',vrem=%ld where un=%d",
        rr,rb,rn,ktoz->pw_name,time(NULL),user);
        break;
      case 6:
        sprintf(strsql,"update icebuser set kod=%d,kob=%d,kon=%d,logz='%s',vrem=%ld where un=%d",
        rr,rb,rn,ktoz->pw_name,time(NULL),user);
        break;
      case 7:
        sprintf(strsql,"update icebuser set krd=%d,krb=%d,krn=%d,logz='%s',vrem=%ld where un=%d",
        rr,rb,rn,ktoz->pw_name,time(NULL),user);
        break;
      case 8:
        sprintf(strsql,"update icebuser set pld=%d,plb=%d,pln=%d,logz='%s',vrem=%ld where un=%d",
        rr,rb,rn,ktoz->pw_name,time(NULL),user);
        break;
      case 9:
        sprintf(strsql,"update icebuser set nnd=%d,nnb=%d,nnn=%d,logz='%s',vrem=%ld where un=%d",
        rr,rb,rn,ktoz->pw_name,time(NULL),user);
        break;
      case 10:
        sprintf(strsql,"update icebuser set udd=%d,udb=%d,udn=%d,logz='%s',vrem=%ld where un=%d",
        rr,rb,rn,ktoz->pw_name,time(NULL),user);
        break;
      default:
        iceb_t_soob("Неизвестный номер подсистемы!");
        return(1);
     };
    sql_zapis(strsql,1,0);     

    break;
   }
 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
clear();
refresh();
return(0);
}
