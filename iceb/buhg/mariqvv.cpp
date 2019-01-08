/* $Id: mariqvv.c,v 5.16 2013/05/17 14:56:04 sasa Exp $ */
/*05.11.2007	23.03.2001	Белых А.И.	mariqvv.c
*/
#include        <time.h>
#ifdef HAVE_TORGDEV
#include        <libdev.h>
#endif
#include        "buhg.h"


void	mariqvv(KASSA kasr, 
int metka) //1 внесение в кассу клиентом 2- получение в кассе
{
char		strsql[512];
char		naim[512];
double		suma,cena,kolih=1;
int		vidnal=1;
int		N=0;
int		K=0;
int		vozvr;
int		kom;

VVOD VV(0);
VVOD MENU(3);

if(metka == 2)
  VV.VVOD_SPISOK_add_ZAG(gettext("Выдача из кассы клиенту"));
if(metka == 1)
  VV.VVOD_SPISOK_add_ZAG(gettext("Внесение в кассу клиентом"));

VV.VVOD_SPISOK_add_data(20);
VV.VVOD_SPISOK_add_data(gettext("С НДС"),20);

VV.VVOD_SPISOK_add_MD("Сумма...");
VV.VVOD_SPISOK_add_MD("Налог...");

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
    if(N == 1)
     {
      MENU.VVOD_delete();
      MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите признак"));
      MENU.VVOD_SPISOK_add_MD(gettext("С НДС"));
      MENU.VVOD_SPISOK_add_MD(gettext("Без НДС"));

      kom=0;
      while(menu3w(stdscr,&MENU,&kom,-1,-1,0) != 0);
      if(kom == -1)
        goto naz; 
      if(kom == 0)
        VV.data_plus(1,gettext("С НДС"));
      if(kom == 1)
        VV.data_plus(1,gettext("Без НДС"));

      vidnal=1;
      if(kom == 1)
       vidnal=2;  
      goto naz;       
     }

//    mygetstr(VV.VVOD_SPISOK_return_data(N),VV.VVOD_SPISOK_return_dls(N),stdscr);
    VV.vgetstr(N,stdscr);
    
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
    break;
   }
 }
attroff(VV.VVOD_return_cs(iceb_CFS));
attron(VV.VVOD_return_cs(iceb_CB));
refresh();
suma=ATOFM(VV.VVOD_SPISOK_return_data(0));
suma=okrug(suma,0.01);
cena=suma;
if(suma == 0.)
 {
  VVOD SOOB(1);
  sprintf(strsql,"%s = 0. !",gettext("Сумма"));
  SOOB.VVOD_SPISOK_add_MD(strsql);
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  goto naz;
 }

GDITE();

if(metka == 1)
 sprintf(naim,gettext("Внесение в кассу клиентом"));
if(metka == 2)
 sprintf(naim,gettext("Выдача из кассы клиенту"));

  sprintf(strsql,"FSTART: ADD_CHECK\n\
%s\n\
%.2f\n\
%.2f\n\
%.6g\n\
%d\n\
FEND:\n",
  naim,suma,cena,kolih,vidnal);  

   if(iceb_cmd(kasr, strsql) != 0)
    {
     sprintf(strsql,"FREE_CHECK:\n");
     iceb_cmd(kasr, strsql);
     return; 
    }  
/*
printw("\n%s\n",strsql);
OSTANOV();
*/

if(metka == 1)
  sprintf(strsql,"FSTART: MARIA_INDICATOR\n\
%s\n\
%s\n\
%d\n\
%s:\n\
FEND:\n",
kasr.prodavec, kasr.parol, 1, gettext("Внести в кассу"));  

if(metka == 2)
  sprintf(strsql,"FSTART: MARIA_INDICATOR\n\
%s\n\
%s\n\
%d\n\
%s:\n\
FEND:\n",
kasr.prodavec, kasr.parol, 1, gettext("Получить в кассе"));  
iceb_cmd(kasr, strsql);

sprintf(strsql,"FSTART: MARIA_INDICATOR\n\
%s\n\
%s\n\
%d\n\
%.2f\n\
FEND:\n",
kasr.prodavec, kasr.parol, 2, suma);  
iceb_cmd(kasr, strsql);

vozvr=0;
if(metka == 2)
 vozvr=1;

double isuma=suma;
double sum_nal=suma;

double sum_tara=0.;
double sum_pchek=0.;
double sum_cred=0.;
int skl=1;

sprintf(strsql,"FSTART: MARIA_PRCHECK\n\
%s\n\
%s\n\
%d\n\
%d\n\
%.2f\n\
%.2f\n\
%.2f\n\
%.2f\n\
%.2f\n\
FEND:\n",
kasr.prodavec, kasr.parol, skl, vozvr, isuma, sum_tara, sum_pchek, sum_cred, sum_nal);
iceb_cmd(kasr, strsql);
//OSTANOV();
return;
}
