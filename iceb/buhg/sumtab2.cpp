/* $Id: sumtab2.c,v 5.14 2013/05/17 14:56:12 sasa Exp $ */
/*21.06.2002	03.03.2000	Белых А.И.	sumtab2.c
Настройка таблицы 2 для кассоого аппарата SUMSUNG
*/
//#include	<libconfig.h>
#ifdef HAVE_TORGDEV
#include	<libdev.h>
#endif
#include	"buhg.h"

#ifdef HAVE_TORGDEV
extern cassa_pars *cp[MAX_CASS];
#endif

void		sumtab2(int nomer,char imqkas[],int termlin,int nomernl)
{
#ifdef HAVE_TORGDEV
int		KLST,dlr,MDLS;
int		K,N;
int		i;
int		tipkas,openbox,vvodsum,sdasha,indik,vihid,komun,speed;
char		strsql[512];

GDITE();
tipkas=openbox=vvodsum=sdasha=indik=vihid=komun=speed=0;

if((i=SG_read_srv(cp[nomer],&tipkas,&openbox,&vvodsum,&sdasha,&indik,&vihid,&komun,&speed)) != 0)
 {
  VVOD SOOB(1);
  SOOB.VVOD_SPISOK_add_MD("Не прочитани налаштовки касового апарата !");
  sprintf(strsql,"%s %d",CA_error,i);
  SOOB.VVOD_SPISOK_add_MD(strsql);
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  return;
 }

K=N=0;
VVOD MENU(3);
VVOD VV(0);

VV.VVOD_SPISOK_add_MD(30);
VV.VVOD_SPISOK_add_MD(30);
VV.VVOD_SPISOK_add_MD(30);
VV.VVOD_SPISOK_add_MD(30);
VV.VVOD_SPISOK_add_MD(30);
VV.VVOD_SPISOK_add_MD(30);
VV.VVOD_SPISOK_add_MD(30);
VV.VVOD_SPISOK_add_MD(30);

if(tipkas == 0)
 VV.data_plus(0,"На виході або один відділ");
if(tipkas == 1)
 VV.data_plus(0,"Декілька відділів");
if(tipkas == 2)
 VV.data_plus(0,"Ресторанна");

if(openbox == 0)
 VV.data_plus(1,"Коли загальна сумма 0");

if(openbox == 1)
 VV.data_plus(1,"При кожному зборі суми");

if(vvodsum == 0)
 VV.data_plus(2,"Необов'язково");
if(vvodsum == 1)
 VV.data_plus(2,"Обов'язково");

if(sdasha == 0)
 VV.data_plus(3,"Нараховується готівкою");
if(sdasha == 1)
 VV.data_plus(3,"Не нараховується");

if(indik == 0)
 VV.data_plus(4,"Завжди ввімкнений");
if(indik == 1)
 VV.data_plus(4,"Вимикається через 30сек.");

if(vihid == 0)
 VV.data_plus(5,"Заборонено");
if(vihid == 1)
 VV.data_plus(5,"Дозволено");

switch(komun)
 {
  case 0:
    VV.data_plus(6,"Автономний");
    break;  

  case 1:
    VV.data_plus(6,"IN-LINE");
    break;  

  case 2:
  case 3:
  case 4:
  case 7:
    sprintf(VV.VVOD_SPISOK_return_data(6),"ON-LINE %d",komun);
    break;  

  case 5:
  case 6:
    sprintf(VV.VVOD_SPISOK_return_data(6),"PRINTER %d",komun);
    break;  

  case 8:
    sprintf(VV.VVOD_SPISOK_return_data(6),"New PRINTER");
    break;  
  
 }

switch(speed)
 {
  case 1:
    VV.data_plus(7,"1200");
    break;  
  case 2:
    VV.data_plus(7,"2400");
    break;  
  case 3:
    VV.data_plus(7,"4800");
    break;  
  case 4:
    VV.data_plus(7,"9600");
    break;  
 }

sprintf(strsql,"Каса:%s Термінальна лінія:%d Номер на лінії:%d",
imqkas,termlin,nomernl);
VV.VVOD_SPISOK_add_ZAG(strsql);

VV.VVOD_SPISOK_add_MD("Тип каси....................................");
VV.VVOD_SPISOK_add_MD("Ящик відчиняється...........................");
VV.VVOD_SPISOK_add_MD("Введення суми перед натисканням кл. ОПЛАТА..");
VV.VVOD_SPISOK_add_MD("Здача.......................................");
VV.VVOD_SPISOK_add_MD("Індикатор...................................");
VV.VVOD_SPISOK_add_MD("Вихід зі стану регістації по кл. ВД.........");
VV.VVOD_SPISOK_add_MD("Тип комунікації.............................");
VV.VVOD_SPISOK_add_MD("Швидкість на линії, біт/сек.................");

naz:;
clear();


helstr(LINES-1,0,"F2/+",gettext("запись"),
"F10",gettext("выход"),NULL);

int PY=-1,PX=-1;

VV.vramka(&KLST,&dlr,&MDLS,&PY,&PX,0);

attron(VV.VVOD_return_cs(iceb_CFS)); 

move(PY+N+1,PX+1+dlr);
printw("%-*s",VV.VVOD_SPISOK_return_dls(N)-1,VV.VVOD_SPISOK_return_data(N));

for(;;)
 {
  refresh();
  K=getch();

  attroff(VV.VVOD_return_cs(iceb_CFS));
  attron(VV.VVOD_return_cs(iceb_CFM));

  move(PY+N+1,PX+1+dlr);
  printw("%-*s",VV.VVOD_SPISOK_return_dls(N)-1,VV.VVOD_SPISOK_return_data(N));
  if(K == KEY_MOUSE)
    K=mouseklic(&N,PY,PX,MDLS,KLST,stdscr);

  if(K == ENTER)
   {
    attroff(VV.VVOD_return_cs(iceb_CFM));
    attron(VV.VVOD_return_cs(iceb_CFV));

    move(PY+N+1,PX+1+dlr);
    printw("%*s",VV.VVOD_SPISOK_return_dls(N)-1," ");
    move(PY+N+1,PX+1+dlr);


    attroff(VV.VVOD_return_cs(iceb_CFV));
    attron(VV.VVOD_return_cs(iceb_CFM));

    if(N == 0) /*Тип кассы*/
     {
      MENU.VVOD_delete();
      MENU.VVOD_SPISOK_add_MD("На виході або один відділ");
      MENU.VVOD_SPISOK_add_MD("Декілька відділів");
      MENU.VVOD_SPISOK_add_MD("Ресторанна");

      helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
      "F10",gettext("выход"),NULL);
      i=0;
      while(menu3w(stdscr,&MENU,&i,-1,-1,0) != 0);
      if(i >= 0)
       {
        tipkas=i;
        VV.data_plus(N,MENU.VVOD_SPISOK_return_MD(tipkas));
       }
      goto naz;
      
     }

    if(N == 1) /*Ящик открывается*/
     {
      MENU.VVOD_delete();

      MENU.VVOD_SPISOK_add_MD("Коли загальна сумма 0");
      MENU.VVOD_SPISOK_add_MD("При кожному зборі суми");

      helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
      "F10",gettext("выход"),NULL);
      i=0;
      while(menu3w(stdscr,&MENU,&i,-1,-1,0) != 0);
      if(i >= 0)
       {
        openbox=i;
        VV.VVOD_SPISOK_zapis_data(N,"");
        VV.data_plus(N,MENU.VVOD_SPISOK_return_MD(openbox));
       }
      goto naz;
      
     }

    if(N == 2) /*Введение суммы перед нажатием*/
     {
      MENU.VVOD_delete();

      MENU.VVOD_SPISOK_add_MD("Необов'язково");
      MENU.VVOD_SPISOK_add_MD("Обов'язково");

      helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
      "F10",gettext("выход"),NULL);

      i=0;
      while(menu3w(stdscr,&MENU,&i,-1,-1,0) != 0);
      if(i >= 0)
       {
        vvodsum=i;
        VV.data_plus(N,MENU.VVOD_SPISOK_return_MD(vvodsum));
       }
      goto naz;
      
     }

    if(N == 3) /*Сдача*/
     {
      MENU.VVOD_delete();

      MENU.VVOD_SPISOK_add_MD("Нараховується готівкою");
      MENU.VVOD_SPISOK_add_MD("Не нараховується");

      helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
      "F10",gettext("выход"),NULL);

      i=0;
      while(menu3w(stdscr,&MENU,&i,-1,-1,0) != 0);
      if(i >= 0)
       {
        sdasha=i;
        VV.data_plus(N,MENU.VVOD_SPISOK_return_MD(sdasha));
       }
      goto naz;
      
     }
    if(N == 4) /*Индикатор*/
     {
      MENU.VVOD_delete();

      MENU.VVOD_SPISOK_add_MD("Завжди ввімкнений");
      MENU.VVOD_SPISOK_add_MD("Вимикається через 30сек.");

      helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
      "F10",gettext("выход"),NULL);

      i=0;
      while(menu3w(stdscr,&MENU,&i,-1,-1,0) != 0);
      if(i >= 0)
       {
        indik=i;
        VV.data_plus(N,MENU.VVOD_SPISOK_return_MD(indik));
       }
      goto naz;
      
     }

    if(N == 5) /*Выход с состояния регистрации*/
     {
      MENU.VVOD_delete();

      MENU.VVOD_SPISOK_add_MD("Заборонено");
      MENU.VVOD_SPISOK_add_MD("Дозволено");

      helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
      "F10",gettext("выход"),NULL);

      i=0;
      while(menu3w(stdscr,&MENU,&i,-1,-1,0) != 0);
      if(i >= 0)
       {
        vihid=i;
        VV.data_plus(N,MENU.VVOD_SPISOK_return_MD(vihid));
       }
      goto naz;
      
     }

    if(N == 6) /*Тип коммуникации*/
     {
      MENU.VVOD_delete();

//      sprintf(LL.MD[0],"Автономний");
      MENU.VVOD_SPISOK_add_MD("IN-LINE");
      MENU.VVOD_SPISOK_add_MD("IN-LINE 2");
      MENU.VVOD_SPISOK_add_MD("IN-LINE 3");
      MENU.VVOD_SPISOK_add_MD("IN-LINE 4");
      MENU.VVOD_SPISOK_add_MD("PRINTER 5");
      MENU.VVOD_SPISOK_add_MD("PRINTER 6");
      MENU.VVOD_SPISOK_add_MD("IN-LINE 7");
      MENU.VVOD_SPISOK_add_MD("New PRINTER");

      helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
      "F10",gettext("выход"),NULL);

      i=0;
      while(menu3w(stdscr,&MENU,&i,-1,-1,0) != 0);
      i++;
      if(i >= 0)
       {
        komun=i;
        VV.VVOD_SPISOK_zapis_data(N,"");
        switch(komun)
         {
          case 0:
            VV.data_plus(N,"Автономний");
            break;  

          case 1:
            VV.data_plus(N,"IN-LINE");
            break;  

          case 2:
          case 3:
          case 4:
          case 7:
            sprintf(VV.VVOD_SPISOK_return_data(N),"ON-LINE %d",komun);
            break;  

          case 5:
          case 6:
            sprintf(VV.VVOD_SPISOK_return_data(N),"PRINTER %d",komun);
            break;  

          case 8:
            sprintf(VV.VVOD_SPISOK_return_data(N),"New PRINTER");
            break;  
          
         }
       }
      goto naz;
      
     }
#ifdef KKKKK

    if(N == 7) /*Скорость на линии*/
     {
      MENU.VVOD_delete();

      MENU.VVOD_SPISOK_add_MD("1200");
      MENU.VVOD_SPISOK_add_MD("2400");
      MENU.VVOD_SPISOK_add_MD("4800");
      MENU.VVOD_SPISOK_add_MD("9600");

      helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
      "F10",gettext("выход"),NULL);

      i=0;
      while(menu3w(stdscr,&MENU,&i,-1,-1,0) != 0);

      if(i >= 0)
       {



        speed=i+1;
        VV.VVOD_SPISOK_zapis_data(N,"");
        switch(speed)
         {
          case 1:
            VV.data_plus(N,"1200");
            break;  
          case 2:
            VV.data_plus(N,"2400");
            break;  
          case 3:
            VV.data_plus(N,"4800");
            break;  
          case 4:
            VV.data_plus(N,"9600");
            break;  
         }
       }
      goto naz;
      
     }
#endif

    move(PY+N+1,PX+1+dlr);
    printw("%*s",VV.VVOD_SPISOK_return_dls(N)-1," ");
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
  if(N > KLST-1)
   {
    N=0;
   }
  if(N < 0)
   {
    N=KLST-1;
   }
  attron(VV.VVOD_return_cs(iceb_CFS)); 
  move(PY+N+1,PX+1+dlr);
  printw("%*s",VV.VVOD_SPISOK_return_dls(N)-1," ");

   if(K == ESC || K == FK10)
    {
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
     N=-1;
     return;
    }
   if(K == FK2 || K == PLU) /*Записываем*/
    {
      attroff(VV.VVOD_return_cs(iceb_CFS));
      attron(VV.VVOD_return_cs(iceb_CFM));
 //     printw("\nspeed=%d\n",speed);
//      OSTANOV();
      if((i=SG_prog_srv(cp[nomer],cp[nomer]->num,tipkas,openbox,vvodsum,
      sdasha,indik,vihid,komun,speed)) != 0)
       {
        VVOD SOOB(1);
        sprintf(strsql,"Помилка запису налаштовок !");
        SOOB.VVOD_SPISOK_add_MD(strsql);
        sprintf(strsql,"%s %d",CA_error,i);
        SOOB.VVOD_SPISOK_add_MD(strsql);
        soobshw(&SOOB,stdscr,-1,-1,0,1);
        break;

       }
 
     return;
    }
 }
#endif
}
