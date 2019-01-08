/* $Id: sumtab56.c,v 5.13 2013/05/17 14:56:12 sasa Exp $ */
/*22.06.2002	03.03.2000	Белых А.И.	sumtab56.c
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
void		sumtab56(int nomer,char imqkas[],int termlin,int nomernl)
{
#ifdef HAVE_TORGDEV
int		K,N;
int		i;
int		klishe,kopst,kolpok,sumnal,imq,sumotd,kolkop,kolkops;
#define		DLINA  25
char		klishe1[3][DLINA],rekl[2][DLINA],valuta[4];
char		strsql[512];

GDITE();

if((i=SG_read_check(cp[nomer],&klishe,&kopst,&kolpok,&sumnal,&imq,
&sumotd,&kolkop,&kolkops,klishe1[0],klishe1[1],klishe1[2],rekl[0],
rekl[1],valuta)) != 0)
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
VV.VVOD_SPISOK_add_data(30);
VV.VVOD_SPISOK_add_data(3);
VV.VVOD_SPISOK_add_data(30);
VV.VVOD_SPISOK_add_data(30);
VV.VVOD_SPISOK_add_data(30);
VV.VVOD_SPISOK_add_data(30);
VV.VVOD_SPISOK_add_data(3);
VV.VVOD_SPISOK_add_data(3);
VV.VVOD_SPISOK_add_data(DLINA);
VV.VVOD_SPISOK_add_data(DLINA);
VV.VVOD_SPISOK_add_data(DLINA);
VV.VVOD_SPISOK_add_data(DLINA);
VV.VVOD_SPISOK_add_data(sizeof(valuta));

if(klishe == 0)
 VV.data_plus(0,"Не печатьується");
if(klishe == 1)
 VV.data_plus(0,"Печатьується");


sprintf(VV.VVOD_SPISOK_return_data(1),"%d",kopst);


if(kolpok == 0)
 VV.data_plus(2,"Не печатьується");
if(kolpok == 1)
 VV.data_plus(2,"Печатьується");

if(sumnal == 0)
 VV.data_plus(3,"Не печатьується");
if(sumnal == 1)
 VV.data_plus(3,"Печатьується");


if(imq == 0)
 VV.data_plus(4,"Не печатьується");
if(imq == 1)
 VV.data_plus(4,"Печатьується");


if(sumotd == 0)
 VV.data_plus(5,"Відокремлюється");
if(sumotd == 1)
 VV.data_plus(5,"Не відокремлюється");

sprintf(VV.VVOD_SPISOK_return_data(6),"%d",kolkop);
sprintf(VV.VVOD_SPISOK_return_data(7),"%d",kolkops);
VV.data_plus(8,klishe1[0]);
VV.data_plus(9,klishe1[1]);
VV.data_plus(10,klishe1[2]);
VV.data_plus(11,rekl[0]);
VV.data_plus(12,rekl[1]);
VV.data_plus(13,valuta);

sprintf(strsql,"Каса:%s Термінальна лінія:%d Номер на лінії:%d",
imqkas,termlin,nomernl);
VV.VVOD_SPISOK_add_ZAG(strsql);

VV.VVOD_SPISOK_add_MD("Клише та рекламний текст...............");
VV.VVOD_SPISOK_add_MD("Кількість пустих стрічок в кінці чека..");
VV.VVOD_SPISOK_add_MD("Кількість покупок......................");
VV.VVOD_SPISOK_add_MD("Сумма налогу............................");
VV.VVOD_SPISOK_add_MD("Ім'я оператора.........................");
VV.VVOD_SPISOK_add_MD("Сумма пустими стрічками.................");
VV.VVOD_SPISOK_add_MD("Кількість копій чека...................");
VV.VVOD_SPISOK_add_MD("Кількість службових чеків..............");
VV.VVOD_SPISOK_add_MD("Кліше строка 1.........................");
VV.VVOD_SPISOK_add_MD("Кліше строка 2.........................");
VV.VVOD_SPISOK_add_MD("Кліше строка 3.........................");
VV.VVOD_SPISOK_add_MD("Рекламна строка 1......................");
VV.VVOD_SPISOK_add_MD("Рекламна строка 2......................");
VV.VVOD_SPISOK_add_MD("Валюта.................................");


naz:;
clear();


helstr(LINES-1,0,"F2/+",gettext("запись"),
"F10",gettext("выход"),NULL);


VV.vramka(-1,-1,0);

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


    if(N == 0) /*Клише*/
     {
      MENU.VVOD_delete();
      MENU.VVOD_SPISOK_add_MD("Не печатьується");
      MENU.VVOD_SPISOK_add_MD("Печатьується");

      helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
      "F10",gettext("выход"),NULL);
      i=0;
      while(menu3w(stdscr,&MENU,&i,-1,-1,0) != 0);
      if(i >= 0)
       {
        klishe=i;
        VV.data_plus(N,MENU.VVOD_SPISOK_return_MD(klishe));
       }
      goto naz;
      
     }

    if(N == 1) /*Количество пустых строчек в конце чека*/
     {

//      mygetstr(VV.VVOD_SPISOK_return_data(N),VV.VVOD_SPISOK_return_dls(N),stdscr);
      VV.vgetstr(N,stdscr);
      kopst=(int)ATOFM(VV.VVOD_SPISOK_return_data(N));
      
     }

    if(N == 2) /*Количество покупок*/
     {
      MENU.VVOD_delete();

      MENU.VVOD_SPISOK_add_MD("Не печатьується");
      MENU.VVOD_SPISOK_add_MD("Печатьується");

      helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
      "F10",gettext("выход"),NULL);

      if(i >= 0)
       {
        kolpok=i;
        VV.data_plus(N,MENU.VVOD_SPISOK_return_MD(kolpok));
       }
      goto naz;
      
     }

    if(N == 3) /*Сумма налога*/
     {
      MENU.VVOD_delete();

      MENU.VVOD_SPISOK_add_MD("Не печатьується");
      MENU.VVOD_SPISOK_add_MD("Печатьується");

      helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
      "F10",gettext("выход"),NULL);

      i=0;
      while(menu3w(stdscr,&MENU,&i,-1,-1,0) != 0);
      if(i >= 0)
       {
        sumnal=i;
        VV.data_plus(N,MENU.VVOD_SPISOK_return_MD(sumnal));
       }
      goto naz;
      
     }

    if(N == 4) /*Имя оператора*/
     {
      MENU.VVOD_delete();

      MENU.VVOD_SPISOK_add_MD("Не печатьується");
      MENU.VVOD_SPISOK_add_MD("Печатьується");

      helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
      "F10",gettext("выход"),NULL);

      if(i >= 0)
       {
        imq=i;
        VV.data_plus(N,MENU.VVOD_SPISOK_return_MD(imq));
       }
      goto naz;
      
     }

    if(N == 5) /*Сумма пустыми строчками*/
     {
      MENU.VVOD_delete();

      MENU.VVOD_SPISOK_add_MD("Відокремлюється");
      MENU.VVOD_SPISOK_add_MD("Невідокремлюється");


      helstr(LINES-1,0,"Enter",gettext("выбор нужного"),
      "F10",gettext("выход"),NULL);

      i=0;
      while(menu3w(stdscr,&MENU,&i,-1,-1,0) != 0);
      if(i >= 0)
       {
        sumotd=i;
        VV.data_plus(N,MENU.VVOD_SPISOK_return_MD(sumotd));
       }
      goto naz;
      
     }

    if(N == 6) /*Количество копий чека*/
     {

//      mygetstr(VV.VVOD_SPISOK_return_data(N),VV.VVOD_SPISOK_return_dls(N),stdscr);
      VV.vgetstr(N,stdscr);
      kolkop=(int)ATOFM(VV.VVOD_SPISOK_return_data(N));
      
     }

    if(N == 7) /*Количество служебных копий чека*/
     {

//      mygetstr(VV.VVOD_SPISOK_return_data(N),VV.VVOD_SPISOK_return_dls(N),stdscr);
      VV.vgetstr(N,stdscr);
      kolkops=(int)ATOFM(VV.VVOD_SPISOK_return_data(N));
      
     }

    if(N == 8) /*Клише 1*/
     {

//      mygetstr(VV.VVOD_SPISOK_return_data(N),VV.VVOD_SPISOK_return_dls(N),stdscr);
      VV.vgetstr(N,stdscr);
      strcpy(klishe1[0],VV.VVOD_SPISOK_return_data(N));
      
     }

    if(N == 9) /*Клише 2*/
     {

//      mygetstr(VV.VVOD_SPISOK_return_data(N),VV.VVOD_SPISOK_return_dls(N),stdscr);
      VV.vgetstr(N,stdscr);
      strcpy(klishe1[1],VV.VVOD_SPISOK_return_data(N));
      
     }

    if(N == 10) /*Клише 3*/
     {

//      mygetstr(VV.VVOD_SPISOK_return_data(N),VV.VVOD_SPISOK_return_dls(N),stdscr);
      VV.vgetstr(N,stdscr);
      strcpy(klishe1[2],VV.VVOD_SPISOK_return_data(N));
      
     }
    if(N == 11) /*Реклама 1*/
     {

//      mygetstr(VV.VVOD_SPISOK_return_data(N),VV.VVOD_SPISOK_return_dls(N),stdscr);
      VV.vgetstr(N,stdscr);
      strcpy(rekl[0],VV.VVOD_SPISOK_return_data(N));
      
     }
    if(N == 12) /*Реклама 2*/
     {

//      mygetstr(VV.VVOD_SPISOK_return_data(N),VV.VVOD_SPISOK_return_dls(N),stdscr);
      VV.vgetstr(N,stdscr);
      strcpy(rekl[1],VV.VVOD_SPISOK_return_data(N));
      
     }
    if(N == 13) /*Валюта*/
     {

      VV.vgetstr(N,stdscr);
//      mygetstr(VV.VVOD_SPISOK_return_data(N),VV.VVOD_SPISOK_return_dls(N),stdscr);
      strcpy(valuta,VV.VVOD_SPISOK_return_data(N));
      
     }
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
  attron(VV.VVOD_return_cs(iceb_CFS)); 
  VV.vprintw(N,stdscr);

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
      if((i=SG_prog_check(cp[nomer],klishe,kopst,kolpok,sumnal,imq,
      sumotd,kolpok,kolkops,klishe1[0],klishe1[1],klishe1[2],rekl[0],
      rekl[1],valuta)) != 0)
       {
        VVOD SOOB(1);
        SOOB.VVOD_SPISOK_add_MD("Помилка запису налаштовок !");
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
