/* $Id: vprovnbs.c,v 5.27 2013/09/26 09:43:44 sasa Exp $ */
/*16.04.2017	02.06.2001	Белых А.И.	vprovnbs.c
Ввод проводок для не балансовых счетов
*/
#include        "buhg.h"

extern struct  passwd  ktor;

void vprovnbs(const char *shet,OPSHET *sheto)
{
static class iceb_tu_str dat1("");
class iceb_tu_str kodo("");
class iceb_tu_str kred(""),kred1("");
class iceb_tu_str debt0(""),debt1("");
class iceb_tu_str datpr("");
class iceb_tu_str komen("");
class iceb_tu_str nai("");
int             N=0,K=0;
short		d,m,g;
char		strsql[1024];
double		deb=0.,kre=0.;
class iceb_tu_str dkk("");
int		i;
SQL_str		row;
SQLCURSOR       cur;
time_t		vrem=0;
int kekv=0;
clear();
VVOD VVOD1(2);
VVOD VV(0);
VV.VVOD_SPISOK_add_MD(gettext("Кредит......"));
VV.VVOD_SPISOK_add_MD(gettext("Дебет......."));
VV.VVOD_SPISOK_add_MD(gettext("Дата........"));
VV.VVOD_SPISOK_add_MD(gettext("Коментарий.."));
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(11);
VV.VVOD_SPISOK_add_data(60);



VV.data_plus(2,dat1.ravno());
datpr.new_plus(dat1.ravno());

NAZ:;

for(;;)
 {
  VV.data_plus(2,dat1.ravno());
  move(LINES-1,0);


  move(0,0);
  printw(gettext("Выполнение проводок для счёта %s"),shet);
  move(1,0);
  if(sheto->tips == 0)
    printw(gettext("Счёт активный"));
  if(sheto->tips == 1)
    printw(gettext("Счёт пассивный"));
  if(sheto->tips == 2)
    printw(gettext("Счёт активнo-пассивный"));
  if(sheto->saldo == 3)
    printw("  %s",gettext("Р/с"));
  attron(A_BLINK);
  printw(" %s",gettext("Внебалансовий"));
  attroff(A_BLINK);

  printw("\n%s\n",sheto->naim);
  
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

  move(LINES-5,0);
  printw("%s %s %s\n%s %s",
  debt1.ravno(),kred1.ravno(),datpr.ravno(),kodo.ravno(),nai.ravno());


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
	datpr.new_plus(VV.VVOD_SPISOK_return_data(N));
	dat1.new_plus(VV.VVOD_SPISOK_return_data(N));
        if(rsdat(&d,&m,&g,datpr.ravno(),0) != 0)
         {
          attroff(VV.VVOD_return_cs(iceb_CFV));
          attron(VV.VVOD_return_cs(iceb_CB));
          VVOD SOOB(1);
          SOOB.VVOD_SPISOK_add_MD(gettext("Не верно введена дата проводки !"));
          soobshw(&SOOB,stdscr,-1,-1,0,1);
          break;
         }
       }
      if(N == 3)
       {
        VV.vgetstr(N,stdscr);
	komen.new_plus(VV.VVOD_SPISOK_return_data(N));
       }

        attroff(VV.VVOD_return_cs(iceb_CFV));
        attron(VV.VVOD_return_cs(iceb_CFM));

        VV.vprintw(N,stdscr);
      if(N == 0)
       N++;

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
    attron(VV.VVOD_return_cs(iceb_CFS)); 
    VV.vprintw(N,stdscr);

   if(K == ESC || K == FK10)
    {
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CB));
     return;
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

    if(VV.VVOD_SPISOK_return_data(0)[0] == '\0' && VV.VVOD_SPISOK_return_data(1)[0] == '\0')
     {
      iceb_t_soob(gettext("Не введена сумма !"));
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


    /*Код организации для главного счета*/
    kodo.new_plus("");
    if(sheto->saldo == 3)
     {
iii:;
      helstr(LINES-1,0,"Enter",gettext("контрагенты"),
      "F10",gettext("выход"),NULL);

      dkk.new_plus("");
      VVOD1.VVOD_delete();
      sprintf(strsql,gettext("Введите код контрагента для счёта %s"),shet);
      VVOD1.VVOD_SPISOK_add_MD(strsql);

      if((i=vvod1(&dkk,16,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
       {
	clear();
	break;
       }
      if(i == ESC)
       {
	clear();
	break;
       }

      GDITE();

      if(dkk.ravno()[0] == '\0' || dkk.ravno()[0] == '+')
       {
        if(dkk.ravno()[0] == '+')
         nai.new_plus(iceb_tu_adrsimv(1,dkk.ravno()));
        else 
         nai.new_plus("");
	if(dirskont(shet,&kodo,&nai,1) != 0)
	 {
	  clear(); /*Очистить экран и закрасить фоновым цветом*/
	  break;
	 }
	clear();
       }
      else
       kodo.new_plus(dkk.ravno());


      sprintf(strsql,"select kodkon from Skontr where ns='%s' and \
kodkon='%s'",shet,kodo.ravno());
      if(sql_readkey(&bd,strsql,&row,&cur) != 1)
       {
        sprintf(strsql,gettext("Не нашли код контрагента %s в списке счета %s !"),kodo.ravno(),shet);
        iceb_t_soob(strsql);
        goto iii;
       }

      sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kodo.ravno());
      if(sql_readkey(&bd,strsql,&row,&cur) != 1)
       {
        sprintf(strsql,gettext("Не нашли код контрагента %s в общем списке !"),kodo.ravno());
        iceb_t_soob(strsql);

        goto iii;
       }
      nai.new_plus(row[0]);
     }


   GDITE();

    rsdat(&d,&m,&g,datpr.ravno(),0);
    deb=debt0.ravno_atof();
    kre=kred.ravno_atof();
    deb=okrug(deb,0.01);
    kre=okrug(kre,0.01);
      

    time(&vrem);

    zapprov(-1,g,m,d,shet,"\0",kodo.ravno(),"\0","","","",deb,kre,komen.ravno_filtr(),1,0,vrem,0,0,0,0,kekv);
    debt1.new_plus(deb,2);
    kred1.new_plus(kre,2);

    attroff(A_REVERSE);  /*Выключить инверсию*/
    clear(); /*Очистить экран и закрасить фоновым цветом*/
    N=0;
    break;
   }
  if(K == FK1) /*Помощ*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));
    GDITE();

    iceb_t_pdoc("buhg3_1_1.txt");

    clear();
    break;
   }
  if(K == FK3) /*Смотрим план счетов*/
   {
    attroff(VV.VVOD_return_cs(iceb_CFS));

    class iceb_tu_str kod("");
    class iceb_tu_str naimkod("");
    dirshet(&kod,&naimkod,0);

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
//    iceb_redfil("blok.alx",0);
    clear();
    break;
   }
  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));
  attron(VV.VVOD_return_cs(iceb_CB));
  }
 }
}
