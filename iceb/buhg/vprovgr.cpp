/*$Id: vprovgr.c,v 1.34 2013/09/26 09:43:44 sasa Exp $*/
/*26.06.2013	29.01.2002	Белых А.И. 	vprovgr.c
Выполение проводок для счетов с развернутым сальдо по группам контрагентов
*/
#include        "buhg.h"

void		vprovnbs(char*,OPSHET*);


void            vprovgr()
{
struct OPSHET	shetv;
struct OPSHET	shetvh;
class iceb_tu_str nsh("");
class iceb_tu_str naimsh("");
static char     dat1[11];
int		K=0,N=0;
class iceb_tu_str kred("");
class iceb_tu_str debt0("");
class iceb_tu_str nshk("");
class iceb_tu_str datpr("");
class iceb_tu_str komen("");
short		d,m,g;
char		strsql[1024];
double		deb,kre;
class iceb_tu_str kontr("");
class iceb_tu_str grupa("");
time_t		vrem;
int		kolstr,kolstr1;
SQL_str		row,row1;
int		kolpr=0;
int kekv=0;

VVOD VVOD1(2);
VVOD1.VVOD_SPISOK_add_ZAG(gettext("Ввод проводок по группам контрагентов"));
VVOD1.VVOD_SPISOK_add_MD(gettext("Введите номер счета (субсчета)"));

naz:;

clear();

helstr(LINES-1,0,"F10",gettext("выход"),NULL);


nsh.new_plus("");
if(vvod1(&nsh,24,&VVOD1,NULL,stdscr,-1,-1) == FK10)
  return;

GDITE();

if(nsh.getdlinna() <= 1)
 {
  nsh.new_plus("");
  while(dirshet(&nsh,&naimsh,1) != 0)
   {
    clearstr(0,0);
    return;
   }
  clear();
  
 }

if(prsh1(nsh.ravno(),&shetv) != 0)
  goto naz;

if(shetv.saldo != 3)
 {
  iceb_t_soob(gettext("Счёт не с развернутым сальдо !"));
  goto naz;
 }
if(shetv.stat == 1)
 {
  sprintf(strsql,gettext("Счёт %s внебалансовый !"),nsh.ravno());
  iceb_t_soob(strsql);
  goto naz;
 }
VVOD VV(0);
VV.VVOD_SPISOK_add_MD(gettext("Кредит......"));
VV.VVOD_SPISOK_add_MD(gettext("Дебет......."));
VV.VVOD_SPISOK_add_MD(gettext("Счёт к-нт..."));
VV.VVOD_SPISOK_add_MD(gettext("Дата........"));
VV.VVOD_SPISOK_add_MD(gettext("Коментарий.."));
VV.VVOD_SPISOK_add_MD(gettext("Группа (,,)."));

VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(11);
VV.VVOD_SPISOK_add_data(60);
VV.VVOD_SPISOK_add_data(60);


NAZ:;
clear();

VV.data_plus(3,dat1);
datpr.new_plus(dat1);

for(;;)
 {
  VV.data_plus(3,dat1);
  move(LINES-1,0);


  move(0,0);
  printw(gettext("Выполнение проводок для счёта %s"),nsh.ravno());
  move(1,0);
  if(shetv.tips == 0)
    printw(gettext("Счёт активный"));
  if(shetv.tips == 1)
    printw(gettext("Счёт пассивный"));
  if(shetv.tips == 2)
    printw(gettext("Счёт активнo-пассивный"));
  if(shetv.saldo == 3)
    printw("  %s",gettext("Р/с"));
 
  printw("\n%s\n",shetv.naim);

  short    dt,mt,gt;
  poltekdat(&dt,&mt,&gt);
  printw("%s %d.%d.%d%s",
  gettext("Текущая дата"),
  dt,mt,gt,gettext("г."));

  helstr(LINES-1,0,"F1",gettext("помощь"),
  "F2/+",gettext("запись"),
  "F3",gettext("план счетов"),
  "F4",gettext("просм.проводок"),
  "F5",gettext("блок.пр."),
  "F10",gettext("выход"),NULL);

  VV.vramka(5,0,0);

  attron(VV.VVOD_return_cs(iceb_CFS)); 

  VV.vprintw(N,stdscr);

  for(;;)
   {
    refresh();
    K=getch();

    attroff(VV.VVOD_return_cs(iceb_CFS));
    attron(VV.VVOD_return_cs(iceb_CFM));

    VV.vprintw(N,stdscr);
    
    if(K == KEY_MOUSE)
      K=mouseklic(&N,VV.PY,VV.PX,VV.MDLS,VV.KLST,stdscr);

    if(K == ENTER)
     {
      attroff(VV.VVOD_return_cs(iceb_CFM));
      attron(VV.VVOD_return_cs(iceb_CFV));
      VV.vprintw_clear(N,stdscr);

      if(N == 0)
       {
        VV.vgetstr(N,stdscr);
	kred.new_plus(VV.VVOD_SPISOK_return_data(N));
       }
      if(N == 1)
       {
        VV.vgetstr(N,stdscr);
 	debt0.new_plus(VV.VVOD_SPISOK_return_data(N));
       }

      if(N == 2)
       {
        VV.vgetstr(N,stdscr);
	nshk.new_plus(VV.VVOD_SPISOK_return_data(N));
        if(nshk.ravno()[0] != '\0') 
         {
            attroff(VV.VVOD_return_cs(iceb_CFV));
  	  if(prsh1(nshk.ravno(),&shetvh) != 0)
           {
  	    break;
           }
          if(shetvh.stat == 1)
           {
            sprintf(strsql,gettext("Счёт %s внебалансовый !"),nshk.ravno());
            iceb_t_soob(strsql);
            break;
           }
         }
       }

      if(N == 3)
       {
        VV.vgetstr(N,stdscr);
	datpr.new_plus(VV.VVOD_SPISOK_return_data(N));
	strncpy(dat1,VV.VVOD_SPISOK_return_data(N),sizeof(dat1)-1);
        if(rsdat(&d,&m,&g,datpr.ravno(),0) != 0)
         {
          attroff(VV.VVOD_return_cs(iceb_CFV));
          attron(VV.VVOD_return_cs(iceb_CB));
          iceb_t_soob(gettext("Не верно введена дата проводки !"));
          break;
         }
       }
      if(N == 4)
       {
        VV.vgetstr(N,stdscr);
	komen.new_plus(VV.VVOD_SPISOK_return_data(N));
       }
      if(N == 5)
       {
        VV.vgetstr(N,stdscr);
	grupa.new_plus(VV.VVOD_SPISOK_return_data(N));
       }

        attroff(VV.VVOD_return_cs(iceb_CFV));
        attron(VV.VVOD_return_cs(iceb_CFM));


      VV.vprintw(N,stdscr);
      if(N == 0)
       N++;

      N++;
     }

    if(K==SDN) /*Стрелка вниз*/
      N++;
    if(K==SUP) /*Стрелка вверх*/
      N--;
    if(N > VV.KLST-1)
      N=0;
    if(N < 0)
      N=VV.KLST-1;

    attron(VV.VVOD_return_cs(iceb_CFS)); 
    VV.vprintw(N,stdscr);

   if(K == ESC || K == FK10)
    {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     attron(VV.VVOD_return_cs(iceb_CB));
     return;
    }
   if(K == FK1) /*Помощ*/
    {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     GDITE();

     iceb_t_pdoc("buhg3_4.txt");

     clear();
     break;
    }
   if(K == PLU || K == FK2) /*Записываем*/
    {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     attron(VV.VVOD_return_cs(iceb_CFM));
     GDITE();
     if(rsdat(&d,&m,&g,datpr.ravno(),0) != 0)
      {
       iceb_t_soob(gettext("Не верно введена дата проводки !"));
       break;
      }
     if(VV.VVOD_SPISOK_return_data(2)[0] == '\0')
      {
       iceb_t_soob(gettext("Не веден счёт корреспондент !"));
       break;
      }
     if(VV.VVOD_SPISOK_return_data(0)[0] == '\0' && VV.VVOD_SPISOK_return_data(1)[0] == '\0')
      {
       iceb_t_soob(gettext("Не введена сумма !"));
       break;
      }
     if(VV.VVOD_SPISOK_return_data(5)[0] == '\0')
      {
       iceb_t_soob(gettext("Не введена группа !"));
       break;
      }

     if(iceb_t_pvglkni(m,g) != 0)
      goto NAZ;


     deb=debt0.ravno_atof();
     kre=kred.ravno_atof();
     if(deb+kre == 0.)
      {
       iceb_t_soob(gettext("Не введён ни дебет ни кредит !"));
       break;
      }

     if( deb != 0. && kre != 0.)
      {
       iceb_t_soob(gettext("Введен и дебет и кредит !"));
       break;
      }
     if(nshk.ravno()[0] == '\0')
      {
       iceb_t_soob(gettext("Не введён кореспондирующий счёт !"));
       break;
      }
     
     if(prsh1(nshk.ravno(),&shetvh) != 0)
      break;

     if(shetvh.stat == 1)
      {
       sprintf(strsql,gettext("Счёт %s внебалансовый !"),nshk.ravno());
       iceb_t_soob(strsql);
       break;
      }

     rsdat(&d,&m,&g,datpr.ravno(),0);
     deb=debt0.ravno_atof();
     kre=kred.ravno_atof();
     deb=okrug(deb,0.01);
     kre=okrug(kre,0.01);
       

     sprintf(strsql,"select kodkon from Skontr where ns='%s'",nsh.ravno());
     SQLCURSOR cur;
     SQLCURSOR cur1;

     if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
       msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
     kolpr=kolstr1=0;
   
     while(cur.read_cursor(&row) != 0)
      {     
       strzag(LINES-1,0,kolstr,++kolstr1);       
       sprintf(strsql,"select grup from Kontragent where kodkon='%s'",
       row[0]);
       if(sql_readkey(&bd,strsql,&row1,&cur1) <= 0)
         continue;
       if(proverka(grupa.ravno(),row1[0],0,1) != 0)
         continue;
       kontr.new_plus(row[0]);

       kolpr++;         

       //sprintf(komen1,"%d %s",kolpr,komen); //Чтобы обеспечить возможность индивидуального удаления проводки
       komen.plus(" ");
       komen.plus(kolpr);
       time(&vrem);
       zapprov(0,g,m,d,nsh.ravno(),nshk.ravno(),kontr.ravno_filtr(),kontr.ravno_filtr(),"","","",deb,kre,komen.ravno_filtr(),2,0,vrem,0,0,0,0,kekv);
      }
     VVOD SOOB(1);
     sprintf(strsql,"%s %d",
     gettext("Количество выполненых проводок"),kolpr);
     SOOB.VVOD_SPISOK_add_MD(strsql);
     soobshw(&SOOB,stdscr,-1,-1,0,1);

     break;
    }

   if(K == FK3) /*Смотрим план счетов*/
    {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     class iceb_tu_str kod("");
     class iceb_tu_str naim("");
     if(dirshet(&kod,&naim,1) == 0)
      {
       nshk.new_plus(kod.ravno());
      }
     clear(); /*Очистить экран и закрасить фоновым цветом*/
     break;
    }

   if(K == FK4) /*Смотрим проводки*/
    {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     provtime();

     clear(); /*Очистить экран и закрасить фоновым цветом*/
     break;
    }

   if(K == FK5) /*Ввод блокировок*/
    {
     attroff(VV.VVOD_return_cs(iceb_CFS));
     iceb_tl_blok();
//     iceb_redfil("blok.alx",0);

     clear();
     break;
    }
   attroff(VV.VVOD_return_cs(iceb_CFS));
   attron(VV.VVOD_return_cs(iceb_CFM));
   attron(VV.VVOD_return_cs(iceb_CB));



   }
 }
}
